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

2025.0609
