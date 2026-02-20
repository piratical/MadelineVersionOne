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

/*                                         */
/* marker.h                                */
/*                                         */
/* init: 19990316 ET                       */
/*                                         */
/* -> defines ALLELE and MARKER structures */
/*                                         */

/*////////////////// */
/*                   */
/* MADELINE INCLUDES */
/*                   */
/*////////////////// */
#include "stringbuffersizes.h"

/*/////////////////////////////////////////////////////////// */
/*                                                            */
/* ALLELE structure implements a binary sorted tree           */
/* for storing the allele label (value), its frequency count, */
/* and index or rank.                                         */
/*                                                            */
/*/////////////////////////////////////////////////////////// */
typedef struct sALLELE{

	unsigned char  i;     /* index or rank of the allele   */
	unsigned short n;     /* frequency count of the allele */
	unsigned short v;     /* allele label or value         */
	
	struct sALLELE *l;    /* left node for allele with lower label   */
	struct sALLELE *r;    /* right node for allele with higher label */
	
} ALLELE,*PALLELE;


/*//////////////////////////////////////////////// */
/*                                                 */
/* MARKER structure stores information on a marker */
/*                                                 */
/*//////////////////////////////////////////////// */

typedef struct sMARKER{

	PALLELE      a;  /* root node of BST of alleles                             */
	int          s;  /* total sum used as divisor for frequencies -must be even */
	int          n;  /* total number of alleles for this marker                 */
	int          r;  /* highest rank for this marker                            */
	char nm[MAXMARKERNAMEWIDTH]; /* name of the marker                          */
	int        nmip; /* Non-mendelian inheritance issue counter -- pedigrees    */
	int        nmin; /* Non-mendelian inheritance issue counter -- nuclear fams */
}MARKER,*PMARKER;

/********************************/
/*                              */
/* FUNCTION DECLARATIONS        */
/*                              */
/********************************/
PMARKER NewMarker(int n);
PALLELE NewAllele(void);
PALLELE InsertAllele(PALLELE h,int v);
void TraverseAllele(PALLELE a,void (*visit)(PALLELE));
void TraverseAllele2(FILE *fp,PALLELE a,int s,void (*visit)(FILE *,PALLELE,int));
void ReverseTraverseAllele2(FILE *fp,PALLELE a,int s,void (*visit)(FILE *,PALLELE,int));
void TraverseAllele3(FILE *fp,PMARKER m,PALLELE a,int s,void (*visit)(FILE *,PMARKER,PALLELE,int));
void TraverseAllele4(PALLELE a,int s,void (*visit)(PALLELE,int));
void SetAlleleRanksAndSumCounts(PMARKER m,PALLELE a);
int  GetAlleleRank(PMARKER m, int v);
PALLELE InsertAlleleAndFrequency(PALLELE h,int v,double f);
void ZeroMarkerRanksAndSumCounts(PMARKER m);

#define MARKER_INCLUDED


