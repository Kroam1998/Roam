#pragma once

#include <atomic>
#include <regex>
#include <string>
#include <ranges>
#include <filesystem>

using namespace std;
namespace fs = std::filesystem;

namespace Fx
{
    const string e = "\x1b[";     //* Escape sequence start
    const string b = e + "1m";    //* Bold on/off
    const string ub = e + "22m";  //* Bold off
    const string d = e + "2m";    //* Dark on
    const string ud = e + "22m";  //* Dark off
    const string i = e + "3m";    //* Italic on
    const string ui = e + "23m";  //* Italic off
    const string ul = e + "4m";   //* Underline on
    const string uul = e + "24m"; //* Underline off
    const string bl = e + "5m";   //* Blink on
    const string ubl = e + "25m"; //* Blink off
    const string s = e + "9m";    //* Strike/crossed-out on
    const string us = e + "29m";  //* Strike/crossed-out on/off

    const string reset_base = e + "0m";
    extern string reset;
} // namespace Fx

namespace Term
{
    extern atomic<bool> initialized;
    extern atomic<int> width;
    extern atomic<int> height;
    extern string fg, bg, current_tty;
    bool init();

} // namespace Term

namespace Tools
{
    constexpr auto SSmax = std::numeric_limits<std::streamsize>::max();
    //* Left-trim <t_str> from <str> and return new string
	string_view ltrim(string_view str, string_view t_str = " ");

	//* Right-trim <t_str> from <str> and return new string
	string_view rtrim(string_view str, string_view t_str = " ");

	//* Left/right-trim <t_str> from <str> and return new string
	inline string_view trim(string_view str, string_view t_str = " ") {
		return ltrim(rtrim(str, t_str), t_str);
	}

    //* find index of <find_val> in <vec> return size_of(vec) if not found
    template <typename T>
    inline size_t v_index(const vector<T> &vec, const T &find_val)
    {
        return std::ranges::distance(vec.begin(), std::ranges::find(vec, find_val));
    }
    inline uint64_t time_s(){ //返回一个表示当前时刻距离 Unix 纪元（通常指 1970 年 1 月 1 日午夜）过去了多少秒的整数
        return std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }
    //* Compare <first> with all following values
	template<typename First, typename ... T>
	inline bool is_in(const First& first, const T& ... t) {
		return ((first == t) or ...);
	}

    //* Check if a string is a valid bool value
	inline bool isbool(const std::string_view str) {
		return is_in(str, "true", "false", "True", "False");
	}
    //* Convert string to bool, returning any value not equal to "true" or "True" as false
	inline bool stobool(const std::string_view str) {
		return is_in(str, "true", "True");
	}
    //* Check if a string is a valid integer value (only positive)
	constexpr bool isint(const std::string_view str) {
		return std::ranges::all_of(str, ::isdigit);
	}

    //* Check if vector <vec> contains value <find_val>
    template <typename T, typename T2>
    inline bool v_contains(const vector<T>& vec, const T2& find_val) {
		return std::ranges::find(vec, find_val) != vec.end();
	}
    template <typename T>
    constexpr bool s_contains(const std::string_view str, const T& find_val) {
		return str.find(find_val) != string::npos;
	}
    static inline void busy_wait (void) {
	#if defined __i386__ || defined __x86_64__
		__builtin_ia32_pause();
	#elif defined __ia64__
		__asm volatile("hint @pause" : : : "memory");
	#elif defined __sparc__ && (defined __arch64__ || defined __sparc_v9__)
		__asm volatile("membar #LoadLoad" : : : "memory");
	#else
		__asm volatile("" : : : "memory");
	#endif
	}
    void atomic_wait(const atomic<bool>& atom, bool old = true) noexcept;

    //* set atomic<bool> to true on construct,set to false on destruct
    class atomic_lock
    {
        atomic<bool> &atom;
        bool not_true{};

    public:
        explicit atomic_lock(atomic<bool> &atom, bool wait = false);
        ~atomic_lock() noexcept;
        atomic_lock(const atomic_lock &other) = delete;
        atomic_lock &operator=(const atomic_lock &other) = delete;
        atomic_lock(atomic_lock &&other) = delete;
        atomic_lock &operator=(atomic_lock &&other) = delete;
    };

}

// simple implementation of logger
namespace Logger
{
    const vector<string> log_levels = {"DEBUG", "INFO", "WARNING", "ERROR", "DISABLED"};
    extern std::optional<std::filesystem::path> logfile;
    enum Level : std::uint8_t
    {
        DEBUG = 0,
        INFO = 1,
        WARNING = 2,
        ERROR = 3,
        DISABLED = 4
    };
    // set log level
    void set(const string &level);
    void log_write(const Level level, const std::string_view msg);
    inline void error(const string_view &msg) { log_write(ERROR, msg); }
    inline void info(const string_view &msg) { log_write(INFO, msg); }
    inline void warning(const string_view &msg) { log_write(WARNING, msg); }
    inline void debug(const string_view &msg) { log_write(DEBUG, msg); }

}
