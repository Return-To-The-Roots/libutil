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
#include "ucString.h"
#include "../utf8/utf8.h"
#include <boost/type_traits.hpp>
#include <algorithm>

/// Decorator for an iterator that converts the elements to their unsigned equivalents
/// Required because sign of char/wchar_t is undefined and we need it to be unsigned for some conversions
template<class T_Container>
struct GetUnsignedIterator{
    typedef typename T_Container::const_iterator OldIterator;
    typedef std::iterator_traits<OldIterator> oldTraits;
    typedef typename oldTraits::value_type OldValueType;
    typedef typename boost::make_unsigned<OldValueType>::type NewValueType;

    class UnsignedIterator: public std::iterator<typename std::forward_iterator_tag, NewValueType>
    {
        OldIterator oldIt;
    public:
        UnsignedIterator(OldIterator it): oldIt(it) {}
        UnsignedIterator(const UnsignedIterator& it): oldIt(it.oldIt) {}
        UnsignedIterator& operator++() { ++oldIt; return *this; }
        UnsignedIterator operator++(int) { UnsignedIterator old(*this); ++oldIt; return old; }
        bool operator==(const UnsignedIterator& rhs) { return oldIt == rhs.oldIt; }
        bool operator!=(const UnsignedIterator& rhs) { return oldIt != rhs.oldIt; }
        NewValueType operator*() { return *oldIt; }
    };
};

ucString cvUTF8ToUnicode(const std::string& other)
{
    ucString result;
    typedef utf8::iterator<std::string::const_iterator> iterator;
    result.assign(iterator(other.begin(), other.begin(), other.end()), iterator(other.end(), other.begin(), other.end()));
    return result;
}

std::string cvUnicodeToUTF8(const ucString& other)
{
    std::string result;
    result.reserve(other.length());
    utf8::utf32to8(other.begin(), other.end(), std::back_inserter(result));
    return result;
}

std::string cvWideStringToUTF8(const std::wstring& other)
{
    std::string result;
    result.reserve(other.length());
    try
    {
        // Try to convert from UTF-16 (UCS-2 is part of UTF-16)
        utf8::utf16to8(other.begin(), other.end(), std::back_inserter(result));
        return result;
    } catch(...)
    {
        // If that fails, assume pure unicode
        result.clear();
        // Use unsigned elements to avoid wrong sign extension
        typedef GetUnsignedIterator<std::wstring>::UnsignedIterator UIterator;
        utf8::utf32to8(UIterator(other.begin()), UIterator(other.end()), std::back_inserter(result));
        return result;
    }
}

ucString cvWideStringToUnicode(const std::wstring& other)
{
    try
    {
        // Try to convert from UTF-16 (UCS-2 is part of UTF-16)
        std::string tmp;
        tmp.reserve(other.length());
        utf8::utf16to8(other.begin(), other.end(), std::back_inserter(tmp));
        return cvUTF8ToUnicode(tmp);
    }
    catch(...)
    {
        // If that fails, assume pure unicode
        ucString result;
        result.reserve(other.size());
        // Use unsigned elements to avoid wrong sign extension
        typedef GetUnsignedIterator<std::wstring>::UnsignedIterator UIterator;
        result.assign(UIterator(other.begin()), UIterator(other.end()));
        return result;
    }
}

std::wstring cvUTF8ToWideString(const std::string& other)
{
    std::string text;
    if(!utf8::is_valid(other.begin(), other.end()))
    {
        text.reserve(other.length());
        utf8::replace_invalid(other.begin(), other.end(), std::back_inserter(text));
    } else
        text = other;

    std::wstring tmp;
    tmp.reserve(text.length());
    utf8::utf8to16(text.begin(), text.end(), std::back_inserter(tmp));
    return tmp;
}

std::string cvStringToUTF8(const std::string& other)
{
    if(utf8::is_valid(other.begin(), other.end()))
        return other;

    std::string result;
    result.reserve(other.length());
    // Use unsigned elements to avoid wrong sign extension
    typedef GetUnsignedIterator<std::string>::UnsignedIterator UIterator;
    utf8::utf32to8(UIterator(other.begin()), UIterator(other.end()), std::back_inserter(result));
    return result;
}

ucString cvStringToUnicode(const std::string& other)
{
    ucString result;
    // Use unsigned elements to avoid wrong sign extension
    typedef GetUnsignedIterator<std::string>::UnsignedIterator UIterator;
    result.assign(UIterator(other.begin()), UIterator(other.end()));
    return result;
}
