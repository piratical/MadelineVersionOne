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
/* "pedigree.h" is part of the Madeline software distribution.                       */
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
/* pedigree.h                                           */
/*                                                      */
/* Library to model family pedigree structures          */
/* in Madeline                                          */
/*                                                      */
/* initiated by ET 970110                               */
/*                                                      */

/*///////////////    */
/*                   */
/* ANSI INCLUDES:    */
/*                   */
/*///////////////    */
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>


/*/////////////////// */
/*                    */
/* MADELINE INCLUDES: */
/*                    */
/*/////////////////// */
#ifndef DBF_INCLUDED
#include "dbf.h"
#endif

#include "tools.h"

#ifndef STRINGBUFFERSIZES_INCLUDED
#include "stringbuffersizes.h"
#endif

/*///////// */
/*          */
/* DEFINES: */
/*          */
/*///////// */

/*                                                                                           */
/* Default mate and offspring counts: Vectors of pointers are allocated in these             */
/* block sizes.  Vectors (i.e. pointers) to pointers are used so you can always              */
/* call realloc() if the default allocation is too small for a particular individual.        */
/* You can of course change the #defined block sizes depending on application.  For example, */
/* in human pedigrees, one mate and five offspring seems reasonable.  If we were modeling    */
/* oak trees instead, we might have hundreds of mates and thousands of offsprings --what a   */
/* a mess!  Fortunately, you are probably not going to model oak trees! (Note: if you were   */
/* really modeling large numbers of mates or offspring, you would have to change the         */
/* counters in the INDIVIDUAL structure from char to int anyway)                             */
/*                                                                                           */
#define DEFAULTMATECOUNT       1     /* default allocation of vector of pointers to mates    */
#define DEFAULTOFFSPRINGCOUNT  5     /* default allocation of vector of pointers to kids     */

#define UNAFFECTED  0
#define NOTAFFECTED 0
#define AFFECTED    1

#ifndef FALSE
#define FALSE       0
#endif
#ifndef TRUE
#define TRUE        1
#endif

#ifndef FEMALE
#define FEMALE 1
#endif
#ifndef MALE
#define MALE   0
#endif

#ifndef DEAD
#define DEAD   1
#endif
#ifndef ALIVE
#define ALIVE  0
#endif

/*                                                                  */
/* family membership (or class) definitions (based on FUSION study) */
/* FYI, These are only used for manipulating FUSION data, so you    */
/* can safely ignore these!                                         */
/*                                                                  */
#define UNCLASSIFIED     -9
#define CONTROL           0
#define PROBAND           1
#define FATHER            2
#define MOTHER            3
#define SIBLING           4
#define PROBANDSPOUSE     5
#define PROBANDOFFSPRING  6
#define SIBLINGSPOUSE     7
#define SIBLINGOFFSPRING  8
#define MAXCLASSES        8

/*
   Missing indicator used if a FIELD is missing.  This is ONLY used
   when a field is missing (note that zero is a valid field index):
*/
#ifndef MISSING
#define MISSING -9
#endif

#define DATEWIDTH    9


/*

   STRUCTURE DEFINITIONS

*/

/*                                                                                        */
/* An INDIVIDUAL models the relationships of an                                           */
/* individual in a pedigree: this is really the key                                       */
/* structure in this library.  A pointer to pointer                                       */
/* to INDIVIDUAL is used for mates and for offspring so that                              */
/* one can model multiple mates and of course multiple offspring.                         */
/* Note the counters na and nk for number of mates and offspring,                         */
/* respectively: if these exceed ma and mk (the maximum holders), then you have to        */
/* reallocate the a and k pointer-to-pointers.                                            */
/*                                                                                        */
/* Also, it is important to note that while you can keep track of                         */
/* the number of offspring a male sires with his mate, all the children                   */
/* should be tracked through the mother, so don't allocate the k pointer                  */
/* of males                                                                               */
/*                                                                                        */
/* Finally, a "next" pointer has been provided so that you can create a                   */
/* completely unsorted singly-linked list of individuals in a family prior to knowing the */
/* exact pedigree relationships                                                           */
/*                                                                                        */
/*  pointer to   pointer to                                                               */
/*     father   mother                                                                    */
/*         ^    ^                                                                         */
/*         |    |                                                                         */
/*        *f   *m                                                                         */
/*       +----------+ na: # of mates                                                      */
/*       |INDIVIDUAL|**a----> +-> pointer to 1st mate ("a" for "ai ren" in Mandarin)      */
/*       +----------+         |                       ( Mandarin Chinese, that is! )      */
/*       nk: # of kids      +-> pointer to 2nd mate                                       */
/*         |                etc.                                                          */
/*         |                                                                              */
/*       **k----> +-> pointer to 1st offspring ("k" for "kid")                            */
/*                |                                                                       */
/*                +-> pointer to second offspring                                         */
/*                |                                                                       */
/*               etc.                                                                     */
/*                                                                                        */
/* ("l" and "r" pointers not shown in schematic above)                                    */
/*                                                                                        */
typedef struct sINDIVIDUAL{

	char id[MAXIDWIDTH];    /* individual's ID                                                       */
	char female;            /* 1 if female, 0 if male, or MISSING                                    */
	char affected;          /* generic affection status: 1 if affected, 0 if not, or MISSING         */
	char dead;              /* 1 if dead, 0 if alive, or MISSING                                     */
	char proband;           /* proband status: 1 if proband, 0 if not                                */
	char clas;              /* individual's class (FOUNDER, SIBLING, ETC.)                           */
	char lclass;            /* Liability Class:                                                      */ 
	char twin;              /* mz, dz, and undetermined twin status                                  */
	int  sid;               /* sequential id no.                                                     */
	 
	char fid[MAXIDWIDTH];    /* father's ID       */
	struct sINDIVIDUAL *f;   /* pointer to father */
	char mid[MAXIDWIDTH];    /* mother's ID       */
	struct sINDIVIDUAL *m;   /* pointer to mother */
	
	struct sINDIVIDUAL *l;   /* left pointer for bst             */
	struct sINDIVIDUAL *r;   /* right pointer for bst            */
	char red;                /* red-black flag for red-black bst */
	
	/*                                                                                          */
	/* NOTE that ma and ka need to be multiples of MAXIMUMMATECOUNT and MAXIMUMOFFSPRINGCOUNT:  */
	/*                                                                                          */
	char ma;                /* maximum no. of mates that can be ref'd in current allocation     */
	char na;                /* number of mates                                                  */
	struct sINDIVIDUAL **a; /* pointer to pointer to mates                                      */
	char mk;                /* maximum no. of offspring that can be ref'd in current allocation */
	char nk;                /* number of offspring (kids)                                       */
	struct sINDIVIDUAL **k; /* pointer to pointer to kids                                       */

	PDBF db;                /* database containing the record of this person */
	UINT32 rci;             /* record index of this person in the database   */
    
	int  fl;                /* generic state flag                                                       */
	int included;           /* set to one when individual included in output                            */
	int genotypecount;       /* 0 by default; holds count of # markers for which person is typed        */
	
	int nw;                 /* an integer indicating the width of the subtree below this person         */
	int vlevel;             /* an integer indicating the vertical "level" of this individual            */
	double x;               /* x-coordinate of an individual in a drawing                               */
	double y;               /* y-coordinate of an individual in a drawing                               */
	int excluded;           /* set to one when user marks for exclusion (can still be __included__ !! ) */
    
	long dob;               /* date of birth stored as a Julian day */
	long dod;               /* date of death stored as a Julian day */
    
	int unconnected;        /* 0 by default, 1 if unconnected                              */
	int csp;                /* 0 by default, 1 if a childless spouse                       */
	int hasdata;            /* 0 by default, 1 if person "has data" in the current context */
	int haspdata;           /* 0 by default, 1 if person has "P" phenotype data            */
	int hasgdata;           /* 0 by default, 1 if person has "G" genotype data             */
	int counted;            /* 0 by default, 1 if already counted for bookkeeping purposes */
    
	int consanguinity;      /* 0 by default, 1 if consanguinity w/ some spouse                             */
    
	int drawn;              /* 0 by default, 1 if already drawn once, 2 if twice, etc.                     */
	int page;               /* 0 by default, page on which drawn if non-zero                               */

	struct sPEDIGREE *pd;   /* pd points back to the pedigree to which the individual belongs.             */
	                        /* This is NEW starting in version 0.92 and will simplify the code in a lot of */
	                        /* places if the code ever gets a complete overhaul                            */
	int foundinggroup;      /* The founding group to which an individual belongs                           */
	
	struct sNUCLEARFAMILY *pnf; /* Pointer to the nuclear family formed by this individual's parents           */
	struct sNUCLEARFAMILY *snf; /* Pointer to the nuclear family formed by this indivudual as a parent himself */
	
	unsigned char *fieldflag;   /* A vector of field flags: set to 1 to exclude phenotype/genotype information */
	unsigned char *integrity;   /* A vector of integrity flags: set to non-zero values indicating              */
	                            /* Mendelian inconsistencies                                                   */
	int visited;                /* flag: TRUE if marked as visited by a routine, else FALSE                    */
	
	UFLAG8 IsPrimaryFounder;    /* flag: TRUE if is a primary founder, else FALSE                              */
	
}INDIVIDUAL,*PINDIVIDUAL,**PPINDIVIDUAL;

/* 2002.04.04.ET ADDENDUM:                                          */
/*                                                                  */
/* NUCLEARFAMILY: Used for integrity checking and for enhanced      */
/*                pedigree drawing. Each individual can belong to   */
/*                TWO nuclear families: first, the nuclear family   */
/*                formed by his parents; and secondly, the nuclear  */
/*                family formed as a parent himself.                */
/*                                                                  */
/*                Thus, there is a _P_arents _N_uclear _F_amily     */
/*                pointer, "pnf", and a _S_elf _N_uclear _F_amily   */
/*                pointer, "snf", when the individual himself       */
/*                becomes a parent.                                 */
/*                                                                  */
/*                Referring to person "A" below:                    */
/*                                                                  */
/*       [F]---+---(M)                     Nuclear Family #1 con-   */
/*             |                           sists of F,M,S, and A.   */
/* NF #1  +----+----+                      This is A's "pnf".       */
/*        |         |                                               */
/*       (S)       [A]---+---(W)           Nuclear Family #2 con-   */
/*                       |                 sists of A,W, and O.     */
/*                       |       NF #2     This is A's "snf".       */
/*                      (O)                                         */
/*                                                                  */
/* Unused PNUCLEARFAMILY pointers are set to NULL.                  */
/*                                                                  */
typedef struct sNUCLEARFAMILY{

	/* Linked List Pointer to next */
	struct sNUCLEARFAMILY *next;
	
	/* Pointers to the father and mother */
	struct sINDIVIDUAL *f;
	struct sINDIVIDUAL *m;
	
	/* Integrity flags vector: vector length depends on no. of  */
	/* genotypes.                                               */
	char *ifv;
	

}NUCLEARFAMILY,*PNUCLEARFAMILY,**PPNUCLEARFAMILY;

/*                                                                                               */
/* FOUNDINGGROUP: A "founding group" consists of one ancestral founder and his or her spouses.   */
/*                These are the founders at the top of a V-shaped pedigree tree.  Usually it is  */
/*                just a single male-female pair.  But Madeline allows the possibility that it   */
/*                is a group consisting of a male with multiple wives, or a female with multiple */
/*                male mates.  There is no limit on the number, but can only draw about 10.      */
/*                                                                                               */
/*         OK:    ( )-----[X]-----( )     or    [ ]-----(X)-----[ ]     [ ] = male, ( ) = female */
/*                                                                       X  = index founder      */
/*                                                                                               */
/*                Note that the code currently simply does not handle the (presumably rare) case */
/*                where one or more of the mates of a founder with multiple mates also has more  */
/*                than one mate:                                                                 */
/*                                                                                               */
/*  NOT HANDLED:  [ ]-----( )-----[ ]-----( )  or similar configurations                         */
/*                                                                                               */
typedef struct sFOUNDINGGROUP{
	int idx;                /* Founding group number                            */
	PINDIVIDUAL f;          /* pointer to one founder                           */
	int rwidth;             /* right subtree width                              */
	int lwidth;             /* left subtree width                               */
	int vlevel;             /* vlevel (vertical positioning cue) for this group */
	
	double lowx;            /* lowest x-coordinate	 */
	double highx;           /* highest x-coordinate */
	double lowy;            /* lowest y-coordinate  */
	double highy;           /* highest y-coordinate */
	double width;           /* width                */
	double height;          /* height               */
	
}FOUNDINGGROUP,*PFOUNDINGGROUP,**PPFOUNDINGGROUP;


/*                                                    */
/* a PEDIGREE has a BST root node, h, for gathering   */
/* individuals in a pedigree, and a pointer           */
/* to the founding father and mother for use once the */
/* pedigree is reconstructed:                         */
/*                                                    */
typedef struct sPEDIGREE{

	char fid[MAXIDWIDTH];  /* family/pedigree identifier                 */
	int n;                 /* number of people in family/pedigree        */
	int ex;                /* number of people excluded                  */
	int nun;               /* number of unconnected people               */
	int ncsp;              /* number of childless spouses                */
	int nvi;               /* number of virtual (dummied-in) individuals */
	
	char red;              /* for red/black balanced BST tree implementation */
	
	int nff;               /* number of *PRIMARY* founding fathers  */
	PPINDIVIDUAL ff;       /* vector of *PRIMARY* founding fathers  */
	int nfm;               /* number of *PRIMARY* founding mothers  */
	PPINDIVIDUAL fm;       /* vector of *PRIMARY* founding mothers  */
	int nfg;               /* number of *PRIMARY* founding groups   */
	PFOUNDINGGROUP fg;     /* vector of *PRIMARY* founding groups   */
		
	struct sPEDIGREE *l;   /* left node                                  */
	struct sPEDIGREE *r;   /* right node                                 */
	int fl;                /* generic flag for holding state information */
	int sid;               /* sequential id no.                          */
	PINDIVIDUAL h;         /* root node of INDIVIDUAL binary sorted tree */
	PINDIVIDUAL a;         /* root node of added-on INDIVIDUALs          */
	
	int exclude;           /* exclude flag: 0 by default */
	
	PNUCLEARFAMILY nfhead; /* Head of singly-linked list of NUCLEARFAMILIES */
	PNUCLEARFAMILY nftail; /* Tail of singly-linked list of NUCLEARFAMILIES */
	
	/* 2003.05.05.ET Addenda: Used to track pedigree complexity                  */
	int NumberOfFounders;     /* Number of founders (including primary founders) */
	int NumberOfNonFounders;  /* Number of non-founders                          */
	
	/* 2003.10.22.ET Addenda: Number of Affected individuals */
	int NumberOfAffected;
	// int NumberOfAffectedPairs;
	// int NumberOfAffectedSibPairs;
	
}PEDIGREE,*PPEDIGREE,**PPPEDIGREE;

/*
   
   FUNCTION PROTOTYPES
   
*/
PINDIVIDUAL NewIndividual(void);
void PDErrorReport(char *s,...);
void Warning(char *s,...);
PPEDIGREE   NewPedigree(int n);
PINDIVIDUAL IndividualLookup(PINDIVIDUAL h,const char *id);
/*                                          */
/* Functions for red-black INDIVIDUAL tree: */
/*                                          */
PINDIVIDUAL IndividualTreeHeadNode(const char *id,PDBF db,UINT32 rci);
PINDIVIDUAL IndividualTreeAddNode(PINDIVIDUAL pn,PPINDIVIDUAL ppi,const char *id,PDBF db,UINT32 rci,int sw);

/*                                         */
/* 19990416 ET MODIFIED IndividualInstall: */
/*                                         */

/*                                                                             */
/* Normally InstallIndividual() is used for creating balanced INDIVIDUAL bsts: */
/*                                                                             */
PINDIVIDUAL InstallIndividual(PPEDIGREE pd,const char *id,PDBF db,UINT32 rci);
/*                                                                          */
/* The following is used only for installing virtual parents when right in  */
/* the middle of traversing a tree (so node rotations cannot be permitted): */
/*                                                                          */
PINDIVIDUAL UnoptomizedInstallIndividual(PPEDIGREE pd,const char *id,PDBF db,UINT32 rci);
/*                         */
/* Free an INDIVIDUAL bst: */
/*                         */
void FreeIndividualBST(PINDIVIDUAL);
/*                                                                    */
/* 980515 ET: Lookup individual based on family id and individual id: */
/*                                                                    */
void IndividualLookup2(PPEDIGREE hpd,char *fid,char *id,PPPEDIGREE ppd,PPINDIVIDUAL ppi);

/*                                                                                                         */
/* Functions to build PedigreeTree:                                                                        */
/*                                                                                                         */
/*                                                                                                         */
/* OBSOLETE: PPEDIGREE AddToBinaryPedigreeTree(PPEDIGREE pd,char *fid,char *id,PDBF db,UINT32 rci,int sw); */
/*                                                                                                         */
PPEDIGREE BinaryPedigreeTreeHeadNode(char *fid,char *id,PDBF db,UINT32 rci,PPINDIVIDUAL ppi);
PPEDIGREE AddIndividualToBinaryPedigreeTree(PPEDIGREE pd,const char *fid,const char *id,PDBF db,UINT32 rci,PPINDIVIDUAL ppi,int sw);
/*                     */
/* FOR DEBUGGING ONLY: */
/*                     */
void PedigreeTreePrint(PPEDIGREE pd);
void IndividualTreePrint(PINDIVIDUAL p);

/*                                                                          */
/* PedigreeRightRotation() and PedigreeLeftRotation()                       */
/* implement single rotations for the red/black balanced BST pedigree tree: */
/*                                                                          */
PPEDIGREE PedigreeRightRotation(PPEDIGREE h);
PPEDIGREE PedigreeLeftRotation(PPEDIGREE h);

void TraverseBinaryPedigreeTree(PPEDIGREE p,void (*func)(PPEDIGREE p));
int  IsEmptyOrMissing(char *s);
void EliminateFloaters(PPEDIGREE pd);
void FreeBinaryPedigreeTree(PPEDIGREE pd);
PPEDIGREE SeekPedigree(PPEDIGREE pd,char *fid);
void TraverseRangeInBinaryPedigreeTree(PPEDIGREE pd,char *stt,char *end,void (*func)(PPEDIGREE p));
void TraverseIndividualBST(PINDIVIDUAL p,void (*visit)(PINDIVIDUAL));
int  IndividualIsProband(PINDIVIDUAL p);

/*                               */
/* BST Rotation for Individuals: */
/*                               */
PINDIVIDUAL IndividualRightRotation(PINDIVIDUAL h);
PINDIVIDUAL IndividualLeftRotation(PINDIVIDUAL h);

/*                                                                        */
/* Functions for Removing select individuals from a PINDIVIDUAL BST tree: */
/*                                                                        */
/*                                                                        */
void        RemoveIndividualsFromBST(PPEDIGREE pd,PPINDIVIDUAL i,int (*criteria)(PINDIVIDUAL) );
int         RemoveIndividual(PPINDIVIDUAL i,int (*remove)(PINDIVIDUAL));
void       *RandomlyChoose(void *one,void *theother);
void        FreeSingleIndividual(PINDIVIDUAL p);

/*                                                                    */
/* Functions for Removing select pedigrees from a PPEDIGREE BST tree: */
/*                                                                    */
int  RemovePedigree(PPPEDIGREE i,int (*remove)(PPEDIGREE));
void FreeSinglePedigree(PPEDIGREE pd);
/*                                   */
/* Functions for building pedigrees: */
/*                                   */
void ConnectIndividual(PPEDIGREE pd,PINDIVIDUAL p);
void JoinIndividualAndParents(PINDIVIDUAL p);
/*                                                                    */
/* Function added for connecting inserted parents when the parent, p, */
/* is a FUSION study 100 or 400 sibling:                              */
/*                                                                    */
/* JoinIndividualAndParents() represents a packaging of originally    */
/* in-lined code from ConnectIndividual() into a separate function:   */
/*                                                                    */
void ConnectInsertedParent(PPEDIGREE pd,PINDIVIDUAL p);

/*                                     */
/* Pedigree drawing support functions: */
/*                                     */
void CalculatePedigreeWidth(PPEDIGREE pd);
int  PedigreeWidth(PINDIVIDUAL p);
void CalculatePedigreeVLevel(PPEDIGREE pd);
int  PedigreeVLevel(PINDIVIDUAL p);
void SetFoundingGroups(PPEDIGREE pd);
PFOUNDINGGROUP NewFoundingGroupVector(int fgcount);
int DetermineFoundingGroup(PINDIVIDUAL p,PPEDIGREE pd);

/* 2002.04.05.ET: */
void DetermineFounders(PPEDIGREE pd,PINDIVIDUAL p);

PNUCLEARFAMILY NewNuclearFamily(PINDIVIDUAL pf,PINDIVIDUAL pm,int ng);
void FreeNuclearFamily(PNUCLEARFAMILY p);
void FreeNuclearFamilyLinkedList(PNUCLEARFAMILY head);
void FreeIndividual(PINDIVIDUAL p);

/* 2002.04.25.ET & 2002.06.11.ET: */
unsigned char *NewFieldFlagVector(int FieldCount);
unsigned char *NewIntegrityVector(int FieldCount);


#define PEDIGREE_INCLUDED


