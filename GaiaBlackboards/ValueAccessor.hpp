#pragma once

#include "AccessorBase.hpp"

namespace Gaia::Blackboards
{
    /**
     * @brief Accessor can store the iterator of the bound item, thus can boost the query process.
     * @tparam ValueType The type of the bound item.
     */
    template <typename ValueType>
    class ValueAccessor : public AccessorBase<ValueType>
    {
    public:
        /**
         * @brief Get the value.
         * @param default_value Use to initialize the value item if it does not exist.
         * @return Optional value of the connected item,
         *         or std::nullopt if this accessor is not connected or
         *         the type of the value has changed.
         */
        std::optional<ValueType> Get()
        {
            if (this->IsConnected() &&
                this->GetItemIterator()->second.has_value() &&
                this->GetItemIterator()->second.type() == typeid(ValueType))
            {
                return std::optional{std::any_cast<ValueType>(this->GetItemIterator()->second)};
            }

            return std::nullopt;
        }

        /**
         * @brief Set the value.
         * @param value The value to update.
         * @details This function will do noting if this accessor is not connected.
         */
        void Set(ValueType value)
        {
            if (!this->IsConnected()) return;
            this->GetItemIterator()->second.template emplace<ValueType>(value);
        }
    };
}