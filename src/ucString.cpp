// Copyright (c) 2005 - 2017 Settlers Freaks (sf-team at siedler25.org)
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

#include "ucString.h"
#include <utf8.h>
#include <type_traits>

/// Decorator for an iterator that converts the elements to their unsigned equivalents
/// Required because sign of char/wchar_t is undefined and we need it to be unsigned for some conversions
template<class T_Container>
struct GetUnsignedIterator
{
    using OldIterator = typename T_Container::const_iterator;
    using oldTraits = std::iterator_traits<OldIterator>;
    using OldValueType = typename oldTraits::value_type;
    using NewValueType = std::make_unsigned_t<OldValueType>;

    class UnsignedIterator : public std::iterator<typename std::forward_iterator_tag, NewValueType> //-V690
    {
        OldIterator oldIt;

    public:
        UnsignedIterator(OldIterator it) : oldIt(it) {}
        UnsignedIterator(const UnsignedIterator& it) : oldIt(it.oldIt) {}
        UnsignedIterator& operator++()
        {
            ++oldIt;
            return *this;
        }
        UnsignedIterator operator++(int)
        {
            UnsignedIterator old(*this);
            ++oldIt;
            return old;
        }
        bool operator==(const UnsignedIterator& rhs) { return oldIt == rhs.oldIt; }
        bool operator!=(const UnsignedIterator& rhs) { return oldIt != rhs.oldIt; }
        NewValueType operator*() { return *oldIt; }
    };

    using type = UnsignedIterator;
};

std::u32string cvUTF8ToUnicode(const std::string& other)
{
    return utf8::utf8to32(other);
}

std::string cvUnicodeToUTF8(const std::u32string& other)
{
    return utf8::utf32to8(other);
}

bool isValidUTF8(const std::string& text)
{
    return utf8::is_valid(text);
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
        using UIterator = GetUnsignedIterator<std::wstring>::type;
        utf8::utf32to8(UIterator(other.begin()), UIterator(other.end()), std::back_inserter(result));
        return result;
    }
}

std::u32string cvWideStringToUnicode(const std::wstring& other)
{
    try
    {
        // Try to convert from UTF-16 (UCS-2 is part of UTF-16)
        std::string tmp;
        tmp.reserve(other.length());
        utf8::utf16to8(other.begin(), other.end(), std::back_inserter(tmp));
        return cvUTF8ToUnicode(tmp);
    } catch(...)
    {
        // If that fails, assume pure unicode
        std::u32string result;
        result.reserve(other.size());
        // Use unsigned elements to avoid wrong sign extension
        using UIterator = GetUnsignedIterator<std::wstring>::UnsignedIterator;
        result.assign(UIterator(other.begin()), UIterator(other.end()));
        return result;
    }
}

std::wstring cvUTF8ToWideString(const std::string& other)
{
    std::wstring result;
    result.reserve(other.length());

    if(!isValidUTF8(other))
    {
        std::string tmp;
        tmp.reserve(other.length());
        utf8::replace_invalid(other.begin(), other.end(), std::back_inserter(tmp));
        utf8::utf8to16(tmp.begin(), tmp.end(), std::back_inserter(result));
    } else
        utf8::utf8to16(other.begin(), other.end(), std::back_inserter(result));

    return result;
}

std::string cvStringToUTF8(const std::string& other)
{
    if(isValidUTF8(other))
        return other;

    std::string result;
    result.reserve(other.length());
    // Use unsigned elements to avoid wrong sign extension
    using UIterator = GetUnsignedIterator<std::string>::type;
    utf8::utf32to8(UIterator(other.begin()), UIterator(other.end()), std::back_inserter(result));
    return result;
}

std::u32string cvStringToUnicode(const std::string& other)
{
    // Use unsigned elements to avoid wrong sign extension
    using UIterator = GetUnsignedIterator<std::string>::type;
    return std::u32string(UIterator(other.begin()), UIterator(other.end()));
}
