/**
 * @file combine_hashes.hpp
 * @brief Defines the `combineHashes` function, which combines two hashed values.
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
        constexpr size_t entropy = 0x9e3779b9;
        constexpr size_t lshift = 6;
        constexpr size_t rshift = 2;

        size_t result = 0;
        for (const auto& val : hashes) {
            result ^= std::hash<size_t> {}(val) + entropy + (result << lshift) + (result >> rshift);
        }
        return result;
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
        ([&hashes](const auto& item) { hashes.emplace_back(std::hash<ItemType> {}(item)); }(items), ...);
        return combineHashes(hashes);
    }

}  //  namespace Util

#endif  //  COMBINE_HASHES_HPP
