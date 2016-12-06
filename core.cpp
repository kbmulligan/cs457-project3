// core.cpp - core of the stepping stone program listens,
// Author: K. Brett Mulligan
// Date: Fall 2016
// CSU - Comp Sci
// CS457 - Networks
// Dr. Indrajit Ray
// License: MIT
// References : Beej's Guide to Socket Programming
//              https://beej.us/guide/bgnet/

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <ctime>
#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <sstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <unistd.h>
#include <vector>

#include "core.h"
#include "project3.h"

using namespace std;


// FUNCTIONS //////////////////////////////////////////////

string get_ip () {
 
    string ip;

    struct addrinfo hints, *res, *p;
    int status;
    char ipstr[INET6_ADDRSTRLEN];

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;


    char hostname[64];

    gethostname(hostname, 64);
    
    //cout << "Getting hostname : " << hostname << endl;
    status = getaddrinfo(hostname,  NULL, &hints, &res); 
    if (status != 0) {
        cerr << "getaddrinfo: " << gai_strerror(status) << endl;
        return string("ERROR: check cerr");
    }

    for (p = res; p != NULL; p = p->ai_next) {
        void *addr;
        string ipver;

        if (p->ai_family == AF_INET) {
            struct sockaddr_in *ipv4 = (struct sockaddr_in *)p->ai_addr;
            addr = &(ipv4->sin_addr);
            ipver = string("IPv4");
        } else {
            struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *)p->ai_addr;
            addr = &(ipv6->sin6_addr);
            ipver = string("IPv6");
        }

        inet_ntop(p->ai_family, addr, ipstr, sizeof(ipstr));
        //cout << ipver << " : " << ipstr << endl;

    }

    freeaddrinfo(res);

    ip = string(ipstr);

    return ip;
}

short read_short (int connectionfd) { 
    short data = 0;
    int bytes_received = recv(connectionfd, &data, sizeof(data), 0);
    if (bytes_received == -1) {
        cout << "read_short: recv returned -1" << endl;
    }
    return ntohs(data);
}

int send_short (int connectionfd, short data) { 
    short net_data = htons(data);
    int bytes_sent = send(connectionfd, &net_data, sizeof(net_data), 0);
    if (bytes_sent == -1) {
        cout << "send_short: send returned -1" << endl;
    }
    return 0; 
}

// reads string_legnth bytes of data from socket connectionfd and returns it as string
string read_string (int connectionfd, int string_length) {
   
    cout << "Reading string... of length: " << string_length <<
            " on socket: " << connectionfd << endl; 

    char buffer[string_length];
    int status = read(connectionfd, buffer, string_length); 
    if (status < string_length) {
       cout << "read_string didn't read it all! read: " << status << " bytes" << endl; 
    }
    string new_string(buffer);
    return new_string;
}

int send_string (int connectionfd, string str) {

    cout << "Sending string... of length: " << str.size() <<
            " on socket: " << connectionfd << endl; 

    int flags = 0;
    char string_data[str.size()];
    strcpy(string_data, str.c_str()); // copy str into string_data location

    int status = send(connectionfd, &string_data, str.size(), flags);
    int size = str.size();
    if ( status == -1 || (status != size) ) {
        cout << "send_string: send failed, status: " << status << endl;
    }
    return 0;
}

string timestamp () {
    const int MAX_SIZE = 64;
    char stamp[MAX_SIZE];
    string format = "%T";

    struct timeval tv;
    time_t t = time(NULL);

    gettimeofday(&tv, NULL);    
    t = tv.tv_sec;    

    strftime(stamp, MAX_SIZE, format.c_str(), gmtime(&t)); 

    string output = string(stamp) + "." + to_string(tv.tv_usec);
    
    // add extra space because aesthetics
    output += ' ';

    return output; 
}

fstream open_logfile (string fn) {

    //cout << "Opening logfile... " << endl;

    fstream ofile(fn.c_str(), ios::out);

    if ( !ofile.is_open() ) {
        cout << "File is not open...probably error opening file" << endl;
    } else {
        ;
    }
    
    return ofile;
}

string log_entry (string entry) {
    return timestamp() + entry + string("\n");
}

int start_listening_TCP (int portreq) {
    return start_listening (portreq, SOCK_STREAM);
}

int start_listening_UDP (int portreq) {
    return start_listening (portreq, SOCK_DGRAM);
}

int start_listening (int portreq, int type) {
    
    string port(to_string(portreq)); 
    struct addrinfo hints, *res, *p;
    int sockfd, connectedfd = -1;

    int returninfo = -1;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = type;            // SOCK_STREAM or SOCK_DGRAM
    hints.ai_flags = AI_PASSIVE;         // use the IP of this machine

    int status = getaddrinfo(NULL, port.c_str(), &hints, &res);
    if (status == -1) {
        cerr << "start_listening error: getaddrinfo" << endl;
        return -1;
    }
    
    int usable_addresses = 0;
    for (p = res; p != NULL; p = p->ai_next) {
        usable_addresses++;
        
        char ipaddrstr[INET6_ADDRSTRLEN];
        inet_ntop(p->ai_family, &((struct sockaddr_in *)(res->ai_addr))->sin_addr, 
                    ipaddrstr, INET6_ADDRSTRLEN); 
        // cout << "Address info : ";
        // cout << ipaddrstr << endl;
    } 

    // cout << "getaddrinfo usable addresses : " << usable_addresses << endl;

    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (status == -1) {
        cerr << "start_listening error: socket" << endl;
        return -1;
    }

    status = bind(sockfd, res->ai_addr, res->ai_addrlen);
    if (status == -1) {
        close(sockfd);
        cerr << "start_listening error: bind" << endl;
        return -1;
    }
    
    // TCP-ONLY STUFF
    if (type == SOCK_STREAM) {
        status = listen(sockfd, BACKLOG);
        if (status == -1) {
            close(sockfd);
            cerr << "start_listening error: listen" << endl;
            return -1;
        }
 
        // cout << "TCP waiting for a connection on " << get_ip() << " : " << port << endl;

        struct sockaddr_storage peeraddr;
        socklen_t peeraddrsize = sizeof(peeraddr);

        connectedfd = accept(sockfd, (struct sockaddr *)&peeraddr, &peeraddrsize);
        if (connectedfd == -1) {
            cerr << "start_listening error: accept" << endl;
            return -1;
        } else {
            char peeraddrstr[INET6_ADDRSTRLEN];
            inet_ntop(AF_INET, &((struct sockaddr_in*) &peeraddr)->sin_addr, 
                        peeraddrstr, INET6_ADDRSTRLEN);
            //cout << "Good connection!" << endl;
            //cout << "Connection from : " << peeraddrstr << endl;

            // cout << "Received connection..." << endl;

            talk_to_client (connectedfd);
            returninfo = connectedfd;
        }

    } 

    // UDP-ONLY STUFF 
    else {
        //cout << "UDP listening on " << get_ip() << " : " << port << endl;
        returninfo = sockfd;
    }
    
    freeaddrinfo(res);

    //close(sockfd);
    //close(connectedfd);

    // return portrequested (UDP) or connection file descriptor (TCP)
    return returninfo;
}


int connect_to (int portreq) {

    string ip = get_ip();
    string port = to_string(portreq);

    if (VERBOSE) {
        cout << "Connecting to..." << endl;
        cout << "SERVER : " << ip << endl;
        cout << "PORT   : " << port << endl;
    }

    struct addrinfo hints, *res;
    int sockfd = -1;

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int status = getaddrinfo(ip.c_str(), port.c_str(), &hints, &res);
    if (status == -1) {
        cerr << "connect_to error: getaddrinfo" << endl;
        return 2;
    }
    
    sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (status == -1) {
        cerr << "connect_to error: socket" << endl;
        return 2;
    }

    status = connect(sockfd, res->ai_addr, res->ai_addrlen);
    if (status == -1) {
        close(sockfd);
        cerr << "connect_to error: connect failed" << endl;
        return 2;
    }

    // done with this addrinfo
    freeaddrinfo(res);
    
    // check if everything's good to go, then start comm
    if (!sockfd) {
        cerr << "connect_to error: sockfd == NULL" << endl;
    }
    
    return sockfd;
}

// pass info from manager back to client
int talk_to_client (int connectedfd) {

    // cout << "Talking to client now..." << endl;


    sleep(0);
    return 0;
}



string printable_msg (Message msg) {
    return  "SRC:" + to_string(msg.src_router) + " " + 
            "DST:" + to_string(msg.dst_router) + " " + 
            "MSG:" + to_string(msg.message);
}

string translate_signal (int signal) {
    return to_string(signal);
}

vector<string> split_string (string input) {
    vector<string> tokens;

    stringstream ss(input);
    string buffer;

    while (ss >> buffer) {
        tokens.push_back(buffer);
    }

    return tokens;
}

void send_connection_data (int socketfd, Connection c) {

    ;

}

int send_udp_short (unsigned short port, short data) {
    

    //cout << "Sending (h): " << data << endl;

    short datasend = htons(data);                     // network byte order

    //cout << "Sending (n): " << datasend << endl;

    send_udp_data(port, &datasend, sizeof(datasend)); 

    return 0;
}

unsigned short read_udp_short (int sockfd) {
    unsigned short data = -1;

    short buffer = 0;
    
    read_udp_data(sockfd, (void *)&buffer, sizeof(buffer));

    //data = *(unsigned short *)buffer;
    memcpy((void *)&data, (void *)&buffer, sizeof(data));
    data = ntohs(data);                     // host bytes order

    return data;
}

int read_udp_data (int sockfd, void* buffer, int buflen) {

    // const int MAX_BYTES = 128;
    // char buffer[MAX_BYTES];
    unsigned int flags = 0;
    struct sockaddr *from = NULL;
    socklen_t *fromlen = NULL;

    int bytes_read = recvfrom(sockfd, buffer, buflen, flags, from, fromlen);
    if (bytes_read < buflen) {
        cerr << "read_udp_data: not all data read" << endl;
    }
    //cout << "read_udp_data: " << buffer << " (" << bytes_read << ")" << endl;
    //cout << "read_udp_data: " << ((char *)buffer)[0] << ((char *)buffer)[1] << endl;

    
    return 0;
}

// this function cheats and uses the first struct returned by getaddrinfo
int send_udp_data (unsigned short port, void* data, int datalen) {

    unsigned int flags = 0;
    //struct sockaddr *to = NULL;
    //socklen_t *tolen = NULL;

    struct addrinfo *servinfo;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_PASSIVE; 

    int status = getaddrinfo(NULL, to_string(port).c_str(), &hints, &servinfo);
    if (status != 0) {
        cerr << "send_udp_data: getaddrinfo error" << endl;
    }

    int sockfd = socket(servinfo->ai_family, 
                        servinfo->ai_socktype, 
                        servinfo->ai_protocol);
    if (sockfd == -1) {
        cerr << "send_udp_data: socket error" << endl;
    }

    int bytes_sent = sendto(sockfd, data, datalen, flags, 
                            servinfo->ai_addr, servinfo->ai_addrlen);

    if (bytes_sent < datalen) {
        cerr << "send_udp_data: not all data sent" << endl;
    }
    //cout << "send_udp_data: " << *(short *)data << endl;    
    //cout << "send_udp_data: " << bytes_sent << " bytes sent" << endl;

    return 0;
}
