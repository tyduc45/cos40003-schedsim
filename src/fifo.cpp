#include "Process.h"
#include <algorithm>
#include <numeric>

Result schedule_fifo(std::vector<Process> procs)
{
    std::sort(procs.begin(), procs.end(),
              [](auto &a, auto &b)
              { return a.arrival < b.arrival ||
                       (a.arrival == b.arrival && a.pid < b.pid); });
    Result r;
    r.name = "First-Come, First-Served (FIFO)";
    int t = 0;
    for (auto &p : procs)
    {
        if (t < p.arrival)
            t = p.arrival;
        if (p.start_time < 0)
            p.start_time = t;
        int start = t;
        t += p.burst;
        p.finish_time = t;
        p.turnaround = p.finish_time - p.arrival;
        p.waiting = p.turnaround - p.burst;
        p.response = p.start_time - p.arrival;
        r.timeline.push_back({p.pid, start, t});
    }
    r.procs = procs;
    auto n = (double)procs.size();
    r.avg_wait = std::accumulate(procs.begin(), procs.end(), 0.0, [](double s, const Process &x)
                                 { return s + x.waiting; }) /
                 n;
    r.avg_turn = std::accumulate(procs.begin(), procs.end(), 0.0, [](double s, const Process &x)
                                 { return s + x.turnaround; }) /
                 n;
    r.avg_resp = std::accumulate(procs.begin(), procs.end(), 0.0, [](double s, const Process &x)
                                 { return s + x.response; }) /
                 n;
    return r;
}
