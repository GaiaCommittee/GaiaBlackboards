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
        /// Shared auto pointer type for item access.
        template <typename VariableType>
        using Pointer = std::shared_ptr<VariableType>;

        /// Clear all values in this blackboard.
        void ClearItems();

        /// Remove the value with the given name.
        void RemoveItem(const std::string& name);

        /// Check whether item with the given name exist or not.
        bool HasItem(const std::string& name);

        /**
         * @brief Get the item as the shared auto pointer of the given type.
         * @tparam VariableType Type of the variable insides shared pointer.
         * @tparam ConstructorArguments Type of arguments for variable constructor.
         * @param name Name of the item to get.
         * @param arguments Arguments for variable constructor.
         * @return Shared auto pointer to the desired item, or nullptr if item exists but
         *         mismatches in variable type.
         */
        template <typename VariableType, typename... ConstructorArguments>
        Pointer<VariableType> GetItem(const std::string& name, ConstructorArguments... arguments)
        {
            std::shared_lock lock_searching(ItemsMutex);
            auto item_finder = Items.find(name);
            lock_searching.unlock();

            if (item_finder == Items.end())
            {
                std::unique_lock lock_editing(ItemsMutex);
                item_finder = std::get<0>(Items.template emplace(name, std::any()));
                item_finder->second = std::make_shared<VariableType>(arguments...);
                lock_editing.unlock();
            }
            try{
                return std::any_cast<Pointer<VariableType>>(item_finder->second);
            } catch (std::bad_any_cast&){
                return {nullptr};
            }
        }

        /**
         * @brief Get the item as the raw pointer of the given type.
         * @tparam VariableType Type of the variable insides shared pointer.
         * @tparam ConstructorArguments Type of arguments for variable constructor.
         * @param name Name of the item to get.
         * @param arguments Arguments for variable constructor.
         * @return Raw pointer to the desired item, or nullptr if item exists but
         *         mismatches in variable type.
         */
        template <typename VariableType, typename... ConstructorArguments>
        VariableType* GetPointer(const std::string& name, ConstructorArguments... arguments)
        {
            return GetItem<VariableType>(name, arguments...).get();
        }
    };
}