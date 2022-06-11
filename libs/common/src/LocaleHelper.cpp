// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "LocaleHelper.h"
#include "System.h"
#include <boost/filesystem/path.hpp>
#include <boost/optional/optional.hpp>
#include <boost/predef/os.h>
#include <iostream>
#if BOOST_OS_WINDOWS
#    include <boost/locale.hpp>
#elif !BOOST_OS_MACOS
#    include "utf8.h"
#    include <iomanip>
#    include <sstream>
#endif

namespace bfs = boost::filesystem;

namespace {
class LocaleResetter
{
    boost::optional<std::locale> originalLocale;

public:
    ~LocaleResetter()
    {
        if(originalLocale)
        {
            try
            {
                // Reset locale back to default to avoid memory leaks
                std::locale::global(*originalLocale);
            } catch(...)
            {} // Ignore any error
        }
    }

    std::locale changeGlobalLocale(const std::locale& locale)
    {
        std::locale oldLocale = std::locale::global(locale);
        if(!originalLocale)
            originalLocale = oldLocale;
        return locale;
    }
};
std::locale getBfsLocale()
{
    std::locale result;
    try
    {
        result = bfs::path::imbue(std::locale());
        // Reset
        bfs::path::imbue(result);
        return result;
    } catch(std::exception& e)
    {
        if(!BOOST_OS_WINDOWS && System::getEnvVar("LC_CALL") != "C")
        {
            std::cerr << "Caught an exception while setting locale: " << e.what() << std::endl
                      << "Setting LC_ALL=\"C\" and trying again..." << std::endl;
            System::setEnvVar("LC_ALL", "C");
            return getBfsLocale();
        } else
            throw e;
    }
}
#if BOOST_OS_WINDOWS
std::locale createUtf8Locale()
{
    // On windows we want to enforce the encoding (mostly UTF8) so use boost to generate it
    return boost::locale::generator().generate("");
}
#elif BOOST_OS_MACOS
std::locale createUtf8Locale()
{
    // Don't change the locale on OSX. Using "" fails with 'locale::facet::_S_create_c_locale name not valid'
    return LocaleHelper::getBfsDefaultLocale();
}
#elif BOOST_OS_HAIKU
std::locale createUtf8Locale()
{
    // Don't change the locale on HAIKU. Using "" fails with 'locale::facet::_S_create_c_locale name not valid'
    return LocaleHelper::getBfsDefaultLocale();
}
#else
std::locale createUtf8Locale()
{
    // In linux we use the system locale but change the codecvt facet to the one boost is using
    // (Assumed to be correct for our system)
    std::locale newLocale("");
    // Check that the system locale produces correct UTF-8 numbers
    std::stringstream ss;
    ss.imbue(newLocale);
    ss << std::fixed << std::setprecision(10) << 123456.789; // Include thousand and number separator
    const auto sNumber = ss.str();
    // If the string is not a valid UTF-8 string, fall back to classic locale
    if(!s25util::isValidUTF8(sNumber))
        newLocale = std::locale::classic();
    return newLocale.combine<bfs::path::codecvt_type>(LocaleHelper::getBfsDefaultLocale());
}
#endif
} // namespace

const std::locale& LocaleHelper::getBfsDefaultLocale()
{
    static std::locale bfsDefaultLocale = getBfsLocale();
    return bfsDefaultLocale;
}

bool LocaleHelper::init()
{
    static LocaleResetter localeResetter;
    // Check and set locale (avoids errors caused by invalid locales later like #420)
    try
    {
        // Call first to correctly init static variable
        getBfsDefaultLocale();
        // Check for errors and use system locale.
        // So don't rely on string conversions to yield identical results on all systems as locale settings can be
        // changed!
        std::locale newLocale = createUtf8Locale();
        localeResetter.changeGlobalLocale(newLocale);
        // Use also the encoding (mostly UTF8) for bfs paths: http://stackoverflow.com/questions/23393870
        bfs::path::imbue(newLocale);
    } catch(std::exception& e)
    {
        std::cerr << "Error initializing your locale setting. ";
#if BOOST_OS_WINDOWS
        std::cerr << "Check your system language configuration!";
#else
#    if BOOST_OS_MACOS
        std::cerr
          << "OS X has known problems when using libstdc++. Try setting LC_ALL and LANG to \"C\" or compile with libc++"
          << std::endl;
#    endif
        std::string lcAll = System::getEnvVar("LC_ALL");
        std::string lang = System::getEnvVar("LANG");
        std::cerr << "Check your environment for invalid settings (e.g. LC_ALL";
        if(!lcAll.empty())
            std::cerr << "=" << lcAll;
        std::cerr << " or LANG";
        if(!lang.empty())
            std::cerr << "=" << lang;
        std::cerr << ")";
#endif
        std::cerr << std::endl;
        std::cerr << e.what() << std::endl;
        return false;
    }
    return true;
}
