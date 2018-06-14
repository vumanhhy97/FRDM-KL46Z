/**
Copyright (c) 2008 Freescale Semiconductor
Freescale Confidential Proprietary
\file       rtc.h
\brief      real time clock basic functionality
\author     Freescale Semiconductor
\version    1.0
\date       Sep 14, 2011
*/

#define ALARM_TIME 5   


void default_time_date(void);


extern uint32 alarm_seconds_count;
extern uint32 rtc_seconds_isrv_count;


typedef struct { 
  char seconds;
  char minutes;
  char hours;
  char weekday;
  
  char day;
  char month;
  int  year;
}time_date_t;




extern time_date_t *current_time;

void rtc_init(void);
void set_time(time_date_t * td, char hr, char min, char sec);
void set_date(time_date_t * td, char day, char month, int year);
