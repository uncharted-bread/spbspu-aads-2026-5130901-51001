#ifndef SHA1_HPP
#define SHA1_HPP

#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>

namespace gordejchik {
  struct sha1_digest_t {
    uint8_t bytes[20];
  };

  sha1_digest_t computeSha1(const uint8_t* data, size_t len);

  template< class T >
  struct Sha1Hash;

  template<>
  struct Sha1Hash< std::string > {
    size_t operator()(const std::string& key) const;
  };

  template<>
  struct Sha1Hash< std::pair< std::string, std::string > > {
    size_t operator()(const std::pair< std::string, std::string >& key) const;
  };

  struct StringEqual {
    bool operator()(const std::string& a, const std::string& b) const
    {
      return a == b;
    }
  };

  struct PairStringEqual {
    bool operator()(const std::pair< std::string, std::string >& a,
        const std::pair< std::string, std::string >& b) const
    {
      return a.first == b.first && a.second == b.second;
    }
  };

}

#endif
