/**
 * @file combine_hashes.hpp
 * @brief Defines the `combineHashes` functions, which combines two hashed values.
 * @details Because these functions are inlined and templated, implementation is in the header file.
 */

#ifndef COMBINE_HASHES_HPP
#define COMBINE_HASHES_HPP

#include <cstddef>
#include <functional>
#include <vector>

namespace Util {

    /**
     * @brief Combines multiple hashed values in a deterministic way.
     *
     * @param hashes The list of hashed values to use.
     * @return A deterministic hashed combination of the given hashed values.
     */
    inline size_t combineHashes(const std::vector<size_t>& hashes) {
        //  Reference: https://www.cs.hmc.edu/~geoff/classes/hmc.cs070.200101/homework10/hashfuncs.html
        const auto multiply = [](size_t key) {
            constexpr size_t byteSize = 8;
            constexpr size_t lshift = byteSize * sizeof(size_t) - 1;
            constexpr double knuthsNumber = 0.6180339887498949;

            const double scaled = (double)key * knuthsNumber;
            const double fraction = scaled - (double)(size_t)scaled;
            return (size_t)(fraction * (double)((size_t)1 << lshift));
        };

        //  Reference: https://stackoverflow.com/questions/2590677/how-do-i-combine-hash-values-in-c0x
        constexpr size_t entropy = 0x9e3779b9;
        constexpr size_t lshift = 6;
        constexpr size_t rshift = 2;

        size_t resultHashVal = 0;
        for (const auto& hash : hashes) {
            resultHashVal ^= multiply(hash) + entropy + (resultHashVal << lshift) + (resultHashVal >> rshift);
        }

        return resultHashVal;
    }

    /**
     * @brief Hashes multiple items together in a deterministic way.
     *
     * @param items The items to hash.
     * @return A deterministic hashed combination of the given items.
     */
    template<typename... ItemType>
    inline size_t combineHashes(const ItemType&... items) {
        std::vector<size_t> hashes;
        ([&](const auto& item) { hashes.emplace_back(std::hash<ItemType> {}(item)); }(items), ...);
        return combineHashes(hashes);
    }

}  //  namespace Util

#endif  //  COMBINE_HASHES_HPP
