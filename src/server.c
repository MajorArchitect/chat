#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main()
{
  //sets the address structure
  struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(8888);
  server_address.sin_addr.s_addr = INADDR_ANY;

  fd_set master_fds;
  fd_set read_fds;
  int fdmax;
  int msgsize;

  int listener;
  int connections = 0;
  int max_connections = 10;
  int clients[max_connections];

  char buffer[256];

  //zeros the file descriptor lists
  FD_ZERO(&master_fds);
  FD_ZERO(&read_fds);

  //gets a socket and binds it
  listener = socket(AF_INET, SOCK_STREAM, 0);
  bind(listener, (struct sockaddr*)&server_address, sizeof(server_address));

  //make listener listen
  listen(listener, 10);

  //add listener to the faster file descriptor set
  FD_SET(listener, &master_fds);

  fdmax = listener;

  while (1)
  {
    read_fds = master_fds; //copies master_fds into read_fds
    if (select(fdmax + 1, &read_fds, NULL, NULL, NULL) == -1)
    {
      exit(0);
    }
    for (int i = 0; i <= fdmax; i++)//for every file descriptor. i is the current file descriptor
    {
      if (FD_ISSET(i, &read_fds))//if the checked file descriptor is in the list
      {
        if (i == listener) //if the new connection is from the listener/is a new client
        {
          //adds an empty client integer as a socket
          clients[connections] = accept(listener, NULL, NULL);
          FD_SET(clients[connections], &master_fds); //add new connection to the master set
          if (clients[connections] > fdmax) //keeps track of the largest socket descriptor
          {
            fdmax = clients[connections];
          }
          connections++;
          printf("New connection! Current connections: %d\n", connections);
        }
        else //this would be from an existing client
        {
          if ((msgsize = recv(i, buffer, sizeof(buffer), 0)) == 0) //if the message was empty, someone left
          {
            connections--;
            printf("Someone closed the connection. Current connections: %d\n", connections);
            close(i);
            FD_CLR(i, &master_fds);
          }
          else //otherwise we got data
          {
            for (int j = 0; j <= fdmax; j++) //for every file descrpitor. j is the current file descriptor
            {
              if (FD_ISSET(j, &master_fds))
              {
                if ((j != listener) && (j != i)) //send to everyone except the listener and the sender
                {
                  send(j, buffer, strlen(buffer), 0);
                }
              }
            }
          }
        }
      }
    }
  }

  return 0;
}
