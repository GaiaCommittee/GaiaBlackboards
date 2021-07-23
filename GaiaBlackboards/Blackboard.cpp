#include "Blackboard.hpp"

namespace Gaia::Blackboards
{
    /// Clear all values in this blackboard.
    void Blackboard::ClearItems()
    {
        std::unique_lock lock(ItemsMutex);
        Items.clear();
    }

    /// Remove the value with the given name.
    void Blackboard::RemoveItem(const std::string &name)
    {
        std::unique_lock lock(ItemsMutex);
        auto item_iterator = Items.find(name);
        if (item_iterator != Items.end())
        {
            Items.erase(item_iterator);
        }
    }

    /// Check whether item with the given name exist or not.
    bool Blackboard::HasItem(const std::string &name)
    {
        std::shared_lock lock(ItemsMutex);
        return Items.find(name) != Items.end();
    }
}