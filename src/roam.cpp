#include "roam.h"
#include <unistd.h>

#include <iostream>
#include <vector>

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

    if (!Term::init())
    {
        Global::exit_error_msg = "No tty detected!\nRoam needs an interactive terminal to run.";
        clean_quit(1);
    }

    return 0;
}
