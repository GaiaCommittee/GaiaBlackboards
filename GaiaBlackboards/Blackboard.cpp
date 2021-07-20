#include "Blackboard.hpp"

namespace Gaia::Blackboards
{
    /// Clear all values in this blackboard.
    void Blackboard::Clear()
    {
        std::unique_lock lock(ItemsMutex);
        Items.clear();
    }

    /// Remove the value with the given name.
    void Blackboard::Remove(const std::string &name)
    {
        std::unique_lock lock(ItemsMutex);
        auto item_iterator = Items.find(name);
        if (item_iterator != Items.end())
        {
            Items.erase(item_iterator);
        }
    }
}