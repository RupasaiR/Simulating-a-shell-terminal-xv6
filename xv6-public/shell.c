#include "types.h"
#include "user.h"
#include "fcntl.h"

#define nargs 10
#define argsize 50


struct cmd{
int pipe;
char *arguments[nargs];
int inp;
int oup;
char *inpstr;
char *outstr;
};

struct pipest{
int set;
struct cmd *lcmd;
struct cmd *rcmd;
};

struct parellelcmd{
int set;
struct cmd *cmd1;
struct cmd *cmd2;
};

struct succexeccmd{
int set;
int both;
struct cmd *cmd1;
struct cmd *cmd2;
};

struct pipest *pipest=0;
struct pipest *pipest1=0;
struct cmd *cmd=0;
struct parellelcmd *parellelcmd=0;
struct succexeccmd *succexeccmd=0;		
int execcmd(struct cmd* cmd);
int ioredexec(struct cmd* cmd);
void parseandexec(char * line1);
void storevalues(struct cmd *cmd,char arg[nargs][argsize],int j,char *inpstr,char *outstr,int inp,int oup);
int readcmd(char *b){
	memset(b,0,argsize);
	gets(b,argsize);
	if(b[0]==0)
		return -1;
	return 0;
}

void printstr(char *b,int k){
	//0 represents new line
	if(k==0)
		printf(2,"%s\n",b);
	else
		printf(2,"%s",b);
}


void chkargs(struct cmd *cmd){
for(int i=0;i<nargs;i++)
	printstr(cmd->arguments[i],0);
if(cmd->inp)
	printstr(cmd->inpstr,0);
if(cmd->oup)
	printstr(cmd->outstr,0);
}

int checkcmd(char *cmd){
	if(strcmp(cmd,"exit")==0||strcmp(cmd,"ls")==0||strcmp(cmd,"grep")==0||strcmp(cmd,"cat")==0||strcmp(cmd,"wc")==0||strcmp(cmd,"ps")==0||strcmp(cmd,"procinfo")==0||strcmp(cmd,"echo")==0||strcmp(cmd,"executeCommands")==0)
		return 1;
	if(strcmp(cmd," ")==0 || strcmp(cmd,"/0")==0||cmd==0){
		return 0;
	}
	printstr("llegal command or arguments",0);
	return 0;
}

void strcpy1(char *a,char *b){
	while(*b!='\n'){
	*a=*b;
	a++;
	b++;
	}
	*a='\n';	
}
void parse(char *b,struct cmd *cmd,struct pipest *pipest1,struct parellelcmd *parellelcmd){
	char arg[nargs][argsize];
	char outstr[argsize];
	char inpstr[argsize];
	int op=0,ip=0;
	for(int i=0;i<nargs;i++){
		memset(arg[i],0,argsize);
	}
  int i=0;
	int j=0,k=0;
	while(i<strlen(b) && b[i] == ' '){
		i++;
	}
	while(i<strlen(b)){
		if(b[i]==' ' && (i+1<strlen(b)) && b[i+1]==' '){
			continue;
			}
		else if(b[i]==' ' || b[i]=='\n'||b[i]=='\t'||b[i]=='\0'||b[i]==0){
			if(k==0)
				break;
			if(ip==1){
				ip=0;
				inpstr[k]='\0';
				cmd->inpstr=inpstr;
				cmd->inp=1;
				}
			else if(op==1){
				op=0;
				outstr[k]='\0';
				cmd->outstr=outstr;
				cmd->oup=1;
				}
			else{
				arg[j][k]='\0';
				j++;
				}
			k=0;
		}
		else if(b[i]=='|'&& b[i+1]!='|'){
			cmd->pipe=1;
			if(k!=0){
			arg[j][k]='\0';
			j++;
			}
			if(pipest1->set!=1){
				pipest1->set=1;
				parse(b+i+1,pipest1->rcmd,pipest1,parellelcmd);
			}
			break;
		}
		else if(b[i]=='<'){
			ip=1;
			if(b[i-1]!=' '){
				arg[j][k]='\0';
				j++;
				k=0;
			}
			i++;
			while(i<strlen(b) && b[i]==' ')
				i++;
			i--;
		}
		else if(b[i]=='>'){
			op=1;
			if(b[i-1]!=' '){
				if(ip==1){
					ip=0;
					inpstr[k]='\0';
					cmd->inpstr=inpstr;
					cmd->inp=1;
					k=0;
				}
				else{
				arg[j][k]='\0';
				j++;
				k=0;
				}
				}
			i++;
			while(i<strlen(b) && b[i]==' '){
				i++;
			}
			i--;
		}
		else if(b[i]==';'){
			if(parellelcmd->set!=1){
				parellelcmd->set=1;
				parse(b+i+2,parellelcmd->cmd2,pipest1,parellelcmd);
				}
			break;
		}
		else if(b[i]=='&'&&b[i-1]=='&'){
			succexeccmd->set=1;
			succexeccmd->both=0;
			storevalues(succexeccmd->cmd1,arg,j,cmd->inpstr,cmd->outstr,cmd->inp,cmd->oup);
			succexeccmd->cmd2=malloc(sizeof(struct cmd));
			parse(b+i+1,succexeccmd->cmd2,pipest1,parellelcmd);
			break;
		}
		else if(b[i]=='&'){
		i++;
		continue;
		}
		else if(b[i]=='|'&&b[i+1]=='|'){
			succexeccmd->set=1;
			succexeccmd->both=1;
			storevalues(succexeccmd->cmd1,arg,j,cmd->inpstr,cmd->outstr,cmd->inp,cmd->oup);
			succexeccmd->cmd2=malloc(sizeof(struct cmd));
			parse(b+i+2,succexeccmd->cmd2,pipest1,parellelcmd);
			break;
		}
		else{
			if(ip==1)
				inpstr[k]=b[i];
			else if(op==1)
				outstr[k]=b[i];
			else
				arg[j][k]=b[i];
			k++;
		}
		i++;
		}
		for(int k=0;k<j;k++){
			cmd->arguments[k]=arg[k];
		}
		for(int k=j;k<nargs;k++){
			cmd->arguments[k]=0;
		}
//		if(parellelcmd->set==1)
	//		parellelcmd->cmd1=cmd;
		//printstr(cmd->arguments[0],0);
}
void storevalues(struct cmd *cmd,char arg[nargs][argsize],int j,char *inpstr,char *outstr,int inp,int oup){
	if(inp==1){
		cmd->inpstr=inpstr;
		cmd->inp=inp;
	}
	if(oup==1){
		cmd->outstr=outstr;
		cmd->oup=oup;
	}
	for(int k=0;k<j;k++){
		cmd->arguments[k]=arg[k];
	}
	for(int k=j;k<nargs;k++){
		cmd->arguments[k]=0;
	}
}
void runpipe(struct cmd *left,struct cmd *right){
	int pid[2];
	pipe(pid);
	int id1,id2;
	id1=fork();
	if(id1 == 0){
		close(1);
		dup(pid[1]);
		close(pid[0]);
		close(pid[1]);
		if(left->inp==1){
			left->inp=0;
			close(0);
			dup(open(left->inpstr,O_RDONLY));
			exec(left->arguments[0],left->arguments);	
		}
			exec(left->arguments[0],left->arguments);
		}
		id2=fork();
		if(id2 == 0){
			close(0);
			dup(pid[0]);
			close(pid[0]);
			close(pid[1]);
			if(right->oup==1){
				right->oup=0;
				close(1);
				unlink(right->outstr);
				dup(open(right->outstr,O_CREATE|O_WRONLY));
				exec(right->arguments[0],right->arguments);
			}
			exec(right->arguments[0],right->arguments);
		}
		close(pid[0]);
		close(pid[1]);
		wait(0);
		wait(0);
		left->inp=0;
		right->oup=0;
}

int ioredexec(struct cmd *cmd)
{
	int st=-1;
	if(checkcmd(cmd->arguments[0])==1){
	int fd1,fd2;
	if(fork()==0){
		if(cmd->inp==1){
			fd1=open(cmd->inpstr,O_RDONLY);
			close(0);
			dup(fd1);
		}
		if(cmd->oup==1){
			unlink(cmd->outstr);
			fd2=open(cmd->outstr,O_RDWR|O_CREATE);
			close(1);
			dup(fd2);				
		}
		st=exec(cmd->arguments[0],cmd->arguments);
		cmd->inp=0;
		cmd->oup=0;
		return -1;
	}
	else{
		wait(&st);
		cmd->inp=0;
	cmd->oup=0;
	return st;

	}
	}
	return st;
}
	
void preprocs(char *line,char *line1){
int i=0,j=0;
memset(line1,0,argsize);
while(i<strlen(line)){
	if(line[i]==' ' && line[j-1] == ' ')
		{
		i++;
		continue;
		}
	if(line[i]=='<' || line[i]=='>'|| line[i]=='|' || line[i]==';' || line[i]=='&'||line[i]=='&'){
		if(line1[j-1]==' ')
			line1[j++]=line[i];
		else if(line1[j-1]!='&'&& line1[j-1]!='|'){
		line1[j++]=' ';
		line1[j++]=line[i];
		}
		else
		line1[j++]=line[i];
		}
	else
	{
		if(j>0 && line[i]!=' ' &&(line1[j-1]=='<' || line1[j-1]=='>'|| line1[j-1]=='|' || line1[j-1]==';'||line1[j-1]=='|'||line1[j-1]=='&')){
			line1[j++]=' ';
		}
		line1[j++]=line[i];
	}
	i++;
	}
	line1[j]='\0';
	//printstr(line1,0);
}

void runparallel(struct cmd* cmd1,struct cmd* cmd2){
	if(fork()==0){
		int fd1,fd2;
		if(cmd->inp==1){
		fd1=open(cmd1->inpstr,O_RDONLY);
		close(0);
		dup(fd1);
		}
		if(cmd->oup==1){
			unlink(cmd1->outstr);
			fd2=open(cmd1->outstr,O_RDWR|O_CREATE);
			close(1);
			dup(fd2);				
			}
			exec(cmd1->arguments[0],cmd1->arguments);
	}
	if(fork()==0){
		// if(cmd2->inp||cmd2->oup)
		// 	ioredexec(cmd2);
		// else
		int fd1,fd2;
		// if(pipest->set==1)
		// {
		// 	runpipe(cmd2,pipest->rcmd);
		// }
		//printf(2,"%d",cmd2->pipe);
		// chkargs(cmd1);
		// chkargs(cmd2);
		// chkargs(pipest->rcmd);
		// printf(2,"ppp : %d ",pipest->set);
		// if(cmd2->pipe==1)
		// {
		// 	printstr("here11",0);
		// 	runpipe(cmd2,pipest->rcmd);
		// 	exit(0);
		// }
		if(cmd2->inp==1){
			fd1=open(cmd2->inpstr,O_RDONLY);
			close(0);
			dup(fd1);
		}
		if(cmd2->oup==1){
			unlink(cmd2->outstr);
			fd2=open(cmd2->outstr,O_RDWR|O_CREATE);
			close(1);
			dup(fd2);				
			}

		exec(cmd2->arguments[0],cmd2->arguments);
		}
	
	wait(0);
	wait(0);
	pipest->set=0;
	cmd2->pipe=0;
	cmd2->inp=0;
	pipest->rcmd->oup=0;
}

int execcmd(struct cmd* cmd)
{
	if(checkcmd(cmd->arguments[0])==1){				
			return exec(cmd->arguments[0],cmd->arguments);
			exit(-1);
		}
	return -1;
}

void runsucces(struct succexeccmd* succexeccmd){
	int st=-1;
	if(fork()==0){
		st=ioredexec(succexeccmd->cmd1);
		exit(st);
	}
	wait(&st);
	if(succexeccmd->both==1 || st==0){
	if(fork()==0){
		st=ioredexec(succexeccmd->cmd2);
		exit(st);
	}
	wait(&st);
	}
}

void execcmds(struct cmd *cmd){
	char buf[512];
	char line[argsize];
	char line1[argsize];
	char file[10];
	memset(line,0,argsize);
	int n,i=0,k=0;
	strcpy(file,cmd->arguments[1]);
	int fd=open(file,O_RDONLY);
	if(fd<0)
		printstr("cannot open file",0);
	else{
		for(;;){
		n=read(fd,buf,sizeof(buf));
		if(n==0)
			break;
		i=0;
		while(i<n){
			cmd->inp=0;
			cmd->oup=0;
			if(buf[i]=='\n'){
				line[k]='\0';
				preprocs(line,line1);
				line1[strlen(line1)]='\n';
				parseandexec(line1);
				i++;
				k=0;
				continue;
			}
			line[k++]=buf[i++];
		}
		}
	}
}
void parseandexec(char * line1){
	cmd->pipe=0;
	parse(line1,cmd,pipest,parellelcmd);
	if(checkcmd(cmd->arguments[0])==1){
	if(strcmp(cmd->arguments[0],"exit")==0){
			exit(0);
	}
	else if(strcmp(cmd->arguments[0],"executeCommands")==0){
			execcmds(cmd);
	}
	else if(succexeccmd->set==1){
		succexeccmd->set=0;
		runsucces(succexeccmd);
	}
	else if(parellelcmd->set==1){
		parellelcmd->set=0;
		runparallel(cmd,parellelcmd->cmd2);
	}
	else if(pipest->set==1){
		pipest->set=0;
		cmd->pipe=0;
		runpipe(cmd,pipest->rcmd);
	}
	else if(cmd->inp!=0||cmd->oup!=0){
		ioredexec(cmd);
		
	}
	else if(checkcmd(cmd->arguments[0])==1){
		int pid=fork();
		if(pid!=0){
			int status;
			wait(&status);
		}	
		else{
		execcmd(cmd);
		}		
	
	}
	}
}
int main(){
	char line[argsize];
	char line1[argsize];
	pipest=malloc(sizeof(struct pipest));
	pipest->lcmd=0;
	pipest->rcmd=0;
	cmd=malloc(sizeof(struct cmd));
	parellelcmd=malloc(sizeof(struct parellelcmd));
	succexeccmd=malloc(sizeof(struct succexeccmd));
	cmd->inp=0;
	cmd->oup=0;
		
	while(1){	
	printstr("MyShell>",1);
	if(readcmd(line)==0){
		//printf(2,"%d",line[0]);
		preprocs(line,line1);
		//printstr(line1,0);
		parseandexec(line1);
		
	}
	}	
	return 0;
}
