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
/* "associativearrays.c" is part of the Madeline software distribution.              */
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
/* associativearrays.c                                  */
/*                                                      */
/* history:                                             */
/*                                                      */
/* 2002.04.18.ET initiated                              */
/*                                                      */

/*                                                                   */
/*   Associative Arrays in Madeline                                  */
/*   ------------------------------                                  */
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



/*                                                      */
/* CompareVariableObjects(): Comparison function to use */
/* with tree.c BST routines:                            */
/*                                                      */

#include "associativearrays.h"
#include "logs.h"
#include "token.h"
#include "madeline.h"

#ifndef CONSOLE_INCLUDED
#include "console.h"
#endif

/*                        */
/* CompareKeyValuePairs() */
/*                        */
int CompareKeyValuePairs(const void *a,const void *b){
	
	PKEYVALUEPAIR aa;
	PKEYVALUEPAIR bb;
	
	/*                                 */
	/* Cast pointers to VARIABLE type: */
	/*                                 */
	aa=(PKEYVALUEPAIR)a;
	bb=(PKEYVALUEPAIR)b;
	
	/*                                                     */
	/* FIRST HANDLE THE CASE OF DIFFERENT KEY TYPES        */
	/* --------------------------------------------        */
	/*                                                     */
	/* The segregation of different types is arbitrary:    */
	/* As long as they sort separately, everything works.  */
	/* As it currently stands in "tokentypes.h", simple    */
	/* comparisons on the TYPE ITSELF will yield:          */
	/*                                                     */
	/* - NUMERICVALUES compare less than STRINGVALUES      */
	/* - STRINGVALUES  compare less than DATEVALUES        */
	/*                                                     */
	/* - This would only change if someone changed the     */
	/*   #defines in "tokentypes.h"                        */
	/*                                                     */
	if(aa->key.t!=bb->key.t){
		
		/* KEY TYPES DIFFERENT */
		
		return aa->key.t<bb->key.t?-1:1;
		
	}else{
		
		/* KEY TYPES THE SAME */
		
		switch(aa->key.t){
		case NUMERICVALUE:
		case DATEVALUE:
			/*                                                */
			/* julian day values and numeric values are both  */
			/* stored as a double in e.v:                     */
			/*                                                */
			if(aa->key.e.v==bb->key.e.v)       return  0;
			else if(aa->key.e.v < bb->key.e.v) return -1;
			else                               return  1;
			
		case STRINGVALUE:
			return strcmp(aa->key.e.s,bb->key.e.s);
			
		default:
			TreeErrorReport("CompareVariableObjects(): type %i not recognized for this function",aa->key.t);
			break;
		}
	}
	/*                      */
	/* please the compiler: */
	/*                      */
	return 0;
}


/*                                                         */
/* FreeKeyValuePair(): For use with tree.c BST routines: */
/*                                                         */
void FreeKeyValuePair(void *KeyValuePair){

	PKEYVALUEPAIR pkvp;

	pkvp=(PKEYVALUEPAIR)KeyValuePair;
	/*                                               */
	/* if the element type is a STRINGVALUE,         */
	/* then we must first free a copy of the string: */
	/*                                               */
	if(pkvp->key.t  ==STRINGVALUE) free(pkvp->key.e.s  );
	if(pkvp->value.t==STRINGVALUE) free(pkvp->value.e.s);
	/*                           */
	/* Now free the main object: */
	/*                           */
	free(pkvp);
}

/*                                                                     */
/* CopyKeyValuePair(): Returns a pointer to a KEYVALUEPAIR object      */
/* that contains a copy of the data in KeyValuePair passed to          */
/* the function.  The return pointer is cast to void for compatability */
/* with the generic routines in tree.c:                                */
/*                                                                     */
void *CopyKeyValuePair(const void *KeyValuePair){

	PKEYVALUEPAIR ov; /* original key-value pair    */
	PKEYVALUEPAIR nv; /* new copy of key-value pair */

	/*                                          */
	/* assign a pointer to the original object: */
	/*                                          */
	ov=(PKEYVALUEPAIR) KeyValuePair;
	
	nv=NewKeyValuePair(&ov->key,&ov->value);
	
	return (void *) nv;
	
}



/*                   */
/* NewKeyValuePair() */
/*                   */
PKEYVALUEPAIR NewKeyValuePair(PVARIABLE key,PVARIABLE value){

	PKEYVALUEPAIR nkvp;
	
	nkvp=(PKEYVALUEPAIR)malloc(sizeof(KEYVALUEPAIR));
	if(!nkvp) TreeErrorReport("NewKeyValuePair(): no memory for KEYVALUEPAIR object");
	
	/*                             */
	/* elementwise KEY assignment: */
	/*                             */
	nkvp->key.t=key->t;
	nkvp->key.i=key->i;
	if(nkvp->key.t==STRINGVALUE){
		/*                           */
		/* allocate and copy string: */
		/*                           */
		nkvp->key.e.s=(char *)malloc((unsigned)strlen(key->e.s)+1);
		if(!nkvp->key.e.s) TreeErrorReport("CopyKeyValuePair(): no memory for string copy");
		strcpy(nkvp->key.e.s,key->e.s);
	}else{
		/* assign value: could be julian date or numeric: */
		nkvp->key.e.v=key->e.v;
	}
	/*                               */
	/* elementwise VALUE assignment: */
	/*                               */
	nkvp->value.t=value->t;
	nkvp->value.i=value->i;
	if(nkvp->value.t==STRINGVALUE){
		/*                           */
		/* allocate and copy string: */
		/*                           */
		nkvp->value.e.s=(char *)malloc((unsigned)strlen(value->e.s)+1);
		if(!nkvp->value.e.s) TreeErrorReport("CopyKeyValuePair(): no memory for string copy");
		strcpy(nkvp->value.e.s,value->e.s);
	}else{
		/* assign value: could be julian date or numeric: */
		nkvp->value.e.v=value->e.v;
	}
	
	return nkvp;
	
}


/*                                                                                    */
/* NewAssociativeArray(): Create a new Associative Array with set key and value types */
/*                                                                                    */
PASSOCARRAY NewAssociativeArray(unsigned char keytype, unsigned char valuetype){

	PASSOCARRAY aa;
	
	/* check key and value type */
	if(!(keytype==NUMERICVALUE   || keytype==DATEVALUE   || keytype==STRINGVALUE  )) TreeErrorReport("NewAssociativeArray(): Unrecognized key type.");
	if(!(valuetype==NUMERICVALUE || valuetype==DATEVALUE || valuetype==STRINGVALUE)) TreeErrorReport("NewAssociativeArray(): Unrecognized value type.");
	
	aa=(PASSOCARRAY)malloc((unsigned)sizeof(ASSOCARRAY));
	if(!aa) TreeErrorReport("NewAssociativeArray(): Unable to allocate ASSOCARRAY.");
	
	/* assign elements */
	aa->kt=keytype;
	aa->vt=valuetype;
	aa->n=0;
	aa->h=NULL;
	
	return aa;
	
}

/*                                 */
/* AssociativeArrayAddKeyValuePair */
/*                                 */
void AssociativeArrayAddKeyValuePair(PASSOCARRAY aa,PVARIABLE key,PVARIABLE value){
	
	KEYVALUEPAIR kvp;
	PKEYVALUEPAIR pkvp;
	PNODE node;
	
	/*                                                               */
	/* Make sure the key and value types are correct for this array: */
	/*                                                               */
	
	/*                                                               */
	/* NOTE THAT aa->kt can be NULL, indicating a FREE array that    */
	/* will accept ALL key types.                                    */
	/*                                                               */
	if(aa->kt && key->t!=aa->kt) MadelineErrorReport("AssociativeArrayAddKeyValuePair(): The key type is incorrect for this array.");
	
	/*                                                               */
	/* NOTE THAT aa->vt can be NULL, indicating a FREE array that    */
	/* will accept ALL value types.                                  */
	/*                                                               */
	if(aa->vt && value->t!=aa->vt) MadelineErrorReport("AssociativeArrayAddKeyValuePair(): The value type is incorrect for this array.");
	
	/* Put the data into the KEYVALUEPAIR */
	/* Set up the KEY: */
	kvp.key.t=key->t;
	kvp.key.i=key->i;
	if(kvp.key.t==NUMERICVALUE || kvp.key.t==DATEVALUE) kvp.key.e.v=key->e.v;
	else                                                kvp.key.e.s=key->e.s;
	/* Set up the VALUE: */
	kvp.value.t=value->t;
	kvp.value.i=value->i;
	if(kvp.value.t==NUMERICVALUE || kvp.value.t==DATEVALUE) kvp.value.e.v=value->e.v;
	else                                                    kvp.value.e.s=value->e.s;
	
	/*                     */
	/* See if node exists: */
	/*                     */
	node=SeekNode(aa->h,(const void *)&kvp,CompareKeyValuePairs);
	if(node){
		/*                                             */
		/* Node exists, so replace value if different. */
		/* If data is the same, increment counter      */
		/*                                             */
		pkvp=(PKEYVALUEPAIR)node->o;
		switch(aa->vt){
		case NUMERICVALUE:
		case DATEVALUE:
			
			if(pkvp->value.e.v!=value->e.v){
				/* Reassign value */
				pkvp->value.e.v=value->e.v;
			}else{
				/* Increment node counter */
				node->n++;
			}
			break;
			
		case STRINGVALUE:
			
			if(strcmp(pkvp->value.e.s,value->e.s)){
				/* Reassign value by first freeing old string */
				free(pkvp->value.e.s);
				/* Allocate and copy new string: */
				pkvp->value.e.s=(char *)malloc((unsigned)strlen(value->e.s)+1);
				if(!pkvp->value.e.s) TreeErrorReport("AssociativeArrayAddKeyValuePair(): no memory for string copy");
				strcpy(pkvp->value.e.s,value->e.s);
			}else{
				/* Increment node counter */
				node->n++;
			}
			break;
			
		}
		
		
	}else{
		/*                         */
		/* Node doesn't exist yet, */
		/* so add to tree:         */
		if(!aa->h) aa->h=HeadNode((void *)&kvp,CopyKeyValuePair);
		else       aa->h=AddNode(aa->h,(void *)&kvp,CompareKeyValuePairs,CopyKeyValuePair,1);
		/* Increment the array element counter */
		aa->n++;
	}
}

/*                      */
/* LookupValueFromKey() */
/*                      */
PVARIABLE LookupValueFromKey(PASSOCARRAY paa,PVARIABLE key){
	
	PNODE node;
	KEYVALUEPAIR kvp;
	PKEYVALUEPAIR pkvp;
	PVARIABLE pv;
	
	kvp.key.t=key->t;
	if(kvp.key.t==NUMERICVALUE || kvp.key.t==DATEVALUE) kvp.key.e.v=key->e.v;
	else                                                kvp.key.e.s=key->e.s;
	
	node=SeekNode(paa->h,(const void *)&kvp,CompareKeyValuePairs);
	
	if(node){
		
		pkvp=(PKEYVALUEPAIR) node->o;
		pv=&pkvp->value;
		return pv;
		
	} else {
		
		return NULL;
		
	}
}

/*                                                */
/* ListKeyValuePair(): Prints out key-value pairs */
/*                                                */
void ListKeyValuePair(void *KeyValuePair){
	
	PKEYVALUEPAIR pkvp;
	
	extern PTOKENINFO pT;
	
	pkvp=(PKEYVALUEPAIR)KeyValuePair;
	
	ConsoleBold(stdout);
	lprintf("%s",pT->p);
	ConsoleNormal(stdout);
	lprintf("[%s]=",
		PrintVariableValue(&pkvp->key)
	);
	lprintf("%s\n",
		PrintVariableValue(&pkvp->value)
	);
	
}


/*                             */
/* AssignToAssociativeArray(): */
/*                             */
void AssignToAssociativeArray(short int AssignmentType,PASSOCARRAY aa,PVARIABLE key, PVARIABLE value){
	
	KEYVALUEPAIR kvp;
	PKEYVALUEPAIR pkvp;
	PNODE node;
	
	if(AssignmentType=='='){
		
		/*                                                            */
		/* Add the key-value pair to the associative array container: */
		/*                                                            */
		
		AssociativeArrayAddKeyValuePair(aa,key,value);
		
	}else if(AssignmentType==INCREMENT_ASSIGN || AssignmentType==DECREMENT_ASSIGN){
		
		/*                                                                           */
		/* We treate INCREMENT_ASSIGN and DECREMENT_ASSIGN separately because there  */
		/* is no target value whose type we can investigate.  So, it all depends on  */
		/* the type of the value already present in the node                         */
		/*                                                                           */
		
		kvp.key.t=key->t;
		if(kvp.key.t==NUMERICVALUE || kvp.key.t==DATEVALUE) kvp.key.e.v=key->e.v;
		else                                                kvp.key.e.s=key->e.s;
		
		node=SeekNode(aa->h,(const void *)&kvp,CompareKeyValuePairs);
		if(node){
			/*                                                      */
			/* Node exists, so increment the node value if possible */
			/*                                                      */
			pkvp=(PKEYVALUEPAIR)node->o;
			
			switch(pkvp->value.t){
			case NUMERICVALUE:
			case DATEVALUE:
				
				if(AssignmentType==INCREMENT_ASSIGN) pkvp->value.e.v+=1.0;
				else                                 pkvp->value.e.v-=1.0;
				break;
				
			case STRINGVALUE:
				
				MadelineErrorReport("AssignToAssociativeArray(): ++ and -- assignment operators \nare not supported for character string values.");
				break;
				
			}
			
		}else{
			
			MadelineErrorReport("AssignToAssociativeArray(): Node does not exist.\nThe key you specified is not present in the array.");
			
		}
		
	}else{
		
		if(value->t==STRINGVALUE){
			MadelineErrorReport("AssignToAssociativeArray(): a string value cannot be the object of the +=, -=, *= or /= assignment operators");
		}
		if(value->t==DATEVALUE){
			MadelineErrorReport("AssignToAssociativeArray(): a date value cannot be the object of the +=, -=, *= or /= assignment operators.");
		}
		
		/*                                                     */
		/* Get here if type of specified value is NUMERICVALUE */
		/*                                                     */
		kvp.key.t=key->t;
		if(kvp.key.t==NUMERICVALUE || kvp.key.t==DATEVALUE) kvp.key.e.v=key->e.v;
		else                                                kvp.key.e.s=key->e.s;
		
		node=SeekNode(aa->h,(const void *)&kvp,CompareKeyValuePairs);
		
		if(node){
			
			pkvp=(PKEYVALUEPAIR)node->o;
			/* Verify that the current node value is either NUMERICVALUE or DATEVALUE, not STRINGVALUE */
			if(pkvp->value.t==STRINGVALUE){
				MadelineErrorReport("AssignToAssociativeArray(): a string value cannot be the subject of the +=, -=, *= or /= assignment operators.");
			}
			
			/*                                              */
			/* Get here if it's a NUMERICVALUE or DATEVALUE */
			/*                                              */
			
			switch(AssignmentType){
				
			case PLUS_ASSIGN:
				
				pkvp->value.e.v += value->e.v;
				break;
				
			case MINUS_ASSIGN:
				
				pkvp->value.e.v -= value->e.v;
				break;
				
			case MULTIPLY_ASSIGN:
				
				pkvp->value.e.v *= value->e.v;
				break;
				
			case DIVIDE_ASSIGN:
				
				if(value->e.v==0) MadelineErrorReport("AssignToAssociativeArray(): Division by zero is not permitted.");
				pkvp->value.e.v /= value->e.v;
				break;
			}
			
		}else{
			
			MadelineErrorReport("AssignToAssociativeArray(): Node does not exist.\nThe key you specified is not present in the array.");
			
		}
	}
}

/*                         */
/* NewPointerToArrayVector */
/*                         */
PPASSOCARRAY NewPointerToAssociativeArrayVector(int n){

	PPASSOCARRAY pa;
	int i;
	
	pa=(PPASSOCARRAY)malloc((unsigned)n*sizeof(PASSOCARRAY));
	if(!pa) MadelineErrorReport("NewPointerToAssociativeArrayVector(): unable to allocate memory for PPASSOCARRAY.");
	
	for(i=0;i<n;i++) pa[i]=NULL;
	return pa;
}

//
// FreeAssociativeArray
//
void FreeAssociativeArray(PASSOCARRAY paa){
	
	//
	// Free the tree of key-value pairs:
	// 
	FreeTree(paa->h,FreeKeyValuePair);
	
}

//
// FreeUserAssociativeArray
//
void FreeUserAssociativeArray(PASSOCARRAY paa){
	
	//
	// Free the tree of key-value pairs:
	// 
	FreeAssociativeArray(paa);
	
	// Now free the memory for the associative
	// array itself:
	free(paa);
	
}


/* end of associativearrays.c */

