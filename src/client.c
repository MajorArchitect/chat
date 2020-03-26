#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

int main()
{
  int socket1;
  socket1 = socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(8888);
  server_address.sin_addr.s_addr = INADDR_ANY;

  connect(socket1, (struct sockaddr*)&server_address, sizeof(server_address));

  send(socket1, "hello", 5, 0);



}
