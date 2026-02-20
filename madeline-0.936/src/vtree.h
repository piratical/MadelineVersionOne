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
/* "vtree.h" is part of the Madeline software distribution.                          */
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

/*                       */
/* vtree.h               */
/*                       */
/* 19990325 ET initiated */
/*                       */
/* vtree.h defines the   */
/* functions needed for  */
/* building BSTs for     */
/* Madeline VARIABLEs    */
/* using tree.c to       */
/* implement the actual  */
/* trees and methods     */
/*                       */

/*                */
/* ANSI INCLUDES: */
/*                */
#include <string.h>

#ifndef TREE_INCLUDED
#include "tree.h"   /* provides the core red-black BST implementation   */
#endif

#ifndef VAR_AND_ARR_INCLUDED
#include "variablesandarrays.h" /* provides definition of a VARIABLE    */
#endif

#ifndef TOKENTYPES_INCLUDED
#include "tokentypes.h"
#endif


/*                      */
/* FUNCTION PROTOTYPES: */
/*                      */
int CompareVariableObjects(const void *a,const void *b);
void FreeVariableObject(void *VariableObject);
void *CopyVariableObject(const void *VariableObject);


#define VTREE_INCLUDED

