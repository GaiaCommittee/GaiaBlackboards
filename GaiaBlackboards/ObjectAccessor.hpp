#pragma once

#include "AccessorBase.hpp"

namespace Gaia::Blackboards
{
    /**
     * @brief Accessor can store the iterator of the bound item, thus can boost the query process.
     * @tparam ValueType The type of the bound item.
     */
    template <typename ObjectType>
    class ObjectAccessor : public AccessorBase<ObjectType>
    {
    public:
        /**
         * @brief Get the pointer to the object.
         * @param default_value Use to initialize the value item if it does not exist.
         * @return Optional value of the connected item,
         *         or std::nullopt if this accessor is not connected or
         *         the type of the value has changed.
         */
        ObjectType* Get()
        {
            if (this->IsConnected() &&
                this->GetItemIterator()->second.has_value() &&
                this->GetItemIterator()->ItemIterator->second.type() == typeid(ObjectType))
            {
                return std::any_cast<ObjectType>(&this->ItemIterator->second);
            }

            return nullptr;
        }

        /**
         * @brief Set the object instance.
         * @param value The object instance to update.
         * @details This function will do noting if this accessor is not connected.
         */
        template <typename = typename std::enable_if_t<std::is_move_constructible_v<ObjectType>>>
        void Set(ObjectType&& object_instance)
        {
            if (!this->IsConnected()) return;
            this->GetItemIterator()->second.template emplace<ObjectType>(std::move(object_instance));
        }

        template <typename = typename std::enable_if_t<std::is_copy_constructible_v<ObjectType>>>
        void Set(const ObjectType& object_instance)
        {
            if (!this->IsConnected()) return;
            this->GetItemIterator()->second.template emplace<ObjectType>(object_instance);
        }
    };
}