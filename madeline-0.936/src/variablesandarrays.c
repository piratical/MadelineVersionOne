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
/* "variablesandarrays.c" is part of the Madeline software distribution.             */
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
/* Last modified by Ed Trager, April, 2002.                                          */
/*                                                                                   */
/* ********************************************************************************* */

/*                              */
/* variablesandarrays.c         */
/*                              */
/* Originally called "shared.c" */
/* which name was completely    */
/* undescriptive of the purpose */
/* of this file                 */
/*                              */
/* History:                     */
/*                              */
/* initiated: 980610 ET         */
/* 2002.04.18.ET: renamed       */
/*                              */
/* Contains some basic things   */
/* that Madeline uses all over  */
/* such as functions to operate */
/* on ELEMENT, VARIABLE, and    */
/* ARRAY structures.            */
/*                              */

#include "variablesandarrays.h"
#include "tools.h"
#include "tokentypes.h"
#include "date.h"
#include "madeline.h"

/*                                                                   */
/* character and numeric missing value arrays:                       */
/*                                                                   */
ARRAY   Cmv={STRINGVALUE ,0,0,NULL}; /* character missing value list */
ARRAY   Nmv={NUMERICVALUE,0,0,NULL}; /* numeric missing value list   */
PARRAY pCmv=&Cmv;
PARRAY pNmv=&Nmv;

/*////////////////////////////////////////////////////////////////////// */
/*                                                                       */
/* IsCMV(): returns 1 (true) if string matches a character missing value */
/* indicator already stored in pCmv:                                     */
/*                                                                       */
/*////////////////////////////////////////////////////////////////////// */
int IsCMV(char *s){

	int i;
	
	extern ARRAY Cmv;
	
	for(i=0;i<Cmv.n;i++)
		if(!strcmp(s,Cmv.e[i].s)) return 1;
	return 0;
}

/*//////////////////////////////////////////////////////////////////////// */
/*                                                                         */
/* IsNMV(): returns 1 (true) if value is a numeric missing value indicator */
/* already stored in pNmv, or if v=DOUBLEMISSINGVALUE                      */
/*                                                                         */
/*//////////////////////////////////////////////////////////////////////// */
int IsNMV(double v){

	int i;

	extern ARRAY Nmv;
	
	if(v==DOUBLEMISSINGVALUE) return 1;
		
	for(i=0;i<Nmv.n;i++)
		if(v==Nmv.e[i].v) return 1;   

	return 0;
}



/*          */
/* NewArray */
/*          */
PARRAY NewArray(unsigned char t){

	PARRAY a;
	
	a=(PARRAY)malloc((unsigned)sizeof(ARRAY));
	if(!a) ToolsErrorReport("NewArray(): unable to allocate memory for PARRAY");
	
	a->t=t;
	a->n=a->m=0;
	a->e=NULL;
	IncreaseArray(a);

	return a;

}


/*                         */
/* NewPointerToArrayVector */
/*                         */
PPARRAY NewPointerToArrayVector(int n){

	PPARRAY pa;
	int i;
	
	pa=(PPARRAY)malloc((unsigned)n*sizeof(PARRAY));
	if(!pa) ToolsErrorReport("NewArray(): unable to allocate memory for PARRAY");
	
	for(i=0;i<n;i++) pa[i]=NULL;
	return pa;
}


/*                                               */
/* IncreaseArray: increases the size of an array */
/*                                               */
PARRAY IncreaseArray(PARRAY a){


	int oldsize,i;

	if(!a) ToolsErrorReport("IncreaseArray(): array is NULL");

	oldsize=a->m;
	a->m+=ARRAYALLOCATIONUNIT;
	if(!a->e) a->e=(PELEMENT) malloc((unsigned)a->m*sizeof(ELEMENT));
	else      a->e=(PELEMENT) realloc(a->e,a->m*sizeof(ELEMENT));     
	if(!a->e) ToolsErrorReport("IncreaseArray(): out of memory");
	for(i=oldsize;i<a->m;i++){
		a->e[i].s=NULL;
	};
	return a;
}


/*                                                                            */
/* FreeArray: Frees the elements and strings, if present, in an INTERNALARRAY */
/* or USERARRAY (but does not free a USERARRAY itself):                       */
/*                                                                            */
void FreeArray(PARRAY a){


	int i;
	/*                               */
	/* free any strings, if present: */
	/*                               */
	if(a->t==STRINGVALUE){
		for(i=0;i<a->m;i++){
			if(a->e[i].s){
				free(a->e[i].s);
				a->e[i].s=NULL;
			}
		}
	}
	/*                            */
	/* free ELEMENTS, if present: */
	/*                            */
	if(a->e){
		free(a->e);
		a->e=NULL;
		a->n=a->m=0;
	}
}

/*                                    */
/* FreeUserArray(): Frees a USERARRAY */
/*                                    */
void FreeUserArray(PARRAY a){

	/*                                                */
	/* FreeArray clears the allocated strings and     */
	/* the element vector, but does not deallocate    */
	/* a itself because a could be an internal array: */
	/*                                                */
	FreeArray(a);
	/*                              */
	/* here we deallocate a itself: */
	/*                              */
	free(a);
}


/*                                                                           */
/* FreeVariable(): Frees the string, if present, of an INTERNALVARIABLE      */
/* or USERVARIABLE (but does not free the memory for a USERVARIABLE itself): */
/*                                                                           */
void FreeVariable(PVARIABLE v){

	/*                  */
	/* Free the string: */
	/*                  */
	if(v->t==STRINGVALUE || v->t==FIELDNAMEVALUE) free(v->e.s);
	v->e.s=NULL;
}



/*                                           */
/* FreeUserVariable(): Frees a USERVARIABLE: */
/*                                           */
void FreeUserVariable(PVARIABLE v){

	FreeVariable(v);
	free(v);
}


char *PrintVariableValue(PVARIABLE v){
	
	static char s[MINIMUMBUFFERSIZE];
	extern PCONTROL g;
	extern PLANGUAGEOBJECT Language[LANGUAGE_COUNT];
	
	switch(v->t){
	case STRINGVALUE:

		sprintf(s,"\"%s\"",v->e.s);
		break;
		
	case DATEVALUE:
	
		sprintf(s,"{%s}",Language[g->lang]->PrintDate((long)v->e.v));
		break;

	case NUMERICVALUE:

		if(v->e.v==DOUBLEMISSINGVALUE){
			sprintf(s,"#MISSING");
		}else{
			sprintf(s,"%g",v->e.v);
		}
		break;
	    
	}
	return (char *)s;
}
/* end of shared.c */
