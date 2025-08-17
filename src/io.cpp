#include "io.h"
#include <iostream>
#include <iomanip>
#include <algorithm>

std::vector<Process> read_processes_from_stdin()
{
    int N;
    if (!(std::cin >> N))
    {
        std::cerr << "Failed to read N\n";
        std::exit(1);
    }
    std::vector<Process> v;
    v.reserve(N);
    for (int i = 0; i < N; i++)
    {
        int pid, arr, bur;
        int tickets = 1;
        if (!(std::cin >> pid >> arr >> bur))
        {
            std::cerr << "Failed to read process line " << i + 1 << "\n";
            std::exit(1);
        }
        if (!(std::cin.peek() == '\n' || std::cin.peek() == EOF))
        {
            if (std::cin >> tickets)
            {
                // ok
            }
            else
            {
                std::cin.clear();
            }
        }
        v.emplace_back(pid, arr, bur, tickets <= 0 ? 1 : tickets);
    }
    return v;
}

void print_result(const Result &r)
{
    std::cout << "== " << r.name << " ==\n";
    std::cout << "Timeline (pid:start->end):\n";
    for (auto &s : r.timeline)
    {
        std::cout << s.pid << ":" << s.start << "->" << s.end << "  ";
    }
    std::cout << "\n\nPer-process metrics:\n";
    std::cout << std::left << std::setw(6) << "PID"
              << std::setw(10) << "Arrival"
              << std::setw(8) << "Burst"
              << std::setw(10) << "Start"
              << std::setw(10) << "Finish"
              << std::setw(10) << "Wait"
              << std::setw(12) << "Turnaround"
              << std::setw(10) << "Response"
              << "\n";
    std::vector<Process> p = r.procs;
    std::sort(p.begin(), p.end(), [](auto &a, auto &b)
              { return a.pid < b.pid; });
    for (auto &x : p)
    {
        std::cout << std::left << std::setw(6) << x.pid
                  << std::setw(10) << x.arrival
                  << std::setw(8) << x.burst
                  << std::setw(10) << x.start_time
                  << std::setw(10) << x.finish_time
                  << std::setw(10) << x.waiting
                  << std::setw(12) << x.turnaround
                  << std::setw(10) << x.response
                  << "\n";
    }
    std::cout << "\nAverages: "
              << "Wait=" << std::fixed << std::setprecision(2) << r.avg_wait
              << ", Turnaround=" << r.avg_turn
              << ", Response=" << r.avg_resp << "\n";
}
