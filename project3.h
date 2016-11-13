// project3.h - Includes data structures for Project 3
// Author: kbmulligan
// CSU
// CS457 - Networks
// Dr. Ray
// Fall 2016

#include <string>

#ifndef PROJECT3HEADER
#define PROJECT3HEADER

const int DEFAULT_ID = 1;
const int MAX_CHARS = 255;
const std::string log_fn_manager = "manager.log";
const std::string log_fn_router = "manager.log";

class Network {

    int nodes;
    

public:
    Network () {
        nodes = 0;
    }

    int read_config (std::string fn) {
    
        return 0;
    }

};

#endif
