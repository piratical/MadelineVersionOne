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
/* "languageconventions.h" is part of the Madeline software distribution.            */
/* Written by Edward H. Trager.                                                      */
/*                                                                                   */
/* COPYRIGHT (C) 2003 THE REGENTS OF THE UNIVERSITY OF MICHIGAN,                     */
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
/* General Public License for more details.                                          */
/*                                                                                   */
/* You should have received a copy of the GNU General Public                         */
/* License along with this software source code distribution; if not,                */
/* write to the Free Software Foundation, Inc., 59 Temple Place -                    */
/* Suite 330, Boston, MA 02111-130DAY_SLOTS, USA.                                    */
/*                                                                                   */
/* Last modified by Ed Trager, March 2003.                                           */
/*                                                                                   */
/* ********************************************************************************* */

/*                                                      */
/* languageconventions.c                                */
/*                                                      */
/*                                                      */
/* history:                                             */
/*                                                      */
/*                                                      */
#include "languageconventions.h"
#include "variablesandarrays.h"
#include "logs.h"
 
/*                                  */
/* GetLanguageConventionFromLocale  */
/*                                  */
LANGUAGE GetLanguageConventionFromLocale(void){
	
	char *LC_ALL, *LC_CTYPE, *LC_TIME, *LANG, *LC=NULL;
	
	char *UTF8 = "UTF-8";
	
	//
	// NOTE BENE: Excepting the default "C" or "POSIX"
	// locales, Madeline *only* deals with UTF-8 locales:
	// Non-UTF-8 locales are treated like the "C"
	// locale and one gets AMERICAN_ENGLISH by default.
	// 
	// Since the language convention setting in Madeline
	// is currently limited to defining how dates are displayed,
	// we put LC_TIME in second precedence after LC_ALL:
	// 
	if     ( (LC_ALL   = getenv("LC_ALL"  )) && strstr(LC_ALL  ,UTF8)) LC = LC_ALL;
	else if( (LC_TIME  = getenv("LC_TIME" )) && strstr(LC_TIME ,UTF8)) LC = LC_TIME;
	else if( (LC_CTYPE = getenv("LC_CTYPE")) && strstr(LC_CTYPE,UTF8)) LC = LC_CTYPE;
	else if( (LANG     = getenv("LANG"    )) && strstr(LANG    ,UTF8)) LC = LANG;
	
	if(LC){
		//
		// Excellent! The user is using a UTF-8 locale!
		// So, we will now check for their language settings:
		// 
		if(strstr(LC,"en_")){
			//
			// Well, we are going to make a big *assumption*
			// here that most places use British conventions.
			// We are going to assume that only the
			// Americans and Canadians use the American
			// conventions for printing the dates, and that
			// everybody else does it the British way.  This
			// way, we don't have to check specific
			// "en_XX.UTF-8" locales that we may not even 
			// know about.  So, the code works for all English
			// UTF-8 locales, and is probably even correct for
			// the majority of those locales ;-):
			// 
			if(strstr(LC,"CA") || strstr(LC,"US")) return AMERICAN_ENGLISH;
			else                                   return BRITISH_ENGLISH;
		}
		//
		// This is too simplistic: in the future we
		// must distinguish Egyptian and non-Egyptian 
		// Arabic:
		// 
		if(strstr(LC,"ar_")) return ARABIC;
		if(strstr(LC,"zh_")) return CHINESE;
		if(strstr(LC,"fi_")) return FINNISH;
		if(strstr(LC,"fr_")) return FRENCH;
		if(strstr(LC,"de_")) return GERMAN;
		if(strstr(LC,"el_")) return GREEK;
		if(strstr(LC,"it_")) return ITALIAN;
		if(strstr(LC,"ja_")) return JAPANESE;
		if(strstr(LC,"pt_")) return PORTUGESE;
		if(strstr(LC,"ru_")) return RUSSIAN;
		if(strstr(LC,"es_")) return SPANISH;
		if(strstr(LC,"th_")) return THAI;
		
	}
	//
	// Default: If you aren't using a UTF-8 locale,
	// then we default to the American convention:
	// 
	return AMERICAN_ENGLISH;
	
}

/*                                         */
/* GENERIC ROUTINES THAT ARE NOT LANGUAGE- */
/* SPECIFIC to be used when there are no   */
/* differences between the generic and     */
/* locale, -- or when a specific routine   */
/* hasn't been written yet!                */
/*                                         */

char *EuropeanPrintBriefDate(long julian){

	static char s[DATESTRINGLENGTH];
	int m,d,y;
	
	if(julian==MISSINGDATE) return (char *)CHARMISSINGSTRING;
	/*                           */
	/* Convert to calendar date: */
	/*                           */
	CalendarDate(julian,&m,&d,&y);
	/* DD.MM.YYYY: */
	sprintf(s,"%i.%i.%i%s",
		d,
		m,
		y<0?-y:y,
		y<0?" BCE":""
	);
	return s;
}

/*                               */
/* AMERICAN AND BRITISH ENGLISH: */
/*                               */

char *EnglishMonths[MONTH_SLOTS]={
	"Unknown Month",
	"January",
	"February",
	"March",
	"April",
	"May",
	"June",
	"July",
	"August",
	"September",
	"October",
	"November",
	"December"
};

char *EnglishMonthAbbreviations[MONTH_SLOTS]={
	"???",
	"Jan",
	"Feb",
	"Mar",
	"Apr",
	"May",
	"Jun",
	"Jul",
	"Aug",
	"Sep",
	"Oct",
	"Nov",
	"Dec"
};

char *EnglishDays[DAY_SLOTS]={
	"Sunday",
	"Monday",
	"Tuesday",
	"Wednesday",
	"Thursday",
	"Friday",
	"Saturday"
};

char *WesternDigits[DIGIT_SLOTS]={
	"0",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9"
};

char *EnglishPrintDate(long julian){
	
	static char s[DATESTRINGLENGTH];
	int y,m,d;
	
	if(julian==MISSINGDATE) return (char *)CHARMISSINGSTRING;
	/*                           */
	/* Convert to calendar date: */
	/*                           */
	CalendarDate(julian,&m,&d,&y);
	
	sprintf(s,"%s, %s %i, %i%s",
		EnglishDays[DayOfWeek(julian)],
		EnglishMonths[m],
		d,
		y<0?-y:y,
		y<0?" BCE":""
	);
	
	return s;
	
}

char *AmericanPrintBriefDate(long julian){

	static char s[DATESTRINGLENGTH];
	int m,d,y;
	
	if(julian==MISSINGDATE) return (char *)CHARMISSINGSTRING;
	/*                           */
	/* Convert to calendar date: */
	/*                           */
	CalendarDate(julian,&m,&d,&y);
	/* MM/DD/YYYY */
	sprintf(s,"%i/%i/%i%s",
		m,
		d,
		y<0?-y:y,
		y<0?" BCE":""
	);
	return s;
}

/*                                                                   */
/* EnglishCurrentDate(): returns a pointer to a static object that   */
/* may be overwritten by other calls:                                */
/*                                                                   */
char *EnglishCurrentDate(void){
	
	/*                                                 */
	/* return static char * containing formatted date: */
	/*                                                 */
	return EnglishPrintDate(CurrentJulianDay());
	
}

/*                             */
/* char *EnglishCurrentTime(): */
/*                             */
char *EnglishCurrentTime(void){

	time_t current_time;
	struct tm *t;
	long julian;
	static char s[DATESTRINGLENGTH];
	
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
	/*                                            */
	/* convert to julian day:                     */
	/*                                            */
	/* -- the tm struct count years since 1900:   */
	/* -- ... and months (0-11) __since__ January */
	/*                                            */
	julian=JulianDay(t->tm_mon+1,t->tm_mday,t->tm_year+1900);
	
	sprintf(s,"%02i:%02i %s",
		t->tm_hour,
		t->tm_min,
		EnglishPrintDate(julian)
		);
	
	return s;
	
}

LANGUAGEOBJECT American={
	"American English",
	"American English",
	EnglishMonths,
	EnglishMonthAbbreviations,
	EnglishDays,
	WesternDigits,
	EnglishPrintDate,
	AmericanPrintBriefDate,
	EnglishCurrentDate,
	EnglishCurrentTime,
	Read_YYYYMMDD_DateString
};

 LANGUAGEOBJECT British={
	"British English",
	"British English",
	EnglishMonths,
	EnglishMonthAbbreviations,
	EnglishDays,
	WesternDigits,
	EnglishPrintDate,
	EuropeanPrintBriefDate,
	EnglishCurrentDate,
	EnglishCurrentTime,
	Read_YYYYMMDD_DateString
};

/*                               */
/* ARABIC:                       */
/*                               */
char *ArabicMonths[MONTH_SLOTS]={
	"Unknown Month",
	"كانون ثاني",
	"شباط",
	"آذار",
	"نيسان",
	"أيار",
	"حزيران",
	"تموز",
	"آب",
	"أيلول",
	"تشرين أول",
	"تشرين ثاني",
	"كانون أول"
};

char *ArabicMonthAbbreviations[MONTH_SLOTS]={
	"???",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	""
};


char *ArabicDays[DAY_SLOTS]={
	"ألأحد",
	"ألأثنين",
	"ألثلاثاء",
	"ألأربعاء",
	"ألخميس",
	"ألجمعة",
	"ألسبت"
};

char *ArabicDigits[DIGIT_SLOTS]={
	"٠",
	"١",
	"٢",
	"٣",
	"٤",
	"٥",
	"٦",
	"٧",
	"٨",
	"٩"
};

char *ArabicPrintDate(long julian){
	
	static char s[DATESTRINGLENGTH];
	int y,m,d;
	
	if(julian==MISSINGDATE) return (char *)CHARMISSINGSTRING;
	/*                           */
	/* Convert to calendar date: */
	/*                           */
	CalendarDate(julian,&m,&d,&y);
	sprintf(s,"%s %i %s %i%s",
		ArabicDays[DayOfWeek(julian)],
		d,
		ArabicMonths[m],
		y<0?-y:y,
		y<0?"BCE":""
	);
	
	return s;
	
}

char *ArabicPrintBriefDate(long julian){

	static char s[DATESTRINGLENGTH];
	int m,d,y;
	
	if(julian==MISSINGDATE) return (char *)CHARMISSINGSTRING;
	/*                           */
	/* Convert to calendar date: */
	/*                           */
	CalendarDate(julian,&m,&d,&y);
	/* DD.MM.YYYY: */
	sprintf(s,"%i%s/%i/%i",
		y<0?-y:y,
		y<0?" BCE":"",
		m,
		d
	);
	return s;
}

/*                                                                   */
/* ArabicCurrentDate(): returns a pointer to a static object that   */
/* may be overwritten by other calls:                                */
/*                                                                   */
char *ArabicCurrentDate(void){
	
	/*                                                 */
	/* return static char * containing formatted date: */
	/*                                                 */
	return ArabicPrintDate(CurrentJulianDay());
	
}

/*                            */
/* char *ArabicCurrentTime(): */
/*                            */
char *ArabicCurrentTime(void){

	time_t current_time;
	struct tm *t;
	long julian;
	static char s[DATESTRINGLENGTH];
	
	time(&current_time);
	t=localtime(&current_time);
	julian=JulianDay(t->tm_mon+1,t->tm_mday,t->tm_year+1900);
	sprintf(s,"%02i:%02i %s",
		t->tm_hour,
		t->tm_min,
		ArabicPrintDate(julian)
	);
	return s;
	
}

LANGUAGEOBJECT Arabic={
	"Arabic",
	"اﻟﻠﻐﺔ",
	ArabicMonths,
	ArabicMonthAbbreviations,
	ArabicDays,
	ArabicDigits,
	ArabicPrintDate,
	ArabicPrintBriefDate,
	ArabicCurrentDate,
	ArabicCurrentTime,
	Read_YYYYMMDD_DateString
};


/*                               */
/* CHINESE (MANDARIN):           */
/*                               */
char *ChineseMonths[MONTH_SLOTS]={
	"Unknown Month",
	"一月",
	"二月",
	"三月",
	"四月",
	"五月",
	"六月",
	"七月",
	"八月",
	"九月",
	"十月",
	"十一月",
	"十二月"
};

char *ChineseMonthAbbreviations[MONTH_SLOTS]={
	"???",
	"一月",
	"二月",
	"三月",
	"四月",
	"五月",
	"六月",
	"七月",
	"八月",
	"九月",
	"十月",
	"十一月",
	"十二月"
};

char *ChineseDays[DAY_SLOTS]={
	"星期天",
	"星期一",
	"星期二",
	"星期三",
	"星期四",
	"星期五",
	"星期六"
};

char *HanDigits[DIGIT_SLOTS]={
	"0",
	"一",
	"二",
	"三",
	"四",
	"五",
	"六",
	"七",
	"八",
	"九"
};

char *ChinesePrintDate(long julian){
	
	static char s[DATESTRINGLENGTH];
	int y,m,d;
	
	if(julian==MISSINGDATE) return (char *)CHARMISSINGSTRING;
	/*                           */
	/* Convert to calendar date: */
	/*                           */
	CalendarDate(julian,&m,&d,&y);
	sprintf(s,"%s%i年%i月%i日 (%s)",
		y<0?"公元前":"",
		y<0?-y:y,
		m,
		d,
		ChineseDays[DayOfWeek(julian)]
	);
	return s;
	
}

char *ChinesePrintBriefDate(long julian){

	static char s[DATESTRINGLENGTH];
	int m,d,y;
	
	if(julian==MISSINGDATE) return (char *)CHARMISSINGSTRING;
	/*                           */
	/* Convert to calendar date: */
	/*                           */
	CalendarDate(julian,&m,&d,&y);
	/* DD.MM.YYYY: */
	sprintf(s,"%i.%i.%i%s",
		d,
		m,
		y<0?-y:y,
		y<0?" BCE":""
	);
	return s;
}

/*                                                                   */
/* ChineseCurrentDate(): returns a pointer to a static object that   */
/* may be overwritten by other calls:                                */
/*                                                                   */
char *ChineseCurrentDate(void){
	
	/*                                                 */
	/* return static char * containing formatted date: */
	/*                                                 */
	return ChinesePrintDate(CurrentJulianDay());
	
}

/*                             */
/* char *ChineseCurrentTime(): */
/*                             */
char *ChineseCurrentTime(void){

	time_t current_time;
	struct tm *t;
	long julian;
	static char s[DATESTRINGLENGTH];
	
	time(&current_time);
	t=localtime(&current_time);
	julian=JulianDay(t->tm_mon+1,t->tm_mday,t->tm_year+1900);
	sprintf(s,"%s %02i:%02i",
		ChinesePrintDate(julian),
		t->tm_hour,
		t->tm_min
	);
	return s;
	
}

 LANGUAGEOBJECT Chinese={
	"Chinese",
	"中文",
	ChineseMonths,
	ChineseMonthAbbreviations,
	ChineseDays,
	HanDigits,
	ChinesePrintDate,
	ChinesePrintBriefDate,
	ChineseCurrentDate,
	ChineseCurrentTime,
	Read_YYYYMMDD_DateString
};


/*                  */
/* SUOMI (FINNISH): */
/*                  */
char *FinnishMonths[MONTH_SLOTS]={
	"unknown month",
	"tammikuu",
	"helmikuu",
	"maaliskuu",
	"huhtikuu",
	"tuokokuu",
	"kesäkuu",
	"heinäkuu",
	"elokuu",
	"syyskuu",
	"lokakuu",
	"marraskuu",
	"joulukuu"
};

char *FinnishMonthAbbreviations[MONTH_SLOTS]={
	"???",
	"tammi",
	"helmi",
	"maalis",
	"huhti",
	"tuoko",
	"kesä",
	"heinä",
	"elo",
	"syys",
	"loka",
	"marras",
	"joulu"
};

char *FinnishDays[DAY_SLOTS]={
	"sunnuntai",
	"maanantai",
	"tiistai",
	"keskiviikko",
	"torstai",
	"perjantai",
	"lauantai"
};

char *FinnishPrintDate(long julian){
	
	static char s[DATESTRINGLENGTH];
	int y,m,d;
	
	if(julian==MISSINGDATE) return (char *)CHARMISSINGSTRING;
	/*                           */
	/* Convert to calendar date: */
	/*                           */
	CalendarDate(julian,&m,&d,&y);
	sprintf(s,"%s %i.%i.%i%s",
		FinnishDays[DayOfWeek(julian)],
		d,
		m,
		y<0?-y:y,
		y<0?" eKr.":""
	);
	
	return s;
	
}

char *FinnishPrintBriefDate(long julian){

	static char s[DATESTRINGLENGTH];
	int m,d,y;
	
	if(julian==MISSINGDATE) return (char *)CHARMISSINGSTRING;
	/*                           */
	/* Convert to calendar date: */
	/*                           */
	CalendarDate(julian,&m,&d,&y);
	sprintf(s,"%i.%i.%i%s",
		d,
		m,
		y<0?-y:y,
		y<0?" eKr.":""
	);
	return s;
}

/*                                                                   */
/* FinnishCurrentDate(): returns a pointer to a static object that   */
/* may be overwritten by other calls:                                */
/*                                                                   */
char *FinnishCurrentDate(void){
	
	/*                                                 */
	/* return static char * containing formatted date: */
	/*                                                 */
	return FinnishPrintDate(CurrentJulianDay());
	
}

char *FinnishCurrentTime(void){

	time_t current_time;
	struct tm *t;
	long julian;
	static char s[DATESTRINGLENGTH];
	
	time(&current_time);
	t=localtime(&current_time);
	julian=JulianDay(t->tm_mon+1,t->tm_mday,t->tm_year+1900);
	sprintf(s,"%s, %02i:%02i",
		FinnishPrintDate(julian),
		t->tm_hour,
		t->tm_min
	);
	return s;
	
}

 LANGUAGEOBJECT Finnish={
	"Finnish",
	"Suomi",
	FinnishMonths,
	FinnishMonthAbbreviations,
	FinnishDays,
	WesternDigits,
	FinnishPrintDate,
	FinnishPrintBriefDate,
	FinnishCurrentDate,
	FinnishCurrentTime,
	Read_YYYYMMDD_DateString
};

/*         */
/* FRENCH: */
/*         */
char *FrenchMonths[MONTH_SLOTS]={
	"unknown month",
	"janvier",
	"février",
	"mars",
	"avril",
	"mai",
	"juin",
	"juillet",
	"août",
	"septembre",
	"octobre",
	"novembre",
	"décembre"
};

char *FrenchMonthAbbreviations[MONTH_SLOTS]={
	"???",
	"jan",
	"fév",
	"mar",
	"avr",
	"mai",
	"jun",
	"jul",
	"auo",
	"sep",
	"oct",
	"nov",
	"déc"
};

char *FrenchDays[DAY_SLOTS]={
	"dimanche",
	"lundi",
	"mardi",
	"mercredi",
	"jeudi",
	"vendredi",
	"samedi"
};

char *FrenchPrintDate(long julian){
	
	static char s[DATESTRINGLENGTH];
	int y,m,d;
	
	if(julian==MISSINGDATE) return (char *)CHARMISSINGSTRING;
	/*                           */
	/* Convert to calendar date: */
	/*                           */
	CalendarDate(julian,&m,&d,&y);
	sprintf(s,"le %s %2i %s %4i%s",
		FrenchDays[DayOfWeek(julian)],
		d,
		FrenchMonths[m],
		y<0?-y:y,
		y<0?" avant J.C.":""
	);
	
	return s;
	
}

char *FrenchPrintBriefDate(long julian){

	static char s[DATESTRINGLENGTH];
	int m,d,y;
	
	if(julian==MISSINGDATE) return (char *)CHARMISSINGSTRING;
	/*                           */
	/* Convert to calendar date: */
	/*                           */
	CalendarDate(julian,&m,&d,&y);
	sprintf(s,"%i.%i.%i%s",
		d,
		m,
		y<0?-y:y,
		y<0?" avant J.C.":""
	);
	return s;
}

/*                                                                  */
/* FrenchCurrentDate(): returns a pointer to a static object that   */
/* may be overwritten by other calls:                               */
/*                                                                  */
char *FrenchCurrentDate(void){
	
	/*                                                 */
	/* return static char * containing formatted date: */
	/*                                                 */
	return FrenchPrintDate(CurrentJulianDay());
	
}

/*                            */
/* char *FrenchCurrentTime(): */
/*                            */
char *FrenchCurrentTime(void){

	time_t current_time;
	struct tm *t;
	long julian;
	static char s[DATESTRINGLENGTH];
	
	time(&current_time);
	t=localtime(&current_time);
	julian=JulianDay(t->tm_mon+1,t->tm_mday,t->tm_year+1900);
	sprintf(s,"%02i:%02i %s",
		t->tm_hour,
		t->tm_min,
		FrenchPrintDate(julian)
	);
	
	return s;
	
}

 LANGUAGEOBJECT French={
	"French",
	"Français",
	FrenchMonths,
	FrenchMonthAbbreviations,
	FrenchDays,
	WesternDigits,
	FrenchPrintDate,
	FrenchPrintBriefDate,
	FrenchCurrentDate,
	FrenchCurrentTime,
	Read_YYYYMMDD_DateString
};


/*                               */
/* GERMAN:                       */
/*                               */
char *GermanMonths[MONTH_SLOTS]={
	"Unknown Month",
	"Januar",
	"Februar",
	"März",
	"April",
	"Mai",
	"Juni",
	"Juli",
	"August",
	"September",
	"Oktober",
	"November",
	"Dezember"
};

char *GermanMonthAbbreviations[MONTH_SLOTS]={
	"???",
	"Jan",
	"Feb",
	"Mär",
	"Apr",
	"Mai",
	"Jun",
	"Jul",
	"Aug",
	"Sep",
	"Okt",
	"Nov",
	"Dez"
};

char *GermanDays[DAY_SLOTS]={
	"Sonntag",
	"Montag",
	"Dienstag",
	"Mittwoch",
	"Donnerstag",
	"Freitag",
	"Samstag"
};

char *GermanPrintDate(long julian){
	
	static char s[DATESTRINGLENGTH];
	int y,m,d;
	
	if(julian==MISSINGDATE) return (char *)CHARMISSINGSTRING;
	/*                           */
	/* Convert to calendar date: */
	/*                           */
	CalendarDate(julian,&m,&d,&y);
	sprintf(s,"%s, %i. %s %i%s",
		GermanDays[DayOfWeek(julian)],
		d,
		GermanMonths[m],
		y<0?-y:y,
		y<0?" vor der Zeitenwende":""
	);
	return s;
	
}

char *GermanPrintBriefDate(long julian){
	
	static char s[DATESTRINGLENGTH];
	int y,m,d;
	
	if(julian==MISSINGDATE) return (char *)CHARMISSINGSTRING;
	/*                           */
	/* Convert to calendar date: */
	/*                           */
	CalendarDate(julian,&m,&d,&y);
	sprintf(s,"%i.%i.%i%s",
		d,
		m,
		y<0?-y:y,
		y<0?" vor der Zeitenwende":""
	);
	return s;
	
}

/*                                                                   */
/* GermanCurrentDate(): returns a pointer to a static object that   */
/* may be overwritten by other calls:                                */
/*                                                                   */
char *GermanCurrentDate(void){
	
	/*                                                 */
	/* return static char * containing formatted date: */
	/*                                                 */
	return GermanPrintDate(CurrentJulianDay());
	
}

/*                            */
/* char *GermanCurrentTime(): */
/*                            */
char *GermanCurrentTime(void){

	time_t current_time;
	struct tm *t;
	long julian;
	static char s[DATESTRINGLENGTH];
	
	time(&current_time);
	t=localtime(&current_time);
	julian=JulianDay(t->tm_mon+1,t->tm_mday,t->tm_year+1900);
	sprintf(s,"%02i:%02i %s",
		t->tm_hour,
		t->tm_min,
		GermanPrintDate(julian)
	);
	return s;
	
}

 LANGUAGEOBJECT German={
	"German",
	"Deutsch",
	GermanMonths,
	GermanMonthAbbreviations,
	GermanDays,
	WesternDigits,
	GermanPrintDate,
	GermanPrintBriefDate,
	GermanCurrentDate,
	GermanCurrentTime,
	Read_YYYYMMDD_DateString
};


/*                               */
/* Greek:                        */
/*                               */
char *GreekMonths[MONTH_SLOTS]={
	"Unknown Month",
	"Ιανουάριος",
	"Φεβρουάριος",
	"Μάρτιος",
	"Απρίλιος",
	"Μάϊος",
	"Ιούνιος",
	"Ιούλιος",
	"Αύγουστος",
	"Σεπτέμβριος",
	"Οκτώβριος",
	"Νοέμβριος",
	"Δεκέμβριος"
};

char *GreekMonthAbbreviations[MONTH_SLOTS]={
	"???",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	""
};

char *GreekDays[DAY_SLOTS]={
	"Κυριακή",
	"Δευτέρα",
	"Τρίτη",
	"Τετάρτη",
	"Πέμπτη",
	"Παρασκευή",
	"Σάββατο"
};

char *GreekPrintDate(long julian){
	
	static char s[DATESTRINGLENGTH];
	int y,m,d;
	
	if(julian==MISSINGDATE) return (char *)CHARMISSINGSTRING;
	/*                           */
	/* Convert to calendar date: */
	/*                           */
	CalendarDate(julian,&m,&d,&y);
	sprintf(s,"%s, %i %s %i%s",
		GreekDays[DayOfWeek(julian)],
		d,
		GreekMonths[m],
		y<0?-y:y,
		y<0?" BCE":""
	);
	return s;
	
}

char *GreekPrintBriefDate(long julian){
	
	static char s[DATESTRINGLENGTH];
	int y,m,d;
	
	if(julian==MISSINGDATE) return (char *)CHARMISSINGSTRING;
	/*                           */
	/* Convert to calendar date: */
	/*                           */
	CalendarDate(julian,&m,&d,&y);
	sprintf(s,"%i.%i.%i%s",
		d,
		m,
		y<0?-y:y,
		y<0?" BCE":""
	);
	return s;
	
}

/*                                                                   */
/* GreekCurrentDate(): returns a pointer to a static object that     */
/* may be overwritten by other calls:                                */
/*                                                                   */
char *GreekCurrentDate(void){
	
	/*                                                 */
	/* return static char * containing formatted date: */
	/*                                                 */
	return GreekPrintDate(CurrentJulianDay());
	
}

/*                            */
/* char *GreekCurrentTime(): */
/*                            */
char *GreekCurrentTime(void){

	time_t current_time;
	struct tm *t;
	long julian;
	static char s[DATESTRINGLENGTH];
	
	time(&current_time);
	t=localtime(&current_time);
	julian=JulianDay(t->tm_mon+1,t->tm_mday,t->tm_year+1900);
	sprintf(s,"%02i:%02i %s",
		t->tm_hour,
		t->tm_min,
		GreekPrintDate(julian)
	);
	return s;
	
}

LANGUAGEOBJECT Greek={
	"Greek",
	"",
	GreekMonths,
	GreekMonthAbbreviations,
	GreekDays,
	WesternDigits,
	GreekPrintDate,
	GreekPrintBriefDate,
	GreekCurrentDate,
	GreekCurrentTime,
	Read_YYYYMMDD_DateString
};

/*                               */
/* Italian:                      */
/*                               */
char *ItalianMonths[MONTH_SLOTS]={
	"Unknown Month",
	"gennaio",
	"febbraio",
	"marzo",
	"aprile",
	"maggio",
	"giugno",
	"luglio",
	"agosto",
	"settembre",
	"ottobre",
	"novembre",
	"dicembre"
};

char *ItalianMonthAbbreviations[MONTH_SLOTS]={
	"???",
	"gen",
	"feb",
	"mar",
	"apr",
	"mag",
	"giu",
	"lug",
	"ago",
	"set",
	"ott",
	"nov",
	"dic"
};

char *ItalianDays[DAY_SLOTS]={
	"domenica",
	"lunedí",
	"martedí",
	"mercoledé",
	"giovedé",
	"venerdé",
	"sabato"
};

char *ItalianPrintDate(long julian){
	
	static char s[DATESTRINGLENGTH];
	int y,m,d;
	
	if(julian==MISSINGDATE) return (char *)CHARMISSINGSTRING;
	/*                           */
	/* Convert to calendar date: */
	/*                           */
	CalendarDate(julian,&m,&d,&y);
	sprintf(s,"%s, %i %s %i%s",
		ItalianDays[DayOfWeek(julian)],
		d,
		ItalianMonths[m],
		y<0?-y:y,
		y<0?" BCE":""
	);
	return s;
	
}

char *ItalianPrintBriefDate(long julian){
	
	static char s[DATESTRINGLENGTH];
	int y,m,d;
	
	if(julian==MISSINGDATE) return (char *)CHARMISSINGSTRING;
	/*                           */
	/* Convert to calendar date: */
	/*                           */
	CalendarDate(julian,&m,&d,&y);
	sprintf(s,"%i.%i.%i%s",
		d,
		m,
		y<0?-y:y,
		y<0?" BCE":""
	);
	return s;
	
}

/*                                                                     */
/* ItalianCurrentDate(): returns a pointer to a static object that     */
/* may be overwritten by other calls:                                  */
/*                                                                     */
char *ItalianCurrentDate(void){
	
	/*                                                 */
	/* return static char * containing formatted date: */
	/*                                                 */
	return ItalianPrintDate(CurrentJulianDay());
	
}

/*                             */
/* char *ItalianCurrentTime(): */
/*                             */
char *ItalianCurrentTime(void){

	time_t current_time;
	struct tm *t;
	long julian;
	static char s[DATESTRINGLENGTH];
	
	time(&current_time);
	t=localtime(&current_time);
	julian=JulianDay(t->tm_mon+1,t->tm_mday,t->tm_year+1900);
	sprintf(s,"%s ore %02i:%02i",
		ItalianPrintDate(julian),
		t->tm_hour,
		t->tm_min
	);
	return s;
	
}

LANGUAGEOBJECT Italian={
	"Italian",
	"Italiano",
	ItalianMonths,
	ItalianMonthAbbreviations,
	ItalianDays,
	WesternDigits,
	ItalianPrintDate,
	ItalianPrintBriefDate,
	ItalianCurrentDate,
	ItalianCurrentTime,
	Read_YYYYMMDD_DateString
};


/*                               */
/* JAPANESE:                     */
/*                               */
char *JapaneseMonths[MONTH_SLOTS]={
	"Unknown Month",
	"いちがつ",
	"にがつ",
	"さんがつ",
	"しがつ",
	"ごがつ",
	"ろくがつ",
	"しちがつ",
	"はちがつ",
	"くがつ",
	"じゅうがつ",
	"じゅういちがつ",
	"じゅうにがつ"
};

char *JapaneseMonthAbbreviations[MONTH_SLOTS]={
	"???",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	""
};

char *JapaneseDays[DAY_SLOTS]={
	"日曜日",
	"月曜日",
	"火曜日",
	"水曜日",
	"木曜日",
	"金曜日",
	"土曜日"
};
char *JapanesePrintDate(long julian){
	
	static char s[DATESTRINGLENGTH];
	int y,m,d;
	
	if(julian==MISSINGDATE) return (char *)CHARMISSINGSTRING;
	/*                           */
	/* Convert to calendar date: */
	/*                           */
	CalendarDate(julian,&m,&d,&y);
	sprintf(s,"%s%i年%i月%i日 (%s)",
		y<0?"紀元前":"",
		y<0?-y:y,
		m,
		d,
		JapaneseDays[DayOfWeek(julian)]
	);
	
	return s;
	
}

char *JapanesePrintBriefDate(long julian){

	static char s[DATESTRINGLENGTH];
	int m,d,y;
	
	if(julian==MISSINGDATE) return (char *)CHARMISSINGSTRING;
	/*                           */
	/* Convert to calendar date: */
	/*                           */
	CalendarDate(julian,&m,&d,&y);
	/* DD.MM.YYYY: */
	sprintf(s,"%s%i年%i月%i日",
		y<0?"紀元前":"",
		y,
		m,
		d
	);
	return s;
}

/*                                                                   */
/* JapaneseCurrentDate(): returns a pointer to a static object that   */
/* may be overwritten by other calls:                                */
/*                                                                   */
char *JapaneseCurrentDate(void){
	
	/*                                                 */
	/* return static char * containing formatted date: */
	/*                                                 */
	return JapanesePrintDate(CurrentJulianDay());
	
}

/*                              */
/* char *JapaneseCurrentTime(): */
/*                              */
char *JapaneseCurrentTime(void){

	time_t current_time;
	struct tm *t;
	long julian;
	static char s[DATESTRINGLENGTH];
	
	time(&current_time);
	t=localtime(&current_time);
	julian=JulianDay(t->tm_mon+1,t->tm_mday,t->tm_year+1900);
	sprintf(s,"%s %02i時%02i分",
		JapanesePrintDate(julian),
		t->tm_hour,
		t->tm_min
	);
	return s;
	
}

LANGUAGEOBJECT Japanese={
	"Japanese",
	"日本語",
	JapaneseMonths,
	JapaneseMonthAbbreviations,
	JapaneseDays,
	HanDigits,
	JapanesePrintDate,
	JapanesePrintBriefDate,
	JapaneseCurrentDate,
	JapaneseCurrentTime,
	Read_YYYYMMDD_DateString
};


/*                               */
/* Portugese (Brazilian)         */
/*                               */
char *PortugeseMonths[MONTH_SLOTS]={
	"Unknown Month",
	"janeiro",
	"fevereiro",
	"março",
	"abril",
	"maio",
	"junho",
	"julho",
	"agosto",
	"setembro",
	"outubro",
	"novembro",
	"dezembro"
};

char *PortugeseMonthAbbreviations[MONTH_SLOTS]={
	"???",
	"jan",
	"fev",
	"mar",
	"abr",
	"mai",
	"jun",
	"jul",
	"ago",
	"set",
	"out",
	"nov",
	"dez"
};

char *PortugeseDays[DAY_SLOTS]={
	"domingo",
	"segunda-feira",
	"terça-feira",
	"quarta-feira",
	"quinta-feira",
	"sexta-feira",
	"sábado"
};

char *PortugesePrintDate(long julian){
	
	static char s[DATESTRINGLENGTH];
	int y,m,d;
	
	if(julian==MISSINGDATE) return (char *)CHARMISSINGSTRING;
	/*                           */
	/* Convert to calendar date: */
	/*                           */
	CalendarDate(julian,&m,&d,&y);
	sprintf(s,"%s, %i de %s de %i%s",
		PortugeseDays[DayOfWeek(julian)],
		d,
		PortugeseMonths[m],
		y<0?-y:y,
		y<0?" BCE":""
	);
	return s;
	
}

char *PortugesePrintBriefDate(long julian){
	
	static char s[DATESTRINGLENGTH];
	int y,m,d;
	
	if(julian==MISSINGDATE) return (char *)CHARMISSINGSTRING;
	/*                           */
	/* Convert to calendar date: */
	/*                           */
	CalendarDate(julian,&m,&d,&y);
	sprintf(s,"%i.%i.%i%s",
		d,
		m,
		y<0?-y:y,
		y<0?" BCE":""
	);
	return s;
	
}

/*                                                                     */
/* PortugeseCurrentDate(): returns a pointer to a static object that   */
/* may be overwritten by other calls:                                  */
/*                                                                     */
char *PortugeseCurrentDate(void){
	
	/*                                                 */
	/* return static char * containing formatted date: */
	/*                                                 */
	return PortugesePrintDate(CurrentJulianDay());
	
}

/*                               */
/* char *PortugeseCurrentTime(): */
/*                               */
char *PortugeseCurrentTime(void){

	time_t current_time;
	struct tm *t;
	long julian;
	static char s[DATESTRINGLENGTH];
	
	time(&current_time);
	t=localtime(&current_time);
	julian=JulianDay(t->tm_mon+1,t->tm_mday,t->tm_year+1900);
	sprintf(s,"%s ore %02i:%02i",
		PortugesePrintDate(julian),
		t->tm_hour,
		t->tm_min
	);
	return s;
	
}

LANGUAGEOBJECT Portugese={
	"Portugese",
	"",
	PortugeseMonths,
	PortugeseMonthAbbreviations,
	PortugeseDays,
	WesternDigits,
	PortugesePrintDate,
	PortugesePrintBriefDate,
	PortugeseCurrentDate,
	PortugeseCurrentTime,
	Read_YYYYMMDD_DateString
};

/*                             */
/* Russian                     */
/*                             */
char *RussianMonths[MONTH_SLOTS]={
	"Unknown Month",
	"январь",
	"февраль",
	"март",
	"апрель",
	"май",
	"июнь",
	"июль",
	"август",
	"сентябрь",
	"октябрь",
	"ноябрь",
	"декабрь"
};

char *RussianMonthAbbreviations[MONTH_SLOTS]={
	"???",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	""
};

char *RussianDays[DAY_SLOTS]={
	"воскресенье",
	"понедельник",
	"вторник",
	"среда",
	"четверг",
	"пятница",
	"суббота"
};

char *RussianPrintDate(long julian){
	
	static char s[DATESTRINGLENGTH];
	int y,m,d;
	
	if(julian==MISSINGDATE) return (char *)CHARMISSINGSTRING;
	/*                           */
	/* Convert to calendar date: */
	/*                           */
	CalendarDate(julian,&m,&d,&y);
	sprintf(s,"%s, %i %s %i%s",
		RussianDays[DayOfWeek(julian)],
		d,
		RussianMonths[m],
		y<0?-y:y,
		y<0?" BCE":""
	);
	return s;
	
}

char *RussianPrintBriefDate(long julian){
	
	static char s[DATESTRINGLENGTH];
	int y,m,d;
	
	if(julian==MISSINGDATE) return (char *)CHARMISSINGSTRING;
	/*                           */
	/* Convert to calendar date: */
	/*                           */
	CalendarDate(julian,&m,&d,&y);
	sprintf(s,"%i.%i.%i%s",
		d,
		m,
		y<0?-y:y,
		y<0?" BCE":""
	);
	return s;
	
}

/*                                                                     */
/* RussianCurrentDate(): returns a pointer to a static object that   */
/* may be overwritten by other calls:                                  */
/*                                                                     */
char *RussianCurrentDate(void){
	
	/*                                                 */
	/* return static char * containing formatted date: */
	/*                                                 */
	return RussianPrintDate(CurrentJulianDay());
	
}

/*                               */
/* char *RussianCurrentTime(): */
/*                               */
char *RussianCurrentTime(void){

	time_t current_time;
	struct tm *t;
	long julian;
	static char s[DATESTRINGLENGTH];
	
	time(&current_time);
	t=localtime(&current_time);
	julian=JulianDay(t->tm_mon+1,t->tm_mday,t->tm_year+1900);
	sprintf(s,"%s ore %02i:%02i",
		RussianPrintDate(julian),
		t->tm_hour,
		t->tm_min
	);
	return s;
	
}

LANGUAGEOBJECT Russian={
	"Russian",
	"",
	RussianMonths,
	RussianMonthAbbreviations,
	RussianDays,
	WesternDigits,
	RussianPrintDate,
	RussianPrintBriefDate,
	RussianCurrentDate,
	RussianCurrentTime,
	Read_YYYYMMDD_DateString
};


/*                               */
/* SPANISH:                      */
/*                               */
char *SpanishMonths[MONTH_SLOTS]={
	"Unknown Month",
	"enero",
	"febrero",
	"marzo",
	"abril",
	"mayo",
	"junio",
	"julio",
	"agosto",
	"septiembre",
	"octubre",
	"noviembre",
	"diciembre"
};

char *SpanishMonthAbbreviations[MONTH_SLOTS]={
	"???",
	"ene",
	"feb",
	"mar",
	"abr",
	"may",
	"jun",
	"jul",
	"ago",
	"sep",
	"oct",
	"nov",
	"dic"
};


char *SpanishDays[DAY_SLOTS]={
	"domingo",
	"lunes",
	"martes",
	"miércoles",
	"jueves",
	"viernes",
	"sábado"
};

char *SpanishPrintDate(long julian){
	
	static char s[DATESTRINGLENGTH];
	int y,m,d;
	
	if(julian==MISSINGDATE) return (char *)CHARMISSINGSTRING;
	/*                           */
	/* Convert to calendar date: */
	/*                           */
	CalendarDate(julian,&m,&d,&y);
	sprintf(s,"%s, %i de %s de %i%s",
		SpanishDays[DayOfWeek(julian)],
		d,
		SpanishMonths[m],
		y<0?-y:y,
		y<0?" AC":""
	);
	return s;
	
}

char *SpanishPrintBriefDate(long julian){

	static char s[DATESTRINGLENGTH];
	int m,d,y;
	
	if(julian==MISSINGDATE) return (char *)CHARMISSINGSTRING;
	/*                           */
	/* Convert to calendar date: */
	/*                           */
	CalendarDate(julian,&m,&d,&y);
	/* DD.MM.YYYY: */
	sprintf(s,"%i/%i/%i%s",
		d,
		m,
		y<0?-y:y,
		y<0?" AC":""
	);
	return s;
}

/*                                                                   */
/* SpanishCurrentDate(): returns a pointer to a static object that   */
/* may be overwritten by other calls:                                */
/*                                                                   */
char *SpanishCurrentDate(void){
	
	/*                                                 */
	/* return static char * containing formatted date: */
	/*                                                 */
	return SpanishPrintDate(CurrentJulianDay());
	
}

/*                             */
/* char *SpanishCurrentTime(): */
/*                             */
char *SpanishCurrentTime(void){

	time_t current_time;
	struct tm *t;
	long julian;
	static char s[DATESTRINGLENGTH];
	
	time(&current_time);
	t=localtime(&current_time);
	julian=JulianDay(t->tm_mon+1,t->tm_mday,t->tm_year+1900);
	sprintf(s,"%02i:%02i %s",
		t->tm_hour,
		t->tm_min,
		SpanishPrintDate(julian)
	);
	return s;
	
}

LANGUAGEOBJECT Spanish={
	"Spanish",
	"Español",
	SpanishMonths,
	SpanishMonthAbbreviations,
	SpanishDays,
	WesternDigits,
	SpanishPrintDate,
	SpanishPrintBriefDate,
	SpanishCurrentDate,
	SpanishCurrentTime,
	Read_YYYYMMDD_DateString
};


/*                               */
/* THAI:                         */
/*                               */
char *ThaiMonths[MONTH_SLOTS]={
	"Unknown Month",
	"มกราคม",
	"กุมภาพันธ์",
	"มีนาคม",
	"เมษายน",
	"พฤษภาคม",
	"มิถุนายน",
	"กรกฏาคม",
	"สิงหาคม",
	"กันยายน",
	"ตุลาคม",
	"พฤศจิกายน",
	"ธันวาคม"
};

char *ThaiMonthAbbreviations[MONTH_SLOTS]={
	"???",
	"ม.ค.",
	"ก.พ.",
	"ม.ค.",
	"เม.ย.",
	"พ.ค.",
	"ม.ย.",
	"ก.ค.",
	"ส.ค.",
	"ก.ย.",
	"ต.ค.",
	"พ.ย.",
	"ธ.ค."
};

char *ThaiDays[DAY_SLOTS]={
	"วันอาทิตย์",
	"วันจันทร์",
	"วันอังคาร",
	"วันพุธ",
	"วันพฤหัสบดี",
	"วันศุกร์ู",
	"วันเสาร์"
};

char *ThaiDigits[DIGIT_SLOTS]={
	"๐",
	"๑",
	"๒",
	"๓",
	"๔",
	"๕",
	"๖",
	"๗",
	"๘",
	"๙"
};

char *ThaiPrintDate(long julian){
	
	static char s[DATESTRINGLENGTH];
	int y,m,d;
	
	if(julian==MISSINGDATE) return (char *)CHARMISSINGSTRING;
	/*                           */
	/* Convert to calendar date: */
	/*                           */
	CalendarDate(julian,&m,&d,&y);
	/* Years expressed in Buddhist Era: add 543 */
	y+=543;
	
	sprintf(s,"%sที่่ %i %s พ.ศ. %4i %s",
		ThaiDays[DayOfWeek(julian)],
		d,
		ThaiMonths[m],
		y<0?-y:y,
		y<0?"":""
	);
	return s;
	
}

char *ThaiPrintBriefDate(long julian){

	static char s[DATESTRINGLENGTH];
	int m,d,y;
	
	if(julian==MISSINGDATE) return (char *)CHARMISSINGSTRING;
	/*                           */
	/* Convert to calendar date: */
	/*                           */
	CalendarDate(julian,&m,&d,&y);
	/* DD.MM.YYYY: */
	y+=543;
	sprintf(s,"%i %s %i%s",
		d,
		ThaiMonthAbbreviations[m],
		y<0?-y:y,
		y<0?" (ก.พ.ศ.)":""
	);
	return s;
}

/*                                                                   */
/* ThaiCurrentDate(): returns a pointer to a static object that      */
/* may be overwritten by other calls:                                */
/*                                                                   */
char *ThaiCurrentDate(void){
	
	/*                                                 */
	/* return static char * containing formatted date: */
	/*                                                 */
	return ThaiPrintDate(CurrentJulianDay());
	
}

/*                          */
/* char *ThaiCurrentTime(): */
/*                          */
char *ThaiCurrentTime(void){

	time_t current_time;
	struct tm *t;
	long julian;
	static char s[DATESTRINGLENGTH];
	
	time(&current_time);
	t=localtime(&current_time);
	julian=JulianDay(t->tm_mon+1,t->tm_mday,t->tm_year+1900);
	sprintf(s,"%02i:%02i %s",
		t->tm_hour,
		t->tm_min,
		ThaiPrintDate(julian)
	);
	return s;
	
}

 LANGUAGEOBJECT Thai={
	"Thai",
	"ไทย",
	ThaiMonths,
	ThaiMonthAbbreviations,
	ThaiDays,
	ThaiDigits,
	ThaiPrintDate,
	ThaiPrintBriefDate,
	ThaiCurrentDate,
	ThaiCurrentTime,
	Read_YYYYMMDD_DateString
};

/*                                       */
/* ARRAY OF POINTERS TO LANGUAGE OBJECTS */
/*                                       */
PLANGUAGEOBJECT Language[LANGUAGE_COUNT]={
	&American, 
	&British,
	&Arabic,
	&Chinese,
	&Finnish,
	&French,
	&German,
	&Greek,
	&Italian,
	&Japanese,
	&Portugese,
	&Russian,
	&Spanish,
	&Thai
};

/*                              */
/* END OF LANGUAGECONVENTIONS.C */
/*                              */

