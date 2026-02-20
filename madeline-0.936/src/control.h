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
/* "control.h" is part of the Madeline software distribution.                        */
/* Written by Edward H. Trager.                                                      */
/*                                                                                   */
/* COPYRIGHT (C) 2000 THE REGENTS OF THE UNIVERSITY OF MICHIGAN,                     */
/* Ann Arbor, Michigan, USA.  ALL RIGHTS RESERVED.                                   */
/*                                                                                   */
/* Copyright (C) 1996-2000 by Edward H. Trager and the FUSION Study Group            */
/* at the University of Michigan School of Public Health in Ann Arbor,               */
/* Michigan, USA.                                                                    */
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

/*                                           */
/* control.h                                 */
/*                                           */
/* init: 19990316 ET                         */
/*                                           */
/* -> Defines CONTROL structure for Madeline */
/*                                           */


/*//////////////////////////////// */
/*                                 */
/* MADELINE INCLUDES:              */
/*                                 */
/* -> pedigree.h includes:         */
/*         - variablesandarrays.h  */
/*         - dbf.h which includes: */
/*           - portable.h          */
/*           - marker.h            */
/*                                 */
/* -> map.h defines PMARKERMAP     */
/*                                 */
/*//////////////////////////////// */
#ifndef PEDIGREE_INCLUDED
#include "pedigree.h"
#endif

#ifndef MAP_INCLUDED
#include "map.h"
#endif

/*                                                             */
/* Indices for the g->ivar[] array                             */
/* (used to simplify global access to the internal variables): */
/* IVI just stands for internal variable index:                */
/*                                                             */
/* 19990630 ET ADDENDA:                                        */
/*                                                             */
#define AFFECTIONSTATUSFIELD_IVI  0
#define CHROMOSOMEFIELD_IVI       1
#define DATEOFBIRTHFIELD_IVI      2
#define DATEOFDEATHFIELD_IVI      3
#define DEATHSTATUSFIELD_IVI      4
#define DZTWINFIELD_IVI           5
#define FAMILYIDFIELD_IVI         6
#define FATHERIDFIELD_IVI         7
#define GENDERFIELD_IVI           8
#define INDEXCASEFIELD_IVI        9
#define INDIVIDUALIDFIELD_IVI    10
#define LIABILITYCLASSFIELD_IVI  11
#define MARKERFIELD_IVI          12
#define MOTHERIDFIELD_IVI        13
#define MZTWINFIELD_IVI          14
#define ORDINALFIELD_IVI         15
#define POSITIONFIELD_IVI        16
#define ALLELE1FIELD_IVI         17
#define ALLELE2FIELD_IVI         18
#define FEMALEPOSITIONFIELD_IVI  19
#define MALEPOSITIONFIELD_IVI    20
#define TERMINATOR_IVI           21


/*                                                                 */
/* DUALCOUNTER: stores two counters, one int for inclusions (in),  */
/*            one int for exclusions.  This is used for keeping    */
/*            track of things like number of pedigrees included    */
/*            and excluded, number of individuals included and     */
/*            excluded, number of sib pairs included and excluded, */
/*            etc.                                                 */

typedef struct sDUALCOUNTER{
	int in;                    /* number included */
	int ex;                    /* number excluded */
}DUALCOUNTER,*PDUALCOUNTER;


/*                                         */
/* CORECOUNTERS: core bookkeeping counters */
/*               used in Madeline          */
/*                                         */

typedef struct sCORECOUNTERS{

	DUALCOUNTER pedg;  /* for pedigrees                                                               */
	DUALCOUNTER sibp;  /* for sibpairs (when appropriate -- i.e., when dealing with nuclear pedigrees */

	DUALCOUNTER din;  /* data included individuals: individuals w/ data in database   */
	DUALCOUNTER dno;  /* individuals in database without data: p->nodata individuals  */
	DUALCOUNTER dex;  /* data excluded individuals                                    */
	DUALCOUNTER csp;  /* childless spouses                                            */
	DUALCOUNTER flt;  /* floaters: completely unattached individuals                  */
	DUALCOUNTER vir;  /* for virtual individuals (added by Madeline, not in database) */

}CORECOUNTERS,*PCORECOUNTERS;



/*                                                       */
/* STREAMS structure: stores FILE * streams for Madeline */
/*                                                       */

typedef struct sSTREAMS{
	
	FILE *fp;      /* primary output file for the various WRITE formats         */
	FILE *fpar;    /* associated parameter file, if present                     */
	FILE *fpar2;   /* a second associated parameter or control file, if present */
	FILE *log;     /* log file                                                  */
	FILE *detail;  /* detail file                                               */
	FILE *prn;     /* Postscript pedigree drawing file stream                   */
	FILE *graph;   /* Postscript graph drawing file stream                      */
	FILE *err;     /* serious data error conditions log file                    */
	FILE *command; /* Commands entered by the user log file                     */
	
}STREAMS,*PSTREAMS;


/*                                                                                 */
/* DRAWINGPARAMETERS:                                                              */
/*                                                                                 */
/*                                                                                 */
/*           |<-- HorizontalInterval --->|                                         */
/*           |                           |                                         */
/*        +-----+                      _____                                       */
/*        |     |                     /     \                                      */
/*        |  x  |---------+----------|   x   |                                     */
/*        |     |         |    ^      \     /                                      */
/*        +-----+         |    |       -----                                       */
/*                        | VerticalDrop1                                          */
/*        xxxxxxx         |    |      xxxxxxx                                      */
/*        xxxxxxx         |    |      xxxxxxx                                      */
/*        xxxxxxx         |    |      xxxxxxx                                      */
/*                        |    v                                                   */
/* ----------+------------+--------------+---------------------------+             */
/*           |       ^                   |                           |             */
/*           | VerticalDrop2             |                           |             */
/*         _____     |                +-----+                      _____           */
/*        /     \    v                |     |                     /     \          */
/*       |   x   |  ---               |  x  |                    |   x   |         */
/*        \     /                     |     |                     \     /          */
/*         -----                      +-----+                      -----           */
/*           |           |                                                         */
/*           |<---Half-->|      xxxxxxxxxxxxxxxxxxx                                */
/*           | HZInterval|      xxxxxxxxxxxxxxxxxxx                                */
/*           |           |      xxxxxxxxxxxxxxxxxxx                                */
/*                              |                 |                                */
/*                              |<-- CellWidth -->|                                */
/*                                                                                 */
typedef struct sDRAWINGPARAMETERS{

	char   UnitOfMeasure;      /* unit of measure CENTIMETERS,MM,INCHES - Use CM          */
	double IconRadius;         /* symbol radius                                           */
	double IconDiameter;       /* symbol diameter                                         */
	double HorizontalInterval; /* Horizontal interval between individuals                 */
	double HalfHZInterval;     /* Half the horizontal interval                            */
	double CellWidth;          /* Standard label cell width                               */
	double TickLength;         /* tick length                                             */
	double VerticalDrop1;      /* vertical drop from parent line to offspring line        */
	double VerticalDrop2;      /* vertical drop from offspring line to icon symbol center */
	UFLAG8 Orientation;        /* drawing orientation                                     */
	UFLAG8 InColor;            /* drawing color                                           */
	UFLAG8 ReverseShading;     /* Reverse the icon shading B->W => W->B :                 */
	UFLAG8 DividedDrawings;    /* drawings divided into founding group trees              */
	UFLAG8 PaperSize;          /* paper size                                              */
	double PageHeight;         /* paper height                                            */
	double PageWidth;          /* paper width                                             */
	double PageMargin;         /* paper margin                                            */
	UFLAG8 CrossOverTags;      /* Show crossover tags?                                    */
	
}DRAWINGPARAMETERS,*PDRAWINGPARAMETERS;



/*                                                                           */
/* CONTROL structure: stores the set of core objects being manipulated       */
/*                    by Madeline.  This includes the database, db, the      */
/*                    binary pedigree tree, pd, the output FILE pointers,    */
/*                    and the basic stat counters used for bookkeeping, etc. */
/*                                                                           */

typedef struct sCONTROL{
	
	PDBF         db;             /* the pedigree database              */
	PPEDIGREE    pd;             /* root node of the pedigree BST tree */
	
	PDBF         graphdb;        /* the graph database                 */
	
	PPEDIGREE    cpd;            /* current pedigree   */
	PINDIVIDUAL  cp;             /* current individual */
	
	PMARKER      m;              /* used for saving a previous PMARKER vector              */
	int          ms;             /* used for saving a previous PMARKER vector: vector size */
	
	PDBF         mdb;            /* the map database                     */
	PMARKERMAP   mp;             /* root node of the marker map BST tree */
	
	STREAMS      streams;        /* Madeline's output streams */
	CORECOUNTERS counters;       /* Madeline's core counters  */
	
	int          psid;           /* pedigree sequential id   */
	int          isid;           /* individual sequential id */
	
	CORECOUNTERS tally;          /* for VIEW, EXCLUDE, and UNEXCLUDE tallies       */
	CORECOUNTERS nuclear;        /* for nuclear family-based and sibpair counting  */
	PPEDIGREE    lastpd;         /* the last pedigree examined (by a VIEW command) */
	
	int          OutputFormat;   /* output format type */
	
	UFLAG8       AutoDetect;     /* auto detect field type as G vs. P */
	UFLAG8       AutoExclude;    /* auto exclude pedigrees            */
	UFLAG8       Eliminate;      /* eliminate floaters                */
	UFLAG8       Silent;         /* silent mode                       */
	UFLAG8       Verbose;        /* verbose mode                      */
	UFLAG8       SaveMarkerInfo; /* save the PMARKER vector           */
	UFLAG8       HtmlHelp;       /* use html help if true.            */
	UFLAG8       LabelCreatedVirtuals; /* Label virtual individuals having random IDs created by Madeline  */
	UFLAG8       HighlightRows;        /* Highlight alternate groups of 5 rows of data in color on drawings */
	UFLAG8       AutoCheckInheritance; /* Automatically check inheritance on OPENing a pedigree table.  Default: ON */
	UFLAG8       ConsoleHighlights;    /* Console highlighting (bold or color text). Default: ON */
	UFLAG8       MapDetails;           /* View map details (inter-marker distances, recombination fractions). Default: OFF */
	
	int          HasSyntaxErrors;      /* true if syntax errors have occurred: also count of said errors             */
	int          HasWarnings;          /* true if warnings occurred: also count of number of warnings                */
	int          HasSevereWarnings;    /* true if severe warnings occurred: also count of number of severe warnings  */
	int          HasErrors;            /* true if a serious error has been detected: ALSO COUNTS NO. OF ERRORS       */
	int          HasInheritanceErrors; /* true if inheritance inconsistencies have been detected: also count thereof */
	UFLAG8       ResetPrompt;          /* true if we should reset the prompt on next round                           */
	UFLAG8       lang;           /* display language (multi-lingual support is limited)                */
	UFLAG8       Haplotype;      /* display genotypes formatted as haplotypes using "|" instead of "/" */
	int          sort;           /* flag indicating a sort expression is present                       */

	DRAWINGPARAMETERS d;                 /* Pedigree drawing parameters             */
	PVARIABLE    ivar[TERMINATOR_IVI+1]; /* array of pointers to internal variables */
	
	char        *MadelineSystem; /* MADELINESYSTEM environment variable */
	
	UFLAG8       FusionSupport;  /* true if supporting FUSION customizations */
	
	char         delimiter;      /* current delimiter for tables and other output */
	
}CCONTROL,*PCONTROL;

#define CONTROL_INCLUDED

