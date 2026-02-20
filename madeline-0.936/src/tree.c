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
/* "tree.c" is part of the Madeline software distribution.                           */
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


/*                                                      */
/* tree.c -implements a generic red-black BST in ANSI C */
/*                                                      */
/* 19990202 ET initiated                                */
/*                                                      */
#include "tree.h"

/*                                                                            */
/* NewNode(): return a new node bearing an attached instantiated data object: */
/*                                                                            */
PNODE NewNode(const void *object,void *(*CopyObject)(const void *)){

	PNODE p;
	
	/* int i; */
	
	p=(PNODE)malloc((unsigned) sizeof(NODE));
	if(p==NULL) TreeErrorReport("tree.c: NewNode(): out of memory");
	/*                       */
	/* Instantiate new node: */
	/*                       */
	p->l=p->r=NULL;
	p->n=1; /* key repeat count                                    */
	p->rank=0; /* node rank (calculated after tree has been built) */
	
	/*                              */
	/* attach a copy of the object: */
	/*                              */
	p->o=CopyObject(object);	
	/*           */
	/* return p: */
	/*           */
	return p;
}


/*                                  */
/* AddNode: Add a node to the tree: */
/*                                  */
PNODE AddNode(PNODE pn,const void *object,int (*Compare)(const void *,const void *),void *(*CopyObject)(const void *),int sw){

	int cmp;
	
	/*                  */
	/* Insert new node? */
	/*                  */
	if(pn==NULL){
		/*          */
		/* new node */
		/*          */
		pn=NewNode(object,CopyObject);
		/*                                                         */
		/* Every new node is made red: Then, checks below are used */
		/* to fix up colors:                                       */
		/*                                                         */
		pn->red=1;
		
		return pn;      
	}
	/*                                                             */
	/* If two children are both red, make this node                */
	/* red and the children black.  Since this BST implementation  */
	/* does not use a sentinel node, we have to check explicitely  */
	/* against NULL nodes first, then check for the red condition: */
	/*                                                             */
	if(pn->l && pn->l->red && pn->r && pn->r->red){
		pn->red   =1;
		pn->l->red=0;
		pn->r->red=0;    
	}
	/*                                  */
	/* now check where new object goes: */
	/*                                  */
	cmp=Compare(object,pn->o);
	if(cmp==0){
		/*                                                         */
		/* repeat: data in object identical, so just add to count: */
		/*                                                         */
		pn->n++;

	}else if(cmp<0){
		/*                                           */
		/* into left node (with switch set to zero): */
		/*                                           */
		pn->l=AddNode(pn->l,object,Compare,CopyObject,0);
		/*                                        */
		/* The following implements the rotation  */
		/* scheme for a red-black tree:           */
		/*                                        */
		if(pn->red && pn->l && pn->l->red && sw){
			pn=NodeRightRotation(pn);
		}
		if(pn->l && pn->l->red && pn->l->l && pn->l->l->red){
			pn=NodeRightRotation(pn);
			pn->red   =0;
			pn->r->red=1;           
		}
	}else{
		/*                                           */
		/* into right node (with switch set to one): */
		/*                                           */
		pn->r=AddNode(pn->r,object,Compare,CopyObject,1);
		/*                                        */
		/* The following implements the rotation  */
		/* scheme for a red-black tree:           */
		/*                                        */
		if(pn->red && pn->r && pn->r->red && !sw){
			pn=NodeLeftRotation(pn);
		}
		if(pn->r && pn->r->red && pn->r->r && pn->r->r->red){
			pn=NodeLeftRotation(pn);
			pn->red   =0;
			pn->l->red=1;
		}
		
	}
	return pn;
}


/*                                                  */
/* HeadNode():                                      */
/*                                                  */
/* Note that first node is a black node: pn->red=0; */
/*                                                  */
PNODE HeadNode(const void *object,void *(*CopyObject)(const void *)){

	PNODE pn;
	
	/* int cmp; */
	
	/*          */
	/* new node */
	/*          */
	pn=NewNode(object,CopyObject);
	/*                                */
	/* The head node is a black node: */
	/*                                */
	pn->red=0;
		
	return pn;      
}




/*                                                    */
/* NodeRightRotation()                                */
/*                                                    */
/* Implements right rotation for a binary search tree */
/*                                                    */
/*                                                    */
PNODE NodeRightRotation(PNODE h){

	PNODE x;

	x   =h->l;
	h->l=x->r;
	x->r=h;
	return x;
}

/*                                                   */
/* NodeLeftRotation()                                */
/*                                                   */
/* Implements left rotation for a binary search tree */
/*                                                   */
/*                                                   */
PNODE NodeLeftRotation(PNODE h){

	PNODE x;
	
	x   =h->r;
	h->r=x->l;
	x->l=h;
	return x;
}


/*                                                   */
/*                                                   */
/* FreeTree(): frees storage of a binary search tree */
/*                                                   */
/*                                                   */
void FreeTree(PNODE pn,void (*FreeObject)(void *)){


	if(!pn) return;
	if(pn->l) FreeTree(pn->l,FreeObject);
	if(pn->r) FreeTree(pn->r,FreeObject);
	
	/*                  */
	/* Free the object: */
	/*                  */
	FreeObject(pn->o);

	/*                                */
	/* Finally, free the node itself: */
	/*                                */
	free(pn);
}




/*                                                          */
/* TraverseTree():                                          */
/*                                                          */
/* Traverses a binary search tree, applying a user-supplied */
/* function, ActOnObject, to process each object            */
/*                                                          */
void TraverseTree(PNODE pn,void (*ActOnObject)(void *object)){

	if(pn){
		if(pn->l) TraverseTree(pn->l,ActOnObject);
		ActOnObject(pn->o);
		if(pn->r) TraverseTree(pn->r,ActOnObject);
	}
}


/*                                                                   */
/* SetNodeRanks()                                                    */
/*                                                                   */
/* Traverses a completed tree, setting the ordered rank of each node */
/* and returning the total number of nodes in the tree:              */
/*                                                                   */
/* ->The very first call should have r=0                             */
/*                                                                   */
int SetNodeRanks(PNODE pn,int rank){

	if(pn){
		if(pn->l) rank=SetNodeRanks(pn->l,rank);
		/*                                                   */
		/* increment r and set the rank of the current node: */
		/*                                                   */
		rank++;
		pn->rank=rank;
		if(pn->r) rank=SetNodeRanks(pn->r,rank);
	}
	return rank;
}


/*                                                        */
/* SeekNode(): Find the node bearing the matching object: */
/*                                                        */
PNODE SeekNode(PNODE pn,const void *object,int (*Compare)(const void *,const void *)){

	int cmp;
	
	if(!pn)
		/*                                    */
		/* not found: return the NULL pointer */
		/*                                    */
		return pn;
	else if(!(cmp=Compare(pn->o,object)))
		/*        */
		/* found: */
		/*        */
		return pn;
	else if(cmp<0)
		/*                     */
		/* seek in right node: */
		/*                     */
		return SeekNode(pn->r,object,Compare);
	else
		/*                    */
		/* seek in left node: */
		/*                    */
		return SeekNode(pn->l,object,Compare);
}


/*                                           */
/* TreeErrorReport: error reporting function */
/*                                           */
void TreeErrorReport(char *s,...){

	va_list argptr;
	
	fprintf(stderr,"Tree processor report:\n");
	va_start(argptr,s);
	vfprintf(stderr,s,argptr);
	va_end(argptr);
	fprintf(stderr,"\n");
	exit(1);

};

/* end of tree.c */
