#include <sys/select.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <netdb.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[])
{  //variable declaration
    int fd_sock, nonum, n, no_station_num, waiter = 0;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    FILE *input_file;
    FILE *outfile;

    char buffer[256];
    char bufferFile[256];
     // Check if the correct number of command-line arguments is provided
    if (4 != argc)
    {
        fprintf(stderr, "Usage: ./SP <server name> <port> <station number> \n");
        exit(0);
    }
    // Convert port number from command line argument to integer
    nonum = atoi(argv[2]);

    //create a socket
    fd_sock = socket(AF_INET, SOCK_STREAM, 0);

    // Check for errors in socket creation
    if (0 > fd_sock)
    {
        perror("ERROR opening socket");
        exit(1);
    }
    
    // Get host information from the server name provided
    server = gethostbyname(argv[1]);
    
    //check if the server is NULL
    if (NULL == server)
    {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }
    // Convert station number from command line argument to integer
    no_station_num = atoi(argv[3]);

    // Check if the station number is valid
    if (no_station_num < 0)
    {
        fprintf(stderr, "ERROR, Input station number is missing \n");
        exit(0);
    }

    // Initialize server address structure
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(nonum);

    // Connect to the server
    if (connect(fd_sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
    {
        perror("ERROR connecting");
        exit(1);
    }
    
    // Create an output file with a name based on the station number
    bzero(bufferFile, 256);
    sprintf(bufferFile, "Station_Process_Output_%d.txt", no_station_num);
    outfile = fopen(bufferFile, "w");
    
    // Check for errors in opening the output file
    if (outfile == NULL)
    {
        perror("Error while opening the output file");
        return (-1);
    }
    
    // Open an input file based on the station number
    switch (no_station_num)
    {
    case 1:
        input_file = fopen("station_process1.txt", "r");
        break;
    case 2:
        input_file = fopen("station_process2.txt", "r");
        break;
    case 3:
        input_file = fopen("station_process3.txt", "r");
        break;
    case 4:
        input_file = fopen("station_process4.txt", "r");
        break;
    case 5:
        input_file = fopen("station_process5.txt", "r");
        break;
    case 6:
        input_file = fopen("station_process6.txt", "r");
        break;
    case 7:
        input_file = fopen("station_process7.txt", "r");
        break;
    case 8:
        input_file = fopen("station_process8.txt", "r");
        break;
    case 9:
        input_file = fopen("station_process9.txt", "r");
        break;
    case 10:
        input_file = fopen("station_process10.txt", "r");
        break;
    }
    // Check for errors in opening the input file
    if (input_file == NULL)
    {
        perror("Error while opening the input file");
        return (-1);
    }

    // Initialize a file descriptor set for select
    fd_set rset;

    FD_ZERO(&rset);

    bzero(bufferFile, 256);

    while (fgets(bufferFile, sizeof bufferFile, input_file) != NULL)
    {
        // Add the socket to the file descriptor set
        FD_SET(fd_sock, &rset);

        int frameid, tostation, waitreq;
        char s1[100], s2[100], s3[100], s4[100];
        fprintf(outfile, "Sending request to CSP to send data %s", bufferFile);

        if (strstr(bufferFile, "Wait") != NULL)
        {
            sscanf(bufferFile, "%s %d", s1, &waitreq);
            waiter = waitreq;
            bzero(bufferFile, 256);
            sprintf(bufferFile, "wait %d %d", waitreq, no_station_num);
        }
        else
        {
            sscanf(bufferFile, "%s %d %s %s %d", s1, &frameid, s2, s3, &tostation);
            bzero(bufferFile, 256);
            sprintf(bufferFile, "%d %d %d", frameid, no_station_num, tostation);
        }
       
        // Prepare a buffer for sending permission request
        bzero(buffer, 256);
        sprintf(buffer, "%s %d", "permission", no_station_num);
       
        // Send the permission request to the server
        n = write(fd_sock, buffer, strlen(buffer));
        
        //Check for the error
        if (n < 0)
        {
            perror("ERROR writing to socket");
            exit(1);
        }

        bzero(buffer, 256);
        if ((n = read(fd_sock, buffer, 255)) > 0)
        {
            // Process the server's respons
            if (strstr(buffer, "Accepted") != NULL)
            {
              
                // Print a positive reply message to the output file
                fprintf(outfile, "Receive positive reply (permission) from CSP to send data frame %d to SP %d \n", frameid, tostation);
                
                // Send the data frame to the server via CSP
                n = write(fd_sock, bufferFile, strlen(bufferFile));
               
                // Check for errors in writing from the socket
                if (0 > n)
                {
                    perror("ERROR writing from socket");
                    exit(1);
                }
               
                // Print a message to the output file about sending the data frame
                fprintf(outfile, "Send (via CSP) data frame %d to SP %d \n", frameid, tostation);
            }
            else if (strstr(buffer, "rejected") != NULL)
            {
                
                // Print a message about retransmitting the request to the output file
                fprintf(outfile, "Retransmit request to CSP to send data %s \n", bufferFile);
            }
        }
       
        // Clear buffers and remove the socket from the file descriptor set
        bzero(buffer, 256);
        bzero(bufferFile, 256);
        FD_CLR(fd_sock, &rset);
    }
    // Close files and the socket
    fclose(input_file);
    fclose(outfile);

    close(fd_sock);

    return 0;
}
