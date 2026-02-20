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
/* COPYRIGHT (C) 2002 THE REGENTS OF THE UNIVERSITY OF MICHIGAN,                     */
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
/* Suite 330, Boston, MA 02111-1307, USA.                                            */
/*                                                                                   */
/* Last modified by Ed Trager, April 18, 2002.                                       */
/*                                                                                   */
/* ********************************************************************************* */

/*                                                      */
/* languageconventions.h                                */
/*                                                      */
/*                                                      */
/* history:                                             */
/*                                                      */
/* 2002.04.18.ET: Segregated out of the old "shared.h"  */
/*                                                      */

/*                          */
/* LANGUAGES in Madeline:   */
/*                          */
/* NOTE BENE: These are     */
/* really used to define    */
/* the conventions used for */
/* both reading and writing */
/* dates.  Values is stored */
/* in g->lang:              */
/*                          */

#include "date.h"

#define DATESTRINGLENGTH 100
#define MONTH_SLOTS  13
#define DAY_SLOTS     7
#define DIGIT_SLOTS  10

#define MASK80 0x80;
#define MASKC0 0xC0;
#define MASKE0 0xE0;
#define MASKF0 0xF0;

/*                                 */
/* Enumeration of Language locales */
/* Always put LANGUAGE_COUNT last: */
/*                                 */
/*                                 */
enum LANGUAGE {
	
	AMERICAN_ENGLISH=0, 
	BRITISH_ENGLISH,
	ARABIC,
	CHINESE,
	FINNISH,
	FRENCH,
	GERMAN,
	GREEK,
	ITALIAN,
	JAPANESE,
	PORTUGESE,
	RUSSIAN,
	SPANISH,
	THAI,
	LANGUAGE_COUNT
};


typedef struct sLANGUAGEOBJECT {
	
	char *EnglishName; /* Name of the language (or locale) in English     */
	char *NativeName;  /* Name of the language or locale in this language */
	
	char **Months;
	char **MonthAbbreviations;
	char **Days;
	char **Digits;
	
	char *(*PrintDate)(long julian);
	char *(*PrintBriefDate)(long julian);
	char *(*CurrentTime)(void);
	char *(*CurrentDate)(void);
	long (*ReadDate)(const char *);
	
} LANGUAGEOBJECT, *PLANGUAGEOBJECT, **PPLANGUAGEOBJECT;

/* AMERICAN AND BRITISH ENGLISH */
/* AND "GENERIC"                */
char *EuropeanPrintBriefDate(long julian);
char *EnglishPrintDate(long julian);
char *AmericanPrintBriefDate(long julian);
char *EnglishCurrentDate(void);
char *EnglishCurrentTime(void);

/* ARABIC */
char *ArabicPrintDate(long julian);
char *ArabicPrintBriefDate(long julian);
char *ArabicCurrentDate(void);
char *ArabicCurrentTime(void);

/* CHINESE */
char *ChinesePrintDate(long julian);
char *ChinesePrintBriefDate(long julian);
char *ChineseCurrentDate(void);
char *ChineseCurrentTime(void);

/* FINNISH */
char *FinnishPrintDate(long julian);
char *FinnishPrintBriefDate(long julian);
char *FinnishCurrentDate(void);
char *FinnishCurrentTime(void);

/* FRENCH */
char *FrenchPrintDate(long julian);
char *FrenchPrintBriefDate(long julian);
char *FrenchCurrentDate(void);
char *FrenchCurrentTime(void);

/* GERMAN */
char *GermanPrintDate(long julian);
char *GermanPrintBriefDate(long julian);
char *GermanCurrentDate(void);
char *GermanCurrentTime(void);

/* ITALIAN */
char *ItalianPrintDate(long julian);
char *ItalianPrintBriefDate(long julian);
char *ItalianCurrentDate(void);
char *ItalianCurrentTime(void);

/* JAPANESE */
char *JapanesePrintDate(long julian);
char *JapanesePrintBriefDate(long julian);
char *JapaneseCurrentDate(void);
char *JapaneseCurrentTime(void);

/* PORTUGESE */
char *PortugesePrintDate(long julian);
char *PortugesePrintBriefDate(long julian);
char *PortugeseCurrentDate(void);
char *PortugeseCurrentTime(void);

/* RUSSIAN */
char *RussianPrintDate(long julian);
char *RussianPrintBriefDate(long julian);
char *RussianCurrentDate(void);
char *RussianCurrentTime(void);

/* SPANISH */
char *SpanishPrintDate(long julian);
char *SpanishPrintBriefDate(long julian);
char *SpanishCurrentDate(void);
char *SpanishCurrentTime(void);

/* THAI */
char *ThaiPrintDate(long julian);
char *ThaiPrintBriefDate(long julian);
char *ThaiCurrentDate(void);
char *ThaiCurrentTime(void);

long ReadDateString(char *s,int lang);

/* 2003.12.19.ET ADDENDUM */
LANGUAGE GetLanguageConventionFromLocale(void);

extern PLANGUAGEOBJECT Language[LANGUAGE_COUNT];

#define LANGCONVENTIONS_INCLUDED
