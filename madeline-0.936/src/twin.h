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
/* "twin.h" is part of the Madeline software distribution.                           */
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

/*                                                 */
/* twin.h:                                         */
/*                                                 */
/* 19980318 Ed Trager                              */
/*                                                 */
/* -Provides a BST and support functions           */
/*  for keeping track of twin groups in a pedigree */
/*                                                 */

/*           */
/* INCLUDES: */
/*           */
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#ifndef PEDIGREE_INCLUDED
#include "pedigree.h"
#endif

/*                       */
/* STRUCTURE DEFINITIONS */
/*                       */
#define MAXTWINS 6

/*                     */
/* TWINGROUP structure */
/*                     */
typedef struct sTWINGROUP{

	char key;                /* twin group key               */
	int  n;                  /* number in group              */
	PINDIVIDUAL t[MAXTWINS]; /* vector of pointers to twins; */
	struct sTWINGROUP *l;    /* left pointer                 */
	struct sTWINGROUP *r;    /* right pointer                */
	
}TWINGROUP,*PTWINGROUP;

/*                      */
/* FUNCTION PROTOTYPES: */
/*                      */
PTWINGROUP NewTwinGroup(void);
PTWINGROUP InsertTwinGroup(PTWINGROUP h,char key,PINDIVIDUAL pindv);
void       TraverseTwinGroup(PTWINGROUP p,void (*visit)(PTWINGROUP));
PTWINGROUP LookupTwinGroup(PTWINGROUP h, char key);
void       FreeTwinGroup(PTWINGROUP p);
void       TwinErrorReport(char *s,...);

#define TWIN_INCLUDED


