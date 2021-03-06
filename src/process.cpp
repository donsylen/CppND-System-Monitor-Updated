#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "process.h"
#include "linux_parser.h"

#include <iostream>

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid) {
 pid_ = pid;
 active_time_pid =  LinuxParser::ActiveJiffies(Pid()) / sysconf(_SC_CLK_TCK);
 total_time_pid = LinuxParser:: UpTime(Pid());
 cpu_usage_pid = float(active_time_pid) / float(total_time_pid);
}


// TODO: Return this process's ID
int Process::Pid() const { return pid_; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() const{ 
    return cpu_usage_pid;

}
// TODO: Return the command that generated this process
string Process::Command() {
    return LinuxParser::Command(Pid()); }

// TODO: Return this process's memory utilization
string Process::Ram() { 
    return LinuxParser:: Ram(Pid());
     }

// TODO: Return the user (name) that generated this process
string Process::User() { 
    return LinuxParser::User(Pid()); }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { 
    return LinuxParser::UpTime(Pid());}

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const { 
    return a.CpuUtilization() < this->CpuUtilization(); }