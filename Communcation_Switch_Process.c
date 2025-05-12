#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>

#define MAX_STATIONS 10

// Global array to store client data
int clientDataList[MAX_STATIONS];
// File pointer for the output file
FILE *outfile;

// Function prototype for processing client requests
void doprocessing(int sock);

int main(int argc, char *argv[])
{   
    // Variable declarations
    char buffer[256];
    int fd_sock, fd_sock_new, nonum, cyll;
    int n, id_p, i, i_max;
    struct sockaddr_in serv_addr, cli_addr;

    if (argc != 2)
    {
        // Check if the correct number of command-line arguments is provided
        fprintf(stderr, "\n Please give : ./CSP <port number> \n");
        exit(1);
    }
   
   // Convert port number from command line argument to integer
    nonum = atoi(argv[1]);
   
    // Open an output file for logging
    outfile = fopen("CS_Output.txt", "w");
   
    // Check for errors in opening the output file
    if (outfile == NULL)
    {
        perror("There is an error opening the output file");
        return (-1);
    }
   
    // Initialize the client data list array
    for (i = 0; i < MAX_STATIONS; i++)
        clientDataList[i] = -1;

   
    // Create a socket
    fd_sock = socket(AF_INET, SOCK_STREAM, 0);

    // Check for errors in socket creation
    if (fd_sock < 0)
    {
        perror("ERROR opening socket");
        exit(1);
    }

    // Initialize server address structure
    bzero((char *)&serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(nonum);

    // Bind the socket to the server address
    if (bind(fd_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("Error on binding");
        exit(1);
    }
    
    // Start listening for incoming connections
    listen(fd_sock, 5);
   
    // Initialize variables for handling multiple clients using select
    cyll = sizeof(cli_addr);

    fd_set rset;

    FD_ZERO(&rset);
    i_max = -1;

    while (1)
    {
        FD_SET(fd_sock, &rset);
        int nready = select(fd_sock + 1, &rset, NULL, NULL, NULL);
      
        // Check if there is a connection request on the main socket
        if (FD_ISSET(fd_sock, &rset))
        { 
              // Accept the new connection
            fd_sock_new = accept(fd_sock, (struct sockaddr *)&cli_addr, (socklen_t *)&cyll);

            if (0 > fd_sock_new)
            {
                perror("Error on accept");
                exit(1);
            }
           
            for (i = 0; i < MAX_STATIONS; i++)
            {
                if (clientDataList[i] < 0)
                {
                    clientDataList[i] = fd_sock_new;
                    break;
                }
            }
           
             // Handle the case when there are too many clients
            if (MAX_STATIONS == i)
                printf("There are too many clients");
           
            // Update the maximum index for tracking active clients
            if (i_max < 1)
                i_max = i;
           
            // Create a child process to handle the new client
            id_p = fork();

            
            // Check for errors in fork
            if (id_p < 0)
            {
                perror("Error on fork");
                exit(1);
            }
             
             // Child process
            if (0 == id_p)
            {
                close(fd_sock);
                for (;;)
                {
                    doprocessing(fd_sock_new);
                }
                exit(0);
            }
            else
            {
                close(fd_sock_new);
            }
        }
    }

    //Close the output file
    fclose(outfile);
}

// Function to process requests from the client
void doprocessing(int sock)
{
    // Variable declarations
    int n;
    char buffer[256];

    int fromStation, toStation, frameid, waittime;
    char s1[100];
    char permissionText[20] = "permission";
    char rejectText[20] = "rejected";
    char waitText[20] = "wait";

    // Clear the buffer and read data from the client
    bzero(buffer, 256);
    n = read(sock, buffer, 255);
    // Check for errors in reading from the socket
    if (0 > n)
    {
        exit(1);
    }

    // Process the received data based on its content
    if (strstr(buffer, permissionText) != NULL)
    {
        // Parse the permission request
        sscanf(buffer, "%s %d", s1, &fromStation);

        // Log the received request from a station
        fprintf(outfile, "Receive request from SP %d \n", fromStation);

        // Send a positive reply to the station
        n = write(sock, "Accepted", 8);

        // Check for errors in writing to the socket
        if (0 > n)
        {
            perror("ERROR writing to socket");
            exit(1);
        }

        // Log the positive reply to the station
        fprintf(outfile, "Send positive reply to SP %d \n", fromStation);
    }
    else if (strstr(buffer, waitText) != NULL)
    {
        // Parse the wait request
        sscanf(buffer, "%s %d %d", s1, &waittime, &fromStation);

        // Log that the server is waiting for the specified time
        printf("Waiting for SP %d", fromStation);
    }
    else
    {
        // Parse the data frame
        sscanf(buffer, "%d %d %d", &frameid, &fromStation, &toStation);

        // Log the received data frame and forward it to the specified station
        fprintf(outfile, "Receive data frame from SP %d (to SP %d) \n", fromStation, toStation);
      
        // Clear the buffer and send a confirmation message to the station
        bzero(buffer, 256);
        strncpy(buffer, "Frame Received", 256);
        n = write(sock, buffer, 256);
       
        // Log the forwarding of the data frame
        fprintf(outfile, "Forward data Frame (from SP %d) to SP %d \n", fromStation, toStation);
    }
}