// Copyright (c) 2005 - 2015 Settlers Freaks (sf-team at siedler25.org)
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

#include "libUtilDefines.h" // IWYU pragma: keep
#include "UPnP.h"

#ifdef _WIN32
#include "Log.h"
#include <windows.h>
#include <iphlpapi.h>
#include <winsock2.h>

#ifdef _MSC_VER
#pragma comment(lib, "iphlpapi.lib")

#include <natupnp.h>
#include <ole2.h>
//#include <atlconv.h>

inline BSTR A2WBSTR(LPCSTR lp, int nLen = -1)
{
    if(lp == NULL || nLen == 0)
        return NULL;
    BSTR str = NULL;

    int nConvertedLen = MultiByteToWideChar(CP_THREAD_ACP, 0, lp, nLen, NULL, 0);

    int nAllocLen = nConvertedLen;
    if(nLen == -1)
        nAllocLen -= 1; // Don't allocate terminating '\0'
    str = ::SysAllocStringLen(NULL, nAllocLen);

    if(str != NULL)
    {
        int nResult;
        nResult = MultiByteToWideChar(CP_THREAD_ACP, 0, lp, nLen, str, nConvertedLen);
        if(nResult != nConvertedLen)
        {
            SysFreeString(str);
            return NULL;
        }
    }
    return str;
}

inline BSTR A2BSTR(LPCSTR lp)
{
    return A2WBSTR(lp);
}
#endif // _MSC_VER

#else // _WIN32
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#endif // _WIN32

#ifndef _MSC_VER
#include <miniupnpc/miniupnpc.h>
#include <miniupnpc/upnpcommands.h>
#endif // _WIN32

#include <algorithm>
#include <cstring>
#include <sstream>

UPnP::UPnP() : remote_port_(0) {}

UPnP::~UPnP()
{
    ClosePort();
}

/**
 *  Erstellt per UPnP ein Portforwarding.
 */
bool UPnP::OpenPort(const unsigned short& port)
{
    if(remote_port_ != 0)
        ClosePort();

    remote_port_ = port;

#ifdef _MSC_VER
    HRESULT hr;

    CoInitialize(NULL);

    IUPnPNAT* upnpnat;
    hr = CoCreateInstance(CLSID_UPnPNAT, NULL, CLSCTX_INPROC_SERVER, IID_IUPnPNAT, (void**)&upnpnat);
    if(FAILED(hr) || !upnpnat)
    {
        if(!upnpnat)
            hr = E_NOINTERFACE;
        SetLastError(hr);
        return false;
    }

    IStaticPortMappingCollection* upnpspmc = NULL;
    hr = upnpnat->get_StaticPortMappingCollection(&upnpspmc);
    if(FAILED(hr) || !upnpspmc)
    {
        if(!upnpspmc)
            hr = E_NOINTERFACE;
        SetLastError(hr);
        return false;
    }

    std::string local_address;
    std::vector<std::string> addresses = GetAllv4Addresses();

    // if we have multiple addresses, search the private one
    if(addresses.size() > 1)
    {
        for(std::vector<std::string>::iterator addr = addresses.begin(); addr != addresses.end(); ++addr)
        {
            std::string ss = *addr;
            std::stringstream s, sc;
            s << ss;
            std::getline(s, ss, '.'); //-V760
            sc << ss << " ";
            std::getline(s, ss, '.');
            sc << ss << " ";

            int a, b;
            sc >> a;
            sc >> b;

            int ab = (a << 24) | (b << 16);

            if((ab & 0xff000000) == 0x0a000000 || // 10.0.0.0/8
               (ab & 0xff000000) == 0x7f000000 || // 127.0.0.0/8
               (ab & 0xfff00000) == 0xac100000 || // 172.16.0.0/12
               (ab & 0xffff0000) == 0xc0a80000)   // 192.168.0.0/16
                local_address = *addr;
        }
    }

    // otherwise use the first one
    if(local_address.empty() && !addresses.empty())
        local_address = addresses.front();

    // I hope we found one ...
    if(local_address.empty())
    {
        SetLastError(E_FAIL);
        return false;
    }

    BSTR bstrProtocol = A2BSTR("TCP");
    BSTR bstrLocalAddress = A2BSTR(local_address.c_str());
    BSTR bstrDescription = A2BSTR("Return To The Roots");

    IStaticPortMapping* upnpspm = NULL;
    hr = upnpspmc->Add(port, bstrProtocol, port, bstrLocalAddress, VARIANT_TRUE, bstrDescription, &upnpspm);

    SysFreeString(bstrProtocol);
    SysFreeString(bstrLocalAddress);
    SysFreeString(bstrDescription);

    if(SUCCEEDED(hr) && !upnpspm)
        hr = E_NOINTERFACE;

    SetLastError(hr);

    if(SUCCEEDED(hr) && upnpspm)
        return true;
#else
    int hr;
    UPNPDev* devicelist = NULL;
#ifdef UPNPDISCOVER_SUCCESS
    int upnperror = 0;
#if(MINIUPNPC_API_VERSION >= 14) /* miniUPnPc API version 14 adds TTL parameter */
    devicelist = upnpDiscover(2000, NULL, NULL, 0, 0 /* ipv6 */, 2, &upnperror);
#else
    devicelist = upnpDiscover(2000, NULL, NULL, 0, 0 /* ipv6 */, &upnperror);
#endif
#else
    devicelist = upnpDiscover(2000, NULL, NULL, 0);
#endif
    if(!devicelist)
        return false;

    UPNPUrls urls;
    IGDdatas data;
    char lanAddr[64];
    hr = UPNP_GetValidIGD(devicelist, &urls, &data, lanAddr, sizeof(lanAddr));

    if(hr == 1 || hr == 2)
    {
        std::stringstream p;
        p << port;

#ifdef UPNPDISCOVER_SUCCESS
        hr = UPNP_AddPortMapping(urls.controlURL, data.first.servicetype, p.str().c_str(), p.str().c_str(), lanAddr, "Return To The Roots",
                                 "TCP", NULL, NULL);
#else
        hr = UPNP_AddPortMapping(urls.controlURL, data.first.servicetype, p.str().c_str(), p.str().c_str(), lanAddr, "Return To The Roots",
                                 "TCP", NULL);
#endif
    }

    freeUPNPDevlist(devicelist);

    if(hr == 0)
        return true;
#endif
    return false;
}

void UPnP::ClosePort()
{
    if(remote_port_ == 0)
        return;

#ifdef _MSC_VER
    HRESULT hr;

    IUPnPNAT* upnpnat;
    hr = CoCreateInstance(CLSID_UPnPNAT, NULL, CLSCTX_INPROC_SERVER, IID_IUPnPNAT, (void**)&upnpnat);
    if(FAILED(hr) || !upnpnat)
        return;

    IStaticPortMappingCollection* upnpspmc = NULL;
    hr = upnpnat->get_StaticPortMappingCollection(&upnpspmc);
    if(FAILED(hr) || !upnpspmc)
        return;

    BSTR bstrProtocol = A2BSTR("TCP");

    hr = upnpspmc->Remove(remote_port_, bstrProtocol);

    if(FAILED(hr))
        LOG.writeLastError("Automatisches Entfernen des Portforwardings mit UPnP fehlgeschlagen\nFehler");

    SysFreeString(bstrProtocol);

    if(FAILED(hr))
        return;
#else
    int hr;
    UPNPDev* devicelist = NULL;
#ifdef UPNPDISCOVER_SUCCESS
    int upnperror = 0;
#if(MINIUPNPC_API_VERSION >= 14) /* miniUPnPc API version 14 adds TTL parameter */
    devicelist = upnpDiscover(2000, NULL, NULL, 0, 0 /* ipv6 */, 2, &upnperror);
#else
    devicelist = upnpDiscover(2000, NULL, NULL, 0, 0 /* ipv6 */, &upnperror);
#endif
#else
    devicelist = upnpDiscover(2000, NULL, NULL, 0);
#endif
    if(!devicelist)
        return;

    UPNPUrls urls;
    IGDdatas data;
    hr = UPNP_GetValidIGD(devicelist, &urls, &data, NULL, 0);
    if(hr == 1 || hr == 2)
    {
        std::stringstream p;
        p << remote_port_;
        hr = UPNP_DeletePortMapping(urls.controlURL, data.first.servicetype, p.str().c_str(), "TCP", NULL);
    }

    freeUPNPDevlist(devicelist);

    if(hr != 0)
        return;
#endif

    remote_port_ = 0;
}

/**
 *  Liest alle Adapteradressen aus.
 */
std::vector<std::string> UPnP::GetAllv4Addresses()
{
    std::vector<std::string> addresses;

#ifdef _WIN32
    // read addresses from adapters
    ULONG OutBufLen = 0;
    GetAdaptersInfo(NULL, &OutBufLen);

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
#else
    struct ifaddrs *ifaddr, *ifa;
    int family;
    char address[NI_MAXHOST];

    if(getifaddrs(&ifaddr) > 0)
    {
        for(ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next)
        {
            family = ifa->ifa_addr->sa_family;

            if(family == AF_INET || family == AF_INET6)
            {
                if(getnameinfo(ifa->ifa_addr, (family == AF_INET) ? sizeof(struct sockaddr_in) : sizeof(struct sockaddr_in6), address,
                               NI_MAXHOST, NULL, 0, NI_NUMERICHOST)
                   == 0)
                    addresses.push_back(address);
            }
        }
        freeifaddrs(ifaddr);
    }
#endif

    // and failback solution: read address from hostname
    char host[512];
    gethostname(host, 512);

    struct hostent* hosts = gethostbyname(host);
    if(hosts)
    {
        for(int i = 0; hosts->h_addr_list[i] != 0; ++i)
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
    std::vector<std::string>::iterator begin = std::unique(addresses.begin(), addresses.end());
    addresses.erase(begin, addresses.end());

    return addresses;
}
