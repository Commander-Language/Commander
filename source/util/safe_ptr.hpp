#ifndef SAFE_PTR_HPP
#define SAFE_PTR_HPP

#include <memory>
#include <mutex>

namespace Util {
    template<typename DataType>
    class SafePtr {
        using MutexType = std::recursive_mutex;
        using LockType = std::unique_lock<MutexType>;

        template<typename ReqLockType>
        class AutoLockType {
            DataType* const _data;
            ReqLockType _lock;

        public:
            AutoLockType(DataType* const data, MutexType& guard) : _data(data), _lock(guard) {}

            AutoLockType(AutoLockType&& other) noexcept
                : _data(std::move(other._data)), _lock(std::move(other._lock)) {}

            DataType* operator->() { return _data; }

            const DataType* operator->() const { return _data; }
        };

        template<typename ReqLockType>
        class AutoLockObjType {
            DataType* const _data;
            ReqLockType _lock;

        public:
            AutoLockObjType(DataType* const data, MutexType& guard) : _data(data), _lock(guard) {}

            AutoLockObjType(AutoLockObjType&& other) noexcept
                : _data(std::move(other._data)), _lock(std::move(other._lock)) {}

            template<typename ArgType>
            auto operator[](ArgType arg) -> decltype((*_data)[arg]) {
                return (*_data)[arg];
            }

            explicit operator DataType() { return *_data; }
        };

        const std::shared_ptr<DataType> _data;

        std::shared_ptr<MutexType> _guard;

    public:
        template<typename... ArgType>
        SafePtr(ArgType... args) : _data(std::make_shared<DataType>(args...)), _guard(std::make_shared<MutexType>()) {}

        AutoLockType<LockType> operator->() { return AutoLockType<LockType>(_data.get(), *_guard); }

        AutoLockType<LockType> operator->() const { return AutoLockType<LockType>(_data.get(), *_guard); }

        AutoLockObjType<LockType> operator*() { return AutoLockObjType<LockType>(_data.get(), *_guard); }

        AutoLockObjType<LockType> operator*() const { return AutoLockObjType<LockType>(_data.get(), *_guard); }
    };

}  // namespace Util

#endif  // SAFE_PTR_HPP
