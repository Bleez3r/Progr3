#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include "ask2.h"

int main(int argc,char *argv[]){

    int fd_client_to_server[CUSTOMERS_NUM][2]; // 0 read 1 write o pelaths grafei o server akouei
    int fd_server_to_client[CUSTOMERS_NUM][2]; //                 o server grafei o pelaths akouei
   
    for(int i=0;i<CUSTOMERS_NUM;i++){

        if(pipe(fd_client_to_server[i]) < 0 || pipe(fd_server_to_client[i]) < 0){
            printf("Pipe creation error \n");
            return 1;
        }   
    }

    for (int i=0;i<CUSTOMERS_NUM;i++){
        pid_t pid = fork();
        if (pid<0){
            printf("Fork Error\n");
            return 1;
        }
        if (pid ==0){
            close(fd_client_to_server[i][0]);
            close(fd_server_to_client[i][1]);
            client(fd_client_to_server[i],fd_server_to_client[i]);
            return 0;
        }
        else{
            
            close(fd_client_to_server[i][1]);
            close(fd_server_to_client[i][0]);
            
        }

    }   
       
        
    server(fd_client_to_server,fd_server_to_client);
    
    return 0;
    
}
