#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

int main(){
	char cmd[4] = "";
	char msg[10] = "";
	//char msg[4] = "";
	printf("Enter a number to guess code or enter \'stop\': ");
	 //scanf("%d", guess);
	scanf("%s",cmd);
	printf("%s\n",cmd);
	if(strcmp(cmd,"stop") == 0)
		printf("Is string: stop\n");
	else if(isdigit(cmd[0])){
		int guess = (int) (cmd[0] - '0');
		printf("%d\n", guess);
		sprintf(msg, "TRY%d",guess);
	}
	else{
		printf("Invalid Input\n");
	}
	

	//int guess = (int)(cmd[0] - '0');
	//printf("%d\n",guess);
	
	/*if(isdigit(cmd)){
		int guess = cmd[0] - '0';
		printf("%d\n",guess);
	}
	if(isalpha(cmd))
		printf("%s\n",cmd);*/
}