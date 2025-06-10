#pragma once

#include <filesystem>
#include <optional>
#include <vector>
#include <string>
#include <unordered_map>

using std::string;
using std::vector;

namespace Config{

    extern std::filesystem::path conf_dir;
    extern std::filesystem::path conf_file;

	std::unordered_map<std::string_view, string> strings = {
		{"color_theme", "Default"},
		{"shown_boxes", "cpu mem net proc"},
		{"graph_symbol", "braille"},
		{"presets", "cpu:1:default,proc:0:default cpu:0:default,mem:0:default,net:0:default cpu:0:block,net:0:tty"},
		{"graph_symbol_cpu", "default"},
		{"graph_symbol_gpu", "default"},
		{"graph_symbol_mem", "default"},
		{"graph_symbol_net", "default"},
		{"graph_symbol_proc", "default"},
		{"proc_sorting", "cpu lazy"},
		{"cpu_graph_upper", "Auto"},
		{"cpu_graph_lower", "Auto"},
		{"cpu_sensor", "Auto"},
		{"selected_battery", "Auto"},
		{"cpu_core_map", ""},
		{"temp_scale", "celsius"},
		{"clock_format", "%X"},
		{"custom_cpu_name", ""},
		{"disks_filter", ""},
		{"io_graph_speeds", ""},
		{"net_iface", ""},
		{"base_10_bitrate", "Auto"},
		{"log_level", "WARNING"},
		{"proc_filter", ""},
		{"proc_command", ""},
		{"selected_name", ""},
	#ifdef GPU_SUPPORT
		{"custom_gpu_name0", ""},
		{"custom_gpu_name1", ""},
		{"custom_gpu_name2", ""},
		{"custom_gpu_name3", ""},
		{"custom_gpu_name4", ""},
		{"custom_gpu_name5", ""},
		{"show_gpu_info", "Auto"}
	#endif
	};
	std::unordered_map<std::string_view, string> stringsTmp;


    std::unordered_map<std::string_view, bool> boolsTmp;
    std::unordered_map<std::string_view, bool> bools = {
		{"theme_background", true},
		{"truecolor", true},
		{"rounded_corners", true},
		{"proc_reversed", false},
		{"proc_tree", false},
		{"proc_colors", true},
		{"proc_gradient", true},
		{"proc_per_core", false},
		{"proc_mem_bytes", true},
		{"proc_cpu_graphs", true},
		{"proc_info_smaps", false},
		{"proc_left", false},
		{"proc_filter_kernel", false},
		{"cpu_invert_lower", true},
		{"cpu_single_graph", false},
		{"cpu_bottom", false},
		{"show_uptime", true},
		{"check_temp", true},
		{"show_coretemp", true},
		{"show_cpu_freq", true},
		{"background_update", true},
		{"mem_graphs", true},
		{"mem_below_net", false},
		{"zfs_arc_cached", true},
		{"show_swap", true},
		{"swap_disk", true},
		{"show_disks", true},
		{"only_physical", true},
		{"use_fstab", true},
		{"zfs_hide_datasets", false},
		{"show_io_stat", true},
		{"io_mode", false},
		{"base_10_sizes", false},
		{"io_graph_combined", false},
		{"net_auto", true},
		{"net_sync", true},
		{"show_battery", true},
		{"show_battery_watts", true},
		{"vim_keys", false},
		{"tty_mode", false},
		{"disk_free_priv", false},
		{"force_tty", false},
		{"lowcolor", false},
		{"show_detailed", false},
		{"proc_filtering", false},
		{"proc_aggregate", false},
	#ifdef GPU_SUPPORT
		{"nvml_measure_pcie_speeds", true},
		{"rsmi_measure_pcie_speeds", true},
		{"gpu_mirror_graph", true}
	#endif
	};
    
    std::unordered_map<std::string_view, int> ints = {
		{"update_ms", 2000},
		{"net_download", 100},
		{"net_upload", 100},
		{"detailed_pid", 0},
		{"selected_pid", 0},
		{"selected_depth", 0},
		{"proc_start", 0},
		{"proc_selected", 0},
		{"proc_last_selected", 0}
	};
	std::unordered_map<std::string_view, int> intsTmp;

    extern string validError;
    constexpr int ONE_DAY_MILLIS = 1000 * 60 * 60 * 24;

	bool intValid(const std::string_view name, const string& value);
    bool stringValid(const std::string_view name, const string& value);

    void load(const std::filesystem::path& conf_file, vector<string>& load_warnings);
    [[nodiscard]] std::optional<std::filesystem::path> get_config_dir() noexcept;
    bool _locked(const std::string_view name);

    // return bool value of name
    inline bool getB(const std::string_view name) { return bools.at(name);}
    //* Return string for config key <name> 
	inline const string& getS(const std::string_view name) { return strings.at(name); }

    inline void set(const string& name, bool value) {
        if (_locked(name)) boolsTmp.insert_or_assign(name, value);
        else bools.at(name) = value;
    }

    inline void set(const string& name, const int value) {
        if (_locked(name)) intsTmp.insert_or_assign(name,value);
        else ints.at(name) = value;
    }

    inline void set(const string& name, const string& value) {
        if (_locked(name)) stringsTmp.insert_or_assign(name, value);
        else strings.at(name) = value;
    }
    

    auto get_log_file() -> std::optional<std::filesystem::path>;   
}