// project3.h - Includes data structures for Project 3
// Author: kbmulligan
// CSU
// CS457 - Networks
// Dr. Ray
// Fall 2016

#ifndef PROJECT3HEADER
#define PROJECT3HEADER

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

// CONSTANTS ///////////////////////////////
const int start_port_manager = 48000;
const int start_port_router = 49000;
const int DEFAULT_ID = 1;
const std::string log_fn_manager = "manager.out";
const std::string log_fn_router = "router.out";


// DATA STRUCTURES /////////////////////////
typedef struct _Message {
    int message;
    int src;
    int dest;
} Message;


class Network {

    std::string fn;
    int nodes;
    int edges;    
    int transmissions;    

    const int MAX_CHARS = 127;
    std::vector<std::string> config;

public:
    Network (std::string filename) {
        fn = filename;
        nodes = 0;
        edges = 0;
        transmissions = 0;
    }

    int get_nodes () {
        return nodes;
    }

    int get_edges () {
        return edges;
    }

    int get_transmissions () {
        return transmissions;
    }

    int read_config (std::string filename) {

        if (filename.empty()) {
            ;
        } else {
            fn = filename;
        } 
    
        std::cout << "Reading network config file..." << std::endl;

        // open file
        std::fstream config_file(fn, std::ios::in); 

        if ( !config_file.is_open() ) {
            std::cout << "File is not open...probably error opening file" << std::endl;
        } else {
            ;
        }
        
        // read data
        char buffer[MAX_CHARS];
        config_file.getline(buffer, MAX_CHARS);
        while ( std::string(buffer) != std::string("-1") ) {
            std::cout << buffer << std::endl;
            config.push_back(std::string(buffer));
            config_file.getline(buffer, MAX_CHARS); 
        }


        nodes = std::stoi(config[0]);
        edges = config.size() - 1;

        config_file.getline(buffer, MAX_CHARS); 

        // this part picks up the packet transmissions
        while ( std::string(buffer) != std::string("-1") ) {
            std::cout << buffer << std::endl;
            config.push_back(std::string(buffer));
            config_file.getline(buffer, MAX_CHARS); 
        }

        transmissions = config.size() - edges - 1;

        config_file.close();
        return 0;
    }

    int print_config () {

        if (config.empty()) {
            std::cout << "CANNOT PRINT: EMPTY CONFIG!" << std::endl;
        } else {
            std::cout << "--- Network configuration ---" << std::endl;
            for (std::string s : config) {
                std::cout << s << std::endl;
            }
            std::cout << "Nodes: " << nodes << std::endl;
            std::cout << "Edges: " << edges << std::endl;
            std::cout << "Transmissions: " << transmissions << std::endl;
            std::cout << "-----------------------------" << std::endl;
        }

        return 0;
    }
    

};


// FUNCTION ////////////////////////////////

int initialize_router(int data);

#endif
