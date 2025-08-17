#include "io.h"
#include <iostream>
#include <string>

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cerr << "Usage:\n"
                  << "  ./schedsim FIFO\n"
                  << "  ./schedsim SJFnp\n"
                  << "  ./schedsim SJFp\n"
                  << "  ./schedsim RR <quantum>\n"
                  << "  ./schedsim Lottery [seed]\n"
                  << "  ./schedsim MLFQ\n";
        return 1;
    }

    std::string algo = argv[1];
    auto procs = read_processes_from_stdin();

    if (algo == "FIFO")
    {
        print_result(schedule_fifo(procs));
    }
    else if (algo == "SJFnp")
    {
        print_result(schedule_sjf_np(procs));
    }
    else if (algo == "SJFp")
    {
        print_result(schedule_sjf_p(procs));
    }
    else if (algo == "RR")
    {
        if (argc < 3)
        {
            std::cerr << "RR requires quantum\n";
            return 1;
        }
        int q = std::stoi(argv[2]);
        print_result(schedule_rr(procs, q));
    }
    else if (algo == "Lottery")
    {
        int seed = (argc >= 3) ? std::stoi(argv[2]) : 1;
        print_result(schedule_lottery(procs, seed));
    }
    else if (algo == "MLFQ")
    {
        print_result(schedule_mlfq(procs));
    }
    else
    {
        std::cerr << "Unknown algorithm\n";
        return 1;
    }
    return 0;
}
