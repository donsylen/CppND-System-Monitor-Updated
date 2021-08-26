#include <dirent.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

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
  string os, version, kernel;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
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


// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  string line;
  string key{""};
  string val{""};
  string total_mem{""};
  string free_mem{""};
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> val) {
        if (key == "MemTotal:") total_mem = val;
        if (key == "MemFree:") free_mem = val;
      }
    }
  }

  return (std::stof(total_mem) - std::stof(free_mem)) / std::stof(total_mem);
}
 

// TODO: Read and return the system uptime
long LinuxParser::UpTime() { 
  string line,str;
  string  uptime;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  
  while (stream.is_open()) {
    std::getline(stream,line);
    std::istringstream linestream (line);
    linestream >> uptime ;
   }
  
    return stol(uptime);
  }


// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  return LinuxParser::ActiveJiffies() + LinuxParser::IdleJiffies();
}
  
// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) { 
  
  string line, token;
  long activejiffies_proc, user, kernel, children_user, children_kernel;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
   if (stream.is_open()) {
     while (std:: getline(stream,line)) {
       std:: istringstream linestream(line);
       int i = 0;
       while (linestream >> token) 
       { if (i == 13) { 
         user = stol(token);}
         else if (i == 14) {
          kernel = stol(token);}
         else if (i == 15) { 
          children_user = stol(token);}
         else if (i == 16) {
          children_kernel = stol(token);}
          i++;
          }
       activejiffies_proc = user + kernel + children_user + children_kernel; 
       }
    }
   return activejiffies_proc;
}

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() {
  // Read and return the number of active jiffies for the system
  vector<string> jiffies = LinuxParser:: CpuUtilization();

  return stol(jiffies[CPUStates::kUser_]) + stol(jiffies[CPUStates::kNice_]) +
         stol(jiffies[CPUStates::kSystem_]) + stol(jiffies[CPUStates::kIRQ_]) +
         stol(jiffies[CPUStates::kSoftIRQ_]) + stol(jiffies[CPUStates::kSteal_])+
         stol(jiffies[CPUStates::kGuest_]) + stol(jiffies[CPUStates::kGuestNice_]);
         
}


  
// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  vector <string> time = CpuUtilization();
  return  (stol(time[CPUStates::kIdle_]) + stol(time[CPUStates::kIOwait_]));

}

// TODO: Read and return CPU utilization
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
  

             
         
// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  string line;
  string key;
  string val;
  int num;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open())
  {
    while (std::getline(stream,line)) {
       std::istringstream linestream(line);
       linestream >> key >>val;
        if (key == "processes") {
          num =  stoi(val);
          break;
        }
      }
  }
  return num;
 }

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  string line;
  string key;
  string val;
  int num;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open())
  {
    while (std::getline(stream,line)) {
      std::istringstream linestream(line) ;
      linestream >> key >> val;
      if (key == "runningprocesses") {
          num = stoi(val);
          break;
        }
      }
    }
  return num;
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

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
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

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) { 
  string line;
  string key;
  string value;
  string uid;
  
  std::ifstream stream(kProcDirectory + to_string(pid) +kStatusFilename);
  if (stream.is_open()){ 
    while (std::getline(stream, line)) { 
      std::istringstream linestream(line);
      while(linestream >> key >> value); 
      if (key == "Uid:") {
        uid = value;
        break; 
        }
      }
   }
 return uid;
}  

 
// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) { 
 
  string line;
  string value, uid;
  string user;
  std::ifstream stream(kPasswordPath);
  if (stream.is_open())
  {
    while (std::getline(stream, line)) { 
      std::replace(line.begin(),line.end(),'x', ' ');
      std::replace(line.begin(),line.end(),':', ' ');
      std::istringstream linestream(line);
      while (linestream >> value >> uid); 
      if (uid == LinuxParser::Uid(pid)) {

        user = value;
      }
     }
   } 
   return user;
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
  
  return stol(procStat[21])/sysconf(_SC_CLK_TCK); 
}
  