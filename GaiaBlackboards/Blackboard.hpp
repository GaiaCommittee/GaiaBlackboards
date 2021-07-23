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
        void ClearItems();

        /// Remove the value with the given name.
        void RemoveItem(const std::string& name);

        /// Check whether item with the given name exist or not.
        bool HasItem(const std::string& name);

        /**
         * @brief Get or create a variable on the blackboard.
         * @tparam VariableType Type of the variable.
         * @tparam ConstructorArguments Type of arguments for the variable constructor.
         * @param name Name of the variable.
         * @param arguments Arguments to pass to the constructor of the variable.
         * @return Pointer to the variable.
         */
        template <typename VariableType, typename... ConstructorArguments>
        VariableType* GetVariable(const std::string& name, ConstructorArguments... arguments)
        {
            std::shared_lock lock_searching(ItemsMutex);
            auto item_finder = Items.find(name);
            lock_searching.unlock();

            if (item_finder == Items.end())
            {
                std::unique_lock lock_editing(ItemsMutex);
                item_finder = std::get<0>(Items.template emplace(name, std::make_any<VariableType>(arguments...)));
                lock_editing.unlock();
            }
            try{
                return std::any_cast<VariableType>(&item_finder->second);
            } catch (std::bad_any_cast&){
                return nullptr;
            }
        }

        /**
         * @brief Get or create an object on the blackboard.
         * @tparam ObjectType Type of the object.
         * @tparam ConstructorArguments Type of arguments for the object constructor.
         * @param name Name of the variable.
         * @param arguments Arguments to pass to the constructor of the variable.
         * @return Pointer to the object.
         */
        template <typename ObjectType, typename... ConstructorArguments>
        ObjectType* GetObject(const std::string& name, ConstructorArguments... arguments)
        {
            std::shared_lock lock_searching(ItemsMutex);
            auto item_finder = Items.find(name);
            lock_searching.unlock();

            if (item_finder == Items.end())
            {
                std::unique_lock lock_editing(ItemsMutex);
                item_finder = std::get<0>(Items.template emplace(name,
                        std::make_shared<ObjectType>(arguments...)));
                lock_editing.unlock();
            }
            try{
                return std::any_cast<std::shared_ptr<ObjectType>>(&item_finder->second)->get();
            } catch (std::bad_any_cast&){
                return nullptr;
            }
        }
    };
}