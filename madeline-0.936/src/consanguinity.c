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
/* "consanguinity.c" is part of the Madeline software distribution.                  */
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
/* The consanguinity functions were originally placed in madeline.c.  They have      */
/* been moved into consanguinity.c for clarity and to simplify code management.      */
/* --2002.03.21.ET                                                                   */
/* ********************************************************************************* */

#include "consanguinity.h"

/*                                  */
/* START OF CONSANGUINITY FUNCTIONS */
/*                                  */

/*                       */
/* CheckConsanguinity(): */
/*                       */
void CheckConsanguinity(PPEDIGREE pd){

	TraverseIndividualBST(pd->h,CheckMateConsanguinity);
	
}



/*                           */
/* CheckMateConsanguinity(): */
/*                           */
void CheckMateConsanguinity(PINDIVIDUAL p){

	int i;
	/*                                          */
	/* loop through all mates of an individual: */
	/*                                          */
	for(i=0;i<p->na;i++){
		/*                                                                  */
		/* only check if the pair is not already known to be consanguinous: */
		/*                                                                  */
		if(!(p->consanguinity && p->a[i]->consanguinity)){
			/*                       */
			/* show results to user: */
			/*                       */
			if(ConsanguinityTraceUpLeft(p,p->a[i])){
				/*             */
				/* log result: */
				/*             */
				p->consanguinity=p->a[i]->consanguinity=1;
				lprintf("Consanguinity detected between %s and %s in pedigree %s.\n",p,p->a[i],p->pd->fid);
			}
		}	
	}
}
	

/*                                                             */
/* ConsanguinityTraceUpLeft():                                 */
/*                                                             */
/* -Recurses up left's tree, calling TraceUpRight() to recurse */
/*  up right's tree and do the actual comparison               */
/*                                                             */
int ConsanguinityTraceUpLeft(PINDIVIDUAL left,PINDIVIDUAL right){

	if(!(left && right)) return 0;
	
	/*                                                          */
	/* if left and right are already known to be consanguinous, */
	/* were already done (They've already been reported, so     */
	/* we return zero):                                         */
	/*                                                          */
	if(left->consanguinity && right->consanguinity) return 0;
	
	
	/*                                                                */
	/* Check individual left against a trace up of right's ancestors: */
	/*                                                                */
	if(ConsanguinityTraceUpRight(left,right)) return 1;
	/*                                         */
	/* Recursion to trace up left's ancestors: */
	/*                                         */
	if(ConsanguinityTraceUpLeft(left->f,right)) return 1;
	if(ConsanguinityTraceUpLeft(left->m,right)) return 1;
	/*                   */
	/* no consanguinity: */
	/*                   */
	return 0;
}


/*                                                              */
/* ConsanguinityTraceUpRight(): traces up right's tree, keeping */
/* left fixed, and do the comparison against left:              */
/*                                                              */
int ConsanguinityTraceUpRight(PINDIVIDUAL left,PINDIVIDUAL right){

	/*                                            */
	/* No comparison to do if one or the other is */
	/* NULL:                                      */
	/*                                            */
	if(!(left && right)) return 0;

	/*                                                          */
	/* if left and right are already known to be consanguinous, */
	/* were already done (They've already been reported, so     */
	/* we return zero):                                         */
	/*                                                          */
	if(left->consanguinity && right->consanguinity) return 0;
	

	/*                                                 */
	/* Check left against right:                       */
	/*                                                 */
	/* if humans were doing this, we'd check to make   */
	/* sure the sex was the same first, and only after */
	/* that check that the people match.  But, from    */
	/* a programming perspective, that's just an extra */
	/* comparison:                                     */
	/*                                                 */
	if(left==right){
		/*                         */
		/* return positive result: */
		/*                         */
		return 1;
	}
	
	/*                                          */
	/* Recursion to trace up right's ancestors: */
	/*                                          */
	if(ConsanguinityTraceUpRight(left,right->f)) return 1;
	if(ConsanguinityTraceUpRight(left,right->m)) return 1;

	/*             */
	/* No matches: */
	/*             */
	return 0;
}

/*                                */
/* END OF CONSANGUINITY FUNCTIONS */
/*                                */
