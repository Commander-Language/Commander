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
    inline std::size_t combineHashes(const std::vector<std::size_t>& hashes) {
        //  Reference: https://www.cs.hmc.edu/~geoff/classes/hmc.cs070.200101/homework10/hashfuncs.html
        const auto multiply = [](const std::size_t key) {
            constexpr std::size_t byteSize = 8;
            constexpr std::size_t numSize = sizeof(std::size_t);
            constexpr double knuthsNumber = 0.6180339887498949;
            constexpr auto scale = static_cast<std::size_t>(
                    knuthsNumber * static_cast<double>(std::size_t {1} << (byteSize * numSize - 1)));
            constexpr std::size_t rshift = byteSize * numSize / 2;

            return key * scale >> rshift;
        };

        //  Reference: https://stackoverflow.com/questions/2590677/how-do-i-combine-hash-values-in-c0x
        std::size_t resultHashVal = 0;
        for (const auto& hash : hashes) {
            constexpr std::size_t entropy = 0x9e3779b9;
            constexpr std::size_t lshift = 6;
            constexpr std::size_t rshift = 2;
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
    std::size_t combineHashes(const ItemType&... items) {
        std::vector<std::size_t> hashes;
        ([&](const auto& item) { hashes.emplace_back(std::hash<ItemType> {}(item)); }(items), ...);
        return combineHashes(hashes);
    }

}  //  namespace Util

#endif  //  COMBINE_HASHES_HPP
