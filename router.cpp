// router.cpp - router simulates a single router node 
//
// Author: kbmulligan
// CSU
// CS457 - Networks
// Dr. Ray
// Fall 2016

#include <iostream>
#include <string>
#include <unistd.h>

#include "core.h"
#include "project3.h"

using namespace std;


const string logfile_prefix = "router";
const string logfile_suffix = ".out";

int initialize_router (int data) {

    int id = data;

    //cout << "Router iniitialized from router.cpp ... " << id << endl;

    string log_fn = logfile_prefix + to_string(id) + logfile_suffix;
    fstream logfile = open_logfile(log_fn);
    logfile << timestamp() << "Router logfile opened for router: " << id << endl; 

    // create UDP port and listen
    unsigned short port = (unsigned short)start_listening_UDP (start_port_router + id);    
    logfile << timestamp() << "Router listening on port: " << port << endl;


    // artificially wait so that the manager will be ready
    sleep(0);


    // create TCP port for talking with manager 
    unsigned short control_port = start_port_manager + id;
    logfile << log_entry("Connecting to manager...");
    int manager_cfd = connect_to (control_port);
    logfile << log_entry("Connected on socket: " + to_string(manager_cfd));

    // send UDP port number and request node address and connectivity table
    //cout << "Router " << id << " sending data: " << port << endl;
    logfile << timestamp() << "Router sending data to manager: " << port << endl;
    send_short(manager_cfd, port);

    // wait for info from manager
    



    // send ready msg
    Message msg;
    msg.src_router = id;
    msg.dst_router = MANAGER_ID;    
    msg.message = READY;

    logfile << timestamp() << "Router sending message: " << printable_msg(msg) << endl;
    send_message(manager_cfd, msg);

    // wait for go msg from manager
    short signal = read_short(manager_cfd);
    cout << "Router(" << id << ") received signal: " 
         << translate_signal(signal) << endl;

    // send link request to neighbors


    // receive all ACKs from neighbors

    // tell manager all connections are good


    // wait for network is up msg


    // send LSP to neighbors


    // perform limited broadcast on received LSPs 

    // calc SPT using Dijkstra's algo

    // output SPT table by appending it to log file

    // update forwarding table

    
    logfile << log_entry("Routing complete!"); 
    logfile.close(); 

    return 0;
}
