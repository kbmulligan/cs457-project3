#ifndef CORE_H_INCLUDED 
#define CORE_H_INCLUDED

#include <vector>
#include <string>
#include <fstream>
#include <iostream>

// CONSTANTS //////////////////////////////////////////////
const bool VERBOSE = true;
const int MAX_CHARS = 255;
const int MAX_URL_SIZE = MAX_CHARS;
const int BACKLOG = 1;


// FUNCTIONS //////////////////////////////////////////////
std::string get_ip (void);

short read_short (int connectionfd);
int send_short (int connectionfd, short data);
std::string read_string (int connectionfd, int string_len);
int send_string (int connectionfd, std::string str);

#endif

