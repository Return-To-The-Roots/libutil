// Copyright (C) 2005 - 2021 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#include "s25util/System.h"
#include "s25util/utf8.h"
#include <boost/filesystem.hpp>
#include <boost/nowide/convert.hpp>
#include <boost/test/unit_test.hpp>

namespace fs = boost::filesystem;

namespace boost::test_tools::tt_detail {
template<>
struct print_log_value<std::wstring>
{
    void operator()(std::ostream& out, const std::wstring& value) { out << boost::nowide::narrow(value); }
};
} // namespace boost::test_tools::tt_detail

BOOST_AUTO_TEST_SUITE(SystemTestSuite)

BOOST_AUTO_TEST_CASE(GetSetRemoveEnvVar)
{
#ifdef _WIN32
    const std::string varName = boost::nowide::narrow(L"_RTTR_TEST_VAR_WITH_UMLAUTS_\u00E4\u00F6\u00FC_END_");
    const std::string varValue = boost::nowide::narrow(L"ValueWithSpecialChars_\u0139\u00D4_END");
#else
    // Use UTF8 (wide string not portable, either 16 or 32 bit)
    const std::string varName = "_RTTR_TEST_VAR_WITH_UMLAUTS_\xC3\xA4\xC3\xB6\xC3\xBC"
                                "_END_";
    const std::string varValue = "ValueWithSpecialChars_\xC4\xB9\xC3\x94"
                                 "_END";
#endif // _WIN32

    // Var should not exist
    BOOST_TEST_REQUIRE(!System::envVarExists(varName));
    BOOST_TEST_REQUIRE(System::getEnvVar(varName) == "");

    // Set variable
    BOOST_TEST_REQUIRE(System::setEnvVar(varName, varValue));
    // Now it should exist
    BOOST_TEST_REQUIRE(System::envVarExists(varName));
    const std::string value = System::getEnvVar(varName);
    BOOST_TEST_REQUIRE(s25util::isValidUTF8(value));
    BOOST_TEST_REQUIRE(value == varValue);
    // Remove
    BOOST_TEST_REQUIRE(System::removeEnvVar(varName));
    BOOST_TEST_REQUIRE(!System::envVarExists(varName));
}

BOOST_AUTO_TEST_CASE(GetPathFromEnv)
{
    std::string nonExistingVarName = "NonExistantVar";
#ifdef _WIN32
    std::string existingVarName = "WINDIR";
#else
    std::string existingVarName = "HOME";
#endif // _WIN32
    BOOST_TEST_REQUIRE(!System::envVarExists(nonExistingVarName));
    BOOST_TEST_REQUIRE(System::getPathFromEnvVar(nonExistingVarName).empty());
    BOOST_TEST_REQUIRE(System::envVarExists(existingVarName));
    BOOST_TEST_REQUIRE(!System::getPathFromEnvVar(existingVarName).empty());
}

BOOST_AUTO_TEST_CASE(GetHome)
{
    fs::path homePath = System::getHomePath();
    BOOST_TEST_REQUIRE(!homePath.empty());
    BOOST_TEST_REQUIRE(fs::exists(homePath));
}

BOOST_AUTO_TEST_CASE(GetUsername)
{
    BOOST_TEST_REQUIRE(!System::getUserName().empty());
    BOOST_TEST_REQUIRE(s25util::isValidUTF8(System::getUserName()));
}

BOOST_AUTO_TEST_CASE(GetExePath)
{
    fs::path exePath = System::getExecutablePath();
    BOOST_TEST_REQUIRE(!exePath.empty());
    BOOST_TEST_REQUIRE(fs::exists(exePath));
    BOOST_TEST_REQUIRE(fs::is_regular_file(exePath));
}

BOOST_AUTO_TEST_SUITE_END()
