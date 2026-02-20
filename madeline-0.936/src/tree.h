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
/* "tree.h" is part of the Madeline software distribution.                          */
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


/*                                                                                   */
/* tree.h -implements a generic red-black BST in ANSI C                              */
/*                                                                                   */
/* 19990202 ET initiated                                                             */
/*                                                                                   */
/*                                                                                   */
/* Since this is a generic implementation, you need to supply:                       */
/*                                                                                   */
/*   1. a Compare(void *,void *) function to compare two objects                     */
/*   2. a FreeObject(void *) function to free a dynamically created object           */
/*   3. one or more void *NewObject(...) functions for instantiating objects         */
/*   4. one or more void ActOnObject(void *object) functions for use when traversing */
/*   5. a void *CopyObject(void *) to make a copy of the object                      */
/*                                                                                   */

/*           */
/* INCLUDES: */
/*           */
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

/*          */
/* DEFINES: */
/*          */
#define KEYSEPARATOR '~'
#define KEYSEPARATORSTRING "~"


/*                        */
/* STRUCTURE DEFINITIONS: */
/*                        */

/*       */
/* NODE: */
/*       */
typedef struct sNODE{
	
	void *o;         /* pointer to data                                        */
	int   n;         /* count of objects                                       */
	int rank;        /* node rank from 1 to n (filled after tree is completed) */
	char red;        /* red/black sentinal                                     */
	struct sNODE *l; /* left node                                              */
	struct sNODE *r; /* right node                                             */
	
}NODE,*PNODE,**PPNODE;

PNODE NewNode(const void *object,void *(*CopyObject)(const void *));
PNODE AddNode(PNODE pn,const void *object,int (*Compare)(const void *,const void *),void *(*CopyObject)(const void *),int sw);
PNODE NodeRightRotation(PNODE h);
PNODE NodeLeftRotation(PNODE h);
void  FreeTree(PNODE pn,void (*FreeObject)(void *));
void  TraverseTree(PNODE pn,void (*ActOnObject)(void *object));

PNODE SeekNode(PNODE pn,const void *object,int (*Compare)(const void *,const void *));
void  TreeErrorReport(char *s,...);
PNODE HeadNode(const void *object,void *(*CopyObject)(const void *));
int   SetNodeRanks(PNODE pn,int rank);

#define TREE_INCLUDED


