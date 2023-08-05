// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "UPnP.h"
#include "s25util/StringConversion.h"
#include "s25util/UniqueHandle.h"
#include <cstring>
#include <miniupnpc/miniupnpc.h>
#include <miniupnpc/upnpcommands.h>
#include <stdexcept>
#include <string>

static bool operator!=(const UPNPUrls& lhs, const UPNPUrls& rhs)
{
    return std::memcmp(&lhs, &rhs, sizeof(lhs)) != 0;
}
inline void freeHandle(UPNPDev* p)
{
    freeUPNPDevlist(p);
}
inline void freeHandle(UPNPUrls& p)
{
    FreeUPNPUrls(&p);
}

namespace s25util::traits {
    template<>
    struct InvalidHandleValue<UPNPUrls>
    {
        static constexpr UPNPUrls value{};
        constexpr operator UPNPUrls() const { return UPNPUrls{}; }
    };
} // namespace s25util::traits

namespace miniupnp {
using DeviceList = s25util::UniqueHandle<UPNPDev*>;
using Urls = s25util::UniqueHandle<UPNPUrls>;

enum class Protocol
{
    TCP,
    UDP
};
inline const char* toString(Protocol proto)
{
    return (proto == Protocol::TCP) ? "TCP" : "UDP";
}

inline std::string errorMessage(int hr)
{
    switch(hr)
    {
        case 402: return "Invalid Args";
        case 501: return "Action Failed";
        case 606: return "Action not authorized";
        case 714: return "NoSuchEntryInArray";
        case 715: return "WildCardNotPermittedInSrcIP";
        case 716: return "WildCardNotPermittedInExtPort";
        case 718: return "ConflictInMappingEntry";
        case 724: return "SamePortValuesRequired";
        case 725: return "OnlyPermanentLeasesSupported";
        case 726: return "RemoteHostOnlySupportsWildcard";
        case 727: return "ExternalPortOnlySupportsWildcard";
        case 728: return "NoPortMapsAvailable";
        case 729: return "ConflictWithOtherMechanisms";
        case 732: return "WildCardNotPermittedInIntPort";
    }
    return "Unknown error";
}

inline DeviceList discover(int delay, const char* multicastIf = nullptr, const char* miniSSDPDSock = nullptr,
                           bool samePort = false)
{
    UPNPDev* devicelist = nullptr;
#ifdef UPNPDISCOVER_SUCCESS
    int error = 0;
    const bool ipv6 = false;
#    if defined(MINIUPNPC_API_VERSION) && (MINIUPNPC_API_VERSION >= 14)
    const int ttl = 2;
    devicelist = upnpDiscover(delay, multicastIf, miniSSDPDSock, samePort, ipv6, ttl, &error);
#    else
    devicelist = upnpDiscover(delay, multicastIf, miniSSDPDSock, samePort, ipv6, &error);
#    endif
#else
    devicelist = upnpDiscover(delay, multicastIf, miniSSDPDSock, samePort);
#endif
    return DeviceList(devicelist);
}

inline bool getValidIGD(const DeviceList& deviceList, Urls& urls, IGDdatas& data, std::string& lanAddr)
{
    lanAddr.resize(15); // Format: aaa.bbb.ccc.ddd
    return UPNP_GetValidIGD(deviceList, &urls, &data, &lanAddr[0], lanAddr.size()) != 0;
}

inline void addPortMapping(const char* controlURL, const char* servicetype, const std::string& extPort,
                           const std::string& inPort, const std::string& inClient, const std::string& desc,
                           Protocol proto, const char* remoteHost = nullptr)
{
#ifdef UPNPDISCOVER_SUCCESS
#    define LEASE_DURATION_ARG , nullptr
#else
#    define LEASE_DURATION_ARG
#endif
    const char* sProtocol = toString(proto);
    const auto hr = UPNP_AddPortMapping(controlURL, servicetype, extPort.c_str(), inPort.c_str(), inClient.c_str(),
                                        desc.c_str(), sProtocol, remoteHost LEASE_DURATION_ARG);
    if(hr == 0)
        return;
    throw std::runtime_error("AddPortMapping failed with: " + errorMessage(hr));
#undef LEASE_DURATION_ARG
}

inline void deletePortMapping(const char* controlURL, const char* servicetype, const std::string& extPort,
                              Protocol proto, const char* remoteHost = nullptr)
{
    const char* sProtocol = toString(proto);
    const auto hr = UPNP_DeletePortMapping(controlURL, servicetype, extPort.c_str(), sProtocol, remoteHost);
    if(hr == 0)
        return;
    throw std::runtime_error("DeletePortMapping failed with: " + errorMessage(hr));
}
} // namespace miniupnp

/**
 *  Erstellt per UPnP ein Portforwarding.
 */
void UPnP::OpenPort(uint16_t port)
{
    const auto deviceList = miniupnp::discover(2000);
    if(!deviceList)
        throw std::runtime_error("Could not get devices");

    miniupnp::Urls urls;
    IGDdatas data;
    std::string landAddr;
    if(!miniupnp::getValidIGD(deviceList, urls, data, landAddr))
        throw std::runtime_error("No gateway found");

    const auto sPort = s25util::toStringClassic(port);
    miniupnp::addPortMapping(urls->controlURL, data.first.servicetype, sPort, sPort, landAddr, "Return To The Roots",
                             miniupnp::Protocol::TCP);
}

void UPnP::ClosePort(uint16_t port)
{
    const auto deviceList = miniupnp::discover(2000);
    if(!deviceList)
        throw std::runtime_error("Could not get devices");

    miniupnp::Urls urls;
    IGDdatas data;
    std::string landAddr;
    if(!miniupnp::getValidIGD(deviceList, urls, data, landAddr))
        throw std::runtime_error("No gateway found");

    const auto sPort = s25util::toStringClassic(port);
    miniupnp::deletePortMapping(urls->controlURL, data.first.servicetype, sPort, miniupnp::Protocol::TCP);
}
