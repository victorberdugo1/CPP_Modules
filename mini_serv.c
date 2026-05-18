#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>

int extract_message(char **buf, char **msg)
{
 char *newbuf;
 int i;
 *msg = 0;
 if (*buf == 0)
  return (0);
 i = 0;
 while ((*buf)[i])
 {
  if ((*buf)[i] == '\n')
  {
   newbuf = calloc(1, sizeof(*newbuf) * (strlen(*buf + i + 1) + 1));
   if (newbuf == 0)
    return (-1);
   strcpy(newbuf, *buf + i + 1);
   *msg = *buf;
   (*msg)[i + 1] = 0;
   *buf = newbuf;
   return (1);
  }
  i++;
 }
 return (0);
}

char *str_join(char *buf, char *add)
{
 char *newbuf;
 int len;

 if (buf == 0)
  len = 0;
 else
  len = strlen(buf);
 newbuf = malloc(sizeof(*newbuf) * (len + strlen(add) + 1));
 if (newbuf == 0)
  return (0);
 newbuf[0] = 0;
 if (buf != 0)
  strcat(newbuf, buf);
 free(buf);
 strcat(newbuf, add);
 return (newbuf);
}

char *outbuf[65536], *inbuf[65536];
int ids[65536], max_fd;
fd_set fds, r_ready, w_ready;
char tmp[120000];
int sockfd;

void fatal(void)
{
 write(2, "Fatal error\n", 12);
 exit(1);
}

void queue(int sender, char *msg)
{
 for (int fd = 0; fd <= max_fd; fd++)
  if (FD_ISSET(fd, &fds) && fd != sockfd && fd != sender)
  {
   outbuf[fd] = str_join(outbuf[fd], msg);
   if (!outbuf[fd])
    fatal();
  }
}

void disconnect(int fd)
{
 sprintf(tmp, "server: client %d just left\n", ids[fd]);
 FD_CLR(fd, &fds);
 close(fd);
 free(inbuf[fd]);
 free(outbuf[fd]);
 inbuf[fd] = outbuf[fd] = NULL;
 queue(fd, tmp);
 while (max_fd >= 0 && !FD_ISSET(max_fd, &fds))
  max_fd--;
}

int main(int ac, char **av)
{
 int connfd, len;
 struct sockaddr_in servaddr, cli;
 char *msg;
 int sent, res, next_id = 0;

 if (ac != 2)
  return (write(2, "Wrong number of arguments\n", 26), 1);
 sockfd = socket(AF_INET, SOCK_STREAM, 0);
 if (sockfd == -1)
  fatal();
 bzero(&servaddr, sizeof(servaddr));
 servaddr.sin_family = AF_INET;
 servaddr.sin_addr.s_addr = htonl(2130706433);
 servaddr.sin_port = htons(atoi(av[1]));
 if ((bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr))) != 0)
  fatal();
 if (listen(sockfd, 10) != 0)
  fatal();
 FD_ZERO(&fds);
 FD_SET(sockfd, &fds);
 max_fd = sockfd;
 while (1)
 {
  r_ready = fds;
  FD_ZERO(&w_ready);
  for (int fd = 0; fd <= max_fd; fd++)
   if (FD_ISSET(fd, &fds) && outbuf[fd])
    FD_SET(fd, &w_ready);

  if (select(max_fd + 1, &r_ready, &w_ready, NULL, NULL) < 0)
   fatal();

  for (int fd = 0; fd <= max_fd; fd++)
  {
   if (fd == sockfd || !FD_ISSET(fd, &w_ready) || !outbuf[fd])
    continue;
   sent = send(fd, outbuf[fd], strlen(outbuf[fd]), 0x4000);
   if (sent < 0)
   {
    disconnect(fd);
    continue;
   }
   if (!outbuf[fd][sent])
   {
    free(outbuf[fd]);
    outbuf[fd] = NULL;
    continue;
   }
   char *nb = malloc(strlen(outbuf[fd] + sent) + 1);
   if (!nb)
    fatal();
   strcpy(nb, outbuf[fd] + sent);
   free(outbuf[fd]);
   outbuf[fd] = nb;
  }

  if (FD_ISSET(sockfd, &r_ready))
  {
   len = sizeof(cli);
   connfd = accept(sockfd, (struct sockaddr *)&cli, &len);
   if (connfd < 0)
    continue;
   if (connfd >= FD_SETSIZE)
   {
    close(connfd);
    continue;
   }
   FD_SET(connfd, &fds);
   if (connfd > max_fd)
    max_fd = connfd;
   ids[connfd] = next_id++;
   inbuf[connfd] = outbuf[connfd] = NULL;
   sprintf(tmp, "server: client %d just arrived\n", ids[connfd]);
   queue(connfd, tmp);
  }

  for (int fd = 0; fd <= max_fd; fd++)
  {
   if (fd == sockfd || !FD_ISSET(fd, &r_ready))
    continue;
   res = recv(fd, tmp, 4096, 0);
   if (res <= 0)
   {
    disconnect(fd);
    continue;
   }
   tmp[res] = 0;
   inbuf[fd] = str_join(inbuf[fd], tmp);
   if (!inbuf[fd])
    fatal();
   while ((res = extract_message(&inbuf[fd], &msg)) == 1)
   {
    sprintf(tmp, "client %d: %s", ids[fd], msg);
    queue(fd, tmp);
    free(msg);
   }
   if (res == -1)
    fatal();
  }
 }
}
