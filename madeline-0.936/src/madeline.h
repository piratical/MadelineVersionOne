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
/* "madeline.h" is part of the Madeline software distribution.                       */
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

/*                                                                          */
/*                                                                          */
/* madeline.h (originally called "parser.h")                                */
/*                                                                          */
/*                                                                          */
/* history:                                                                 */
/*                                                                          */
/* 970219 initiated by ET                                                   */
/*                                                                          */
/* 971201->9801  many new formats added at the same time as                 */
/*               many object-oriented programming design                    */
/*               principles were incorporated to improve program            */
/*               clarity and code manageability                             */
/*                                                                          */
/*////////////////////////////////////////////////////////                  */
/*                                                                          */
/* madeline.c, originally called "parser.c", is the main module of          */
/* Madeline.  This module contains Madeline's command line interpreter and  */
/* recursive descent parser, as well as the functions that produce the      */
/* various output formats. "madeline.c" was originally called "parser.c"    */
/* because originally, way back when, there was a command-line-arguments    */
/* -only version of Madeline which did not have any nice command-line       */
/* interpreter as we now have here.                                         */
/*                                                                          */
/* Madeline is written in ANSI C.  Originally, C++-style comments were      */
/* used, but these have now all been converted to ANSI-C-style comments     */
/* (as of Madeline v. 0.93, initiated 2000 02 24 by ET).                    */
/*                                                                          */
/* Some of the code libraries used in Madeline have been adapted from       */
/* other projects of mine, mainly a Master's thesis that I pursued part-    */
/* time between 1990-1997.  ndbf.c and tools.c are two of the code          */
/* libraries that were originally borrowed directly, and only later re-     */
/* vised extensively to provide additional functionality required for       */
/* Madeline.  Although I have tried to clean things up considerably, these  */
/* older code libraries may still contain relics of their past lives        */
/* that may be irrelevant in Madeline.  So, just be aware that these code   */
/* libraries may contain artifacts that provide hints about how long ago    */
/* they must have been written!                                             */
/*                                                                          */
/* On top of the fact that I took legacy C code libraries and adapted them  */
/* for Madeline, some code written just for Madeline started out with       */
/* very little attention given to future manageability.  I have no          */
/* intention of being apologetic, because the reality was that I could not  */
/* write Madeline as fast as her services were required, and many portions  */
/* were written before I really thought about the advantages of object-     */
/* oriented design principles.                                              */
/*                                                                          */
/* Since her birth, however, Madeline has been incrementally revised and    */
/* expanded.  I have increasingly tried to employ object-oriented design    */
/* principles in executing changes to the program, hoping that these        */
/* changes will lead to a more readable and manageable program.             */
/*                                                                          */
/* -Ed Trager <edtrager@umich.edu>                                          */
/*  Ann Arbor, Michigan, January, 1998                                      */
/*  Revised, February 25, 2000                                              */
/*                                                                          */

/*                                       */
/* ***********************               */
/*                                       */
/* VERSION CONTROL NUMBER: in portable.h */
/*                                       */
/* ***********************               */
/*                                       */

/*/////////////// */
/*                */
/* ANSI INCLUDES: */
/*                */
/*/////////////// */
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <math.h>
#include <setjmp.h>
#include <time.h>

/*////////////////// */
/*                   */
/* MADELINE INCLUDES */
/*                   */
/*////////////////// */
#ifndef PEDIGREE_INCLUDED
#include "pedigree.h"
#endif

#ifndef NDBF_INCLUDED
#include "ndbf.h"
#endif

#ifndef GRAPHICS_INCLUDED
#include "graphics.h"
#endif

#ifndef MAP_INCLUDED
#include "map.h"
#endif

#ifndef TWIN_INCLUDED
#include "twin.h"
#endif

#ifndef CONTROL_INCLUDED
#include "control.h"
#endif

#ifndef TOKEN_INCLUDED
#include "token.h"
#endif

//#ifndef TOKNTREE_INCLUDED
//#include "tokentree.h"
//#endif

#ifndef TREE_INCLUDED
#include "tree.h"
#endif

#ifndef COUNTGENOTYPES_INCLUDED
#include "countgenotypes.h"
#endif

#ifndef CONSANGUINITY_INCLUDED
#include "consanguinity.h"
#endif

#ifndef PAPERSIZES_INCLUDED
#include "papersizes.h"
#endif

#ifndef LANGCONVENTIONS_INCLUDED
#include "languageconventions.h"
#endif

#ifndef FIELDFLAGS_INCLUDED
#include "fieldflags.h"
#endif

#ifndef VAR_AND_ARR_INCLUDED
#include "variablesandarrays.h"
#endif

#ifndef ASSOCIATIVEARRAY_INCLUDED
#include "associativearrays.h"
#endif

#ifndef TOKENTYPES_INCLUDED
#include "tokentypes.h"
#endif

#ifndef ASSIGNMENT_INCLUDED
#include "assignment.h"
#endif

#ifndef INTEGRITY_INCLUDED
#include "integrity.h"
#endif

#ifndef REGEXP_INCLUDED
#include "regexp.h"
#endif

#ifndef ASSOCIATIVEMAPS_INCLUDED
#include "associativemaps.h"
#endif

#ifndef GRAPH_INCLUDED
#include "graph.h"
#endif

#ifndef CONSOLE_INCLUDED
#include "console.h"
#endif

#ifndef KNOWNTOKENS_INCLUDED
#include "knowntokens.h"
#endif

#ifndef HALDANE_KOSAMBI_INCLUDED
#include "haldanekosambi.h"
#endif

#ifndef TOGGLEBYCHR_INCLUDED
#include "togglebychromosome.h"
#endif

#ifndef PROGRAMCONTROL_INCLUDED
#include "programcontrol.h"
#endif

#ifndef METHODS_INCLUDED
#include "methods.h"
#endif

#ifndef SHOW_STATS_INCLUDED
#include "showstatistics.h"
#endif

#ifndef TRAVERSAL_INCLUDED
#include "traversal.h"
#endif

#ifndef DATA_EVAL_INCLUDED
#include "dataevaluation.h"
#endif

#ifndef GENERIC_FORMAT_INCLUDED
#include "generic_format.h"
#endif

#ifndef MENDEL_FORMAT_INCLUDED
#include "mendel_format.h"
#endif

#ifndef GENEHUNTER_FORMAT_INCLUDED
#include "genehunter_format.h"
#endif

#ifndef PROGRAMCONTROL_INCLUDED
#include "programcontrol.h"
#endif

#ifndef COMPLEXITY_INCLUDED
#include "complexity.h"
#endif

#ifndef ALLELE_FREQUENCIES_INCLUDED
#include "allelefrequencies.h"
#endif

/*                              */
/* A quick sort implementation: */
/*                              */
#include "qsort.h"

/*                              */
/* For Convert() functionality: */
/*                              */
#include "convertdelimitedfiles.h"

/*                                              */
/* For Rectify() and Recognize() functionality: */
/*                                              */
#include "recognize.h"

/*                                                      */
/* For some reasonably portable types like UFLAG8, etc. */
/*                                                      */
#ifndef PORTABLE_INCLUDED
#include "portable.h"
#endif


#ifndef LOGS_INCLUDED
#include "logs.h"
#endif

#ifndef RESULT_INCLUDED
#include "result.h"
#endif

/*                               */
/* defines for old SUN platform: */
/*                               */
#ifndef CLOCKS_PER_SEC
#define CLOCKS_PER_SEC 1000.0
#endif

#ifndef RAND_MAX
#define RAND_MAX 0x7FFFU
#endif

/*                                                              */
/* define a short-cut way of specifying the DOUBLEMISSINGVALUE: */
/*                                                              */
#define DMV DOUBLEMISSINGVALUE

#define TOKENSTACKALLOCATIONUNIT 300
#define REPEATSIGNAL             '~' /* signal for expression parser to repeat            */
#define MAXPARAMETERS             10 /* maximum number of parameters passed to a function */


/*                     */
/* Minimum difference: */
/*                     */
#define MINIMUMDIFFERENCE 1.0E-15


#define FSPMAXRECLENGTH 132

/*                                   */
/* The global token stack variables: */
/*                                   */
extern PTOKENINFO Token;
extern PTOKENINFO pT;
extern int TokenStackSize;
extern int RepeatInProgress;

/*                                                                  */
/* The global expression (hence "e") pointer:                       */
/*                                                                  */
extern char *estt;  /* always points to the start of the expression */
extern char *e;     /* points to the next token                     */


/*//////////////////////////////         */
/*                                       */
/* Global pointer to control structure,  */
/* g:                                    */
/*                                       */
/*//////////////////////////////         */
extern PCONTROL g;

/*                                             */
/* character and numeric missing value arrays: */
/*                                             */
extern ARRAY Cmv;
extern ARRAY Nmv;
extern PARRAY pCmv;
extern PARRAY pNmv;

/*                                                                     */
/* character and numeric sex designation arrays and internal variable: */
/*                                                                     */
extern ARRAY Csv;
extern ARRAY Nsv;
extern PARRAY pCsv;
extern PARRAY pNsv;

/*                                               */
/* character and numeric affection status arrays */
/*                                               */
extern ARRAY Caff; 
extern ARRAY Naff;
extern PARRAY pCaff;
extern PARRAY pNaff;

/*                                           */
/* character and numeric death status array: */
/*                                           */
extern ARRAY Cds;
extern ARRAY Nds;
extern PARRAY pCds;
extern PARRAY pNds;


/*                                                         */
/* Required input field set default field names:           */
/* these are initialized in InitializeInternalVariables(): */
/*                                                         */
extern VARIABLE gfnFamID;
extern VARIABLE gfnID;
extern VARIABLE gfnFather;
extern VARIABLE gfnMother;
extern VARIABLE gfnSex;
extern VARIABLE gfnMZTwin;
extern VARIABLE gfnAffected;
extern VARIABLE gfnProband;
extern VARIABLE gfnClass;
extern VARIABLE gfnLClass;
extern VARIABLE gfnDead;

extern VARIABLE gDatabaseFile;

extern VARIABLE gAllLogFiles;

extern VARIABLE gLogFile;
extern VARIABLE gDetailFile;
extern VARIABLE gErrorFile;
extern VARIABLE gCommandFile;
extern VARIABLE gOutputFile;
extern VARIABLE gParameterOutputFile;

extern PPEDIGREEDRAWING gDrawing; /* global pedigree drawing object    */
extern VARIABLE LabelFontSize;    /* pedigree drawing label font size  */
extern VARIABLE LegendFontSize;   /* pedigree drawing legend font size */

/*                                    */
/* The global recognized token table: */
/*                                    */
extern RECOGNIZEDTOKEN RecognizedToken[];


/*                                                                                    */
/* The global task state for setjmp() and longjmp():                                  */
/*                                                                                    */
extern jmp_buf BunnyHop;    /* Well, you won't forget the name of this var, will you? */

/*                                              */
/* globals for pedigree drawing routines:       */
/*                                              */
extern double gu;     /* global unit of measure */
extern double gdiam;  /* diameter               */
extern double grad;   /* radius                 */
extern double gym;    /* y-margin               */
extern double gxu;
extern double lxu;        
extern double gyu;
extern double gxex;


/*////////////////////// */
/*                       */
/* Functions:            */
/*                       */
/*////////////////////// */

/*//////////////////////////////////// */
/*                                     */
/* Recursive descent parser functions: */
/*                                     */
/*//////////////////////////////////// */
int GetToken(void);
void EvaluateValueOrVariable(PRESULT result);
void EvaluateParenthesizedExpression(PRESULT result);
void EvaluateFunctionCallOrArray(PRESULT result);
void EvaluateUnaryOperator(PRESULT result);
void EvaluateExponentiation(PRESULT result);
void EvaluateFactors(PRESULT result);
void EvaluateTerms(PRESULT result);
void EvaluateComparators(PRESULT result);
void EvaluateJoiners(PRESULT result);
/*                                                            */
/* EvaluateExpression(): this is the entry point into parser: */
/*                                                            */
void EvaluateExpression(PRESULT result);

/*///////////////////////////////////////////////// */
/*                                                  */
/* Non-fatal (recoverable) error handling function: */
/*                                                  */
/*///////////////////////////////////////////////// */
void ParserErrorReport(const char *s,...);


/*/////////////////////////////////////////////// */
/*                                                */
/* Wrapper functions for mathematical operations: */
/*                                                */
/*/////////////////////////////////////////////// */
double Inverse(double v);

/*/////////////////////// */
/*                        */
/* Token stack functions: */
/*                        */
/*/////////////////////// */

PTOKENINFO NewTokenStack(int stacksize);
PTOKENINFO IncreaseTokenStack(PTOKENINFO tstack,int oldsize,int newsize);
void       ClearTokenStack(PTOKENINFO, int stacksize);

/*//////////////////////////////////////////////// */
/*                                                 */
/* Initialize internal variables/arrays functions: */
/*                                                 */
/*//////////////////////////////////////////////// */
void InitializeInternalVariables(void);


/*///////////////////////////////// */
/*                                  */
/* String functions for the parser: */
/*                                  */
/*///////////////////////////////// */
void SubString(char *dest,char *src,int stt,int howmany);

/*///////////////////////////////////// */
/*                                      */
/* Parser command evaluation functions: */
/*                                      */
/*///////////////////////////////////// */
/* void Assignment(void);  <-- moved to assignment.c */
void Banner(void);
void CommandLine(FILE *fp);
void Draw(void);
void Go(void);
void Hello(void);
void Help(void);
void Interpret(char *expr);
void List(void);
void Locus(void);
void Open(void);
void Quiet(void);
void Quit(void);
void Run(void);
void Show(void);
void Silence(void);
void System(void);
void Toggle(void);
void Turn(void);
void Verbose(void);
void View(void);
void WhatIs(void);
void Write(void);


/*/////////////////////////////////////////////////////////// */
/*                                                            */
/* Functions to examine missing values and read known fields  */
/* like SexField or DeathStatusField:                         */
/*                                                            */
/*/////////////////////////////////////////////////////////// */
int IsCMV(char *s);
int IsNMV(double v);
signed char ReadFlagField(PDBF db,int i,UINT32 rci);

/*////////////////////// */
/*                       */
/* Field flag functions: */
/*                       */
/*////////////////////// */

void ShowFieldStatusColumnwise(PDBF db,int columns,void (*showfunc)(PDBF db,int f));
void DisplayFieldInformation(PDBF db,int f);
void SetFieldFlags(PDBF db);
void SetKnownFieldFieldFlags(PDBF db);

/*////////////////////////////////// */
/*                                   */
/* Functions used to determine field */
/* contents:                         */
/*                                   */
/*////////////////////////////////// */
int IsAGenotypeField(PDBF db, int j);
int ContainsAGenotype(char *s);


/*/////////////////////////////////////////////////////////// */
/*                                                            */
/* Call ReadCoreInformation() before constructing a pedigree: */
/*                                                            */
/*/////////////////////////////////////////////////////////// */
PPEDIGREE ReadCoreInformation(PPEDIGREE pd,PDBF db);

/*////////////////////////////////////// */
/*                                       */
/* Pedigree drawing function prototypes: */
/*                                       */
/*////////////////////////////////////// */
PCELL  NewPedigreeDrawingCell(int s,int z);
PCELL  NewPedigreeLegendCell(int s, int z);
void   DrawPedigree(PPEDIGREE pd);
void   DrawSingleFoundingGroupPedigree(PPEDIGREE pd,int i);
void   DrawFamily(FILE *fp,PINDIVIDUAL p,double xorg,double yorg,PCELL c,PPEDIGREE pd,int i);
/* 2001.12.14.ET ADDENDUM: */
void DrawConsanguinousConnector(FILE *fp,double xorg,double yorg,double xdest,double ydest);
void   DrawOffspring(FILE *fp,PINDIVIDUAL dad,PINDIVIDUAL mom,double xorg,double yorg,PCELL c,PPEDIGREE pd,int i);
double IndividualXOriginShift(PINDIVIDUAL p);
void   HorizontalLineToSpouse(FILE *fp,double xorg,double yorg,double xend,int idx,int maxidx,int consanguinous);
void   ClearIndividualDrawFlags(PINDIVIDUAL p);

/*                                                                            */
/* wrapper functions used for sorting offspring vectors for pedigree drawing: */
/*                                                                            */
void  SortTwinGroup(PTWINGROUP);
void  SortIndividualVector(PPINDIVIDUAL v,int n,PCONTROL c);

/*///////////////////////////////////////// */
/*                                          */
/* Bookkeeping and data-tracking functions: */
/*                                          */
/*///////////////////////////////////////// */
int PedigreeHasProband(PPEDIGREE pd);

/*////////////////////////////////////////////////////////////////////// */
/*                                                                       */
/* NumerizeID transforms characters in an ID to ASCII digit equivalents: */
/* This is required for CRIMAP format, and may be required for other     */
/* formats too:                                                          */
/*                                                                       */
/*////////////////////////////////////////////////////////////////////// */
char *NumerizeID(char *b,char *s);


/*/////////////////////////////////////////////////////////////// */
/*                                                                */
/* CRIMAP:                                                        */
/*                                                                */
/* Routines originally implemented for Crimap output and support: */
/* (added to parser.c on 970915 by ET)                            */
/*                                                                */
/*/////////////////////////////////////////////////////////////// */
void CrimapDataHeaderProcessor(PCONTROL c,char *fn);
void CrimapOutput(PCONTROL c,PPEDIGREE pd);
void CrimapDataFooterProcessor(PCONTROL c);
/*                                                                          */
/* TraverseOffspring() and TraverseOffspring2() operate on the spouses only */
/* if the spouse has children.                                              */
/*                                                                          */
/* TraverseOffspring3() operates on the spouses regardless of the number of */
/* children they have -- used by the EXCLUDE and UNEXECLUDE command set     */
/*                                                                          */
void TraverseOffspring(PCONTROL c,PINDIVIDUAL pm,void (*func)(PCONTROL,PINDIVIDUAL));
void TraverseOffspring2(PCONTROL c,PPEDIGREE pd,PINDIVIDUAL pm,void (*func)(PCONTROL,PPEDIGREE,PINDIVIDUAL));
void TraverseOffspring3(PCONTROL c,PPEDIGREE pd,PINDIVIDUAL pm,void (*func)(PCONTROL,PPEDIGREE,PINDIVIDUAL));
void OutputCrimapIndividualRecord(PCONTROL,PINDIVIDUAL p);

/*/////////////////////////////////// */
/*                                    */
/* FISHER-specific routines:          */
/*                                    */
/*/////////////////////////////////// */
void FisherDataHeaderProcessor(PCONTROL c, char *fn);
void FisherOutput(PCONTROL c,PPEDIGREE pd);


/*/////////////////////////////////// */
/*                                    */
/* USERM13-specific routines:         */
/*                                    */
/*/////////////////////////////////// */
void UserM13DataHeaderProcessor(PCONTROL c, char *fn);
void UserM13Output(PCONTROL c,PPEDIGREE pd);
void OutputUserM13IndividualRecord(PCONTROL c,PINDIVIDUAL p);


/*/////////////////////////////////// */
/*                                    */
/* Relpair-specific routines:         */
/*                                    */
/*/////////////////////////////////// */
void RelpairOutput(PCONTROL c,PPEDIGREE pd);
void OutputRelpairIndividualRecord(PCONTROL c,PINDIVIDUAL p);



/*/////////////////////////////// */
/*                                */
/* SAGE PACKAGE: FSP              */
/*                                */
/* FSP-specific output functions: */
/*                                */
/*/////////////////////////////// */
void FSPDataHeaderProcessor(PCONTROL c,char *fn);
void FSPOutput(PCONTROL c,PPEDIGREE pd);
void OutputFSPIndividualRecord(PCONTROL c,PPEDIGREE pd,PINDIVIDUAL p);

/*////////////////////////////////// */
/*                                   */
/* SAGE PACKAGE: SIBPAL              */
/*                                   */
/* Sibpal-specific output functions: */
/*                                   */
/*////////////////////////////////// */
void SibpalDataHeaderProcessor(PCONTROL c,char *fn);
void SibpalHeader(PCONTROL c);
int  OutputSibpalField(PCONTROL c,PINDIVIDUAL p,int i,int j);
void OutputSibpalIndividualRecord(PCONTROL c,PPEDIGREE pd,PINDIVIDUAL p);
void SibpalOutput(PCONTROL c,PPEDIGREE pd);


/*////////////////////////////////// */
/*                                   */
/* SIBLINK routines:                 */
/*                                   */
/*////////////////////////////////// */
void SiblinkDataHeaderProcessor(PCONTROL c,char *fn);
void SiblinkOutput(PCONTROL c,PPEDIGREE pd);
void SiblinkTraverseNuclearBlock(PCONTROL c,PPEDIGREE pd,PINDIVIDUAL s1,PINDIVIDUAL s2,void (*func)(PCONTROL c,PPEDIGREE pd,PINDIVIDUAL p1,PINDIVIDUAL p2,int sss));
void SiblinkSibPairBlockOutput(PCONTROL c,PPEDIGREE pd,PINDIVIDUAL p1,PINDIVIDUAL p2,int sss);
void OutputSiblinkIndividualRecord(PCONTROL c,PINDIVIDUAL p);
void SiblinkDataFooterProcessor(PCONTROL c);
int  SiblinkNuclearFamilyHasData(PCONTROL c,PPEDIGREE pd,PINDIVIDUAL s1,PINDIVIDUAL s2);


/*                                */
/*                                */
/* SOLAR PACKAGE:                 */
/*                                */
/* Relate format:                 */
/* NOTE: OBSOLETE - ET 2000 02 25 */
/*                                */
void SolarRelateOutput(PCONTROL c,PPEDIGREE p);
void OutputSolarRelateIndividualRecord(PCONTROL c,PPEDIGREE pd,PINDIVIDUAL p);
void AssignSequentialID(PCONTROL c,PINDIVIDUAL p);


/*//////////////////////////////////// */
/*                                     */
/* SOLAR PACKAGE:                      */
/*                                     */
/* new SOLAR formats for SOLAR FISHER  */
/* OBSOLETE ? Not sure - ET 2000 02 25 */
/*//////////////////////////////////// */
void SolarFisherOutput(PCONTROL c,PPEDIGREE pd);
void OutputSolarFisherIndividualRecord(PCONTROL c,PINDIVIDUAL p);


/*/////////////////////////////////// */
/*                                    */
/* CONTROL structure routines:        */
/*                                    */
/*/////////////////////////////////// */
void InitializeControl(PCONTROL c);
void ResetCounters(PCONTROL c);
void TraverseBinaryPedigreeTree2(PCONTROL c,PPEDIGREE pd,void (*func)(PCONTROL,PPEDIGREE));

void GenehunterDataHeaderProcessor(PCONTROL c,char *fn);

/*////////////////////////// */
/*                           */
/* MENDEL Locus File Format: */
/*                           */
/*////////////////////////// */
void    OutputMendelMarker(PCONTROL c,int i);
void    OutputMendelMarkerAllele(FILE *fp,PALLELE a,int s);

/*//////////////////////// */
/*                         */
/* SAGE Locus File Format: */
/*                         */
/*//////////////////////// */
void    OutputSageMarker(PCONTROL c,int i);
void    OutputSageMarkerAllele(FILE *fp,PMARKER m,PALLELE a,int s);

/*//////////////////////////// */
/*                             */
/* GENEHUNTER LocusFileFormat: */
/*                             */
/*//////////////////////////// */
void    OutputGenehunterMarker(PCONTROL c,int i);
void    OutputGenehunterMarkerAllele(FILE *fp,PMARKER m,PALLELE a,int s);
void    OutputGenehunterLocusFileHeader(PCONTROL c,char *fn);
void    OutputGenehunterLocusFileFooter(PCONTROL c);

/*//////////////////////////// */
/*                             */
/* SIBLINK LocusFileFormat:    */
/*                             */
/*//////////////////////////// */
void OutputSiblinkMarker(PCONTROL c,int i);
void OutputSiblinkMarkerAllele(FILE *fp,PALLELE a,int s);
void OutputCurrentFormatSiblinkMarkerAllele(FILE *fp,PALLELE a,int s);

void ConstructPedigree(PPEDIGREE pd);


/*//////////////////////////// */
/*                             */
/* marker map functions:       */
/*                             */
/*//////////////////////////// */
void GetMapData(PCONTROL c,const char *fn);
void Load(void);
void DisplayMarkerMapInformation(PMARKERMAP p);
void DisplayMarkerMapInformationHeader(void);


/*/////////////////////////////////////////// */
/*                                            */
/* Generic delimited output support routines: */
/*                                            */
/*/////////////////////////////////////////// */
double GetNumericCoreFieldValue(PDBF db,int i,PPEDIGREE pd,PINDIVIDUAL p);
char * GetCharacterCoreFieldValue(PDBF db,int i,PPEDIGREE pd,PINDIVIDUAL p);
void   OutputDelimitedIndividualRecord(PCONTROL c,PPEDIGREE pd,PINDIVIDUAL p);
void   DelimitedOutput(PCONTROL c,PPEDIGREE pd);

/*/////////////////////////////////////////////       */
/*                                                    */
/* Functions for excluding pedigrees and individuals: */
/*                                                    */
/*/////////////////////////////////////////////       */
void Exclude(void);
void SetExcludeFlags(PCONTROL c,PPEDIGREE pd);
void SetIndividualExcludeFlag(PCONTROL c,PPEDIGREE pd,PINDIVIDUAL p);
void ClearExcludeFlags(PCONTROL c,PPEDIGREE pd);

void Unexclude(void);
void UnsetExcludeFlags(PCONTROL c,PPEDIGREE pd);
void UnsetIndividualExcludeFlag(PCONTROL c,PPEDIGREE pd,PINDIVIDUAL p);

/*//////////////////////////////////////       */
/*                                             */
/* Functions for recognizing flat files as     */
/* databases and for dealing with mBase files: */
/*                                             */
/*//////////////////////////////////////       */
void Convert(void);
void Rectify(void);
void Recognize(void);

/*//////////////////////////////////////// */
/*                                         */
/*                                         */
/* UserFQTL Routines                       */
/*                                         */
/*                                         */
/*//////////////////////////////////////// */
void UserFQTLLocusHeaderProcessor(PCONTROL c,char *fn);
void OutputUserFQTLMarker(PCONTROL c,int i);
void OutputUserFQTLMarkerAllele(FILE *fp,PALLELE a,int s);
void UserFQTLDataHeaderProcessor(PCONTROL c,char *fn);
void UserFQTLOutput(PCONTROL c,PPEDIGREE pd);
void OutputUserFQTLIndividualRecord(PCONTROL c,PINDIVIDUAL p,int parent);
void OutputUserFQTLField(PCONTROL c,PINDIVIDUAL p,int i,int fixedwidth);
/*                     */
/* For nuclear output: */
/*                     */
void UserFQTLNuclearBlockOutput(PCONTROL c,PPEDIGREE pd);
void TraverseUserFQTLNuclearBlock(PCONTROL c,PPEDIGREE pd,PINDIVIDUAL s1,PINDIVIDUAL s2);
int  UserFQTLNuclearFamilyHasData(PCONTROL c,PPEDIGREE pd,PINDIVIDUAL s1,PINDIVIDUAL s2);
int  UserFQTLIndividualHasData(PINDIVIDUAL p);

int  NonProbandHasOutputData(PINDIVIDUAL p);

void JoinFloatingSpouses(PPEDIGREE pd,PINDIVIDUAL p);
void CountAffectedsVirtualsAndUnconnecteds(PPEDIGREE pd,PINDIVIDUAL p);

/*                                                              */
/* (*criteria)() functions for removing individuals, pedigrees: */
/*                                                              */
int  IndividualGenderMissing(PINDIVIDUAL p);


void BatchCommandLine(char *fn);

/*                                           */
/* Support functions for the View() command: */
/*                                           */
void ViewIndividuals(PCONTROL c,PPEDIGREE pd);
void ViewIndividual(PCONTROL c,PPEDIGREE pd,PINDIVIDUAL p);

/*                                */
/* Clear() and support functions: */
/*                                */
void Clear(void);
void ClearPedigreeExclusions(PPEDIGREE pd);
void ClearExcludeFlag(PINDIVIDUAL p);

/*                                                              */
/* Functions to support exclusion of individuals with exclusion */
/* of their families by recursion:                              */
/*                                                              */
void SetExcludeFlagsRecursively(PCONTROL c,PPEDIGREE pd);
void SetIndividualExcludeFlagRecursively(PCONTROL c,PPEDIGREE pd,PINDIVIDUAL p);
void ExcludeIndividual(PCONTROL c,PPEDIGREE pd,PINDIVIDUAL p);

void UnsetIndividualExcludeFlagRecursively(PCONTROL c,PPEDIGREE pd,PINDIVIDUAL p);
void UnsetExcludeFlagsRecursively(PCONTROL c,PPEDIGREE pd);
void UnexcludeIndividual(PCONTROL c,PPEDIGREE pd,PINDIVIDUAL p);

/*                                              */
/* Support functions for drawing twin sibships: */
/*                                              */
double TraverseTwinGroupToDrawSibship(FILE *fp,PINDIVIDUAL f,PTWINGROUP t,
    double sttx,double liney,PCELL c,int OffspringDrawn,double xorg,
    PPEDIGREE pd,int ith,
    double (*DrawFunction)(FILE *,PINDIVIDUAL,PTWINGROUP,double,double,PCELL,int,double,PPEDIGREE,int)
    );
double DrawTwinSibship(FILE *fp,PINDIVIDUAL f,PTWINGROUP t,double sttx,double liney,PCELL c,int OffspringDrawn,double xorg,PPEDIGREE pd,int ith);

/*                                 */
/* ASPEX format support functions: */
/*                                 */
void AspexDataHeaderProcessor(PCONTROL c,char *fn);
void OutputAspexIndividualRecord(PCONTROL c,char *famid,PINDIVIDUAL p, int parent);
void AspexNuclearBlockOutput(PCONTROL c,PPEDIGREE pd);
void TraverseAspexNuclearBlock(PCONTROL c,PPEDIGREE pd,PINDIVIDUAL s1,PINDIVIDUAL s2);
int  AspexNuclearFamilyHasData(PCONTROL c,PINDIVIDUAL s1,PINDIVIDUAL s2);
void OutputAspexMarker(PCONTROL c,int i);
void OutputAspexMarkerAllele(FILE *fp,PMARKER m,PALLELE a,int s);

/*                                                              */
/* Functions to check for unpaired "twins" and remove twin flag */
/* on any such individuals:                                     */
/*                                                              */
void CheckTwins(PPEDIGREE pd);
void CheckTwinStatus(PINDIVIDUAL p);
void UnmarkSingletonTwins(PINDIVIDUAL f);

/*                                                        */
/* Relpair support functions -- Locus file is nearly      */
/* identical with UserFQTL format.  Data file is standard */
/* Mendel format.  Control file is similar to UserFQTL    */
/* format.                                                */
/*                                                        */
void RelpairLocusHeaderProcessor(PCONTROL c,char *fn);
void OutputRelpairMarker(PCONTROL c,int i);
void OutputRelpairMarkerAllele(FILE *fp,PALLELE a,int s);
void RelpairDataHeaderProcessor(PCONTROL c,char *fn);

/*                                     */
/* Field reordering support functions: */
/*                                     */
void AutoSetFieldOrdering(PCONTROL c,PDBF db);
void UserSetFieldOrdering(PCONTROL c,PDBF db);
void SetFieldIndex(PDBF db,char *nm,int *cc,int *pp,int *gg);
void TraverseMarkerMapByPosition3(PMARKERMAP p,PDBF db,int *cc,int *pp,int *gg,void (*visit)(PDBF,char *,int *,int *,int *));


/*                                */
/* View record support functions: */
/*                                */
void ViewIndividualsInDetail(PCONTROL c,PPEDIGREE pd);
void ViewIndividualInDetail(PCONTROL c,PPEDIGREE pd,PINDIVIDUAL p);
void ViewIndividualRecord(PDBF db);
void PrintViewRecordField(PDBF db,int i,int *j);

/*                                                                 */
/* Functions to support evaluation of internal variables and data, */
/* such as data inside an INDIVIDUAL structure:                    */
/*                                                                 */
void EvaluateIndividualVariableReference(PRESULT result);
void EvaluateIndividualArrayReference(PCONTROL c,PTOKENINFO pop,PRESULT result);
void EvaluateIndividualPointerReference(PCONTROL c,PRESULT result);
void GetReferencesToIndividual(PCONTROL c); /* get the PPEDIGREE and PINDIVIDUAL references */
void EvaluateAggregateFunction(PCONTROL c,PTOKENINFO pop,PRESULT result);

/*                                         */
/* User interface to set display language: */
/*                                         */
void UserSetLanguage(PCONTROL c);

/*                                        */
/* Utility function for rounding numbers: */
/*                                        */
double RoundToWhole(double v);

/*                                                  */
/* Functions to support different data sort orders: */
/*                                                  */
void Sort(void);
int CompareExpression(void *a,void *b,int ascend);
/*                                */
/* Evaluating a saved expression: */
/*                                */
void EvaluateExpressionOnStack2(PRESULT result);
/*             */
/* show tally: */
/*             */
void ShowTally(PCONTROL c,char *message);
/*                            */
/* Additional free() routines */
/*                            */
void FreeArray(PARRAY a);
void FreeInternalVariables(void);
PTOKENINFO FreeTokenStack(PTOKENINFO tstack,int *stacksize);

int  FisherIndividualHasData(PINDIVIDUAL p);

void TraverseIndividualBSTMarkDataFlag(PINDIVIDUAL p,int (*HasData)(PINDIVIDUAL));
void TraverseIndividualBSTClearDataFlag(PINDIVIDUAL p);

/*                                            */
/* New function for generic delimited output: */
/*                                            */
char *OutputGenericField(PCONTROL c,PPEDIGREE pd,PINDIVIDUAL p, int i);
/*                             */
/* New PrintPrompt() function: */
/*                             */
void PrintPrompt(PCONTROL c,FILE *source);
/* 2000 05 07 ET Addendum to support prompt using GNU readline(): */
char *ConstructPrompt(PCONTROL c);
void PrintErrorLogHeader(PCONTROL c);

/*                                                           */
/* 19990327 ET ADDENDUM: View distinct values of a variable: */
/*                                                           */
void ViewDistinct(PCONTROL c);
/*                                                                */
/* 19990328 ET ADDENDUM: edit a file using user-specified editor: */
/*                                                                */
void Edit(void);
/*                                                        */
/* 19990402 ET ADDENDUM: Transpose ABI-type output table: */
/* 20030212 ET CHANGE: Transpose() now called Compose()   */
/* in order to complement the Decompose() command.        */
/*                                                        */
void Compose(void);
/*                                       */
/* 19990405 ET ADDENDUM: Merge n tables: */
/*                                       */
void Merge(void);

/*                                        */
/* 19990414 ET ADDENDUM: Help file stuff: */
/*                                        */
void RemoveHelpTextDelimiters(char *HelpTextBuffer);
/*                                                                               */
/* 19990416 ET ADDENDUM: Debug() is an interactive wrapper for any debug routine */
/*                       you want to put in there:                               */
void Debug(void);

/*                                                            */
/* 19990506 ET ADDENDA: For PEDCHECK format (a LINKAGE format */
/* very similar to GENEHUNTER format)                         */
/*                                                            */
void OutputPedCheckLocusFileHeader(PCONTROL c,char *fn);
void OutputPedCheckLocusFileFooter(PCONTROL c);
void PedCheckOutput(PCONTROL c,PPEDIGREE pd);
void PedCheckDataHeaderProcessor(PCONTROL c,char *fn);


/*                                                                            */
/* 19990519 ET ADDENDA: For Linkage Disequilibrium / Haplotyping trio format: */
/*                                                                            */
int  MarkTrioOffspringHavingGenotypeData(PINDIVIDUAL p);
void LDEQHOutput(PCONTROL c,PPEDIGREE pd);
void LDEQHPedigreeHasDataBookkeeping(PCONTROL c,PPEDIGREE pd);
PINDIVIDUAL LDEQHTraverseToFindBestTrio(PINDIVIDUAL p,int reset);
void OutputLDEQHIndividualRecord(PCONTROL c,PPEDIGREE pd,PINDIVIDUAL p,int IsParent);
int  LDEQHCountIncludedPedigreeIndividual(PCONTROL c,PINDIVIDUAL p);


/*                                                                 */
/* IndividualLeftSubtreeWidth() and IndividualRightSubtreeWidth(): */
/*                                                                 */
double IndividualLeftSubtreeWidth(PINDIVIDUAL p);
double IndividualRightSubtreeWidth(PINDIVIDUAL p);

/*                      */
/* 19990603 ET ADDENDA: */
/*                      */
void DrawFor(PCONTROL c);
void DrawPedigreeIfTrueForOneIndividual(PCONTROL c,PPEDIGREE pd);
int TraverseIndividualBSTUntilTrue2(PCONTROL c,PPEDIGREE pd,PINDIVIDUAL p,int (*eval)(PCONTROL,PPEDIGREE,PINDIVIDUAL));
int DrawPedigreeIfTrueForThisIndividual(PCONTROL c,PPEDIGREE pd,PINDIVIDUAL p);

/*                      */
/* 19990621 ET ADDENDA: */
/*                      */
int  FieldValuesTooLongForLabel(PNODE node);
void PrintIconLegend(FILE *fp,double cx,double cy,PDBF db);
int  PrintIconLegendLineItem(FILE *fp,PDBF db,int fieldIndex,PNODE node,int levelIndex);
/* 19990910 ET ADDENDUM */
void PrintStatusTableLine(const char *state,const char *setting,const char *description);

/* 2000 03 07 ET ADDENDUM */
void License(void);

/* 2000 05 09 ET ADDENDUM */
void Lookup(void);

/* 2001.10.30 ET ADDENDUM */
void PromptUser(void);

/* 2002.05.24 ET ADDENDUM */
void EvaluateMultipleExpressions(int StopToken,int PrintFlag);
void ViewIndividualsData(PCONTROL c,PPEDIGREE pd);
void ViewIndividualData(PCONTROL c,PPEDIGREE pd,PINDIVIDUAL p);

/* 2002.11.26 ET ADDENDUM */
int CountIndividualMarkerInconsistencies(PCONTROL g,PINDIVIDUAL p);

/* 2003.01.28.ET ADDENDUM */
void ConvertMapAndOtherFileTypes(int filetype);

/* 2003.02.12.ET ADDENDUM              */
/* This is the complement of Compose() */
/*                                     */
void Decompose(void);
void Square(void);

/*///////////////////////////////////////////// */
/*                                              */
/* main():                                      */
/*                                              */
/*///////////////////////////////////////////// */
int main(int argc,char *argv[]);


