// Copyright (c) 2005 - 2017 Settlers Freaks (sf-team at siedler25.org)
//
// This file is part of Return To The Roots.
//
// Return To The Roots is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Return To The Roots is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Return To The Roots. If not, see <http://www.gnu.org/licenses/>.

#include "SingletonPolicies.h"
#include "Singleton.h"
#include <algorithm>
#include <set>

namespace SingletonPolicies {

/// Manages the lifetime of all Singletons with a longevity. Class (only) used by SetLongevity
class LifetimeTracker : public Singleton<LifetimeTracker, SingletonPolicies::DefaultLifetime>
{
    /// Container for storing the data required for destroying an item (in this case calling the destroyer function)
    struct LifetimeTrackerItem
    {
        const unsigned longevity_;
        DestructionFunPtr destFunc_;

        LifetimeTrackerItem(unsigned longevity, DestructionFunPtr destFunc) : longevity_(longevity), destFunc_(destFunc)
        {}

        friend bool operator<(const LifetimeTrackerItem& lhs, const LifetimeTrackerItem& rhs)
        {
            return lhs.longevity_ < rhs.longevity_;
        }
    };

    // Queue that sorts items in ascending order (front() points to smallest element)
    std::multiset<LifetimeTrackerItem> items_;

public:
    ~LifetimeTracker()
    {
        // Destroy all items
        for(const auto& item : items_)
            item.destFunc_();
    }

    void add(unsigned longevity, DestructionFunPtr destFunc)
    {
        // Remove same entries first. Calling a dtor twice is not supported!
        const auto it =
          std::find_if(items_.begin(), items_.end(), [destFunc](const auto& x) { return x.destFunc_ == destFunc; });
        if(it != items_.end())
            items_.erase(it);
        items_.insert(LifetimeTrackerItem(longevity, destFunc));
    }
};

void SetLongevity(unsigned longevity, DestructionFunPtr pFun)
{
    LifetimeTracker::inst().add(longevity, pFun);
}
} // namespace SingletonPolicies
