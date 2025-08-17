#include "Process.h"
#include <random>
#include <algorithm>
#include <numeric>
#include <vector>

Result schedule_lottery(std::vector<Process> procs, int seed)
{
    Result r;
    r.name = "Lottery Scheduling (seed=" + std::to_string(seed) + ")";
    std::sort(procs.begin(), procs.end(), [](auto &a, auto &b)
              { return a.arrival < b.arrival || (a.arrival == b.arrival && a.pid < b.pid); });

    int n = procs.size(), done = 0, t = 0, i = 0;
    std::mt19937 rng(seed);

    // 时间粒度：1 单位步进；每步一次彩票抽签（在就绪集合中按票数加权）
    while (done < n)
    {
        // 收集就绪集合
        std::vector<int> ready;
        while (i < n && procs[i].arrival <= t)
            ready.push_back(i++);
        // 合并之前的就绪（保持在 ready 中）
        // 需要维护跨时间步的 ready 集：把上一轮未完成的也加进去
        static std::vector<int> carry;
        for (int idx : carry)
            ready.push_back(idx);
        carry.clear();

        // 去重
        std::sort(ready.begin(), ready.end());
        ready.erase(std::unique(ready.begin(), ready.end()), ready.end());

        // 过滤掉已经完成的
        std::vector<int> active;
        for (int idx : ready)
            if (procs[idx].remaining > 0)
                active.push_back(idx);

        if (active.empty())
        {
            // 没有就绪，跳到下一个到达
            if (i < n)
                t = std::max(t, procs[i].arrival);
            else
                break;
            // 下一轮继续
            continue;
        }

        // 按票数抽签
        int total_tickets = 0;
        for (int idx : active)
            total_tickets += std::max(1, procs[idx].tickets);
        std::uniform_int_distribution<int> dist(1, total_tickets);
        int draw = dist(rng);

        int chosen = -1, cum = 0;
        for (int idx : active)
        {
            cum += std::max(1, procs[idx].tickets);
            if (draw <= cum)
            {
                chosen = idx;
                break;
            }
        }

        Process &p = procs[chosen];
        if (p.start_time < 0)
            p.start_time = t;

        int start = t;
        // 执行 1 个时间单位（也可改成时间片 >1，但经典实现常按粒度 1）
        p.remaining -= 1;
        t += 1;

        r.timeline.push_back({p.pid, start, t});

        if (p.remaining == 0)
        {
            p.finish_time = t;
            p.turnaround = p.finish_time - p.arrival;
            p.waiting = p.turnaround - p.burst;
            p.response = p.start_time - p.arrival;
            done++;
            // 把其他仍未完成的带到下一轮
            for (int idx : active)
                if (idx != chosen && procs[idx].remaining > 0)
                    carry.push_back(idx);
        }
        else
        {
            // 全部仍未完成的进下一轮
            for (int idx : active)
                carry.push_back(idx);
        }
    }

    // 收尾（若有）
    for (auto &p : procs)
        if (p.finish_time < 0)
        {
            p.finish_time = t;
            p.turnaround = p.finish_time - p.arrival;
            p.waiting = p.turnaround - p.burst;
            p.response = (p.start_time < 0 ? 0 : p.start_time - p.arrival);
        }

    r.procs = procs;
    double dn = n;
    r.avg_wait = std::accumulate(procs.begin(), procs.end(), 0.0, [](double s, const Process &x)
                                 { return s + x.waiting; }) /
                 dn;
    r.avg_turn = std::accumulate(procs.begin(), procs.end(), 0.0, [](double s, const Process &x)
                                 { return s + x.turnaround; }) /
                 dn;
    r.avg_resp = std::accumulate(procs.begin(), procs.end(), 0.0, [](double s, const Process &x)
                                 { return s + x.response; }) /
                 dn;
    return r;
}
