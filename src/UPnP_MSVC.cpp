// Copyright (c) 2005 - 2019 Settlers Freaks (sf-team at siedler25.org)
//
// This file is part of Return To The Roots.
//
// Return To The Roots is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Return To The Roots is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Return To The Roots. If not, see <http://www.gnu.org/licenses/>.

#include "IP.hpp"
#include "UPnP.h"
#include "UniqueHandle.h"
#include <windows.h>
#include <algorithm>
#include <array>
#include <iphlpapi.h>
#include <natupnp.h>
#include <ole2.h>
#include <vector>
#include <winsock2.h>

#pragma comment(lib, "iphlpapi.lib")

namespace s25util { namespace traits {
    void freeHandle(BSTR p) { SysFreeString(p); }
}} // namespace s25util::traits

namespace {
using ByteString = s25util::UniqueHandle<BSTR>;

ByteString A2WBSTR(LPCSTR lp, int nLen = -1)
{
    if(lp == nullptr || nLen == 0)
        return ByteString();

    int nConvertedLen = MultiByteToWideChar(CP_THREAD_ACP, 0, lp, nLen, nullptr, 0);

    int nAllocLen = nConvertedLen;
    if(nLen == -1)
        nAllocLen -= 1; // Don't allocate terminating '\0'
    auto str = ByteString(::SysAllocStringLen(nullptr, nAllocLen));

    if(str)
    {
        const int nResult = MultiByteToWideChar(CP_THREAD_ACP, 0, lp, nLen, str, nConvertedLen);
        if(nResult != nConvertedLen)
            return ByteString();
    }
    return str;
}

ByteString A2BSTR(LPCSTR lp)
{
    return A2WBSTR(lp);
}

std::vector<std::string> GetAllv4Addresses()
{
    std::vector<std::string> addresses;

    // read addresses from adapters
    ULONG OutBufLen = 0;
    GetAdaptersInfo(nullptr, &OutBufLen);

    PIP_ADAPTER_INFO pAdapterInfo = (PIP_ADAPTER_INFO)HeapAlloc(GetProcessHeap(), 0, OutBufLen);

    if(GetAdaptersInfo(pAdapterInfo, &OutBufLen) == NO_ERROR)
    {
        PIP_ADAPTER_INFO pAdapter = pAdapterInfo;
        while(pAdapter)
        {
            std::string address = pAdapter->IpAddressList.IpAddress.String;
            if(address != "0.0.0.0")
                addresses.push_back(address);

            pAdapter = pAdapter->Next;
        }
    }

    HeapFree(GetProcessHeap(), 0, pAdapterInfo);

    // and failback solution: read address from hostname
    std::array<char, 512> host;
    gethostname(host.data(), sizeof(host));

    struct hostent* hosts = gethostbyname(host.data());
    if(hosts)
    {
        for(int i = 0; hosts->h_addr_list[i] != nullptr; ++i)
        {
            struct in_addr addr;
            memcpy(&addr, hosts->h_addr_list[i], sizeof(struct in_addr));
            std::string address = inet_ntoa(addr);
            if(address != "0.0.0.0")
                addresses.push_back(address);
        }
    }

    // remove duplicates
    std::sort(addresses.begin(), addresses.end());
    auto begin = std::unique(addresses.begin(), addresses.end());
    addresses.erase(begin, addresses.end());

    return addresses;
}

[[noreturn]] void throwSystemError(HRESULT ec)
{
    throw std::system_error(std::error_code(ec, std::system_category()));
}
} // namespace

void UPnP::OpenPort(uint16_t port)
{
    HRESULT hr;

    CoInitialize(nullptr);

    IUPnPNAT* upnpnat;
    hr = CoCreateInstance(CLSID_UPnPNAT, nullptr, CLSCTX_INPROC_SERVER, IID_IUPnPNAT, (void**)&upnpnat);
    if(FAILED(hr) || !upnpnat)
    {
        if(!upnpnat)
            hr = E_NOINTERFACE;
        throwSystemError(hr);
    }

    IStaticPortMappingCollection* upnpspmc = nullptr;
    hr = upnpnat->get_StaticPortMappingCollection(&upnpspmc);
    if(FAILED(hr) || !upnpspmc)
    {
        if(!upnpspmc)
            hr = E_NOINTERFACE;
        throwSystemError(hr);
    }

    const std::vector<std::string> addresses = GetAllv4Addresses();

    std::string local_address;
    // if we have multiple addresses, search the private one
    if(addresses.size() > 1)
    {
        for(const std::string& addr : addresses)
        {
            auto const ip = s25util::stringToIP(addr);

            if((ip & 0xff000000) == s25util::makeIP(10, 0, 0, 0) ||   // 10.0.0.0/8
               (ip & 0xff000000) == s25util::makeIP(127, 0, 0, 0) ||  // 127.0.0.0/8
               (ip & 0xfff00000) == s25util::makeIP(172, 16, 0, 0) || // 172.16.0.0/12
               (ip & 0xffff0000) == s25util::makeIP(192, 168, 0, 0))  // 192.168.0.0/16
                local_address = addr;
        }
    }

    // if none set use the first one
    if(local_address.empty() && !addresses.empty())
        local_address = addresses.front();

    if(local_address.empty())
        throw std::runtime_error("Local IP not found");

    const auto bstrProtocol = A2BSTR("TCP");
    const auto bstrLocalAddress = A2BSTR(local_address.c_str());
    const auto bstrDescription = A2BSTR("Return To The Roots");

    IStaticPortMapping* upnpspm = nullptr;
    hr = upnpspmc->Add(port, bstrProtocol, port, bstrLocalAddress, VARIANT_TRUE, bstrDescription, &upnpspm);

    if(SUCCEEDED(hr))
    {
        if(upnpspm)
            return;
        hr = E_NOINTERFACE;
    }
    throwSystemError(hr);
}

void UPnP::ClosePort(uint16_t port)
{
    HRESULT hr;

    IUPnPNAT* upnpnat;
    hr = CoCreateInstance(CLSID_UPnPNAT, nullptr, CLSCTX_INPROC_SERVER, IID_IUPnPNAT, (void**)&upnpnat);
    if(FAILED(hr) || !upnpnat)
        throwSystemError(hr);

    IStaticPortMappingCollection* upnpspmc = nullptr;
    hr = upnpnat->get_StaticPortMappingCollection(&upnpspmc);
    if(FAILED(hr) || !upnpspmc)
        throwSystemError(hr);

    const auto bstrProtocol = A2BSTR("TCP");

    hr = upnpspmc->Remove(port, bstrProtocol);

    if(FAILED(hr))
        throwSystemError(hr);
}
