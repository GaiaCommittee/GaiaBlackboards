#pragma once
#include "Blackboard.hpp"

namespace Gaia::Blackboards
{
    /**
     * @brief Accessor can store the iterator of the bound item, thus can boost the query process.
     * @tparam ValueType The type of the bound item.
     */
    template <typename ValueType>
    class AccessorBase
    {
    private:
        /// Whether this Accessor is connected to a blackboard item or not.
        bool Connected {false};
        /// Iterator to the connected blackboard item.
        std::unordered_map<std::string, std::any>::iterator ItemIterator;

    protected:
        /**
         * @brief Get the iterator to the bound item.
         * @return Iterator to the bound item.
         */
        [[nodiscard]] inline const decltype(ItemIterator)& GetItemIterator() const noexcept
        {
            return ItemIterator;
        }

    public:
        /**
         * @brief Connect this accessor to a blackboard.
         * @param blackboard The blackboard to connect.
         * @param name The name of the value to connect in the given blackboard.
         * @param default_value The value to set in the given blackboard
         *                      if the item with the demanded name does not exist.
         */
        void Connect(Blackboard &blackboard, const std::string &name,
                     std::optional<ValueType> default_value = std::nullopt)
        {
            std::unique_lock lock(blackboard.ItemsMutex);
            auto item_iterator = blackboard.Items.find(name);
            if (item_iterator != blackboard.Items.end() &&
                item_iterator->second.has_value() && item_iterator->second.type() != typeid(ValueType))
            {
                return;
            }
            if (item_iterator == blackboard.Items.end() && !default_value.has_value())
            {
                return;
            }
            if (item_iterator == blackboard.Items.end() || !item_iterator->second.has_value())
            {
                item_iterator = std::get<0>(blackboard.Items.emplace(name, std::make_any<ValueType>(*default_value)));
            }
            else if (item_iterator->second.has_value() && item_iterator->second.type() != typeid(ValueType))
            {
                return;
            }
            ItemIterator = item_iterator;
            Connected = true;
        }

        /// Disconnect from the connected item in the blackboard.
        void Disconnect()
        {
            Connected = false;
        }

        /// Check whether this accessor has connected to the blackboard or not.
        [[nodiscard]] inline bool IsConnected() const noexcept
        {
            return Connected;
        }
    };
}