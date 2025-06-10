#include "roam.h"
#include <unistd.h>
#include <fmt/core.h>

#include <iostream>
#include <vector>

#include "roam_cli.h"
#include "roam_config.h"
#include "roam_theme.h"
#include "roam_tools.h"

#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#include <mach-o/dyld.h>
#include <limits.h>
#endif

using namespace std;
using namespace Tools;

namespace Global
{
    const vector<array<string, 2>> Banner_src = {
        {"#E62525", "██████╗  ██████╗  █████╗ ███╗   ███╗"},
        {"#CD2121", "██╔══██╗██╔═══██╗██╔══██╗████╗ ████║"},
        {"#B31D1D", "██████╔╝██║   ██║███████║██╔████╔██║"},
        {"#9A1919", "██╔══██╗██║   ██║██╔══██║██║╚██╔╝██║"},
        {"#801414", "██║  ██║╚██████╔╝██║  ██║██║ ╚═╝ ██║"},
        {"#801414", "╚═╝  ╚═╝ ╚═════╝ ╚═╝  ╚═╝╚═╝     ╚═╝"},
    };

    const string Version = "0.0.1";
    string exit_error_msg;
    atomic<bool> quitting(false);
    uid_t real_uid, set_uid;
    uint64_t start_time;

    fs::path self_path;
    bool debug{};

    atomic<bool> init_conf(false);

} // namespace Global

string ansi_color_code(const string &hexColor)
{
    if (hexColor == "#E62525")
        return "\033[38;2;230;37;37m";
    if (hexColor == "#CD2121")
        return "\033[38;2;205;33;33m";
    if (hexColor == "#B31D1D")
        return "\033[38;2;179;29;29m";
    if (hexColor == "#9A1919")
        return "\033[38;2;154;25;25m";
    if (hexColor == "#801414")
        return "\033[38;2;128;20;20m";
    if (hexColor == "#000000")
        return "\033[0m"; // 重置颜色
    return "\033[0m";
}

void display()
{
    for (const auto &item : Global::Banner_src)
    {
        string colorCode = ansi_color_code(item[0]);
        cout << colorCode << item[1] << "\033[0m" << endl;
    }
}

void clean_quit(const int code)
{
    if (Global::quitting)
        return;
    Global::quitting = true;
    cout << Global::exit_error_msg << endl;
    exit(code);
}

void init_config(bool low_color)
{
    atomic_lock lck(Global::init_conf);
    vector<string> load_warinings;
    Config::load(Config::conf_file, load_warinings);
    Config::set("lowcolor",(low_color? true: !Config::getB("truecolor")));
    
    static bool first_init = true;
    if (Global::debug and first_init) {
        Logger::set("DEBUG");
        Logger::debug("Running in DEBUG mode!");        
    } else Logger::set(Config::getS("log_level"));
    static string log_level;
    if(const string current_level = Config::getS("log_level"); log_level != current_level){
        log_level = current_level;
        Logger::info("Logger set to " + (Global::debug? "DEBUG" : log_level));        
    }
    for (const auto& err_str :load_warinings) Logger::warning(err_str);
    first_init = false;
}

int main(const int argc, const char **argv)
{
    display();
    Global::start_time = time_s();
    Global::real_uid = getuid();
    Global::set_uid = geteuid();
    if (Global::real_uid != Global::set_uid)
    {
        if (seteuid(Global::real_uid) != 0)
        {
            Global::real_uid = Global::set_uid;
            Global::exit_error_msg = "Failed to change effective user ID. Unset btop SUID bit to ensure security on this system. Quitting!";
            clean_quit(1);
        }
    }
    Cli::Cli cli;
    {
        const std::vector<std::string_view> args{
            std::next(argv, std::ptrdiff_t{1}),
            std::next(argv, static_cast<std::ptrdiff_t>(argc))};
        auto cli_or_ret = Cli::parse(args);
        if (std::holds_alternative<Cli::Cli>(cli_or_ret))
        {
            cli = std::get<Cli::Cli>(cli_or_ret);
        }
        else
        {
            auto ret = std::get<std::int32_t>(cli_or_ret);
            if (ret != 0)
            {
                Cli::usage();
                Cli::help_hint();
            }
            return ret;
        }
    }

    Global::debug = cli.debug;
    {
        const auto config_dir = Config::get_config_dir();
        if (config_dir.has_value())
        {
            Config::conf_dir = config_dir.value();
            if (cli.config_file.has_value())
            {
                Config::conf_file = cli.config_file.value();
            }
            else
            {
                Config::conf_file = Config::conf_dir / "roam.conf";
            }
            Logger::logfile = Config::get_log_file();
            cout << "logile" << Logger::logfile.value() << endl;
            Theme::user_theme_dir = Config::conf_dir / "themes";
            cout << "theme dir" << Theme::user_theme_dir << endl;
            std::error_code error;
            if (!fs::exists(Theme::user_theme_dir, error) and !fs::create_directories(Theme::user_theme_dir, error))
            {
                Theme::user_theme_dir.clear();
                Logger::warning("Failed to create user theme directory:" + error.message());
            }
        }
    }
#ifdef __linux__
    {
        std::error_code ec;
        Global::self_path = fs::read_symlink("/proc/self/exe", ec).remove_filename();
    }
#elif __APPLE__
    {
        char buf[PATH_MAX];
        uint32_t bufsize = PATH_MAX;
        if (!_NSGetExecutablePath(buf, &bufsize))
            Global::self_path = fs::path(buf).remove_filename();
    }
#endif
    if (std::error_code ec; !Global::self_path.empty())
    {
        Theme::theme_dir = fs::canonical(Global::self_path / "../share/roam/themes", ec);
        cout << "theme_dir" << Theme::theme_dir << endl;
        if (ec or !fs::is_directory(Theme::theme_dir) or access(Theme::theme_dir.c_str(), R_OK))
        {
            Theme::theme_dir.clear();
        }
    }
    if (Theme::theme_dir.empty())
    {
        for (auto theme_path : {"/usr/local/share/roam/themes", "/usr/share/roam/themes"})
        {
            if (fs::is_directory(fs::path(theme_path)) and access(theme_path, R_OK) != -1)
            {
                Theme::theme_dir = fs::path(theme_path);
                break;
            }
        }
    }
    // init config
    cout << "cli.low_color:" << cli.low_color << endl;
    init_config(cli.low_color);

    // utf-8 skip for temporal

    if (!Term::init())
    {
        Global::exit_error_msg = "No tty detected!\nRoam needs an interactive terminal to run.";
        cout << Global::exit_error_msg <<endl;
        clean_quit(1);
    }
    if (Term::current_tty != "unknown") Logger::info("Running on " + Term::current_tty);
    cout << "Term size " << Term::width << " x " << Term::height << endl ;
    // {
	// 	int t_count = 0;
	// 	while (Term::width <= 0 or Term::width > 10000 or Term::height <= 0 or Term::height > 10000) {
	// 		sleep_ms(10);
	// 		Term::refresh();
	// 		if (++t_count == 100) {
	// 			Global::exit_error_msg = "Failed to get size of terminal!";
	// 			clean_quit(1);
	// 		}
	// 	}
	// }

    return 0;
}
