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
/* "tokentree.c" is part of the Madeline software distribution.                      */
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
/* tokntree.c            */
/*                       */
/* 19990413 ET initiated */
/*                       */
/*                       */
/*                       */

/*                                                                      */
/* These are included here to avoid problems with multiple declarations */
/* which arise if the next several lines are in the tokntree.h file:    */
/*                                                                      */

#include "tokentree.h"
#include "logs.h"
#include "console.h"

/*                                                             */
/* CompareRecognizedTokenObjects(): Comparison function to use */
/* with tree.c BST routines:                                   */
/*                                                             */
int CompareRecognizedTokenObjects(const void *a,const void *b){
	
	PRECOGNIZEDTOKEN aa;
	PRECOGNIZEDTOKEN bb;
	
	/*                                        */
	/* Cast pointers to RECOGNIZEDTOKEN type: */
	/*	                                       */
	aa=(PRECOGNIZEDTOKEN)a;
	bb=(PRECOGNIZEDTOKEN)b;
	/*                                                            */
	/* compare the external string representations of the tokens: */
	/*                                                            */
	
	return strcmp(aa->e,bb->e);
}


/*                                                                */
/* FreeRecognizedTokenObject(): For use with tree.c BST routines: */
/*                                                                */
void FreeRecognizedTokenObject(void *RecognizedTokenObject){

	PRECOGNIZEDTOKEN pt;

	pt=(PRECOGNIZEDTOKEN)RecognizedTokenObject;
	/*                                                       */
	/* Free the external string representation of the token: */
	/*                                                       */
	if(pt->e) free(pt->e);
	if(pt->p) free(pt->p);
	/*                                                           */
	/* (The pointer to help text does not need to be free here   */
	/*  because the complete help text is read into a buffer and */
	/*  managed elsewhere)                                       */
	/*                                                           */
	
	/*                                                            */
	/* If the RECOGNIZEDTOKEN is a USERVARIABLE or USERARRAY,     */
	/* this was allocated from memory and needs to be deallocated */
	/* accordingly:	                                              */
	/*                                                            */
	if(pt->t==USERARRAY)    FreeUserArray(pt->u.a);
	if(pt->t==USERVARIABLE) FreeUserVariable(pt->u.v);
	if(pt->t==ASSOCARRAY)   FreeAssociativeArray(pt->u.aa);
	
	/*                           */
	/* Now free the main object: */
	/*                           */
	free(pt);
}

/*                                                                            */
/* CopyRecognizedTokenObject(): Returns a pointer to a RECOGNIZEDTOKEN object */
/* that contains a copy of the data in RecognizedTokenObject passed to        */
/* the function.  The return pointer is cast to void for compatability        */
/* with the generic routines in tree.c:                                       */
/*                                                                            */
void *CopyRecognizedTokenObject(const void *RecognizedTokenObject){

	PRECOGNIZEDTOKEN ot; /* original token    */
	PRECOGNIZEDTOKEN nt; /* new copy of token */

	/*                                          */
	/* assign a pointer to the original object: */
	/*                                          */
	ot=(PRECOGNIZEDTOKEN) RecognizedTokenObject;
	
	/*                                      */
	/* allocate for the copy of the object: */
	/*	                                     */
	nt=(PRECOGNIZEDTOKEN)malloc(sizeof(RECOGNIZEDTOKEN));
	if(!nt) TreeErrorReport("CopyRecognizedTokenObject(): no memory for RECOGNIZEDTOKEN object");
		
	/*                                                     */
	/* allocate and copy string (external representation): */
	/*                                                     */
	nt->e=(char *)malloc((unsigned)strlen(ot->e)+1);
	if(!nt->e) TreeErrorReport("CopyRecognizedTokenObject(): no memory for string copy");
	strcpy(nt->e,ot->e);

	/*                                                       */
	/* allocate and copy string (properized representation): */
	/*                                                       */
	nt->p=(char *)malloc((unsigned)strlen(ot->p)+1);
	if(!nt->p) TreeErrorReport("CopyRecognizedTokenObject(): no memory for string copy");
	strcpy(nt->p,ot->p);


	/*                                                                 */
	/* elementwise assignment of remaining structure elements:         */
	/*                                                                 */
	nt->i=ot->i; /* internal representation                            */
	nt->t=ot->t; /* type of token                                      */
	nt->v=ot->v; /* value of a constant (like PI)                      */
	nt->u=ot->u; /* union of pointer to VARIABLE, ARRAY, or a function */
	
	/*                                   */
	/* Return pointer to new token copy: */
	/*                                   */
	return nt;
}

/*                                                                      */
/* InstallKnownToken(): Installs a token into the RECOGNIZEDTOKEN tree. */
/* If a help file is available, help text is attached too:              */
/*                                                                      */
/* e: external token representation                                     */
/* i: internal token representation                                     */
/* t: token type                                                        */
/* v: value (of a constant like PI)                                     */
/* p: void * to function or ARRAY or VARIABLE or NULL if not used       */
/*                                                                      */
/* h: pointer to help text buffer                                       */
/*                                                                      */
PNODE InstallKnownToken(PNODE tree,char *e,char *pe,unsigned short i,unsigned char t,double v,void *p){

	RECOGNIZEDTOKEN tt,*ptt=&tt;
	//char h[MINIMUMBUFFERSIZE]="~";
	 
	
	/*                                                                     */
	/* Elementwise Assignment:                                             */
	/*                                                                     */
	ptt->e=e;   /* external representation of token                        */
	ptt->p=pe;  /* Properized representation of token                      */
	ptt->i=i;   /* internal representation of token                        */
	ptt->t=t;   /* token type                                              */
	ptt->v=v;   /* value if token represents a constant like PI            */
	ptt->u.p=p; /* void * used for assignment only: access depends on type */
	
	/*                 */
	/* Build the tree: */
	/*                 */
	if(!tree) tree=HeadNode((void *)ptt,CopyRecognizedTokenObject);
	else      tree=AddNode(tree,(void *)ptt,CompareRecognizedTokenObjects,CopyRecognizedTokenObject,1);
	/*                              */
	/* return pointer to head node: */
	/*                              */
	return tree;
}


/*                                                              */
/* LookupKnownToken(): If a RECOGNIZEDTOKEN is found, it's data */
/* is transferred into the passed TOKENINFO pointer.            */
/* Return value is token type:                                  */
/*                                                              */
unsigned char LookupKnownToken(PTOKENINFO pti,PNODE tree){


	PNODE found;
	RECOGNIZEDTOKEN seek;
	PRECOGNIZEDTOKEN data;
	
	/*                               */
	/* Only really need to assign e: */
	/*                               */
	seek.e=pti->e;
	/*                          */
	/* then look it up in tree: */
	/*                          */
	found=SeekNode(tree,(void *)&seek,CompareRecognizedTokenObjects);
	
	if(found){
		/*                                 */
		/* set pointer to RECOGNIZEDTOKEN: */
		/*                                 */
		data=(PRECOGNIZEDTOKEN)found->o;		
		/*                     */
		/* fill info into pti: */
		/*                     */
		pti->p  =data->p;
		pti->i  =data->i;
		pti->t  =data->t;
		pti->v  =data->v;
		pti->f  =MISSING;
		pti->u.p=data->u.p;
		/*              */
		/* return type: */
		/*              */
		return pti->t;
	}else{
		/*            */
		/* not found: */
		/*            */
		return (unsigned char) 0;
	}
}


/*           */
/* PrintHelp */
/*           */
void PrintKnownTokenHelp(PNODE node,char *SeekKey,int SeekLength){


	PRECOGNIZEDTOKEN pt;
	
	/*                                                           */
	/* These have to match with the #defines in tokentypes.h:    */
	/* and actually probably should be in there instead of here: */
	static char *desc[TOKENTYPES]={
		"not a defined token type (0)",
		"a numeric constant",
		"a numeric variable",
		"an operator",
		"a string value",
		"a string variable",
		"an internal variable",
		"a field name value",
		"a command",
		"a function taking and returning a number",
		"a function taking and returning a string",
		"a function taking a string and returning a number",
		"an aggregate function",
		"a function taking two string arguments and returning a boolean result",
		"an internal array",
		"a function taking two string and two numeric arguments and returning a string",
		"a keyword",
		"a numeric property of an individual in a pedigree",
		"an array property of an individual in a pedigree",
		"a string property of an individual in a pedigree",
		"a pointer belonging to an individual that points to another individual",
		"a date value",
		"a date variable",
		"a user variable",
		"a user array",
		"an associative array",
		"a program control operator",
		"a function taking a date and returning a number",
		"a function taking a number and returning a date",
		"a function taking no parameters and returning a date"
	};

	if(!node) return;
	
	/*                    */
	/* Process left node: */
	/*                    */
	if(node->l) PrintKnownTokenHelp(node->l,SeekKey,SeekLength);
	
	
	/*//////////////////// */
	/*                     */
	/* Process this node:  */
	/*                     */
	/*//////////////////// */

	pt=(PRECOGNIZEDTOKEN) node->o;
	
	/*                               */
	/* Print information if matches: */
	/*                               */
	if(strncmp(pt->e,SeekKey,SeekLength)==0){
		
		ConsoleBold(stdout);
		lprintf("%s",pt->p);
		ConsoleNormal(stdout);
		lprintf(" is %s",desc[pt->t]);
		
		/*                                           */
		/* Show the value of internal variables:     */
		/* We could show the values of other items   */
		/* too, but showing internal variable values */
		/* is probably the most important & useful:  */
		/*                                           */
		switch(pt->t){
		case NUMERICVALUE:
			lprintf(". It's value is %g",pt->v);
			break;
		case INTERNALVARIABLE:
			switch(pt->u.v->t){
			case NUMERICVALUE:
				if(pt->u.v->e.v==DOUBLEMISSINGVALUE){
					lprintf(". Its current value is #MISSING");
				}else{
					lprintf(". Its current value is %.3f",pt->u.v->e.v);
				}
				break;
			case STRINGVALUE:
			case FIELDNAMEVALUE:
				lprintf(". Its current value is \"%s\"",pt->u.v->e.s);
				break;
			default:
				lprintf(". Unrecognized value type: %x",pt->u.v->t);
			}
			break;
		case INTERNALARRAY:
		case USERARRAY:
			lprintf(". It stores %s values",ValueType(pt->u.a->t)); 
			break;
		case ASSOCIATIVEARRAY:
			lprintf(". It accepts %s keys and maps them to %s values",
				ValueType(pt->u.aa->kt),
				ValueType(pt->u.aa->vt)
			);
			break;
		}
		
		lprintf(".\n");
		
	}
	
	/*                     */
	/* Process right node: */
	/*                     */
	if(node->r) PrintKnownTokenHelp(node->r,SeekKey,SeekLength);
}


