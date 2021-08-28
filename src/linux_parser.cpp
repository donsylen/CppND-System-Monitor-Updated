#include "linux_parser.h"

#include <dirent.h>
#include <unistd.h>
#include <iostream>

// #include <filesystem>
#include <string>
#include <vector>

using std::stof;
using std::stoi;
using std::string;
using std::to_string;
using std::vector;
// namespace fs = std::experimental::filesystem;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, hostname, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> hostname >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}


// DONE: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  float memTotal = 0.0;
  float memFree = 0.0;
  string line;
  string key;
  string value;

  // read file /proc/meminfo and look for MemTotal and MemFree
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      // remove ' kB' at the end, spaces and :
      // std::remove(line.begin(), line.end(), 'kB');
      std::remove(line.begin(), line.end(), ' ');
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        // search for key memTotal
        if (key == "MemTotal") {
          memTotal = std::stof(value);
        }
        // search for key memFree
        else if (key == "MemFree") {
          memFree = std::stof(value);
          break;
        }
      }
    }
  }

  // Total used memory = (memTotal - MemFree) / memTotal
  return ((memTotal - memFree) / memTotal);
}

// DONE: Read and return the system uptime
long LinuxParser::UpTime() {
  string line;
  string wholeTime;
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> wholeTime) {
        try {
          return std::stol(wholeTime);
        } catch (const std::invalid_argument& arg) {
          return 0;
        }
      }
    }
  }
  return 0;
}

// DONE: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() {
  return LinuxParser::ActiveJiffies() + LinuxParser::IdleJiffies();
}

// DONE: Read and return the number of active jiffies for a PID


long LinuxParser::ActiveJiffies(int pid) {
  string line, token;
  vector<string> values;
  std::ifstream filestream(LinuxParser::kProcDirectory + to_string(pid) +
                           LinuxParser::kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    while (linestream >> token) {
      values.push_back(token);
    }
  }
  long jiffies{0};
  if (values.size() > 21) {
    long user = stol(values[13]);
    long kernel = stol(values[14]);
    long children_user = stol(values[15]);
    long children_kernel = stol(values[16]);
    jiffies = user + kernel + children_user + children_kernel;
  }

  return jiffies;
}
// DONE: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  // Read and return the number of active jiffies for the system
  vector<string> jiffies = LinuxParser:: CpuUtilization();

  return stol(jiffies[CPUStates::kUser_]) + stol(jiffies[CPUStates::kNice_]) +
         stol(jiffies[CPUStates::kSystem_]) + stol(jiffies[CPUStates::kIRQ_]) +
         stol(jiffies[CPUStates::kSoftIRQ_]) + stol(jiffies[CPUStates::kSteal_]) +
         stol(jiffies[CPUStates::kGuest_]) + stol(jiffies[CPUStates::kGuestNice_]);
}

// DONE: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  vector <string> time = CpuUtilization();
  return  stol(time[CPUStates::kIdle_]) + stol(time[CPUStates::kIOwait_]);
}

// DONE: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  vector<string> cpuValues;
  string line;
  string key;
  string vuser, vnice, vsystem, vidle, viowait, virq, vsoftirq, vsteal, vguest,
      vguest_nice;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> vuser >> vnice >> vsystem >> vidle >>
             viowait >> virq >> vsoftirq >> vsteal >> vguest >> vguest_nice) {
        if (key == "cpu") {
          cpuValues.push_back(vuser);
          cpuValues.push_back(vnice);
          cpuValues.push_back(vsystem);
          cpuValues.push_back(vidle);
          cpuValues.push_back(viowait);
          cpuValues.push_back(virq);
          cpuValues.push_back(vsoftirq);
          cpuValues.push_back(vsteal);
          cpuValues.push_back(vguest);
          cpuValues.push_back(vguest_nice);
        }
      }
    }
  }
  return cpuValues;
}
  

int LinuxParser::TotalProcesses() {
  string line;
  string key;
  int processes;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key ) {
        if (key == "processes") {
          linestream >> processes;
          }
        }
      }
    }
  
  return processes;
}


// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  string line;
  string key;
  int processes;
  
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open())
  {
    while (std::getline(stream,line)) {
      std::istringstream linestream(line) ;
      linestream >> key ;
      if (key == "procs_running") {
          linestream >> processes;

        }
      }
    }
    return processes;
 } 

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) { 
  string line;
  string key;
  std::ifstream stream(kProcDirectory + to_string(pid) +kCmdlineFilename);
  if (stream.is_open()){
    std::getline(stream, line);
    }
  return line;
 }

// DONE: Read and return the memory used by a process
string LinuxParser::Ram(int pid) { 
  string line;
  string key;
  string memstr;
  long mem;
  std::ifstream stream(kProcDirectory + to_string(pid) +kStatusFilename);
  if (stream.is_open()){ 
    while (std::getline(stream, line)) { 
      std::istringstream linestream(line);
      while(linestream >> key >> mem) {
        if (key == "VmSize:") {
          mem /= 1024;
          memstr = to_string(mem); 
          break;
          }
        }
      }
    }
  return memstr;  
}

// DONE: Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) {
  // Read and return the user ID associated with a process
  string line, key, uid;
  std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> key;    
      if (key == "Uid:") {
        linestream >> uid;
        break;
      }
    }
  }
  return uid;
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  string uid = LinuxParser::Uid(pid);
  string line;
  string name, other, key;
  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    while (std::getline(stream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> name >> other >> key) {
        if (key == uid) {
          return name;
        }
      }
    }
  }
  return string(); 
  }
// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) { 
  string line;
  string token;
  vector<string> procStat;
  std:: ifstream stream(kProcDirectory + to_string(pid) +kStatusFilename);
  if (stream.is_open()) { 
    while (std::getline(stream, line)) { 
    std::istringstream linestream(line);
     while (linestream >> token)  
     {
       procStat.push_back(token); }
    }

  }
  long uptime_pid = LinuxParser::UpTime() - stol(procStat[21])/sysconf(_SC_CLK_TCK);
  return uptime_pid; 
}
  
  