/*
** client.c -- a stream socket client demo
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <strings.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <arpa/inet.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <ctype.h>

/*#define SVRPORT "55000"
#define SVRNAME "localhost"*/
#define SVRPORT "55001"
#define SVRNAME "dns.postel.org"

#define MAXDATASIZE 1000

//////////////////////////////////////
// send a string ending in \n
//////////////////////////////////////
void mealysend(int sendsock, char *b, int bsize)
{
  char sendbuf[MAXDATASIZE + 3] = "";

  strncpy(sendbuf, b, MAXDATASIZE);
  strncat(sendbuf, "\n", MAXDATASIZE+2);

  if (send(sendsock,sendbuf,strlen(sendbuf), 0) == -1){
    perror("Client: send");
    exit(1);
  }
  printf("Client sent: %s",b);
  printf("  -- and it was %d bytes.\n",bsize);
  fflush(stdout);
}

//////////////////////////////////////
// receive up to the first \n
// (but omit the \n)
//	return 0 on EOF (socket closed)
//	return -1 on timeout
//////////////////////////////////////
int mealyrecvto(int recvsock, char *b, int bsize, int to)
{
  int num;
  int selectresult;
  fd_set myset;
  struct timeval tv;  // Time out

  int count = 0;
  char c = '\127';

  memset(b,0,MAXDATASIZE);
  
  while ((count < (bsize-2)) && (c != '\n') && (c != '\0')) {
    FD_ZERO(&myset);
    FD_SET(recvsock, &myset);
    tv.tv_sec = to;
    tv.tv_usec = 0;
    if ((to > 0) &&
	((selectresult = select(recvsock+1, &myset, NULL, NULL, &tv)) <= 0)) {
      // timeout happened (drop what you were waiting for -
      // if it was delimited by \n, it didn't come!
      return -1;
    }
    // got here because select returned >0, i.e., no timeout
    if ((num = recv(recvsock, &c, 1, 0)) == -1) {
      perror("Client: recv failed");
      exit(1);
    }
    if (num == 0) {
      // nothing left to read (socket closed)
      // no need to wait for a timeout; you're already done by now
      return 0;
    }
    b[count] = c;
    count++;
  }
  // at this point, either c is \n, \r or bsize has been reached
  // so just add a string terminator
  char *place;
  place = strchr(b,'\r');
  if (place != NULL) {
    *place = '\0';
  }
  place = strchr(b,'\n');
  if (place != NULL) {
    *place = '\0';
  }

  printf("Client received :%s",b);
  printf(": and it was %d bytes.\n", (int)strlen(b));
  return strlen(b);
}

void mealy(int msock)
{
  enum state{
    red, green, blue, purple, orange, yellow
  };
  char buf[MAXDATASIZE] = "";
  int numbytes;

  char str1[20];
  enum state CurrentState;
  CurrentState = red;
  strcpy(buf,"JimmyC");
  mealysend(msock,buf,strlen(buf));
  while (1) {
    if(CurrentState == red)
    {
      printf("Currently in state: red \n");
      while(1){
        printf("Enter \'go\' to start the communication: ");
        scanf("%s", str1);
        if(strncmp(str1,"go",3) == 0){
          strcpy(buf,"YO");
          mealysend(msock,buf,strlen(buf));
          CurrentState = green;
          break;
        }
      }
    }
    if(CurrentState == green)
    {
      printf("Currently in state: green\n");
      do{
        numbytes = mealyrecvto(msock, buf, MAXDATASIZE, 5);
        if(numbytes == -1){
          printf("Client timeout.\n");          
          strcpy(buf,"YO");
          mealysend(msock,buf,strlen(buf));
        }
        else if(strcmp(buf,"DUDE") == 0){
          strcpy(buf,"SUP");
          mealysend(msock,buf,strlen(buf));
          CurrentState = blue;
        }
        else if (numbytes == 0) {
          printf("Client stop; server socket closed.\n");
          break;
        }

      } while(numbytes == -1);
    }
    if(CurrentState == blue)
    {
      printf("Currently in state: blue\n");
      while(1)
      {
        printf("Enter \'shut\' to continue: ");
        scanf("%s", str1);
        if(strcmp(str1, "shut") == 0)
        {
          strcpy(buf,"LOCK");
          mealysend(msock,buf,strlen(buf));
          CurrentState = orange;
          break;
        }
      }
    }
    if(CurrentState == orange)
    {
      printf("Currently in state: orange\n");
      char msg[10] = "";
      char cmd[10] = "";
      while(1){
        printf("Enter a number from 0-9 to guess code or enter \'stop\': ");
        //scanf("%d", guess);
        scanf("%s",cmd);
        //printf("You entered: %s\n",cmd);
        if( strcmp(cmd,"stop") == 0){
          strcpy(buf, "LATER");
          mealysend(msock,buf,strlen(buf));          
          CurrentState = yellow;
          break;
        }
        else if(isdigit(cmd[0]) && cmd[1] == '\0'){
          int guess = (int)(cmd[0] - '0');
          if(guess >= 0 && guess <= 9)
          {
            //printf("Sending: TRY%d\n", guess);
            sprintf(msg, "TRY%d", guess);
            strcpy(buf, msg);
            mealysend(msock,buf,strlen(buf));
            CurrentState = purple;
            break;
          }
        }
        else{
          printf("Not a valid input.\n");
        }
      }
    }
    if(CurrentState == purple)
    {
      printf("Currently in state: purple\n");
      numbytes = mealyrecvto(msock, buf, MAXDATASIZE, 5);
      //printf("Received buffer: %s\n",buf);
      if (numbytes == -1) {
        CurrentState = orange;
        printf("Client timeout.\n");
      }
      else if (numbytes == 0) {
        printf("Client stop; server socket closed.\n");
        break;
      }
      else if( strncmp(buf, "CLACK",5) == 0) { 
        CurrentState = orange;
        //printf("Comparing to \'CLACK\' and setting state to orange\n");
      }
      else if( strncmp(buf, "CLICK",5) == 0){
        CurrentState = blue;
        //printf("Comparing to \'CLICK\' and setting state to blue\n");
      }
      else if( strncmp(buf, "BZZT",4) == 0){
        strcpy(buf, "LATER");
        mealysend(msock,buf,strlen(buf));
        CurrentState = yellow;
        //printf("Comparing to \'BZZT\' and setting state to yellow\n");
      }
      else{
        CurrentState = purple;
      }
    }
    if(CurrentState == yellow)
    {
      printf("Currently in state: yellow\n");
      numbytes = mealyrecvto(msock, buf, MAXDATASIZE, 5);
      //printf("Received buffer: %s\n",buf);
      //Send later if client timeout otherwise send BYE
      while(numbytes == -1){
        strcpy(buf, "LATER");
        mealysend(msock,buf,strlen(buf));
        numbytes = mealyrecvto(msock, buf, MAXDATASIZE, 5);
      }
      strcpy(buf, "BYE");
      mealysend(msock,buf,strlen(buf));
      CurrentState = red;
    }
  }

/*  ////// Send
  strcpy(buf,"YO");
  mealysend(msock,buf,strlen(buf));

  ////// Receive - either one works
  numbytes = mealyrecvto(msock, buf, MAXDATASIZE, 5);
  // numbytes = mealyrecvto(msock, buf, MAXDATASIZE, 0);

  ////// Receive with timeout (returns 0 on timeout)
  numbytes = mealyrecvto(msock, buf, MAXDATASIZE, 5);
  if (numbytes == -1) {
    printf("Client timeout.\n");
  } else if (numbytes == 0) {
    printf("Client stop; server socket closed.\n");
  }*/
}

int main(int argc, char *argv[])
{
  int sockfd, sen, valopt;  
  
  struct hostent *hp;
  struct sockaddr_in server;
  socklen_t lon;
  
  extern char *optarg;
  extern int optind;
  int c;
  char *sname=SVRNAME, *pname = SVRPORT;
  static char usage[] = "usage: %s [-s sname] [- p pname]\n";
  
  long arg;
  fd_set myset;
  struct timeval tv;  // Time out
  tv.tv_sec = 5;
  tv.tv_usec = 0;

  // parse the command line arguments
  while ((c = getopt(argc, argv, "s:p:")) != -1)
    switch (c) {
    case 'p':
      pname = optarg;
      break;
    case 's':
      sname = optarg;
      break;
    case '?':
      fprintf(stderr, usage, argv[0]);
      exit(1);
      break;
    }

  // convert the port string into a network port number
  server.sin_family = AF_INET;
  server.sin_port = htons(atoi(pname));

  // convert the hostname to a network IP address
  hp = gethostbyname(sname);
  if ((hp = gethostbyname(sname)) == NULL) {  // get the host info
    herror("Client: gethostbyname failed");
    return 2;
  }
  bcopy(hp->h_addr, &(server.sin_addr.s_addr), hp->h_length);
  bzero(&server.sin_zero, 8);

  // get a socket to play with
  if ((sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1) {
    perror("Client: could not create socket");
    exit(-1);
  }
  
  //////////////////////////////////////
  // open the connection
  
  // set the connection to non-blocking 
  arg = fcntl(sockfd, F_GETFL, NULL);
  arg |= O_NONBLOCK;
  fcntl(sockfd, F_SETFL, arg);
  //
  if ((sen = connect(sockfd,(struct sockaddr *)&server,sizeof(server))) == -1) {
    if (errno == EINPROGRESS) {
      FD_ZERO(&myset);
      FD_SET(sockfd, &myset);
      if (select(sockfd + 1, NULL, &myset, NULL, &tv) > 0){
	lon = sizeof(int);
	getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (void*)(&valopt), &lon);
	if (valopt) {
	  fprintf(stderr, "Client error in connection() %d - %s\n", valopt, strerror(valopt));
	  exit(-1);
	}
      }
      else {
	perror("Client: connection time out");
	exit(-1);
      }				
    }
    else {
      fprintf(stderr, "Client error connecting %d - %s\n", errno, strerror(errno));
      exit(0);
    }
  }
  //
  ///////////////////////////////
  
  // Set to blocking mode again
  arg = fcntl(sockfd, F_GETFL, NULL);
  arg &= (~O_NONBLOCK);
  fcntl(sockfd, F_SETFL, arg);

  //////////////////////////////
  mealy(sockfd);

  // and now it's done
  //////////////////////////////
  
  close(sockfd);
  return 0;
}
