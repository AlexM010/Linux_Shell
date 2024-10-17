/*
* Assignment 1 CS345
* Alexandros Markodimitrakis - csd4337@csd.uoc.gr
*/

#include <stdio.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <termios.h>
#include <fcntl.h>

#define MAX 1000
#define  MAX_PARAMS 100
void print_command(char** args[] );
char commands[2][MAX]={"quit","chdir"};

void command_prompt(){
    char buff[MAX];
    assert(getcwd(buff,MAX));
    fprintf(stdout,"\033[1m\x1b[32mcsd4337-hy345sh@%s\033[m:\x1b[34m%s\033[m$ ",getlogin(),getcwd(buff,MAX));
}

void read_command(char* input ,char*** parameters){
    int len=0;
    int i=0;
    int j=0;
    parameters[0]=calloc(MAX_PARAMS,sizeof(char*));
    while(input[i]!='\n'){
        if(input[i]==' '||input[i]==';'||input[i]=='|'||input[i]=='>'||input[i]=='<'||input[i]=='\n'){
            if(len>0){
                parameters[0][j]=calloc(len+1,1);
                strncpy(parameters[0][j],input+i-len,len);
                parameters[0][j][len]='\0';
                j++;
                len=0;
            }
            if(input[i]==';'||input[i]=='|'||input[i]=='>'||input[i]=='<'){
                if(input[i]=='>'&&input[i+1]=='>'){
                    parameters[0][j]=calloc(3,1);
                    strncpy(parameters[0][j],input+i,2);
                    parameters[0][j][2]='\0';
                    i++;
                }else{
                    parameters[0][j]=calloc(2,1);
                    strncpy(parameters[0][j],input+i,1);
                    parameters[0][j][1]='\0';
                }
                j++;
            }
            i++;
        }else{
            len++;
            i++;
            if(input[i]=='\n'){
                parameters[0][j]=calloc(len+1,1);
                strncpy(parameters[0][j],input+i-len,len);
                parameters[0][j][len]='\0';
                j++;
                len=0;
            }
        }
    }

}

void execute_commands(char** args){
     /*Creating child process*/
    int i=0;
    int j;
    int k=0;
    int pid;
    int status;
    char **curr_command;

    while(args[i]!=NULL){
        j=i;
        while(args[j]!=NULL&&strcmp(args[j],"|")!=0&&strcmp(args[j],">")!=0&&strcmp(args[j],">>")!=0&&strcmp(args[j],"<")!=0&&strcmp(args[j],";")!=0){
            j++;
        }
        curr_command=calloc(j-i+2,sizeof(char*));
        int k=0;
        while(i<j){
            curr_command[k]=args[i];
            k++;
            i++;
        }
        if(strcmp(curr_command[0],"quit")==0&&curr_command[1]==NULL){
            exit(0);
        }else if(strcmp(curr_command[0],"chdir")==0||strcmp(curr_command[0],"cd")==0){
            if(curr_command[2]!=NULL){
                fprintf(stdout,"Too many arguments for chdir!\n");
            }
            chdir(curr_command[1]);
        }else{
            if(args[i]==NULL||strcmp(args[i],";")==0){
                pid=fork();
                if(pid==0){
                    if(execvp(args[0],args)==-1){
                        fprintf(stdout,"%s: command not found\n",args[0]);
                    }
                    exit(0);
                }else if(pid>0){
                    waitpid(-1,&status,WUNTRACED);

                }else{
                    fprintf(stdout,"Fork did not executed succesfuly!\n");
                }
            }
        }
        i++;
        free(curr_command);
    }
}

/*Function to check if read_command works - Not used*/
void print_command(char** args[] ){
    int i=1;
    fprintf(stdout,"Command given: %s",args[0][0]);

    while(args[0][i]!=NULL){
        fprintf(stdout," - Argument %d given: %s",i,args[0][i]);
        i++;
    }

    fprintf(stdout,"\n");
}

void free_command(char*** parameters){
    int i=0;
    while(parameters[0][i]!=NULL){
        free(parameters[0][i]);
        i++;
    }
    free(parameters[0]);

}

int main(){
    int status;
    size_t size=0;
    char* line;
    char** parameters;
    while(1){
        command_prompt();
        getline(&line,&size,stdin);
        read_command(line,&parameters);
        execute_commands(parameters);
        free_command(&parameters);
    }
}