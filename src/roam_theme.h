#pragma once

#include<filesystem>
#include<vector>
#include<string>

using std::array;
using std::vector;
using std::string;

namespace Theme{
    extern std::filesystem::path theme_dir;
    extern std::filesystem::path user_theme_dir;
    extern vector<string> themes;
}