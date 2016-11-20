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
   
    cout << "Reading string... of length: " << string_length << " on socket: " << connectionfd << endl; 

    char buffer[string_length];
    int status = read(connectionfd, buffer, string_length); 
    if (status < string_length) {
       cout << "read_string didn't read it all! read: " << status << " bytes" << endl; 
    }
    string new_string(buffer);
    return new_string;
}

int send_string (int connectionfd, string str) {

    cout << "Sending string... of length: " << str.size() << " on socket: " << connectionfd << endl; 

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

    cout << "Opening logfile... " << endl;

    fstream ofile(fn.c_str(), ios::out);

    if ( !ofile.is_open() ) {
        cout << "File is not open...probably error opening file" << endl;
    } else {
        ;
    }
    
    return ofile;
}
