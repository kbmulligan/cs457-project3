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
    Router router(id);

    string log_fn = logfile_prefix + to_string(id) + logfile_suffix;
    fstream logfile = open_logfile(log_fn);
    logfile << timestamp() << "Router logfile opened for router: " << id << endl; 

    // create UDP port and listen
    int p = start_port_router + id;
    int sockfd = (unsigned short)start_listening_UDP(p);    
    logfile << timestamp() << "Router listening on port: " << p << endl;


    // artificially wait so that the manager will be ready
    sleep(0);


    // create TCP port for talking with manager 
    unsigned short control_port = start_port_manager + id;
    logfile << log_entry("Connecting to manager...");
    int manager_cfd = connect_to (control_port);
    logfile << log_entry("Connected on socket: " + to_string(manager_cfd));

    // send UDP port number and request node address and connectivity table
    //cout << "Router " << id << " sending data: " << port << endl;
    logfile << timestamp() << "Router sending data to manager: " << p << endl;
    send_short(manager_cfd, p);

    // wait for info from manager
    // read connectivity table
    int n = read_short(manager_cfd);          // number of neighbors 
    for (int i = 0; i < n; i++) {
        unsigned short nid = read_short(manager_cfd);
        unsigned short cost = read_short(manager_cfd);

        router.add_neighbor((int)nid, (int)cost, 0);
    }

    // print connectivity table
    logfile << timestamp() << "Connectivitng table:" << endl;
    for (unsigned int i = 0; i < router.get_neighbors().size(); i++) {
        int id = router.get_neighbors()[i];
        logfile << "ID: " << id 
                << " Cost: " << router.get_cost_for_neighbor(id)
                << " Port: " << router.get_port_for_neighbor(id) << endl;
    }

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
    
    logfile << timestamp() << "Sending Hello's to neighbors..." << endl;
    for (int n : router.get_neighbors()) {
        send_udp_data(router.get_port_for_neighbor(n), 1);
        logfile << timestamp() << "data sent to " << n << " from " << id << endl;
    }
    

    // receive all ACKs from neighbors

    logfile << timestamp() << "Waiting for ACKs from neighbors..." << endl;
    for (int n : router.get_neighbors()) {
        read_udp_data(sockfd);
        cout << "recevied from " << n << endl;
    }



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
