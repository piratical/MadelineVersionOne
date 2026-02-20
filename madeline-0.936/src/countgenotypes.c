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
/* "countgenotypes.c" is part of the Madeline software distribution.                 */
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
/* Library General Public License for more details.                                  */
/*                                                                                   */
/* You should have received a copy of the GNU Library General Public                 */
/* License along with this software source code distribution; if not,                */
/* write to the Free Software Foundation, Inc., 59 Temple Place -                    */
/* Suite 330, Boston, MA 02111-1307, USA.                                            */
/*                                                                                   */
/* Last modified by Ed Trager, March 2002.                                           */
/*                                                                                   */
/* ********************************************************************************* */
/* Initiated 2002.03.20 by ET                                                        */
/*                                                                                   */
/* Count how many genotypes toggled "on" for output are typed for a given            */
/* individual.  The result is stored as an attribute of the individual and can be    */
/* queried from the command line using _GenotypeCount and _PercentGenotyped.         */
/*                                                                                   */
/* ********************************************************************************* */
#include "countgenotypes.h"


/*                                     */
/* SetGenotypeCountAttributes()        */
/*                                     */
void SetGenotypeCountAttributes(PPEDIGREE hpd){
	
	TraverseBinaryPedigreeTree(hpd,PedigreeCountGenotypes);
	
}



/*                                    */
/*  PedigreeCountGenotypes()          */
/*                                    */
void PedigreeCountGenotypes(PPEDIGREE pd){
	
	TraverseIndividualBST(pd->h,SetIndividualGenotypeCountAttribute);
	
}


/*                                     */
/* SetIndividualGenotypeCountAttribute */
/*                                     */
void SetIndividualGenotypeCountAttribute(PINDIVIDUAL p){
	
	p->genotypecount=CountGenotypes(p);
	
}


/*                                                                          */
/* CountGenotypes(): returns the number of markers for which                */
/* the individual is typed among the set of markers flagged for             */
/* output.                                                                  */
/*                                                                          */
int CountGenotypes(PINDIVIDUAL p){

	int i,ii,count;
	char hold[MINIMUMBUFFERSIZE];
	char *o;

	/*                           */
	/* prophylactic checks:      */
	/* If not in the database,   */
	/* or no genotypes selected  */
	/* for output, just return 0 */
	/*                           */
	if(!p->db || !p->db->n.go) return 0;
	/*                        */
	/* check selected fields: */
	/*                        */
	for(count=i=0;i<p->db->n.go;i++){
		/*                  */
		/* get field index: */
		/*                  */
		ii=p->db->fi->g[i];
		o=p->db->ReadString(p->db,hold,ii,p->rci);
		/* ReadString() returns an empty string in    */
		/* the case of a missing value so this works: */                              
		if(o[0]) count++;
	}
	return count;
}


