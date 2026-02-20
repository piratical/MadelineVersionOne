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
/* "complexity.c" is part of the Madeline software distribution.                     */
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
/* Last modified by Ed Trager, May, 2003.                                            */
/*                                                                                   */
/* ********************************************************************************* */

/*                                                      */
/* complexity.c                                         */
/*                                                      */
/* Calculate pedigree complexity as 2N-F where:         */
/*                                                      */
/*  N = Number of non-founders                          */
/*  F = Number of founders                              */
/*                                                      */
#include "complexity.h"

/*                           */
/* ResetComplexityCounters() */
/*                           */
void ResetComplexityCounters(PPEDIGREE pd){
	
	pd->NumberOfFounders=0;
	pd->NumberOfNonFounders=0;
	
}

/*                            */
/* AddToComplexityCounters()  */
/*                            */
void AddToComplexityCounters(PINDIVIDUAL p){
	
	/* Only count individuals who are not marked */
	/* for exclusion                             */
	if(!p->excluded){
		
		/* People with parents are non-founders: */
		if(p->m && p->f) p->pd->NumberOfNonFounders++;
		else             p->pd->NumberOfFounders++;
		
	}
}

/*                                     */
/* CalculateComplexityOfOnePedigree() */
/*                                     */
void CalculateComplexityOfOnePedigree(PPEDIGREE pd){
	
	ResetComplexityCounters(pd);
	TraverseIndividualBST(pd->h,AddToComplexityCounters);
	
}

/*                                   */
/* CalculateComplexityOfPedigrees()  */
/*                                   */
void CalculateComplexityOfPedigrees(PCONTROL c){
	
	TraverseBinaryPedigreeTree(c->pd,CalculateComplexityOfOnePedigree);
	
}


