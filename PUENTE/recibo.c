#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<unistd.h>
#include<sys/shm.h>

int main(){
	int id;
	void *mem;
	while(1){
		id=shmget((key_t)2345, 1024, 0666);
		if(id!=-1){
			mem = shmat(id,NULL,0);
			char* temp = (char *) mem;
			if(temp == "1"){
				printf("He sido pulsado %s vez\n",temp);		
			} else {
				printf("He sido pulsado %s veces\n",temp);
			}
		} else {
			printf(" ");
		}
	sleep(1);	
	}
}
