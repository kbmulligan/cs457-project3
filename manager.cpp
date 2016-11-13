// manager.cpp - manager reads the input file and starts the router processes
//
// Author: kbmulligan
// CSU
// CS457 - Networks
// Dr. Ray
// Fall 2016

// DATA STRUCTURE HEADERS //////////////////
#include <iostream>
#include <vector>
#include <list>
#include <string>
#include <numeric>
#include <fstream>

// PROCESS MANAGEMENT HEADERS //////////////
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "project3.h"

using namespace std;

// LOCAL FUNCTIONS
int read_network_config(string fn);
int wait_for_children (vector<pid_t> pids);

////////////////////////////////////////////
int main (int argc, char* argv[]) {

    cout << "manager starting..." << endl;

    // read input file
    string config_fn = "network.conf";
    read_network_config(config_fn);

    int router_count = 3;

    // create and label routers in order
    vector<int> routers(router_count, DEFAULT_ID);
    iota(routers.begin(), routers.end(), 0);

    vector<pid_t> router_pids;


    pid_t parent_pid = (long)getpid();

    pid_t pid = 0;
    // start child router processes
    for ( int router : routers ) {
        pid = fork();

        if (pid == 0) {
            cout << "I'm the child!" << endl;
            cout << "Router: " << router << endl;

            
            // do child process things ...

            // make parent wait a bit for child to quit
            sleep(1);

            break;
        } else {
            cout << "I'm the parent!" << endl;
            cout << "Router " << router << " created, with pid: " << pid << endl;

            // track child pid
            router_pids.push_back(pid);


            
            // do parent process things ...



        }

    }



    // wait on child processes to complete -- only if parent
    if ( parent_pid == (long)getpid() ) {
        wait_for_children(router_pids);
    }
    cout << "Goodbye!" << endl;

    return 0;
}

int read_network_config(string fn) {

    cout << "Reading network config file..." << endl;

    // open file
    fstream config_file(fn, ios::in); 

    if ( !config_file.is_open() ) {
        cout << "File is not open...probably error opening file" << endl;
    } else {
        ;
    }
    
    // read data
    char buffer[MAX_CHARS];
    config_file.getline(buffer, MAX_CHARS);
    while ( string(buffer) != string("-1") ) {
        cout << buffer << endl;
        config_file.getline(buffer, MAX_CHARS); 
    }


    config_file.close();

    return 0;
}

fstream open_logfile (string fn) {

    cout << "Opening logfile... " << endl;

    fstream ofile(fn.c_str(), ios::out | ios::app);

    if ( !ofile.is_open() ) {
        cout << "File is not open...probably error opening file" << endl;
    } else {
        ;
    }
    
    return ofile;
}

int wait_for_children (vector<pid_t> pids) {

    int status = 0;
    if ( pids.empty() ) {
        cout << "wait_for_children: empty pid list" << endl;
    } else { 
        for (pid_t pid : pids) {
            pid_t finished = waitpid(pid, &status, 0);

            if (finished == -1) {
                switch (errno) {
                    case ECHILD:
                        cout << "wait error: ECHILD" << endl;
                        break;
                    case EINTR:
                        cout << "wait error: EINTR" << endl;
                        break;
                    case EINVAL:
                        cout << "wait error: EINVAL" << endl;
                        break;
                    default:
                        cout << "wait error: unknown" << endl;
                        break;
                }
            } else {
                cout << "Process completed: " << finished << ", " << pid << endl;
            }

        }
    }

    cout << "All child prcesses have terminated." << endl;
    return 0;
}
