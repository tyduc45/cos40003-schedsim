#include "Process.h"
#include <queue>
#include <algorithm>
#include <numeric>

Result schedule_rr(std::vector<Process> procs, int q)
{
    Result r;
    r.name = "Round-Robin (q=" + std::to_string(q) + ")";
    std::sort(procs.begin(), procs.end(), [](auto &a, auto &b)
              { return a.arrival < b.arrival || (a.arrival == b.arrival && a.pid < b.pid); });
    int n = procs.size(), i = 0, t = 0;
    std::queue<int> rq; // index in procs

    auto push_arrived = [&](int time)
    {
        while (i < n && procs[i].arrival <= time)
        {
            rq.push(i++);
        }
    };

    push_arrived(t);
    if (rq.empty())
    {
        t = procs[0].arrival;
        push_arrived(t);
    }

    while (!rq.empty())
    {
        int idx = rq.front();
        rq.pop();
        Process &p = procs[idx];
        if (p.start_time < 0)
            p.start_time = t;

        int run = std::min(q, p.remaining);
        int start = t;
        t += run;
        p.remaining -= run;
        r.timeline.push_back({p.pid, start, t});

        // 新到达
        push_arrived(t);

        if (p.remaining > 0)
        {
            rq.push(idx); // 轮转
        }
        else
        {
            p.finish_time = t;
            p.turnaround = p.finish_time - p.arrival;
            p.waiting = p.turnaround - p.burst;
            p.response = p.start_time - p.arrival;
            if (rq.empty() && i < n && t < procs[i].arrival)
            {
                t = procs[i].arrival;
                push_arrived(t);
            }
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
