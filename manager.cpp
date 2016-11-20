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
#include "core.h"

using namespace std;

// LOCAL FUNCTIONS
vector<string> read_network_config (string fn);
void print_config (vector<string> config);

int wait_for_children (vector<pid_t> pids);

int start_router (int id);

////////////////////////////////////////////
int main (int argc, char* argv[]) {

    cout << "manager starting..." << endl;

    // testing 
    cout << "timestamp test: " << timestamp() << endl;
    cout << "timestamp test: " << timestamp() << endl;
    cout << "timestamp test: " << timestamp() << endl;
    cout << "timestamp test: " << timestamp() << endl;

    // open log file
    fstream logfile = open_logfile (log_fn_manager);
    logfile << timestamp() << "Logfile opened..." << endl;

    // read input file
    string config_fn = "network.conf";
    vector<string> config = read_network_config(config_fn);

    // show input file
    print_config(config);

    // parse input file
    // parse_config(config);




    int router_count = 3;

    // create routers 
    vector<int> routers(router_count, DEFAULT_ID);

    // label routers in range order from 0 to router_count - 1
    iota(routers.begin(), routers.end(), 0);

    vector<pid_t> router_pids;


    pid_t parent_pid = (long)getpid();

    pid_t pid = 0;
    // start child router processes
    for ( int router : routers ) {
        pid = fork();

        if (pid == 0) {
            if (VERBOSE) {
                cout << "I'm the child!" << endl;
                cout << "Router: " << router << endl;
            }
            
            // do child process things ...
            start_router(router);

            break;
        } else {
            if (VERBOSE) {
                cout << "I'm the parent!" << endl;
            }
            cout << "Router " << router << " created, with pid: " << pid << endl;

            // track child pid
            router_pids.push_back(pid);


            // do parent process things ...


        }

    }


    // do main work depending on client or router
    if ( parent_pid == (long)getpid() ) {
        cout << "Router creation complete... Starting router management..." << endl;

        // do router management .........



    }



    // close log file
    logfile.close();

    // wait on child processes to complete -- only if parent
    if ( parent_pid == (long)getpid() ) {
        wait_for_children(router_pids);

        cout << "timestamp test: " << timestamp() << endl;
        cout << "Goodbye!" << endl;
    }

    return 0;
}

vector<string> read_network_config(string fn) {

    cout << "Reading network config file..." << endl;

    // open file
    fstream config_file(fn, ios::in); 

    if ( !config_file.is_open() ) {
        cout << "File is not open...probably error opening file" << endl;
    } else {
        ;
    }
    
    // read data
    vector<string> config_data;
    char buffer[MAX_CHARS];
    config_file.getline(buffer, MAX_CHARS);
    while ( string(buffer) != string("-1") ) {
        cout << buffer << endl;
        config_data.push_back(string(buffer));
        config_file.getline(buffer, MAX_CHARS); 
    }


    config_file.close();

    return config_data;
}

void print_config (vector<string> config) {

    if (config.empty()) {
        cout << "CANNOT PRINT: EMPTY CONFIG!" << endl;
    } else {
        cout << "--- Network configuration ---" << endl;
        for (string s : config) {
            cout << s << endl;
        }
        cout << "Nodes: " << config[0] << endl;
        cout << "Edges (well, size - 1): " << config.size() - 1 << endl;
        cout << "-----------------------------" << endl;
    }
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

// router needs to learn its ID and port
int start_router (int data) {

    //cout << "Router started: " << data << endl;

    initialize_router (data);                         // from router.cpp

    // make parent wait a bit for child to quit
    sleep(1);

    return 0;
}
