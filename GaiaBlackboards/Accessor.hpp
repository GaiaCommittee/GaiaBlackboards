#pragma once

#include "Blackboard.hpp"

namespace Gaia::Blackboards
{
    /**
     * @brief Accessor can store the iterator of the bound item, thus can boost the query process.
     * @tparam ValueType The type of the bound item.
     */
    template <typename ValueType>
    class Accessor
    {
    protected:
        /// Whether this Accessor is connected to a blackboard item or not.
        bool IsConnected {false};
        /// Iterator to the connected blackboard item.
        std::unordered_map<std::string, std::any>::iterator ItemIterator;

    public:
        /**
         * @brief Connect this accessor to a blackboard.
         * @param blackboard The blackboard to connect.
         * @param name The name of the value to connect in the given blackboard.
         * @param default_value The value to set in the given blackboard
         *                      if the item with the demanded name does not exist.
         */
        void Connect(Blackboard& blackboard, const std::string& name, ValueType default_value = ValueType())
        {
            std::unique_lock lock(blackboard.ValuesMutex);
            auto item_iterator = blackboard.Values.find(name);
            if (item_iterator == blackboard.Values.end())
            {
                item_iterator = std::get<0>(
                        blackboard.Values.emplace(name, std::make_any<ValueType>(default_value)));
            }
            else if (!item_iterator->second.has_value() && item_iterator->second.type() != typeid(ValueType))
            {
                item_iterator->second = std::make_any<ValueType>(default_value);
            }

            ItemIterator = item_iterator;
            IsConnected = true;
        }

        /// Disconnect from the connected item in the blackboard.
        void Disconnect()
        {
            IsConnected = false;
        }

        /**
         * @brief Get the value.
         * @return Optional value of the connected item,
         *         or std::nullopt if this accessor is not connected or
         *         the type of the value has changed.
         */
        std::optional<ValueType> Get()
        {
            if (IsConnected &&
                ItemIterator->second.has_value() && ItemIterator->second.type() == typeid(ValueType))
            {
                return std::optional{std::any_cast<ValueType>(ItemIterator->second)};
            }

            return std::nullopt;
        }

        /**
         * @brief Get the value.
         * @param default_value The value to return if this accessor is not connected
         *                      or the type of the value has changed.
         * @return Value of the connected item,
         *         or default_value if this accessor is not connected or
         *         the type of the value has changed.
         */
        ValueType Get(ValueType default_value)
        {
            if (IsConnected &&
                ItemIterator->second.has_value() && ItemIterator->second.type() == typeid(ValueType))
            {
                return std::any_cast<ValueType>(ItemIterator->second);
            }

            return default_value;
        }

        /**
         * @brief Set the value.
         * @param value The value to update.
         * @details This function will do noting if this accessor is not connected.
         */
        void Set(ValueType value)
        {
            if (IsConnected)
            {
                ItemIterator->second = std::make_any<ValueType>(value);
            }
        }
    };
}