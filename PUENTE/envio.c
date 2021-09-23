#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <signal.h>
#include<sys/shm.h>
#include <time.h>

#define REG_CURRENT_TASK _IOW('a','a',int32_t*)
 
#define SIGETX 44
 
static int done = 0;
int check = 0;
 
void ctrl_c_handler(int n, siginfo_t *info, void *unused){
    if (n == SIGINT) {
        printf("\nabortando\n");
        done = 1;
    }
}
 
void sig_event_handler(int n, siginfo_t *info, void *unused){
    if (n == SIGETX) {
        check = info->si_int;
        printf ("Received signal from kernel : Value =  %u\n", check);
    }
}
 
int main(){
    int fd;
    int32_t value, number;
    struct sigaction act;
    
    /*Intervalo de tiempo establecido por el usuario*/
    int rango;
    printf("Establezca el intervalo de tiempo para las pulsaciones: ");
    scanf("%d",&rango);
 
    /* install ctrl-c interrupt handler to cleanup at exit */
    sigemptyset (&act.sa_mask);
    act.sa_flags = (SA_SIGINFO | SA_RESETHAND);
    act.sa_sigaction = ctrl_c_handler;
    sigaction (SIGINT, &act, NULL);
 
    /* install custom signal handler */
    sigemptyset(&act.sa_mask);
    act.sa_flags = (SA_SIGINFO | SA_RESTART);
    act.sa_sigaction = sig_event_handler;
    sigaction(SIGETX, &act, NULL);
 
    printf("Installed signal handler for SIGETX = %d\n", SIGETX);
 
    printf("\nOpening Driver\n");
    fd = open("/dev/etx_device", O_RDWR);
    if(fd < 0) {
            printf("Cannot open device file...\n");
            return 0;
    }
 
    printf("Registering application ...");
    /* register this task with kernel for signal */
    if (ioctl(fd, REG_CURRENT_TASK,(int32_t*) &number)) {
        printf("Failed\n");
        close(fd);
        exit(1);
    }
    printf("Done!!!\n");
   
    while(1) {
	if(check){
		int i;
		void *memo1;
		void *memo2;
		int conteo;
		int shmid;
		int shmid2;
		int bool1=0;
		time_t now;
		struct tm *ts;
		char buf[80];
		/* Get the current time */
		now = time(NULL);
		/* Format and print the time, "hh:mm:ss" */
		ts = localtime(&now);
		strftime(buf, sizeof(buf), "%H:%M:%S", ts);
		shmid=shmget((key_t)2345, 1024, 0666);
		shmid2=shmget((key_t)2346, 1024, 0666);
		printf("Key of shared memory is -------------------------> %d\n",shmid);
		printf("Key of shared memory is -------------------------> %d\n",shmid2);
		if(shmid==-1){
		
		//FALTA VERIFICAR ESO AL MOMENTO DE ENCENDER LA COMPU HACER OTRA PRUEBA, SINO DA METER EL IF QUE FUE BORRADO
		//	shmid=shmget((key_t)2345, 1024, 0666|IPC_CREAT);
		//	bool1=1;
		}
		if(shmid2==-1){
			shmid=shmget((key_t)2346, 1024, 0666|IPC_CREAT);
		}
		memo1=shmat(shmid,NULL,0);
		memo2=shmat(shmid2,NULL,0);
		//tiempo de la ultima pulsacion
		struct tm tm;
		strptime(memo2, "%H:%M:%S", &tm);
		time_t t = mktime(&tm);
		//tiempo actual
		int sec = ts->tm_sec - tm.tm_sec;
		printf("Execution time = %d\n", sec);
		if(sec >= rango || sec < 0){
			conteo = 1;
		} else {
			int flag = 0;
			for(int a = 0; a < rango; a++){
				char zz[10];
				sprintf(zz,"%d",conteo);
				if(strcmp(memo1, zz) == 0){
					conteo = conteo +1;
					flag = 1;
					break;
				}
			}
			if(flag == 0){
				conteo = 1;
			}
		} 
		char a[2] ;
		*a= conteo+'0';
		strcpy(memo1,a);
		printf("You wrote : %s\n",(char *)memo1);
		memo2=shmat(shmid2,NULL,0); 
		strcpy(memo2,buf);
		printf("You wrote : %s\n",(char *)memo2);
		printf("end");	
		check=!check;
	 }
    }
    printf("Closing Driver\n");
    close(fd);
}
