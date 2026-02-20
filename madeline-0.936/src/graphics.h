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
/* "graphics.h" is part of the Madeline software distribution.                       */
/* Written by Edward H. Trager.                                                      */
/*                                                                                   */
/* COPYRIGHT (C) 2000 THE REGENTS OF THE UNIVERSITY OF MICHIGAN,                     */
/* Ann Arbor, Michigan, USA.  ALL RIGHTS RESERVED.                                   */
/*                                                                                   */
/*                                                                                   */
/* Portions of this code were originally part of "gprint.h" copyright                */
/* (c) 1995 by Edward H. Trager                                                      */
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

/*                                                                         */
/* graphics.h.  Graphics.h and graphics.c replace gprint.h/.c in           */
/*              new versions of Madeline with PostScript graphics.         */
/*              Unlike gprint.c which originated before Madeline           */
/*              and was designed as a much more generic graphic report     */
/*              library (even though it was limited to PCL and HP/GL),     */
/*              graphics.c has been written for use in Madeline            */
/*              and therefore contains more specialized functionality.     */
/*              Some of the generality built into the structures like CELL */
/*              and CELLARRAY are retained here, even though the full      */
/*              flexibility with which those structures were originally    */
/*              designed may not be employed in Madeline at the current    */
/*              time.                                                      */
/*                                                                         */
/* 19990427 ET initiated.                                                  */
/*                                                                         */
/*                                                                         */
/* ANSI INCLUDES:                                                          */
/*                                                                         */
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

/*                    */
/* MADELINE INCLUDES: */
/*                    */
#ifndef NDBF_INCLUDED
#include "ndbf.h"
#define NDBF_INCLUDED
#endif

#ifndef DATE_INCLUDED
#include "date.h"
#endif

#ifndef POSTSCRIPT_INCLUDED
#include "postscript.h"
#endif

#ifndef LOGS_INCLUDED
#include "logs.h"
#endif

#ifndef FIELDFLAGS_INCLUDED
#include "fieldflags.h"
#endif

#ifndef TOKENTYPES_INCLUDED
#include "tokentypes.h"
#endif
/*                                       */
/*                                       */
/* Global pointer to control structure,  */
/* g:                                    */
/*                                       */
/*                                       */
extern PCONTROL g;

/*                                                    */
/* 19990427 ET Printer drawing orientation constants: */
/*                                                    */
#define PORTRAITORIENTATION  1
#define LANDSCAPEORIENTATION 2
#define AUTOMATICORIENTATION 3
#define MULTIPAGEORIENTATION 4

/*                          */
/* Justification constants: */
/*                          */
#define LEFT   0
#define CENTER 1
#define RIGHT  2

/*                                        */
/* Cell border constants: NONE is default */
/*                                        */
#define NONE       0
#define NOLINE     0
#define THINLINE   1
#define NORMLINE   2
#define THICKLINE  3
#define DOUBLELINE 4
#define MAXLINESTYLES 5
 
/*                                         */
/* Text style constants: normal is default */
/*                                         */
#define NORMAL 0
#define BOLD   1
#define ITALIC 2
#define BOLDITALIC 3

/*                               */
/* Unit constants: CM is default */
/*                               */
#define CENTIMETERS  0
#define CM           0
#define MILLIMETERS  1
#define MM           1
#define INCHES       2
#define IN           2
#define POINTS       3
#define PT           3

/*                       */
/* Text color constants: */
/*                       */
#define TCBLACK    0
#define TCWHITE    1

/*                               */
/* Background shading constants: */
/*                               */
#define BCWHITE    0
#define BCBLACK  100

/*                                                                                    */
/* DATASOURCE quick formatting                                                        */
/* directive constants:                                                               */
/*                                                                                    */
/* be sure to check the bit masks first:                                              */
/*                                                                                    */
#define CUSTOMFORMAT 128    /* use the custom format instead                          */
#define DOLLARSIGN    64    /* add a dollar sign                                      */
#define PERCENTSIGN   32    /* add a percent sign                                     */
#define TEXTDATA      16    /* output a text string                                   */
#define INTEGERDATA  256    /* output a number with no decimal places                 */
#define NODATA       512    /* a DATASOURCE with no data: just print fmt with no args */
#define ONEBEFORE   1024    /* pull data from one record before the current record:   */
#define DATEDATA    2048    /* output a (long) julian day as a date                   */
			    /* --this is useful for printing footers and footnotes on tables               */
#define GENOTYPEDATA 4096   /* genotype data in "nnn/nnn" form                        */
/*                                                                                    */
/* then check least significant bits for formatting of floating point                 */
/* numbers:                                                                           */
/*                                                                                    */
#define ONEDECIMAL     1    /* output a number with 1 decimal place                   */
#define TWODECIMAL     2    /* output a number with two decimal places                */
#define THREEDECIMAL   3    /* output a number with three decimal places              */
#define FOURDECIMAL    4    /* output a number with four decimal places               */
#define FIVEDECIMAL    5    /* output a number with five decimal places               */

#define MAXSTRINGLENGTH  256 /* maximum length of a string in a TEXTLABEL */
#define MAXLONGSTRING   2500 /* maximum length of a long string           */

/*                                 */
/* CELL border indexing constants: */
/*                                 */
#define MAXBORDERS   4
#define LEFTBORDER   0
#define TOPBORDER    1
#define RIGHTBORDER  2
#define BOTBORDER    3
#define BOTTOMBORDER 3

/*                 */
/* Font constants: */
/*                 */
#define TIMESROMAN 1
#define ARIAL      2

/*                           */
/* CELL anchoring constants: */
/*                           */
#define TOPLEFT      1
#define CENTERTOP    2
#define CENTERCENTER 3

/*                           */
/* PS_PEDIGREE_PROC_NAME_FMT */
/*                           */
#define PS_PEDIGREE_PROC_NAME_FMT "Pedigree-%s_FoundingGroup-%05i"

/*////////////////////// */
/*                       */
/* Structure Definitions */
/*                       */
/*////////////////////// */


/*                                                         */
/* A DATASOURCE contains a pointer to a currently          */
/* opened database, dbf, an integer for the field          */
/* number, fn, an integer for using a simple, pre-defined  */
/* quick string formatting constant, qfmt, and an optional */
/* pointer to a character string containing more complex   */
/* "printf()"-formatting directives,fmt.  If qfmt is -1,   */
/* then fmt better be a non-NULL string containing the     */
/* formatting directives                                   */
/*                                                         */
typedef struct sDATASOURCE{
	
	PDBF   dbf;     /* source data table                                          */
	UINT16 fn;       /* field number                                              */
	UINT16 qfmt;  /* quick formatting -see constants above                        */
	char  *fmt;    /* custom printf() format used when qfmt includes CUSTOMFORMAT */

} DATASOURCE,*PDATASOURCE;


/*                                                    */
/* A TEXTLABEL holds a string of text along with      */
/* the designated font style and size.  It will       */
/* be printed in the current font or the font         */
/* defined in the CELL of which the TEXTLABEL is      */
/* a member.                                          */
/*                                                    */
/* Note that TEXTLABELs have now been made data-aware */
/* by inclusion of a DATASOURCE pointer, d.           */
/*                                                    */
typedef struct sTEXTLABEL{
	
	char *t;                /* the text to print         */
	int s;                  /* style: NORMAL, BOLD, etc. */
	int z;                  /* size in points            */
	
	PDATASOURCE d;  /* NULL if text is fixed constant        */
		
} TEXTLABEL,*PTEXTLABEL;


/*                                                  */
/* A CELL defines the rectangular block in which    */
/* one or more TEXTLABELS are printed.  The block   */
/* may have bordering lines, b[0]-b[3], a           */
/* shaded background, bc, and a text color, tc, of  */
/* black or white.                                  */
/* The CELL has a font f and justification j which  */
/* apply to all TEXTLABELs present, if any.         */
/*                                                  */
/*                                                  */
/* Border indexing:   +------1------+               */
/* --use LEFTBORDER,  |             |               */
/*   TOPBORDER,       0             2               */
/*   RIGHTBORDER,     |             |               */
/*   and BOTTOMBORDER +------3------+               */
/*                                                  */
typedef struct sCELL{

	int u;                  /* unit of measure                                                      */
	double x,y;             /* coordinates of anchor (may be LEFTTOP, CENTERTOP, CENTERCENTER, etc. */
	char anchor;            /* anchor : LEFTTOP, CENTERTOP, CENTERCENTER                            */
	double xex,yex;         /* x and y extents                                                      */
	double m;               /* margins for left or right justif. text                               */
	
	int b[MAXBORDERS];      /* border attributes                  */
	int tc;                 /* text color: TEXTBLACK or TEXTWHITE */
	int bc;                 /* background shading (0 to 100%)     */
	int j;                  /* justification                      */
	int f;                  /* font identifier                    */
	
	int n;                  /* number of TEXTLABELS   */
	PTEXTLABEL t;           /* pointer to text labels */
		
} CELL,*PCELL;

/*                                              */
/*                                              */
/* A CELLARRAY encapsulates an array of CELLs   */
/* and the count of the number of cells present */
/*                                              */
typedef struct sCELLARRAY{
	
	int n;
	PCELL c;

} CELLARRAY,*PCELLARRAY;


/*                                                             */
/*                                                             */
/* A REPORT encapsulates all the elements of a standard        */
/* publication-quality report table having some or all         */
/* of the following elements:                                  */
/*                                                             */
/*                                                             */
/* +--                                             --+         */
/* | +---------------------------------+             |         */
/*   | page header at very top of page |                       */
/*   +---------------------------------+                       */
/*                                                             */
/*                                                             */
/*                                                             */
/*             +-----------------------+                       */
/*             | title of report/table |                       */
/*             +-----------------------+                       */
/*                                                             */
/*                                                             */
/*   +---------------------------------------------+           */
/*   | header cells |    |               |         |           */
/*   |              |    +---------------+         |           */
/*   |              |    |       |       |         |           */
/*   |              |    |       |       |         |           */
/*   +---------------------------------------------+           */
/*                .                                            */
/*   one or more templates for the actual detail               */
/*   lines and summary lines of the report:                    */
/*                .                                            */
/*                .                                            */
/*   +---------------------------------------------+           */
/*   |detail template    |       |       |         |           */
/*   +---------------------------------------------+           */
/*                .                                            */
/*                .                                            */
/*                .                                            */
/*   +---------------------------------------------+           */
/*   |summary template   |       |       |         |           */
/*   +---------------------------------------------+           */
/*                .                                            */
/*                .                                            */
/*                .                                            */
/*   +---------------------------------------------+           */
/*   |footer cells  |    |       |       |         |           */
/*   |              |    |       |       |         |           */
/*   +---------------------------------------------+           */
/*                                                             */
/*                                                             */
/*                                                             */
/*   +------------------------------------+                    */
/*   | page footer at very bottom of page |                    */
/* | +------------------------------------+          |         */
/* +--                                             --+         */
/*                                                             */
/*  Most of the elements of a report are CELLs or              */
/*  CELLARRAYs containing pointers to CELLs.  In addition,     */
/*  a REPORT also has x,y to represent the starting            */
/*  coordinates of the body of the report (same as upper       */
/*  left coordinate of the header) and xex,yex to represent    */
/*  the dimension of the body of the report.  The number of    */
/*  detail lines in the report can therefore be calculated     */
/*  as the REPORT's y-extent minus the header's y-extent minus */
/*  the footer's y-extent, then divide by the y-extent of one  */
/*  line of the report (this is true for most simple reports:  */
/*  it may not be true for more complex page compositions)     */
/*                                                             */
/*  Pointers are used for most of the elements of the report,  */
/*  which can be compared to NULL as long as you adhere to the */
/*  convention of instantiating reports themselves as run-time */
/*  objects using NewReport();                                 */
typedef struct sREPORT{

	/*                                                                           */
	/* This first group (u,x,y,xex,yex,m) are to be used as                      */
	/* report-global variables.  u indicates the unit for the                    */
	/* report. x and y indicate the start of the body of the                     */
	/* report which is the first detail line of the report AFTER                 */
	/* the header: it's done this way because you can then print                 */
	/* a page of the report, incrementing the y-coordinate of the                */
	/* r->l line array as you go along for each line, then reset                 */
	/* the r->l y-coordinate back to r->y for printing the next                  */
	/* page of the report.  Similarly, xex and yex indicate the                  */
	/* width and height of the body of the report, so and one can                */
	/* quickly determine lines per page from r->yex:                             */
	/*                                                                           */
	int u;             /* unit                                                   */
	double x,y;        /* start of the body (i.e., the first line of a report)   */
	double xex,yex;    /* horizontal width and y-depth of the body of the report */
	double CellMargin; /* margin offset for cells                                */

	PCELL PageHeader;  /* page header   */
	PCELL Title;       /* report title  */
	PCELLARRAY Header; /* report header */
	
	int nl;            /* number of line templates            */
	PCELLARRAY Line;   /* array of one or more line templates */
	
	PCELLARRAY Footer; /* report footer */
	PCELL PageFooter;  /* page footer   */
	
	int fl;            /* a general-purpose flag for report control 970708 ET */
	int page;          /* report's current page number                        */
		
} REPORT,*PREPORT;


/*                                                          */
/* PEDIGREEDRAWING                                          */
/*                                                          */
/* This is new to Madeline.  A pedigree drawing in Madeline */
/* consists of some or all of the following elements:       */
/*                                                          */
/*+------------                                             */
/*|       ^                                                 */
/*|       |                                                 */
/*|     Margin (same on all sides for simplicity)           */
/*|       |                                                 */
/*|       v                                                 */
/*   +---         <--  xex (x extant) -->         ---+      */
/*   |                                               |      */
/*   |               +---------------+               |      */
/*                   | Title (CELL)  |                      */
/*                   +---------------+                      */
/*      +---------+                                         */
/*      | Legend  |                                  ^      */
/*      |  (CELL) |                                  |      */
/*      +---------+                                         */
/*                                                   y      */
/*                       [ ]-+-( )                   e      */
/*                           |                       x      */
/*                    +------+------+                       */
/*                    |      |      |                y      */
/*                   [ ]    [ ]    ( )                      */
/*                                                   e      */
/*                +------+                           x      */
/*                |Indi- |                           t      */
/*                |vidual|                           a      */
/*                |(CELL)|                           n      */
/*                +------+                           t      */
/*                                                          */
/*                                                   |      */
/*                                                   |      */
/*                                                   v      */
/*                                                          */
/*  --> PageHeader and PageFooter just as in REPORT         */
/*                                                          */
/*                                                          */
typedef struct sPEDIGREEDRAWING{

	int u;              /* unit (always CENTIMETER for now)                     */
	double x,y;         /*                                                      */
	double xex,yex;     /* horizontal width and y-depth of the pedigree drawing */
	double CellMargin;  /* global margin offset for cells                       */

	PCELL PageHeader;   /* page header                */
	PCELL Title;        /* drawing title              */
	PCELL Legend;       /* Main or key legend         */
	PCELL Individual;   /* cell for the INDIVIDUALs   */
	PCELL PageFooter;   /* page footer                */
	
	int page;           /* Current page number of this drawing */
		
} PEDIGREEDRAWING,*PPEDIGREEDRAWING;

/*                      */
/* NewXXXX() Functions: */
/*                      */
PCELL NewCell(int n);
PPEDIGREEDRAWING NewPedigreeDrawing(int n);
PCELL NewAssignedCell(int u,double x,double y,double xex,double yex,double m,
		      int bleft,int btop,int bright,int bbot,
		      int tc,int bc,int j,int f,
		      int n, ... );
PCELL AssignCellText(PCELL c,int n, ... );
PDATASOURCE NewDataSource(PDBF dbf,UINT16 fn,UINT16 qfmt,char *fmt);
PTEXTLABEL NewTextLabel(int n);

/* */
/* */
/* */
void SetPostScriptEnvironment(PCONTROL c,int LegendFontSize,int CellFontSize);
void SetPostScriptPageEnvironment(PCONTROL c,char *DrawingTitle,char *PostScriptPage, int PageOrdinal);

/*                */
/* PrintIconCell: */
/*                */
void PrintIconCell(FILE *fp,PCELL c,PINDIVIDUAL p);

/*               */
/* Line drawing: */
/*               */
void AbsoluteLine(FILE *fp,double sttx,double stty,double endx,double endy);
void HorizontalLine(FILE *fp,double dx,double dy,double dlen);
void DoubleHorizontalLine(FILE *fp,double dx,double dy,double dlen);
void HorizontalGrayLine(FILE *fp,double dx,double dy,double dlen,int width,int uom,double density);
void VerticalLine(FILE *fp,double dx,double dy,double dlen);
void VerticalDrop1(FILE *fp,double dx,double dy);
void VerticalDrop2(FILE *fp,double dx,double dy);
void VerticalDrop2MinusIconRadius(FILE *fp,double dx,double dy);
void DrawIndividual(FILE *fp,PINDIVIDUAL p,double px,double py,PCELL cell,PCONTROL control,PPEDIGREE pd,int ith);
void PrintIconSetArray(FILE *fp,PINDIVIDUAL p,PCONTROL c);

/*                */
/* Free routines: */
/*                */
void FreeCell(PCELL c);
void FreeCellArray(PCELLARRAY ca);
void FreeReport(PREPORT r);
void FreeDrawing(PPEDIGREEDRAWING d);

void AutoLabel(FILE *fp,double x, double y,int b,int u,int j,int font,int n,...);
void QuickLabel(FILE *fp,double x,double y,double w,int b,int n,...);
/*                       */
/* 19990520 ET addendum: */
/*                       */
void SaveMinMaxCoordinates(PINDIVIDUAL p, PPEDIGREE pd, int ith);
double PrintPedigreeDrawing(PCONTROL c,PPEDIGREE pd,int i,char *ProcName);
void LogMultiplePagePrintOrder(int MaxRows,int MaxCols);
double NewPrintPedigreeDrawing(PCONTROL c,PPEDIGREE pd);



