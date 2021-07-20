#pragma once

#include <unordered_map>
#include <any>
#include <string>
#include <shared_mutex>

namespace Gaia::Blackboards
{
    /**
     * @brief Blackboard is a kind of key-value container which supports generic value types.
     */
    class Blackboard
    {
        /// Friend class accessor for boosting query process.
        template <typename ValueType>
        friend class Accessor;
    protected:
        /// Mutex for Values.
        std::shared_mutex ValuesMutex;
        /// Map for key-values storage.
        std::unordered_map<std::string, std::any> Values;

    public:
        /// Clear all values in this blackboard.
        void Clear();

        /// Remove the value with the given name.
        void RemoveValue(const std::string& name);

        /**
         * @brief Add a value to this blackboard.
         * @tparam ValueType The type of the value to add or replace.
         * @param name The name of the value to add or replace.
         * @param value The value to create or update.
         * @details
         *  The previous value with the same name will be replaced.
         */
        template <typename ValueType>
        void SetValue(const std::string& name, ValueType value)
        {
            std::unique_lock lock(ValuesMutex);
            auto item_iterator = Values.find(name);

            if (item_iterator != Values.end())
            {
                item_iterator->second = std::make_any<ValueType>(value);
            }
            else
            {
                Values.emplace(name, value);
            }
        }

        /**
         * @brief Get a value from this blackboard.
         * @tparam ValueType The type of the value to get,
         *                   it should be totally the same to the original type of the value
         *                   when it is firstly set in this blackboard.
         * @param name The name of the value to get.
         * @return Optional value with the given name,
         *         std::nullopt if it does not exist or does not fully match the demanded type.
         */
        template <typename ValueType>
        std::optional<ValueType> GetValue(const std::string& name)
        {
            std::shared_lock lock(ValuesMutex);
            auto item_iterator = Values.find(name);
            lock.unlock();

            if (item_iterator != Values.end() &&
                item_iterator->second.has_value() && item_iterator->second.type() == typeid(ValueType))
            {
                /* Due to the manually type check, this function call should not throw std::bad_any_cast
                   because of not matched types. */
                return std::any_cast<ValueType>(item_iterator->second);
            }

            return std::nullopt;
        }

        /**
         * @brief Get a value from this blackboard, or set it if it does not exist.
         * @tparam ValueType The type of the value to get,
         *                   it should be totally the same to the original type of the value
         *                   when it is firstly set in this blackboard.
         * @param name The name of the value to get.
         * @param default_value The value to set if it does not exist.
         * @return Optional value with the given name,
         *         std::nullopt if it does not exist or does not fully match the demanded type.
         */
        template <typename ValueType>
        ValueType& AcquireValue(const std::string& name, ValueType default_value)
        {
            std::unique_lock lock(ValuesMutex);
            auto item_iterator = Values.find(name);

            if (item_iterator == Values.end())
            {
                auto [inserted_iterator, inserted] = Values.emplace(name, default_value);
                item_iterator = inserted_iterator;
            }
            if (item_iterator->second.type() != typeid(ValueType))
            {
                item_iterator->second = std::make_any<ValueType>(default_value);
            }

            return *std::any_cast<ValueType>(&item_iterator->second);
        }

        /**
         * @brief Get a pointer to the value with the given name.
         * @tparam ValueType The type of the value to get,
         *                   it should be totally the same to the original type of the value
         *                   when it is firstly set in this blackboard.
         * @param name The name of the value to get.
         * @return The pointer to the value with the given name,
         *         nullptr if value with the demanded name does not exist,
         *         or its type can not fully match the demanded type.
         */
        template <typename ValueType>
        ValueType* GetReference(const std::string& name)
        {
            std::shared_lock lock(ValuesMutex);
            auto item_iterator = Values.find(name);
            lock.unlock();

            if (item_iterator != Values.end() &&
                item_iterator->second.has_value() && item_iterator->second.type() == typeid(ValueType))
            {
                /// Get the pointer of the value inside std::any container.
                return std::any_cast<ValueType>(&item_iterator->second);
            }
            return nullptr;
        }

        /**
         * @brief Get a pointer to the value with the given name, or set it if it does not exist.
         * @tparam ValueType The type of the value to get,
         *                   it should be totally the same to the original type of the value
         *                   when it is firstly set in this blackboard.
         * @param name The name of the value to get.
         * @param default_value The value to set if it does not exist.
         * @return The pointer to the value with the given name.
         */
        template <typename ValueType>
        ValueType* AcquireReference(const std::string& name, ValueType default_value)
        {
            std::unique_lock lock(ValuesMutex);
            auto item_iterator = Values.find(name);

            if (item_iterator == Values.end())
            {
                item_iterator = std::get<0>(
                        Values.emplace(name, std::make_any<ValueType>(default_value)));
            }
            if (!item_iterator->second.has_value() || item_iterator->second.type() != typeid(ValueType))
            {
                item_iterator->second = std::make_any<ValueType>(default_value);
            }

            /// Get the pointer of the value inside std::any container.
            return std::any_cast<ValueType>(&item_iterator->second);
        }
    };
}