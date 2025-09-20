#pragma once

#include <string_view>
#include "REL/Version.h"

namespace Plugin
{
    inline constexpr std::string_view NAME = "PrismaUI-Example-Plugin";
    inline constexpr REL::Version VERSION{ 1, 0, 0, 0 };
}
