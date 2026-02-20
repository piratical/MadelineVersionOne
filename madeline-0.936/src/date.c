/* ********************************************************************************* */
/*  _______________________________________________________________________________  */
/* |_______________________________________________________________________________| */
/*   __    __       _       ______     _______   _          _   __    _   _______    */
/*  |  \  /  |     / \     |  ___  \  |  _____| | |        | | |  \  | | |  _____|   */
/*  |   \/   |    / ^ \    | |   \  \ | |___    | |        | | |   \ | | | |___      */
/*  | |\  /| |   / /_\ \   | |    | | |  ___|   | |        | | | |\ \| | |  ___|     */
/*  | | \/ | |  /  ___  \  | |___/  / | |_____  | |______  | | | | \   | | |_____    */
/*  |_|    |_| /__/   \__\ |_______/  |_______| |________| |_| |_|  \__| |_______|   */
/*  _______________________________________________________________________________  */
/* |_______________________________________________________________________________| */
/*                                                                                   */
/* "date.c" is part of the Madeline software distribution.                           */
/* Written by Edward H. Trager.                                                      */
/*                                                                                   */
/* COPYRIGHT (C) 2000 THE REGENTS OF THE UNIVERSITY OF MICHIGAN,                     */
/* Ann Arbor, Michigan, USA.  ALL RIGHTS RESERVED.                                   */
/*                                                                                   */
/* This program is free software; you can redistribute it and/or                     */
/* modify it under the terms of the GNU General Public                               */
/* License as published by the Free Software Foundation; either                      */
/* version 2 of the License, or (at your option) any later version.                  */
/*                                                                                   */
/* This program is distributed in the hope that it will be useful,                   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of                    */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU                 */
/* Library General Public License for more details.                                  */
/*                                                                                   */
/* You should have received a copy of the GNU Library General Public                 */
/* License along with this software source code distribution; if not,                */
/* write to the Free Software Foundation, Inc., 59 Temple Place -                    */
/* Suite 330, Boston, MA 02111-1307, USA.                                            */
/*                                                                                   */
/* Last modified by Ed Trager, March 2000.                                           */
/*                                                                                   */
/* ********************************************************************************* */


/*                                                     */
/*   date.c                                            */
/*                                                     */
/*                                                     */
/*   Routines to support date manipulation in Madeline */
/*                                                     */
/*   Initiated: 980609 ET                              */
/*                                                     */

#include "date.h"

/* for Madeline error reporting: */
#include "logs.h"

/*                                       */
/*      DateErrorReport                  */
/*                                       */
/*      Simple error reporting mechanism */
/*      used in prototyping date.c       */
/*                                       */
void DateErrorReport(char *s,...){
	
	va_list argptr;

	fprintf(stderr,"\nDate processor report:\n");
	va_start(argptr,s);
	vfprintf(stderr,s,argptr);
	va_end(argptr);        
	fprintf(stderr,"\n");
	exit(1);

}


/*                                                               */
/* JulianDay(): returns the Julian Day from                      */
/* the supplied month mm, day dd, and year yyyy:                 */
/* Positive years signify A.D., negative B.C.  Remember that the */
/* year after 1 B.C. was 1 A.D.                                  */
/*                                                               */
long JulianDay(int mm, int dd, int yyyy){

	long julian;
	int ja,jy,jm;
	
	if(yyyy==0) MadelineErrorReport("JulianDay(): There is no year zero in the Western calendar.");
	if(yyyy==1582 && mm==10 && dd>04 && dd<15) MadelineErrorReport("JulianDay(): October 5th to 14th were skipped in October of 1582\nwhen Pope Gregory XIII implemented the calendar reforms\nof the commission headed by the Jesuit mathematician and astronomer\nChristoph Clavius.");
	if(yyyy<0) yyyy++;
	if(mm>2){
		jy=yyyy;
		jm=mm+1;
	}else{
		jy=yyyy-1;
		jm=mm+13;
	}
	julian=(long)(floor(365.25*jy)+floor(30.6001*jm)+dd+1720995);
	if(dd+31L*(mm+12L*yyyy)>=IGREG){
		/*                                                             */
		/* Original code from Press et. al. does not show cast here,   */
		/* but assignment to 'int' from 'double' will generate warning */
		/* without cast:                                               */
		/*                                                             */
		ja=(int)(0.01*jy);
		julian+= 2-ja+(int)(0.25*ja);
	}
	return julian;
}


/*                                                            */
/* CalendarDate(): Stores the month, mm, day, dd, and year,   */
/* yyyy, in the int addresses based on the julian day passed: */
/*                                                            */
/*                                                            */
void CalendarDate(long julian,int *mm,int *dd,int *yyyy){

	long ja,jalpha,jb,jc,jd,je;
	long gregorianStart  = 2299161;
	
	if(julian>=gregorianStart){
		/*                                                                */
		/* Original code from Press et. al. does not show cast to         */
		/* long here, but missing that will generate warning on ANSI C++  */
		/* compilers:                                                     */
		/*                                                                */
		jalpha=(long)(((double)(julian-1867216)-0.25)/36524.25);
		ja=julian+1+jalpha-(long)(0.25*jalpha);
	}else{
		ja=julian;
	}
	/*                                                              */
	/* Assignment to 'long int' from 'double' will generate warning */
	/* on ANSI C++ compilers:                                       */
	/*                                                              */
	jb=ja+1524;
	jc=(long)(6680.0+((double)(jb-2439870)-122.1)/365.25);
	jd=(long)(365*jc+(0.25*jc));
	je=(long)((jb-jd)/30.6001);
	*dd=jb-jd-(int)(30.6001*je);
	*mm=je-1;
	if(*mm>12) *mm-=12;
	*yyyy=jc-4715;
	if(*mm>2) --(*yyyy);
	if(*yyyy<=0) --(*yyyy);
}


/*             */
/* DayOfWeek() */
/*             */
int DayOfWeek(long julian){

	return (julian+1)%7;

}

/*                                                      */
/* CurrentJulianDay(): returns the julian day for today */
/*                                                      */
long CurrentJulianDay(void){
	
	time_t current_time;
	struct tm *t;
	long julian;
	
	/*                                                              */
	/* record current time:                                         */
	/*                                                              */
	/* the ANSI C time() function returns the time, in seconds, and */
	/* when a long is 4 bytes, as it usually is, has                */
	/* a range of only 68 years!                                    */
	/*                                                              */
	time(&current_time);
	/*                                                               */
	/* convert time_t information to local time in struct tm format: */
	/*                                                               */
	t=localtime(&current_time);
	/*                                            */
	/* convert to julian day:                     */
	/*                                            */
	/* -- the tm struct count years since 1900:   */
	/* -- ... and months (0-11) __since__ January */
	/*                                            */
	julian=JulianDay(t->tm_mon+1,t->tm_mday,t->tm_year+1900);
	
	return julian;
	
}


/*                      */
/* char *CurrentTime(): */
/*                      */
void CurrentTime(int *hour,int *min){

	time_t current_time;
	struct tm *t;
	/*                                                              */
	/* record current time:                                         */
	/* the ANSI C time() function returns the time, in seconds, and */
	/* when a long is 4 bytes, as it usually is, has                */
	/* a range of only 68 years!                                    */
	/*                                                              */
	time(&current_time);
	/*                                                               */
	/* convert time_t information to local time in struct tm format: */
	/*                                                               */
	t=localtime(&current_time);
	*hour = t->tm_hour;
	*min  = t->tm_min;
	return;
	
}

/*                                                               */
/*  IsDateDelimiter(): Returns 1 if c==' ' or '/' or '-' or '.'  */
/*                                                               */
int IsDateDelimiter(char c){

   return (c=='/' || c==',' || c=='.' || c==' ' || c=='-');

}

/*                                                             */
/* Read_YYYY_MM_DD_DateString(): Read a date in YYYY.MM.DD     */
/* format only                                                 */
/*                                                             */
long Read_YYYYMMDD_DateString(const char *s){

	char    hold[11];  /* "DD.MM.YYYY" or similar format plus null terminator */
	int     dd;
	int     mm;
	int     yyyy;
	int     i;

	/*                        */
	/* copy string into hold: */
	/*                        */
	strncpy(hold,s,10);
	/*                 */
	/* Check if empty: */
	/*                 */
	if(!*hold) return MISSINGDATE;
	/*                                              */
	/* Check whether delimiters are at the correct  */
	/* positions for reading YYYY.MM.DD:            */
	/*                                              */
	if(IsDateDelimiter(hold[4]) && IsDateDelimiter(hold[7])){
		/* Make we are only dealing with digits */
		/* in addition to the known delimiters  */
		for(i=0;i<10;i++){
			if(i==4 || i==7) continue;
			if(!isdigit(hold[i])) return MISSINGDATE;
		}
		/* read day first: */
		dd=atoi(hold+8);
		if(dd<0 || dd>31){
			return MISSINGDATE;
		}
		hold[7]='\0';
		/* read month next: */
		mm=atoi(hold+5);
		if(mm<1 || mm>12){
			return MISSINGDATE;
		}
		hold[4]='\0';
		/* finally, read the year */
		yyyy=atoi(hold);
		/* convert to julian day and return */
		return JulianDay(mm,dd,yyyy);
	}else{
		return MISSINGDATE;
	}
}



/*                                                             */
/* JulianToday(): returns today's date as a julian day integer */
/*                                                             */
long JulianToday(void){

	time_t current_time;
	struct tm *t;
	
	/*                                                              */
	/* record current time:                                         */
	/*                                                              */
	/* the ANSI C time() function returns the time, in seconds, and */
	/* when a long is 4 bytes, as it usually is, has                */
	/* a range of only 68 years!                                    */
	/*                                                              */
	time(&current_time);
	/*                                                               */
	/* convert time_t information to local time in struct tm format: */
	/*                                                               */
	t=localtime(&current_time);
	/*                                            */
	/* convert to julian day:                     */
	/*                                            */
	/* -- the tm struct count years since 1900:   */
	/* -- ... and months (0-11) __since__ January */
	/*                                            */
	return JulianDay(t->tm_mon+1,t->tm_mday,t->tm_year+1900);
}


/*                   */
/* GenericDateString */
/*                   */
char *GenericDateString(long julian){

	static char s[DATESTRINGLENGTH];
	int m,d,y;
	
	if(julian==MISSINGDATE) return (char *)DATEMISSINGSTRING;
	/*                           */
	/* Convert to calendar date: */
	/*	                          */
	CalendarDate(julian,&m,&d,&y);
	/*                                        */
	/* format date string in YYYYMMDD format: */
	/*                                        */
	sprintf(s,"%04i%02i%02i",y,m,d);
	return s;
}

/*                            */
/* DelimitedGenericDateString */
/*                            */
char *DelimitedGenericDateString(long julian,char dlm){

	static char s[DATESTRINGLENGTH];
	int m,d,y;
	
	if(julian==MISSINGDATE) {
		sprintf(s,"0000%c00%c00",dlm,dlm);
		return s;
	}
	/*                           */
	/* Convert to calendar date: */
	/*                           */
	CalendarDate(julian,&m,&d,&y);
	/*                                                              */
	/* if dlm='\0', the result is the same as in GenericDateString: */
	/*                                                              */
	if(dlm=='\0') sprintf(s,"%04i%02i%02i",y,m,d);
	/*                                                              */
	/* otherwise format date string in YYYY.MM.DD format:           */
	/*                                                              */
	else          sprintf(s,"%04i%c%02i%c%02i",y,dlm,m,dlm,d);
	return s;
	
}

//
// DateToJulian(): Wrapper for printing out the Julian day number from a date
//
double DateToJulian( double jdn ){ 
	
	return jdn;
	
}

//
// double JulianToDate(): Wrapper for printing out the Date from a Julian number
//
double JulianToDate( double jdn ){
	
	return jdn;
	
}

//
// Today(): Wrapper for printing out today's date
//          --> Note bene: the double argument is not used, but is retained
//              so that we can use the existing TNRDFUNCTION category instead
//              of having to invent a new category.
//              
double Today( double offset ){
	
	return (double) JulianToday() + offset;
	
}

