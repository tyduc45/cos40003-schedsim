#include "Process.h"
#include <queue>
#include <algorithm>
#include <numeric>

Result schedule_sjf_np(std::vector<Process> procs)
{
    Result r;
    r.name = "Shortest Job First (Non-Preemptive)";
    std::sort(procs.begin(), procs.end(), [](auto &a, auto &b)
              { return a.arrival < b.arrival || (a.arrival == b.arrival && a.pid < b.pid); });
    int t = 0, i = 0, n = procs.size();
    auto cmp = [](const Process *a, const Process *b)
    {
        if (a->burst != b->burst)
            return a->burst > b->burst; // min-heap by burst
        return a->pid > b->pid;
    };
    std::priority_queue<Process *, std::vector<Process *>, decltype(cmp)> pq(cmp);

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
        int start = t;
        t += p->burst;
        p->finish_time = t;
        p->turnaround = p->finish_time - p->arrival;
        p->waiting = p->turnaround - p->burst;
        p->response = p->start_time - p->arrival;
        r.timeline.push_back({p->pid, start, t});
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
