#ifdef READ_H
#define READ_H

#include <cctype>
#include <cstdio>
#include <cstdlib>
#include <ios>
#include <iostream>
#include <ostream>
#include <string>
#include <fstream>
#include <sstream>
#include <dirent.h>
#include <cstring>
#include <sys/stat.h>

class Read {
public:
    struct _Process {
        int pid;
        std::string process_name;
        long int mem;
    };

    static _Process read_process(int pid) {
        _Process process;

        std::string path = "/proc/" + std::to_string(pid) + "/comm";
        std::ifstream file(path);
        if (!file) {
            std::cerr << "Unable to open file!" << std::endl;
            exit(1);
        }

        getline(file, process.process_name);

        process.pid = pid;
        process.mem = get_mem(pid);

        return process;
    }

    static long int get_mem(int pid) {
        long int mem_usage = 0;

        std::string path = "/proc/" + std::to_string(pid) + "/status";
        std::ifstream file(path);
        if (!file) {
            std::cerr << "Failed to open dir " << std::endl;
            exit(1);
        }
        std::string line;
        while (std::getline(file, line)) {
            if (line.rfind("VmRSS:", 0) == 0) {
                std::istringstream iss(trim(line.substr(6)));
                long kb;
                iss >> kb;
                // process.mem = kb * 1024; you can directly use this if you want !
                mem_usage = kb * 1024;
                break;
            }
        }

        return mem_usage;
    }

    static std::string trim(const std::string& str) {
        size_t start = str.find_first_not_of(" \t");
        return (start != std::string::npos) ? str.substr(start) : "";
    }

    static bool is_all_digit(const std::string &name){
        for (int i = 0; name[i]; ++i) {
            if (!std::isdigit(static_cast<unsigned char>(name[i]))) {
                return false;
            
            }
        
        }
        return true;
    }

    static bool is_dir(const std::string &path){
        struct stat st;
        return stat(path.c_str(), &st) == 0 && S_ISDIR(st.st_mode);
    }

    static int get_by_name(std::string name){
        DIR *dir = opendir("/proc/");
       

        if (dir == nullptr || !dir) {
            std::cerr << "Unable to open directory";
            exit(1);
        }

        dirent *entry;
    
        while ((entry = readdir(dir)) != nullptr) {
            std::string proc_path = entry -> d_name;

            if (strcmp(entry -> d_name, ".") == 0 || strcmp(entry -> d_name, "..") == 0) {
                continue;
            }

            if (is_all_digit(entry->d_name) == true) {
                std::string full_path = "/proc/" + proc_path;
                std::string name_path = "/proc/" + proc_path + "/comm";
                if (is_dir(proc_path) == true) {
                    std::ifstream file(name_path);

                    if (!file) {
                        std::cerr << "Unable to open file \n" << full_path << std::endl;
                        exit(1);
                    }

                    std::string line;
                    getline(file, line);

                    if (line == name) {
                        std::cout << "\n<----------Found---------->\n";
                        std::cout << "Process_name : " << name;
                        std::cout << "Pid : " << proc_path << std::endl;
                        return std::stoi(proc_path);
                    }
                }
            
            }
            
            
        }
        closedir(dir);
        return -1;
    }
};

#endif
