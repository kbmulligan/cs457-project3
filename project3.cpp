#include <vector> 

#include "core.h"
#include "project3.h"
using namespace std;

vector<Connection> Network::get_connections_for_node (int node) {
    vector<Connection> list;
    
    for (Connection c : links) {
        if (c.src_id == node) {
            list.push_back(c);
        }
    }

    return list;
}

int Network::read_config (string filename) {

    if (filename.empty()) {
        ;
    } else {
        fn = filename;
    } 

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
        config.push_back(string(buffer));
        connections.push_back(string(buffer));
        config_file.getline(buffer, MAX_CHARS); 
    }

    nodes = stoi(config[0]);                   // get number nodes in network

    connections.erase(connections.begin());    // remove first line (num nodes)
    edges = connections.size();                // calc number of edges

    for (string conn : connections) {
        Connection c = create_connection(split_string(conn));
        links.push_back(c);                     // add connection
        links.push_back(reverse_connection(c)); // add reverse connection also
    }

    config_file.getline(buffer, MAX_CHARS); 

    // this part picks up the packet transmissions
    while ( string(buffer) != string("-1") ) {
        //cout << buffer << endl;
        config.push_back(string(buffer));
        packet_moves.push_back(string(buffer));
        config_file.getline(buffer, MAX_CHARS); 
    }

    transmissions = packet_moves.size();

    config_file.close();

    return 0;
}

// FUNCTIONS
Connection create_connection (vector<string> c) {

    Connection conn;

    if (c.size() != ITEMS_IN_CONNECTION) {
        cerr << "create_connection error: incorrect number of string elements"
             << endl;
    } else {
        unsigned short src_id = (unsigned short)stoi(c[0]);    
        unsigned short dst_id = (unsigned short)stoi(c[1]);    
        unsigned short cost = (unsigned short)stoi(c[2]);    

        conn.src_id = src_id;
        conn.dst_id = dst_id;
        conn.cost = cost; 
    }

    return conn;
}

Connection reverse_connection (Connection c) {
    Connection reversed;
    reversed.src_id = c.dst_id;
    reversed.dst_id = c.src_id;
    reversed.cost = c.cost;

    return reversed;
}





// send message sends 3 shorts that make up a message
int send_message_tcp (int conn, Message msg) {

    send_short(conn, (short)msg.src_router);
    send_short(conn, (short)msg.dst_router);
    send_short(conn, (short)msg.message);

    return 0;
}

// get message reads 3 shorts that make up a message
Message get_message_tcp (int conn) {

    short src = read_short(conn);
    short dst = read_short(conn);
    short msg = read_short(conn);

    Message m;
    m.src_router = src;
    m.dst_router = dst;
    m.message = msg;

    return m;
}

int send_message_udp (unsigned short port, Message msg) {

    send_udp_short(port, (short)msg.src_router);
    send_udp_short(port, (short)msg.dst_router);
    send_udp_short(port, (short)msg.message);

    return 0;
}

Message get_message_udp (int conn) {

    short src = read_udp_short(conn);
    short dst = read_udp_short(conn);
    short msg = read_udp_short(conn);

    Message m;
    m.src_router = src;
    m.dst_router = dst;
    m.message = msg;

    return m;
}
