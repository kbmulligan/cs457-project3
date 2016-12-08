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
#include <cstring>

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

    // read number of total routers in network
    int num_routers = read_short(manager_cfd);          // number of routers 
    logfile << timestamp() << "Router knows there are " 
            << num_routers << " routers in the network." << endl;
    

    // wait for info from manager
    // read connectivity table
    int n = read_short(manager_cfd);          // number of neighbors 
    for (int i = 0; i < n; i++) {
        unsigned short nid = read_short(manager_cfd);
        unsigned short cost = read_short(manager_cfd);

        router.add_neighbor((int)nid, (int)cost, 0);
    }

    // print connectivity table
    logfile << timestamp() << "Connectivity table:" << endl;
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

    logfile << timestamp() << "Router sending message: " 
            << printable_msg(msg) << endl;
    send_message_tcp(manager_cfd, msg);

    // wait for go build table msg from manager
    short signal = read_short(manager_cfd);
    logfile << timestamp() << "Router(" << id << ") received signal: " 
            << translate_signal(signal) << endl;

    // send link request to neighbors
    
    logfile << timestamp() << "Sending Hello's to neighbors..." << endl;
    for (int n : router.get_neighbors()) {
        unsigned short port = router.get_port_for_neighbor(n);
 
        Message msg;
        msg.src_router = id;
        msg.dst_router = n;
        msg.message = HELLO;

        send_message_udp(port, msg);
        logfile << timestamp() << "HELLO MSG sent from " << id 
                << " to " << n << endl;
    }
    sleep(TRANSMISSION_DELAY);
    

    // receive all ACKs from neighbors

    logfile << timestamp() << "Waiting for ACKs from neighbors..." << endl;
    for (int n : router.get_neighbors()) {
        Message msg = get_message_udp(sockfd);
        logfile << timestamp() << n << " Received: " << printable_msg(msg) << endl;
    }
    logfile << timestamp() << "Received all expected HELLOs from neighbors..." 
            << endl;

    // tell manager all local connections are good
    Message goodcon;
    goodcon.src_router = id;
    goodcon.dst_router = MANAGER_ID;
    goodcon.message = GOOD_CONN;
    send_message_tcp(manager_cfd, goodcon);
    logfile << timestamp() << "Router->Manager: Good local connections" << endl;


    // wait for network is up msg



    // send LSP to neighbors
    for (int n : router.get_neighbors()) {

        int p = router.get_port_for_router(n); 

        int num_conns = router.get_links().size();
        Connection connections[num_conns]; 

        int i = 0;
        for (Connection c : router.get_links()) {
            connections[i] = c;
        }

        Packet lsp;
        lsp.src_router = id;
        lsp.dst_router = n;
        lsp.type = TYPE_CONNECTION_TABLE;
        lsp.bytes = sizeof(connections);
        
        memcpy(&(lsp.data), connections, sizeof(connections)); 
     
        send_udp_packet(p, lsp);

        logfile << timestamp() << "Sent LSP to router " << n << " -- "
                << printable_packet(lsp) << endl;
    }
    sleep(TRANSMISSION_DELAY);
    

    // perform limited broadcast on received LSPs 

    // calc SPT using Dijkstra's algo



    // output SPT table by appending it to log file



    // TEST
    // router.add_connection(9, 10, 5);

    // tell manager routing table complete
    Message goodglobal;
    goodglobal.src_router = id;
    goodglobal.dst_router = MANAGER_ID;
    goodglobal.message = GOOD_TABLE;
    send_message_tcp(manager_cfd, goodglobal);
    logfile << timestamp() << "Router->Manager: Routing table complete" << endl;

    // log forwarding table
    logfile << timestamp() << endl;
    logfile << "----- Forwarding Table -----" << endl;
    logfile << router.get_gateways();




    // wait for packet instructions
    // exit when quit message received
    Packet latest;
    read_udp_packet(sockfd, &latest);
    while (latest.type != QUIT) {

        // log all packet arrivals
        logfile << timestamp() << "Router received message: " 
                << printable_packet(latest) << endl; 

        // only forward updates packets and test packets
        if (latest.type >= TABLE_UPDATE && latest.dst_router != id) {

            // update table if packet calls for it
            if (latest.bytes == TABLE_UPDATE) {
                ;//router.update_table(latest);
            }

            int next_hop = router.get_next_hop(latest.dst_router);
            if (next_hop == -1) {
                logfile << timestamp() << "Unable to forward packet(hop): "
                        << "Router not in connectivity table -- " 
                        << printable_packet(latest) << endl;
            }

            unsigned short dstport = router.get_port_for_neighbor(next_hop);
            if (dstport > 0) {
                send_udp_packet(dstport, latest);
                logfile << timestamp() << "Packet forwarded from " << id
                        << " to " << latest.dst_router << endl;
            } else {
                logfile << timestamp() << "Unable to forward packet(port): "
                        << "Router not in connectivity table -- " 
                        << printable_packet(latest) << endl;
                        
            }
        } else if (latest.dst_router == id) {
            logfile << timestamp() << "PACKET RECEIVED! Packet was for me!" << endl;
            
        }       
        
        // retrieve latest packet 
        read_udp_packet(sockfd, &latest);
    }
    logfile << timestamp() << "Router received QUIT message... " << endl; 

    logfile << log_entry("Routing complete!"); 
    logfile.close(); 

    return 0;
}
