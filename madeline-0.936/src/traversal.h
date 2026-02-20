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
/* "traversal.h" is part of the Madeline software distribution.                      */
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


#ifndef CONTROL_INCLUDED
#include "control.h"
#endif

#ifndef PEDIGREE_INCLUDED
#include "pedigree.h"
#endif

/*                                                                 */
/* TraverseIndividualBST2(): Recursively visit every node in a     */
/* PINDIVIDUAL Binary Sorted Tree with PCONTROL and PINDIVIDUAL as */
/* function parameters                                             */
/*                                                                 */
void TraverseIndividualBST2(PCONTROL c,PINDIVIDUAL p,void (*visit)(PCONTROL,PINDIVIDUAL));

/*                                                                       */
/* TraverseIndividualBSTUntilTrue(): Recursively visit nodes in a        */
/* PINDIVIDUAL Binary Sorted Tree until a condition is met: if condition */
/* (*eval)() is met, the function returns immediately without bothering  */
/* to evaluate further nodes                                             */
/*                                                                       */
int TraverseIndividualBSTUntilTrue(PINDIVIDUAL p,int (*eval)(PINDIVIDUAL));

/*                                                                       */
/* TraverseIndividualBSTUntilTrue2(): Recursively visit nodes in a       */
/* PINDIVIDUAL Binary Sorted Tree until a condition is met: if condition */
/* (*eval)() is met, the function returns immediately without bothering  */
/* to evaluate further nodes.                                            */
/*                                                                       */
/* TraverseIndividualBSTUntilTrue2() uses more parameters                */
/* than the original function, TraverseIndividualBSTUntilTrue().         */
/*                                                                       */
int TraverseIndividualBSTUntilTrue2(PCONTROL c,PPEDIGREE pd,PINDIVIDUAL p,int (*eval)(PCONTROL,PPEDIGREE,PINDIVIDUAL));

/*                                                                       */
/* TraverseIndividualBSTUntilDataFound(): Recursively visit nodes in a   */
/* PINDIVIDUAL Binary Sorted Tree until a condition is met: if condition */
/* (*eval)() is met, the function returns immediately without bothering  */
/* to evaluate further nodes                                             */
/*                                                                       */
int TraverseIndividualBSTUntilDataFound(PINDIVIDUAL p);

/*                                                             */
/* TraverseIndividualBST4(): Recursively visit every node in a */
/* PINDIVIDUAL Binary Sorted Tree:                             */
/*                                                             */
void TraverseIndividualBST4(PCONTROL c,PPEDIGREE pd,PINDIVIDUAL p,void (*visit)(PCONTROL,PPEDIGREE,PINDIVIDUAL));

/*                                                             */
/* TraverseIndividualBST5(): Recursively visit every node in a */
/* PINDIVIDUAL Binary Sorted Tree:                             */
/*                                                             */
void TraverseIndividualBST5(PPEDIGREE pd,PINDIVIDUAL p,void (*visit)(PPEDIGREE,PINDIVIDUAL));

/*                                                                       */
/* TraverseIndividualBST6(): Recursively visit every node in a           */
/* PINDIVIDUAL Binary Sorted Tree: returns an integer sum obtained       */
/* by adding up the return value of the visit function for everybody     */
/* in the BST. A CONTROL is passed along too so we can do bookkeeping in */
/* the control too.                                                      */
/*                                                                       */
int TraverseIndividualBST6(PCONTROL c,PINDIVIDUAL p,int (*visit)(PCONTROL,PINDIVIDUAL));

/*                                                                   */
/* TraverseIndividualBST7(): Recursively visit every node in a       */
/* PINDIVIDUAL Binary Sorted Tree: returns an integer sum obtained   */
/* by adding up the return value of the visit function for everybody */
/* in the BST. Unlike TraverseIndividualBST6() above, no pointer to  */
/* a CONTROL structure is passed in this case:                       */
/*                                                                   */
int TraverseIndividualBST7(PINDIVIDUAL p,int (*visit)(PINDIVIDUAL));

/*                                                                        */
/* TraverseIndividualBSTVisitProband(): Recursively visit every node in a */
/* PINDIVIDUAL Binary Sorted Tree with PCONTROL and PINDIVIDUAL as        */
/* function parameters: ONLY apply visit() to the PROBANDS                */
/*                                                                        */
void TraverseIndividualBSTVisitProband(PCONTROL c,PINDIVIDUAL p,void (*visit)(PCONTROL,PINDIVIDUAL));

/*                                                                           */
/* TraverseIndividualBSTVisitNonProband(): Recursively visit every node in a */
/* PINDIVIDUAL Binary Sorted Tree with PCONTROL and PINDIVIDUAL as           */
/* function parameters: ONLY apply visit to the NON-PROBANDS                 */
/*                                                                           */
void TraverseIndividualBSTVisitNonProband(PCONTROL c,PINDIVIDUAL p,void (*visit)(PCONTROL,PINDIVIDUAL));

/*                                                                        */
/* TraverseIndividualBSTMarkDataFlag(): Recursively visit every node in a */
/* PINDIVIDUAL Binary Sorted Tree and mark individuals who "have data"    */
/* based on the return value of the (*eval)() function.                   */
/*                                                                        */
void TraverseIndividualBSTMarkDataFlag(PINDIVIDUAL p,int (*HasData)(PINDIVIDUAL));

/*                                                                         */
/* TraverseIndividualBSTClearDataFlag(): Recursively visit every node in a */
/* PINDIVIDUAL Binary Sorted Tree and reset the p->hasdata flag to zero    */
/*                                                                         */
void TraverseIndividualBSTClearDataFlag(PINDIVIDUAL p);

#define TRAVERSAL_INCLUDED
