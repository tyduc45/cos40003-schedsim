#pragma once
#include "Process.h"
#include <vector>
#include <string>

std::vector<Process> read_processes_from_stdin();
void print_result(const Result &r);