#include "Blackboard.hpp"

namespace Gaia::Blackboards
{
    /// Clear all values in this blackboard.
    void Blackboard::Clear()
    {
        std::unique_lock lock(ValuesMutex);
        Values.clear();
    }

    /// Remove the value with the given name.
    void Blackboard::RemoveValue(const std::string &name)
    {
        std::unique_lock lock(ValuesMutex);
        auto item_iterator = Values.find(name);
        if (item_iterator != Values.end())
        {
            Values.erase(item_iterator);
        }
    }
}