#include "roam_tools.h"
#include "roam_shared.h"

#include <atomic>
#include <iostream>
#include <sstream>
#include <fstream>
#include <string>
#include <ctime>
#include <chrono>
#include <ranges>
#include <unistd.h>

namespace Term
{

    std::atomic<bool> initialized{false};
    std::atomic<int> width{};
    std::atomic<int> height{};
    std::string current_tty;

    // 初始化终端状态
    bool init()
    {
        if (!initialized.load())
        {                                            // 使用load获取原子变量值
            bool is_terminal = isatty(STDIN_FILENO); // 判断是否连接到终端
            initialized.store(is_terminal);          // 使用store设置原子变量值
        }
        return initialized.load(); // 返回当前初始化状态
    }

} // namespace Term

namespace Tools
{
    atomic_lock::atomic_lock(atomic<bool> &atom, bool wait) : atom(atom)
    {
        if (wait)
            while (!this->atom.compare_exchange_strong(this->not_true, true))
                ;
        else
            this->atom.store(true);
    }
    atomic_lock::~atomic_lock() noexcept
    {
        this->atom.store(false);
    }
    string strf_time(const string &strf)
    { // 将时间结构按照指定格式转换为字符串
        auto in_time_t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
        std::tm bt{};
        std::stringstream ss;
        ss << std::put_time(localtime_r(&in_time_t, &bt), strf.c_str());
        return ss.str();
    }
    
}

namespace Logger
{
    using namespace Tools;
    std::atomic<bool> busy(false);
    bool first = true;
    const string tdf = "%Y/%m/%d (%T) | "; // 时间格式
    size_t loglevel;
    std::optional<std::filesystem::path> logfile;

    //* lower priviledge
    class lose_priv
    {
        int status = -1;

    public:
        lose_priv()
        {
            if (getuid() != Global::real_uid)
            {
                this->status = setuid(Global::real_uid);
            }
        }
        ~lose_priv()
        {
            if (status == 0)
            {
                status = seteuid(Global::set_uid);
            }
        }
        lose_priv(const lose_priv &) = delete;
        lose_priv &operator=(const lose_priv &) = delete;
        lose_priv(lose_priv &&) = delete;
        lose_priv &operator=(lose_priv &&other) = delete;
    };

    void set(const string &level)
    {
        loglevel = v_index(log_levels, level);
    }

    void log_write(const Level &level, const std::string_view &msg)
    {
        if (loglevel < level || !logfile.has_value())
        {
            return;
        }
        auto &log_file = logfile.value();
        atomic_lock lck(busy, true);
        lose_priv neutered{};
        std::error_code ec;
        try
        {
            if (fs::exists(log_file) and fs::file_size(log_file, ec) > 1024 << 10 and not ec)
            {
                auto old_log = log_file;
                old_log += ".1";

                if (fs::exists(old_log))
                    fs::remove(old_log);
                if (not ec)
                {
                    fs::rename(log_file, old_log, ec);
                }
            }
            if (not ec)
            {
                std::ofstream lwrite(log_file, std::ios::app);
                if (first)
                {
                    first = false;
                    lwrite << "\n"
                           << strf_time(tdf) << "===> roam v." << Global::Version << "\n";
                }
                lwrite << "\n"
                       << strf_time(tdf) << log_levels.at(level) << ": " << msg << "\n";
            }
            else
                log_file.clear();
        }
        catch (const std::exception &e)
        {
            log_file.clear();
            throw std::runtime_error("Exception in Logger::log_write: " + std::string(e.what()));
        }
    }
}
