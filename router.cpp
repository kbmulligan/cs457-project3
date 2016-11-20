// router.cpp - router simulates a single router node 
//
// Author: kbmulligan
// CSU
// CS457 - Networks
// Dr. Ray
// Fall 2016

#include <iostream>
#include <string>

#include "core.h"
#include "project3.h"

using namespace std;


const string logfile_prefix = "router";
const string logfile_suffix = ".out";

int initialize_router (int data) {

    cout << "Router iniitialized from router.cpp ... " << data << endl;

    string log_fn = logfile_prefix + to_string(data) + logfile_suffix;
    fstream logfile = open_logfile(log_fn);
    logfile << timestamp() << "Router logfile opened for router: " << data << endl; 

    // do more stuff here and log it


    logfile.close(); 

    return 0;
}
