/**
* @file combine_hashes.hpp
* @brief Defines the `combineHashes` function, which combines two hashed values.
*/

#ifndef COMBINE_HASHES_HPP
#define COMBINE_HASHES_HPP

#include <cstddef>

namespace Util {

   /**
    * @brief Combines two hashed values in a deterministic way.
    *
    * @param hash1 The first hashed value to use.
    * @param hash2 The second hashed value to use.
    * @return A deterministic hashed combination of the two given hashed values.
    */
   size_t combineHashes(size_t hash1, size_t hash2);

}  //  namespace Util

#endif  //  COMBINE_HASHES_HPP
