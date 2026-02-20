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
/* "map.h" is part of the Madeline software distribution.                            */
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
/* map.h */
/*       */
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "stringbuffersizes.h"
#include "tools.h"

/*                                                                                      */
/* MARKERMAP is a kind of fun structure.  Here, we have a structure with nodes for      */
/* building TWO binary trees -- one for alphabetic ordering of the markers by name, and */
/* the other for ordering by chromosome and position.  A naive BST implementation is    */
/* sufficient for these.                                                                */
/*                                                                                      */
/* NOTE: Madeline mostly uses the cumulative distance field, but can display the        */
/*       recombination fractions and inter-marker distances when, for example, using    */
/*       the LIST MAP command.                                                          */
/*                                                                                      */
typedef struct sMARKERMAP{

	struct sMARKERMAP *l;  /* left node   (alphabetic ordering by marker name) */
	struct sMARKERMAP *r;  /* right node  (alphabetic ordering by marker name) */

	struct sMARKERMAP *ll; /* left node   (numeric ordering by chromosome and position) */
	struct sMARKERMAP *rr; /* right node  (numeric ordering by chromosome and position) */
		
	char nm[MAXMARKERNAMEWIDTH]; /* name of marker                                              */
	int  ch;                     /* chromosome: 1-23                                            */
	int  i;                      /* rank index (ordinal)                                        */
	double ps;                   /* position in centiMorgans on sex-averaged map                */
	double psf;                  /* position in centiMorgans on female-specific map, if present */
	double psm;                  /* position in centiMorgans on male-specific map, if present   */
	
}MARKERMAP,*PMARKERMAP;



/*/////////////////////////////           */
/*                                        */
/* MARKERMAP Object allocation functions: */
/*                                        */
/*/////////////////////////////           */
PMARKERMAP NewMarkerMap(void);


/*/////////////////////////////////////////////// */
/*                                                */
/* Functions to insert and traverse ALLELE nodes  */
/* and for setting the node rank and summing up   */
/* allele frequencies:                            */
/*                                                */
/*/////////////////////////////////////////////// */
PMARKERMAP InsertMarkerMap(PMARKERMAP h,char *nm,int ch,int i,double ps,double psf, double psm);
void    TraverseMarkerMap(PMARKERMAP h,void (*visit)(PMARKERMAP));
void    TraverseMarkerMapForChromosome(PMARKERMAP p,int chr,void (*visit)(PMARKERMAP));


/*void    SetIntermarkerDistances(PMARKERMAP p); */
PMARKERMAP LookupMarkerMap(PMARKERMAP h, const char *nm);
void FreeMap(PMARKERMAP p);
/*                    */
/* 980507 ET Addenda: */
/*                    */
int     CompareChromosomeAndRank(PMARKERMAP p,PMARKERMAP q);
void    TraverseMarkerMapByPosition(PMARKERMAP h,void (*visit)(PMARKERMAP));
void    TraverseMarkerMapByPositionForChromosome(PMARKERMAP p,int chr,void (*visit)(PMARKERMAP));

#define MAP_INCLUDED


