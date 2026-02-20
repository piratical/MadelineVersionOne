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
/* "qsort.h" is part of the Madeline software distribution.                          */
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

/*                                                                                    */
/* qsort.h                                                                            */
/*                                                                                    */
/* Non-recursive implementation of Hoare's Quick Sort algorithm using Insertion       */
/* Sort when the subarrays are small and using a random pivot element to avoid        */
/* worst-case behaviour.  The implementation is derived from one presented in         */
/* Numerical Recipes by Press et. al. (Cambridge University Press) which in turn      */
/* follows Knuth quite closely.                                                       */
/*                                                                                    */
/* Note:                                                                              */
/*                                                                                    */
/* QuickSortPointerVector() rearranges pointers in a vector of pointers, which is     */
/* what we really want to do most of the time, and certainly so in Madeline.          */
/*                                                                                    */
/* The comparison function takes two void pointers (which your comparison function    */
/* can cast to the proper type and an integer which can represent whatever you        */
/* need it to.  The int can be a flag indicating ascending vs. descending comparison, */
/* or it can be an index indicating which fields within the objects should be used    */
/* for comparison.                                                                    */
/*                                                                                    */
/*                                                                                    */

#include <stdlib.h>
#include <stdio.h>


#define INSERTIONSORTCUTOFF 7
/*                                     */
/* stack used for subarray boundaries: */
/*                                     */
#define NSTACK 50
/*                                            */
/* defines used for random number generation: */
/*                                            */
#define FM 7875
#define FA 211
#define FC 1663
/*                        */
/* FUNCTION DECLARATIONS: */
/*                        */
void QuickSortPointerVector(void **arr,int n,int idx,int(*cf)(void *a,void *b,int));


