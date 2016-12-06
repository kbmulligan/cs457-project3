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

// SPECIFIC HEADERS /////////////////////////
#include "core.h"
#include "project3.h"

using namespace std;

// LOCAL FUNCTIONS
void usage (int argc, char* argv[]);
vector<string> read_network_config (string fn);
void print_config (vector<string> config);

int wait_for_children (vector<pid_t> pids);

int start_router (int id);
int send_udp_lookup_table(int fd, vector<int> router_ports);
void send_msg_all_tcp (vector<int> routers, vector<int> conns, int msg);
void send_msg_all_udp (vector<int> routers, vector<int> ports, int msg);

// CONSTANTS ///////////////////////////////
const int ARGS = 2;

////////////////////////////////////////////
int main (int argc, char* argv[]) {

    // get options
    string config_fn = "network.conf";

    if (argc != ARGS) {
        usage(argc, argv);
        exit(0);
    } else {
        config_fn = string(argv[1]);
    }

    // start printing
    cout << "manager starting..." << endl;

    // testing 
    cout << "timestamp: " << timestamp() << endl;

    // open log file
    fstream logfile = open_logfile (log_fn_manager);
    logfile << timestamp() << "Logfile opened..." << endl;

    // read input file
    Network network(config_fn);
    network.read_config(config_fn);
    network.print_config();


    int router_count = network.get_nodes();

    // create routers 
    vector<int> routers(router_count, DEFAULT_ID);         // ids
    vector<int> router_ports(router_count, 0);             // udp port
    vector<int> router_connections (router_count, 0);      // management port

    // label routers in range order from 0 to router_count - 1
    iota(routers.begin(), routers.end(), ROUTER_LABEL_START);

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
            logfile << timestamp() << "Router " << router << 
                       " created, with pid: " << pid << endl;

            // track child pid
            router_pids.push_back(pid);


            // do parent process things ... like listen for contact
            // and get UDP port from router
            int listen_port = start_port_manager + router;

            logfile << timestamp() << string("Listening on TCP port: ") <<
                                  to_string(listen_port) << endl;

            router_connections.at(router) = start_listening_TCP (listen_port);
            int connection = router_connections.at(router);
            

            // get connection
            logfile << timestamp() << string("Received connection from socket: ") << 
                                  to_string(connection) << endl;

            // read UDP port for connecting router
            unsigned short udp_port = read_short(connection);
            cout << "Connected router is listening on UDP port: " << udp_port << endl;

            // add UDP port data to directory
            router_ports.at(router) = udp_port;


            // send connectivity table to routers
           
            // get connections from Network object
            vector<Connection> links = network.get_connections_for_node(router);
 
            // connections to follow 
            send_short(connection, (unsigned short)links.size()); 

            for ( Connection c : links ) {
                send_short(connection, c.dst_id);        // neighbor 
                send_short(connection, c.cost);          // cost
            }

            logfile << timestamp() << "Router " << router << 
                       " given local connectivity table..." << endl;
        }

    }


    // do main work depending on client or router
    if ( parent_pid == (long)getpid() ) {
        
        // TODO: send_udp_lookup_table();
        // for ( int router_fd : router_connections ) {
        //     send_udp_lookup_table(router_fd, router_ports);
        // }
        // TODO: Activate receiving code in router process

        // routers have been given connectivity tables
        // routers have been given UDP port lookup table
        // now let them know they can build routing tables
        cout << "Router creation complete... " << endl;
        logfile << log_entry(string("Router creation complete... "));
        for ( int router : routers ) {
            int connection = router_connections.at(router);
            send_short(connection, TABLE_BUILD);
            // send_message_tcp(connection, TABLE_BUILD);
            logfile << timestamp() << "Router " << router << 
                       " instructed to build table..." << endl;
        }

        for ( int r : routers ) {
            int connection = router_connections.at(r);
            Message msg = get_message_tcp(connection);

            // cout << printable_msg(msg) << endl;
            if (msg.message == READY) { 
                logfile << timestamp() << "Router " << r << 
                           " reports READY..." << endl;
            }
        }
        cout << "Routers reporting READY..." << endl;
        logfile << log_entry(string("All routers reporting READY..."));

        for ( int r : routers ) {
            int connection = router_connections.at(r);
            Message msg = get_message_tcp(connection);

            // cout << printable_msg(msg) << endl;
            if (msg.message == GOOD_CONN) { 
                logfile << timestamp() << "Router " << r << 
                           " reports good local connections..." << endl;
            }
        }
        cout << "Routers reporting good connections..." << endl 
             << "Building tables..." << endl;
        logfile << log_entry(string("Routers report good connections..."))
                << timestamp() << "Building tables..." << endl;

        // delay to give routers time to build
        sleep(BUILD_DELAY);

        cout << "Tables complete..." << endl
             << "Starting packet routing..." << endl;
        logfile << endl 
                << log_entry(string("Tables complete..."))
                << timestamp() << "Starting packet routing..." << endl;

        // do packet spawning .........

        vector<string> packets = network.get_packets(); 
        for (string s : packets) {
            vector<string> info = split_string(s);

            int src = stoi(info[0]);
            int dst = stoi(info[1]);
             
            cout << "Transmitting packet from router " 
                 << src << " to " << dst << endl;
            logfile << timestamp()
                    << "Transmitting packet from router " 
                    << src << " to " << dst << endl;

            // tell src router to send packet to dst
            Message sendpacket;
            sendpacket.src_router = src;
            sendpacket.dst_router = dst;
            sendpacket.message = TEST_PACKET;
 
            send_message_udp(router_ports.at(src), sendpacket);
            
        }

        sleep(TRANSMISSION_DELAY);


        // send QUIT to all routers
        logfile << timestamp()
                << "Sending QUIT signal to all routers..." << endl;

        send_msg_all_udp(routers, router_ports, QUIT);
    }


    

    // wait on child processes to complete -- only if parent
    if ( parent_pid == (long)getpid() ) {
        wait_for_children(router_pids);
        logfile << log_entry(string("All child processes have completed."));
        
        // close log file
        logfile << log_entry("Closing logfile...");
        logfile.close();

        cout << "timestamp: " << timestamp() << endl;
        cout << "Goodbye!" << endl;
    }

    return 0;
}

void usage (int argc, char* argv[]) {
    cout << "Usage: " << argv[0] << " <input file>" << endl;
}

void send_msg_all_tcp (vector<int> routers, vector<int> conns, int msg) {

    Message m;
    m.src_router = -1;
    m.dst_router = -1;
    m.message = msg;

    for ( int r : routers ) {
            int c = conns.at(r);
            send_message_tcp(c, m);
    }

    return;
}

void send_msg_all_udp (vector<int> routers, vector<int> ports, int msg) {

    Message m;
    m.src_router = -1;
    m.dst_router = -1;
    m.message = msg;

    for ( int r : routers ) {
            int c = ports.at(r);
            send_message_udp(c, m);
    }

    return;
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

    config_file.getline(buffer, MAX_CHARS); 

    // this part picks up the packet transmissions
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
                if (VERBOSE) {
                    cout << "Process completed: " << finished << ", " << pid << endl;
                }
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

// sends UDP router ports in router order from first to last
int send_udp_lookup_table(int fd, vector<int> router_ports) {

    for (int port : router_ports) {
        send_short(fd, (unsigned short)port);
    } 

    return 0;
}
