# Roam

A todo list program written by c++.

# TODO:

1. 实现一个TPF系统， Task，Property，Filter
2. 基本工作流的实现
3. 界面优化
4. 大模型接入，总结每日任务，规划未来任务
5. OKR 接入，自动生成周报、月报。
6. 多系统联动支持

2025.05.28

1. add Logger, todo: log_write
2. Tools: add atomic_lock class.
3. Term init()
4. fix bazel compile error of macosx

2025.0529

1. finish Logger. log_write finish. lose_priv 降级uid
2. 实现 tool ： time_s 利用std::chrono 计算 当前时间（s）
3. 实现 tool ： strf_time ： 输入一个string表示时间格式，返回当前时间根据该格式 format 的结果。
4. 增加roam_shared.h  用来声明global 的变量。
5. 写日志 log_write 逻辑： log file 如果已经存在且文件大小大于1MB，则将老日志文件rename 到 .1  原来的.1文件如果存在的话删除
   1. 另外如果第一次写日志会记录version， 之后正常写日志。
6. vector.at() 和 vector[] 的区别是 at 在越界时会抛异常，[]不检查越界， 但性能更好。
7. clean_quit没有实现干净，里面要先实现runner。
8. 准备先实现cli class

2025.0604

1. TODO： bazel 自动记录 config.h记录提交commit 号和compile信息 （利用genrule实现）
2. btop的项目，用cmake进行的编译，其中在obj文件夹里有一个config.h里面记录了commit号，另外在src下面还有一个config.h.in 这是怎么实现
3. 

2025.0609 \ 0610

1. add config implementation， 复用了一些config。 config 配置流数据读取抄了一些实现，字符串处理写的有点复杂，收获不多。
2. 静态bool 变量 first init 可以利用只初始化一次的特性来判断该程序是否是第一次被执行。
3. std:ranges::all_of( str, ::isdigit)这个函数可以用来快速判断一个字符串是否是整数。all_of会对str 进行遍历，::isdigit用于判断char 是否在 ‘0’-‘9’之间。
4. 模版判断一个给定值 和其他值， 例如判断一个值是否在其他值内， 类似python 的 in 。
   ```
   template<typename First, typename ... T>
   	inline bool is_in(const First& first, const T& ... t) {
   		return ((first == t) or ...);
   	}
   ```

第一个是first 类型，第二个是可变数量的T类型， 调用方法：

```
inline bool isbool(const std::string_view str) {
		return is_in(str, "true", "false", "True", "False");
	}
```

5. std::ranges::find_if(R， pred， Proj) 返回一个迭代器，指向第一个满足谓词条件的元素，如果没有找到符合条件的， 则返回范围的结束迭代器。
6. ```
   #if defined __i386__ || defined __x86_64__
       __builtin_ia32_pause();  // PAUSE 指令，减少功耗和总线竞争
   #elif defined __ia64__
       __asm volatile("hint @pause" : : : "memory");  // 类似 PAUSE 指令
   #elif defined __sparc__ && (defined __arch64__ || defined __sparc_v9__)
       __asm volatile("membar #LoadLoad" : : : "memory");  // 内存屏障指令，确保内存一致性
   #else
       __asm volatile("" : : : "memory");  // 空汇编指令，防止编译器优化
   #endif
   ```

   cpu 在等待锁时利用pause 指令， 减少cpu 功耗发热， 同时减少总线上cpu消息， 减少对总线竞争，提高多核程序效率。
   
7. C++ 17 后 unordered_map map 引入了 insert_or_assign方法， 用于不存在的key 创建，存在的key 更新值的效果。

   1. 优点：支持非默认构造的mapped_type, map[key] = value 如果value 类型没有默认构造函数会出错。
   2. 性能更好，insert_or_assign 直接构造或赋值， 后者默认先构造再赋值
   3. 返回值信息更多，insert_or_assign 返回迭代器+是否插入的标志， 后者无法知道是否插入还是修改。
   4. 完美转发构造参数，支持emplace 风格的参数传递。
   
8. unordered_map at 方法。
   map[key] 和 map.at(key)的区别：
   map[key] 允许访问不存在的键，自动插入默认值。 map.at(key) 不允许访问不存在的键， 会抛出异常。
   map[key] 可能会插入新元素 map.at(key) 不会改变容器内容。
   map[key] 不适用于const map map.at(key) 可用于const map。
   都返回值引用
   map[key]不抛出异常， map.at(key)抛出 std::out_of_range
   map[key]不适合用于只读访问和存在性检查， 后者适合。
   