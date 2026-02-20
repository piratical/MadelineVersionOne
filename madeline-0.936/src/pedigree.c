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
/* "pedigree.c" is part of the Madeline software distribution.                       */
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
/* General Public License for more details.                                          */
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
/* pedigree.c                                           */
/*                                                      */
/* Function library to model family pedigree structures */
/* for use in data management and analysis programs.    */
/*                                                      */
/* In this library, I have attempted to model           */
/* family pedigree structures based closely on the way  */
/* people think about them, which has hopefully         */
/* resulted in a clear and easy to understand code      */
/*                                                      */
/* initiated by ET 970110                               */
/*                                                      */
#ifndef PEDIGREE_INCLUDED
#include "pedigree.h"
#endif

/*                                    */
/* logs.h: for lprintf, eprintf, etc: */
/*                                    */
#include "logs.h"

#ifndef VAR_AND_ARR_INCLUDED
#include "variablesandarrays.h"
#endif

/*                  */
/* Global Variables */
/*                  */

/*                                         */
/* PDErrorReport: error reporting function */
/*                                         */
void PDErrorReport(char *s,...){

	va_list argptr;
	
	fprintf(stderr,"Pedigree processor report:\n");
	va_start(argptr,s);
	vfprintf(stderr,s,argptr);
	va_end(argptr);
	fprintf(stderr,"\n");
	exit(1);

};


/*                                                   */
/* NewPedigree(): return a vector of n new PEDIGREEs */
/*                                                   */
PPEDIGREE NewPedigree(void){

	PPEDIGREE p;
	
	p=(PPEDIGREE)malloc((unsigned)sizeof(PEDIGREE));
	if(p==NULL) eprintf(FATALFLAG,"NewPedigree(): out of memory.\n");
	
	/* initialize members */
	p->fid[0]=CHARMISSINGVALUE;
	p->fid[1]='\0';
	p->n   =0;    /* number of people                                            */
	p->ex  =0;    /* number currently excluded                                   */
	p->nun =0;    /* number of unconnected people in pedigree                    */
	p->ncsp=0;    /* number of childless spouses                                 */
	p->nvi =0;    /* number of virtual (dummied-in) individuals                  */
	p->red=0;     /* for red/black balanced BST implementation: 0=black          */
	p->nff=0;     /* number of founding fathers                                  */
	p->nfm=0;     /* number of founding mothers                                  */
	p->nfg=0;     /* number of founding groups                                   */
	p->ff=NULL;   /* vector of founding fathers: allocate to size needed         */
	p->fm=NULL;   /* vector of founding mothers: allocate to size needed         */
	p->fg=NULL;   /* vector of founding groups (FOUNDINGGROUP): allocate to size */
	p->l=NULL;    /* left node                                                   */
	p->r=NULL;    /* right node                                                  */
	p->fl=0;      /* generic flag for state information                          */
	p->sid=0;     /* sequential id                                               */
	p->exclude=0; /* exclude flag                                                */
	p->h=NULL;    /* NULL BST root node for database individuals                 */
	p->a=NULL;    /* NULL BST root node for add-on (virtual) individuals         */
	p->nfhead=NULL; /* Head node of singly-linked list of NUCLEARFAMILYs         */
	p->nftail=NULL; /* Tail node of singly-linked list of NUCLEARFAMILYs         */ 
	
	p->NumberOfFounders=0;     /* Founders     */
	p->NumberOfNonFounders=0;  /* Non-founders */
	
	p->NumberOfAffected=0;
	/*        */
	/* return */
	/*        */
	return p;
}


/*                                        */
/* NewIndividual: return a new INDIVIDUAL */
/*                                        */
PINDIVIDUAL NewIndividual(void){

	PINDIVIDUAL p;
	int j;
		
	p=(PINDIVIDUAL)malloc((unsigned)sizeof(INDIVIDUAL));
	if(p==NULL) eprintf(FATALFLAG,"NewIndividual(): out of memory!\n");
	
	/* initialize members                                                                     */
	p->id[0]=CHARMISSINGVALUE; /* individual's ID                                             */
	p->id[1]='\0';            /* individual's ID terminated                                   */
	p->sid=0;               /* sequential ID no.                                              */
	p->female=MISSING;      /* female                                                         */
	p->dead  =MISSING;      /* dead=1, alive=0                                                */
	p->affected=MISSING;    /* affection status unknown                                       */
	p->twin='\0';           /* twin status                                                    */
	p->proband=FALSE;       /* proband status                                                 */
	p->clas=UNCLASSIFIED;   /* individual's class (FOUNDER, SIBLING, ETC.)                    */
	p->lclass=MISSING;      /* Liability class unknown                                        */         
	p->fid[0]='\0';         /* father's id                                                    */
	p->f=NULL;              /* father                                                         */
	p->mid[0]='\0';         /* mother's id                                                    */
	p->m=NULL;              /* mother                                                         */

	p->l=NULL;              /* left and ...                     */
	p->r=NULL;              /* ... right pointers for bst       */
	p->red=0;               /* for red-black BST implementation */

	p->ma=DEFAULTMATECOUNT; /* maximum no. of mates in current allocation */
	p->na=0;                /* current no. of mates                       */
	p->a=(PPINDIVIDUAL)malloc((unsigned)DEFAULTMATECOUNT*sizeof(PINDIVIDUAL));
	if(p->a==NULL) eprintf(FATALFLAG,"NewIndividual(): out of memory allocating vector a\n");
	p->a[0]=NULL;
	p->mk=DEFAULTOFFSPRINGCOUNT; /* max. no. of offspring in current allocation */
	p->nk=0;                     /* current number of offspring                 */
	p->k=(PPINDIVIDUAL)malloc((unsigned)DEFAULTOFFSPRINGCOUNT*sizeof(PINDIVIDUAL));
	if(p->k==NULL) eprintf(FATALFLAG,"NewIndividual(): out of memory allocating vector k\n");
	for(j=0;j<DEFAULTOFFSPRINGCOUNT;j++)
		p->k[j]=NULL;                 
	p->db=NULL;             /* database containing the record of this person                                */
	p->rci=0;               /* record index of this person in the database -- unsigned int can't be MISSING */
	
	p->fl     =0;           /* no flags set                                      */
	p->included=0;          /* included set to zero by default                   */
	//p->typed  =0;           /* untyped by default                                */
	p->genotypecount=0;    /* holds count of markers typed for this individual  */
	p->nw     =0;           /* width of tree below this indiv.: used for drawing */
	p->vlevel =0;           /* vertical "level" of individual in a drawing       */
	p->x      =0.0;         /* x coordinate of an individual in a drawing        */
	p->y      =0.0;         /* y coordinate of an individual in a drawing        */
	p->excluded=0;          /* exclude flag: defaults to zero                    */
	p->counted=0;           /* assume not yet counted                            */

	p->pd=NULL;             /* NULL pedigree pointer */
	
	p->dob=MISSINGDATE;     /* date of birth, if known */
	p->dod=MISSINGDATE;     /* date of death, if known */
	
	p->unconnected=0;       /* unconnected=0 by default, 1 if unconnected */
	p->csp=0;               /* childless spouse=0 by default, 1 if so...  */
	p->hasdata=0;           /* person assumed to have no data by default  */
	p->haspdata=0;          /* no phenotype data by default               */
	p->hasgdata=0;          /* no genotype data by default                */

	p->consanguinity=0;       /* 0: no consanguinity: 1: consanguinity w/ some spouse */
	p->drawn=0;               /* not drawn=0: 1 if already drawn.                     */
	p->page=0;                /* non-zero indicates page on which drawn.              */
	p->foundinggroup=MISSING; /* The founding group to which the individual belongs   */  

	p->pnf=NULL;              /* Pointer to the nuclear family formed by this person's parents           */
	p->snf=NULL;              /* Pointer to the nuclear family formed by this person as a parent himself */

	p->fieldflag=NULL;        /* Pointer to field flag is NULL when there are no exclusions */
	p->integrity=NULL;
	p->visited=0;
	p->IsPrimaryFounder=0;    /* Defaults to zero (false): determined by DetermineFounders() */
	return p;
}


/*                                                                */
/* NewNuclearFamily(): Returns a new NUCLEARFAMILY.               */
/*                                                                */
/* pf: Pointer to the father -- can't be NULL                     */
/* pm: Pointer to the mother -- can't be NULL                     */
/* ng: number of genotypes (needed to allocate integrity flag vec.*/
/*                                                                */
PNUCLEARFAMILY NewNuclearFamily(PINDIVIDUAL pf,PINDIVIDUAL pm,int ng){

	PNUCLEARFAMILY p;
	int i;
	
	/* Allocate NUCLEARFAMILY */
	p=(PNUCLEARFAMILY)malloc((unsigned)sizeof(NUCLEARFAMILY));
	if(p==NULL) eprintf(FATALFLAG,"NewNuclearFamily(): out of memory!\n");
	
	/* Next Pointer: */
	p->next=NULL;
	
	/* Allocate Integrity Vector and set to zero: */
	p->ifv=(char *)malloc((unsigned)ng*sizeof(char));
	if(p->ifv==NULL) eprintf(FATALFLAG,"NewNuclearFamily(): out of memory allocating integrity flag vector!\n");
	for(i=0;i<ng;i++) p->ifv[i]=0;
	
	return p;
}


/*                   */
/* FreeNuclearFamily */
/*                   */
void FreeNuclearFamily(PNUCLEARFAMILY p){

	if(p->ifv) free(p->ifv);
	free(p);

}


/*                             */
/* FreeNuclearFamilyLinkedList */
/*                             */
void FreeNuclearFamilyLinkedList(PNUCLEARFAMILY head){

	PNUCLEARFAMILY p;
	PNUCLEARFAMILY next;
	
	for(p=head;p;p=next){
		/* Save the next pointer: */
		next=p->next;
		/* Delete the current node: */
		FreeNuclearFamily(p);
	}
}


/*                                                               */
/* IndividualLookup(): find an individual in an PINDIVIDUAL bst: */
/* h: root node of bst                                           */
/* id: string ID to find                                         */
/*                                                               */
PINDIVIDUAL IndividualLookup(PINDIVIDUAL h,const char *id){

	PINDIVIDUAL p;
	int cf;
	
	/*                                            */
	/* Go through the bst looking for the id key: */
	/*		                                          */
	for(p=h;p && (cf=strcmp(id,p->id))!=0;p=cf<0?p->l:p->r);
	/*                              */
	/* p will be NULL if not found: */
	/*                              */
	return p;
}


/*                                                  */
/* IndividualTreeHeadNode():                        */
/*                                                  */
/* Note that first node is a black node: pn->red=0; */
/*                                                  */
PINDIVIDUAL IndividualTreeHeadNode(const char *id,PDBF db,UINT32 rci){

	PINDIVIDUAL p;

	/*                                       */
	/* NewIndividual() returns a BLACK node: */
	/*                                       */
	p=NewIndividual();
	/*                                */
	/* insert only core data for now: */
	/*                                */
	p->db=db;
	p->rci=rci;
	strcpy(p->id,id);
	/*                                        */
	/* this is now the head of the root node: */
	/*                                        */
	return p;
}


/*                           */
/* IndividualRightRotation() */
/*                           */
PINDIVIDUAL IndividualRightRotation(PINDIVIDUAL h){
	
	PINDIVIDUAL x;
	x   =h->l;
	h->l=x->r;
	x->r=h;
	return x;
	
}


/*                          */
/* IndividualLeftRotation() */
/*                          */
PINDIVIDUAL IndividualLeftRotation(PINDIVIDUAL h){
	
	PINDIVIDUAL x;
	x   =h->r;
	h->r=x->l;
	x->l=h;
	return x;

}


/*                                                          */
/* IndividualTreeAddNode: Add a node to an INDIVIDUAL tree: */
/*                                                          */
PINDIVIDUAL IndividualTreeAddNode(PINDIVIDUAL pn,PPINDIVIDUAL ppi,const char *id,PDBF db,UINT32 rci,int sw){

	int cmp;
	
	/*                  */
	/* Insert new node? */
	/*                  */
	if(pn==NULL){

		/*                                       */
		/* NewIndividual() returns a BLACK node: */
		/*                                       */
		pn=NewIndividual();
		/*                                */
		/* insert only core data for now: */
		/*                                */
		pn->db=db;
		pn->rci=rci;
		strcpy(pn->id,id);
		/*                                                         */
		/* Every new node is made red: Then, checks below are used */
		/* to fix up colors:                                       */
		/*                                                         */
		pn->red=1;
		/*                        */
		/* the individual's node: */
		/*                        */
		*ppi=pn;

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
	cmp=strcmp(id,pn->id);
	if(cmp==0){
		/*                                                         */
		/* repeat: data in object identical, so just add to count: */
		/*                                                         */
		/* pn->n++;                                                */

	}else if(cmp<0){
		/*                                                   */
		/* into left node (with switch set to zero):         */
		/*                                                   */
		/* pn->l=AddNode(pn->l,object,Compare,CopyObject,0); */
		/*                                                   */
		pn->l=IndividualTreeAddNode(pn->l,ppi,id,db,rci,0);
		/*                                        */
		/* The following implements the rotation  */
		/* scheme for a red-black tree:           */
		/*                                        */
		if(pn->red && pn->l && pn->l->red && sw){
			pn=IndividualRightRotation(pn);
		}
		if(pn->l && pn->l->red && pn->l->l && pn->l->l->red){
			pn=IndividualRightRotation(pn);
			pn->red   =0;
			pn->r->red=1;           
		}
	}else{
		/*                                                   */
		/* into right node (with switch set to one):         */
		/*                                                   */
		/* pn->r=AddNode(pn->r,object,Compare,CopyObject,1); */
		/*                                                   */
		pn->r=IndividualTreeAddNode(pn->r,ppi,id,db,rci,1);
		
		/*                                        */
		/* The following implements the rotation  */
		/* scheme for a red-black tree:           */
		/*                                        */
		if(pn->red && pn->r && pn->r->red && !sw){
			pn=IndividualLeftRotation(pn);
		}
		if(pn->r && pn->r->red && pn->r->r && pn->r->r->red){
			pn=IndividualLeftRotation(pn);
			pn->red   =0;
			pn->l->red=1;
		}
		
	}
	return pn;
}



/*                                                                                              */
/*                                                                                              */
/* InstallIndividual: install an INDIVIDUAL into a PINDIVIDUAL bst: data are read from database */
/*                                                                                              */
/* InstallIndividual() builds a balanced red-black tree and is therefore                        */
/* used to install individuals in the database and their parents into                           */
/* INDIVIDUAL trees almost all of the time (see UnoptomizedInstallIndividual() below            */
/* for exceptions).                                                                             */
/*                                                                                              */
/* pd: pointer to PEDIGREE structure (contains head node for INDIVIDUAL tree)                   */
/* db: database containing the individual's data                                                */
/* rci: record index of individual in database                                                  */
/*                                                                                              */
/* returns: pointer to most recently installed individual                                       */
/*                                                                                              */
PINDIVIDUAL InstallIndividual(PPEDIGREE pd,const char *id,PDBF db,UINT32 rci){
	
	PINDIVIDUAL p;
	
	/*                                */
	/* Red-black tree implementation: */
	/*                                */
	if(!pd->h) p=pd->h=IndividualTreeHeadNode(id,db,rci);
	else{
		/*                                                      */
		/* Check that individual does not already occur:        */
		/* -- oooh! Awful things happen if this is not checked! */
		/*                                                      */
		p=IndividualLookup(pd->h,id);
		if(p) eprintf(WARNINGFLAG,
			"In pedigree %s, individual %s occurs more than once in data set.\n",
			pd->fid,
			id
		);
		else  pd->h=IndividualTreeAddNode(pd->h,&p,id,db,rci,1);
	}
	/*                                        */
	/* Set the individual's pointer to point  */
	/* back to his or her own pedigree:       */
	/*                                        */
	p->pd=pd;
	/*                                        */
	/* return node of newly added individual: */
	/*                                        */
	return p;
}


/*                                                                                       */
/* UnoptomizedInstallIndividual(): Non-recursive naive bst installation                  */
/* procedure.  Normally, InstallIndividual() (above) is used because it produces         */
/* balanced trees using the red-black tree algorithm.                                    */
/*                                                                                       */
/* However, there are a few cases where it is convenient to be able to install           */
/* a virtual parent individual while right in the middle of traversing the tree into     */
/* which that virtual parent must be installed.  Because of the node rotations that are  */
/* likely to occur in the red-black tree installation, using InstallIndividual() could   */
/* very well mean that the traversal already in-progress would not, in the end, visit    */
/* all nodes.  In this case, using UnoptomizedInstallIndividual(), which does no node    */
/* rotations, is basically harmless.  It might make the already balanced tree a little   */
/* bit unbalanced, but not by much if the number of virtual parents that need to be      */
/* added at the tail end in this way are few (which should be the case).                 */
/*                                                                                       */
/* The specific and ONLY occasion when UnoptomizedInstallIndividual() is used is when    */
/* a single parent is listed for a group of sibs.  Madeline is forced to generate a      */
/* random parental ID for the missing parent.  It is very convenient to install this     */
/* parent while making the connections between individuals so that all sibs in a sibship */
/* can be assigned the same missing parent (that is, Madeline assumes full sibships      */
/* rather than halfsibships, which is a reasonable assumption, although clearly an       */
/* assumption nonetheless).                                                              */
/*                                                                                       */
PINDIVIDUAL UnoptomizedInstallIndividual(PPEDIGREE pd,const char *id,PDBF db,UINT32 rci){

	PINDIVIDUAL p,q; /* linked list node pointer */
	int cf;
	
	if(!pd->h){
		p=NewIndividual();
		/*                                */
		/* insert only core data for now: */
		/*                                */
		p->db=db;
		p->rci=rci;
		strcpy(p->id,id);
		/*                                        */
		/* Set the individual's pointer to point  */
		/* back to his or her own pedigree:       */
		/*                                        */
		p->pd=pd;
		/*                            */
		/* this is now the root node: */
		/*                            */
		pd->h=p;
		/*                                         */
		/* return pointer to installed individual: */
		/*                                         */
		return p;
	}
	/*                       */
	/* search bst for entry: */
	/*                       */
	for(p=pd->h;p && (cf=strcmp(id,p->id))!=0;q=p,p=cf<0?p->l:p->r);
	/*                        */
	/* if p, its a duplicate: */
	/*                        */
	if(p){
		if(p) eprintf(WARNINGFLAG,
			"In pedigree %s, individual %s occurs more than once in data set.\n",
			pd->fid,
			id
		);
	}else{
		p=NewIndividual();
		/*                                */
		/* insert only core data for now: */
		/*                                */
		p->db=db;
		p->rci=rci;
		strcpy(p->id,id);
		/*                                        */
		/* Set the individual's pointer to point  */
		/* back to his or her own pedigree:       */
		/*                                        */
		p->pd=pd;
		/*                                    */
		/* plug p into bst without balancing  */
		/* the tree so that we can avoid node */
		/* rotations:                         */
		/*                                    */
		if(cf<0)
			q->l=p;
		else
			q->r=p;
	}
	/*                                        */
	/* return node of newly added individual: */
	/*                                        */
	return p;
}

		

/*                                     */
/* Warning(): prints a warning message */
/*                                     */
void Warning(char *s,...){

	va_list argptr;
	
	fprintf(stderr,"Warning:\n");
	va_start(argptr,s);
	vfprintf(stderr,s,argptr);
	va_end(argptr);
	printf("\n");
};


/*                                               */
/* PedigreeTreePrint(): Used for debugging only: */
/*                                               */
void PedigreeTreePrint(PPEDIGREE pd){

	/*                 */
	/* return if NULL: */
	/*                 */
	if(!pd) return;
	
	
	/*/////////////////// */
	/*                    */
	/* Process this node: */
	/*                    */
	/*/////////////////// */
	
	lprintf("%c %s: L=%s R=%s\n",
		pd->red?'R':'B',
		pd->fid,
		pd->l?pd->l->fid:"----",
		pd->r?pd->r->fid:"----"
	);


	/*                    */
	/* Process left node: */
	/*                    */
	if(pd->l) PedigreeTreePrint(pd->l);
	/*                     */
	/* Process right node: */
	/*                     */
	if(pd->r) PedigreeTreePrint(pd->r);	
}



/*                                                 */
/* IndividualTreePrint(): Used for debugging only: */
/*                                                 */
void IndividualTreePrint(PINDIVIDUAL p){

	/*                 */
	/* return if NULL: */
	/*                 */
	if(!p) return;
	
	
	/*/////////////////// */
	/*                    */
	/* Process this node: */
	/*                    */
	/*/////////////////// */
	
	lprintf("%c %s: L=%s R=%s V=%i NW=%i\n",
		p->red?'R':'B',
		p->id,
		p->l?p->l->id:"--------",
		p->r?p->r->id:"--------",
		p->vlevel,
		p->nw
	);


	/*                    */
	/* Process left node: */
	/*                    */
	if(p->l) IndividualTreePrint(p->l);
	/*                     */
	/* Process right node: */
	/*                     */
	if(p->r) IndividualTreePrint(p->r);	
}


/*                                                                            */
/* BinaryPedigreeTreeHeadNode(): The head node of a red-black tree starts out */
/* black.                                                                     */
/*                                                                            */
PPEDIGREE BinaryPedigreeTreeHeadNode(char *fid,char *id,PDBF db,UINT32 rci,PPINDIVIDUAL ppi){

	PPEDIGREE HeadNode;

	/*                                                    */
	/* New node: NewPedigree returns a BLACK node already */
	/*                                                    */
	HeadNode=NewPedigree();
	/*                                              */
	/* Add family ID key:                           */
	/*                                              */
	strcpy(HeadNode->fid,fid); /* family identifier */
	/*                                              */
	/* add the first person:                        */
	/*                                              */
	*ppi=InstallIndividual(HeadNode,id,db,rci);
	HeadNode->n++;
	
	return HeadNode;
}

/*                                                                                    */
/* AddIndividualToBinaryPedigreeTree(): Add individuals to a binary tree of pedigrees */
/*                                                                                    */
/* This function creates a binary tree of PEDIGREEs based on family ID and            */
/* adds new individuals to a linked list of family members                            */
/* (relationships within the family are not yet known at this point however).         */
/*                                                                                    */
/* The pointer to pointer to individual is used to retrieve the address of the newest */
/* individual so that additional information can be added for that individual after   */
/* they have been inserted into the tree and linked list                              */
/*                                                                                    */
PPEDIGREE AddIndividualToBinaryPedigreeTree(PPEDIGREE pd,const char *fid,const char *id,PDBF db,UINT32 rci,PPINDIVIDUAL ppi,int sw){

	int cmp;

	/*                  */
	/* Insert new node? */
	/*                  */
	if(pd==NULL){
		/*                            */
		/* start a new pedigree tree: */
		/*                            */
		pd=NewPedigree();
		strcpy(pd->fid,fid); /* family identifier */
		/*                                        */
		/* add the first person:                  */
		/*                                        */
		*ppi=InstallIndividual(pd,id,db,rci);
		pd->n++;
		/*                             */
		/* Every new node is made red: */
		/*                             */
		pd->red=1;
		
		return pd;
	}
	/*                                                             */
	/* If two children are both red, make this node                */
	/* red and the children black.  Since this BST implementation  */
	/* does not use a sentinel node, we have to check explicitely  */
	/* against NULL nodes first, then check for the red condition: */
	/*                                                             */
	if(pd->l && pd->r && pd->l->red && pd->r->red){
		pd->red   =1;
		pd->l->red=0;
		pd->r->red=0;    
	}
	/*                                      */
	/* now check where new individual goes: */
	/*                                      */
	if((cmp=strcmp(fid,pd->fid))==0){
		/*                                           */
		/* put person into linked list at this node: */
		/*                                           */
		*ppi=InstallIndividual(pd,id,db,rci);
		pd->n++;
	}else if(cmp<0){
		/*                                           */
		/* into left node (with switch set to zero): */
		/*                                           */
		pd->l=AddIndividualToBinaryPedigreeTree(pd->l,fid,id,db,rci,ppi,0);
		/*                                        */
		/* The following implements the rotation  */
		/* scheme for a red-black tree:           */
		/*                                        */
		if(pd->l && pd->red && pd->l->red && sw){
			pd=PedigreeRightRotation(pd);
		}
		if(pd->l && pd->l->l && pd->l->red && pd->l->l->red){
			pd=PedigreeRightRotation(pd);
			pd->red   =0;
			pd->r->red=1;           
		}
	}else{
		/*                                           */
		/* into right node (with switch set to one): */
		/*                                           */
		pd->r=AddIndividualToBinaryPedigreeTree(pd->r,fid,id,db,rci,ppi,1);
		/*                                        */
		/* The following implements the rotation  */
		/* scheme for a red-black tree:           */
		/*                                        */
		if(pd->r && pd->red && pd->r->red && !sw){
			pd=PedigreeLeftRotation(pd);
		}
		if(pd->r && pd->r->r && pd->r->red && pd->r->r->red){
			pd=PedigreeLeftRotation(pd);
			pd->red   =0;
			pd->l->red=1;
		}
	}
	return pd;
}

/*                                                             */
/* PedigreeRightRotation()                                     */
/*                                                             */
/* Implements right rotation for a binary pedigree search tree */
/*                                                             */
/*                                                             */
PPEDIGREE PedigreeRightRotation(PPEDIGREE h){

	PPEDIGREE x;

	x   =h->l;
	h->l=x->r;
	x->r=h;
	return x;
}

/*                                                            */
/* PedigreeLeftRotation()                                     */
/*                                                            */
/* Implements left rotation for a binary pedigree search tree */
/*                                                            */
/*                                                            */
PPEDIGREE PedigreeLeftRotation(PPEDIGREE h){

	PPEDIGREE x;
	
	x   =h->r;
	h->r=x->l;
	x->l=h;
	return x;
}


/*                                                          */
/* FreeIndividualBST(): Recursively frees a PINDIVDUAL bst: */
/*                                                          */
void FreeIndividualBST(PINDIVIDUAL p){

	/*                             */
	/* Nothing to do if NULL node: */
	/*                             */
	if(!p) return;
	
	/*                         */
	/* Recurse left and right: */
	/*                         */
	if(p->l) FreeIndividualBST(p->l);
	if(p->r) FreeIndividualBST(p->r);

	/*                  */
	/* Now do this guy: */
	/*                  */
	FreeIndividual(p);
}


/*                */
/* FreeIndividual */
/*                */
void FreeIndividual(PINDIVIDUAL p){

	/* Free spouse and kid vectors, if present: */
	if(p->a) free(p->a);
	if(p->k) free(p->k);
	
	/* Free the fieldflag vector, if present: */
	if(p->fieldflag) free(p->fieldflag);
	/* Free the integrity vector, if present: */
	if(p->integrity) free(p->integrity);
	
	/* Free the individual himself:           */
	/*                                        */
	/* Note that the NUCLEARFAMILY pointers   */
	/* Are freed by calling a function at the */
	/* level of the pedigree, not here        */
	free(p);

}


/*                                                                   */
/*                                                                   */
/* FreeBinaryPedigreeTree(): frees storage of a binary pedigree tree */
/*                                                                   */
/*                                                                   */
void FreeBinaryPedigreeTree(PPEDIGREE pd){


	if(!pd) return;
	if(pd->l) FreeBinaryPedigreeTree(pd->l);
	if(pd->r) FreeBinaryPedigreeTree(pd->r);
	
	/*                       */
	/* Free the individuals: */
	/*                       */
	FreeIndividualBST(pd->h);

	/*                                              */
	/* Free the founding father and mother vectors: */
	/*                                              */
	if(pd->nff) free(pd->ff);
	if(pd->nfm) free(pd->fm);
	/*                                 */
	/* Free the founding group vector: */
	/*                                 */
	if(pd->nfg) free(pd->fg);
	/*                                           */
	/* Finally, free the pedigree object itself: */
	/*                                           */
	free(pd);
}


/*                                                            */
/* TraverseBinaryPedigreeTree():                              */
/*                                                            */
/* Traverses a binary PEDIGREE tree, applying a user-supplied */
/* function, func, to each node                               */
/*                                                            */
void TraverseBinaryPedigreeTree(PPEDIGREE pd,void (*func)(PPEDIGREE)){

	if(pd){
		if(pd->l) TraverseBinaryPedigreeTree(pd->l,func);
		(*func)(pd);
		if(pd->r) TraverseBinaryPedigreeTree(pd->r,func);
	}
}



/*                                                              */
/* SeekPedigree(): Find the Pedigree having the ID passed in s: */
/*                                                              */
PPEDIGREE SeekPedigree(PPEDIGREE pd,char *fid){

	int cmp;
	
	if(!pd)
		/*                                    */
		/* not found: return the NULL pointer */
		/*                                    */
		return pd;
	else if(!(cmp=strcmp(pd->fid,fid)))
		/*        */
		/* found: */
		/*        */
		return pd;
	else if(cmp<0)
		/*                     */
		/* seek in right node: */
		/*                     */
		return SeekPedigree(pd->r,fid);
	else
		/*                    */
		/* seek in left node: */
		/*                    */
		return SeekPedigree(pd->l,fid);
}



/*                                                                   */
/* TraverseRangeInBinaryPedigreeTree():                              */
/*                                                                   */
/* Traverses only a range of pedigree IDs in a binary PEDIGREE tree, */
/* applying a user-supplied                                          */
/* function, func, to each node in the range, inclusive              */
/*                                                                   */
void TraverseRangeInBinaryPedigreeTree(PPEDIGREE pd,char *stt,char *end,void (*func)(PPEDIGREE)){

	if(pd){ 
		if(pd->l) TraverseRangeInBinaryPedigreeTree(pd->l,stt,end,func);
		/*                                             */
		/* function is only applied to those in range: */
		/*                                             */
		if(strcmp(pd->fid,stt)>=0 && strcmp(pd->fid,end)<=0){
			(*func)(pd);
		}
		if(pd->r) TraverseRangeInBinaryPedigreeTree(pd->r,stt,end,func);
	}
}





/*
 
  IsEmptyOrMissing() returns 1 if a string is all white space 
  characters or NULL string or if the string is equal to the
  CHARMISSINGSTRING "."

  NOTE BENE:
  This function precedes the IsCMV() function and eventually 
  all occurrences of IsEmptyOrMissing() should be updated with
  IsCMV() or other appropriate function.  It's OK to leave it
  this way for now:
  
*/
int IsEmptyOrMissing(char *s){

	if(s==NULL) return 1;
	if(!strcmp(s,CHARMISSINGSTRING)) return 1;
	for(;*s && (*s==' ' || *s=='\t' || *s==0x0D || *s==0x0A);s++);
	return !(*s);
}


/*                                                            */
/* TraverseIndividualBST(): Recursively visit every node in a */
/* PINDIVIDUAL Binary Sorted Tree:                            */
/*                                                            */
void TraverseIndividualBST(PINDIVIDUAL p,void (*visit)(PINDIVIDUAL)){

	if(!p) return;
	TraverseIndividualBST(p->l,visit);
	(*visit)(p);
	TraverseIndividualBST(p->r,visit);

}

/*                                        */
/* IndividualIsProband(): returns TRUE if */
/* an individual is the proband           */
/*                                        */
int IndividualIsProband(PINDIVIDUAL p){
	return p->proband;
}


/*                                                                            */
/* 980225 ET (zao)                                                            */
/*                                                                            */
/* RemoveIndividualsFromBST():                                                */
/*                                                                            */
/* Removes individuals that meet a certain criteria, (*criteria)(),           */
/* from the PINDIVIDUAL BST.                                                  */
/*                                                                            */
/* The function works recursively in order to visit all nodes in the          */
/* tree.  By visiting the top node first, and even repeatedly as successive   */
/* nodes starting from the top get deleted by RemoveIndividual(), it is still */
/* possible to then recurse left and right child nodes knowing that no nodes  */
/* will be missed or visited twice due to rearrangement                       */
/* of the tree by RemoveIndividual()                                          */
/*                                                                            */
/*                                                                            */
void RemoveIndividualsFromBST(PPEDIGREE pd,PPINDIVIDUAL i,int (*criteria)(PINDIVIDUAL) ){


	/*                     */
	/* return if *i==NULL: */
	/*                     */
	if(!*i) return;
	
	/*                                                    */
	/* loop as long as individuals                        */
	/* moving into the deleted position                   */
	/* at the current top of the tree                     */
	/* continue to require removal:                       */
	/* --this insures that the new node replacing the top */
	/* node is itself visited:                            */
	/*                                                    */
	while(RemoveIndividual(i,criteria)) pd->n--;
	/*                                          */
	/* Now we're at a node that wasn't removed: */
	/* it could be NULL:                        */
	/*                                          */
	if(!*i) return;
	/*          */
	/* recurse: */
	/*          */
	RemoveIndividualsFromBST(pd,&(*i)->l,criteria);
	RemoveIndividualsFromBST(pd,&(*i)->r,criteria);
}

/*                     */
/* RemoveIndividual(): */
/*                     */
int RemoveIndividual(PPINDIVIDUAL i,int (*remove)(PINDIVIDUAL)){

	PINDIVIDUAL o; /* address of node to remove              */
	PINDIVIDUAL a; /* node at which to attach opposite child */

	/*                                        */
	/* return if *i is NULL: no one to remove */
	/*                                        */
	if(!*i) return 0;

	/*                             */
	/* leave if not to be removed: */
	/*                             */
	if(!(*remove)(*i)) return 0;	
	/*                                                    */
	/* get here if remove() function evaluates to true... */
	/*		                                                  */

	/*                                         */
	/* save address of the node to be removed: */
	/*                                         */
	o=*i;
	/*                                            */
	/* if there is only one child node, then it's */
	/* really easy to remove the desired node:    */
	/*                                            */
	if(!((*i)->r||(*i)->l))
		/*              */
		/* no children: */
		/*              */
		*i=NULL;

	else if(!(*i)->r)
		/*                 */
		/* one left child: */
		/*                 */
		*i=(*i)->l;

	else if(!(*i)->l)
		/*                  */
		/* one right child: */
		/*                  */
		*i=(*i)->r;

	else{
		/*                                                        */
		/* two children:                                          */
		/* randomly choose one of the children to attach first:   */
		/* on average, this works well, even though we don't know */
		/* specifically how unbalanced this individual tree will  */
		/* become:                                                */
		/*                                                        */
		*i=(PINDIVIDUAL)RandomlyChoose((*i)->l,(*i)->r);
		/*                                                   */
		/* walk down the chosen child to find the            */
		/* insertion point for the other child in the chosen */
		/* child:                                            */
		/*                                                   */
		if(*i==o->r){
			/*                         */
			/* right child was chosen: */
			/*                         */
			for(a=*i;a->l;a=a->l);
			/*              */
			/* attach o->l: */
			/*              */
			a->l=o->l;
		}else{
			/*                        */
			/* left child was chosen: */
			/*                        */
			for(a=*i;a->r;a=a->r);
			/*              */
			/* attach o->r: */
			/*              */
			a->r=o->r;
		}

	}
	/*                      */
	/* Remove the old node: */
	/*                      */
	FreeSingleIndividual(o);
	/*                             */
	/* indicate removal of a node: */
	/*                             */
	return 1;
}


/*                   */
/* RandomlyChoose(): */
/*                   */
void *RandomlyChoose(void *one,void *theother){
	
	return (rand()%2)?one:theother;

}


/*                                                              */
/* FreeSingleIndividual(): Frees a single individual without    */
/* recursing the left and right children who have, presumably,  */
/* been moved elsewhere (in a delete operation):                */
/*                                                              */
void FreeSingleIndividual(PINDIVIDUAL p){

	if(p->a) free(p->a);
	if(p->k) free(p->k);
	free(p);

}



/*                   */
/* RemovePedigree(): */
/*                   */
int RemovePedigree(PPPEDIGREE i,int (*remove)(PPEDIGREE)){

	PPEDIGREE o; /* address of node to remove              */
	PPEDIGREE a; /* node at which to attach opposite child */

	/*                                        */
	/* return if *i is NULL: no one to remove */
	/*                                        */
	if(!*i) return 0;

	/*                             */
	/* leave if not to be removed: */
	/*                             */
	if(!(*remove)(*i)) return 0;	
	/*                                                    */
	/* get here if remove() function evaluates to true... */
	/*		                                                  */

	/*                                         */
	/* save address of the node to be removed: */
	/*                                         */
	o=*i;
	/*                                            */
	/* if there is only one child node, then it's */
	/* really easy to remove the desired node:    */
	/*                                            */
	if(!((*i)->r||(*i)->l))
		/*              */
		/* no children: */
		/*              */
		*i=NULL;

	else if(!(*i)->r)
		/*                 */
		/* one left child: */
		/*                 */
		*i=(*i)->l;

	else if(!(*i)->l)
		/*                  */
		/* one right child: */
		/*                  */
		*i=(*i)->r;

	else{
		/*                                                        */
		/* two children:                                          */
		/* randomly choose one of the children to attach first:   */
		/* on average, this works well, even though we don't know */
		/* specifically how unbalanced this individual tree will  */
		/* become:                                                */
		/*                                                        */
		*i=(PPEDIGREE)RandomlyChoose((*i)->l,(*i)->r);
		/*                                                   */
		/* walk down the chosen child to find the            */
		/* insertion point for the other child in the chosen */
		/* child:                                            */
		/*                                                   */
		if(*i==o->r){
			/*                         */
			/* right child was chosen: */
			/*                         */
			for(a=*i;a->l;a=a->l);
			/*              */
			/* attach o->l: */
			/*              */
			a->l=o->l;
		}else{
			/*                        */
			/* left child was chosen: */
			/*                        */
			for(a=*i;a->r;a=a->r);
			/*              */
			/* attach o->r: */
			/*              */
			a->r=o->r;
		}

	}
	/*                      */
	/* Remove the old node: */
	/*                      */
	FreeSinglePedigree(o);
	/*                             */
	/* indicate removal of a node: */
	/*                             */
	return 1;
}

/*                                                                      */
/* FreeSinglePedigree(): Unlike FreeBinaryPedigreeTree(), which         */
/* frees the entire PPEDIGREE tree via recursion, FreeSinglePedigree()  */
/* frees a single PPEDIGREE node, with the assumption being that the    */
/* left and right children have been moved elsewhere.  This function is */
/* used when deleting individual nodes in a PPEDIGREE BST tree:         */
/*                                                                      */
void FreeSinglePedigree(PPEDIGREE pd){

	/*                             */
	/* nothing to do if NULL node: */
	/*                             */
	if(!pd) return;
	
	/*                       */
	/* Free the individuals: */
	/*                       */
	FreeIndividualBST(pd->h);

	/*                                              */
	/* Free the founding father and mother vectors: */
	/*                                              */
	if(pd->nff) free(pd->ff);
	if(pd->nfm) free(pd->fm);
	/*                                           */
	/* Finally, free the pedigree object itself: */
	/*                                           */
	free(pd);
}


/*                                                                   */
/* IndividualLookup2(): Searches through the binary pedigree         */
/* tree starting from the head node and returns a pointer            */
/* to an individual based on individual's FamilyID and IndividualID, */
/* or NULL if not found:                                             */
/*                                                                   */
/* This allows the program to go from a database record and find     */
/* the corresponding INDIVIDUAL object (normally, we go the other    */
/* way around, i.e., from the INDIVIDUAL who has a member variable   */
/* for the corresponding database record number)                     */
/*                                                                   */
void IndividualLookup2(PPEDIGREE hpd,char *fid,char *id,PPPEDIGREE ppd,PPINDIVIDUAL ppi){

	PPEDIGREE   pd;
	PINDIVIDUAL p;
	int cf;
	
	/*                                                */
	/* Non-recursive lookup through the PEDIGREE BST: */
	/*                                                */
	for(pd=hpd;pd && (cf=strcmp(fid,pd->fid))!=0;pd=cf<0?pd->l:pd->r);
	if(!pd){
		/*                               */
		/* pedigree family id not valid: */
		/*                               */
		*ppd=(PPEDIGREE)   NULL;
		*ppi=(PINDIVIDUAL) NULL;
	 	return;
	}
	/*                                                         */
	/* get here if pd is valid: now look for individual, again */
	/* by non-recursively looking for the id key:              */
	/*                                                         */
	for(p=pd->h;p && (cf=strcmp(id,p->id))!=0;p=cf<0?p->l:p->r);
	/*                              */
	/* p will be NULL if not found: */
	/*                              */
	/* store reference pointers:    */
	/*                              */
	*ppd=pd;
	*ppi=p;
	return;
}


/*                                                                          */
/* ConnectIndividual: Connect's one individual in a pedigree                */
/* to his/her parents.                                                      */
/*                                                                          */
/* NOTE BENE: virtual parents added by ConnectIndividual() MUST             */
/* use UnoptomizedIndividualInstall(), the naive INDIVIDUAL installer       */
/* which does not do node rotations, because node rotations are forbidden   */
/* in the middle of tree traversal (ConnectIndividual() ends up being a     */
/* visit() function in a recursive tree traversal).  This leads to some     */
/* trivial tree imbalance -- trivial because the number of inserted virtual */
/* parents is always small compared to the total number of individuals in   */
/* the pedigree.                                                            */
/*                                                                          */
void ConnectIndividual(PPEDIGREE pd,PINDIVIDUAL p){

	extern PCONTROL g;
	
	/*                      */
	/* return if NULL node: */
	/*                      */
	if(!p) return;
	
	/*////////////////////////////////// */
	/*                                   */
	/* join p to father and mother:      */
	/*                                   */
	/*////////////////////////////////// */
	p->f=IndividualLookup(pd->h,p->fid);
	p->m=IndividualLookup(pd->h,p->mid);

	/*                                */
	/*                                */
	/* Dummying-in of parents:        */
	/*                                */
	/* WHEN individual is in database */
	/* AND  parent is not in database */

	/*////////////////////////////// */
	/*                               */
	/* dummy in FATHER if necessary: */
	/*                               */
	/*////////////////////////////// */
	if(p->db && p->f==NULL){
		/*                       */
		/* if dad id is missing: */
		/*                       */
		if(!p->fid[0]){
			/*                                                        */
			/* Whether an ID is created for                           */
			/* the individual depends on the class of                 */
			/* the individual in the FUSION study.                    */
			/*                                                        */
			/*                                                        */
			/* The final "else" statement permits creation of         */
			/* a random ID in a non-FUSION database that contains     */
			/* the error of having only a single parent listed:       */
			/* This generates an error message to the error log.      */
			/* The problem is that if you don't dummy in the one      */
			/* missing parent, Madeline's other algorithms, such      */
			/* as DetermineFounders(), get really messed up and don't */
			/* do their jobs.  So dummying-in is really the best      */
			/* thing to do for this type of error                     */
			/*                                                        */
			if(g->FusionSupport && p->clas==SIBLING){
				/*                                 */
				/* SIBLING class includes PROBANDs */
				/*                                 */
				strncpy(p->fid,p->id,4);
				/*                                                 */
				/* If a FUSION 2 sampled parent exists, use that,  */
				/* else, use a FUSION 1 parent ID.  So, here we    */
				/* first try a FUSION 2 ID -- the "else if" blocks */
				/* do the same thing:                              */
				/*                                                 */
				p->fid[4]='+';
				strncpy(p->fid+5,"200",3);
				p->fid[8]='\0';

				/*                                                     */
				/* If FUSION 2 sampled parent not found, revert        */
				/* to a FUSION 1 ID with a '-': same thing is done in  */
				/* next two "else if" blocks:                          */
				/*                                                     */
				if(IndividualLookup(pd->h,p->fid)==NULL) p->fid[4]='-';

			}else if(g->FusionSupport && p->clas==SIBLINGOFFSPRING){

				strncpy(p->fid,p->id,4);

				p->fid[4]='+';
				/*                                     */
				/* Determine whether the dummied-in    */
				/* parent must be the "400" sib or the */
				/* "700" sib spouse                    */
				/*                                     */
				/*                                     */
				if(p->mid[0]){
					if     (p->mid[5]=='4') p->fid[5]='7';
					else if(p->mid[5]=='7') p->fid[5]='4';
					strncpy(p->fid+6,p->mid+6,2);
					p->fid[8]='\0';
				}else{
					strncpy(p->fid+5,"420",3);
					p->fid[8]='\0';
					/*                          */
					/* This is really an error: */
					/*                          */
					eprintf(WARNINGFLAG,"ConnectIndividual() Neither parent listed in database:\n\tIn family %s, Madeline has named %s's missing father \"%s\"\n",
					pd->fid,
					p->id,
					p->fid);

				}

				if(IndividualLookup(pd->h,p->fid)==NULL) p->fid[4]='-';
				
			}else if(g->FusionSupport && p->clas==PROBANDOFFSPRING){

				strncpy(p->fid,p->id,4);

				p->fid[4]='+';
				/*                                     */
				/* Need to determine whether the other */
				/* parent, in this case the mom,       */
				/* if present, is the proband or not:  */
				/*                                     */
				if(p->mid[0]){
					if     (p->mid[5]=='1') p->fid[5]='5';
					else if(p->mid[5]=='5') p->fid[5]='1';
					strncpy(p->fid+6,p->mid+6,2);
					p->fid[8]='\0';
				}else{
					/*                     */
					/* dummied-in proband: */
					/*                     */
					strncpy(p->fid+5,"120",3);
					p->fid[8]='\0';
				}

				if(IndividualLookup(pd->h,p->fid)==NULL) p->fid[4]='-';

			}else if(p->mid[0]){
				/*                                                      */
				/* -This is an error condition because you're supposed  */
				/*  to have either NO parents or BOTH parents listed in */
				/*  the database.  So, to avoid real problems, we dummy */
				/*  in the missing parent with a random ID, and emit an */
				/*  error message (which toggles Madeline's "HasErrors" */
				/*  state):                                             */
				/*                                                      */
				
				/*                                                     */
				/* If mom already has a spouse, use the first spouse's */
				/* ID as the father.  Otherwise, generate a new ID:    */
				/*                                                     */
				if(p->m && p->m->na)
					strcpy(p->fid,p->m->a[0]->id);
				else
					GenerateRandomID(p->fid);
				
				eprintf(WARNINGFLAG,"ConnectIndividual(): Only one parent listed in database:\n\tIn pedigree %s, Madeline has named %s's missing father \"%s\"\n",
					pd->fid,
					p->id,
					p->fid);
			}						
		}
		/*                                                  */
		/* see if a non-empty parent id has been added yet: */
		/*                                                  */
		if(p->fid[0]){
			p->f=IndividualLookup(pd->h,p->fid);
			if(p->f==NULL){
				p->f=UnoptomizedInstallIndividual(pd,p->fid,NULL,0);
				/*      */
				/* sex: */
				/*      */
				p->f->female=FALSE;
				/*               */
				/* FUSION class: */
				/*               */
				if(IsFusionID(p->f->id)){
					p->f->clas=p->f->id[5]-'0';
					if(p->f->clas==PROBAND) p->f->clas=SIBLING;
					/*                                     */
					/* If dummied-in parent is 400 or 100, */
					/* then connect dummied-in parent      */
					/* back to 200 and 300 grandparents:   */
					/*                                     */
					if(!p->f->db && p->f->clas==SIBLING)
						ConnectInsertedParent(pd,p->f);
				}								
				pd->n++;
			}
		}
	}
	/*                                     */
	/* prevent infinite looping on FATHER: */
	/*                                     */
	if(p->f==p){
		eprintf(FATALFLAG,"ConnectIndividual(): Bad father data for %s\n",p->id);
	}
	/*                      */
	/* Check sex of father: */
	/*                      */
	if(p->f && p->f->female==MISSING){
		/*                                          */
		/* Automatically change sex, issue warning: */
		/*                                          */
		eprintf(WARNINGFLAG,"ConnectIndividual() Gender in database is incomplete:\n\tGender of %s's father, %s, changed from MISSING to MALE\n",p->id,p->f->id);
		p->f->female=FALSE;
	}
	if(p->f && p->f->female==TRUE   ){
		/*                                                            */
		/* Sex changes won't work if the parent already has children: */
		/*                                                            */
		if(p->f->nk){
			eprintf(FATALFLAG,"ConnectIndividual(): Conflicting gender roles\n\t%s appears as both a father and a mother in the database!\n",p->f->id);
		}
		/*                                                            */
		/* Otherwise, automatically change sex, issue severe warning: */
		/*                                                            */
		eprintf(SEVEREWARNINGFLAG,"ConnectIndividual(): Gender in database is incorrect:\n\tGender of %s's father, %s, changed from FEMALE to MALE\n",p->id,p->f->id);
		p->f->female=FALSE;
	}
	
	/*/////////////////////////////// */
	/*                                */
	/* dummy in MOTHER if necessary:  */
	/*                                */
	/*/////////////////////////////// */
	if(p->db && p->m==NULL){
		/*                                */
		/* If mom id is empty or missing: */
		/*                                */
		if(!p->mid[0]){
			/*                                                        */
			/* Whether an ID is created for                           */
			/* the individual depends on the class of                 */
			/* the individual in the FUSION study:                    */
			/*                                                        */
			/* The final "else" statement permits creation of         */
			/* a random ID in a non-FUSION database that contains     */
			/* the error of having only a single parent listed:       */
			/* This generates an error message to the error log.      */
			/* The problem is that if you don't dummy in the one      */
			/* missing parent, Madeline's other algorithms, such      */
			/* as DetermineFounders(), get really messed up and don't */
			/* do their jobs.  So dummying-in is really the best      */
			/* thing to do for this type of error                     */
			/*                                                        */
			if(g->FusionSupport && p->clas==SIBLING){
				/*                                  */
				/* SIBLING class includes PROBANDs: */
				/*                                  */
				strncpy(p->mid,p->id,4);
				/*                                                  */
				/* If a FUSION 2 sampled parent exists, use that,   */
				/* else, use a FUSION 1 parent ID.  So, here we     */
				/* first try a FUSION 2 ID -- same thing is done in */
				/* succeeding "else if" blocks:                     */
				/*                                                  */
				p->mid[4]='+';
				strncpy(p->mid+5,"300",3);
				p->mid[8]='\0';

				/*                                                 */
				/* If FUSION 2 sampled parent not found, revert    */
				/* to a FUSION 1 ID with a '-': same thing is done */
				/* in next two "else if" blocks:                   */
				/*                                                 */
				if(IndividualLookup(pd->h,p->mid)==NULL) p->mid[4]='-';

			}else if(g->FusionSupport && p->clas==SIBLINGOFFSPRING){

				strncpy(p->mid,p->id,4);

				p->mid[4]='+';
				/*                                     */
				/* Determine whether the dummied-in    */
				/* parent must be the "400" sib or the */
				/* "700" sib spouse                    */
				/*                                     */
				/*                                     */
				if(p->fid[0]){
				 	if     (p->fid[5]=='4') p->mid[5]='7';
				 	else if(p->fid[5]=='7') p->mid[5]='4';
					strncpy(p->mid+6,p->fid+6,2);
					p->mid[8]='\0';
				}else{
					strncpy(p->mid+5,"430",3);
					p->mid[8]='\0';

					/*                          */
					/* This is really an error: */
					/*                          */
					eprintf(WARNINGFLAG,"ConnectIndividual(): Neither parent listed in database:\n\tIn family %s, Madeline has named %s's missing mother \"%s\"\n",
					pd->fid,
					p->id,
					p->mid);

				}

				if(IndividualLookup(pd->h,p->mid)==NULL) p->mid[4]='-';

			}else if(g->FusionSupport && p->clas==PROBANDOFFSPRING){

				strncpy(p->mid,p->id,4);

				p->mid[4]='+';
				/*                                     */
				/* Need to determine whether the other */
				/* parent, in this case the father,    */
				/* if present, is the proband or not:  */
				/*                                     */
				if(p->fid[0]){
					if     (p->fid[5]=='1') p->mid[5]='5';
					else if(p->fid[5]=='5') p->mid[5]='1';
					strncpy(p->mid+6,p->fid+6,2);
					p->mid[8]='\0';
				}else{
					/*                     */
					/* dummied-in proband: */
					/*                     */
					strncpy(p->mid+5,"130",3);
					p->mid[8]='\0';
				}
				
				if(IndividualLookup(pd->h,p->mid)==NULL) p->mid[4]='-';

			}else if(p->fid[0]){
				/*                                                      */
				/* -This is an error condition because you're supposed  */
				/*  to have either NO parents or BOTH parents listed in */
				/*  the database.  So, to avoid real problems, we dummy */
				/*  in the missing parent with a random ID, and emit an */
				/*  error message (which toggles Madeline's "HasErrors" */
				/*  state):                                             */
				/*                                                      */


				/*                                                     */
				/* If dad already has a spouse, use the first spouse's */
				/* ID as the mother.  Otherwise, generate a new ID:    */
				/*                                                     */
				if(p->f && p->f->na)
					strcpy(p->mid,p->f->a[0]->id);
				else
					GenerateRandomID(p->mid);

				eprintf(WARNINGFLAG,"ConnectIndividual(): Only one parent listed in database:\n\tIn pedigree %s, Madeline has named %s's missing mother \"%s\"\n",
					pd->fid,
					p->id,
					p->mid);
			}
		}
		/*                                                  */
		/* see if a non-empty parent id has been added yet: */
		/*                                                  */
		if(p->mid[0]){
			p->m=IndividualLookup(pd->h,p->mid);
			if(p->m==NULL){
			p->m=UnoptomizedInstallIndividual(pd,p->mid,NULL,0);
				p->m->female=TRUE;
				/*               */
				/* FUSION class: */
				/*               */
				if(IsFusionID(p->m->id)){
					p->m->clas=p->m->id[5]-'0';
					if(p->m->clas==PROBAND) p->m->clas=SIBLING;
					/*                                     */
					/* If dummied-in parent is 400 or 100, */
					/* then connect dummied-in parent      */
					/* back to 200 and 300 grandparents:   */
					/*                                     */
					if(!p->m->db && p->m->clas==SIBLING)
						ConnectInsertedParent(pd,p->m);
				}
				pd->n++;
			}
		}
	}
	/*                                     */
	/* prevent infinite looping on MOTHER: */
	/*                                     */
	if(p->m==p){
		eprintf(FATALFLAG,"ConnectIndividual(): Bad mother data for %s\n",p->id);
	}
	/*                      */
	/* Check sex of mother: */
	/*                      */
	if(p->m &&  p->m->female==MISSING){
		/*                                          */
		/* Automatically change sex, issue warning: */
		/*                                          */
		eprintf(WARNINGFLAG,"ConnectIndividual(): Gender in database is incomplete:\n\tGender of %s's mother, %s, changed from MISSING to FEMALE\n",p->id,p->m->id);
		p->m->female=TRUE;
	}
	if(p->m && p->m->female==FALSE  ){
		/*                                                            */
		/* Sex changes won't work if the parent already has children: */
		/*                                                            */
		if(p->m->nk){
			eprintf(FATALFLAG,"ConnectIndividual(): Conflicting gender roles:\n\t%s appears as both a father and a mother in the database!\n",p->m->id);
		}
		/*                                          */
		/* Automatically change sex, issue warning: */
		/*                                          */
		eprintf(SEVEREWARNINGFLAG,"ConnectIndividual() Gender in database is incorrect:\n\tGender of %s's mother, %s, changed from MALE to FEMALE\n",p->id,p->m->id);
		p->m->female=TRUE;
	}
	/*                                                 */
	/* Join individual to mom and dad, and vice-versa: */
	/*                                                 */
	JoinIndividualAndParents(p);
	/*                          */
	/* end of construction loop */
	/*                          */
}


/*                                                         */
/* JoinIndividualAndParents(): join indiv. to mom and dad, */
/* and vice-versa:                                         */
/*                                                         */
void JoinIndividualAndParents(PINDIVIDUAL p){

	int j;
	int joined;
	int listed;
	
	/*                                        */
	/* Both dad and mom must be present       */
	/* to record the children of the parents: */
	/*                                        */
	if(p->f && p->m){
		/*                             */
		/* Are dad and mom connected ? */
		/*                             */
		joined=0;
		for(j=0;j<p->f->na;j++){
			if(p->f->a[j]==p->m){
				joined=1;
				break;
			}
		}
		if(!joined){
			/* father: */
			p->f->na++;
			/*                                                           */
			/* increase father's spouse vector allocation, if necessary: */
			/*                                                           */
			if(p->f->na > p->f->ma){
			p->f->ma+=DEFAULTMATECOUNT;
			p->f->a=(PPINDIVIDUAL)realloc(p->f->a,(unsigned)p->f->ma*sizeof(PINDIVIDUAL));
				if(!p->f->a) eprintf(FATALFLAG,"JoinIndividualAndParents(): out of memory reallocating p->f->a\n");
			}
			/* add mom to dad's spouse list: */
			p->f->a[p->f->na-1]=p->m;
			
			/* mother: */
			p->m->na++;
			/*                                                           */
			/* increase mother's spouse vector allocation, if necessary: */
			/*                                                           */
			if(p->m->na > p->m->ma){
				p->m->ma+=DEFAULTMATECOUNT;
				p->m->a=(PPINDIVIDUAL)realloc(p->m->a,(unsigned)p->m->ma*sizeof(PINDIVIDUAL));
				if(!p->m->a) eprintf(FATALFLAG,"JoinIndividualAndParents(): out of memory reallocating p->m->a\n");
			}
			/* join: */
			p->m->a[p->m->na-1]=p->f; 
		}
		
		/*                                                                               */
		/* Add p as a child of the father and mother BUT ONLY if                         */
		/* he or she is not already listed as a child.                                   */
		/*                                                                               */
		/* (Believe me -- this condition really MUST be checked for because of the       */
		/* way that missing FUSION 100 or 400-level sibs can be                          */
		/* inserted into the pedigree trees when they have been excluded                 */
		/* from the database.  If not checked, an individual can be                      */
		/* added to the mom's child vector more than once due to the                     */
		/* weird fact that the individual may have been inserted during                  */
		/* tree traversal of the individual's "+800"-level offspring.  Since             */
		/* the "+800" sibling offspring sorts before the "-100" or "-400" level          */
		/* parent, the "-400" parent gets visited later -- but has already been inserted */
		/* once!  It sounds complicated -- and it is!)                                   */
		/*                                                                               */
		
		for(listed=j=0;j<p->m->nk;j++){
			if(p==p->m->k[j]) listed++;
		}
		
		
		/*                                           */
		/* add p as a child of the father and mother */
		/* only if not already listed:               */
		/*                                           */
		if(!listed){
				
			p->f->nk++;
			p->m->nk++;
			/*                                                */
			/* increase mom's offspring vector, if necessary: */
			/*                                                */
			if(p->m->nk > p->m->mk){
				p->m->mk+=DEFAULTOFFSPRINGCOUNT;
				p->m->k=(PPINDIVIDUAL)realloc(p->m->k,(unsigned)p->m->mk*sizeof(PINDIVIDUAL));
				if(!p->m->k) eprintf(FATALFLAG,"JoinIndividualAndParents(): out of memory reallocating p->m->k\n");
			}
			/* add kid to mom's kid vector: */
			p->m->k[p->m->nk-1]=p;
		}
	}
	/*            */
	/* That's it! */
	/*            */
}

/*                                                        */
/* ConnectInsertedParent(): connect a dummied-in FUSION   */
/* 400 or 100 parent: be sure to only call for dummied-in */
/* 400s and 100s:                                         */
/*                                                        */
void ConnectInsertedParent(PPEDIGREE pd,PINDIVIDUAL p){

	/*                                                   */
	/* Note how we start out with '+' FUSION 2 IDs       */
	/* (This is arbitrary: we could just as well start   */
	/* out with '-' IDs, but either way we need to check */
	/* for the possibility of the parent being sampled   */
	/* in FUSION 2:                                      */
	/*                                                   */
	
	/*            */
	/* father id: */
	/*            */
	strncpy(p->fid,p->id,4);
	p->fid[4]='+';
	strncpy(p->fid+5,"200",3);
	p->fid[8]='\0';
	/*            */
	/* mother id: */
	/*            */
	strncpy(p->mid,p->id,4);
	p->mid[4]='+';
	strncpy(p->mid+5,"300",3);
	p->mid[8]='\0';
	/*                                                  */
	/* (1) Look FATHER up based on FUSION 2 ID with '+' */
	/* (2) If not found, then try FUSION 1 ID with '-'  */
	/* (3) If still not found, then insert using        */
	/*     UnoptomizedInstallIndvidiual():              */
	/*                                                  */
	p->f=IndividualLookup(pd->h,p->fid);
	if(p->f==NULL){
		p->fid[4]='-';
		p->f=IndividualLookup(pd->h,p->fid);	
	}
	if(p->f==NULL){
		p->f=UnoptomizedInstallIndividual(pd,p->fid,NULL,0); /* note that db is NULL */
		/*                                                                           */
		/* sex:                                                                      */
		/*                                                                           */
		p->f->female=FALSE;
		/*               */
		/* FUSION class: */
		/*               */
		p->f->clas=p->f->id[5]-'0';
		/*                             */
		/* increment number of people: */
		/*                             */
		pd->n++;
	}
	/*                                                  */
	/* (1) Look up MOTHER based on FUSION 2 ID with '+' */
	/* (2) If not found, then try FUSION 1 ID with '-'  */
	/* (3) If still not found, then insert using        */
	/*     UnoptomizedInstallIndvidiual():              */
	/*                                                  */
	p->m=IndividualLookup(pd->h,p->mid);
	if(p->m==NULL){
		p->mid[4]='-';
		p->m=IndividualLookup(pd->h,p->mid);
	}
	if(p->m==NULL){
		p->m=UnoptomizedInstallIndividual(pd,p->mid,NULL,0); /* note that db is NULL */
		/*                                                                           */
		/* sex:                                                                      */
		/*                                                                           */
		p->m->female=TRUE;
		/*               */
		/* FUSION class: */
		/*               */
		p->m->clas=p->m->id[5]-'0';
		/*                             */
		/* increment number of people: */
		/*                             */
		pd->n++;
	}
	/*                                                           */
	/* join the father and mother together, and to the child, p: */
	/*                                                           */
	JoinIndividualAndParents(p);
	/*                    */
	/* that's it - return */
	/*                    */
}



/*                                                 */
/* CalculatePedigreeWidth(): Calls PedigreeWidth() */
/* in order to calculate the width of a pedigree   */
/*                                                 */
void CalculatePedigreeWidth(PPEDIGREE pd){
	
	int i;
	
	/*                                       */
	/* Traverse each founding group subtree: */
	/*                                       */
	for(i=0;i<pd->nfg;i++){
		PedigreeWidth(pd->fg[i].f);
	}
	
}


/*                                                                               */
/* PedigreeWidth() returns an integer indicating how "wide", in individuals,     */
/* a regular pedigree tree will be when drawn in an optimally balanced           */
/* manner.  PedigreeWidth operates recursively, and stores the width of the tree */
/* below each relevant individual in the p->nw element of the INDIVIDUAL         */
/*                                                                               */
int PedigreeWidth(PINDIVIDUAL p){

	int sw=0;
	int i,j;

	/* */
	/* */
	/* */
	if(p->na==0)
		/*                                                           */
		/* no mate, therefore no offspring: width is just individual */
		/*                                                           */
		return p->nw=1;
	else if(p->na){
		/*            */
		/* has mates: */
		/*            */
		if(p->female){
			/*                                                    */
			/* Loop through all mates, storing the width for each */
			/* mate, and the grand sum width for the originating  */
			/* female:                                            */
			/*                                                    */
			for(i=0;i<p->na;i++){
				/*                           */
				/* start with p->a[i]->nw=0: */
				/*                           */
				p->a[i]->nw=0;
				
				/*                                                  */
				/* for each male mate, loop through all the kids... */
				/*                                                  */
				for(j=0;j<p->nk;j++){
					/*                                               */
					/* ... and look only at kids sired by this male: */
					/*                                               */
					if(p->k[j]->f==p->a[i]){
						p->a[i]->nw+=PedigreeWidth(p->k[j]);
					}
				}
				/*                                    */
				/* correction if only a single child: */
				/*                                    */
				if(p->a[i]->nw==1) p->a[i]->nw++;

				/*                                        */
				/* Correction if showing males            */
				/* who sired no children (Eliminate OFF): */
				/*                                        */
				if(!p->a[i]->nw) p->a[i]->nw++;
								
				/*               */
				/* grand sum nw: */
				/*               */
				sw+=p->a[i]->nw;
			}
			
			/*                                                   */
			/* Compare width of subsequent tree, sw, to width of */
			/* p->a, plus self: take the wider:                  */
			/*                                                   */
			if(p->na+1>sw) sw=p->na+1;
			return p->nw=sw;
		}else{
			/*                                                       */
			/* male: loop through all female mates and get widths of */
			/* their offspring:                                      */
			/*                                                       */
			for(i=0;i<p->na;i++){

				/*                           */
				/* start with p->a[i]->nw=0: */
				/*                           */
				p->a[i]->nw=0;

				/*                                                         */
				/* for each female mate, get summed width of her children: */
				/*                                                         */
				for(j=0;j<p->a[i]->nk;j++){
					p->a[i]->nw+=PedigreeWidth(p->a[i]->k[j]);
				}
				/*                                    */
				/* correction if only a single child: */
				/*                                    */
				if(p->a[i]->nw==1) p->a[i]->nw++;

				/*                                    */
				/* Correction if showing female mates */
				/* who bore no children:              */
				/*                                    */
				if(!p->a[i]->nw) p->a[i]->nw++;

				sw+=p->a[i]->nw;
			}
			/*                                                               */
			/* Based on the way the current spacing algorithms are set       */
			/* up, the width of an individual with multiple spouses requires */
			/* one extra unit of spacing:                                    */
			/*                                                               */
			if(p->na>1) sw++;
						
			/*                                                 */
			/* Compare width of sw to width of p->a plus self: */
			/* take the wider:                                 */
			/*                                                 */
			if(p->na+1>sw) sw=p->na+1;
			return p->nw=sw;
		}
	}
	/*                 */
	/* never get here: */
	/*                 */
	return p->nw;
}


/*                                                      */
/* CalculatePedigreeVLevel(): Calls PedigreeVLevel()    */
/* in order to calculate the "vlevel" of a pedigree     */
/* which is a single founding-group pedigree's vertical */
/* depth.                                               */
/*                                                      */
/* NOTE BENE: This function provides scaffolding for    */
/* implementing future improvements to the pedigree     */
/* drawing code -ET, 2000 03 02                         */
/*                                                      */
void CalculatePedigreeVLevel(PPEDIGREE pd){
	
	int i;
	/* int vlevel;                            */
	/*                                        */
	/* Traverse each founding group sub tree: */
	/*                                        */
	for(i=0;i<pd->nfg;i++){
		PedigreeVLevel(pd->fg[i].f);
		pd->fg[i].vlevel=pd->fg[i].f->vlevel;
	}	
}



/*                                                                              */
/* PedigreeVLevel(): Calculates the vlevel of a pedigree or subtree.            */
/* The vlevel is zero for the lowest person in a pedigree, 1 for his parents,   */
/* 2 for his grandparents, and so on up the tree until the ultimate founder     */
/* is reached.  By comparing the "vlevels" of founders in different founding    */
/* groups, one can figure out how to adjust the vertical placement of trees     */
/* for best appearance when printing.  In effect, the "vlevel" is an indication */
/* of the vertical "depth" of a pedigree.  It is similar to the number of       */
/* generations.  However, because of the possibility of inbred loops, "vlevels" */
/* which can be considered a simple graphic drafting parameter are used instead */
/* of generations which has a biological meaning that can become quite obscure  */
/* in inbred or complicated pedigrees.                                          */
/*                                                                              */
/* NOTE BENE: This function provides scaffolding for future improvements to the */
/* pedigree drawing code that have not yet been implemented - ET, 2000 03 02    */
/*                                                                              */
int PedigreeVLevel(PINDIVIDUAL p){

	int vlevel;
	/* int maxvlevel; */
	int i,j;

	/* */
	/* */
	/* */
	if(p->na==0){
		/*                                                      */
		/* no mate, therefore no offspring.  The vlevel of this */
		/* individual is zero.  Return 1 to indicate the vlevel */
		/* of his or her parent:                                */
		
		p->vlevel=0;
		return p->vlevel+1;
		
		
	}else if(p->na){
		/*            */
		/* has mates: */
		/*            */
		if(p->female){
			/*                                                     */
			/* Loop through all mates, storing the vlevel for each */
			/* mate, and the maximum vlevel for the originating    */
			/* female:                                             */
			/*                                                     */
			for(i=0;i<p->na;i++){
				
				/*                                                  */
				/* for each male mate, loop through all the kids... */
				/*                                                  */
				for(j=0;j<p->nk;j++){
					/*                                               */
					/* ... and look only at kids sired by this male: */
					/*                                               */
					if(p->k[j]->f==p->a[i]){
						/*                                              */
						/* get the vlevel derived from the current kid: */
						/*                                              */
						vlevel=PedigreeVLevel(p->k[j]);
						/*                                  */
						/* keep only the maximum vlevel as: */
						/* the vlevel of this spouse:       */
						/*                                  */
						if(vlevel>p->a[i]->vlevel){
							p->a[i]->vlevel=vlevel;
						}
					}
				}
				/*                                                */
				/* keep only the maximum vlevel among all spouses */
				/* as the true vlevel of the originating spouse:  */
				/*                                                */
				if(p->a[i]->vlevel>p->vlevel){
					p->vlevel=p->a[i]->vlevel;
				}
			}
			/*                                                       */
			/* return the vlevel of the originating spouse plus one: */
			/*                                                       */
			return p->vlevel+1;
		}else{
			/*                                                       */
			/* male: loop through all female mates and get widths of */
			/* their offspring:                                      */
			/*                                                       */
			for(i=0;i<p->na;i++){
				/*                       */
				/* for each female mate: */
				/*                       */
				for(j=0;j<p->a[i]->nk;j++){
					/*                                              */
					/* get the vlevel derived from the current kid: */
					/*                                              */
					vlevel=PedigreeVLevel(p->a[i]->k[j]);
					/*                                  */
					/* keep only the maximum vlevel as: */
					/* the vlevel of this spouse:       */
					/*                                  */
					if(vlevel>p->a[i]->vlevel){
						p->a[i]->vlevel=vlevel;
					}
				}
				/*                                                */
				/* keep only the maximum vlevel among all spouses */
				/* as the true vlevel of the originating spouse:  */
				/*                                                */
				if(p->a[i]->vlevel>p->vlevel){
					p->vlevel=p->a[i]->vlevel;
				}
			}
			/*                                                       */
			/* return the vlevel of the originating spouse plus one: */
			/*                                                       */
			return p->vlevel+1;
		}
	}
	/*                 */
	/* never get here: */
	/*                 */
	return p->vlevel;
}



/*                                                                       */
/* SetFoundingGroups(): Determines and allocates                         */
/* founding groups.  Each element in the fg vector                       */
/* contains a pointer to one of the ultimate founders                    */
/* of each subtree.  These founders are already stored on                */
/* the nff and nfm vectors, but they aren't sorted according             */
/* to who is multiply-mated.  The fg vector is set up so that            */
/* the multiply-mated ancestral founders are stored in the vector        */
/* first, followed by any remaining singly-mated ancestors who           */
/* aren't mated to one of the multiply-mated ancestors.  Thus, the       */
/* additional abstraction of a "founding group" provides a way to        */
/* access the entire pedigree tree for the purposes of drawing via       */
/* the fewest number of ancestral founders.                              */
/*                                                                       */
/* SetFoundingGroups() uses the INDIVIDUAL'S "counted" flag to keep      */
/* track of whose been accessed and who hasn't.  The founder who         */
/* will be stored on the founding group vector is designated with a      */
/* special value: others are just designated by incrementing p->counted. */
/*                                                                       */
/* FOUNDINGGROUPFOUNDER is a totally arbitrary distinct value that's     */
/* greater than one and also greater than the possible number of         */
/* mates that an individual could ever be mated to:                      */
/*                                                                       */
#define FOUNDINGGROUPFOUNDER 99999
/* */
/* */
/* */
void SetFoundingGroups(PPEDIGREE pd){

	int i;
	int j;
	
	/*                                          */
	/* PHASE I: Count number of founding groups */
	/*                                          */

	/*                                                               */
	/* First process the multiply-mated ancestors, checking both the */
	/* male and female founder vectors for such:                     */
	/*                                                               */

	/*                                                           */
	/* female founding vector, pass for multiply-mated founders: */
	/*                                                           */
	for(pd->nfg=i=0;i<pd->nfm;i++){
		if(pd->fm[i]->na>1){
			/*                    */
			/* increment fgcount: */
			/*                    */
			pd->nfg++;
			/*                                                  */
			/* mark this ancestor and all of her mates as seen: */
			/*                                                  */
			pd->fm[i]->counted=FOUNDINGGROUPFOUNDER;
			for(j=0;j<pd->fm[i]->na;j++) pd->fm[i]->a[j]->counted++;
		}
	}
	/*                                                         */
	/* male founding vector, pass for multiply-mated founders: */
	/*                                                         */
	for(i=0;i<pd->nff;i++){
		if(pd->ff[i]->na>1){
			/*                    */
			/* increment fgcount: */
			/*                    */
			pd->nfg++;
			/*                                                  */
			/* mark this ancestor and all of his mates as seen: */
			/*                                                  */
			pd->ff[i]->counted=FOUNDINGGROUPFOUNDER;
			for(j=0;j<pd->ff[i]->na;j++) pd->ff[i]->a[j]->counted++;		
		}
	}
	
	/*                                                                */
	/* final pass for any singly-mated founders who, because of the   */
	/* previous traversals, can only be married to other singly-mated */
	/* founders, so we only need traverse one of the two founding     */
	/* vectors (we picked the female vector):                         */
	/*                                                                */
	for(i=0;i<pd->nfm;i++){
		/*                                                          */
		/* Process only unprocessed individuals (check p->counted): */
		/*                                                          */
		if(!pd->fm[i]->counted){
			pd->nfg++;
			pd->fm[i]->counted=FOUNDINGGROUPFOUNDER;
			/*                                           */
			/* Be careful! Here we check for singletons: */
			/*                                           */
			if(pd->fm[i]->na) pd->fm[i]->a[0]->counted++;
		}
	}
	
	/*                                                                */
	/* PHASE II: Allocate and then store the founding group founders: */
	/*                                                                */

	/*                                                  */
	/* Singleton pedigrees don't have a founding group: */
	/*                                                  */
	if(!pd->nfg) return;

	/*                          */
	/* Allocate and initialize: */
	/*                          */
	pd->fg=NewFoundingGroupVector(pd->nfg);
	/*                                                           */
	/* Now loop back over the nfm and nff vectors as before, but */
	/* this time storing the founders: j indexes the fg vector:  */
	/*                                                           */
	for(j=i=0;i<pd->nfm;i++){
		/*                                                               */
		/* ">=" is used to allow for the rare possibility that           */
		/* one multiply-mated founder is mated to another multiply-mated */
		/* founder:                                                      */
		/*                                                               */
		if(pd->fm[i]->counted>=FOUNDINGGROUPFOUNDER){
			/*                    */
			/* store the founder: */
			/*                    */
			pd->fg[j].f=pd->fm[i];
			/*                     */
			/* increment fg index: */
			/*                     */
			j++;
		}
	}
	/*                      */
	/* now the male vector: */
	/*                      */
	for(i=0;i<pd->nff;i++){
		/*                                                               */
		/* ">=" is used to allow for the rare possibility that           */
		/* one multiply-mated founder is mated to another multiply-mated */
		/* founder:                                                      */
		/*                                                               */
		if(pd->ff[i]->counted>=FOUNDINGGROUPFOUNDER){
			/*                    */
			/* store the founder: */
			/*                    */
			pd->fg[j].f=pd->ff[i];
			/*                     */
			/* increment fg index: */
			/*                     */
			j++;
		}
	}
	/*           */
	/* All done! */
	/*           */
}



/*                        */
/* NewFoundingGroupVector */
/*                        */
PFOUNDINGGROUP NewFoundingGroupVector(int fgcount){
	
	int i;
	PFOUNDINGGROUP pfg;
		
	/*                          */
	/* Allocate and initialize: */
	/*                          */
	pfg=(PFOUNDINGGROUP) malloc((unsigned)fgcount*sizeof(FOUNDINGGROUP));
	if(!pfg) eprintf(FATALFLAG,"NewFoundingGroupVector(): unable to allocate FOUNDINGGROUP vector\n");
	for(i=0;i<fgcount;i++){
	
		pfg[i].idx=i;
		pfg[i].f=NULL;
		pfg[i].rwidth=0;
		pfg[i].lwidth=0;
		pfg[i].vlevel=0;
		
		pfg[i].lowx  =DBL_MAX;
		pfg[i].lowy  =DBL_MAX;
		pfg[i].highx =DBL_MIN;
		pfg[i].highy =DBL_MIN;
		pfg[i].width =0.0;
		pfg[i].height=0.0;
				
	}
	return pfg;
}

/*                                                            */
/* DetermineFoundingGroup() determines the founding group     */
/* to which an individual belongs.  In some cases, this       */
/* may be ambiguous because an individual's ancestors can be  */
/* traced up to more than one founding group.  In this case,  */
/* the function returns the index of one of the individual's  */
/* founding groups, which is good enough:                     */
/*                                                            */
int DetermineFoundingGroup(PINDIVIDUAL p,PPEDIGREE pd){

	PINDIVIDUAL q;
	int i;
	
	/*                                                             */
	/* Locate ultimate ancestors of p.  If we find                 */
	/* an individual whose grandparents are all missing, then      */
	/* the parental pair of that individual are ultimate founders, */
	/* and one of the parental pair must be listed as a founding   */
	/* group founder:                                              */
	/*                                                             */
	for(q=p;
	    q && !(q->f && q->m && !(q->f->f || q->f->m || q->m->f || q->m->m));
	    q=(q->f && (q->f->f || q->f->m))?q->f:q->m);
	/*                                 */
	/* Determine which founding group: */
	/*                                 */
	for(i=0;i<pd->nfg;i++){
		if(q->f==pd->fg[i].f || q->m==pd->fg[i].f) return i;
	}
	/*                          */
	/* Shouldn't ever get here: */
	/*                          */
	return MISSING;
}

/*                                                                                     */
/* DetermineFounders() ... by examining parents' parents                               */
/*                                                                                     */
/* 2003.05.04.ET ADDENDUM: THESE ARE NOW PROPERLY CALLED "PRIMARY FOUNDERS"            */
/*                                                                                     */
/* Definition: founders are any pair of mates who both don't have parents.             */
/* Even though founders are considered as pairs, they are stored individually,         */
/* so that we can have a triplet of say, a founding father with two wives, so          */
/* only three people but two pairs as the entry points for two complete lineages.      */
/*                                                                                     */
/* Every time new founders are encountered, the PPINDIVIDUAL vectors for ff and fm     */
/* need to be increased to hold the pointers:                                          */
/*                                                                                     */
void DetermineFounders(PPEDIGREE pd,PINDIVIDUAL p){

	int j;
	int joined;

	/*                                                    */
	/* the easiest way is to look at offspring:           */
	/* if ALL the grandparents are NULL, then the parents */
	/* are founders:                                      */
	/*                                                    */
	if(p->f && p->m && !(p->f->f||p->f->m||p->m->f||p->m->m)){
		/*                                               */
		/* add father as founder if not already present: */
		/*                                               */
		joined=0;
		for(j=0;j<pd->nff;j++){
			if(pd->ff[j]==p->f){
				joined=1;
				break;
			}
		}
		if(!joined){
			pd->nff++;
			if(!pd->ff) pd->ff=(PPINDIVIDUAL)malloc((unsigned)pd->nff*sizeof(PINDIVIDUAL));
			else        pd->ff=(PPINDIVIDUAL)realloc(pd->ff,pd->nff*sizeof(PINDIVIDUAL));
			if(!pd->ff) PDErrorReport("DetermineFounders(): no memory for founding father vector");
			pd->ff[pd->nff-1]=p->f;
			/* 2003.05.05.ET: Explicitly mark person as a PrimaryFounder: */
			p->f->IsPrimaryFounder++;
		}
		/*                                               */
		/* add mother as founder if not already present: */
		/*                                               */
		joined=0;
		for(j=0;j<pd->nfm;j++){
			if(pd->fm[j]==p->m){
				joined=1;
				break;
			}
		}
		if(!joined){
			pd->nfm++;
			if(!pd->fm) pd->fm=(PPINDIVIDUAL)malloc((unsigned)pd->nfm*sizeof(PINDIVIDUAL));
			else        pd->fm=(PPINDIVIDUAL)realloc(pd->fm,pd->nfm*sizeof(PINDIVIDUAL));
			if(!pd->fm) PDErrorReport("DetermineFounders(): No memory for fm vector.");
			pd->fm[pd->nfm-1]=p->m;
			/* 2003.05.05.ET: Explicitly mark person as a PrimaryFounder: */
			p->m->IsPrimaryFounder++;
		}
	}
	/*                                           */
	/* Done examining this individual's parent's */
	/*                                           */
}


/*                                                         */
/* NewFieldFlagVector(): Allocates a new field flag vector */
/*                                                         */
unsigned char *NewFieldFlagVector(int FieldCount){
	
	unsigned char *v;
	int i;
	
	v=(unsigned char *)malloc((unsigned)FieldCount);
	if(!v) PDErrorReport("NewFieldFlagVector(): Out of memory.");
	
	/* Initialize vector to zero */
	for(i=0;i<FieldCount;i++) v[i]=0;
	return v;
	
}

/*                                                         */
/* NewIntegrityVector(): Allocates a new integrity vector  */
/*                                                         */
/* NOTE BENE: The vector length is simply the total number */
/* of fields, which is actually longer than the number of  */
/* genotype fields, so some of this is WASTED.  However,   */
/* in general the MAJORITY of fields in a data table will  */
/* be GENOTYPE fields, at least when one wants to check    */
/* for inheritance inconsistencies.  Moreover, the vector  */
/* is only allocated after an inconsistency is found, so   */
/* a bunch of people won't even have the vector anyway.    */
/*                                                         */
unsigned char *NewIntegrityVector(int FieldCount){
	
	unsigned char *v;
	int i;
	
	v=(unsigned char *)malloc((unsigned)FieldCount);
	if(!v) PDErrorReport("NewFieldFlagVector(): Out of memory.");
	
	/* Initialize vector to zero */
	for(i=0;i<FieldCount;i++) v[i]=0;
	return v;
	
}

/*                        */
/* end of file pedigree.c */
/*                        */

