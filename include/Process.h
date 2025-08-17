#pragma once
#include <vector>
#include <string>

struct Process
{
    int pid;
    int arrival;
    int burst;
    int remaining;   // 动态剩余时间（用于抢占类）
    int start_time;  // 第一次开始执行时间（用于响应时间）
    int finish_time; // 完成时间
    int waiting;     // 等待时间
    int turnaround;  // 周转时间
    int response;    // 响应时间
    int tickets;     // Lottery 票数（其他算法可忽略）
    Process(int pid_, int arr_, int bur_, int t_ = 1)
        : pid(pid_), arrival(arr_), burst(bur_), remaining(bur_),
          start_time(-1), finish_time(-1), waiting(0), turnaround(0),
          response(0), tickets(t_) {}
};

// 调度结果：执行序列（pid, start, end）
struct Slice
{
    int pid;
    int start;
    int end;
};

struct Result
{
    std::vector<Process> procs;  // 计算后的每个进程
    std::vector<Slice> timeline; // 调度切片
    double avg_wait = 0, avg_turn = 0, avg_resp = 0;
    std::string name;
};

Result schedule_fifo(std::vector<Process> procs);
Result schedule_sjf_np(std::vector<Process> procs);
Result schedule_sjf_p(std::vector<Process> procs);
Result schedule_rr(std::vector<Process> procs, int quantum);
Result schedule_lottery(std::vector<Process> procs, int seed = 1);
Result schedule_mlfq(std::vector<Process> procs);