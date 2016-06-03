/**************************************************************************
 * File Name : parentCtrol
 *
 * Description: 
 *   use ebatbles to control Children's surface time
 *
 * parentControl validate $weekdays $t1_start $t1_end $t2_start $t2_end $currentWeeekday $currentTime $lastValidatevalue
 * 
 * eg. parentControl validate 1,2,3,4 17:44 12:00 17:45 24:00 3 11:30 10   
 * -->check if it need to refresh ebtables rules current time
 *
 * parentControl process $weekdays $mac $t1_enable $t1_start $t1_end $t2_enable $t2_start $t2_end $current_weekday $current_time
 *
 * eg.parentControl process  1,2,3,4  00:17:d0:11:22:33 1 11:20 15:00 0 10:00 12:00 5 09:05
 * -->process if it need to create relative rbtables rules 
 *
 * Author:Cai Juanjuan
 * Updates : 02/06/2016 Created.
 ***************************************************************************/


/** Includes. **/

#include <stdio.h>
#include <string.h> 


static int inWeekdas(const char *weekdays,const char *currentWeekday)
{
	if(strstr(weekdays, currentWeekday))
	{
		return 1;
	}
	return 0;
} 


static int currentTimeIsLargeThanTheTime(const char* time,const char* currentTime)
{
	int ctime_hour;
	int ctime_minute;
	int time_hour;
	int time_minute;
	sscanf(currentTime,"%d:%d",&ctime_hour,&ctime_minute);
	sscanf(time,"%d:%d",&time_hour,&time_minute);
	 
	if (( ctime_hour == time_hour)&&( ctime_minute == time_minute))
		return 0;
	else if(( ctime_hour == time_hour)&&( ctime_minute > time_minute))	
		return 1;
	else if(( ctime_hour == time_hour)&&( ctime_minute < time_minute))	
		return -1;
	else if( ctime_hour < time_hour)	
		return -1;
	else if( ctime_hour > time_hour)	
		return 1;	
}

static int isInTime(const char* startTime, const char* endTime,const char* crurrentTime)
{
	/*
	printf(" %d %d\n",
	currentTimeIsLargeThanTheTime(startTime,crurrentTime),
	currentTimeIsLargeThanTheTime(endTime,crurrentTime));
	 */
	if(( currentTimeIsLargeThanTheTime(startTime,crurrentTime) >= 0 ) 
		&& ( currentTimeIsLargeThanTheTime(endTime,crurrentTime) <= 0 )  )
		return 1;
	else  
		return 0;
}

static int needFreshRules(const char* weekdays, const char* startTime, const char* endTime,
							const char* currentWeekday, const char* crurrentTime)
{
	if(inWeekdas(weekdays,currentWeekday))
	{
		if (( currentTimeIsLargeThanTheTime(startTime,crurrentTime) == 0)
		  || (currentTimeIsLargeThanTheTime(endTime,crurrentTime) == 0) )
		{
			return 1;
		}
	}
	return 0;
}

void addRules(const char* macAddr)
{
	char cmd[128] = "";
	sprintf(cmd,"ebtables -I PARENT_CONTROL -s %s -j DROP",macAddr);
	//printf("\nAdd ebtables\n%s\n\n",cmd);
	system(cmd);
}


void processRule(const char* weekdays,const char* macAddr,
				const char* t1_enable,const char* t1_start, const char* t1_end,
				const char* t2_enable, const char* t2_start, const char* t2_end, 
				const char* currentWeekday, const char* currentTime)
{

	//printf("inWeekdas: %d \n",inWeekdas(weekdays,currentWeekday));

	if( inWeekdas(weekdays,currentWeekday) )
	{

		//printf("t1_enable: %d \n",atoi(t1_enable));
		//printf("t2_enable: %d \n",atoi(t2_enable));
		//printf("c[%s] [%s]-[%s] needDrop: %d \n",currentTime,t1_start,t1_end,isInTime(t1_start,t1_end,currentTime));
		//printf("c[%s] [%s]-[%s] needDrop: %d \n",currentTime,t2_start,t2_end,isInTime(t2_start,t2_end,currentTime));
		
		if( (atoi(t1_enable) && isInTime(t1_start,t1_end,currentTime))
		   || (atoi(t2_enable) && isInTime(t2_start,t2_end,currentTime)) )
		{
			
		}   
		else
			addRules(macAddr);
	}
	else
		addRules(macAddr);
}

int main(int argc, char **argv)
{
	char rule_week[32] = "";
	char mac[32]="";
	char t1_enable[4] = "";
	char t1_start[8] = "";
	char t1_end[8]="";
	char t2_enable[4] = "";
	char t2_start[8] = "";
	char t2_end[8]="";
	
	char current_weekday[4]="";
	char current_time[8]="";
	int oldValue = 0;
	char cmd[16]="";
	
	strcpy(cmd,argv[1]);
	fprintf(stderr,"\nCJJ %s:%s:%d: -------cmd:%s   \n",__FILE__,__FUNCTION__,__LINE__,cmd);

	if( !strcmp(cmd,"process") )
	{	
		strcpy(rule_week,argv[2]);
		strcpy(mac,argv[3]);
		strcpy(t1_enable,argv[4]);
		strcpy(t1_start,argv[5]);
		strcpy(t1_end,argv[6]);
		strcpy(t2_enable,argv[7]);
		strcpy(t2_start,argv[8]);
		strcpy(t2_end,argv[9]);
		strcpy(current_weekday,argv[10]);
		strcpy(current_time,argv[11]);
		processRule(rule_week,mac,t1_enable,t1_start,t1_end,t2_enable,t2_start,t2_end,current_weekday,current_time);
	}
	else if( !strcmp(argv[1],"validate") )
	{
		strcpy(rule_week,argv[2]);
		strcpy(t1_start,argv[3]);
		strcpy(t1_end,argv[4]);		
		strcpy(t2_start,argv[5]);
		strcpy(t2_end,argv[6]);
		strcpy(current_weekday,argv[7]);
		strcpy(current_time,argv[8]);
		oldValue = atoi(argv[9]);
		oldValue += needFreshRules(rule_week,t1_start,t1_end,current_weekday,current_time);
		oldValue +=  needFreshRules(rule_week,t2_start,t2_end,current_weekday,current_time);		
		fprintf(stderr,"%d", oldValue);
	}	
	fprintf(stderr,"\nCJJ %s:%s:%d: -------cmd:%s   \n",__FILE__,__FUNCTION__,__LINE__,cmd);
	return oldValue;
}
