// project3.h - Includes data structures for Project 3
// Author: kbmulligan
// CSU
// CS457 - Networks
// Dr. Ray
// Fall 2016

#ifndef PROJECT3HEADER
#define PROJECT3HEADER

#include <vector>
#include <map>
#include <string>
#include <fstream>
#include <iostream>
#include <algorithm>

// CONSTANTS ///////////////////////////////
const int ROUTER_LABEL_START = 0;
const int start_port_manager = 48000;
const int start_port_router = 49000;
const int DEFAULT_ID = 1;
const int MANAGER_ID = -1;
const std::string log_fn_manager = "manager.out";
const std::string log_fn_router = "router.out";

const int BUILD_DELAY = 5;        // sec to wait for routers to build tables
const int TRANSMISSION_DELAY = 2; // sec to wait for routers to send packets

const int PACKET_SIZE = 128;
const int HEADER_SIZE = sizeof(unsigned short) * 4;



// DATA STRUCTURES /////////////////////////

// message AKA packet
// message field is one of:
// 0 = empty
// 1 = ready
// 2 = default / hello
// 3 = quit
// 55 = table update follows (LSP)
// 5 = build table 
// 6 = good local connections
// 7 = test packet (USED FOR SIMULATED PACKETS) 
// 8 = good global connectivity (entries for all routers in tabl)
// 99 = error 
const int EMPTY = 0;
const int READY = 1;
const int HELLO = 2;
const int QUIT = 3;
const int TABLE_BUILD = 5;
const int GOOD_CONN = 6;
const int GOOD_TABLE = 8;
const int TABLE_UPDATE = 55;
const int TEST_PACKET = 100;
const int ERROR = 99;

const int TYPE_PACKET = 70;
const int TYPE_SHORT = 71;
const int TYPE_STRING = 72;
const int TYPE_MESSAGE = 73;
const int TYPE_CONNECTION = 74;
const int TYPE_CONNECTION_TABLE = 75;

// simple message packet format
const unsigned int ITEMS_IN_MESSAGE = 3;
typedef struct _Message {
    int message;
    int src_router;
    int dst_router;
} Message;

const unsigned int ITEMS_IN_CONNECTION = 3;
typedef struct _Connection {
    unsigned short src_id;
    unsigned short dst_id;
    unsigned short cost;
} Connection;

const unsigned int ITEMS_IN_PACKET = 4;
typedef struct _Packet {
    unsigned short src_router;              // source id
    unsigned short dst_router;              // destination id
    unsigned short type;                    // type of data
    unsigned short bytes;                   // size of data
    char data[PACKET_SIZE - HEADER_SIZE];
} Packet;

// router config
class Router {
    int id;
    std::vector<Connection> neighbor_connections;
    std::vector<int> neighbors;
    std::vector<int> costs;
    std::vector<int> ports;
    std::map<int,int> gateways;       // connectivity table
                                     // contains best next-hop given 
                                     // router id as index
    std::map<int,int> global_costs;       // total costs 

public:
    
    Router (int new_id) {
        id = new_id;
    }

    void add_neighbor (int nid, int cost, int port) {
        neighbors.push_back(nid);
        costs.push_back(cost);
        ports.push_back(port);

        Connection n;
        n.src_id = id;
        n.dst_id = nid;
        n.cost = cost;
        neighbor_connections.push_back(n);

        gateways[nid] = nid;
        global_costs[nid] = cost;
    }

    void add_connection (int nid, int cost, int gateway) {
        
        // check current connectivity table for destination id
         


        // compare costs



        // used this connection if cheaper

        gateways[nid] = gateway;
        global_costs[nid] = cost;
        
        return;
    }

    void update_table (Message update) {

        ;

    }

    int get_id () {
        return id;
    }
   
    std::vector<int> get_neighbors() {
        return neighbors;
    }

    std::vector<int> get_costs() {
        return costs;
    }

    std::vector<int> get_ports() {
        return ports;
    }

    std::vector<Connection> get_links () {
        return neighbor_connections;
    }

    int get_cost_for_neighbor (int nid) {
        int n_index = -1;        // find index of nid
        for (unsigned int i = 0; i < neighbor_connections.size(); i++) {
            if (neighbor_connections.at(i).dst_id == nid) {
                n_index = i;
                break;
            }
        }
 
        int n_cost = costs.at(n_index);        // get costs at index
        return n_cost;
    }

    int get_cost_for_dest (int dst) {
        int total = 99;

        std::map<int,int>::iterator it = global_costs.find(dst);
        if (it != gateways.end()) {
            total = global_costs.at(dst);
        }

        return total;
    }

    int get_port_for_neighbor (int nid) {
        int port  = 0;

        std::map<int,int>::iterator it = gateways.find(nid);
        if (it != gateways.end()) {
            port = get_port_for_router(gateways.at(nid));
        }

        return port; 
    }

    int get_port_for_router (int rid) {
        return rid + start_port_router;
    }

    int get_next_hop (int dst) {
        int next = -1;

        std::map<int,int>::iterator it = gateways.find(dst);
        if (it != gateways.end()) {
            next = gateways.at(dst);
        } else {
            next = -1;   
        }

        return next;
    }

    std::string get_gateways () {
        std::string output;
        output.append(std::string("Dest : Next Hop : Cost"));
        output.append("\n");

        for (std::map<int,int>::iterator it = gateways.begin(); 
             it != gateways.end(); ++it) {
            output.append(std::to_string(it->first));
            output.append(std::string("    : ")); 
            output.append(std::to_string(gateways.at(it->first)));
            output.append(std::string(" "));
            output.append(std::string("       : ")); 
            output.append(std::to_string(get_cost_for_dest(it->first))); 
            output.append("\n");
        }

        return output;
    }
 
};

// network class describes and manages network
class Network {

    const int MAX_CHARS = 127;

    std::string fn;
    int nodes;
    int edges;                              // number of edges
    int transmissions;                      // number of packet transmissions 

    std::vector<std::string> config;
    std::vector<std::string> connections;   // edges
    std::vector<std::string> packet_moves;  // transmissions

    std::vector<Connection> links;          // edges

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

    std::vector<std::string> get_packets () {
        return packet_moves;
    }

    std::vector<std::string> get_connections () {
        return connections;
    }


    std::vector<Connection> get_connections_for_node (int node);

    int read_config (std::string filename);

    int print_config () {

        if (config.empty()) {
            std::cout << "CANNOT PRINT: EMPTY CONFIG!" << std::endl;
        } else {
            std::cout << "--- Network configuration ---" << std::endl;
            std::cout << "Nodes: " << nodes << std::endl;

            std::cout << "Edges: " << edges << std::endl;
            for (std::string e : connections) {
                std::cout << e << std::endl;
            }

            std::cout << "Transmissions: " << transmissions << std::endl;
            for (std::string t : packet_moves) {
                std::cout << t << std::endl;
            }
            std::cout << "-----------------------------" << std::endl;
        }

        return 0;
    }
    

};


// FUNCTION ////////////////////////////////

int initialize_router(int data);
int send_message (int conn, Message msg);
std::string printable_msg (Message msg);
std::string printable_packet (Packet msg);
std::string translate_signal (int signal);
Connection create_connection (std::vector<std::string> c);
Connection reverse_connection (Connection c);

int send_message_tcp (int conn, Message msg);
int send_message_udp (unsigned short port, Message msg);
Message get_message_tcp (int conn);
Message get_message_udp (int conn);

int read_udp_data (int sockfd, Packet* packet, int buflen);
int send_udp_data (unsigned short port, void* data, int datalen, 
                   short src, short dst, short type);

int read_udp_packet (int sfd, Packet *packet);
int send_udp_packet (int port, Packet packet);

void print_packet (Packet p);
#endif
