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
        links.push_back( create_connection(split_string(string(buffer))) );
        config_file.getline(buffer, MAX_CHARS); 
        
    }

    nodes = stoi(config[0]);

    connections.erase(connections.begin());
    edges = connections.size();

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
