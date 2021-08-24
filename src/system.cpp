#include <unistd.h>
#include <cstddef>
#include <set>
#include <string>
#include <vector>

#include "process.h"
#include "processor.h"
#include "system.h"
#include "linux_parser.h"

using std::set;
using std::size_t;
using std::string;
using std::vector;

// TODO: Return the system's CPU  
Processor& System::Cpu() { return cpu_; }

// TODO: Return a container composed of the system's processes
vector<Process>& System::Processes() { 
    
    processes_.clear();
    vector <int> pids = LinuxParser:: Pids();
    for (int pid  : pids)
     {
      Process proc(pid);
      processes_.push_back(proc);
     } 
    return processes_; }

// TODO: Return the system's kernel identifier (string)
std::string System::Kernel() { 
string kernel = LinuxParser::Kernel();
return kernel; }

// TODO: Return the system's memory utilization
float System::MemoryUtilization() { 
    float mem = LinuxParser::MemoryUtilization();
    return mem; }

// TODO: Return the operating system name
std::string System::OperatingSystem() { 
    string os = LinuxParser::OperatingSystem();
    return os;
 }

// TODO: Return the number of processes actively running on the system
int System::RunningProcesses() { 
    int run_proc = LinuxParser::RunningProcesses();
    return run_proc;
 }

// TODO: Return the total number of processes on the system
int System::TotalProcesses() { 
    int totalprocess_ = LinuxParser:: TotalProcesses();
    return totalprocess_; }

// TODO: Return the number of seconds since the system started running
long int System::UpTime() { 
    long uptime_ = LinuxParser::UpTime();
    return uptime_;
 }