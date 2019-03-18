//          Copyright Kevin Sopp 2007.
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

#include "md5.hpp"
#include <algorithm>

namespace s25util {
namespace {
    // auxiliary functions
    struct aux_f
    {
        constexpr uint32_t operator()(uint32_t x, uint32_t y, uint32_t z) const { return (x & y) | (~x & z); }
    };
    struct aux_g
    {
        constexpr uint32_t operator()(uint32_t x, uint32_t y, uint32_t z) const { return (x & z) | (y & ~z); }
    };
    struct aux_h
    {
        constexpr uint32_t operator()(uint32_t x, uint32_t y, uint32_t z) const { return x ^ y ^ z; }
    };
    struct aux_i
    {
        constexpr uint32_t operator()(uint32_t x, uint32_t y, uint32_t z) const { return y ^ (x | ~z); }
    };

    constexpr uint32_t left_rotate(uint32_t x, uint32_t num_bits) { return (x << num_bits) | (x >> (32 - num_bits)); }
    template<typename AuxFunctor>
    constexpr void transform(uint32_t& a, uint32_t b, uint32_t c, uint32_t d, uint32_t k, uint32_t s, uint32_t i)
    {
        a = b + left_rotate(a + AuxFunctor()(b, c, d) + k + i, s);
    }

} // namespace

md5::context::context()
{
    reset();
}

void md5::context::reset()
{
    state[0] = 0x67452301;
    state[1] = 0xefcdab89;
    state[2] = 0x98badcfe;
    state[3] = 0x10325476;
    num_bits = 0;
    std::fill(buffer.begin(), buffer.end(), 0);
}

std::string md5::toString() const
{
    std::string result(32, 0);
    for(int i = 0; i < 16; ++i)
        std::sprintf(&result[i * 2], "%02x", digest()[i]);
    return result;
}

void md5::process(const void* vdata, size_type len, bool add)
{
    const auto* data = reinterpret_cast<const uint8_t*>(vdata);
    if(add)
        ctx_ = ctx_backup_;

    size_type index = 0;                             // index into data array
    size_type bytes_in_buf = ctx_.num_bits / 8 % 64; // bytes in ctx.buffer

    // process any bytes that may still be left in the context from a previous
    // invocation
    if(bytes_in_buf)
    {
        const size_type bytes_to_copy = len > 64 - bytes_in_buf ? 64 - bytes_in_buf : len;
        std::copy_n(data, bytes_to_copy, ctx_.buffer.begin() + bytes_in_buf);
        index += bytes_to_copy;
        ctx_.num_bits += bytes_to_copy * 8;
        if(len + bytes_in_buf >= 64)
        {
            process(ctx_, ctx_.buffer.data());
            ctx_.num_bits += (len - index) * 8;
            bytes_in_buf = 0;
        } else
            bytes_in_buf += bytes_to_copy;
    } else
        ctx_.num_bits += len * 8;

    // now process the data in 64 byte chunks
    for(; len - index >= 64; index += 64)
        process(ctx_, data + index);

    const size_type remaining_bytes = len - index;

    // backup the context and copy the remaining bytes over so that we can pick up
    // where we left off if this function gets called with add=true again
    ctx_backup_ = ctx_;
    std::copy_n(&data[index], remaining_bytes, ctx_backup_.buffer.begin());

    // now add the padding and store the message digest
    context::Buffer padding = {};

    if(bytes_in_buf)
        std::copy_n(ctx_.buffer.data(), bytes_in_buf, padding.data());

    std::copy_n(data + index, remaining_bytes, padding.begin() + bytes_in_buf);
    padding[bytes_in_buf + remaining_bytes] = 0x80;
    if(remaining_bytes >= 56)
    {
        process(ctx_, padding.data());
        std::fill(padding.begin(), padding.end(), 0);
    }

    // store num_bits in little endian format
    for(int i = 0; i < 8; ++i)
        padding[56 + i] = (ctx_.num_bits >> 8 * i) & 0xff;

    process(ctx_, padding.data());
    store_msg_digest(ctx_);
    ctx_.reset();
}

void md5::clear()
{
    ctx_.reset();
    ctx_backup_.reset();
}

// processes one chunk of 64 bytes
void md5::process(context& ctx, const uint8_t* msg) const
{
    // store msg in x buffer in little endian format
    uint32_t x[16];
    for(int i = 0, j = 0; i < 16; ++i, j += 4)
        x[i] = (uint32_t)msg[j] | ((uint32_t)msg[j + 1] << 8) | ((uint32_t)msg[j + 2] << 16) | ((uint32_t)msg[j + 3] << 24);

    uint32_t a = ctx.state[0];
    uint32_t b = ctx.state[1];
    uint32_t c = ctx.state[2];
    uint32_t d = ctx.state[3];

    // round 1
    transform<aux_f>(a, b, c, d, x[0], 7, 0xd76aa478);
    transform<aux_f>(d, a, b, c, x[1], 12, 0xe8c7b756);
    transform<aux_f>(c, d, a, b, x[2], 17, 0x242070db);
    transform<aux_f>(b, c, d, a, x[3], 22, 0xc1bdceee);
    transform<aux_f>(a, b, c, d, x[4], 7, 0xf57c0faf);
    transform<aux_f>(d, a, b, c, x[5], 12, 0x4787c62a);
    transform<aux_f>(c, d, a, b, x[6], 17, 0xa8304613);
    transform<aux_f>(b, c, d, a, x[7], 22, 0xfd469501);
    transform<aux_f>(a, b, c, d, x[8], 7, 0x698098d8);
    transform<aux_f>(d, a, b, c, x[9], 12, 0x8b44f7af);
    transform<aux_f>(c, d, a, b, x[10], 17, 0xffff5bb1);
    transform<aux_f>(b, c, d, a, x[11], 22, 0x895cd7be);
    transform<aux_f>(a, b, c, d, x[12], 7, 0x6b901122);
    transform<aux_f>(d, a, b, c, x[13], 12, 0xfd987193);
    transform<aux_f>(c, d, a, b, x[14], 17, 0xa679438e);
    transform<aux_f>(b, c, d, a, x[15], 22, 0x49b40821);

    // round 2
    transform<aux_g>(a, b, c, d, x[1], 5, 0xf61e2562);
    transform<aux_g>(d, a, b, c, x[6], 9, 0xc040b340);
    transform<aux_g>(c, d, a, b, x[11], 14, 0x265e5a51);
    transform<aux_g>(b, c, d, a, x[0], 20, 0xe9b6c7aa);
    transform<aux_g>(a, b, c, d, x[5], 5, 0xd62f105d);
    transform<aux_g>(d, a, b, c, x[10], 9, 0x2441453);
    transform<aux_g>(c, d, a, b, x[15], 14, 0xd8a1e681);
    transform<aux_g>(b, c, d, a, x[4], 20, 0xe7d3fbc8);
    transform<aux_g>(a, b, c, d, x[9], 5, 0x21e1cde6);
    transform<aux_g>(d, a, b, c, x[14], 9, 0xc33707d6);
    transform<aux_g>(c, d, a, b, x[3], 14, 0xf4d50d87);
    transform<aux_g>(b, c, d, a, x[8], 20, 0x455a14ed);
    transform<aux_g>(a, b, c, d, x[13], 5, 0xa9e3e905);
    transform<aux_g>(d, a, b, c, x[2], 9, 0xfcefa3f8);
    transform<aux_g>(c, d, a, b, x[7], 14, 0x676f02d9);
    transform<aux_g>(b, c, d, a, x[12], 20, 0x8d2a4c8a);

    // round 3
    transform<aux_h>(a, b, c, d, x[5], 4, 0xfffa3942);
    transform<aux_h>(d, a, b, c, x[8], 11, 0x8771f681);
    transform<aux_h>(c, d, a, b, x[11], 16, 0x6d9d6122);
    transform<aux_h>(b, c, d, a, x[14], 23, 0xfde5380c);
    transform<aux_h>(a, b, c, d, x[1], 4, 0xa4beea44);
    transform<aux_h>(d, a, b, c, x[4], 11, 0x4bdecfa9);
    transform<aux_h>(c, d, a, b, x[7], 16, 0xf6bb4b60);
    transform<aux_h>(b, c, d, a, x[10], 23, 0xbebfbc70);
    transform<aux_h>(a, b, c, d, x[13], 4, 0x289b7ec6);
    transform<aux_h>(d, a, b, c, x[0], 11, 0xeaa127fa);
    transform<aux_h>(c, d, a, b, x[3], 16, 0xd4ef3085);
    transform<aux_h>(b, c, d, a, x[6], 23, 0x4881d05);
    transform<aux_h>(a, b, c, d, x[9], 4, 0xd9d4d039);
    transform<aux_h>(d, a, b, c, x[12], 11, 0xe6db99e5);
    transform<aux_h>(c, d, a, b, x[15], 16, 0x1fa27cf8);
    transform<aux_h>(b, c, d, a, x[2], 23, 0xc4ac5665);

    // round 4
    transform<aux_i>(a, b, c, d, x[0], 6, 0xf4292244);
    transform<aux_i>(d, a, b, c, x[7], 10, 0x432aff97);
    transform<aux_i>(c, d, a, b, x[14], 15, 0xab9423a7);
    transform<aux_i>(b, c, d, a, x[5], 21, 0xfc93a039);
    transform<aux_i>(a, b, c, d, x[12], 6, 0x655b59c3);
    transform<aux_i>(d, a, b, c, x[3], 10, 0x8f0ccc92);
    transform<aux_i>(c, d, a, b, x[10], 15, 0xffeff47d);
    transform<aux_i>(b, c, d, a, x[1], 21, 0x85845dd1);
    transform<aux_i>(a, b, c, d, x[8], 6, 0x6fa87e4f);
    transform<aux_i>(d, a, b, c, x[15], 10, 0xfe2ce6e0);
    transform<aux_i>(c, d, a, b, x[6], 15, 0xa3014314);
    transform<aux_i>(b, c, d, a, x[13], 21, 0x4e0811a1);
    transform<aux_i>(a, b, c, d, x[4], 6, 0xf7537e82);
    transform<aux_i>(d, a, b, c, x[11], 10, 0xbd3af235);
    transform<aux_i>(c, d, a, b, x[2], 15, 0x2ad7d2bb);
    transform<aux_i>(b, c, d, a, x[9], 21, 0xeb86d391);

    ctx.state[0] += a;
    ctx.state[1] += b;
    ctx.state[2] += c;
    ctx.state[3] += d;
}

// store message digest in little endian format
void md5::store_msg_digest(const context& ctx)
{
    for(int i = 0, j = 0; i < 16; i += 4, ++j)
    {
        msg_digest_[i] = ctx.state[j] & 0xff;
        msg_digest_[i + 1] = (ctx.state[j] >> 8) & 0xff;
        msg_digest_[i + 2] = (ctx.state[j] >> 16) & 0xff;
        msg_digest_[i + 3] = (ctx.state[j] >> 24) & 0xff;
    }
}

bool operator==(const md5& lhs, const md5& rhs)
{
    return lhs.digest() == rhs.digest();
}

bool operator!=(const md5& lhs, const md5& rhs)
{
    return !(lhs == rhs);
}

bool operator==(const md5& m, const std::string& digest)
{
    return m.toString() == digest;
}

bool operator!=(const md5& m, const std::string& digest)
{
    return !(m == digest);
}

bool operator==(const std::string& digest, const md5& m)
{
    return m == digest;
}

bool operator!=(const std::string& digest, const md5& m)
{
    return m != digest;
}
} // namespace s25util
