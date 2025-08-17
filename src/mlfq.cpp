#include "Process.h"
#include <queue>
#include <algorithm>
#include <numeric>
#include <vector>

Result schedule_mlfq(std::vector<Process> procs)
{
    Result r;
    r.name = "Multi-Level Feedback Queue (3 levels, q=4/8/16)";

    // 经典 3 层：Q0=4, Q1=8, Q2=16；到达插入 Q0，时间片用完则降级；更高优先级到达会抢占
    const int Q = 3;
    const int quanta[Q] = {4, 8, 16};

    std::sort(procs.begin(), procs.end(), [](auto &a, auto &b)
              { return a.arrival < b.arrival || (a.arrival == b.arrival && a.pid < b.pid); });

    int n = procs.size(), i = 0, t = 0, done = 0;

    std::queue<int> q[Q];         // 存 idx
    std::vector<int> level(n, 0); // 进程当前队列层级
    std::vector<bool> enqueued(n, false);

    auto enqueue_arrivals = [&](int time)
    {
        while (i < n && procs[i].arrival <= time)
        {
            q[0].push(i);
            level[i] = 0;
            enqueued[i] = true;
            i++;
        }
    };

    enqueue_arrivals(t);
    if (q[0].empty())
    {
        t = procs[0].arrival;
        enqueue_arrivals(t);
    }

    int current = -1; // 当前 idx
    int current_level = -1;
    int remaining_quantum = 0;
    int slice_start = -1;

    auto start_run = [&](int idx, int lv)
    {
        current = idx;
        current_level = lv;
        remaining_quantum = quanta[lv];
        if (procs[idx].start_time < 0)
            procs[idx].start_time = t;
        slice_start = t;
        r.timeline.push_back({procs[idx].pid, slice_start, -1});
    };

    auto end_run = [&]()
    {
        if (!r.timeline.empty() && r.timeline.back().end == -1)
        {
            r.timeline.back().end = t;
        }
        current = -1;
        current_level = -1;
        remaining_quantum = 0;
        slice_start = -1;
    };

    while (done < n)
    {
        // 挑选最高优先级非空队列
        auto pick_next = [&]() -> bool
        {
            for (int lv = 0; lv < Q; ++lv)
            {
                if (!q[lv].empty())
                {
                    int idx = q[lv].front();
                    q[lv].pop();
                    start_run(idx, lv);
                    return true;
                }
            }
            return false;
        };

        if (current == -1)
        {
            if (!pick_next())
            {
                if (i < n)
                {
                    t = procs[i].arrival;
                    enqueue_arrivals(t);
                }
                else
                    break;
            }
        }

        // 执行 1 个时间单位（细粒度，便于抢占）
        enqueue_arrivals(t); // 有新高优先级到达会在下一步抢占
        procs[current].remaining -= 1;
        t += 1;
        remaining_quantum -= 1;

        // 执行期间是否有更高优先级到达？（我们在每个 tick 前后都 enqueue，到这里检查即时抢占）
        // 规则：如果有更高优先级队列不为空，则抢占
        bool higher_ready = false;
        for (int lv = 0; lv < current_level; ++lv)
            if (!q[lv].empty())
            {
                higher_ready = true;
                break;
            }

        if (procs[current].remaining == 0)
        {
            // 完成
            end_run();
            procs[current].finish_time = t;
            procs[current].turnaround = procs[current].finish_time - procs[current].arrival;
            procs[current].waiting = procs[current].turnaround - procs[current].burst;
            procs[current].response = procs[current].start_time - procs[current].arrival;
            done++;
        }
        else if (higher_ready)
        {
            // 被更高优先级抢占：回到当前层队列头/尾？经典实现回到同层队尾
            end_run();
            q[current_level].push(current);
        }
        else if (remaining_quantum == 0)
        {
            // 时间片用尽则降级
            int next_level = std::min(current_level + 1, Q - 1);
            int idx = current;
            end_run();
            q[next_level].push(idx);
            level[idx] = next_level;
        }

        // 如果 CPU 空闲，选下一个
        if (current == -1)
        {
            enqueue_arrivals(t);
            pick_next();
        }
    }

    // 收尾处理（理论上都完成）
    for (auto &p : procs)
        if (p.finish_time < 0)
        {
            p.finish_time = t;
            p.turnaround = p.finish_time - p.arrival;
            p.waiting = p.turnaround - p.burst;
            p.response = (p.start_time < 0 ? 0 : p.start_time - p.arrival);
        }

    double dn = n;
    Result r0;
    r0.name = r.name;
    r0.procs = procs;
    r0.timeline = r.timeline;
    r0.avg_wait = std::accumulate(procs.begin(), procs.end(), 0.0, [](double s, const Process &x)
                                  { return s + x.waiting; }) /
                  dn;
    r0.avg_turn = std::accumulate(procs.begin(), procs.end(), 0.0, [](double s, const Process &x)
                                  { return s + x.turnaround; }) /
                  dn;
    r0.avg_resp = std::accumulate(procs.begin(), procs.end(), 0.0, [](double s, const Process &x)
                                  { return s + x.response; }) /
                  dn;
    return r0;
}
