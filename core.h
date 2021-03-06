// core.h - Includes data structures for Project 3
// Author: kbmulligan
// CSU
// CS457 - Networks
// Dr. Ray
// Fall 2016

#ifndef CORE_H_INCLUDED 
#define CORE_H_INCLUDED

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

// CONSTANTS //////////////////////////////////////////////
const bool VERBOSE = false;
const int MAX_CHARS = 127;
const int MAX_URL_SIZE = MAX_CHARS;
const int BACKLOG = 1;


// FUNCTIONS //////////////////////////////////////////////
std::string get_ip (void);
std::string timestamp (void);
std::string log_entry (std::string entry);

short read_short (int connectionfd);
int send_short (int connectionfd, short data);
std::string read_string (int connectionfd, int string_len);
int send_string (int connectionfd, std::string str);

unsigned short read_udp_short (int sockfd);
int send_udp_short (unsigned short port, short data, short src, short dst);
int read_udp_string (int sockfd, std::string output);
int send_udp_string (unsigned short port, std::string str, short src, short data);

std::fstream open_logfile (std::string fn);

int start_listening_TCP (int portreq);
int start_listening_UDP (int portreq);
int start_listening (int portreq, int type);
int connect_to (int portreq);
int talk_to_client (int connectedfd);


std::vector<std::string> split_string (std::string input);

#endif

