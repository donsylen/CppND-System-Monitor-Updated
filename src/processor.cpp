#include "processor.h"
#include <vector>
#include <string>
#include "linux_parser.h"

using std:: vector;
using std:: string;

// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
    vector<string> cpuValues = LinuxParser::CpuUtilization();
    float user = stof(cpuValues[LinuxParser::CPUStates::kUser_]);
    float nice = stof(cpuValues[LinuxParser::CPUStates::kNice_]);
    float system = stof(cpuValues[LinuxParser::CPUStates::kSystem_]);
    float idle = stof(cpuValues[LinuxParser::CPUStates::kIdle_]);
    float iowait = stof(cpuValues[LinuxParser::CPUStates::kIOwait_]);
    float irq = stof(cpuValues[LinuxParser::CPUStates::kIRQ_]);
    float softirq = stof(cpuValues[LinuxParser::CPUStates::kSoftIRQ_]);
    float steal = stof(cpuValues[LinuxParser::CPUStates::kSteal_]);
    float guest = stof(cpuValues[LinuxParser::CPUStates::kGuest_]);
    float guestnice = stof(cpuValues[LinuxParser::CPUStates::kGuestNice_]);

    float PrevIdle = 0.0;
    float Idle = idle + iowait;
    float PrevNonIdle = 0.0;
    float NonIdle = user + nice + irq + softirq + system + steal;
    float PrevTotal = 0.0;
    float Total = Idle + NonIdle;
    //differentiate: actual minus initial
    float totald = Total - PrevTotal;
    float idled = Idle - PrevIdle;
    //calculate CPU Utilization
    float CPU_Percentage = (totald - idled) / totald; 
    return CPU_Percentage;
}
    

 


