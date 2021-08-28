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
}


// TODO: Return this process's ID
int Process::Pid() const { return pid_; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() const{ 
    long active_jiffies_pid =  LinuxParser::ActiveJiffies(Pid());
    float active_time_pid = float(active_jiffies_pid / sysconf(_SC_CLK_TCK)); // in seconds
    float start_time_pid = float(LinuxParser:: UpTime(Pid()));
    float cpu_usage_proc = float(active_time_pid / start_time_pid);
    return cpu_usage_proc;

}
// TODO: Return the command that generated this process
string Process::Command() {
    string cmd = LinuxParser::Command(Pid()); 
    return cmd;}

// TODO: Return this process's memory utilization
string Process::Ram() { 
    return LinuxParser:: Ram(Pid());
     }

// TODO: Return the user (name) that generated this process
string Process::User() { 
    string user_ = LinuxParser::User(Pid());
    return user_; }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { 
    long uptime_ = LinuxParser::UpTime(Pid());
    return uptime_; }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const { 
    return a.CpuUtilization() < this->CpuUtilization(); }