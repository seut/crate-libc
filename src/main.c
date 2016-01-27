#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include "crate_conn.h"
#include "crate_request.h"
#include "crate_response.h"

#define MAXRCVLEN 500
#define PORTNUM 9300

int main(int argc, char *argv[])
{
    crate_conn connection;
    
    ssize_t len;
    struct sockaddr_in dest;
    
    connection.socket = socket(AF_INET, SOCK_STREAM, 0);
    
    memset(&dest, 0, sizeof(dest));                /* zero the struct */
    dest.sin_family = AF_INET;
    dest.sin_addr.s_addr = htonl(INADDR_LOOPBACK); /* set destination IP number - localhost, 127.0.0.1*/
    dest.sin_port = htons(PORTNUM);                /* set destination port number */
    
    connect(connection.socket, (struct sockaddr *)&dest, sizeof(struct sockaddr));
    
    crate_request * request = crate_write_request(&connection, "create table test (id int) with (number_of_replicas=0)");
    len = send(connection.socket, request->outBuffer->buffer, request->outBuffer->maxWrittenByte, 0);
    
    uint16_t msgLength = crate_conn_read_msg_size(&connection);
    crate_response * response = crate_read_response(&connection, msgLength);

    crate_request * request2 = crate_write_request(&connection, "insert into test (id) values (1), (2), (3)");
    len = send(connection.socket, request2->outBuffer->buffer, request2->outBuffer->maxWrittenByte, 0);
    uint16_t msgLength2 = crate_conn_read_msg_size(&connection);
    crate_response * response2 = crate_read_response(&connection, msgLength2);

    crate_request * request3 = crate_write_request(&connection, "select * from test");
    len = send(connection.socket, request3->outBuffer->buffer, request3->outBuffer->maxWrittenByte, 0);
    uint16_t msgLength3 = crate_conn_read_msg_size(&connection);
    crate_response * response3 = crate_read_response(&connection, msgLength3);

    if (response == NULL) {
        return EXIT_FAILURE;
    }
    
    //len = recv(mysocket, buffer, MAXRCVLEN, 0);
    
    /* We have to null terminate the received data ourselves */
    //buffer[len] = '\0';
    
    //printf("Received %s (%d bytes).\n", buffer, len);
    printf("HEE\n");
    
    close(connection.socket);
    return EXIT_SUCCESS;
}

