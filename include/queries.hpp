#pragma once

#include <string>
#include <vector>
#include "context.hpp"

namespace queries {
    void queryPrefix(DataContext& ctx, const std::string& prefix);
    void queryUser(DataContext& ctx, int userId);
    void queryTop(DataContext& ctx, int n, const std::string& genre);
    void queryTags(DataContext& ctx, const std::vector<std::string>& tags);
}
