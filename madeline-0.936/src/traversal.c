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
/* "traversal.c" is part of the Madeline software distribution.                      */
/* Written by Edward H. Trager.                                                      */
/*                                                                                   */
/* COPYRIGHT (C) 2000,2002,2003 THE REGENTS OF THE UNIVERSITY OF MICHIGAN,           */
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
/* General Public License for more details.                                          */
/*                                                                                   */
/* You should have received a copy of the GNU General Public                         */
/* License along with this software source code distribution; if not,                */
/* write to the Free Software Foundation, Inc., 59 Temple Place -                    */
/* Suite 330, Boston, MA 02111-1307, USA.                                            */
/*                                                                                   */
/* Last modified by Ed Trager, February, 2003.                                       */
/*                                                                                   */
/* ********************************************************************************* */

#include "traversal.h"

/*                                                                 */
/* TraverseIndividualBST2(): Recursively visit every node in a     */
/* PINDIVIDUAL Binary Sorted Tree with PCONTROL and PINDIVIDUAL as */
/* function parameters                                             */
/*                                                                 */
void TraverseIndividualBST2(PCONTROL c,PINDIVIDUAL p,void (*visit)(PCONTROL,PINDIVIDUAL)){

	if(!p) return;
	TraverseIndividualBST2(c,p->l,visit);
	(*visit)(c,p);
	TraverseIndividualBST2(c,p->r,visit);

}


/*                                                                       */
/* TraverseIndividualBSTUntilTrue(): Recursively visit nodes in a        */
/* PINDIVIDUAL Binary Sorted Tree until a condition is met: if condition */
/* (*eval)() is met, the function returns immediately without bothering  */
/* to evaluate further nodes                                             */
/*                                                                       */
int TraverseIndividualBSTUntilTrue(PINDIVIDUAL p,int (*eval)(PINDIVIDUAL)){

	if(!p) return 0;
	if((*eval)(p))
		return 1;
	if(TraverseIndividualBSTUntilTrue(p->l,eval))
		return 1;
	if(TraverseIndividualBSTUntilTrue(p->r,eval))
		return 1;
	return 0;
}

/*                                                                       */
/* TraverseIndividualBSTUntilTrue2(): Recursively visit nodes in a       */
/* PINDIVIDUAL Binary Sorted Tree until a condition is met: if condition */
/* (*eval)() is met, the function returns immediately without bothering  */
/* to evaluate further nodes.                                            */
/*                                                                       */
/* TraverseIndividualBSTUntilTrue2() uses more parameters                */
/* than the original function, TraverseIndividualBSTUntilTrue().         */
/*                                                                       */
int TraverseIndividualBSTUntilTrue2(PCONTROL c,PPEDIGREE pd,PINDIVIDUAL p,int (*eval)(PCONTROL,PPEDIGREE,PINDIVIDUAL)){

	if(!p) return 0;
	if((*eval)(c,pd,p))
		return 1;
	if(TraverseIndividualBSTUntilTrue2(c,pd,p->l,eval))
		return 1;
	if(TraverseIndividualBSTUntilTrue2(c,pd,p->r,eval))
		return 1;
	return 0;
}

/*                                                                       */
/* TraverseIndividualBSTUntilDataFound(): Recursively visit nodes in a   */
/* PINDIVIDUAL Binary Sorted Tree until a condition is met: if condition */
/* (*eval)() is met, the function returns immediately without bothering  */
/* to evaluate further nodes                                             */
/*                                                                       */
int TraverseIndividualBSTUntilDataFound(PINDIVIDUAL p){

	if(!p) return 0;
	if(p->hasdata)
		return 1;
	if(TraverseIndividualBSTUntilDataFound(p->l))
		return 1;
	if(TraverseIndividualBSTUntilDataFound(p->r))
		return 1;
	return 0;
}




/*                                                             */
/* TraverseIndividualBST4(): Recursively visit every node in a */
/* PINDIVIDUAL Binary Sorted Tree:                             */
/*                                                             */
void TraverseIndividualBST4(PCONTROL c,PPEDIGREE pd,PINDIVIDUAL p,void (*visit)(PCONTROL,PPEDIGREE,PINDIVIDUAL)){

	if(!p) return;
	TraverseIndividualBST4(c,pd,p->l,visit);
	(*visit)(c,pd,p);
	TraverseIndividualBST4(c,pd,p->r,visit);

}

/*                                                             */
/* TraverseIndividualBST5(): Recursively visit every node in a */
/* PINDIVIDUAL Binary Sorted Tree:                             */
/*                                                             */
void TraverseIndividualBST5(PPEDIGREE pd,PINDIVIDUAL p,void (*visit)(PPEDIGREE,PINDIVIDUAL)){

	if(!p) return;
	TraverseIndividualBST5(pd,p->l,visit);
	(*visit)(pd,p);
	TraverseIndividualBST5(pd,p->r,visit);

}



/*                                                                       */
/* TraverseIndividualBST6(): Recursively visit every node in a           */
/* PINDIVIDUAL Binary Sorted Tree: returns an integer sum obtained       */
/* by adding up the return value of the visit function for everybody     */
/* in the BST. A CONTROL is passed along too so we can do bookkeeping in */
/* the control too.                                                      */
/*                                                                       */
int TraverseIndividualBST6(PCONTROL c,PINDIVIDUAL p,int (*visit)(PCONTROL,PINDIVIDUAL)){

	int sum=0;
	if(!p) return 0;
	sum+=TraverseIndividualBST6(c,p->l,visit);
	sum+=(*visit)(c,p);
	sum+=TraverseIndividualBST6(c,p->r,visit);
	return sum;
}


/*                                                                   */
/* TraverseIndividualBST7(): Recursively visit every node in a       */
/* PINDIVIDUAL Binary Sorted Tree: returns an integer sum obtained   */
/* by adding up the return value of the visit function for everybody */
/* in the BST. Unlike TraverseIndividualBST6() above, no pointer to  */
/* a CONTROL structure is passed in this case:                       */
/*                                                                   */
int TraverseIndividualBST7(PINDIVIDUAL p,int (*visit)(PINDIVIDUAL)){

	int sum=0;
	if(!p) return 0;
	sum+=TraverseIndividualBST7(p->l,visit);
	sum+=(*visit)(p);
	sum+=TraverseIndividualBST7(p->r,visit);
	return sum;
}


/*                                                                        */
/* TraverseIndividualBSTVisitProband(): Recursively visit every node in a */
/* PINDIVIDUAL Binary Sorted Tree with PCONTROL and PINDIVIDUAL as        */
/* function parameters: ONLY apply visit() to the PROBANDS                */
/*                                                                        */
void TraverseIndividualBSTVisitProband(PCONTROL c,PINDIVIDUAL p,void (*visit)(PCONTROL,PINDIVIDUAL)){

	if(!p) return;
	TraverseIndividualBSTVisitProband(c,p->l,visit);
	if(p->proband) (*visit)(c,p);
	TraverseIndividualBSTVisitProband(c,p->r,visit);

}

/*                                                                           */
/* TraverseIndividualBSTVisitNonProband(): Recursively visit every node in a */
/* PINDIVIDUAL Binary Sorted Tree with PCONTROL and PINDIVIDUAL as           */
/* function parameters: ONLY apply visit to the NON-PROBANDS                 */
/*                                                                           */
void TraverseIndividualBSTVisitNonProband(PCONTROL c,PINDIVIDUAL p,void (*visit)(PCONTROL,PINDIVIDUAL)){

	if(!p) return;
	TraverseIndividualBSTVisitNonProband(c,p->l,visit);
	if(!p->proband) (*visit)(c,p);
	TraverseIndividualBSTVisitNonProband(c,p->r,visit);

}



/*                                                                        */
/* TraverseIndividualBSTMarkDataFlag(): Recursively visit every node in a */
/* PINDIVIDUAL Binary Sorted Tree and mark individuals who "have data"    */
/* based on the return value of the (*eval)() function.                   */
/*                                                                        */
void TraverseIndividualBSTMarkDataFlag(PINDIVIDUAL p,int (*HasData)(PINDIVIDUAL)){

	if(!p) return;
	TraverseIndividualBSTMarkDataFlag(p->l,HasData);

	/*                                                                    */
	/* reset any old has data, included, and counted flag states to zero: */
	/*                                                                    */
	/* if(p->hasdata )  p->hasdata =0;                                    */
	/* if(p->haspdata ) p->haspdata=0;                                    */
	/* if(p->hasgdata ) p->hasgdata=0;                                    */
	/* if(p->included)  p->included=0;                                    */
	/* if(p->counted )  p->counted =0;                                    */
	/*                                                                    */
	p->hasdata=p->haspdata=p->hasgdata=p->included=p->counted=0;

	/*                           */
	/* mark under new condition: */
	/*                           */
	if((*HasData)(p)) p->hasdata++;

	TraverseIndividualBSTMarkDataFlag(p->r,HasData);
}


/*                                                                         */
/* TraverseIndividualBSTClearDataFlag(): Recursively visit every node in a */
/* PINDIVIDUAL Binary Sorted Tree and reset the p->hasdata flag to zero    */
/*                                                                         */
void TraverseIndividualBSTClearDataFlag(PINDIVIDUAL p){

	if(!p) return;
	TraverseIndividualBSTClearDataFlag(p->l);

	if(p->hasdata ) p->hasdata =0;
	if(p->included) p->included=0;
	if(p->hasgdata) p->hasgdata=0;
	if(p->haspdata) p->haspdata=0;
	
	TraverseIndividualBSTClearDataFlag(p->r);

}
