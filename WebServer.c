#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define BUFSIZE 32768

struct {
    char *ext;
    char *type;
} exts[] = {
    {"gif", "image/gif" },
    {"jpg", "image/jpeg"},
    {"jpeg","image/jpeg"},
    {"png", "image/png" },
	{"ico","image/ico"},
	{"zip", "image/zip" },
    {"gz",  "image/gz"  },
    {"tar", "image/tar" },
    {"htm", "text/html" },
    {"html","text/html" },
    {"exe","text/plain" },
    {0,0} };

void doget(int taken, int socket_fd,char* buffer){

	char* dir = (char*)0;
	int nowpos=0;
	int file_fd;
	int pointpos;
	long tmp;
	char* fstr = "text/html";
	int buflen = strlen(buffer);
	
	//locate GET dir to char dir
	for(int i=4 ; i < buflen ; i++){
		if(buffer[i] == ' '){
			break;
		}else{
			//dir[nowpos] = buffer[i];
			if(buffer[i] == '.')
				pointpos = nowpos;
			nowpos++;
		}
	}
	//from 4 to nowpos is dir
	dir = malloc(sizeof(char) * nowpos+1);
	dir[0] = '.';
	strncpy(&dir[1], &buffer[4], nowpos);
	
	
	//if don't have dir, give it deafult 
	if(nowpos == 1){
		sprintf(dir,"./deafult.html");
	}
	int len;
	int dirlen = strlen(dir);
	for(int i=0;exts[i].ext!=0;i++){
		len = strlen(exts[i].ext);
		printf("Checking %s and %s\n",&dir[pointpos+2],exts[i].ext);
		if(!strncmp(&dir[pointpos+2], exts[i].ext, len)){
			fstr = exts[i].type;
			break;
		}
	}
	


	printf("///////////////\n");
	printf("My DIR IS %s\n",dir);
	printf("My Type is %s\n",fstr);
	printf("///////////////\n");

	////tell clinet failed
	if((file_fd = open(dir,O_RDONLY)) == -1)
		write(socket_fd, "Failed to open file.",19);
	
	////return file and type
	sprintf(buffer,"HTTP/1.0 200 OK\r\nContent-Type: %s\r\n\r\n", fstr);
	write(socket_fd,buffer,strlen(buffer));
	
	while((tmp = read(file_fd, buffer, BUFSIZE)) > 0){
		printf("writing\n");
		write(socket_fd, buffer, tmp);
	}
	return;
}

void dopost(int taken, int socket_fd, char* buffer){
	
		
	
	
	return;
}
void dealsocket(int socket_fd){
    
	long i, j, taken;
	int file_fd, buflen, len;
    char * fstr;
    static char buffer[BUFSIZE+1];
	
    taken = read(socket_fd,buffer,BUFSIZE);   /* 讀取瀏覽器要求 */
	
	buflen = strlen(buffer);

	if((taken == 0)||(taken == -1)){
		perror("read form socket");
		exit(3);
	}else{
		for(i=0;i<buflen;i++){
			printf("%c",buffer[i]);
		}
	}
	i = 0;
	if((strncmp(buffer,"GET ",4) == 0) || (strncmp(buffer,"get ",4) == 0))
		doget(taken, socket_fd, buffer);
	else if((strncmp(buffer,"POST ",5) == 0) || (strncmp(buffer,"post ",5)))
		dopost(taken, socket_fd, buffer);
	else
		printf("None Should do\n");

	return;
}



int main(int argc, char **argv){

    int i, pid, listenfd, socketfd;
    int length;
    static struct sockaddr_in cli_addr;
    static struct sockaddr_in serv_addr;
	printf("Start of Server\n");

    /* 使用 /tmp 當網站根目錄 */
    if(chdir("./data") == -1){ 
        printf("ERROR: Can't Change to directory %s\n",argv[2]);
        exit(4);
    }

    /* 背景繼續執行 */
    //if(fork() != 0)
    //    return 0;

    /* 讓父行程不必等待子行程結束 */
    signal(SIGCLD, SIG_IGN);

    //open socket
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd<0)
        exit(3);


	//set net
    serv_addr.sin_family = AF_INET;
    //set IP
    serv_addr.sin_addr.s_addr = inet_addr("127.0.0.1");

	// set port
    serv_addr.sin_port = htons(80);

    /* 開啟網路監聽器 */
    if (bind(listenfd, (struct sockaddr *)&serv_addr,sizeof(serv_addr))<0){
		
		perror("bind");
		exit(3);
	}
	
	printf("bind\n");
    
	/* 開始監聽網路 */
    if (listen(listenfd,64)<0){
		
		perror("listen");
		exit(3);
	}
	printf("listen\n");
    
	while(1){
		
		printf("in waiting\n");        
		length = sizeof(cli_addr);
        /* 等待客戶端連線 */
		socketfd = accept(listenfd, (struct sockaddr* )&cli_addr, &length);
        if (socketfd < 0){
			perror("accept");
			exit(3);
		}
		
        /* 分出子行程處理要求 */
        if ((pid = fork()) < 0) {
            perror("fork fail");
			exit(3);
        }else{
        	if (pid == 0) {  /* 子行程 */
            	close(listenfd);
            	dealsocket(socketfd);
            }
    	}
		close(socketfd);
	}

	return 0;
}
