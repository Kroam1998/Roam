#include <filesystem>
#include <optional>
#include <fstream>
#include <atomic>
#include <array>
#include <iostream>

#include <fmt/core.h>
#include <sys/statvfs.h>

#include "roam_config.h"
#include "roam_shared.h"
#include "roam_tools.h"

using std::atomic;
using std::array;
namespace fs = std::filesystem;
namespace rng = std::ranges;
using namespace std;
using namespace Tools;

namespace Config
{
    atomic<bool> locked (false);
    atomic<bool> writelock (false);
    bool write_new;
    const vector<array<string, 2>> descriptions = {
		{"color_theme", 		"#* Name of a btop++/bpytop/bashtop formatted \".theme\" file, \"Default\" and \"TTY\" for builtin themes.\n"
								"#* Themes should be placed in \"../share/btop/themes\" relative to binary or \"$HOME/.config/btop/themes\""},

		{"theme_background", 	"#* If the theme set background should be shown, set to False if you want terminal background transparency."},

		{"truecolor", 			"#* Sets if 24-bit truecolor should be used, will convert 24-bit colors to 256 color (6x6x6 color cube) if false."},

		{"force_tty", 			"#* Set to true to force tty mode regardless if a real tty has been detected or not.\n"
								"#* Will force 16-color mode and TTY theme, set all graph symbols to \"tty\" and swap out other non tty friendly symbols."},

		{"presets",				"#* Define presets for the layout of the boxes. Preset 0 is always all boxes shown with default settings. Max 9 presets.\n"
								"#* Format: \"box_name:P:G,box_name:P:G\" P=(0 or 1) for alternate positions, G=graph symbol to use for box.\n"
								"#* Use whitespace \" \" as separator between different presets.\n"
								"#* Example: \"cpu:0:default,mem:0:tty,proc:1:default cpu:0:braille,proc:0:tty\""},

		{"vim_keys",			"#* Set to True to enable \"h,j,k,l,g,G\" keys for directional control in lists.\n"
								"#* Conflicting keys for h:\"help\" and k:\"kill\" is accessible while holding shift."},

		{"rounded_corners",		"#* Rounded corners on boxes, is ignored if TTY mode is ON."},

		{"graph_symbol", 		"#* Default symbols to use for graph creation, \"braille\", \"block\" or \"tty\".\n"
								"#* \"braille\" offers the highest resolution but might not be included in all fonts.\n"
								"#* \"block\" has half the resolution of braille but uses more common characters.\n"
								"#* \"tty\" uses only 3 different symbols but will work with most fonts and should work in a real TTY.\n"
								"#* Note that \"tty\" only has half the horizontal resolution of the other two, so will show a shorter historical view."},

		{"graph_symbol_cpu", 	"# Graph symbol to use for graphs in cpu box, \"default\", \"braille\", \"block\" or \"tty\"."},
#ifdef GPU_SUPPORT
		{"graph_symbol_gpu", 	"# Graph symbol to use for graphs in gpu box, \"default\", \"braille\", \"block\" or \"tty\"."},
#endif
		{"graph_symbol_mem", 	"# Graph symbol to use for graphs in cpu box, \"default\", \"braille\", \"block\" or \"tty\"."},

		{"graph_symbol_net", 	"# Graph symbol to use for graphs in cpu box, \"default\", \"braille\", \"block\" or \"tty\"."},

		{"graph_symbol_proc", 	"# Graph symbol to use for graphs in cpu box, \"default\", \"braille\", \"block\" or \"tty\"."},

		{"shown_boxes", 		"#* Manually set which boxes to show. Available values are \"cpu mem net proc\" and \"gpu0\" through \"gpu5\", separate values with whitespace."},

		{"update_ms", 			"#* Update time in milliseconds, recommended 2000 ms or above for better sample times for graphs."},

		{"proc_sorting",		"#* Processes sorting, \"pid\" \"program\" \"arguments\" \"threads\" \"user\" \"memory\" \"cpu lazy\" \"cpu direct\",\n"
								"#* \"cpu lazy\" sorts top process over time (easier to follow), \"cpu direct\" updates top process directly."},

		{"proc_reversed",		"#* Reverse sorting order, True or False."},

		{"proc_tree",			"#* Show processes as a tree."},

		{"proc_colors", 		"#* Use the cpu graph colors in the process list."},

		{"proc_gradient", 		"#* Use a darkening gradient in the process list."},

		{"proc_per_core", 		"#* If process cpu usage should be of the core it's running on or usage of the total available cpu power."},

		{"proc_mem_bytes", 		"#* Show process memory as bytes instead of percent."},

		{"proc_cpu_graphs",     "#* Show cpu graph for each process."},

		{"proc_info_smaps",		"#* Use /proc/[pid]/smaps for memory information in the process info box (very slow but more accurate)"},

		{"proc_left",			"#* Show proc box on left side of screen instead of right."},

		{"proc_filter_kernel",  "#* (Linux) Filter processes tied to the Linux kernel(similar behavior to htop)."},

		{"proc_aggregate",		"#* In tree-view, always accumulate child process resources in the parent process."},

		{"cpu_graph_upper", 	"#* Sets the CPU stat shown in upper half of the CPU graph, \"total\" is always available.\n"
								"#* Select from a list of detected attributes from the options menu."},

		{"cpu_graph_lower", 	"#* Sets the CPU stat shown in lower half of the CPU graph, \"total\" is always available.\n"
								"#* Select from a list of detected attributes from the options menu."},
	#ifdef GPU_SUPPORT
		{"show_gpu_info",		"#* If gpu info should be shown in the cpu box. Available values = \"Auto\", \"On\" and \"Off\"."},
	#endif
		{"cpu_invert_lower", 	"#* Toggles if the lower CPU graph should be inverted."},

		{"cpu_single_graph", 	"#* Set to True to completely disable the lower CPU graph."},

		{"cpu_bottom",			"#* Show cpu box at bottom of screen instead of top."},

		{"show_uptime", 		"#* Shows the system uptime in the CPU box."},

		{"check_temp", 			"#* Show cpu temperature."},

		{"cpu_sensor", 			"#* Which sensor to use for cpu temperature, use options menu to select from list of available sensors."},

		{"show_coretemp", 		"#* Show temperatures for cpu cores also if check_temp is True and sensors has been found."},

		{"cpu_core_map",		"#* Set a custom mapping between core and coretemp, can be needed on certain cpus to get correct temperature for correct core.\n"
								"#* Use lm-sensors or similar to see which cores are reporting temperatures on your machine.\n"
								"#* Format \"x:y\" x=core with wrong temp, y=core with correct temp, use space as separator between multiple entries.\n"
								"#* Example: \"4:0 5:1 6:3\""},

		{"temp_scale", 			"#* Which temperature scale to use, available values: \"celsius\", \"fahrenheit\", \"kelvin\" and \"rankine\"."},

		{"base_10_sizes",		"#* Use base 10 for bits/bytes sizes, KB = 1000 instead of KiB = 1024."},

		{"show_cpu_freq", 		"#* Show CPU frequency."},

		{"clock_format", 		"#* Draw a clock at top of screen, formatting according to strftime, empty string to disable.\n"
								"#* Special formatting: /host = hostname | /user = username | /uptime = system uptime"},

		{"background_update", 	"#* Update main ui in background when menus are showing, set this to false if the menus is flickering too much for comfort."},

		{"custom_cpu_name", 	"#* Custom cpu model name, empty string to disable."},

		{"disks_filter", 		"#* Optional filter for shown disks, should be full path of a mountpoint, separate multiple values with whitespace \" \".\n"
									"#* Only disks matching the filter will be shown. Prepend exclude= to only show disks not matching the filter. Examples: disk_filter=\"/boot /home/user\", disks_filter=\"exclude=/boot /home/user\""},

		{"mem_graphs", 			"#* Show graphs instead of meters for memory values."},

		{"mem_below_net",		"#* Show mem box below net box instead of above."},

		{"zfs_arc_cached",		"#* Count ZFS ARC in cached and available memory."},

		{"show_swap", 			"#* If swap memory should be shown in memory box."},

		{"swap_disk", 			"#* Show swap as a disk, ignores show_swap value above, inserts itself after first disk."},

		{"show_disks", 			"#* If mem box should be split to also show disks info."},

		{"only_physical", 		"#* Filter out non physical disks. Set this to False to include network disks, RAM disks and similar."},

		{"use_fstab", 			"#* Read disks list from /etc/fstab. This also disables only_physical."},

		{"zfs_hide_datasets",		"#* Setting this to True will hide all datasets, and only show ZFS pools. (IO stats will be calculated per-pool)"},

		{"disk_free_priv",		"#* Set to true to show available disk space for privileged users."},

		{"show_io_stat", 		"#* Toggles if io activity % (disk busy time) should be shown in regular disk usage view."},

		{"io_mode", 			"#* Toggles io mode for disks, showing big graphs for disk read/write speeds."},

		{"io_graph_combined", 	"#* Set to True to show combined read/write io graphs in io mode."},

		{"io_graph_speeds", 	"#* Set the top speed for the io graphs in MiB/s (100 by default), use format \"mountpoint:speed\" separate disks with whitespace \" \".\n"
								"#* Example: \"/mnt/media:100 /:20 /boot:1\"."},

		{"net_download", 		"#* Set fixed values for network graphs in Mebibits. Is only used if net_auto is also set to False."},

		{"net_upload", ""},

		{"net_auto", 			"#* Use network graphs auto rescaling mode, ignores any values set above and rescales down to 10 Kibibytes at the lowest."},

		{"net_sync", 			"#* Sync the auto scaling for download and upload to whichever currently has the highest scale."},

		{"net_iface", 			"#* Starts with the Network Interface specified here."},

	    {"base_10_bitrate",     "#* \"True\" shows bitrates in base 10 (Kbps, Mbps). \"False\" shows bitrates in binary sizes (Kibps, Mibps, etc.). \"Auto\" uses base_10_sizes."},

		{"show_battery", 		"#* Show battery stats in top right if battery is present."},

		{"selected_battery",	"#* Which battery to use if multiple are present. \"Auto\" for auto detection."},

		{"show_battery_watts",	"#* Show power stats of battery next to charge indicator."},

		{"log_level", 			"#* Set loglevel for \"~/.config/btop/btop.log\" levels are: \"ERROR\" \"WARNING\" \"INFO\" \"DEBUG\".\n"
								"#* The level set includes all lower levels, i.e. \"DEBUG\" will show all logging info."},
	#ifdef GPU_SUPPORT

		{"nvml_measure_pcie_speeds",
								"#* Measure PCIe throughput on NVIDIA cards, may impact performance on certain cards."},
		{"rsmi_measure_pcie_speeds",
								"#* Measure PCIe throughput on AMD cards, may impact performance on certain cards."},
		{"gpu_mirror_graph",	"#* Horizontally mirror the GPU graph."},
		{"custom_gpu_name0",	"#* Custom gpu0 model name, empty string to disable."},
		{"custom_gpu_name1",	"#* Custom gpu1 model name, empty string to disable."},
		{"custom_gpu_name2",	"#* Custom gpu2 model name, empty string to disable."},
		{"custom_gpu_name3",	"#* Custom gpu3 model name, empty string to disable."},
		{"custom_gpu_name4",	"#* Custom gpu4 model name, empty string to disable."},
		{"custom_gpu_name5",	"#* Custom gpu5 model name, empty string to disable."},
	#endif
	};

    [[nodiscard]] std::optional<fs::path> get_config_dir() noexcept
    {
        fs::path config_dir;
        {
            std::error_code error;
            if (const auto xdg_config_home = std::getenv("XDG_CONFIG_HOME"); xdg_config_home != nullptr)
            {
                if (fs::exists(xdg_config_home, error))
                {
                    config_dir = fs::path(xdg_config_home) / "roam";
                }
            }
            else if (const auto home = std::getenv("HOME"); home != nullptr)
            {
                error.clear();
                if (fs::exists(home, error))
                {
                    config_dir = fs::path(home) / ".config" / "roam";
                }

                if (error)
                {
                    fmt::print(stderr, "\033[0;31mWarning: \033[0m{} could not be accessed: {}\n", config_dir.string(), error.message());
                    config_dir = "";
                }
            }
        }
        if (!config_dir.empty())
        {
            std::error_code error;
            if (fs::exists(config_dir, error))
            {
                if (fs::is_directory(config_dir, error))
                {
                    struct statvfs stats{};
                    if ((fs::status(config_dir, error).permissions() & fs::perms::owner_write) == fs::perms::owner_write and statvfs(config_dir.c_str(), &stats) == 0 and (stats.f_flag & ST_RDONLY) == 0)
                    {
                        return config_dir;
                    }
                    else
                    {
                        fmt::print(stderr, "\033[0;31mWarning: \033[0m`{}` is not writable\n", fs::absolute(config_dir).string());
                        if ((fs::status(config_dir, error).permissions() & fs::perms::owner_read) == fs::perms::owner_read)
                        {
                            fmt::print(stderr, "\033[0;31mWarning: \033[0mLogging is disabled, config changes are not persistent\n");
                            return config_dir;
                        }
                    }
                }
                else
                {
                    fmt::print(stderr, "\033[0;31mWarning: \033[0m`{}` is not a directory\n", fs::absolute(config_dir).string());
                }
            }
            else
            { // not exist
                if (fs::create_directories(config_dir, error))
                {
                    return config_dir;
                }
                else
                {
                    fmt::print(stderr, "\033[0;31mWarning: \033[0m`{}` could not be created: {}\n", fs::absolute(config_dir).string(), error.message());
                }
            }
        }
        else
        {
            fmt::print(stderr, "\033[0;31mWarning: \033[0mCould not determine config path: Make sure `$XDG_CONFIG_HOME` or `$HOME` is set\n");
        }
        fmt::print(stderr, "\033[0;31mWarning: \033[0mLogging is disabled, config changes are not persistent\n");
        return {};
    }

    fs::path conf_dir;
    fs::path conf_file;

    bool _locked(const std::string_view name){
        atomic_wait(writelock, true);
        if(!write_new and rng::find_if(descriptions, [&name](const auto &a) { return a.at(0) == name;})!= descriptions.end())
            write_new = true;
        return locked.load();
    }
    string validError;
    bool intValid(const std::string_view name, const string& value) {   
        int i_value;
        try{
            i_value = stoi(value);
        }catch (const std::invalid_argument&){
            validError = "Invalid numerical value!";
			return false;
        }
        catch (const std::out_of_range&) {
			validError = "Value out of range!";
			return false;
		}
		catch (const std::exception& e) {
			validError = string{e.what()};
			return false;
		}
        if (name == "update_ms" and i_value < 100)
			validError = "Config value update_ms set too low (<100).";

		else if (name == "update_ms" and i_value > ONE_DAY_MILLIS)
			validError = fmt::format("Config value update_ms set too high (>{}).", ONE_DAY_MILLIS);
        else
			return true;
        return false;

    }

    bool stringValid(const std::string_view name, const string& value) {
        if (name == "log_level" and not v_contains(Logger::log_levels, value))
			validError = "Invalid log_level: " + value;

		//TODO: add more valid check
        return true;
    }

    void load(const fs::path &conf_file, vector<string> &load_warnings)
    {
        std::error_code error;
        if (conf_file.empty())
        {
            return;
        }
        else if (!fs::exists(conf_file, error))
        {
            write_new = true;
            return;
        }
        if(error){
            return ;
        }
        std::ifstream cread(conf_file);
        if (cread.good()){
            vector<string> valid_names;
            valid_names.reserve(descriptions.size());
            for (const auto &n : descriptions)
				valid_names.push_back(n[0]);
            if (string v_string; cread.peek() != '#' or (getline(cread, v_string, '\n') and not s_contains(v_string, Global::Version)))
				write_new = true;
            while (not cread.eof()) {
				cread >> std::ws;
				if (cread.peek() == '#') {
					cread.ignore(SSmax, '\n');
					continue;
				}
				string name, value;
				getline(cread, name, '=');
				if (name.ends_with(' ')) name = trim(name); // 去除name 左右两侧的空格
				if (not v_contains(valid_names, name)) {
					cread.ignore(SSmax, '\n');
					continue;
				}
				cread >> std::ws;

				if (bools.contains(name)) {
					cread >> value;
					if (not isbool(value))
						load_warnings.push_back("Got an invalid bool value for config name: " + name);
					else
						bools.at(name) = stobool(value); // stobool 只判断是否是true ，减少了2次判断false的情况。
				}
				else if (ints.contains(name)) {
					cread >> value;
					if (not isint(value))
						load_warnings.push_back("Got an invalid integer value for config name: " + name);
					else if (not intValid(name, value)) {
						load_warnings.push_back(validError);
					}
					else
						ints.at(name) = stoi(value);
				}
				else if (strings.contains(name)) {
					if (cread.peek() == '"') {
						cread.ignore(1);
						getline(cread, value, '"');
					}
					else cread >> value;

					if (not stringValid(name, value))
						load_warnings.push_back(validError);
					else
						strings.at(name) = value;
				}

				cread.ignore(SSmax, '\n');
			}

			if (not load_warnings.empty()) write_new = true;
        }
    }

    static auto get_xdg_state_dir() -> std::optional<std::filesystem::path>
    {
        std::optional<std::filesystem::path> xdg_state_home;
        {
            const auto xdg_state_home_ptr = std::getenv("XDG_STATE_HOME");
            if (xdg_state_home_ptr != nullptr)
            {
                xdg_state_home = std::make_optional(fs::path(xdg_state_home_ptr));
            }
            else
            {
                const auto home_ptr = std::getenv("HOME");
                if (home_ptr != nullptr)
                {
                    xdg_state_home = std::make_optional(fs::path(home_ptr) / ".local" / "state");
                }
            }
        }
        if (xdg_state_home.has_value())
        {
            std::error_code err;
            fs::create_directories(xdg_state_home.value(), err);
            if (err)
            {
                return std::nullopt;
            }
            return std::make_optional(xdg_state_home.value());
        }
        return std::nullopt;
    }
    auto get_log_file() -> std::optional<fs::path>
    {
        auto xdg_state_home = get_xdg_state_dir();
        if (xdg_state_home.has_value())
        {
            return std::make_optional(std::filesystem::path(xdg_state_home.value()) / "roam.log");
        }
        return std::nullopt;
    }
}