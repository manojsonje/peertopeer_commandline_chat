
#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<unistd.h>
#include<arpa/inet.h>

//===================================================================================================
//general socket structure

typedef struct socket_data
{
    int conn_fd;
    struct sockaddr_in ip_addr;
}socket_data;



typedef struct socketinfo
{
    int listener_fd,conn_fd;
    struct sockaddr_in listener_addr, client_addr;
}socketinfo;

//====================================================================================================
//client structure 
socket_data * cSenderSocket (char *ipaddr, int port)
{
    socket_data *sock;
    sock=(socket_data *) malloc (sizeof(socket_data));

    memset(&sock->ip_addr,'0',sizeof(sock->ip_addr));
    sock->conn_fd = socket(AF_INET,SOCK_STREAM,0);

        if (sock->conn_fd == -1)
        {
            printf("\nsorry Client_Socket cannot start please try again");
            exit(1);
        }
    sock->ip_addr.sin_family = AF_INET;
    sock->ip_addr.sin_port = htons(port);
    sock->ip_addr.sin_addr.s_addr = inet_addr(ipaddr);

    return sock;
}

//=====================================================


socketinfo * sRecieverSocket(int port)
{
 socketinfo *sock;
 sock=(socketinfo *)malloc(sizeof(socketinfo));
 sock->listener_fd=socket(AF_INET,SOCK_STREAM,0);

 if(sock->listener_fd==-1)
 {
   perror("\nsorry listener_Socket cannot start please try again");
   exit(1);
 }

 memset(&sock->listener_addr,'0',sizeof(sock->listener_addr));     //fill addr with '0'

 sock->listener_addr.sin_family = AF_INET;
 sock->listener_addr.sin_addr.s_addr = htonl(INADDR_ANY);
 sock->listener_addr.sin_port=htons(port);

 if(bind(sock->listener_fd , (struct sockaddr *) &sock->listener_addr , sizeof(sock->listener_addr) ) == -1)
 {
     fprintf(stdout,"sorry! ERROR Address Binding Failed");
     exit(1);
 }

 if(listen(sock->listener_fd,50) == -1)
 {
    fprintf(stdout," Not Able to Listen");
    exit(1);
 }

 return sock;
}



char *get_user_ip()
{
  FILE *fp1;
  static char l1[40],l2[40];
  int i=1,c;
  printf("\nSelect from contacts (ENTER NUMBER)>>>>>>\n");
  fp1 = fopen(".contacts","r+");
  while(fscanf(fp1,"%s %s",l1,l2)==2)
  { 
    printf("[%d]  %s\n",i,l1);
    i++;
  }
  fclose(fp1);
  scanf("%d",&c);
  i = 1;
  fp1 = fopen(".contacts","r+");
  while(fscanf(fp1,"%s %s",l1,l2))
  { 
    if (i == c)
    {
      c = strlen(l2);
      l2[c] = '\0';
      return l2;
    } 
    i++;
  }
  fclose(fp1);
}

int main()
{

   int pid,l8 = 1,i = 0,chk = 0,l,addrlen,ch = 1;
   
   char uname[40],line[1024],msg[1024];
   FILE *unfd,*ufd,*fp;
   
   char *ipaddr;

   system("clear");

   printf("Sign in (without password):- Enter Your Username :- ");
   scanf("%s",uname);

   pid = fork();
// fork the process to create child and parent process

   if (pid == 0) // this is child process
   {
     socket_data *sock;
     while(1)
     {
       ipaddr = get_user_ip();
       sock = cSenderSocket(ipaddr,55555);

       if (connect(sock->conn_fd , (struct sockaddr *) &sock->ip_addr , sizeof(sock->ip_addr)) != -1)
       {
         memset(msg,'\0',1024);
         memset(line,'\0',1024);
         printf("Send Massage :-->> ");
         scanf("%s",msg);
         strcat(line,uname);
         strcat(line,"<< ");
         strcat(line,msg);
         write(sock->conn_fd,line,1024); // massage write to Socket
       }
       close(sock->conn_fd);
     }
   }
//this is parent process
   else 
   {
     socketinfo *sock;
     sock=sRecieverSocket(55555);
     addrlen = sizeof(sock->client_addr);
     while(1)
     {
       if ((sock->conn_fd=accept(sock->listener_fd , (struct sockaddr *) &sock->client_addr  , &addrlen )) != -1)
       {
         memset(msg,'\0',1024);
         read(sock->conn_fd,msg,1024); 
         printf("%s\n",msg);
       }
       close(sock->conn_fd);
     }
   }
}

