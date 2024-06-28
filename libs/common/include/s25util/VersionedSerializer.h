// Copyright (C) 2005 - 2024 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "Serializer.h"
#include <functional>
#include <limits>
#include <optional>

namespace s25util {

/// Add a version to the serializer class which can be used to deserialize data after format changes.
/// Only backwards compatibility for loading is required, i.e. writing always assumes the latest version.
///
/// This class needs to be templated with a class providing the static function `getCurrentVersion`.
/// Any changes should be documented at that place,
/// and removed once the file version changes at which point the current version can be reset to zero.
template<class SubClass>
class VersionedSerializer : public Serializer
{
public:
    /// Create a new serializer with the given data version defaulting to the most recent one
    explicit VersionedSerializer(unsigned dataVersion = SubClass::getCurrentVersion()) : dataVersion_(dataVersion) {}
    /// Create a new serializer for reading using the same storage as the base serializer.
    /// When this serializer is destroyed the read position of the base is advanced by the read data.
    /// This allows to use a versioned serializer for some part of the deserialization
    explicit VersionedSerializer(Serializer& base, unsigned dataVersion = SubClass::getCurrentVersion())
        : Serializer(base.GetData() + base.GetPos(), base.GetBytesLeft()), dataVersion_(dataVersion), base_(base)
    {}
    VersionedSerializer(const VersionedSerializer&) = delete;
    VersionedSerializer(VersionedSerializer&&) = default;
    VersionedSerializer& operator=(VersionedSerializer&&) = default;

    ~VersionedSerializer()
    {
        if(base_)
            base_->get().PopAndDiscard(GetPos());
    }

    unsigned getDataVersion() const { return dataVersion_; }

private:
    unsigned dataVersion_;
    std::optional<std::reference_wrapper<Serializer>> base_;
};

} // namespace s25util
