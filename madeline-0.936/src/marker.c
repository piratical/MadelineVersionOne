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
/* "marker.h" is part of the Madeline software distribution.                         */
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
/* Last modified by Ed Trager, November 2002.                                        */
/*                                                                                   */
/* ********************************************************************************* */

/*          */
/* marker.c */
/*          */

#include <stdio.h>
#include <stdlib.h>

#ifndef MARKER_INCLUDED
#include "marker.h"
#endif

#ifndef LOGS_INCLUDED
#include "logs.h"
#endif

#ifndef TOOLS_INCLUDED
#include "tools.h"
#endif

#include <math.h>

/*                                             */
/*                                             */
/* NewMarker(): returns a pointer to n MARKERs */
/*                                             */
/*                                             */
PMARKER NewMarker(int n){

	int i,j;
	PMARKER m;
	
	m=(PMARKER)malloc((unsigned)n*sizeof(MARKER));
	if(!m) ToolsErrorReport("NewMarker(): Unable to allocate memory for %i markers",n);

	for(i=0;i<n;i++){

		m[i].a =NULL; /* root node                                               */
		m[i].n =0;    /* total number of alleles for this marker                 */
		m[i].s =0;    /* sum used as denominator for frequencies                 */
		m[i].r =1;    /* rank for this marker                                    */
		m[i].nmip=0;   /* number of inheritance errors -- count by pedigree      */
		m[i].nmin=0;   /* number of inheritance errors -- count by nuclear fams. */
		for(j=0;j<MAXMBASEFIELDNAMELENGTH;j++) m[i].nm[j]='\0';

	}
	return m;
}

/*                                                     */
/*                                                     */
/* NewAllele(): returns a pointer to a new ALLELE node */
/*                                                     */
/*                                                     */
PALLELE NewAllele(void){

	PALLELE a;
	
	a=(PALLELE)malloc((unsigned)sizeof(ALLELE));
	if(!a) ToolsErrorReport("NewAllele(): Unable to allocate memory for an ALLELE");

	a->i=0;
	a->n=0;
	a->v=0;
	a->l=NULL;
	a->r=NULL;

	return a;

}


/*                                                             */
/*                                                             */
/* InsertAllele(): Inserts an allele value (or label), v,      */
/*                 into the binary sorted tree having the      */
/*                 head node h.  If the allele value v al-     */
/*                 ready appears in the allele tree, then      */
/*                 only the frequency counter is incremented.  */
/*                                                             */
/*                 To keep things efficient, as well as in-    */
/*                 teresting, a non-recursive implementation   */
/*                 is used for the insert function (while some */
/*                 other functions are recursive).             */
/*                                                             */
/*                 The head node is returned.                  */
/*                                                             */
PALLELE InsertAllele(PALLELE h,int v){

	PALLELE p,a;

	/*                           */
	/* if there is no head node: */
	/*                           */
	if(!h){
		a=NewAllele();
		a->v=v;
		a->n++;
		return a;
	}
	/*                                              */
	/* look for an extant node with key matching v: */
	/*                                              */
	for(a=h;a && v!=a->v;p=a,a=v<a->v?a->l:a->r);
	
	/*                                   */
	/* if no match, then add a new node: */
	/*                                   */
	if(!a){
		a=NewAllele();
		a->v=v;
		if(a->v<p->v) p->l=a;
		else          p->r=a;
	}
	/*                                                 */
	/* regardless of new or repeat, increment counter: */
	/*                                                 */
	a->n++;
	/*                       */
	/* return the head node: */
	/*                       */
	return h;
}


/*                                                            */
/*                                                            */
/* TraverseAllele(): Traverses an allele tree in sorted order */
/*                                                            */
/*                                                            */
void TraverseAllele(PALLELE a,void (*visit)(PALLELE)){

	if(!a) return;
	TraverseAllele(a->l,visit);
	visit(a);
	TraverseAllele(a->r,visit);

}

/*                                                                      */
/* TraverseAllele2, like TraverseAllele(), visits the ALLELEs in sorted */
/* order, but differs in accepting a visit() function with additional   */
/* parameters, a file parameter, and an integer s                       */
/*                                                                      */
void TraverseAllele2(FILE *fp,PALLELE a,int s,void (*visit)(FILE *,PALLELE,int)){

	if(!a) return;
	TraverseAllele2(fp,a->l,s,visit);

	visit(fp,a,s);

	TraverseAllele2(fp,a->r,s,visit);

}


/*                                                       */
/* ReverseTraverseAllele2, like TraverseAllele2(),       */
/* but traverses tree in REVERSE order, from high to low */
/*                                                       */
void ReverseTraverseAllele2(FILE *fp,PALLELE a,int s,void (*visit)(FILE *,PALLELE,int)){

	if(!a) return;

	ReverseTraverseAllele2(fp,a->r,s,visit);

	visit(fp,a,s);

	ReverseTraverseAllele2(fp,a->l,s,visit);

}


/*                                                                      */
/* TraverseAllele3, like TraverseAllele(), visits the ALLELEs in sorted */
/* order, but differs in accepting a visit() function with additional   */
/* parameters, a file parameter, a MARKER parameter, and an integer s   */
/*                                                                      */
void TraverseAllele3(FILE *fp,PMARKER m,PALLELE a,int s,void (*visit)(FILE *,PMARKER,PALLELE,int)){

	if(!a) return;
	TraverseAllele3(fp,m,a->l,s,visit);

	visit(fp,m,a,s);

	TraverseAllele3(fp,m,a->r,s,visit);

}

/*                                                            */
/*                                                            */
/* TraverseAllele(): Traverses an allele tree in sorted order */
/*                                                            */
/*                                                            */
void TraverseAllele4(PALLELE a,int s,void (*visit)(PALLELE,int)){

	if(!a) return;
	TraverseAllele4(a->l,s,visit);
	visit(a,s);
	TraverseAllele4(a->r,s,visit);

}


/*                                                   */
/*                                                   */
/* SetAlleleRanksAndSumCounts(PALLELE a):            */
/* sets the a->i member of each                      */
/* ALLELE to the correct rank value and also         */
/* sums up the counts for each allele                */
/* by recursive traversal                            */
/* through the BST allele tree.  An int *r is passed */
/* for storing and incrementing the current rank,    */
/* and int *s for storing a running sum              */
/*                                                   */
void SetAlleleRanksAndSumCounts(PMARKER m,PALLELE a){

	
	if(!a) return;
	SetAlleleRanksAndSumCounts(m,a->l);
	
	a->i= m->r++;
	m->s+=a->n;
	m->n++;
	
	SetAlleleRanksAndSumCounts(m,a->r);
}


/*                                                             */
/*                                                             */
/* GetAlleleRank(): Returns the rank of an allele label value, */
/*                  or zero if not found:                      */
int GetAlleleRank(PMARKER m, int v){

	PALLELE a;
	
	/*                                              */
	/* look for an extant node with key matching v: */
	/*                                              */
	for(a=m->a;a && v!=a->v;a=v<a->v?a->l:a->r);

	if(!a) return (int)0;
	
	return a->i;
}


/*                                                             */
/*                                                             */
/* InsertAlleleAndFrequency():                                 */
/*                                                             */
/*                 Like InsertAllele(), this function          */
/*                 Inserts an allele value (or label), v,      */
/*                 into the binary sorted tree having the      */
/*                 head node h.  The allele frequency is       */
/*                 also stored.                                */
/*                                                             */
/*                 To keep things efficient, as well as in-    */
/*                 teresting, a non-recursive implementation   */
/*                 is used for the insert function (while some */
/*                 other functions are recursive).             */
/*                                                             */
/*                 The head node is returned.                  */
/*                                                             */
PALLELE InsertAlleleAndFrequency(PALLELE h,int v,double f){

	PALLELE p,a;
	
	
	/*                                                     */
	/* Regardless of new or repeat, alter the count data.  */
	/* Since the structure only stores the counts, we're   */ 
	/* going to multiply the frequency by 10000 and store  */
	/* the resulting integer.  We know that the frequency  */
	/* never exceeds 1.0, so the integer will never exceed */
	/* 10000:                                              */
	/*                                                     */
	f=Round(10000.0*f);
	
	/*                           */
	/* if there is no head node: */
	/*                           */
	if(!h){
		a=NewAllele();
		a->v=v;
		a->n=(short unsigned int)f;
		return a;
	}
	/*                                              */
	/* look for an extant node with key matching v: */
	/*                                              */
	for(a=h;a && v!=a->v;p=a,a=v<a->v?a->l:a->r);
	
	/*                                   */
	/* if no match, then add a new node: */
	/*                                   */
	if(!a){
		a=NewAllele();
		a->v=v;
		if(a->v<p->v) p->l=a;
		else          p->r=a;
	}
	a->n=(short unsigned int)f;
	
	/*                       */
	/* return the head node: */
	/*                       */
	return h;
}

/*                               */
/* ZeroMarkerRanksAndSumCounts() */
/*                               */
void ZeroMarkerRanksAndSumCounts(PMARKER m){
	
	if(!m) return;
	
	m->r=1;
	m->s=0;
	m->n=0;
}

