#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
int
main(int argc, char *argv[]){
	char buffer[1024];
	sprintf(buffer,"./prepare_N");
	if(system(buffer) == -1){
		fprintf(stderr, "Fail to execute command: \"%s\"\n",buffer);
	}
	return 0;
}
