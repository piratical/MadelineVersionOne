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
/* "tools.h" is part of the Madeline software distribution.                          */
/* Written by Edward H. Trager.                                                      */
/*                                                                                   */
/* COPYRIGHT (C) 2000,2002 THE REGENTS OF THE UNIVERSITY OF MICHIGAN,                */
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
/* Last modified by Ed Trager, August, 2002.                                         */
/*                                                                                   */
/* ********************************************************************************* */

/*
   tools.h
   
   set of rather general functions that are useful in many different
   programs.  This version of "tools.h" has been modified for use
   in Madeline specifically.

   history
   -------
   
   970219: initiated

*/


#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>

#ifndef STRINGBUFFERSIZES_INCLUDED
#include "stringbuffersizes.h"
#endif

#ifndef VAR_AND_ARR_INCLUDED
#include "variablesandarrays.h"
#endif

/* Constants to be used as 3rd argument for "fseek" function
*/
#ifndef SEEK_CUR
#define SEEK_CUR    1
#endif
#ifndef SEEK_END
#define SEEK_END    2
#endif
#ifndef SEEK_SET
#define SEEK_SET    0
#endif

/*                                           */
/* Seed for RandomDeviate() function         */
/*                                           */
/* The seed chosen here affects the sequence */
/* of random colors which are generated      */
/* after Madeline "runs out" of the fixed    */
/* colors in the preset color table. Of      */
/* course, the seed may be used for other    */
/* applications that use the                 */
/* RandomDeviate() function too.             */
/*                                           */
//#define RANDOMDEVIATESEED 789402039
//#define RANDOMDEVIATESEED 125673023
#define RANDOMDEVIATESEED 441966268

void  FileCopy(FILE *in,FILE *out);
int   IsEmpty(char *s);
char *Find(char *t,char *s);
char *FindEnd(char *t,char *s);
char *OpenAndBufferFile(const char *filename);
long  FileSize(FILE *fp);
void  ToolsErrorReport(char *s,...);
int   File(const char *fn);

char *UnderscoredNumber(int v,int w);
char *ModifyFileName(const char *original,const char *extension);
char *CopyString(const char *s);
char *ToUpper(char *s);
char *GenerateRandomID(char s[]);
int   IsFusionID(char *s);
char * ValueType(unsigned short type);
char *Itoa(char *b,int v,int w);
void StoreString(PVARIABLE v,const char *s);
char *FormatNumber(double v,char *hext,int wide,int prec);
void SplitPath(const char *FileName,char **PointerToPath,char **PointerToName);
char *Properize(char *s);
void ShowBits(unsigned int u);
int PairCount(int IndividualCount);
FILE *ReassignFile(FILE *oldfp,char *old,char *newfile);
char *Concatenate(const char *delimiter,...);
int irand(int low,int high);
float RandomDeviate(long *idum);
double Round(double v);

