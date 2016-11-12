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


// PROCESS MANAGEMENT HEADERS //////////////
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#include "project3.h"

using namespace std;

// LOCAL FUNCTIONS
int read_network_config(string fn);

////////////////////////////////////////////
int main (int argc, char* argv[]) {

    cout << "manager starting..." << endl;

    // read input file
    string config_fn = "default.config";
    read_network_config(config_fn);

    int router_count = 3;

    vector<int> routers(router_count, DEFAULT_ID);
    list<pid_t> router_pids;


    pid_t parent_pid = (long)getpid();

    pid_t pid = 0;
    // start child router processes
    for ( int router : routers ) {
        pid = fork();

        if (pid == 0) {
            cout << "I'm the child!" << endl;
            cout << "Router: " << router << endl;

            
            // do child process things ...


            break;
        } else {
            cout << "I'm the parent!" << endl;
            cout << "Router " << router << " created, with pid: " << pid << endl;

            // track child pid
            router_pids.push_back(pid);


            
            // do parent process things ...



        }

    }

    //cout << "There are " << router_pids.size() << " router pids." << endl;




    // wait on child processes to complete
    int status = 0;
    while ( !router_pids.empty() && parent_pid == (long)getpid()) {
        pid_t finished = wait(&status);

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
            router_pids.remove(finished);
            cout << "Process completed: " << finished << endl;
        }

    }
    cout << "All child prcesses have terminated." << endl;


    return 0;
}

int read_network_config(string fn) {

    cout << "Reading network config file..." << endl;

    return 0;
}
