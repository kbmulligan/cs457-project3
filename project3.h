// project3.h - Includes data structures for Project 3
// Author: kbmulligan
// CSU
// CS457 - Networks
// Dr. Ray
// Fall 2016

#ifndef PROJECT3HEADER
#define PROJECT3HEADER

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

// CONSTANTS ///////////////////////////////
const int ROUTER_LABEL_START = 0;
const int start_port_manager = 48000;
const int start_port_router = 49000;
const int DEFAULT_ID = 1;
const int MANAGER_ID = -1;
const std::string log_fn_manager = "manager.out";
const std::string log_fn_router = "router.out";

const int BUILD_DELAY = 1;        // sec to wait for routers to build tables
const int TRANSMISSION_DELAY = 1; // sec to wait for routers to send packets

// DATA STRUCTURES /////////////////////////

// message AKA packet
// message field is one of:
// 0 = empty
// 1 = ready
// 2 = default / hello
// 3 = quit
// 4 = table update follows 
// 5 = build table 
// 99 = error 
const int EMPTY = 0;
const int READY = 1;
const int HELLO = 2;
const int QUIT = 3;
const int TABLE_UPDATE = 4;
const int TABLE_BUILD = 4;
const int ERROR = 99;

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


// router config
class Router {
    int id;
    std::vector<Connection> neighbor_connections;
    std::vector<int> neighbors;
    std::vector<int> costs;
    std::vector<int> ports;

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

    int get_port_for_neighbor (int nid) {
        return nid + start_port_router;
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
std::string translate_signal (int signal);
Connection create_connection (std::vector<std::string> c);
Connection reverse_connection (Connection c);

#endif
