#include "sha1.hpp"

static uint32_t rotateLeft32(uint32_t value, unsigned int count)
{
  return (value << count) | (value >> (32 - count));
}

static void processBlock(const uint8_t block[64], uint32_t h[5])
{
  uint32_t w[80] = {};
  for (int i = 0; i < 16; ++i) {
    w[i] = (static_cast< uint32_t >(block[i * 4]) << 24)
        | (static_cast< uint32_t >(block[i * 4 + 1]) << 16)
        | (static_cast< uint32_t >(block[i * 4 + 2]) << 8)
        | static_cast< uint32_t >(block[i * 4 + 3]);
  }
  for (int i = 16; i < 80; ++i) {
    w[i] = rotateLeft32(w[i - 3] ^ w[i - 8] ^ w[i - 14] ^ w[i - 16], 1);
  }

  uint32_t a = h[0];
  uint32_t b = h[1];
  uint32_t c = h[2];
  uint32_t d = h[3];
  uint32_t e = h[4];

  for (int i = 0; i < 80; ++i) {
    uint32_t f = 0;
    uint32_t k = 0;
    if (i < 20) {
      f = (b & c) | ((~b) & d);
      k = 0x5A827999;
    } else if (i < 40) {
      f = b ^ c ^ d;
      k = 0x6ED9EBA1;
    } else if (i < 60) {
      f = (b & c) | (b & d) | (c & d);
      k = 0x8F1BBCDC;
    } else {
      f = b ^ c ^ d;
      k = 0xCA62C1D6;
    }
    uint32_t temp = rotateLeft32(a, 5) + f + e + k + w[i];
    e = d;
    d = c;
    c = rotateLeft32(b, 30);
    b = a;
    a = temp;
  }

  h[0] += a;
  h[1] += b;
  h[2] += c;
  h[3] += d;
  h[4] += e;
}

static size_t digestToSizeT(const gordejchik::sha1_digest_t& digest)
{
  size_t result = 0;
  for (size_t i = 0; i < sizeof(size_t) && i < sizeof(digest.bytes); ++i) {
    result = (result << 8) | digest.bytes[i];
  }
  return result;
}

gordejchik::sha1_digest_t gordejchik::computeSha1(const uint8_t* data, size_t len)
{
  uint32_t h[5] = {
    0x67452301, 0xEFCDAB89, 0x98BADCFE, 0x10325476, 0xC3D2E1F0,
  };

  size_t i = 0;
  for (; i + 64 <= len; i += 64) {
    processBlock(data + i, h);
  }

  uint8_t block[128] = {};
  const size_t remaining = len - i;
  for (size_t j = 0; j < remaining; ++j) {
    block[j] = data[i + j];
  }
  block[remaining] = 0x80;

  const size_t padLen = (remaining < 56) ? 64 : 128;
  const uint64_t bitLen = static_cast< uint64_t >(len) * 8;
  block[padLen - 8] = static_cast< uint8_t >(bitLen >> 56);
  block[padLen - 7] = static_cast< uint8_t >(bitLen >> 48);
  block[padLen - 6] = static_cast< uint8_t >(bitLen >> 40);
  block[padLen - 5] = static_cast< uint8_t >(bitLen >> 32);
  block[padLen - 4] = static_cast< uint8_t >(bitLen >> 24);
  block[padLen - 3] = static_cast< uint8_t >(bitLen >> 16);
  block[padLen - 2] = static_cast< uint8_t >(bitLen >> 8);
  block[padLen - 1] = static_cast< uint8_t >(bitLen);

  processBlock(block, h);
  if (padLen == 128) {
    processBlock(block + 64, h);
  }

  sha1_digest_t result;
  for (int j = 0; j < 5; ++j) {
    result.bytes[j * 4] = static_cast< uint8_t >(h[j] >> 24);
    result.bytes[j * 4 + 1] = static_cast< uint8_t >(h[j] >> 16);
    result.bytes[j * 4 + 2] = static_cast< uint8_t >(h[j] >> 8);
    result.bytes[j * 4 + 3] = static_cast< uint8_t >(h[j]);
  }
  return result;
}

size_t gordejchik::Sha1Hash< std::string >::operator()(const std::string& key) const
{
  const uint8_t* ptr = reinterpret_cast< const uint8_t* >(key.data());
  return digestToSizeT(computeSha1(ptr, key.size()));
}

size_t gordejchik::Sha1Hash< std::pair< std::string, std::string > >::operator()(
    const std::pair< std::string, std::string >& key) const
{
  std::string combined = key.first;
  combined += '\0';
  combined += key.second;
  const uint8_t* ptr = reinterpret_cast< const uint8_t* >(combined.data());
  return digestToSizeT(computeSha1(ptr, combined.size()));
}

bool gordejchik::StringEqual::operator()(const std::string& a,
    const std::string& b) const
{
  return a == b;
}

bool gordejchik::PairStringEqual::operator()(
    const std::pair< std::string, std::string >& a,
    const std::pair< std::string, std::string >& b) const
{
  return a.first == b.first && a.second == b.second;
}
