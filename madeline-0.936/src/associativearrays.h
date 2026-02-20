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
/* "associativearrays.h" is part of the Madeline software distribution.              */
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
/* Last modified by Ed Trager, April, 2002.                                          */
/*                                                                                   */
/* ********************************************************************************* */

/*                                                      */
/*                                                      */
/* associativearrays.h                                  */
/*                                                      */
/* history:                                             */
/*                                                      */
/* 2002.04.18.ET initiated                              */
/*                                                      */

/*                */
/* ANSI INCLUDES: */
/*                */
#include <string.h>

/*                   */
/* MADELINE INCLUDES */
/*                   */
#ifndef TREE_INCLUDED
#include "tree.h"   /* provides the core red-black BST implementation and NODE definition */
#endif

#ifndef VAR_AND_ARR_INCLUDED
#include "variablesandarrays.h" /* provides definition of a VARIABLE */
#endif

#ifndef TOKENTYPES_INCLUDED
#include "tokentypes.h"         /* definitions of NUMERICVALUE, STRINGVALUE, and DATEVALUE */
#endif

/*                       */
/* STRUCTURE DEFINITIONS */
/*                       */
typedef struct sKEYVALUEPAIR{
	
	VARIABLE key;
	VARIABLE value;
	
}KEYVALUEPAIR,*PKEYVALUEPAIR,**PPKEYVALUEPAIR;

/*                                                                   */
/* ASSOCIATIVEARRAY structure                                        */
/*                                                                   */
/* - If the key type "kt" is set to 0 (NULL), then the program       */
/*   will permit keys of ANY ALLOWED type (NUMERICVALUE, DATEVALUE,  */
/*   or STRINGVALUE); otherwise, the key type is limited  to a       */
/*   SINGLE type.                                                    */
/*                                                                   */
/* - The same applies to the value type: if NULL, any allowed type   */
/*   of value will be accepted; otherwise only the specified value   */
/*   type will be accepted.                                          */
/*                                                                   */
/* - Normally, the value type will be fixed, but the key type may    */
/*   not be, thus permitting the use of a single associative array   */
/*   for mapping keys of different types.  For example, the          */
/*   AffectionStatusField can be either CHARACTER or NUMERIC, so we  */
/*   can now have a single association map that handles the field    */
/*   regardless of the field type.                                   */
/*                                                                   */
/* - The Comparison function has been written so that the different  */
/*   types are segregated (i.e., sorted separately) when the tree is */
/*   constructed.                                                    */
/*                                                                   */
typedef struct sASSOCARRAY{

	unsigned char kt; /* type of the keys (only one type allowed)  */
	unsigned char vt; /* type of the values (only one type allowed */
	int           n; /* number of elements currently in the array  */
	PNODE         h; /* pointer to the head node                   */
	
}ASSOCARRAY,*PASSOCARRAY,**PPASSOCARRAY;


/*                      */
/* FUNCTION PROTOTYPES: */
/*                      */

/*                                                           */
/* These take and return void pointers in accordance with    */
/* the requirements of the BST tree implementation in tree.c */
/*                                                           */
int CompareKeyValuePairs(const void *a,const void *b);
void FreeKeyValuePair(void *KeyValuePair);
void *CopyKeyValuePair(const void *KeyValuePair);

PKEYVALUEPAIR NewKeyValuePair(PVARIABLE key,PVARIABLE value);
PASSOCARRAY NewAssociativeArray(unsigned char keytype, unsigned char valuetype);
void AssociativeArrayAddKeyValuePair(PASSOCARRAY parray,PVARIABLE key,PVARIABLE value);
PVARIABLE LookupValueFromKey(PASSOCARRAY paa,PVARIABLE key);
void ListKeyValuePair(void *KeyValuePair);
void AssignToAssociativeArray(short int AssignmentType,PASSOCARRAY aa,PVARIABLE key, PVARIABLE value);
PPASSOCARRAY NewPointerToAssociativeArrayVector(int n);
void FreeAssociativeArray(PASSOCARRAY paa);
void FreeUserAssociativeArray(PASSOCARRAY paa);

#define ASSOCIATIVEARRAY_INCLUDED

