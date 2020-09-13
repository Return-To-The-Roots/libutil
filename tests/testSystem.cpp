// Copyright (c) 2016 - 2020 Settlers Freaks (sf-team at siedler25.org)
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

#include "s25util/System.h"
#include "s25util/utf8.h"
#include <boost/filesystem.hpp>
#include <boost/nowide/convert.hpp>
#include <boost/test/unit_test.hpp>

namespace fs = boost::filesystem;

namespace boost { namespace test_tools { namespace tt_detail {
    template<>
    struct print_log_value<std::wstring>
    {
        void operator()(std::ostream& out, const std::wstring& value) { out << boost::nowide::narrow(value); }
    };
}}} // namespace boost::test_tools::tt_detail

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
