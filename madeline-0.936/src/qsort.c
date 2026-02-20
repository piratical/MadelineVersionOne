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
/* "qsort.c" is part of the Madeline software distribution.                          */
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
/* qsort.c                                                                            */
/*                                                                                    */
/* Non-recursive implementation of Hoare's Quick Sort algorithm using Insertion       */
/* Sort when the subarrays are small and using a random pivot element to avoid        */
/* worst-case behaviour.  The implementation is derived from one presented in         */
/* Numerical Recipes by Press et. al. (Cambridge University Press) which in turn      */
/* follows Knuth quite closely.                                                       */
/*                                                                                    */
/* Note:                                                                              */
/*                                                                                    */
/* Unlike ANSI C qsort which rearranges objects, QuickSortPointerVector()             */
/* rearranges pointers in a vector of pointers, which is what one really              */
/* wants to do most of the time.                                                      */
/*                                                                                    */
/* The comparison function takes two void pointers (which your comparison function    */
/* can cast to the proper type and an integer which can represent whatever you        */
/* need it to.  The int can be a flag indicating ascending vs. descending comparison, */
/* or it can be an index indicating which fields within the objects should be used    */
/* for comparison.                                                                    */
/*                                                                                    */

#include "qsort.h"

/* for error reporting:  */
#include "logs.h"

/*                        */
/* QuickSortPointerVector */
/*                        */
void QuickSortPointerVector(void **arr,int n,int idx,int(*cf)(void *,void *,int)){

	int l=0;        /* left */
	int jstack=0; 
	int j;
	int ir;
	int iq;
	int i;
	/* int istack[NSTACK+1]; */
	int *istack;
	long int fx=0L;

	void *a;

	/*                 */
	/* Allocate stack: */
	/*                 */
	istack=(int *)malloc((unsigned)(NSTACK+1)*sizeof(int));
	if(!istack){
		eprintf(FATALFLAG,"QuickSortPointerVector(): No memory for stack");
	}
        for(i=0;i<NSTACK+1;i++) istack[i]=0;
	
	/*                  */
	/* :                */
	/*                  */
	ir=n-1;    /* right */
	
	for(;;){

		if(ir-l<INSERTIONSORTCUTOFF){
			/*                            */
			/* do insertion sort instead: */
			/*                            */
			for(j=l+1;j<=ir;j++){

				a=arr[j];
				for(i=j-1;i>=0 && cf(a,arr[i],idx)>0;i--) arr[i+1]=arr[i];
				arr[i+1]=a;
			
			}
			if(jstack==0){
				free(istack);
				return;
			}
			/*                                                      */
			/* pop stack: right boundary first, then left boundary: */
			/*                                                      */
			ir=istack[jstack--];
			l =istack[jstack--];
		
		}else{
			/*                */
			/* do quick sort: */
			/*                */
			
			i=l;
			j=ir;
			/*                                              */
			/* generate random integer iq between l and ir  */
			/* to be the pivot element:                     */
			/*                                              */
			fx=(fx*FA+FC)%FM;
			iq=l+((ir-l+1)*fx)/FM;
			a=arr[iq];
			arr[iq]=arr[l];
			/*                                */
			/* here's stack-based quick sort: */
			/*                                */
			for(;;){
				
				while(j>=0 && cf(a,arr[j],idx)>0) j--;
				if(j<=i){
					arr[i]=a;
					break;
				}
				arr[i++]=arr[j];
				
				while(cf(a,arr[i],idx)<0 && i<n) i++;
				if(j<=i){
					arr[(i=j)]=a;
					break;
				}
				arr[j--]=arr[i];
			}
			if(ir-i>=i-l){
				istack[++jstack]=i+1;
				istack[++jstack]=ir;
				ir=i-1;
			}else{
				istack[++jstack]=l;
				istack[++jstack]=i-1;
				l=i+1;
			}
			if(jstack>NSTACK){
				eprintf(FATALFLAG,"QuickSortPointerVector(): Stack overflow");
			}
		}
	}
}


/* end of qsort.c */
