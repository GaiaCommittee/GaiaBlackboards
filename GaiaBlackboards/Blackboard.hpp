#pragma once

#include <unordered_map>
#include <any>
#include <string>
#include <shared_mutex>
#include <type_traits>

namespace Gaia::Blackboards
{
    /**
     * @brief Blackboard is a kind of key-value container which supports generic value types.
     */
    class Blackboard
    {
        /// Friend class accessor for boosting query process.
        template <typename ValueType>
        friend class AccessorBase;
    protected:
        /// Mutex for Items.
        std::shared_mutex ItemsMutex;
        /// Map for key-values storage.
        std::unordered_map<std::string, std::any> Items;

    public:
        /// Clear all values in this blackboard.
        void Clear();

        /// Remove the value with the given name.
        void Remove(const std::string& name);

        /**
         * @brief Set the value for the value item.
         * @tparam ValueType Type of the value.
         * @param name Name of the value item.
         * @param value Value to set for the value item.
         * @retval true Value node has been set or updated
         * @retval false Value node with the same name already exists, and its type does not match the given type.
         */
        template <typename ValueType>
        bool SetValue(const std::string& name, ValueType value)
        {
            std::unique_lock lock(ItemsMutex);
            auto item_iterator = Items.find(name);

            if (item_iterator == Items.end())
            {
                Items.emplace(name, std::make_any<ValueType>(value));
                return true;

            } if (item_iterator->second.type() == typeid(ValueType))
            {
                item_iterator->second.emplace<ValueType>(value);
                return true;
            }

            return false;
        }

        /**
         * @brief Get a value from this blackboard.
         * @tparam ValueType The type of the value to get,
         *                   it should be totally the same to the original type of the value
         *                   when it is firstly set in this blackboard.
         * @param name The name of the value to get.
         * @return Optional value with the given name,
         *         set and returns default value if it does not exists,
         *         returns null if it does not fully match the demanded type.
         */
        template <typename ValueType>
        std::optional<ValueType> GetValue(const std::string& name,
                                          std::optional<ValueType> default_value = std::nullopt)
        {
            std::shared_lock lock(ItemsMutex);
            auto item_iterator = Items.find(name);
            lock.unlock();

            if (item_iterator == Items.end() && default_value.has_value())
            {
                std::unique_lock lock_write(ItemsMutex);
                Items.emplace(name, std::make_any<ValueType>(*default_value));
                lock_write.unlock();
                return default_value;
            }

            if (item_iterator != Items.end() &&
                item_iterator->second.has_value() && item_iterator->second.type() == typeid(ValueType))
            {
                /* Due to the manually type check, this function call should not throw std::bad_any_cast
                   because of not matched types. */
                return std::any_cast<ValueType>(item_iterator->second);
            }

            return std::nullopt;
        }

        /**
         * @brief Set the object instance for the object item via move construction.
         * @tparam ValueType Type of the object.
         * @param name Name of the object item.
         * @param object_instance Instance to set for the value item.
         * @retval true Object node has been set or updated
         * @retval false Object node with the same name already exists, and its type does not match the given type.
         */
        template <typename ObjectType,
                typename = typename std::enable_if_t<std::is_move_constructible_v<ObjectType>>>
        bool SetObject(const std::string& name, ObjectType&& object_instance)
        {
            std::unique_lock lock(ItemsMutex);
            auto item_iterator = Items.find(name);

            if (item_iterator == Items.end())
            {
                Items.emplace(name, std::make_any<ObjectType>(std::move(object_instance)));
                return true;

            } if (item_iterator->second.type() == typeid(ObjectType))
            {
                item_iterator->second.emplace<ObjectType>(std::move(object_instance));
                return true;
            }

            return false;
        }

        /**
         * @brief Set the object instance for the object item via copy construction.
         * @tparam ObjectType Type of the object.
         * @param name Name of the object item.
         * @param object_instance Instance to set for the value item.
         * @retval true Object node has been set or updated
         * @retval false Object node with the same name already exists, and its type does not match the given type.
         */
        template <typename ObjectType,
                typename = typename std::enable_if_t<std::is_copy_constructible_v<ObjectType>>>
        bool SetObject(const std::string& name, const ObjectType& object_instance)
        {
            std::unique_lock lock(ItemsMutex);
            auto item_iterator = Items.find(name);

            if (item_iterator == Items.end())
            {
                Items.emplace(name, std::make_any<ObjectType>(object_instance));
                return true;

            } if (item_iterator->second.type() == typeid(ObjectType))
            {
                item_iterator->second.emplace<ObjectType>(object_instance);
                return true;
            }

            return false;
        }

        /**
         * @brief Get a pointer to the value with the given name.
         * @tparam ObjectType The type of the value to get,
         *                   it should be totally the same to the original type of the value
         *                   when it is firstly set in this blackboard.
         * @param name The name of the value to get.
         * @return The pointer to the value with the given name,
         *         nullptr if value with the demanded name does not exist,
         *         or its type can not fully match the demanded type.
         */
        template <typename ObjectType>
        ObjectType* GetObject(const std::string& name, std::optional<ObjectType> default_object = std::nullopt)
        {
            std::shared_lock lock(ItemsMutex);
            auto item_iterator = Items.find(name);
            lock.unlock();

            if (item_iterator == Items.end() && default_object.has_value())
            {
                std::unique_lock lock_write(ItemsMutex);
                item_iterator = std::get<0>(
                        Items.emplace(name, std::make_any<ObjectType>(*default_object)));
                lock_write.unlock();
                return std::any_cast<ObjectType>(&item_iterator->second);
            }

            if (item_iterator != Items.end() &&
                item_iterator->second.has_value() && item_iterator->second.type() == typeid(ObjectType))
            {
                /// Get the pointer of the value inside std::any container.
                return std::any_cast<ObjectType>(&item_iterator->second);
            }
            return nullptr;
        }
    };
}