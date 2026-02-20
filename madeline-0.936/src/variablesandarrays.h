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
/* "variablesandarrays.h" is part of the Madeline software distribution.             */
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
/* Last modified by Ed Trager, April 2002.                                           */
/*                                                                                   */
/* ********************************************************************************* */

/*                              */
/* variablesandarrays.h         */
/*                              */
/* This file was originally     */
/* called "shared.h".  The new  */
/* name is meant to clarify the */
/* purpose of the file.         */
/*                              */
/* history:                     */
/* initiated: 980610 ET         */
/* 2002.04.18.ET: renamed and   */
/* unrelated stuff relegated    */
/* into separate include files. */
/*                              */
/* Contains some basic things   */
/* that Madeline uses all over  */
/* such as ELEMENT, VARIABLE,   */
/* and ARRAY structures         */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h> /* for INT_MIN etc. definitions */
#include <float.h>  /* for DBL_MIN definition       */


/*//////////////////////                                                       */
/*                                                                             */
/* DEFINES                                                                     */
/*                                                                             */
/*//////////////////////                                                       */
/*                                                                             */
/* Missing values of the various numeric types are just the smallest negative  */
/* number representable by the type:                                           */
/*                                                                             */
#define INT8MISSINGVALUE    SCHAR_MIN
#define INT16MISSINGVALUE   SHRT_MIN
#define INT32MISSINGVALUE   INT_MIN
#define UINT32MISSINGVALUE  INT_MAX
#define FLOATMISSINGVALUE   FLT_MIN
#define DOUBLEMISSINGVALUE  DBL_MIN
#define CHARMISSINGVALUE    '.'
#define CHARMISSINGSTRING   ""       /* empty string */
#define DATEMISSINGSTRING   "."
#define MISSINGDATE         INT32MISSINGVALUE

#define DOUBLE_MINIMUM      DBL_MIN
#define DOUBLE_MAXIMUM      DBL_MAX

#ifndef MISSING
#define MISSING -9
#endif

/*                                                               */
/* Array allocation unit:                                        */
/*                                                               */
#define ARRAYALLOCATIONUNIT       10 /* default size of an array */

/*////////////////////// */
/*                       */
/* STRUCTURE DEFINITIONS */
/*                       */
/*////////////////////// */

/*                */
/* ELEMENT union: */
/*                */
typedef union sELEMENT{
	double         v; /* numeric value */
	char          *s; /* string        */

}ELEMENT,*PELEMENT;

/*                                                            */
/* VARIABLE structure: used to hold internal variables        */
/* that are not arrays (see ARRAY below for internal arrays): */
/*                                                            */
typedef struct sVARIABLE{

	/*char        *nm;  // external name of the variable (OPTIONAL: make NULL if not used) */
	unsigned char t;  /* type of variable: numeric, date, or character string              */
	ELEMENT       e;  /* note that e is not a pointer here, but IS in ARRAY                */
	int           i;  /* integer used to store index number of a recognized database field */
	
}VARIABLE,*PVARIABLE,**PPVARIABLE;

/*                 */
/* ARRAY structure */
/*                 */
typedef struct sARRAY{

	unsigned char t; /* type of the elements of the array (only one type of element allowed) */
	int           n; /* number of elements currently in the array                            */
	int           m; /* maximum number of elements (current allocation size)                 */
	PELEMENT      e; /* pointer to the elements                                              */
}ARRAY,*PARRAY,**PPARRAY;


/*/////////////////////                             */
/*                                                  */
/* EXTERN GLOBALS:                                  */
/*                                                  */
/*/////////////////////                             */
extern ARRAY   Cmv; /* character missing value list */
extern ARRAY   Nmv; /* numeric missing value list   */
extern PARRAY pCmv;
extern PARRAY pNmv;


/*//////////////////////// */
/*                         */
/* FUNCTION DECLARATIONS:  */
/*                         */
/*//////////////////////// */
int IsCMV(char *s);
int IsNMV(double v);
PARRAY NewArray(unsigned char t);
PARRAY IncreaseArray(PARRAY a);
void FreeArray(PARRAY a);     /* for INTERNALARRAYs                             */
void FreeUserArray(PARRAY a); /* for USERARRAYs which are allocated from memory */
void FreeVariable(PVARIABLE v);
void FreeUserVariable(PVARIABLE v);
PPARRAY NewPointerToArrayVector(int n);
char *PrintVariableValue(PVARIABLE v);

#define VAR_AND_ARR_INCLUDED


