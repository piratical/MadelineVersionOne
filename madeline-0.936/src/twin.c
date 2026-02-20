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
/* "twin.c" is part of the Madeline software distribution.                           */
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
/* twin.c:                                         */
/*                                                 */
/* 19980318 Ed Trager                              */
/*                                                 */
/* -Provides a BST and support functions           */
/*  for keeping track of twin groups in a pedigree */
/*                                                 */

/*          */
/* INCLUDE: */
/*          */
#include "twin.h"

/* for Madeline error reporting: */
#include "logs.h"

/*                                                 */
/* NewTwinGroup(): returns a new MARKERMAP pointer */
/*                                                 */
PTWINGROUP NewTwinGroup(void){

	PTWINGROUP p;
	int i;
	
	p=(PTWINGROUP)malloc(sizeof(TWINGROUP));
	if(!p) eprintf(FATALFLAG,"NewTwinGroup(): out of memory allocating TWINGROUP");
	
	/*             */
	/* initialize: */
	/*             */
	p->key  ='\0';
	p->l    =NULL;
	p->r    =NULL;
	p->n    =0;
	/*                                   */
	/* initialize vector of individuals: */
	/*                                   */
	for(i=0;i<MAXTWINS;i++) p->t[i]=NULL;
	
	/*                 */
	/* return pointer: */
	/*                 */
	return p;
}


/*                                                */
/* InsertTwinGroup(): inserts a single twin group */
/*                    into the BST:               */
/*                                                */
PTWINGROUP InsertTwinGroup(PTWINGROUP h,char key,PINDIVIDUAL pindv){

	PTWINGROUP p,t;
	int cmp;
	
	/*                           */
	/* if there is no head node: */
	/*                           */
	if(!h){
		p=NewTwinGroup();
		/*                                                     */
		/* insert key and increment count:                     */
		/*                                                     */
		p->key=key;  /* the key                                */
		p->t[p->n++]=pindv;   /* the pointer to the individual */
		return p;
	}
	/*                                            */
	/* look for an extant node with key matching: */
	/* non-recursive implementation:              */
	/*                                            */
	for(p=h;p && (cmp=key-p->key)!=0;t=p,p=cmp<0?p->l:p->r);
	
	/*                                   */
	/* if no match, then add a new node: */
	/*                                   */
	if(!p){
		p=NewTwinGroup();
		/*                                                     */
		/* insert key and increment count:                     */
		/*                                                     */
		p->key=key;  /* the key                                */
		p->t[p->n++]=pindv;   /* the pointer to the individual */
		/*                                                     */
		/* put node into tree:                                 */
		/*                                                     */
		if(cmp<0) t->l=p;
		else      t->r=p;
	}else{
		/*                     */
		/* repeat of same key: */
		/*                     */
		if(p->n==MAXTWINS)
			eprintf(FATALFLAG,"InsertTwinGroup(): more than MAXTWINS (%i) twins in %s's twinship.",MAXTWINS,pindv->id);
		p->t[p->n++]=pindv; /* pointer to individual */
	}
	/*                       */
	/* return the head node: */
	/*                       */
	return h;
}


/*                                                */
/* TraverseTwinGroup(): Traverse a TWINGROUP BST: */
/*                                                */
void TraverseTwinGroup(PTWINGROUP p,void (*visit)(PTWINGROUP)){

	if(!p) return;
	TraverseTwinGroup(p->l,visit);
	visit(p);
	TraverseTwinGroup(p->r,visit);

}


/*                                                     */
/* LookupTwinGroup(): find the node with the matching  */
/* key, if present: returns NULL if not present        */
/*                                                     */
PTWINGROUP LookupTwinGroup(PTWINGROUP h, char key){

	PTWINGROUP p;
	int cmp;
	
	/*                                              */
	/* look for an extant node with key matching v: */
	/* non-recursive implementation:                */
	/*                                              */
	for(p=h;p && (cmp=key-p->key)!=0;p=cmp<0?p->l:p->r);
	return p;
}


/*                                                    */
/* FreeTwinGroup(): recursively free PTWINGROUP nodes */
/*                                                    */
void FreeTwinGroup(PTWINGROUP p){

	if(!p) return;
	if(p->l) FreeTwinGroup(p->l);
	if(p->r) FreeTwinGroup(p->r);
	
	/*                        */
	/* Free the current node: */
	/*                        */
	free(p);
}


/*                                       */
/*      TwinErrorReport                  */
/*                                       */
/*      Simple error reporting mechanism */
/*      used in prototyping DBF.C        */
/*                                       */
void TwinErrorReport(char *s,...){
	
	va_list argptr;

	fprintf(stderr,"\nTwin processor report:\n");
	va_start(argptr,s);
	vfprintf(stderr,s,argptr);
	va_end(argptr);        
	fprintf(stderr,"\n");
	exit(1);
}

/* end of twin.c */


