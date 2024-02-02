/**
 * @file source/util/generated_map.hpp
 * @brief A wrapper around `std::unordered_map`, with a custom generator function for uninitialized entries.
 * @details Because this is a templated class, implementation is in this file.
 */

#ifndef GENERATED_MAP_HPP
#define GENERATED_MAP_HPP

#include <functional>
#include <unordered_map>

namespace Util {

    /**
     * @brief A wrapper around `std::unordered_map`, with a custom generator function for uninitialized entries.
     *
     * @tparam KeyType The type of the map's keys.
     * @tparam ValueType The type of the map's values.
     * @tparam KeyHashType An invocable class (i.e., a functor class) for hashing the map's `KeyType`.
     */
    template<typename KeyType, typename ValueType, typename KeyHashType = typename KeyType::Hash>
    class GeneratedMap {
    public:
        /**
         * @brief Class constructor.
         *
         * @param generator The generator function to use to generate uninitialized values.
         *                  Must accept a const reference to a key of type `KeyType` and return a new `ValueType`.
         */
        using GeneratorType = std::function<ValueType(const KeyType& key)>;
        GeneratedMap(GeneratorType generator);

        /**
         * @brief Gets the map's value for the given key.
         * @details If the map doesn't contain the given key,
         *          then generates a new value using the saved `_generator` function.
         *
         * @param key The key for which to return the map's value.
         * @return The value associated with the given key.
         */
        ValueType& operator[](const KeyType& key);

    private:
        /**
         * @brief The generator function to use to generate uninitialized values.
         *        Must accept a const reference to a key of type `KeyType` and return a new `ValueType`.
         */
        GeneratorType _generator;

        /**
         * @brief The mapping of `KeyType` keys to `ValueType` values.
         * @details Uses an instance of `KeyHashType` to hash keys.
         */
        std::unordered_map<KeyType, ValueType, KeyHashType> _map;
    };

    template<typename KeyType, typename ValueType, typename KeyHashType>
    GeneratedMap<KeyType, ValueType, KeyHashType>::GeneratedMap(GeneratedMap::GeneratorType generator)
        : _generator(std::move(generator)) {}

    template<typename KeyType, typename ValueType, typename KeyHashType>
    ValueType& GeneratedMap<KeyType, ValueType, KeyHashType>::operator[](const KeyType& key) {
        if (_map.count(key) == 0) _map[key] = _generator(key);

        return _map[key];
    }

}  //  namespace Util

#endif  //  GENERATED_MAP_HPP
