//          Copyright Kevin Sopp 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

// Implements 'The MD5 Message-Digest Algorithm' by R. Rivest
// RFC 1321 http://tools.ietf.org/html/rfc1321
//
// Modified for C++14 and usage in RTTR by the RTTR team 2019

#ifndef BOOST_MD5_HPP
#define BOOST_MD5_HPP

#include <array>
#include <cstdint>
#include <ostream>
#include <string>

namespace s25util {
class md5
{
public:
    typedef std::size_t size_type;

    md5(const std::string& data) { process(data, false); }
    md5(const void* data, size_type len) { process(data, len, false); }

    const uint8_t* digest() const { return msg_digest_.data(); }
    std::string toString() const;
    void process(const void* vdata, size_type len, bool add);
    void process(const std::string& data, bool add) { process(data.data(), data.length(), add); }

private:
    struct context
    {
        using Buffer = std::array<uint8_t, 64>;
        context();
        void reset();

        std::array<uint32_t, 4> state;
        uint64_t num_bits;
        std::array<uint8_t, 64> buffer;
    };

    // resets internal context
    void clear();
    void process(context& ctx, const uint8_t* msg) const;
    void store_msg_digest(const context&);

    context ctx_, ctx_backup_;
    std::array<uint8_t, 16> msg_digest_;
};

bool operator==(const md5& lhs, const md5& rhs);
bool operator!=(const md5& lhs, const md5& rhs);
bool operator==(const md5& m, const std::string& digest);
bool operator!=(const md5& m, const std::string& digest);
bool operator==(const std::string& digest, const md5& m);
bool operator!=(const std::string& digest, const md5& m);

template<typename charT, class traits>
std::basic_ostream<charT, traits>& operator<<(std::basic_ostream<charT, traits>& out, const md5& m)
{
    return out << m.toString();
}
} // namespace s25util

#endif
