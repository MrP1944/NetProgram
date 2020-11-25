#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include <assert.h>
#include <unistd.h>
#include <sched.h>

int numCores=-1;
FILE **openTemp;
time_t rightnow;


////find how many cores the computer have
int findCores(){
	
	FILE* takecpu = fopen("/proc/cpuinfo","r");
	assert(takecpu != NULL);
	char* cores = malloc(sizeof(char) * 200);
	while(~fscanf(takecpu,"%s",cores)){
		
		if(strcmp(cores,"cores") == 0){
			
			fscanf(takecpu,"%s",cores);
			fscanf(takecpu,"%s",cores);
			break;
		}
	}
	fclose(takecpu);
	return atoi(cores);
}

////read temprautre from file at hwmon2/tempX_input
void readTemp(int Temp[]){

	char* dir = malloc(sizeof(char)*300);
	for(int i=0;i < numCores;i++){
		sprintf(dir,"/sys/devices/platform/coretemp.0/hwmon/hwmon2/temp%d_input",i+2);
		openTemp[i] = fopen(dir,"r");
		assert(openTemp[i] != NULL);
		fscanf(openTemp[i],"%d",&Temp[i]);
	}
	rightnow = time(NULL);

	for(int i=0;i < numCores;i++)
		fscanf(openTemp[i],"%d",&Temp[i]);
	for(int i=0;i < numCores;i++)
		fclose(openTemp[i]);

}

////print all temprautre stored in Temp[]
void printTemp(int Temp[]){

	for(int i=0;i<numCores;i++)
		printf("Core %d:\t%d °C\n",i,Temp[i]/1000);
}

////compare all temp lower than limit or not
bool cmpTemp(int Temp[],int limit){
	
	bool LTl=1;//// all cores Lower than limit

	for(int i=0;i<numCores;i++){
		if(Temp[i] > limit){
			LTl = 0;
			break;
		}//end if Temp[i] > limit

	}//end of for i
	return LTl;
}

int main(int argc, char* argv[]){


	numCores = findCores();	
	openTemp = malloc(sizeof(FILE*) * numCores);
	int limit = 50;
	int Temp[numCores];
	if(argc > 1)
		limit = atoi(argv[1]);
	limit*=1000; //casue tempX_input is 10^-3 tempC
	////////init
	
	printf("Start to read temperautre of all cores\n");
	readTemp(Temp);
	printf("@ %s",ctime(&rightnow));
	printTemp(Temp);
	////print time and core's temperautre

	while(cmpTemp(Temp,limit) != 1){ //while all cores not lower than limit
		
		readTemp(Temp);
		sleep(1);//update pre second
	}
	printf("Got temperautre of all cores lower than %d°C\n",limit/1000);
	printf("@ %s",ctime(&rightnow));
	printTemp(Temp);
	////print result

	return 0;
}
