// Copyright (C) 2005 - 2024 Settlers Freaks (sf-team at siedler25.org)
//
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <memory>
#include <cstdio>

namespace s25util{
    namespace detail{
    struct file_closer
    {
        void operator()(std::FILE* fp) { std::fclose(fp); }
    };
    }
    class file_handle: std::unique_ptr<std::FILE, detail::file_closer>{
        using base = std::unique_ptr<std::FILE, detail::file_closer>;

    public:
        using base::base;
        using base::operator=;
        using base::operator bool;
        using base::reset;

        /// Access the file pointer
        FILE* operator*() const noexcept { return get(); }
    };
}
