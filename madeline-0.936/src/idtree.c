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
/* "idtree.c" is part of the Madeline software distribution.                         */
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


/*                       */
/* idtree.c              */
/*                       */
/*                       */
/* 19990203 ET initiated */
/*                       */
#include "idtree.h"


/*                  */
/* CompareIDObjects */
/*                  */
int CompareIDObjects(const void *a,const void *b){

	return strcmp( ((PIDOBJECT)a)->id,((PIDOBJECT)b)->id);

}


/*              */
/* FreeIDObject */
/*              */
void FreeIDObject(void *idobject){

	free(idobject);

}


/*              */
/* CopyIDObject */
/*              */
void *CopyIDObject(const void *object){

	PIDOBJECT p;
	
	p=(PIDOBJECT)malloc(sizeof(IDOBJECT));
	if(!p) TreeErrorReport("CopyIDObject(): Out of memory\n");
	/*                */
	/* copy elements: */
	/*                */
	strcpy(p->id,((PIDOBJECT)object)->id);
	p->female=((PIDOBJECT)object)->female;
	
	return (void *)p;

}


/*                                                       */
/* CheckIDTreeStructure: this is only used for debugging */
/*                                                       */
void CheckIDTreeStructure(PNODE pn){

	PIDOBJECT n;
	PIDOBJECT l;
	PIDOBJECT r;

	IDOBJECT z,*zz=&z;
	
	/*                 */
	/* return if null: */
	/*                 */
	if(!pn) return;
	
	/*                             */
	/* sentinal for null branches: */
	/*                             */
	zz->id[0]='.';
	zz->id[1]='\0';
	
	n=(PIDOBJECT)pn->o;

	if(pn->l) l=(PIDOBJECT)pn->l->o;
	else      l=zz;

	if(pn->r) r=(PIDOBJECT)pn->r->o;
	else      r=zz;

	printf("NODE=%s LEFT=%s RIGHT=%s\n",
		n->id,
		l->id,
		r->id	
	);
	
	CheckIDTreeStructure(pn->l);
	CheckIDTreeStructure(pn->r);
}

/*                                        */
/* int IDTreeHasRepeats()                 */
/*                                        */
/* - returns 1 if repeats of an ID occur: */
/*                                        */
int IDTreeHasRepeats(PNODE pn){

	if(!pn) return 0;

	if(IDTreeHasRepeats(pn->l)) return 1;

	if(pn->n>1) return 1;
	
	return IDTreeHasRepeats(pn->r);
}


