/**
* @file combine_hashes.cpp
* @brief Implements the `combineHashes` function, which combines two hashed values.
*/

#include "combine_hashes.hpp"

namespace Util {

   size_t combineHashes(size_t hash1, size_t hash2) {
       constexpr size_t entropy = 0x9e3779b9;
       constexpr size_t leftShift = 6;
       constexpr size_t rightShift = 2;

       return hash1 + entropy + (hash2 << leftShift) + (hash2 >> rightShift);
   }

}  //  namespace Util
