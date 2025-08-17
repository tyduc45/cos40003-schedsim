#include "Process.h"
#include <queue>
#include <algorithm>
#include <numeric>

Result schedule_sjf_p(std::vector<Process> procs)
{
    Result r;
    r.name = "Shortest Job First (Preemptive, SRTF)";
    std::sort(procs.begin(), procs.end(), [](auto &a, auto &b)
              { return a.arrival < b.arrival || (a.arrival == b.arrival && a.pid < b.pid); });
    int t = 0, i = 0, n = procs.size();
    auto cmp = [](const Process *a, const Process *b)
    {
        if (a->remaining != b->remaining)
            return a->remaining > b->remaining; // min-heap by remaining
        return a->pid > b->pid;
    };
    std::priority_queue<Process *, std::vector<Process *>, decltype(cmp)> pq(cmp);

    int current_pid = -1;
    int slice_start = -1;

    while (i < n || !pq.empty())
    {
        while (i < n && procs[i].arrival <= t)
            pq.push(&procs[i++]);
        if (pq.empty())
        {
            t = procs[i].arrival;
            continue;
        }

        Process *p = pq.top();
        pq.pop();
        if (p->start_time < 0)
            p->start_time = t;
        if (current_pid != p->pid)
        {
            if (current_pid != -1)
            {
                // close previous slice
                r.timeline.back().end = t;
            }
            slice_start = t;
            r.timeline.push_back({p->pid, slice_start, -1});
            current_pid = p->pid;
        }
        // 执行 1 个时间单位
        p->remaining -= 1;
        t += 1;
        // 到 t 的新进程入队
        while (i < n && procs[i].arrival <= t)
            pq.push(&procs[i++]);
        if (p->remaining > 0)
        {
            pq.push(p);
        }
        else
        {
            // 完成
            r.timeline.back().end = t;
            p->finish_time = t;
            p->turnaround = p->finish_time - p->arrival;
            p->waiting = p->turnaround - p->burst;
            p->response = p->start_time - p->arrival;
            current_pid = -1;
        }
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
