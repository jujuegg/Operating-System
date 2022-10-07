#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> 
#include <sys/socket.h>
#include <sys/wait.h>
#include <fcntl.h>        
#include <ctype.h>
#include <stdbool.h>
#define TRUE 1
#define FALSE 0
#define MAX_COMMAND_LENGTH 500
#define MAX_COMMAND_COUNT 50
char *process1[MAX_COMMAND_COUNT],*process2[MAX_COMMAND_COUNT],*file;
bool background_running,in_rederict,out_rederict,pipe_use;

void reset();//
void print_prompt();//
char *read_command();//
bool no_command(char *cmd);//
char **split_command(char *cmd);//
bool check_cmd_type(char **cmds);//
void execute_process();//

int main()
{
	char **separated_command,*command_line;
	
	while(1)
	{
		reset();	//reset all global variable
		print_prompt();
		command_line=read_command();
		if(command_line==NULL) // no cammand   
			continue;

        separated_command=split_command(command_line);
		if(strcmp(separated_command[0],"exit") == 0) //check if intput exit
			break;

        if(check_cmd_type(separated_command) == 0)
			continue;

        execute_process();
	}
	return 0;
}
void reset()
{
	background_running=in_rederict=out_rederict=pipe_use=FALSE;
	file=NULL;
    memset(process2,'\0',sizeof(char *)*MAX_COMMAND_COUNT);
	memset(process1,'\0',sizeof(char *)*MAX_COMMAND_COUNT);
}
void print_prompt()
{
	char cwd[200];
	char hostName[50];
	getcwd(cwd,sizeof(cwd));
	gethostname(hostName,sizeof(hostName));
	printf("@%s:~%s$ ",hostName,cwd);
}
bool no_command(char *cmd)
{
    if (!cmd) //cmd is null   
    	return TRUE;
    while (isspace(*cmd))   //delete space and \n
    	cmd++;
	if (strlen(cmd) == 0)	
		return TRUE;

    return FALSE;
}
char *read_command()
{
	static char command[MAX_COMMAND_LENGTH];
	fgets(command,MAX_COMMAND_LENGTH,stdin); //including '\n'
	if (feof(stdin) || no_command(command))    
		return NULL;

    return command;
}
char **split_command(char *cmd)
{
    char *token;
    static char *commands[MAX_COMMAND_COUNT];	//after split
    memset(commands,'\0',MAX_COMMAND_COUNT);	//initial

    int i=0;
    token=strtok(cmd," \t\n");	//use space to spilt,ignore last '\n'
    while(token != NULL)
    {
        commands[i]=token;
        if(i<MAX_COMMAND_COUNT)
            token=strtok(NULL," \t\n");
        i++;
    }
    return commands;
}
bool check_cmd_type(char **cmds)	// if background or rederict or pipe
{
    int count=0,i,j;
    while (cmds[count]) 
		count++;

    if (strcmp(cmds[count-1],"&")==0) //background (check last cmd is & or not)
    {
        background_running=TRUE;
        cmds[count-1] = NULL;	//delete it
        count--;
    }

    for (i=0; i<count; i++)
    {
        if (!strcmp(cmds[i],">") || !strcmp(cmds[i],"<"))	//rederict
        {
			if(!strcmp(cmds[i],">"))
            	out_rederict=TRUE;
			else
				in_rederict=TRUE;
            file = cmds[i+1];	//rederict target
			if(file==NULL)	//if no target
			{
				fprintf(stderr,"Error: error command.\n");
				return 0;
			}
			break;
        }
        else if (!strcmp(cmds[i],"|"))	//pipe
        {
            pipe_use=TRUE;
            for (j=i+1; j<count; j++)	//cmd after pipe
                process2[j-i-1]=cmds[j];
			break;
        }
		else
			process1[i]=cmds[i];	//cmd before pipe
    }
	if ((!strcmp(process1[0],"cd")&&count-1>1))		//check only 1 parameter after cd 
    {
        fprintf(stderr,"Error: error command.\n");
        return 0;
    }
	return 1;
}
void execute_process()
{
    if (!strcmp(process1[0],"cd"))  //cd is special
	{
		if (chdir(process1[1]))
            fprintf(stderr,"Error: Unknown directory.\n");
	}
	else
	{
		int status,fd,pipe_fd[2];	//fd is rederict's target file
		pid_t pid,pid2;

		pid=fork();
		if (pid<0)		//error
		{
		    fprintf(stderr,"Error\n");
		    exit(EXIT_FAILURE);
		}
		else if(pid==0)  //child1
		{
		    if(pipe_use)
		    {
		        pipe(pipe_fd);
		        pid2=fork();
		        if (pid2<0)		//error
		        {
		            fprintf(stderr,"Error\n");
		            exit(EXIT_FAILURE);
		        }
		        else if (pid2==0) //child1's child 
		        {
		            close(pipe_fd[0]);	//close read
					close(STDOUT_FILENO);	//close stdout
		            dup(pipe_fd[1]);	//pipe write replace stdout to parent
		            close(pipe_fd[1]);
		            if (execvp(process1[0],process1) == -1)	//execute first command 
		            {										//do first					
		                fprintf(stderr,"Error: command not found\n");
		                exit(EXIT_FAILURE);
		            }
		        }
		        else	//child1 (parent)
		        {
		            waitpid(pid2,NULL,0);   //wait child to execute first command 
		            close(pipe_fd[1]);	    //so that parent get can get first command's
					close(STDIN_FILENO);   	//result and execute the second command
		            dup(pipe_fd[0]);
		            close(pipe_fd[0]);
		            if (execvp(process2[0],process2) == -1)	//execute second command 
		            {										//do later
		                fprintf(stderr,"Error: command not found\n");
		                exit(EXIT_FAILURE);
		            }
		        }
		    }
		    else
		    {
		        if(in_rederict)
		        {
		            fd = open(file,O_RDONLY); //read only:in_rederict no need to write
					close(STDIN_FILENO);
		            dup(fd);	//use fd to replace stdin
		            close(fd);
		        }
		        else if(out_rederict)
		        {	//write only|if no file,create one|clean old contents|permission
		            fd = open(file,O_WRONLY|O_CREAT|O_TRUNC,0666);	
					close(STDOUT_FILENO);
		            dup(fd);	//use fd to replace stdout
		            close(fd);
		        }
		        if(execvp(process1[0],process1) == -1)
		        {
		            fprintf(stderr,"Error: command not found\n");
		            exit(EXIT_FAILURE);
		        }
		    }
		}
		else    //parent
		{
		    if (background_running)	//don't wait if no &
		        fprintf(stdout,"[PID]: %u\n",pid);
		    else
		        waitpid(pid,&status,0);
		}
	}
}
