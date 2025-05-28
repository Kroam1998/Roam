#include "roam_tools.h"

#include <atomic>
#include <iostream>
#include <string>
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
}

namespace Logger
{
    using namespace Tools;
    std::atomic<bool> busy(false);
    bool first = true;
    const string tdf = "%Y/%m/%d (%T) | "; // 时间格式
    size_t loglevel;
    std::optional<std::filesystem::path> logfile;

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
    }
}
