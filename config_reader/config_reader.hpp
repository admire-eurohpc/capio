#include <unordered_map>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>

#ifndef CONFIG_READER_AAA_H
#define CONFIG_READER_AAA_H

typedef std::unordered_map<std::string, std::unordered_map<int, int>> config_type;

config_type get_deployment_config(std::string path) {
    config_type config;
    std::string line;
    std::ifstream myfile(path);
    int node = -1;
    std::string app = "";
    if (myfile.is_open()) {
        while ( getline (myfile,line) ) {
            if (line.substr(0, 2) == "  ") {
                if (line.substr(0, 4) == "    ") {
                    config[app][node]= std::stoi(line.substr(6, line.length() - 6));
                }
                else {
                    node = std::stoi(line.substr(4, line.length() - 4));
                }
            }
            else {
                app = line;
                app.pop_back();
            }
        }
        myfile.close();
    }
    else
        std::cout << "Unable to open file" << std::endl;
    return config;
}

#endif //CONFIG_READER_AAA_H