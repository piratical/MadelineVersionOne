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
/* "vtree.c" is part of the Madeline software distribution.                          */
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
/* vtree.c               */
/*                       */
/* 19990325 ET initiated */
/*                       */
/*                       */
/*                       */
#include "vtree.h"

/*                */
/* For debugging: */
/*                */
void CheckVTree(PNODE);

/*                                                      */
/* CompareVariableObjects(): Comparison function to use */
/* with tree.c BST routines:                            */
/*                                                      */
int CompareVariableObjects(const void *a,const void *b){
	
	PVARIABLE aa;
	PVARIABLE bb;
	
	/*                                 */
	/* Cast pointers to VARIABLE type: */
	/*	                                */
	aa=(PVARIABLE)a;
	bb=(PVARIABLE)b;
	
	if(aa->t!=bb->t) TreeErrorReport("CompareVariableObjects(): elements are of different types");
	
	switch(aa->t){
	case NUMERICVALUE:
	case DATEVALUE:
		/*                                                */
		/* julian day values and numeric values are both  */
		/* stored as a double in e.v:                     */
		/*                                                */
		if(aa->e.v==bb->e.v)       return  0;
		else if(aa->e.v < bb->e.v) return -1;
		else                       return  1;
		
	case STRINGVALUE:
		return strcmp(aa->e.s,bb->e.s);
		
	default:
		TreeErrorReport("CompareVariableObjects(): type %i not recognized for this function",aa->t);
		break;
	}
	/*                      */
	/* please the compiler: */
	/*                      */
	return 0;
}


/*                                                         */
/* FreeVariableObject(): For use with tree.c BST routines: */
/*                                                         */
void FreeVariableObject(void *VariableObject){

	PVARIABLE pv;

	pv=(PVARIABLE)VariableObject;
	/*                                               */
	/* if the element type is a STRINGVALUE,         */
	/* then we must first free a copy of the string: */
	/*                                               */
	if(pv->t==STRINGVALUE) free(pv->e.s);
	/*                           */
	/* Now free the main object: */
	/*                           */
	free(pv);
}

/*                                                                     */
/* CopyVariableObject(): Returns a pointer to a VARIABLE object        */
/* that contains a copy of the data in VariableObject passed to        */
/* the function.  The return pointer is cast to void for compatability */
/* with the generic routines in tree.c:                                */
/*                                                                     */
void *CopyVariableObject(const void *VariableObject){

	PVARIABLE ov; /* original variable    */
	PVARIABLE nv; /* new copy of variable */

	/*                                          */
	/* assign a pointer to the original object: */
	/*                                          */
	ov=(PVARIABLE) VariableObject;
	
	/*                                      */
	/* allocate for the copy of the object: */
	/*	                                     */
	nv=(PVARIABLE)malloc(sizeof(VARIABLE));
	if(!nv) TreeErrorReport("CopyVariableObject(): no memory for VARIABLE object");
	
	/*                         */
	/* elementwise assignment: */
	/*                         */
	/*nv->nm=ov->nm;           */
	nv->t=ov->t;
	nv->i=ov->i;
	if(nv->t==STRINGVALUE){
		/*                           */
		/* allocate and copy string: */
		/*                           */
		nv->e.s=(char *)malloc((unsigned)strlen(ov->e.s)+1);
		if(!nv->e.s) TreeErrorReport("CopyVariableObject(): no memory for string copy");
		strcpy(nv->e.s,ov->e.s);		
	}else{
		/* assign value: could be julian date or numeric: */
		nv->e.v=ov->e.v;
	}
	return nv;
}


/*                */
/* FOR DEBUGGING: */
/*                */
void CheckVTree(PNODE t){

	PVARIABLE n;
	PVARIABLE l;
	PVARIABLE r;
	VARIABLE zz,*z=&zz;

	
	if(t){
		n=(PVARIABLE)t->o;
		if(n->t==NUMERICVALUE){
			z->e.v=-9999.0;
		}else{
			z->e.s="-NULL-";
		}
		l=(PVARIABLE)t->l?(PVARIABLE)t->l->o:z;
		r=(PVARIABLE)t->r?(PVARIABLE)t->r->o:z;
		
		switch(n->t){
		case NUMERICVALUE:
			printf("Key=%f : left=%f, right=%f\n",
				n->e.v,
				l->e.v,
				r->e.v
			);
			break;
		case STRINGVALUE:
			printf("Key=%s : left=%s, right=%s\n",
				n->e.s,
				l->e.s,
				r->e.s
			);
			break;
		}
		CheckVTree(t->l);
		CheckVTree(t->r);
	}
}

/* end of vtree.c */


