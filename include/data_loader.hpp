#pragma once
#include <string>
#include "context.hpp"

namespace data_loader {
    void loadMovies(const std::string& path, DataContext& ctx);
    void loadRatings(const std::string& path, DataContext& ctx);
    void loadTags(const std::string& path, DataContext& ctx);
}
