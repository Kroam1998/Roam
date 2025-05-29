#pragma once
#include <array>
#include <atomic>
#include <deque>
#include <filesystem>
#include <string>

using std::string;

namespace Global {
    extern const string Version;
    extern uid_t real_uid, set_uid;

}