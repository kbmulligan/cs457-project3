// project3.h - Includes data structures for Project 3
// Author: kbmulligan
// CSU
// CS457 - Networks
// Dr. Ray
// Fall 2016

#ifndef PROJECT3HEADER
#define PROJECT3HEADER

#include <string>

// CONSTANTS ///////////////////////////////
const int start_port = 48000;
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

public:
    Network () {
        nodes = 0;
    }

    int read_config (std::string fn) {
    
        return 0;
    }

    

};


// FUNCTION ////////////////////////////////

int initialize_router(int data);

#endif
