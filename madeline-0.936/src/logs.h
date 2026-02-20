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
/* "logs.h" is part of the Madeline software distribution.                           */
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

/*                   */
/* logs.h            */
/*                   */
/*                   */

/*//////////////// */
/*                 */
/* ANSI INCLUDES:  */
/*                 */
/*//////////////// */
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <stdarg.h>

/*/////////////////// */
/*                    */
/* MADELINE INCLUDES: */
/*                    */
/*/////////////////// */
#ifndef CONTROL_INCLUDED
#include "control.h"
#endif

#ifndef PORTABLE_INCLUDED
#include "portable.h"
#endif

/*///////// */
/*          */
/* DEFINES: */
/*          */
/*///////// */
#define WARNINGFLAG       0
#define SEVEREWARNINGFLAG 1
#define INHERITANCEFLAG   2
#define ERRORFLAG         3
#define FATALFLAG         4 /* this one terminates the application */

/*/////////////////// */
/*                    */
/* EXTERNS:           */
/*                    */
/*/////////////////// */
extern char *e;
extern char *estt;
extern jmp_buf BunnyHop;
extern VARIABLE gDatabaseFile;


void MadelineErrorReport(const char *s,...);
void dprintf(const char *s,...);
void lprintf(const char *s,...);
void eprintf(int flag,const char *s,...);
void PrintErrorLogHeader(PCONTROL c);
void LinePrint(const char *s,void PrintFunction(const char *s,...));

#define LOGS_INCLUDED


