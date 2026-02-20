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
/* "date.h" is part of the Madeline software distribution.                           */
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
/*   date.h                                            */
/*                                                     */
/*                                                     */
/*   Routines to support date manipulation in Madeline */
/*                                                     */
/*   Initiated: 980609 ET                              */
/*                                                     */

/*//////////////////////// */
/*                         */
/* ANSI C INCLUDES:        */
/*                         */
/*//////////////////////// */
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

/*//////////////////////// */
/*                         */
/* MADELINE INCLUDES:      */
/*                         */
/*//////////////////////// */

#define IGREG (15+31L*(10+12L*1582))  /* Gregorian calendar adopted Oct. 15, 1582 */
#define DATESTRINGLENGTH 100

/*                      */
/*                      */
/* FUNCTION PROTOTYPES: */
/*                      */
/*                      */
void DateErrorReport(char *s,...);
long JulianDay(int mm, int dd, int yyyy);
void CalendarDate(long julian,int *mm,int *dd,int *yyyy);
int DayOfWeek(long julian);
long CurrentJulianDay(void);
void CurrentTime(int *hour,int *min);
int IsDateDelimiter(char c);
long Read_YYYYMMDD_DateString(const char *s);
long  JulianToday(void);
char *GenericDateString(long julian);
char *DelimitedGenericDateString(long julian, char dlm);
// The following is just a wrapper for printing out the Julian Day number:
double DateToJulian( double jdn );
// The following is also just a wrapper for printing out the date from a number:
double JulianToDate( double jdn );
// The following is also just a wrapper for printing out today's date:
double Today( double jdn );
#define DATE_INCLUDED


