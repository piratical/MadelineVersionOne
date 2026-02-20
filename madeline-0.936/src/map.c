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
/* "map.c" is part of the Madeline software distribution.                            */
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

/*       */
/* map.c */
/*       */

/*                                                                         */
/* A marker map database consists of a table containing                    */
/* the following fields of information:                                    */
/*                                                                         */
/* -marker name                                                            */
/* -chromosome                                                             */
/* -position (in CentiMorgans)                                             */
/* -ordinal index or order of the markers enumerated for each chromosome   */
/*  (i.e., enumerated from 1 to n for n markers on Chromosome j).          */
/*                                                                         */
/* In Madeline, a map is read into a BST in memory using the LOAD command. */
/* The structures in map.h and routines in this file, map.c, implement     */
/* the BST and its functionality.                                          */
/*                                                                         */

/*          */
/* INCLUDE: */
/*          */
#include "map.h"
#ifndef LOGS_INCLUDED
#include "logs.h"
#endif

#define DEBUG 0
/*                                                 */
/* NewMarkerMap(): returns a new MARKERMAP pointer */
/*                                                 */
PMARKERMAP NewMarkerMap(void){

	PMARKERMAP p;
	
	p=(PMARKERMAP)malloc(sizeof(MARKERMAP));
	if(!p) eprintf(FATALFLAG,"NewMarkerMap(): out of memory allocating MARKERMAP");
	
	/*                                                    */
	/* initialize:                                        */
	/*                                                    */
	p->l    =NULL;  /* left  node for alphabetic ordering */
	p->r    =NULL;  /* right node for alphabetic ordering */
	
	p->ll   =NULL;  /* left  node for chromosome-position ordering */
	p->rr   =NULL;  /* right node for chromosome-position ordering */
	
	p->nm[0]='\0';
	p->ch   =0;
	p->i    =0;
	p->ps   =DOUBLEMISSINGVALUE;
	p->psf  =DOUBLEMISSINGVALUE;
	p->psm  =DOUBLEMISSINGVALUE;
	
	/*                 */
	/* return pointer: */
	/*                 */
	return p;
	
}


/*                                                                     */
/* InsertMarkerMap(): inserts a single marker and its map information  */
/*                    into the alphabetically- and numerically-sorted  */
/*                    BSTs.                                            */
/*                                                                     */
/* NOTE that the sex-averaged map MUST always be present as numerical  */
/* sorting is based on the chromosome and sex-averaged map position of */
/* the markers. Gender-specific map positions are optional and are not */
/* used for sorting.                                                   */
/*                                                                     */
/* h  : pointer to head of BST                                         */
/* nm : marker name                                                    */
/* ch : chromosome number                                              */
/* i  : rank position                                                  */
/* ps : position in centimorgans for the sex-averaged map              */
/* psf: position in centimorgans for the female-specific map           */
/* psm: position in centimorgans for the male-specific map             */
/*                                                                     */
/* NOTE: Pass DOUBLEMISSINGVALUE for psf and psm if sex-specific maps  */
/* are not present.                                                    */
/*                                                                     */
PMARKERMAP InsertMarkerMap(PMARKERMAP h,char *nm,int ch,int i,double ps,double psf,double psm){

	PMARKERMAP p,q,t;
	int cmp;
	
	/*                           */
	/* if there is no head node: */
	/*                           */
	if(!h){
		p=NewMarkerMap();
		/*                */
		/* copy the name: */
		/*                */
		strncpy(p->nm,nm,MAXMARKERNAMEWIDTH-1);
		p->nm[MAXMARKERNAMEWIDTH]='\0';
		/*                          */
		/* copy the remaining info: */
		/*                          */
		
		if(DEBUG) lprintf("->ADDING HEAD NODE: ch=%2i mn=%s\n",ch,nm);
		
		p->ch=ch;
		p->i =i;
		p->ps=ps;
		p->psf=psf;
		p->psm=psm;
		return p;
	}
	/*                                               */
	/* look for an extant node with key matching nm, */
	/* non-recursive implementation:                 */
	/*                                               */
	for(p=h;p && (cmp=strcmp(nm,p->nm))!=0;t=p,p=cmp<0?p->l:p->r);
	
	/*                                   */
	/* if no match, then add a new node: */
	/*                                   */
	if(!p){
		p=NewMarkerMap();
		/*                */
		/* copy the name: */
		/*                */
		strncpy(p->nm,nm,MAXMARKERNAMEWIDTH-1);
		p->nm[MAXMARKERNAMEWIDTH]='\0';
		/*                          */
		/* copy the remaining info: */
		/*                          */
		p->ch=ch;
		p->i =i;
		p->ps=ps;
		p->psf=psf;
		p->psm=psm;
		/*                                            */
		/* put node into alphabetically-ordered tree: */
		/*                                            */
		if(cmp<0) t->l=p;
		else      t->r=p;
		
		if(DEBUG) lprintf("->ADDING ALPHA NODE: ch=%2i mn=%s LEAF='%c'\n",ch,nm,cmp<0?'L':'R');
		
	}

	/*                                               */
	/* also put node p into numerically-ordered tree */
	/* by chromosome number and position:            */
	/*                                               */
	
	/*                                                                        */
	/* first, double check that there's no other one                          */
	/* matching to the identical chromosome number and position               */
	/* already in there:                                                      */
	/*                                                                        */
	/* non-recursive implementation traversing (now on ll and rr, of course): */
	/*                                                                        */
	for(q=h;q && (cmp=CompareChromosomeAndRank(p,q))!=0;t=q,q=cmp<0?q->ll:q->rr);
	
	if(!q){
		
		if(cmp<0) t->ll=p;
		else      t->rr=p;
	
		if(DEBUG) lprintf("->ADDING NUMERIC NODE: ch=%2i mn=%s ordinal=%2i LEAF='%c'\n",ch,nm,i,cmp<0?'L':'R');

	}else{
		/*                                                         */
		/* Can't have two different markers mapped to exactly same */
		/* ordinal position on the map:                            */
		/*                                                         */
		eprintf(FATALFLAG,"InsertMarkerMap(): %s and %s both map to chromosome %i position %i!",
			p->nm,
			q->nm,
			p->ch,
			p->i
		);	
	}	

	
	/*                       */
	/* return the head node: */
	/*                       */
	return h;
}

/*                                                         */
/* 980507 ET addendum:                                     */
/*                                                         */
/* CompareChromosomeAndRank(PMARKERMAP p,PMARKERMAP q):    */
/*                                                         */
/* p is not yet connected to the tree                      */
/* q is the node being compared that's already in the tree */
/*                                                         */
int CompareChromosomeAndRank(PMARKERMAP p,PMARKERMAP q){

	if     (p->ch<q->ch) return -1;
	else if(p->ch>q->ch) return  1;
	else{
		/*                         */
		/* same chromosome number: */
		/*                         */
		if     (p->i<q->i) return -1;
		else if(p->i>q->i) return  1;
		else               return  0;
	
	}
}


/*                                                               */
/* TraverseMarkerMap(): Traverse a MARKERMAP BST alphabetically: */
/*                                                               */
void TraverseMarkerMap(PMARKERMAP p,void (*visit)(PMARKERMAP)){

	if(!p) return;
	TraverseMarkerMap(p->l,visit);
	visit(p);
	TraverseMarkerMap(p->r,visit);

}

/*                                                                 */
/* TraverseMarkerMapForChromosome():                               */ 
/*                       Traverse a MARKERMAP BST, as in           */
/*                       TraverseMarkerMap() function, but apply   */
/*                       the visit function only if the chromosome */
/*                       matches chr: alphabetic traversal:        */
/*                                                                 */
void TraverseMarkerMapForChromosome(PMARKERMAP p,int chr,void (*visit)(PMARKERMAP)){

	if(!p) return;
	TraverseMarkerMapForChromosome(p->l,chr,visit);
	if(p->ch==chr) visit(p);
	TraverseMarkerMapForChromosome(p->r,chr,visit);

}

/*                                                            */
/* TraverseMarkerMapByPosition(): Traverse a MARKERMAP BST by */
/* chromosome and map position                                */
/*                                                            */
void TraverseMarkerMapByPosition(PMARKERMAP p,void (*visit)(PMARKERMAP)){

	if(!p) return;
	TraverseMarkerMapByPosition(p->ll,visit);
	visit(p);
	TraverseMarkerMapByPosition(p->rr,visit);

}

/*                                                                            */
/* TraverseMarkerMapByPositionForChromosome():                                */
/*                       Traverse a MARKERMAP BST, as in                      */
/*                       TraverseMarkerMap() function, but apply              */
/*                       the visit function only if the chromosome            */
/*                       matches chr: traversal is by chromosome and position */
/*                                                                            */
void TraverseMarkerMapByPositionForChromosome(PMARKERMAP p,int chr,void (*visit)(PMARKERMAP)){

	if(!p) return;
	TraverseMarkerMapByPositionForChromosome(p->ll,chr,visit);
	if(p->ch==chr) visit(p);
	TraverseMarkerMapByPositionForChromosome(p->rr,chr,visit);
	
}





/*                                                         */
/* SetIntermarkerDistances: recursive function             */
/* to set the intermarker distances.                       */
/*                                                         */
/*void SetIntermarkerDistances(PMARKERMAP p){              */
/*	                                                        */
/*	static int    lastchromosome=0;                         */
/*	static double lastposition  =0.0;                       */
/*	                                                        */
/*	if(!p) return;                                          */
/*	//                                                      */
/*	// visit the left node:                                 */
/*	//                                                      */
/*	SetIntermarkerDistances(p->l);                          */
/*	                                                        */
/*	//                                                      */
/*	// visiting this node:                                  */
/*	//                                                      */
/*                                                         */
/*	//                                                      */
/*	// reset things if we're on a new chromosome:           */
/*	//                                                      */
/*	if(lastchromosome!=p->ch){                              */
/*		lastposition  =0.0;                                    */
/*		lastchromosome=p->ch;                                  */
/*	}                                                       */
/*	//                                                      */
/*	// subtract to get distance from previous node:         */
/*	//                                                      */
/*	p->dp=p->ps-lastposition;                               */
/*	                                                        */
/*	//                                                      */
/*	// save the current position for the next time through: */
/*	//                                                      */
/*	lastposition=p->ps;                                     */
/*	                                                        */
/*	//                                                      */
/*	// visit the right node:                                */
/*	//                                                      */
/*	SetIntermarkerDistances(p->r);                          */
/*}                                                        */
/*                                                         */

/*                                                     */
/* LookupMarkerMap(): find the node with the matching  */
/* nm key, if present: returns NULL if not present     */
/*                                                     */
PMARKERMAP LookupMarkerMap(PMARKERMAP h, const char *nm){

	PMARKERMAP p;
	int cmp;
	
	/*                                              */
	/* look for an extant node with key matching v: */
	/* non-recursive implementation:                */
	/*                                              */
	for(p=h;p && (cmp=strcmp(nm,p->nm))!=0;p=cmp<0?p->l:p->r);
	return p;

}


/*                                                  */
/* FreeMap(): recursively free MAPMARKER tree nodes */
/*                                                  */
void FreeMap(PMARKERMAP p){

	if(!p) return;
	if(p->l) FreeMap(p->l);
	if(p->r) FreeMap(p->r);
	
	/*                        */
	/* Free the current node: */
	/*                        */
	free(p);
}

