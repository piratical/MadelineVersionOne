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
/* "madeline.c" is part of the Madeline software distribution.                       */
/* Written by Edward H. Trager.                                                      */
/*                                                                                   */
/* COPYRIGHT (C) 2000,2002 THE REGENTS OF THE UNIVERSITY OF MICHIGAN,                */
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
/* Last modified by Ed Trager, April, 2004.                                          */
/*                                                                                   */
/* ********************************************************************************* */

/*                                                                          */
/*                                                                          */
/* madeline.c                                                               */
/*                                                                          */
/* history:                                                                 */
/*                                                                          */
/* 970219 initiated by ET                                                   */
/* 990320 extensive revisions and reorganization initiated by ET for v. 1.0 */
/*                                                                          */

#include "madeline.h"
/*                                                                           */
/* For OS-specific process management and the CreateChildProcess() function: */
/*                                                                           */
#include "processm.h"

/* for version and copyright information: */
#include "version.h"

/* for readline: */

#include <readline/readline.h>
#include <readline/history.h>

/* For multibyte UTF-8 string related stuff: */
#include <locale.h>
#include "utf8support.h"

/* Configuration options: */
#include "config.h"

/* Field Level Labels: */
#include "fieldlevellabels.h"

/* Directory listing: */
#include "directorylisting.h"

/* For smart data table handling: */
#include "smartselect.h"

/* String class: */
#include <string>

/*//////////////////////                                              */
/*                                                                    */
/* Globals variables:                                                 */
/*                                                                    */
/* In general, global variables are not very nice things to have,     */
/* but, unfortunately, Madeline has some global variables that        */
/* need to be maintained in the current implementation.  Later global */
/* variables all begin with lowercase "g", like gdb, gfp, gfpar, etc. */
/*                                                                    */
/*//////////////////////                                              */

/*                                                                                        */
/* Token stack:                                                                           */
/*                                                                                        */
PTOKENINFO Token    =NULL; /* current token stack                                         */
PTOKENINFO Token2   =NULL; /* reserve token stack                                         */
PTOKENINFO pT       =NULL; /* pointer to either the current or reserve token stack        */
int TokenStackSize  =TOKENSTACKALLOCATIONUNIT; /* keeps track of current token stack size */
int TokenStack2Size =TOKENSTACKALLOCATIONUNIT; /* size of reserve token stack             */
int RepeatInProgress=0;    /* tells expression parser if repeating last expression        */

/*                                                                                */
/* Global expression pointer:                                                     */
/*                                                                                */
char *estt=NULL;           /* always points to the start of the expression        */
char *e=NULL;              /* global expression pointer: points to the next token */

char gSiblinkOutputAlleleMissingValue[]="0";

/*                          */
/* Global number of traits: */
/*                          */
int gnTraits=0;


/*                                                        */
/* BunnyHop: global task state for setjmp() and longjmp() */
/*                                                        */
jmp_buf BunnyHop;


/*//////////////////////////////         */
/*                                       */
/* Global pointer to control structure,  */
/* g:                                    */
/*                                       */
/*//////////////////////////////         */
CCONTROL ggg;
PCONTROL g=&ggg;

/*                        */
/* ASSOCIATIVE ARRAYS     */
/*                        */
ASSOCARRAY aaas={0           ,NUMERICVALUE,0,NULL }; /* The AffectionStatus associative array */
ASSOCARRAY aags={0           ,NUMERICVALUE,0,NULL }; /* The GenderStatus    associative array */
ASSOCARRAY aads={0           ,NUMERICVALUE,0,NULL }; /* The DeathStatus     associative array */
ASSOCARRAY aaps={0           ,NUMERICVALUE,0,NULL }; /* The ProbandStatus   associative array */
ASSOCARRAY aalc={0           ,NUMERICVALUE,0,NULL }; /* The LiabilityClass  associative array */
ASSOCARRAY aaga={STRINGVALUE ,STRINGVALUE ,0,NULL }; /* The GraphAnnotation associative array */

/* ASSOCARRAY aass={STRINGVALUE ,STRINGVALUE ,0,NULL }; */
/* ASSOCARRAY aann={0           ,0           ,0,NULL }; */
/* ASSOCARRAY aasn={STRINGVALUE ,NUMERICVALUE,0,NULL }; */
/* ASSOCARRAY aads={DATEVALUE   ,STRINGVALUE ,0,NULL }; */
 

/*                                                                     */
/* character and numeric sex designation arrays and internal variable: */
/*                                                                     */
ARRAY Csv={STRINGVALUE ,0,0,NULL}; /* character sex level values       */
ARRAY Nsv={NUMERICVALUE,0,0,NULL}; /* numeric   sex level values       */
PARRAY pCsv=&Csv;
PARRAY pNsv=&Nsv;

ARRAY Caff={STRINGVALUE ,0,0,NULL}; /* character affect. status */
ARRAY Naff={NUMERICVALUE,0,0,NULL}; /* numeric affect. status   */
PARRAY pCaff=&Caff;
PARRAY pNaff=&Naff;

ARRAY Cds={STRINGVALUE ,0,0,NULL}; /* character death status */
ARRAY Nds={NUMERICVALUE,0,0,NULL}; /* numeric death status   */
PARRAY pCds=&Cds;
PARRAY pNds=&Nds;

VARIABLE LabelFontSize ={NUMERICVALUE,{0},0};  /* pedigree drawing label font size  */
VARIABLE LegendFontSize={NUMERICVALUE,{0},0};  /* pedigree drawing legend font size */

VARIABLE EvaluationInterval={NUMERICVALUE,{0},0};
VARIABLE OffEndDistance    ={NUMERICVALUE,{0},0};
/*                                                         */
/* Required input field set default field names:           */
/* these are initialized in InitializeInternalVariables(): */
/*                                                         */
VARIABLE gfnFamID   ={FIELDNAMEVALUE,{0},MISSING};
VARIABLE gfnID      ={FIELDNAMEVALUE,{0},MISSING};
VARIABLE gfnFather  ={FIELDNAMEVALUE,{0},MISSING};
VARIABLE gfnMother  ={FIELDNAMEVALUE,{0},MISSING};
VARIABLE gfnSex     ={FIELDNAMEVALUE,{0},MISSING};
VARIABLE gfnMZTwin  ={FIELDNAMEVALUE,{0},MISSING};
VARIABLE gfnDZTwin  ={FIELDNAMEVALUE,{0},MISSING};
VARIABLE gfnAffected={FIELDNAMEVALUE,{0},MISSING};
VARIABLE gfnProband ={FIELDNAMEVALUE,{0},MISSING};
VARIABLE gfnClass   ={FIELDNAMEVALUE,{0},MISSING};
VARIABLE gfnLClass  ={FIELDNAMEVALUE,{0},MISSING};
VARIABLE gfnDead    ={FIELDNAMEVALUE,{0},MISSING};
/*                                         */
/* Date of birth and date of death fields: */
/*                                         */
VARIABLE gfnDOB     ={FIELDNAMEVALUE,{0},MISSING};
VARIABLE gfnDOD     ={FIELDNAMEVALUE,{0},MISSING};

VARIABLE gDatabaseFile       ={STRINGVALUE,{0},0};
VARIABLE gAllLogFiles        ={STRINGVALUE,{0},0};
VARIABLE gLogFile            ={STRINGVALUE,{0},0};
VARIABLE gDetailFile         ={STRINGVALUE,{0},0};
VARIABLE gOutputFile         ={STRINGVALUE,{0},0};
VARIABLE gParameterOutputFile={STRINGVALUE,{0},0};
VARIABLE gErrorFile          ={STRINGVALUE,{0},0};
VARIABLE gCommandFile        ={STRINGVALUE,{0},0};

VARIABLE gMapDatabase    ={STRINGVALUE,{0},0};
VARIABLE gfnMapChromosome={FIELDNAMEVALUE,{0},MISSING};

/* Map Positions are assumed to be in Kosambi centiMorgans                                              */
VARIABLE gfnMapPosition       ={FIELDNAMEVALUE,{0},MISSING}; /* Sex-averaged map position field name    */
VARIABLE gfnMapFemalePosition ={FIELDNAMEVALUE,{0},MISSING}; /* Female-specific map position field name */
VARIABLE gfnMapMalePosition   ={FIELDNAMEVALUE,{0},MISSING}; /* Male-specific map position field name   */

VARIABLE gfnMapPositionBP={FIELDNAMEVALUE,{0},MISSING};
VARIABLE gfnMapOrdinal   ={FIELDNAMEVALUE,{0},MISSING};
VARIABLE gfnMapMarker    ={FIELDNAMEVALUE,{0},MISSING};


VARIABLE gPrintCommand   ={STRINGVALUE,{0},0};
VARIABLE gPedigreeDrawing={STRINGVALUE,{0},0};
VARIABLE gGraphDrawing   ={STRINGVALUE,{0},0};
VARIABLE gEditor         ={STRINGVALUE,{0},0};
VARIABLE gPSViewerName   ={STRINGVALUE,{0},0};
VARIABLE gWebViewerName  ={STRINGVALUE,{0},0};
VARIABLE gWebAddress     ={STRINGVALUE,{0},0};
/*                                     */
/* New variables for Marker databases: */
/*                                     */
VARIABLE gfnAllele1      ={FIELDNAMEVALUE,{0},MISSING};
VARIABLE gfnAllele2      ={FIELDNAMEVALUE,{0},MISSING};
/*                                        */
/* New fields for graphing functionality: */
/*                                        */
VARIABLE gfnGraphPosition={FIELDNAMEVALUE,{0},MISSING};
VARIABLE gfnGraphScore   ={FIELDNAMEVALUE,{0},MISSING};
VARIABLE gGraphTitle     ={STRINGVALUE,{0},0};
VARIABLE gGraphXAxisLabel={STRINGVALUE,{0},0};
VARIABLE gGraphYAxisLabel={STRINGVALUE,{0},0};
VARIABLE gGraphXAxisMin  ={NUMERICVALUE,{0},0};
VARIABLE gGraphXAxisMax  ={NUMERICVALUE,{0},0};
VARIABLE gGraphYAxisMin  ={NUMERICVALUE,{0},0};
VARIABLE gGraphYAxisMax  ={NUMERICVALUE,{0},0};
VARIABLE gGraphXMajorTick={NUMERICVALUE,{0},0};
VARIABLE gGraphXMinorTick={NUMERICVALUE,{0},0};
VARIABLE gGraphYMajorTick={NUMERICVALUE,{0},0};
VARIABLE gGraphYMinorTick={NUMERICVALUE,{0},0};

VARIABLE gColorBlack      ={STRINGVALUE,{0},0};
VARIABLE gColorBlue       ={STRINGVALUE,{0},0};
VARIABLE gColorBrown      ={STRINGVALUE,{0},0};
VARIABLE gColorCyan       ={STRINGVALUE,{0},0};
VARIABLE gColorDarkGray   ={STRINGVALUE,{0},0};
VARIABLE gColorForestGreen={STRINGVALUE,{0},0};
VARIABLE gColorGray       ={STRINGVALUE,{0},0};
VARIABLE gColorGreen      ={STRINGVALUE,{0},0};
VARIABLE gColorLightGray  ={STRINGVALUE,{0},0};
VARIABLE gColorMagenta    ={STRINGVALUE,{0},0};
VARIABLE gColorOrange     ={STRINGVALUE,{0},0};
VARIABLE gColorPeach      ={STRINGVALUE,{0},0};
VARIABLE gColorPurple     ={STRINGVALUE,{0},0};
VARIABLE gColorRed        ={STRINGVALUE,{0},0};
VARIABLE gColorWhite      ={STRINGVALUE,{0},0};
VARIABLE gColorYellow     ={STRINGVALUE,{0},0};

/*                                     */
/* Fields for allele frequency tables: */
/*                                     */
/* NOTE: Allele frequency tables also  */
/*       use gfnMapMarkerField         */
/*                                     */
VARIABLE gfnAlleleField    = {FIELDNAMEVALUE,{0},MISSING};
VARIABLE gfnFrequencyField = {FIELDNAMEVALUE,{0},MISSING};

/*                                                                */
/* GLOBAL VARIABLES for printing pedigree drawings:               */
/*                                                                */
/* Note: A CELL and REPORT object from the                        */
/* gprint.c library are used for pedigree drawing                 */
/* management, but they are used in ways that differ              */
/* from usage in a typical report, since a technical              */
/* drawing, rather than a table or report, is                     */
/* generated in this program:                                     */
/*                                                                */
/* PREPORT grpt=NULL;   // global report                          */
/*                                                                */
PPEDIGREEDRAWING gDrawing=NULL; /* global pedigree drawing object */
/*                                                                */
/* 2000.09.23 ET Addenda: A-Z scratch variables                   */
/*                                                                */
VARIABLE ScratchVar_A    ={NUMERICVALUE,{0},0};
VARIABLE ScratchVar_B    ={NUMERICVALUE,{0},0};
VARIABLE ScratchVar_C    ={NUMERICVALUE,{0},0};
VARIABLE ScratchVar_D    ={NUMERICVALUE,{0},0};
VARIABLE ScratchVar_E    ={NUMERICVALUE,{0},0};
VARIABLE ScratchVar_F    ={NUMERICVALUE,{0},0};
VARIABLE ScratchVar_G    ={NUMERICVALUE,{0},0};
VARIABLE ScratchVar_H    ={NUMERICVALUE,{0},0};
VARIABLE ScratchVar_I    ={NUMERICVALUE,{0},0};
VARIABLE ScratchVar_J    ={NUMERICVALUE,{0},0};
VARIABLE ScratchVar_K    ={NUMERICVALUE,{0},0};
VARIABLE ScratchVar_L    ={NUMERICVALUE,{0},0};
VARIABLE ScratchVar_M    ={NUMERICVALUE,{0},0};
VARIABLE ScratchVar_N    ={NUMERICVALUE,{0},0};
VARIABLE ScratchVar_O    ={NUMERICVALUE,{0},0};
VARIABLE ScratchVar_P    ={NUMERICVALUE,{0},0};
VARIABLE ScratchVar_Q    ={NUMERICVALUE,{0},0};
VARIABLE ScratchVar_R    ={NUMERICVALUE,{0},0};
VARIABLE ScratchVar_S    ={NUMERICVALUE,{0},0};
VARIABLE ScratchVar_T    ={NUMERICVALUE,{0},0};
VARIABLE ScratchVar_U    ={NUMERICVALUE,{0},0};
VARIABLE ScratchVar_V    ={NUMERICVALUE,{0},0};
VARIABLE ScratchVar_W    ={NUMERICVALUE,{0},0};
VARIABLE ScratchVar_X    ={NUMERICVALUE,{0},0};
VARIABLE ScratchVar_Y    ={NUMERICVALUE,{0},0};
VARIABLE ScratchVar_Z    ={NUMERICVALUE,{0},0};


/*///////////////////////////////////////////                       */
/*                                                                  */
/* global METHODS for handling different file                       */
/* output formats:                                                  */
/*                                                                  */
/* A METHODS structure contains function pointers for:              */
/*                                                                  */
/* processing locus files:                                          */
/* ---------------------------                                      */
/* 1. void (*OutputLocusHeader)()                                   */
/* 2. void (*OutputMarker     )()                                   */
/* 3. void (*OutputLocusFooter)()                                   */
/*                                                                  */
/* processing pedigree or data files:                               */
/* ----------------------------------                               */
/* 4. void (*OutputHeader  )()                                      */
/* 5. void (*OutputPedigree)()                                      */
/* 6. void (*OutputFooter  )()                                      */
/*                                                                  */
/* function to display summary statistics:                          */
/* ---------------------------------------                          */
/* 7. void (*ShowStatistics)()                                      */
/*                                                                  */
/* Note that some output formats don't have any                     */
/* special header or footer, and so can be NULL.                    */
/*                                                                  */
/* Also, there may be only a few "locus" file                       */
/* formats, but many different data file formats.                   */
/* Most programs use just one locus file format,                    */
/* but may rely on different data file formats for                  */
/* different analyses or modules.                                   */
/*                                                                  */
/* Note that function pointers numbered 1,3,4 and 6                 */
/* point to functions that, because they write the                  */
/* headers and footers, need to be called just once                 */
/* per file write.  There is one subtlety that the                  */
/* reader should be aware of: OutputFooter() might actually         */
/* do any sort of post-processing required, not just                */
/* writing a footer at the sequential end of a data file.           */
/* In fact, most formats have no need of a file footer              */
/* at all.  But there are formats, like CRIMAP, where               */
/* information obtained in the process of writing pedigree          */
/* data itself needs to be inserted back up in the header           */
/* of the file.  Since I deemed it easiest to do this after         */
/* writing out the pedigree data, the CrimapFooter() function       */
/* actually writes some information for the header, then copies     */
/* in the pedigree data already written to disk.  So, JUST          */
/* REMEMBER that OutputHeader() can perform any kind of             */
/* PRE-processing required prior to actually writing the pedigree   */
/* data, and OutputFooter() can perform any kind of POST-processing */
/* after writing out pedigree data, as required by the program.     */
/*                                                                  */
/* In contrast, OutputMarker() writes                               */
/* out the allele frequency data for a single marker and            */
/* so a loop is required to call OutputMarker() for                 */
/* each marker field marked as a GENOTYPE OUTPUT field.             */
/*                                                                  */
/* Similarly, OutputPedigree() writes out the data for              */
/* a single pedigree.  Since PEDIGREEs are nodes in a               */
/* binary sorted tree in Madeline (in fact, the BST is now          */
/* implemented as a red-black tree which exhibits superior          */
/* balance regardless of the order in which the data occur          */
/* in the database), a traversal function that visits               */
/* each node in order is required.  The traversal function used     */
/* is called TraverseBinaryPedigreeTree() which is implemented      */
/* as a recursive function that calls itself for each left          */
/* and right node present.                                          */
/*                                                                  */
/*///////////////////////////////////////////                       */

/*//////////// */
/*             */
/* ASPEX:      */
/*             */
/* -OK:        */
/*             */
/*//////////// */
METHODS Aspex={

	NULL,
	NULL,
	NULL,
	
	AspexDataHeaderProcessor,
	AspexNuclearBlockOutput,
	GenericDataFooterProcessor,
	
	ShowNuclearStatistics

};

/*////////////// */
/*               */
/* LINKAGE:      */
/*               */
/*////////////// */
METHODS Linkage={

	NULL,
	NULL,
	NULL,
	
	NULL, /* LinkageDataHeaderProcessor, */
	NULL, /* LinkageOutput,              */
	NULL, /* GenericDataFooterProcessor  */
	
	NULL
};

/*////////////////// */
/*                   */
/* GENERIC:          */
/*                   */
/*                   */
/*                   */
/*////////////////// */
METHODS GenericDelimited={

	NULL,
	NULL,
	NULL,
	
	GenericDataHeaderProcessor,
	DelimitedOutput,
	GenericDataFooterProcessor,
	
	ShowPedigreeStatistics
};


/*////////////////// */
/*                   */
/* LDEQHMethod:      */
/*                   */
/*                   */
/*                   */
/*////////////////// */
METHODS LDEQHMethod={

	NULL,
	NULL,
	NULL,
	
	GenericDataHeaderProcessor,
	LDEQHOutput,
	GenericDataFooterProcessor,
	
	ShowPedigreeStatistics
};



/*////////////// */
/*               */
/* CRIMAP        */
/*               */
/* -OK:          */
/*               */
/*////////////// */
METHODS Crimap={

	NULL,
	NULL,
	NULL,
	
	CrimapDataHeaderProcessor,
	CrimapOutput,
	CrimapDataFooterProcessor,
	
	ShowPedigreeStatistics

};



/*////////////////// */
/*                   */
/* PEDCHECK:         */
/*                   */
/*////////////////// */
METHODS PedCheck={
	/*                                                     */
	/* PEDCHECK is a LINKAGE-based format very similar     */
	/* to the GENEHUNTER format, so OutputGenehunterMarker */
	/* can be used intact, unchanged:                      */
	/*                                                     */
	OutputPedCheckLocusFileHeader,
	OutputGenehunterMarker,
	OutputPedCheckLocusFileFooter,
	/*                                                            */
	/* PedCheckOutput() calls some of the Genehunter routines,    */
	/* so AffectionStatusField is still required for this format: */
	/*                                                            */
	PedCheckDataHeaderProcessor,
	PedCheckOutput,
	GenericDataFooterProcessor,
	
	ShowPedigreeStatistics
};

/*///////// */
/*          */
/* GENERIC: */
/*          */
/* -OK:     */
/*          */
/*///////// */
METHODS Generic={

	GenericLocusHeaderProcessor,
	OutputGenericMarker,
	GenericLocusFooterProcessor,
	
	NULL,
	NULL,
	NULL,
	
	ShowPedigreeStatistics

};

/*//////// */
/*         */
/* MENDEL: */
/*         */
/* -OK:    */
/*         */
/*//////// */
/*                                                       */
/* NOTE BENE: THE NEW MENDEL 4.1 STUFF IS IDENTICAL TO   */
/* SIMWALK2 FORMATS, SO THIS IS NOW IN simwalk_format.c: */
/*                                                       */

/*//////// */
/*         */
/* FISHER: */
/*         */
/*         */
/*         */
/*//////// */
METHODS Fisher={

	GenericLocusHeaderProcessor,
	OutputMendelMarker,
	GenericLocusFooterProcessor,
	
	FisherDataHeaderProcessor,
	FisherOutput,
	GenericDataFooterProcessor,
	
	ShowPedigreeStatistics

};



/*////////  */
/*          */
/* USERM13: */
/*          */
/*          */
/*          */
/*////////  */
METHODS UserM13={

	GenericLocusHeaderProcessor,
	OutputMendelMarker,
	GenericLocusFooterProcessor,
	
	UserM13DataHeaderProcessor,
	UserM13Output,
	GenericDataFooterProcessor,
	
	ShowPedigreeStatistics

};


/*/////////////////                */
/*                                 */
/* SAGE Locus File format:         */
/*                                 */
/* Other METHODS refer to specific */
/* SAGE data formats:              */
/*                                 */
/* -OK:                            */
/*                                 */
/*/////////////////                */
METHODS Sage={

	GenericLocusHeaderProcessor,
	OutputSageMarker,
	GenericLocusFooterProcessor,

	NULL,
	NULL,
	NULL,
	
	ShowPedigreeStatistics

};

/*/////////////////  */
/*                   */
/* SAGE FSP Formats  */
/*                   */
/* -OK:              */
/*                   */
/*/////////////////  */
METHODS SageFSP={

	GenericLocusHeaderProcessor,
	OutputSageMarker,
	GenericLocusFooterProcessor,
	
	FSPDataHeaderProcessor,
	FSPOutput,
	GenericDataFooterProcessor,
	
	ShowPedigreeStatistics

};


/*//////////////////// */
/*                     */
/* SAGE Sibpal Formats */
/*                     */
/* -OK:                */
/*                     */
/*//////////////////// */
METHODS SageSibpal={

	GenericLocusHeaderProcessor,
	OutputSageMarker,
	GenericLocusFooterProcessor,
	
	SibpalDataHeaderProcessor,
	SibpalOutput,
	GenericDataFooterProcessor,
	
	ShowPedigreeStatistics

};

/*////////////////////                         */
/*                                             */
/* SIBLINK Formats                             */
/*                                             */
/* The locus file methods are NULL because     */
/* the locus file is created at the same time  */
/* as the data file: the parameter file stream */
/* is used for output.                         */
/*                                             */
/* -OK:                                        */
/*                                             */
/*////////////////////                         */
METHODS Siblink={

	NULL,
	NULL,
	NULL,
	
	SiblinkDataHeaderProcessor,
	SiblinkOutput,
	SiblinkDataFooterProcessor,
	
	ShowNuclearStatistics
};


/*/////////////////////// */
/*                        */
/* SOLAR Relate Format    */
/*                        */
/*/////////////////////// */
METHODS SolarRelate={

	NULL,
	NULL,
	NULL,
	
	GenericDataHeaderProcessor,
	SolarRelateOutput,
	GenericDataFooterProcessor,
	
	ShowPedigreeStatistics
	
};


/*/////////////////////// */
/*                        */
/* SOLAR Fisher Format:   */
/*                        */
/*/////////////////////// */
METHODS SolarFisher={

	NULL,
	NULL,
	NULL,
	
	GenericDataHeaderProcessor,
	SolarFisherOutput,
	GenericDataFooterProcessor,
	
	ShowPedigreeStatistics
	
};

/*/////////////////////////   */
/*                            */
/* USERFQTLNUCLEAR Formats:   */
/*                            */
/* USERFQTL is a MENDEL-based */
/* FORTRAN program written by */
/* Mike Boehnke for doing     */
/* Quantative Traits Linkage  */
/* analysis:                  */
/*                            */
/* -                          */
/*                            */
/*/////////////////////////   */
METHODS UserFQTLNuclear={

	UserFQTLLocusHeaderProcessor,
	OutputUserFQTLMarker,
	GenericLocusFooterProcessor,
	
	UserFQTLDataHeaderProcessor,
	UserFQTLNuclearBlockOutput,
	GenericDataFooterProcessor,
	
	ShowNuclearStatistics

};


/*/////////////////////////                   */
/*                                            */
/* RELPAIR Format:                            */
/*                                            */
/* The relpair locus file format              */
/* is very similar to UserFQTL format         */
/*                                            */
/* The relpair pedigree file format           */
/* is identical to the Mendel format          */
/*                                            */
/* Like USERM13, everybody, even unconnecteds */
/* and childless spouses can be included      */
/* in pedigrees:                              */
/*                                            */
/* -OK:                                       */
/*                                            */
/*/////////////////////////                   */
METHODS Relpair={

	RelpairLocusHeaderProcessor,
	OutputRelpairMarker,
	GenericLocusFooterProcessor,
	
	RelpairDataHeaderProcessor,
	RelpairOutput,
	GenericDataFooterProcessor,
	
	ShowPedigreeStatistics

};


/*///////////////////////           */
/*                                  */
/* HOLDER: Used as a place holder   */
/*         when formats are not     */
/*         yet defined or are still */
/*         incomplete               */
/*                                  */
/*///////////////////////           */
METHODS Holder={

	NULL,
	NULL,
	NULL,
	
	NULL,
	NULL,
	NULL,
	
	NULL
};

/* ************ */
/*              */
/* DATA FORMATS */
/*              */
/* ************ */

/* in merlin_qtdt_format.c: */
extern METHODS MerlinQTDT;
/* in simwalk_format.c: */
extern METHODS Simwalk;

extern METHODS Mendel;
/* in genehunter_format.c: */
extern METHODS Genehunter;


/*                                   */
/* KnownTokens tree: global for now: */
/*                                   */
// PNODE KnownTokens=NULL;

// 
// Global token tree:
// 
TokenTree knownTokens;

/*            */
/* Help text: */
/*            */
char *HelpText;

/*         */
/*         */
/* main(): */
/*         */
/*         */
/*         */
#define RESULTBUFFERSIZE  100

int main(int argc,char *argv[]){

	FILE *source;
	
	
	/* char *ss; */

	int i;

	char hold[SMALLBUFFERSIZE];
	/*                   */
	/* Global help text: */
	/*                   */
	extern char *HelpText;
		
	/*                           */
	/* global CONTROL structure: */
	/*                           */
	/*                           */
	extern PCONTROL g;

	/*              */
	/* KnownTokens: */
	/*              */
	//extern PNODE KnownTokens;

	/*                                                           */
	/* Check for -v, -h, --version or --help                     */
	/* arguments on command line:                                */
	/*                                                           */
	for(i=1;i<argc;i++){
		if(*argv[i]=='-'){
			if( argv[i][1]=='h' || argv[i][1]=='v' || !strcmp(argv[i],"--version") || !strcmp(argv[i],"--help") ){
				fprintf(stdout,"\nMADELINE version %6.3f by Edward H. Trager\n",VERSION);
				fprintf(stdout,"COPYRIGHT © %4i THE REGENTS OF THE UNIVERSITY OF MICHIGAN\n",COPYRIGHTYEAR);
				fprintf(stdout,"PORTIONS COPYRIGHT © 1995 EDWARD H. TRAGER\n");
				fprintf(stdout,"ALL RIGHTS RESERVED\n\n");
				fprintf(stdout,"MADELINE comes with ABSOLUTELY NO WARRANTY.  This is free software\n");
				fprintf(stdout,"and you are welcome to redistribute it under certain conditions.\n");
				fprintf(stdout,"For details, type \"license\" at the Madeline prompt and see the GNU\n");
				fprintf(stdout,"General Public License which accompanied this distribution.\n\n");
				fprintf(stdout,"USAGE: madeline [-h | --help | -v | --version] [<command_script>]\n\n");
				fprintf(stdout,"To use interactively, type \"madeline\" and press return.\n");
				fprintf(stdout,"At the prompt, type \"help 'contents'\" for help.\n\n");

				exit(0);
			}
		}
	}	
	
	/*                                                           */
	/* Set up the locale according to the environment variables: */
	/* This allows us to use the wide character string functions */
	/* on UTF-8 strings:                                         */
	/*                                                           */
	setlocale(LC_ALL, "" );
	
	/* Initialize the history library: */
	using_history();
	
	/*                                                                         */
	/* KnownTokens:                                                            */
	/*                                                                         */
	/* Global BST table of recognized commands, internal variables, and        */
	/* static internal numeric constants.                                      */
	/*                                                                         */
	/* The KnownTokens tree is built using InstallKnownToken()                 */
	/* which basically provides a wrapper around tree.c's HeadNode() and       */
	/* AddNode() functions, but additionally provides help text mapping:       */
	/*                                                                         */
	/* Each RECOGNIZEDTOKEN entry consists of:                                 */
	/*                                                                         */
	/* -external token representation: should be INCAPITALLETTERSLIKETHIS      */
	/* -"Properized" representation  : should be InCapitalAndSmallCaseLikeThis */
	/*   The properized representation is used to facilitate readability only  */
	/* -internal token identifier defined in parser.h                          */
	/* -token type                                                             */
	/* -double value if token type is NUMERICVALUE                             */
	/* -pointer to function, char string, array of doubles or array of char *  */
	/*                                                                         */
	/* Note that in ANSI C a union can only be intitialized by the type of the */
	/* first member, so in the definition of UNION we have put "void *" first  */
	/* and this is why here we cast the pointer of each RECOGNIZEDTOKEN        */
	/* in the vector to void pointer type:                                     */
	/*                                                                         */
	knownTokens.install("ABOVE"                ,"above"                ,ABOVK,KEYWORD      ,0   ,(void *)NULL         );
	knownTokens.install("ABS"                  ,"abs"                  ,FABS ,NFUNCTION    ,0   ,(void *)fabs         );
	knownTokens.install("ACOS"                 ,"acos"                 ,ACOS ,NFUNCTION    ,0   ,(void *)acos         );
	knownTokens.install("ADD"                  ,"add"                  ,ADDK ,KEYWORD      ,0   ,(void *)NULL         );
	knownTokens.install("AFFECTIONSTATUSFIELD" ,"AffectionStatusField" ,AFFFD,INTERNALVARIABLE,0,(void *)&gfnAffected );
	knownTokens.install("ALLELE"               ,"allele"               ,ALLEK,KEYWORD      ,0   ,(void *)NULL         );
	knownTokens.install("ALLELEFIELD"          ,"AlleleField"          ,ALEFD,INTERNALVARIABLE,0,(void *)&gfnAlleleField);
	knownTokens.install("ALLELE1FIELD"         ,"Allele1Field"         ,AL1FD,INTERNALVARIABLE,0,(void *)&gfnAllele1  );
	knownTokens.install("ALLELE2FIELD"         ,"Allele2Field"         ,AL2FD,INTERNALVARIABLE,0,(void *)&gfnAllele2  );
	knownTokens.install("AMERICAN"             ,"American"             ,LAMER,KEYWORD      ,0   ,(void *)NULL         );
	knownTokens.install("AMERICANENGLISH"      ,"AmericanEnglish"      ,LAMER,KEYWORD      ,0   ,(void *)NULL         );
	knownTokens.install("AND"                  ,"and"                  ,AND  ,OPERATOR     ,0   ,(void *)NULL         );
	knownTokens.install("ARABIC"               ,"Arabic"               ,LARAB,KEYWORD      ,0   ,(void *)NULL         );
	knownTokens.install("ARROW"                ,"arrow"                ,ARRWK,KEYWORD      ,0   ,(void *)NULL         );
	knownTokens.install("AS"                   ,"as"                   ,AS_KY,KEYWORD      ,0   ,(void *)NULL         );
	knownTokens.install("AT"                   ,"at"                   ,AT_KY,KEYWORD      ,0   ,(void *)NULL         );
	knownTokens.install("ASCENDING"            ,"ascending"            ,SASCK,KEYWORD      ,0   ,(void *)NULL         );
	knownTokens.install("ASIN"                 ,"asin"                 ,ASIN ,NFUNCTION    ,0   ,(void *)asin         );
	knownTokens.install("ATAN"                 ,"atan"                 ,ATAN ,NFUNCTION    ,0   ,(void *)atan         );
	knownTokens.install("AUTOCHECKINHERITANCE" ,"AutoCheckInheritance" ,AINHK,KEYWORD      ,0   ,(void *)NULL         );
	knownTokens.install("AUTODETECT"           ,"AutoDetect"           ,AUTOK,KEYWORD      ,0   ,(void *)NULL         );
	knownTokens.install("AUTOEXCLUDE"          ,"AutoExclude"          ,AXCLK,KEYWORD      ,0   ,(void *)NULL         );
	knownTokens.install("AUTOMATICALLY"        ,"automatically"        ,AUTOMK,KEYWORD     ,0   ,(void *)NULL         );
	knownTokens.install("BANNER"               ,"banner"               ,BANC ,COMMAND      ,0   ,(void *)Banner       );
	knownTokens.install("BAR"                  ,"bar"                  ,BAR_K,KEYWORD      ,0   ,(void *)NULL         );
	knownTokens.install("BELOW"                ,"below"                ,BELOK,KEYWORD      ,0   ,(void *)NULL         );
	knownTokens.install("BRITISH"              ,"British"              ,LBRIT,KEYWORD      ,0   ,(void *)NULL         );
	knownTokens.install("BRITISHENGLISH"       ,"BritishEnglish"       ,LBRIT,KEYWORD      ,0   ,(void *)NULL         );
	knownTokens.install("CAS"                  ,"CharacterAffectionStatus",CAS  ,INTERNALARRAY,0   ,(void *)(PARRAY)&Caff);
	knownTokens.install("CDS"                  ,"CharacterDeathStatus" ,CDS  ,INTERNALARRAY,0   ,(void *)(PARRAY)&Cds );
	knownTokens.install("CEILING"              ,"ceiling"              ,CEIL ,NFUNCTION    ,0   ,(void *)ceil         );
	knownTokens.install("CENTIMETER"           ,"centimeter"           ,CM__K,KEYWORD      ,0   ,(void *)NULL         );
	knownTokens.install("CENTIMETERS"          ,"centimeters"          ,CM__K,KEYWORD      ,0   ,(void *)NULL         );
	knownTokens.install("CENTIMORGAN"          ,"centiMorgan"          ,CMG_K,KEYWORD      ,0   ,(void *)NULL         );
	knownTokens.install("CENTIMORGANS"         ,"centiMorgans"         ,CMG_K,KEYWORD      ,0   ,(void *)NULL         );
	knownTokens.install("CHARACTERAFFECTIONSTATUS","CharacterAffectionStatus",CAS,INTERNALARRAY,0   ,(void *)(PARRAY)&Caff   );
	knownTokens.install("CHARACTERDEATHSTATUS" ,"CharacterDeathStatus" ,CDS  ,INTERNALARRAY,0   ,(void *)(PARRAY)&Cds  );
	knownTokens.install("CHARACTERMISSINGVALUE","CharacterMissingValue",CMV  ,INTERNALARRAY,0   ,(void *)(PARRAY)&Cmv  );
	knownTokens.install("CHARACTERSEXVALUE"    ,"CharacterSexValue"    ,CSV  ,INTERNALARRAY,0   ,(void *)(PARRAY)&Csv  );
	knownTokens.install("CHECK"                ,"check"                ,CHEKC,COMMAND      ,0   ,(void *)Check         );
	knownTokens.install("CHINESE"              ,"Chinese"              ,LCHIN,KEYWORD      ,0   ,(void *)NULL          );
	knownTokens.install("CHROMOSOME"           ,"chromosome"           ,CHRMK,KEYWORD      ,0   ,(void *)NULL          );
	knownTokens.install("CLASSFIELD"           ,"ClassField"           ,CLSFD,INTERNALVARIABLE,0,(void *)&gfnClass     );
	knownTokens.install("CLEAR"                ,"clear"                ,CLRC ,COMMAND      ,0   ,(void *)Clear         );
	knownTokens.install("CM"                   ,"cm"                   ,CM__K,KEYWORD      ,0   ,(void *)NULL          );
	knownTokens.install("CMV"                  ,"CharacterMissingValue",CMV  ,INTERNALARRAY,0   ,(void *)(PARRAY)&Cmv  );
	knownTokens.install("COMMA"                ,"comma"                ,CMMAK,KEYWORD      ,0   ,(void *)NULL          );
	knownTokens.install("COMMANDFILE"          ,"CommandFile"          ,CMDFV,INTERNALVARIABLE,0,(void *)&gCommandFile );
	knownTokens.install("COMPOSE"              ,"compose"              ,COMPC,COMMAND      ,0   ,(void *)Compose       );
	knownTokens.install("CONSOLEHIGHLIGHTS"    ,"ConsoleHighlights"    ,CNSLK,KEYWORD      ,0   ,(void *)NULL          );
	knownTokens.install("CONVERT"              ,"convert"              ,CONVC,COMMAND      ,0   ,(void *)Convert       );
	knownTokens.install("COS"                  ,"cos"                  ,COS  ,NFUNCTION    ,0   ,(void *)cos           );
	knownTokens.install("COSH"                 ,"cosh"                 ,COSH ,NFUNCTION    ,0   ,(void *)cosh          );
	knownTokens.install("COVARIATE"            ,"covariate"            ,COVAK,KEYWORD      ,0   ,(void *)NULL          );
	knownTokens.install("CRIMAP"               ,"crimap"               ,CRIMK,KEYWORD      ,0   ,(void *)&Crimap       );
	knownTokens.install("CSV"                  ,"CharacterSexValue"    ,CSV  ,INTERNALARRAY,0   ,(void *)(PARRAY)&Csv  );
	knownTokens.install("CURRENTSIBLINK"       ,"CurrentSiblink"       ,CSIBL,KEYWORD      ,0   ,(void *)&Siblink      );
	knownTokens.install("DATA"                 ,"data"                 ,DATAK,KEYWORD      ,0   ,(void *)NULL          );
	knownTokens.install("DATABASEFILE"         ,"DatabaseFile"         ,DBFFV,INTERNALVARIABLE,0,(void *)&gDatabaseFile);
	knownTokens.install("DATEOFBIRTHFIELD"     ,"DateOfBirthField"     ,DOBFV,INTERNALVARIABLE,0,(void *)&gfnDOB       );
	knownTokens.install("DATEOFDEATHFIELD"     ,"DateOfDeathField"     ,DODFV,INTERNALVARIABLE,0,(void *)&gfnDOD       );
	knownTokens.install("DEATHSTATUSFIELD"     ,"DeathStatusField"     ,DSFD ,INTERNALVARIABLE,0,(void *)&gfnDead      );
	knownTokens.install("DECOMPOSE"            ,"Decompose"            ,DECOC,COMMAND      ,0   ,(void *)Decompose     );
	knownTokens.install("DEGREES"              ,"degrees"              ,DEGRK,KEYWORD      ,0   ,(void *)NULL          );
	knownTokens.install("DELIMITED"            ,"delimited"            ,DLMAK,KEYWORD      ,0   ,(void *)&GenericDelimited);
	knownTokens.install("DELIMITER"            ,"delimiter"            ,DLMBK,KEYWORD      ,0   ,(void *)NULL          );
	knownTokens.install("DESCENDING"           ,"descending"           ,SDESK,KEYWORD      ,0   ,(void *)NULL          );
	knownTokens.install("DETAILFILE"           ,"DetailFile"           ,DETFV,INTERNALVARIABLE,0,(void *)&gDetailFile  );
	knownTokens.install("DISTINCT"             ,"distinct"             ,UNIQK,KEYWORD      ,0   ,(void *)NULL          );
	knownTokens.install("DRAW"                 ,"draw"                 ,DRAWC,COMMAND      ,0   ,(void *)Draw          );
	knownTokens.install("DZTWINFIELD"          ,"DZTwinField"          ,DZTFD,INTERNALVARIABLE,0,(void *)&gfnDZTwin    );
	knownTokens.install("EDIT"                 ,"edit"                 ,EDITC,COMMAND      ,0   ,(void *)Edit          );
	knownTokens.install("EDITOR"               ,"editor"               ,EDITV,INTERNALVARIABLE,0,(void *)&gEditor      );
	knownTokens.install("ERRORFILE"            ,"ErrorFile"            ,ERRFV,INTERNALVARIABLE,0,(void *)&gErrorFile   );
	knownTokens.install("EVALUATIONINTERVAL"   ,"EvaluationInterval"   ,NV_EI,INTERNALVARIABLE,0,(void *)&EvaluationInterval);
	knownTokens.install("EXCLUDE"              ,"exclude"              ,EXCLC,COMMAND      ,0   ,(void *)Exclude       );
	knownTokens.install("EXCLUSIONS"           ,"exclusions"           ,EXCLK,KEYWORD      ,0   ,(void *)NULL          );
	knownTokens.install("EXIT"                 ,"exit (goodbye, quit)" ,QUITC,COMMAND      ,0   ,(void *)Quit        );
	knownTokens.install("EXP"                  ,"exp"                  ,EXP  ,NFUNCTION    ,0   ,(void *)exp           );
	knownTokens.install("FAMILIES"             ,"families"             ,FAMK ,KEYWORD      ,0   ,(void *)NULL          );
	knownTokens.install("FAMILY"               ,"family"               ,FAMK ,KEYWORD      ,0   ,(void *)NULL          );
	knownTokens.install("FAMILYIDFIELD"        ,"FamilyIDField"        ,FAMFD,INTERNALVARIABLE,0,(void *)&gfnFamID     );
	knownTokens.install("FATHERIDFIELD"        ,"FatherIDField"        ,FIDFD,INTERNALVARIABLE,0,(void *)&gfnFather    );
	knownTokens.install("FIELD"                ,"field"                ,FLDK ,KEYWORD      ,0   ,(void *)NULL        );
	knownTokens.install("FIELDS"               ,"fields"               ,FLDK ,KEYWORD      ,0   ,(void *)NULL        );
	knownTokens.install("FILE"                 ,"file"                 ,FILEK,KEYWORD      ,0   ,(void *)NULL        );
	knownTokens.install("FILES"                ,"file"                 ,FILEK,KEYWORD      ,0   ,(void *)NULL        );
	knownTokens.install("FINNISH"              ,"Finnish (Suomi)"      ,LSUOM,KEYWORD      ,0   ,(void *)NULL        );
	knownTokens.install("FISHER0"              ,"fisher0"              ,FISH0,KEYWORD      ,0   ,(void *)&Fisher     );
	knownTokens.install("FISHER1"              ,"fisher1"              ,FISH1,KEYWORD      ,0   ,(void *)&Fisher     );
	knownTokens.install("FLAG"                 ,"flag"                 ,FLAGK,KEYWORD      ,0   ,(void *)NULL        );
	knownTokens.install("FLAGS"                ,"flags"                ,FLAGK,KEYWORD      ,0   ,(void *)NULL        );
	knownTokens.install("FLOATERS"             ,"floaters"             ,FLOAK,KEYWORD      ,0   ,(void *)NULL        );
	knownTokens.install("FLOOR"                ,"floor"                ,FLOOR,NFUNCTION    ,0   ,(void *)floor       );
	knownTokens.install("FOR"                  ,"for"                  ,FORK ,COMMAND,0   ,(void *)ExecuteForStatement);
	knownTokens.install("FORMAT"               ,"format"               ,FORMC,KEYWORD      ,0   ,(void *)NULL        );
	knownTokens.install("FREQUENCIES"          ,"frequencies"          ,FREQK,KEYWORD      ,0   ,(void *)NULL         );
	knownTokens.install("FREQUENCYFIELD"       ,"FrequencyField"       ,FRQFD,INTERNALVARIABLE,0,(void *)&gfnFrequencyField);
	knownTokens.install("FRENCH"               ,"French"               ,LFREN,KEYWORD      ,0   ,(void *)NULL        );
	knownTokens.install("FROM"                 ,"from"                 ,FROMK,KEYWORD      ,0   ,(void *)NULL        );
	knownTokens.install("FSP"                  ,"fsp"                  ,FSPK ,KEYWORD      ,0   ,(void *)&SageFSP    );
	knownTokens.install("FSP0"                 ,"fsp0"                 ,FSP0K,KEYWORD      ,0   ,(void *)&SageFSP    );
	knownTokens.install("GENDERFIELD"          ,"GenderField"          ,SEXFD,INTERNALVARIABLE,0,(void *)&gfnSex     );
	knownTokens.install("GENEHUNTER"           ,"genehunter"           ,GHK  ,KEYWORD      ,0   ,(void *)&Genehunter );
	knownTokens.install("GENEHUNTERNPL"        ,"GenehunterNPL"        ,GHNPK,KEYWORD      ,0   ,(void *)&Genehunter );
	knownTokens.install("GENEHUNTERQTL"        ,"GenehunterQTL"        ,GHQTL,KEYWORD      ,0   ,(void *)&Genehunter );
	knownTokens.install("GENERIC"              ,"generic"              ,GENER,KEYWORD      ,0   ,(void *)&Generic    );
	knownTokens.install("GENOTYPE"             ,"genotype"             ,GENEK,KEYWORD      ,0   ,(void *)NULL        );
	knownTokens.install("GERMAN"               ,"German"               ,LGERM,KEYWORD      ,0   ,(void *)NULL        );
	knownTokens.install("GO"                   ,"go"                   ,GOC  ,COMMAND      ,0   ,(void *)Go          );
	knownTokens.install("GOODBYE"              ,"goodbye (exit, quit)" ,QUITC,COMMAND      ,0   ,(void *)Quit        );
	knownTokens.install("GRAPH"                ,"graph"                ,GRAPH,COMMAND      ,0   ,(void *)Graph       );
	knownTokens.install("GRAPHDRAWINGFILE"     ,"GraphDrawingFile"     ,GFDRW,INTERNALVARIABLE,0,(void *)&gGraphDrawing );
	knownTokens.install("GRAPHPOSITIONFIELD"   ,"GraphPositionField"   ,GFPOS,INTERNALVARIABLE,0,(void *)&gfnGraphPosition);
	knownTokens.install("GRAPHSCOREFIELD"      ,"GraphScoreField"      ,GFSCR,INTERNALVARIABLE,0,(void *)&gfnGraphScore);
	knownTokens.install("GRAPHTITLE"           ,"GraphTitle"           ,GTITL,INTERNALVARIABLE,0,(void *)&gGraphTitle);
	knownTokens.install("GRAPHXAXISLABEL"      ,"GraphXAxisLabel"      ,GXAXL,INTERNALVARIABLE,0,(void *)&gGraphXAxisLabel);
	knownTokens.install("GRAPHYAXISLABEL"      ,"GraphYAxisLabel"      ,GYAXL,INTERNALVARIABLE,0,(void *)&gGraphYAxisLabel);
	knownTokens.install("GRAPHXAXISMINIMUM"    ,"GraphXAxisMinimum"    ,GXAMN,INTERNALVARIABLE,0,(void *)&gGraphXAxisMin);
	knownTokens.install("GRAPHXAXISMAXIMUM"    ,"GraphXAxisMaximum"    ,GXAMX,INTERNALVARIABLE,0,(void *)&gGraphXAxisMax);
	knownTokens.install("GRAPHYAXISMINIMUM"    ,"GraphYAxisMinimum"    ,GYAMN,INTERNALVARIABLE,0,(void *)&gGraphYAxisMin);
	knownTokens.install("GRAPHYAXISMAXIMUM"    ,"GraphYAxisMaximum"    ,GYAMX,INTERNALVARIABLE,0,(void *)&gGraphYAxisMax);
	knownTokens.install("GRAPHXAXISMAJORTICK"  ,"GraphXAxisMajorTick"  ,GXMAJ,INTERNALVARIABLE,0,(void *)&gGraphXMajorTick);
	knownTokens.install("GRAPHXAXISMINORTICK"  ,"GraphXAxisMinorTick"  ,GXMIN,INTERNALVARIABLE,0,(void *)&gGraphXMinorTick);
	knownTokens.install("GRAPHYAXISMAJORTICK"  ,"GraphYAxisMajorTick"  ,GYMAJ,INTERNALVARIABLE,0,(void *)&gGraphYMajorTick);
	knownTokens.install("GRAPHYAXISMINORTICK"  ,"GraphYAxisMinorTick"  ,GYMIN,INTERNALVARIABLE,0,(void *)&gGraphYMinorTick);
	knownTokens.install("GREEK"                ,"Greek"                ,LGREK,KEYWORD      ,0   ,(void *)NULL        );
	knownTokens.install("HELLO"                ,"hello (status)"       ,HELOC,COMMAND      ,0   ,(void *)Hello       );
	knownTokens.install("HORIZONTAL"           ,"horizontal"           ,HORZK,KEYWORD      ,0   ,(void *)NULL        );
	knownTokens.install("STATUS"               ,"status (hello)"       ,HELOC,COMMAND      ,0   ,(void *)Hello       );
	knownTokens.install("HELP"                 ,"help"                 ,HELPC,COMMAND      ,0   ,(void *)Help        );
	knownTokens.install("ICON"                 ,"icon"                 ,ICONK,KEYWORD      ,0   ,(void *)NULL        );
	knownTokens.install("IN"                   ,"in"                   ,INC  ,KEYWORD      ,0   ,(void *)NULL        );
	knownTokens.install("INDIVIDUALIDFIELD"    ,"IndividualIDField"    ,IIDFD,INTERNALVARIABLE,0,(void *)&gfnID      );
	knownTokens.install("INHERITANCE"          ,"inheritance"          ,INHRK,KEYWORD      ,0   ,(void *)NULL        );
	knownTokens.install("INHERITANCES"         ,"inheritances"         ,INHRK,KEYWORD      ,0   ,(void *)NULL        );
	knownTokens.install("INV"                  ,"inv"                  ,INV  ,NFUNCTION    ,0   ,(void *)Inverse     );
	knownTokens.install("IS"                   ,"is"                   ,ISC  ,KEYWORD      ,0   ,(void *)NULL        );
	knownTokens.install("ITALIAN"              ,"Italian"              ,LITAL,KEYWORD      ,0   ,(void *)NULL         );
	knownTokens.install("JAPANESE"             ,"Japanese"             ,LJAPN,KEYWORD      ,0   ,(void *)NULL        );
	knownTokens.install("DATETOJULIAN"         ,"DateToJulian"         ,JULDAY,TDRNFUNCTION,0   ,(void *)DateToJulian);
	knownTokens.install("JULIANTODATE"         ,"JulianToDate"         ,JULDAT,TNRDFUNCTION,0   ,(void *)JulianToDate);
	knownTokens.install("HALDANETOTHETA"       ,"HaldaneToTheta"       ,HL2TH,NFUNCTION    ,0   ,(void *)Haldane_cM_To_Theta);
	knownTokens.install("KOSAMBITOTHETA"       ,"KosambiToTheta"       ,KS2TH,NFUNCTION    ,0   ,(void *)Kosambi_cM_To_Theta);
	knownTokens.install("LABEL"                ,"label"                ,LABLK,KEYWORD      ,0   ,(void *)NULL         );
	knownTokens.install("LABELFONTSIZE"        ,"LabelFontSize"        ,LBSZ ,INTERNALVARIABLE,0,(void *)&LabelFontSize);
	knownTokens.install("LANGUAGE"             ,"language"             ,LANGK,KEYWORD      ,0   ,(void *)NULL        );
	knownTokens.install("LEGENDFONTSIZE"       ,"LegendFontSize"       ,LGSZ ,INTERNALVARIABLE,0,(void *)&LegendFontSize);
	knownTokens.install("LIABILITYCLASSFIELD"  ,"LiabilityClassField"  ,LCLFD,INTERNALVARIABLE,0,(void *)&gfnLClass   );
	knownTokens.install("LICENSE"              ,"license"              ,LISCC,COMMAND      ,0   ,(void *)License      );
	knownTokens.install("LINE"                 ,"line"                 ,LINEK,KEYWORD      ,0   ,(void *)NULL         );
	knownTokens.install("LINES"                ,"lines"                ,LINEK,KEYWORD      ,0   ,(void *)NULL         );
	knownTokens.install("LINKAGE"              ,"linkage"              ,LINKAG,KEYWORD     ,0   ,(void *)&Linkage     );
	knownTokens.install("LIST"                 ,"list"                 ,LISTC,COMMAND      ,0   ,(void *)List         );
	knownTokens.install("LOAD"                 ,"load"                 ,LOADC,COMMAND      ,0   ,(void *)Load         );
	knownTokens.install("LOCUS"                ,"locus"                ,LOCUS,KEYWORD      ,0   ,(void *)NULL         );
	knownTokens.install("LODUNIT"              ,"LodUnit"              ,VU__K,KEYWORD      ,0   ,(void *)NULL         );
	knownTokens.install("LODUNITS"             ,"LodUnits"             ,VU__K,KEYWORD      ,0   ,(void *)NULL         );
	knownTokens.install("LOD"                  ,"lod"                  ,LODK ,NFUNCTION    ,0   ,(void *)lod          );
	knownTokens.install("LOG"                  ,"log (ln)"             ,LOG  ,NFUNCTION    ,0   ,(void *)log          );
	knownTokens.install("LN"                   ,"ln (log)"             ,LOG  ,NFUNCTION    ,0   ,(void *)log          );
	knownTokens.install("LOG10"                ,"log10"                ,LOG10,NFUNCTION    ,0   ,(void *)log10        );
	knownTokens.install("ALLLOGFILES"          ,"AllLogFiles"          ,LOGFA,INTERNALVARIABLE,0,(void *)&gAllLogFiles);
	knownTokens.install("LOGFILE"              ,"LogFile"              ,LOGFV,INTERNALVARIABLE,0,(void *)&gLogFile    );
	knownTokens.install("LOOKUP"               ,"lookup"               ,LOOKC,COMMAND      ,0   ,(void *)Lookup       );
	knownTokens.install("MADELINE"             ,"Madeline"             ,MADLN,KEYWORD      ,0   ,(void *)NULL         );
	knownTokens.install("MAP"                  ,"map"                  ,MAPK ,COMMAND      ,0   ,(void *)MapAssignmentCommand);
	knownTokens.install("MAPCHROMOSOMEFIELD"   ,"MapChromosomeField"   ,MFCHR,INTERNALVARIABLE,0,(void *)&gfnMapChromosome);
	knownTokens.install("MAPDETAILS"           ,"MapDetails"           ,MAPDT,KEYWORD      ,0   ,(void *)NULL         );
	knownTokens.install("MAPDATABASE"          ,"MapDatabase"          ,MAPDB,INTERNALVARIABLE,0,(void *)&gMapDatabase);
	knownTokens.install("MAPFEMALEPOSITIONFIELD","MapFemalePositionField",MFFEM,INTERNALVARIABLE,0,(void *)&gfnMapFemalePosition);
	knownTokens.install("MAPMALEPOSITIONFIELD" ,"MapMalePositionField" ,MFMAL,INTERNALVARIABLE,0,(void *)&gfnMapMalePosition);
	knownTokens.install("MAPORDINALFIELD"      ,"MapOrdinalField"      ,MFORD,INTERNALVARIABLE,0,(void *)&gfnMapOrdinal);
	knownTokens.install("MAPPOSITIONFIELD"     ,"MapPositionField"     ,MFPOS,INTERNALVARIABLE,0,(void *)&gfnMapPosition);
	knownTokens.install("MAPPOSITIONBPFIELD"   ,"MapPositionBPField"   ,MFPBP,INTERNALVARIABLE,0,(void *)&gfnMapPositionBP);
	knownTokens.install("MAPMARKERFIELD"       ,"MapMarkerField"       ,MFMRK,INTERNALVARIABLE,0,(void *)&gfnMapMarker);
	knownTokens.install("MARKER"               ,"Marker"               ,MARKK,KEYWORD      ,0   ,(void *)NULL         );
	knownTokens.install("MARKERS"              ,"Markers"              ,MARKK,KEYWORD      ,0   ,(void *)NULL         );
	knownTokens.install("MARSHFIELD"           ,"Marshfield"           ,MARSH,KEYWORD      ,0   ,(void *)NULL         );
	knownTokens.install("MENDEL"               ,"mendel"               ,MENDL,KEYWORD      ,0   ,(void *)&Mendel      );
	knownTokens.install("MERGE"                ,"merge"                ,MERGC,COMMAND      ,0   ,(void *)Merge        );
	knownTokens.install("MERLIN"               ,"Merlin"               ,MERLK,KEYWORD      ,0   ,(void *)&MerlinQTDT  );
	knownTokens.install("MOTHERIDFIELD"        ,"MotherIDField"        ,MIDFD,INTERNALVARIABLE,0,(void *)&gfnMother   );
	knownTokens.install("MZTWINFIELD"          ,"MZTwinField"          ,MZTFD,INTERNALVARIABLE,0,(void *)&gfnMZTwin   );
	knownTokens.install("NAS"                  ,"NumericAffectionStatus",NAS  ,INTERNALARRAY,0   ,(void *)(PARRAY)&Naff);
	knownTokens.install("NEEDLEINHAYSTACK"     ,"NeedleInHaystack"     ,REM  ,ICCFUNCTION  ,0   ,(void *)NeedleInHaystack);
	knownTokens.install("NDS"                  ,"NumericDeathStatus"   ,NDS  ,INTERNALARRAY,0   ,(void *)(PARRAY)&Nds );
	knownTokens.install("NMV"                  ,"NumericMissingValue"  ,NMV  ,INTERNALARRAY,0   ,(void *)(PARRAY)&Nmv );
	knownTokens.install("NOT"                  ,"not"                  ,'!'  ,OPERATOR     ,0   ,(void *)NULL         );
	knownTokens.install("NSV"                  ,"NumericSexValue"      ,NSV  ,INTERNALARRAY,0   ,(void *)(PARRAY)&Nsv );
	knownTokens.install("NUMERICAFFECTIONSTATUS","NumericAffectionStatus",NAS ,INTERNALARRAY,0   ,(void *)(PARRAY)&Naff);
	knownTokens.install("NUMERICDEATHSTATUS"   ,"NumericDeathStatus"   ,NDS  ,INTERNALARRAY,0   ,(void *)(PARRAY)&Nds );
	knownTokens.install("NUMERICMISSINGVALUE"  ,"NumericMissingValue"  ,NMV  ,INTERNALARRAY,0   ,(void *)(PARRAY)&Nmv );
	knownTokens.install("NUMERICSEXVALUE"      ,"NumericSexValue"      ,NSV  ,INTERNALARRAY,0   ,(void *)(PARRAY)&Nsv );
	knownTokens.install("OF"                   ,"of"                   ,OF__K,KEYWORD      ,0   ,(void *)NULL         );
	knownTokens.install("OFF"                  ,"off"                  ,OFFK ,KEYWORD      ,0   ,(void *)NULL         );
	knownTokens.install("OFFENDDISTANCE"       ,"OffEndDistance"       ,NV_OE,INTERNALVARIABLE,0,(void *)&OffEndDistance);
	knownTokens.install("ON"                   ,"on"                   ,ONK  ,KEYWORD      ,0   ,(void *)NULL          );
	knownTokens.install("OPEN"                 ,"open"                 ,OPENC,COMMAND      ,0   ,(void *)Open          );
	knownTokens.install("OR"                   ,"or"                   ,OR   ,OPERATOR     ,0   ,(void *)NULL          );
	knownTokens.install("ORDER"                ,"order"                ,ORDERK,KEYWORD     ,0   ,(void *)NULL          );
	knownTokens.install("OUTPUT"               ,"output"               ,OUTPK,KEYWORD      ,0   ,(void *)NULL          );
	knownTokens.install("OUTPUTFILE"           ,"OutputFile"           ,OUTFV,INTERNALVARIABLE,0,(void *)&gOutputFile  );
	knownTokens.install("PARAMETEROUTPUTFILE"  ,"ParameterOutputFile"  ,PARFV,INTERNALVARIABLE,0,(void *)&gParameterOutputFile);
	knownTokens.install("PEDIGREE"             ,"pedigree"             ,PEDIK,KEYWORD      ,0   ,(void *)NULL        );
	knownTokens.install("PEDIGREEDRAWINGFILE"  ,"PedigreeDrawingFile"  ,PRNDV,INTERNALVARIABLE,0,(void *)&gPedigreeDrawing);
	knownTokens.install("PEDIGREES"            ,"pedigrees"            ,PEDIK,KEYWORD      ,0   ,(void *)NULL        );
	knownTokens.install("PHENOTYPE"            ,"phenotype"            ,PHENK,KEYWORD      ,0   ,(void *)NULL        );
	knownTokens.install("PLOT"                 ,"plot"                 ,PLOTK,KEYWORD      ,0   ,(void *)NULL        );
	knownTokens.install("PORTUGESE"            ,"Portugese"            ,LPORT,KEYWORD      ,0   ,(void *)NULL         );
	knownTokens.install("PRINTCOMMAND"         ,"PrintCommand"         ,PRNCV,INTERNALVARIABLE,0,(void *)&gPrintCommand );
	knownTokens.install("PROBANDFIELD"         ,"ProbandField"         ,PBDFD,INTERNALVARIABLE,0,(void *)&gfnProband );
	knownTokens.install("QUIET"                ,"quiet"                ,QUIEC,COMMAND      ,0   ,(void *)Quiet       );
	knownTokens.install("QUIT"                 ,"quit (exit, goodbye)" ,QUITC,COMMAND      ,0   ,(void *)Quit        );
	knownTokens.install("READ"                 ,"read"                 ,READK,COMMAND      ,0   ,(void *)Read        );
	knownTokens.install("RECOGNIZE"            ,"recognize"            ,RECGC,COMMAND      ,0   ,(void *)Recognize   );
	knownTokens.install("REGEXPMATCH"          ,"RegExpMatch"          ,REM  ,ICCFUNCTION  ,0   ,(void *)RegExpMatch );
	knownTokens.install("RECORD"               ,"record"               ,RECORK,KEYWORD     ,0   ,(void *)NULL        );
	knownTokens.install("RECTIFY"              ,"rectify"              ,RECTC,COMMAND      ,0   ,(void *)Rectify     );
	knownTokens.install("RELATE"               ,"relate"               ,RELAK,KEYWORD      ,0   ,(void *)&SolarRelate);
	knownTokens.install("RELPAIR"              ,"relpair"              ,RELPRK,KEYWORD     ,0   ,(void *)&Relpair    );
	knownTokens.install("ROUND"                ,"round"                ,ROUND,NFUNCTION    ,0   ,(void *)RoundToWhole);
	knownTokens.install("RUN"                  ,"run"                  ,RUNC ,COMMAND      ,0   ,(void *)Run         );
	knownTokens.install("RUSSIAN"              ,"Russian"              ,LRUSS,KEYWORD      ,0   ,(void *)NULL         );
	knownTokens.install("SAGE"                 ,"sage"                 ,SAGEK,KEYWORD      ,0   ,(void *)&Sage       );
	knownTokens.install("SAVE"                 ,"save"                 ,SAVEK,COMMAND      ,0   ,(void *)Save        );
	knownTokens.install("SAVEALLELEFREQUENCIES","SaveAlleleFrequencies",SVAFK,KEYWORD      ,0   ,(void *)NULL        );
	knownTokens.install("SET"                  ,"set (turn)"           ,TURNC,COMMAND      ,0   ,(void *)Turn        );
	knownTokens.install("SHOW"                 ,"show (?, what is)"    ,SHOWC,COMMAND      ,0   ,(void *)Show        );
	knownTokens.install("?"                    ,"? (show, what is)"    ,SHOWC,COMMAND      ,0   ,(void *)Show        );
	knownTokens.install("SIBIBD"               ,"sibibd"               ,SIBIBD,KEYWORD     ,0   ,(void *)&Aspex      );
	knownTokens.install("SIBLINKAFFECTEDPAIRS" ,"SiblinkAffectedPairs" ,SIBLK,KEYWORD      ,0   ,(void *)&Siblink    );
	knownTokens.install("SIBLINKALLPAIRS"      ,"SiblinkAllPairs"      ,SIBLKA,KEYWORD      ,0   ,(void *)&Siblink   );
	knownTokens.install("SIBLINKDISCORDANTPAIRS","SiblinkDiscordantPairs",SIBLKD,KEYWORD      ,0   ,(void *)&Siblink  );
	knownTokens.install("SIBLINKUNAFFECTEDPAIRS","SiblinkUnaffectedPairs",SIBLKU,KEYWORD      ,0   ,(void *)&Siblink  );
	knownTokens.install("SIBMAP"               ,"sibmap"               ,SIBMAP,KEYWORD     ,0   ,(void *)&Aspex      );
	knownTokens.install("SIBPAL1"              ,"sibpal1"              ,SIBP1,KEYWORD      ,0   ,(void *)&SageSibpal );
	knownTokens.install("SIBPAL2"              ,"sibpal2"              ,SIBP2,KEYWORD      ,0   ,(void *)&SageSibpal );
	knownTokens.install("SIBPAL3"              ,"sibpal3"              ,SIBP3,KEYWORD      ,0   ,(void *)&SageSibpal );
	knownTokens.install("SIBPAL4"              ,"sibpal4"              ,SIBP4,KEYWORD      ,0   ,(void *)&SageSibpal );
	knownTokens.install("SIBPHASE"             ,"sibphase"             ,SIBPHS,KEYWORD     ,0   ,(void *)&Aspex      );
	knownTokens.install("SIBTDT"               ,"sibtdt"               ,SIBTDT,KEYWORD     ,0   ,(void *)&Aspex      );
	knownTokens.install("SILENCE"              ,"silence"              ,SILEC,COMMAND      ,0   ,(void *)Silence     );
	knownTokens.install("SILENT"               ,"silent"               ,SILEC,COMMAND      ,0   ,(void *)Silence     );
	knownTokens.install("SIMWALK"              ,"Simwalk"              ,SIMWK,KEYWORD      ,0   ,(void *)&Simwalk    );
	knownTokens.install("SIN"                  ,"sin"                  ,SIN  ,NFUNCTION    ,0   ,(void *)sin         );
	knownTokens.install("SINH"                 ,"sinh"                 ,SINH ,NFUNCTION    ,0   ,(void *)sinh        );
	knownTokens.install("SKIPPING"             ,"skipping"             ,SKIPK,KEYWORD      ,0   ,(void *)NULL        );
	knownTokens.install("SOLARFISHER0"         ,"solarfisher0"         ,SFIS0,KEYWORD      ,0   ,(void *)&Holder     );
	knownTokens.install("SOLARFISHER1"         ,"solarfisher1"         ,SFIS1,KEYWORD      ,0   ,(void *)&Holder     );
	knownTokens.install("SORT"                 ,"sort"                 ,SORTC,COMMAND      ,0   ,(void *)Sort        );
	knownTokens.install("SPACE"                ,"space"                ,SPCAK,KEYWORD      ,0   ,(void *)NULL        );
	knownTokens.install("SPANISH"              ,"Spanish"              ,LSPAN,KEYWORD      ,0   ,(void *)NULL        );
	knownTokens.install("SQUARE"               ,"square"               ,SQUAR,COMMAND      ,0   ,(void *)Square      );
	knownTokens.install("SQRT"                 ,"sqrt"                 ,SQRT ,NFUNCTION    ,0   ,(void *)sqrt        );
	knownTokens.install("STATISTICS"           ,"statistics"           ,STATK,KEYWORD      ,0   ,(void *)NULL        );
	knownTokens.install("STATS"                ,"stats"                ,STATK,KEYWORD      ,0   ,(void *)NULL        );
	knownTokens.install("SUBSTRING"            ,"SubString"            ,SBS  ,XFUNCTION    ,0   ,(void *)SubString   );
	knownTokens.install("SUOMI"                ,"Suomi (Finnish)"      ,LSUOM,KEYWORD      ,0   ,(void *)NULL        );
	knownTokens.install("SYSTEM"               ,"system"               ,SYSC ,COMMAND      ,0   ,(void *)System      );
	knownTokens.install("TAB"                  ,"tab"                  ,TABAK,KEYWORD      ,0   ,(void *)NULL        );
	knownTokens.install("TAN"                  ,"tan"                  ,TAN  ,NFUNCTION    ,0   ,(void *)tan         );
	knownTokens.install("TANH"                 ,"tanh"                 ,TANH ,NFUNCTION    ,0   ,(void *)tanh        );
	knownTokens.install("THAI"                 ,"Thai"                 ,LTHAI,KEYWORD      ,0   ,(void *)NULL        );
	knownTokens.install("THETATOHALDANE"       ,"ThetaToHaldane"       ,TH2HL,NFUNCTION    ,0   ,(void *)Theta_To_Haldane_cM);
	knownTokens.install("THETATOKOSAMBI"       ,"ThetaToKosambi"       ,TH2KS,NFUNCTION    ,0   ,(void *)Theta_To_Kosambi_cM);
	knownTokens.install("TO"                   ,"to"                   ,TOC  ,KEYWORD      ,0   ,(void *)NULL        );
	knownTokens.install("TODAY"                ,"today"                ,TODAY,TVRDFUNCTION ,0   ,(void *)Today       );
	knownTokens.install("TOGGLE"               ,"toggle"               ,TOGLC,COMMAND      ,0   ,(void *)Toggle      );
	knownTokens.install("TURN"                 ,"turn (set)"           ,TURNC,COMMAND      ,0   ,(void *)Turn        );
	knownTokens.install("UNEXCLUDE"            ,"UnExclude"            ,UNEXC,COMMAND      ,0   ,(void *)Unexclude   );
	knownTokens.install("UNIQUE"               ,"unique"               ,UNIQK,KEYWORD      ,0   ,(void *)NULL        );
	knownTokens.install("USERFQTL"             ,"userfqtl"             ,UFQTK,KEYWORD      ,0   ,(void *)&UserFQTLNuclear);
	knownTokens.install("USERFQTLALL"          ,"userfqtlAll"          ,UFNAK,KEYWORD      ,0   ,(void *)&UserFQTLNuclear);
	knownTokens.install("USERFQTLFOUNDERS"     ,"userfqtlFounders"     ,UFNFK,KEYWORD      ,0   ,(void *)&UserFQTLNuclear);
	knownTokens.install("USERFQTLOFFSPRING"    ,"userfqtlOffspring"    ,UFNOK,KEYWORD      ,0   ,(void *)&UserFQTLNuclear);
	knownTokens.install("USERM13"              ,"userm13"              ,UM13 ,KEYWORD      ,0   ,(void *)&UserM13    );
	knownTokens.install("USING"                ,"using"                ,USINGK,KEYWORD     ,0   ,(void *)NULL        );
	knownTokens.install("VALUES"               ,"values"               ,VALUK,KEYWORD      ,0   ,(void *)NULL        );
	knownTokens.install("VERBOSE"              ,"verbose"              ,VERBC,COMMAND      ,0   ,(void *)Verbose     );
	knownTokens.install("VERTICAL"             ,"vertical"             ,VERTK,KEYWORD      ,0   ,(void *)NULL        );
	knownTokens.install("VIEW"                 ,"view"                 ,VIEWC,COMMAND      ,0   ,(void *)View        );
	knownTokens.install("VU"                   ,"vu (VerticalUnits)"   ,VU__K,KEYWORD      ,0   ,(void *)NULL        );
	knownTokens.install("VERTICALUNIT"         ,"VerticalUnit (vu)"    ,VU__K,KEYWORD      ,0   ,(void *)NULL        );
	knownTokens.install("VERTICALUNITS"        ,"VerticalUnits (vu)"   ,VU__K,KEYWORD      ,0   ,(void *)NULL        );
	knownTokens.install("WEBER"                ,"Weber"                ,WEBRK,KEYWORD      ,0   ,(void *)NULL        );
	knownTokens.install("WHAT"                 ,"what"                 ,WHATC,COMMAND      ,0   ,(void *)WhatIs      );
	knownTokens.install("WITH"                 ,"with"                 ,WITHK,KEYWORD      ,0   ,(void *)NULL        );
	knownTokens.install("WRITE"                ,"write"                ,WRITC,COMMAND      ,0   ,(void *)Write       );
	knownTokens.install("XOR"                  ,"xor"                  ,XOR  ,OPERATOR     ,0   ,(void *)NULL        );
	/*                    */
	/* INTERNAL CONSTANTS */
	/*                    */
	knownTokens.install("#AFFECTED"            ,"#affected"   ,NV_AF,NUMERICVALUE ,(double)AFFECTED  ,(void *)NULL);
	knownTokens.install("#ALIVE"               ,"#alive"      ,NV_AL,NUMERICVALUE ,(double)ALIVE     ,(void *)NULL);
	knownTokens.install("#DEAD"                ,"#dead"       ,NV_DE,NUMERICVALUE ,(double)DEAD      ,(void *)NULL);
	knownTokens.install("#E"                   ,"#e"          ,EBASE,NUMERICVALUE ,        M_E       ,(void *)NULL);
	knownTokens.install("#FALSE"               ,"#false"      ,NV_FA,NUMERICVALUE ,(double)FALSE     ,(void *)NULL);
	knownTokens.install("#FEMALE"              ,"#female"     ,FEM  ,NUMERICVALUE ,(double)FEMALE    ,(void *)NULL);
	knownTokens.install("#MALE"                ,"#male"       ,MAL  ,NUMERICVALUE ,(double)MALE      ,(void *)NULL);
	knownTokens.install("#MISSING"             ,"#missing"    ,MI   ,NUMERICVALUE ,DOUBLEMISSINGVALUE,(void *)NULL);
	knownTokens.install("#PI"                  ,"#pi"         ,PIV  ,NUMERICVALUE ,        M_PI      ,(void *)NULL);
	knownTokens.install("#TRUE"                ,"#true"       ,NV_TR,NUMERICVALUE ,(double)TRUE      ,(void *)NULL);
	knownTokens.install("#UNAFFECTED"          ,"#unaffected" ,NV_UN,NUMERICVALUE ,(double)UNAFFECTED,(void *)NULL);
	/*                          */
	/* INDIVIDUALVARIABLES and  */
	/* INDIVIDUALPOINTERS       */
	/*                          */
	/* -> Dramatically revamped */
	/* on 2002.03.22 &          */
	/* 2002.04.29 -ET           */
	/*                          */
	knownTokens.install("_CHILDLESSSPOUSE"     ,"_ChildlessSpouse"     ,INCSP,INDIVIDUALVARIABLE,0,(void *)NULL);
	knownTokens.install("_COMPLEXITY"          ,"_Complexity"          ,INCPX,INDIVIDUALVARIABLE,0,(void *)NULL);
	knownTokens.install("_MARKEDFOREXCLUSION"  ,"_MarkedForExclusion"  ,INEXC,INDIVIDUALVARIABLE,0,(void *)NULL);
	knownTokens.install("_FAMILYID"            ,"_FamilyID"            ,IFID ,INDIVIDUALSTRING  ,0,(void *)NULL);
	knownTokens.install("_FATHER"              ,"_Father"              ,IDAD ,INDIVIDUALPOINTER ,0,(void *)NULL);
	knownTokens.install("_HADDATA"             ,"_HadData"             ,INHDT,INDIVIDUALVARIABLE,0,(void *)NULL);
	knownTokens.install("_ID"                  ,"_ID"                  ,IID  ,INDIVIDUALSTRING  ,0,(void *)NULL);
	knownTokens.install("_INDIVIDUALID"        ,"_IndividualID"        ,IID  ,INDIVIDUALSTRING  ,0,(void *)NULL);
	knownTokens.install("_WASINCLUDED"         ,"_WasIncluded"         ,ININC,INDIVIDUALVARIABLE,0,(void *)NULL);
	knownTokens.install("_ISAFFECTED"          ,"_IsAffected"          ,INAFF,INDIVIDUALVARIABLE,0,(void *)NULL);
	knownTokens.install("_ISCONSANGUINOUS"     ,"_IsConsanguinous"     ,INCSG,INDIVIDUALVARIABLE,0,(void *)NULL);
	knownTokens.install("_ISDECEASED"          ,"_IsDeceased"          ,INDED,INDIVIDUALVARIABLE,0,(void *)NULL);
	knownTokens.install("_ISDIZYGOTICTWIN"     ,"_IsDizygoticTwin"     ,INDZT,INDIVIDUALVARIABLE,0,(void *)NULL);
	knownTokens.install("_ISFEMALE"            ,"_IsFemale"            ,INFEM,INDIVIDUALVARIABLE,0,(void *)NULL);
	knownTokens.install("_ISFOUNDER"            ,"_IsFounder"          ,INFND,INDIVIDUALVARIABLE,0,(void *)NULL);
	knownTokens.install("_ISINDATABASE"        ,"_IsInDatabase"        ,INDBF,INDIVIDUALVARIABLE,0,(void *)NULL);
	
	knownTokens.install("_ISMENDELIANINCONSISTENT","_IsMendelianInconsistent",INMDL,INDIVIDUALVARIABLE,0,(void *)NULL);
	knownTokens.install("_MENDELIANINCONSISTENCYCOUNT","_MendelianInconsistencyCount",INMDC,INDIVIDUALVARIABLE,0,(void *)NULL);
	knownTokens.install("_PERCENTMENDELIANINCONSISTENT","_PercentMendelianInconsistent",INMDP,INDIVIDUALVARIABLE,0,(void *)NULL);
	
	knownTokens.install("_ISMONOZYGOTICTWIN"   ,"_IsMonozygoticTwin"   ,INMZT,INDIVIDUALVARIABLE,0,(void *)NULL);
	knownTokens.install("_ISPRIMARYFOUNDER"    ,"_IsPrimaryFounder"    ,INPFD,INDIVIDUALVARIABLE,0,(void *)NULL);
	knownTokens.install("_ISPROBAND"           ,"_IsProband"           ,INISP,INDIVIDUALVARIABLE,0,(void *)NULL);
	knownTokens.install("_ISUNATTACHED"        ,"_IsUnattached"        ,INFLT,INDIVIDUALVARIABLE,0,(void *)NULL);
	knownTokens.install("_MATE"                ,"_mate"                ,IA   ,INDIVIDUALARRAY   ,0,(void *)NULL);
	knownTokens.install("_MOTHER"              ,"_mother"              ,IMOM ,INDIVIDUALPOINTER ,0,(void *)NULL);
	knownTokens.install("_N"                   ,"_n"                   ,INIP ,INDIVIDUALVARIABLE,0,(void *)NULL);
	knownTokens.install("_NUMBERINPEDIGREE"    ,"_NumberInPedigree"    ,INIP ,INDIVIDUALVARIABLE,0,(void *)NULL);
	knownTokens.install("_NUMBERAFFECTED"      ,"_NumberAffected"      ,INNOA,INDIVIDUALVARIABLE,0,(void *)NULL);
	knownTokens.install("_NUMBEROFAFFECTEDPAIRS","_NumberOfAffectedPairs",INNOAP,INDIVIDUALVARIABLE,0,(void *)NULL);
	knownTokens.install("_NUMBEROFFOUNDERS"    ,"_NumberOfFounders"    ,INNOF,INDIVIDUALVARIABLE,0,(void *)NULL);
	knownTokens.install("_NUMBEROFNONFOUNDERS" ,"_NumberOfNonFounders" ,INNON,INDIVIDUALVARIABLE,0,(void *)NULL);
	knownTokens.install("_NFF"                 ,"_nff"                 ,INFF ,INDIVIDUALVARIABLE,0,(void *)NULL);
	knownTokens.install("_NFM"                 ,"_nfm"                 ,INFM ,INDIVIDUALVARIABLE,0,(void *)NULL);
	knownTokens.install("_NMATES"              ,"_nMates"              ,INA  ,INDIVIDUALVARIABLE,0,(void *)NULL);
	knownTokens.install("_NUMBEROFMATES"       ,"_NumberOfMates"       ,INA  ,INDIVIDUALVARIABLE,0,(void *)NULL);
	knownTokens.install("_NOFFSPRING"          ,"_nOffspring"          ,INK  ,INDIVIDUALVARIABLE,0,(void *)NULL);
	knownTokens.install("_NUMBEROFOFFSPRING"   ,"_NumberOfOffspring"   ,INK  ,INDIVIDUALVARIABLE,0,(void *)NULL);
	knownTokens.install("_OFFSPRING"           ,"_offspring"           ,IK   ,INDIVIDUALARRAY   ,0,(void *)NULL);
	knownTokens.install("_SPOUSE"              ,"_spouse"              ,IPSPS,INDIVIDUALPOINTER ,0,(void *)NULL);
	knownTokens.install("_FIRSTCHILD"          ,"_FirstChild"          ,IPK01,INDIVIDUALPOINTER ,0,(void *)NULL);
	knownTokens.install("_SECONDCHILD"         ,"_SecondChild"         ,IPK02,INDIVIDUALPOINTER ,0,(void *)NULL);
	knownTokens.install("_THIRDCHILD"          ,"_ThirdChild"          ,IPK03,INDIVIDUALPOINTER ,0,(void *)NULL);
	knownTokens.install("_FOURTHCHILD"         ,"_FourthChild"         ,IPK04,INDIVIDUALPOINTER ,0,(void *)NULL);
	knownTokens.install("_FIFTHCHILD"          ,"_FifthChild"          ,IPK05,INDIVIDUALPOINTER ,0,(void *)NULL);
	knownTokens.install("_SELF"                ,"_self"                ,ISELF,INDIVIDUALPOINTER ,0,(void *)NULL);
	knownTokens.install("_SIXTHCHILD"          ,"_SixthChild"          ,IPK06,INDIVIDUALPOINTER ,0,(void *)NULL);
	knownTokens.install("_SEVENTHCHILD"        ,"_SeventhChild"        ,IPK07,INDIVIDUALPOINTER ,0,(void *)NULL);
	knownTokens.install("_EIGHTHCHILD"         ,"_EighthChild"         ,IPK08,INDIVIDUALPOINTER ,0,(void *)NULL);
	knownTokens.install("_NINTHCHILD"          ,"_NinthChild"          ,IPK09,INDIVIDUALPOINTER ,0,(void *)NULL);
	knownTokens.install("_TENTHCHILD"          ,"_TenthChild"          ,IPK10,INDIVIDUALPOINTER ,0,(void *)NULL);
	knownTokens.install("_GENOTYPECOUNT"       ,"_GenotypeCount"       ,INGTC,INDIVIDUALVARIABLE,0,(void *)NULL);
	knownTokens.install("_PERCENTGENOTYPED"    ,"_PercentGenotyped"    ,INGTP,INDIVIDUALVARIABLE,0,(void *)NULL);
	knownTokens.install("_MATERNALGRANDMOTHER" ,"_MaternalGrandmother" ,IPMGM,INDIVIDUALPOINTER ,0,(void *)NULL);
	knownTokens.install("_MATERNALGRANDFATHER" ,"_MaternalGrandfather" ,IPMGF,INDIVIDUALPOINTER ,0,(void *)NULL);
	knownTokens.install("_PATERNALGRANDMOTHER" ,"_PaternalGrandmother" ,IPPGM,INDIVIDUALPOINTER ,0,(void *)NULL);
	knownTokens.install("_PATERNALGRANDFATHER" ,"_PaternalGrandfather" ,IPPGF,INDIVIDUALPOINTER ,0,(void *)NULL);
	knownTokens.install("_RECORD"              ,"_record"              ,IRCI ,INDIVIDUALVARIABLE,0,(void *)NULL);
	/*                      */
	/*  AGGREGATE FUNCTIONS */
	/*                      */
	knownTokens.install("_OFFSPRINGCOUNT"            ,"_OffspringCount"            ,ACOUNT,AFUNCTION   ,0   ,(void *)NULL);
	knownTokens.install("_OFFSPRINGCOUNTFALSE"       ,"_OffspringCountFalse"       ,ACFALS,AFUNCTION   ,0   ,(void *)NULL);
	knownTokens.install("_OFFSPRINGCOUNTMISSING"     ,"_OffspringCountMissing"     ,ACMISS,AFUNCTION   ,0   ,(void *)NULL);
	knownTokens.install("_OFFSPRINGCOUNTTRUE"        ,"_OffspringCountTrue"        ,ACTRUE,AFUNCTION   ,0   ,(void *)NULL);
	knownTokens.install("_OFFSPRINGMEAN"             ,"_OffspringMean"             ,AMEAN ,AFUNCTION   ,0   ,(void *)NULL);
	knownTokens.install("_OFFSPRINGSTDDEV"           ,"_OffspringStdDev"           ,ASTDV ,AFUNCTION   ,0   ,(void *)NULL);
	knownTokens.install("_OFFSPRINGSTANDARDDEVIATION","_OffspringStandardDeviation",ASTDV ,AFUNCTION   ,0   ,(void *)NULL);
	knownTokens.install("_OFFSPRINGSUM"              ,"_OffspringSum"              ,ASUM  ,AFUNCTION   ,0   ,(void *)NULL);
	knownTokens.install("_OFFSPRINGVARIANCE"         ,"_OffspringVariance"         ,AVAR  ,AFUNCTION   ,0   ,(void *)NULL);
	knownTokens.install("_OFFSPRINGMINIMUM"          ,"_OffspringMinimum"          ,AMIN  ,AFUNCTION   ,0   ,(void *)NULL);
	knownTokens.install("_OFFSPRINGMAXIMUM"          ,"_OffspringMaximum"          ,AMAX  ,AFUNCTION   ,0   ,(void *)NULL);
	knownTokens.install("_OFFSPRINGCOUNTPAIRSTRUE"   ,"_OffspringCountPairsTrue"   ,ACPAIR,AFUNCTION   ,0   ,(void *)NULL);
	/*                             */
	/* 19990416 ET ADDENDUM:       */
	/* DEBUG interactive command   */
	/* wrapper for a debug routine */
	/*                             */
	knownTokens.install("DEBUG"                ,"DEBUG",DBUGC,COMMAND      ,0   ,(void *)Debug       );
	/*                             */
	/* 19990428 ET Addenda for     */
	/* enhanced pedigree graphics: */
	/*                             */
	knownTokens.install("ORIENTATION"            ,"orientation"          ,ORIEK,KEYWORD      ,0   ,(void *)NULL          );
	knownTokens.install("LANDSCAPE"              ,"landscape"            ,LANDK,KEYWORD      ,0   ,(void *)NULL          );
	knownTokens.install("PORTRAIT"               ,"portrait"             ,PORTK,KEYWORD      ,0   ,(void *)NULL          );
	knownTokens.install("AUTOMATIC"              ,"automatic"            ,AUTPK,KEYWORD      ,0   ,(void *)NULL          );
	knownTokens.install("MULTIPAGE"              ,"MultiPage"            ,MLTPK,KEYWORD      ,0   ,(void *)NULL          );
	knownTokens.install("COLOR"                  ,"color (colour)"       ,COLRK,KEYWORD      ,0   ,(void *)NULL          );
	knownTokens.install("COLOUR"                 ,"colour (color)"       ,COLRK,KEYWORD      ,0   ,(void *)NULL          );
	knownTokens.install("REVERSESHADING"         ,"ReverseShading"       ,RSHDK,KEYWORD      ,0   ,(void *)NULL          );
	knownTokens.install("POSTSCRIPTVIEWER"       ,"PostscriptViewer"     ,PSVNM,INTERNALVARIABLE,0,(void *)&gPSViewerName);
	knownTokens.install("DIVIDEDDRAWINGS"        ,"DividedDrawings"      ,DIVDK,KEYWORD      ,0   ,(void *)NULL          );
	knownTokens.install("PEDCHECK"               ,"pedcheck"               ,PEDCK,KEYWORD      ,0   ,(void *)&PedCheck   );
	knownTokens.install("LDEQMARKER"             ,"LDEQMarker"             ,LDEQK,KEYWORD      ,0   ,(void *)&LDEQHMethod);
	knownTokens.install("LDEQAFFECTEDSPOUSE"     ,"LDEQAffectedSpouse"     ,LDE2K,KEYWORD      ,0   ,(void *)&LDEQHMethod);
	knownTokens.install("LDEQTDT"                ,"LDEQTDT"                ,LDE3K,KEYWORD      ,0   ,(void *)&LDEQHMethod);
	knownTokens.install("PAPERSIZE"              ,"PaperSize"              ,SZPAK,KEYWORD      ,0   ,(void *)NULL        );
	knownTokens.install("USLETTER"               ,"USLetter"               ,SZLTK,KEYWORD      ,0   ,(void *)NULL        );
	knownTokens.install("USLEGAL"                ,"USLegal"                ,SZLGK,KEYWORD      ,0   ,(void *)NULL        );
	knownTokens.install("A4"                     ,"A4"                     ,SZA4K,KEYWORD      ,0   ,(void *)NULL        );
	knownTokens.install("A4LONG"                 ,"A4Long"                 ,SZALK,KEYWORD      ,0   ,(void *)NULL        );
	knownTokens.install("A4SUPER"                ,"A4Super"                ,SZASK,KEYWORD      ,0   ,(void *)NULL        );
	knownTokens.install("PAPERMARGIN"            ,"PaperMargin"            ,PMRGK,KEYWORD      ,0   ,(void *)NULL        );
	knownTokens.install("WEBBROWSER"             ,"WebBrowser"             ,WEBVK,INTERNALVARIABLE ,0   ,(void *)&gWebViewerName);
	knownTokens.install("WEBADDRESS"             ,"WebAddress"             ,WEBAK,INTERNALVARIABLE ,0   ,(void *)&gWebAddress   );
	knownTokens.install("TEXT"                   ,"Text"                   ,TEXTK,KEYWORD          ,0   ,(void *)NULL           );
	knownTokens.install("HTML"                   ,"html"                   ,HTMLK,KEYWORD          ,0   ,(void *)NULL           );
	knownTokens.install("HAPLOTYPEDISPLAY"       ,"HaplotypeDisplay"       ,HAPLK,KEYWORD          ,0   ,(void *)NULL          );
	knownTokens.install("ALPHA"                  ,"alpha"                  ,ALPHAK,KEYWORD         ,0   ,(void *)NULL          );
	knownTokens.install("PHYSICAL"               ,"physical"               ,PHYSIK,KEYWORD         ,0   ,(void *)NULL          );
	knownTokens.install("FUSIONSUPPORT"          ,"FusionSupport"          ,FUSNK,KEYWORD          ,0   ,(void *)NULL          );
	knownTokens.install("A"                      ,"a"                      ,SVR_A,INTERNALVARIABLE ,0   ,(void *)&ScratchVar_A );
	knownTokens.install("B"                      ,"b"                      ,SVR_B,INTERNALVARIABLE ,0   ,(void *)&ScratchVar_B );
	knownTokens.install("C"                      ,"c"                      ,SVR_C,INTERNALVARIABLE ,0   ,(void *)&ScratchVar_C );
	knownTokens.install("D"                      ,"d"                      ,SVR_D,INTERNALVARIABLE ,0   ,(void *)&ScratchVar_D );
	knownTokens.install("E"                      ,"e"                      ,SVR_D,INTERNALVARIABLE ,0   ,(void *)&ScratchVar_E );
	knownTokens.install("F"                      ,"f"                      ,SVR_F,INTERNALVARIABLE ,0   ,(void *)&ScratchVar_F );
	knownTokens.install("G"                      ,"g"                      ,SVR_G,INTERNALVARIABLE ,0   ,(void *)&ScratchVar_G );
	knownTokens.install("H"                      ,"h"                      ,SVR_H,INTERNALVARIABLE ,0   ,(void *)&ScratchVar_H );
	knownTokens.install("I"                      ,"i"                      ,SVR_I,INTERNALVARIABLE ,0   ,(void *)&ScratchVar_I );
	knownTokens.install("J"                      ,"j"                      ,SVR_J,INTERNALVARIABLE ,0   ,(void *)&ScratchVar_J );
	knownTokens.install("K"                      ,"k"                      ,SVR_K,INTERNALVARIABLE ,0   ,(void *)&ScratchVar_K );
	knownTokens.install("L"                      ,"l"                      ,SVR_L,INTERNALVARIABLE ,0   ,(void *)&ScratchVar_L );
	knownTokens.install("M"                      ,"m"                      ,SVR_M,INTERNALVARIABLE ,0   ,(void *)&ScratchVar_M );
	knownTokens.install("N"                      ,"n"                      ,SVR_N,INTERNALVARIABLE ,0   ,(void *)&ScratchVar_N );
	knownTokens.install("O"                      ,"o"                      ,SVR_O,INTERNALVARIABLE ,0   ,(void *)&ScratchVar_O );
	knownTokens.install("P"                      ,"p"                      ,SVR_P,INTERNALVARIABLE ,0   ,(void *)&ScratchVar_P );
	knownTokens.install("Q"                      ,"q"                      ,SVR_Q,INTERNALVARIABLE ,0   ,(void *)&ScratchVar_Q );
	knownTokens.install("R"                      ,"r"                      ,SVR_R,INTERNALVARIABLE ,0   ,(void *)&ScratchVar_R );
	knownTokens.install("S"                      ,"s"                      ,SVR_S,INTERNALVARIABLE ,0   ,(void *)&ScratchVar_S );
	knownTokens.install("T"                      ,"t"                      ,SVR_T,INTERNALVARIABLE ,0   ,(void *)&ScratchVar_T );
	knownTokens.install("U"                      ,"u"                      ,SVR_U,INTERNALVARIABLE ,0   ,(void *)&ScratchVar_U );
	knownTokens.install("V"                      ,"v"                      ,SVR_V,INTERNALVARIABLE ,0   ,(void *)&ScratchVar_V );
	knownTokens.install("W"                      ,"w"                      ,SVR_W,INTERNALVARIABLE ,0   ,(void *)&ScratchVar_W );
	knownTokens.install("X"                      ,"x"                      ,SVR_X,INTERNALVARIABLE ,0   ,(void *)&ScratchVar_X );
	knownTokens.install("Y"                      ,"y"                      ,SVR_Y,INTERNALVARIABLE ,0   ,(void *)&ScratchVar_Y );
	knownTokens.install("Z"                      ,"z"                      ,SVR_Z,INTERNALVARIABLE ,0   ,(void *)&ScratchVar_Z );
	knownTokens.install("PROMPT"                 ,"prompt"                 ,PROMP,COMMAND          ,0   ,(void *)PromptUser    );
	knownTokens.install("LABELCREATEDINDIVIDUALS","LabelCreatedIndividuals",LBLVI,KEYWORD      ,0   ,(void *)NULL        );
	knownTokens.install("HIGHLIGHTROWS"          ,"HighlightRows"          ,HIGHL,KEYWORD      ,0   ,(void *)NULL        );
	
	knownTokens.install("AFFECTIONSTATUS"        ,"AffectionStatus"        ,AAAS ,ASSOCIATIVEARRAY,0   ,(void *)&aaas          );
	knownTokens.install("DEATHSTATUS"            ,"DeathStatus"            ,AADS ,ASSOCIATIVEARRAY,0   ,(void *)&aads          );
	knownTokens.install("GENDERSTATUS"           ,"GenderStatus"           ,AAGS ,ASSOCIATIVEARRAY,0   ,(void *)&aags          );
	knownTokens.install("PROBANDSTATUS"          ,"ProbandStatus"          ,AAPS ,ASSOCIATIVEARRAY,0   ,(void *)&aaps          );
	knownTokens.install("LIABILITYCLASS"         ,"LiabilityClass"         ,AALC ,ASSOCIATIVEARRAY,0   ,(void *)&aalc          );
	knownTokens.install("GRAPHANNOTATIONS"       ,"GraphAnnotations"       ,AAGA ,ASSOCIATIVEARRAY,0   ,(void *)&aaga          );
	
	knownTokens.install("BLACK"                  ,"black"                  ,BLACK,INTERNALVARIABLE ,0  ,(void *)&gColorBlack      );
	knownTokens.install("BLUE"                   ,"blue"                   ,BLUE ,INTERNALVARIABLE ,0  ,(void *)&gColorBlue       );
	knownTokens.install("BROWN"                  ,"brown"                  ,BROWN,INTERNALVARIABLE ,0  ,(void *)&gColorBrown      );
	knownTokens.install("CYAN"                   ,"cyan"                   ,CYAN ,INTERNALVARIABLE ,0  ,(void *)&gColorCyan       );
	knownTokens.install("DARKGRAY"               ,"DarkGray"               ,DKGRY,INTERNALVARIABLE ,0  ,(void *)&gColorDarkGray   );
	knownTokens.install("FORESTGREEN"            ,"ForestGreen"            ,FRGRN,INTERNALVARIABLE ,0  ,(void *)&gColorForestGreen);
	knownTokens.install("GRAY"                   ,"gray"                   ,GRAY ,INTERNALVARIABLE ,0  ,(void *)&gColorGray       );
	knownTokens.install("GREEN"                  ,"green"                  ,GREEN,INTERNALVARIABLE ,0  ,(void *)&gColorGreen      );
	knownTokens.install("LIGHTGRAY"              ,"LightGray"              ,LTGRY,INTERNALVARIABLE ,0  ,(void *)&gColorLightGray  );
	knownTokens.install("MAGENTA"                ,"magenta"                ,MAGEN,INTERNALVARIABLE ,0  ,(void *)&gColorMagenta    );
	knownTokens.install("ORANGE"                 ,"orange"                 ,ORANG,INTERNALVARIABLE ,0  ,(void *)&gColorOrange     );
	knownTokens.install("PEACH"                  ,"peach"                  ,PEACH,INTERNALVARIABLE ,0  ,(void *)&gColorPeach      );
	knownTokens.install("PURPLE"                 ,"purple"                 ,PURPL,INTERNALVARIABLE ,0  ,(void *)&gColorPurple     );
	knownTokens.install("RED"                    ,"red"                    ,RED  ,INTERNALVARIABLE ,0  ,(void *)&gColorRed        );
	knownTokens.install("WHITE"                  ,"white"                  ,WHITE,INTERNALVARIABLE ,0  ,(void *)&gColorWhite      );
	knownTokens.install("YELLOW"                 ,"yellow"                 ,YELLW,INTERNALVARIABLE ,0  ,(void *)&gColorYellow     );
	
	/*                                   */
	/* Initialize the control structure: */
	/*                                   */
	InitializeControl(g);
	
	/*                                           */
	/* initialize internal variables and arrays: */
	/*                                           */
	InitializeInternalVariables();
	
	/*                                        */
	/* Initialize associative array mappings: */
	/*                                        */
	InitializeAssociativeArrayMappings();
	
	/*                               */
	/* See if running in batch mode: */
	/*	                              */
	if(argc==2){
		source=fopen(argv[1],"rt");
		if(!source){
			ErrorReport("main(): Unable to open batch file, \"%s\"",argv[1]);
		}
	}else{
		source=stdin;
	}
	
	/*                                             */
	/* Open Log, Detail, Error, and Command files: */
	/*	                                       */
	g->streams.log=fopen(gLogFile.e.s,"wt");
	if(!g->streams.log)
		ErrorReport("main(): Unable to open log file, \"%s\"",gLogFile.e.s);
	
	g->streams.detail=fopen(gDetailFile.e.s,"wt");
	if(!g->streams.detail)
		ErrorReport("main(): Unable to open detail file, \"%s\"",gDetailFile.e.s);
		
	g->streams.err=fopen(gErrorFile.e.s,"wt");
	if(!g->streams.err)
		ErrorReport("main(): Unable to open error log file, \"%s\"",gErrorFile.e.s);
		
	g->streams.command=fopen(gCommandFile.e.s,"wt");
	if(!g->streams.command){
		ErrorReport("main(): Unable to open command log file, \"%s\"",gCommandFile.e.s);
	}else{
		fprintf(g->streams.command,"//\n// Madeline v.%6.3f\n// Command Log\n// %s\n//\n",
			VERSION,
			DelimitedGenericDateString(JulianToday(),'.')
		);
	}
	/*                          */
	/* Program start-up banner: */
	/*                          */
	Banner();
	
	/*                                              */
	/* Tell user if text help is loaded:            */
	/* (Note that HTML help is the default anyway): */
	/*                                              */
	if(HelpText) lprintf("\nHelp facility loaded.\n\n");
	
	/*                                                           */
	/* Check for initialization script file:                     */
	/*                                                           */
	/* * If script is present in the local directory,            */
	/*   use it.                                                 */
	/*                                                           */
	/* * Else if it is present in the Madeline system directory, */
	/*   use that instead:                                       */
	/*                                                           */
	if(File(INITIALIZATION_SCRIPT_NAME)){
		/*                          */
		/* Run local script: */
		/*                          */
		BatchCommandLine(INITIALIZATION_SCRIPT_NAME);
	}else{
		/*                                                   */
		/* See if system initialization script is available: */
		/*                                                   */
		if(g->MadelineSystem){
			strcpy(hold,g->MadelineSystem);
			strcat(hold,DIRECTORYSEPARATOR);
			strcat(hold,INITIALIZATION_SCRIPT_NAME);
			if(File(hold)){
				BatchCommandLine(hold);
			}
		}
	}
	
	/*                                                                                  */
	/* Hello (status information) is only shown in verbose mode (which is the default): */
	/*                                                                                  */
	if(g->Verbose) Hello();
	
	
	CommandLine(source); /* command-line: interactive or batch mode */
	
	return 0;
	
}


/*                                                                        */
/*                                                                        */
/* GetToken()                                                             */
/*                                                                        */
/* GetToken() is part of the expression parser.                           */
/*                                                                        */
/* Gets the next token from the global "e" expression buffer              */
/* and places token information into the global Token structure by way of */
/* the global pointer "pT"                                                */
/*                                                                        */
/* Return value: The token's type is returned.                            */
/*                                                                        */
/* When the external RepeatInProgress flag is true, GetToken() just pulls */
/* the pre-parsed token off the token stack.                              */
/*                                                                        */

int GetToken(void){

	extern PTOKENINFO pT;
	extern int RepeatInProgress;

	extern PCONTROL g;
	
	char *ee;
	char c;
	char *t; // temp pointer
	
	/*                                                      */
	/* Push the last token (if present) by incrementing the */
	/* stack pointer prior to getting a token:              */
	/*                                                      */
	pT++;

	/*                                                          */
	/* When repeating, the token has already been evaluated and */
	/* is already available in internal form at current address */
	/* of pT, so just return:                                   */
	/*                                                          */
	if(RepeatInProgress) return pT->t;
	
	/*                                                  */
	/* If not repeating, we need to evaluate the tokens */
	/* by picking them off the expression buffer e:     */
	/*                                                  */
	
	ee=pT->e;

	/*                                                  */
	/* Check against NULL pointer: should never happen: */
	/*                                                  */
	if(e==NULL) MadelineErrorReport("Pointer to expression is NULL");

	/*                   */
	/* Skip white space: */
	/*                   */
	for(;*e && (*e==' ' || *e=='\t' || *e=='\n' || *e=='\r');e++);
	
	/*                           */
	/* Start classifying tokens: */
	/*                           */
	switch(*e){              
	
	case '\0':
	case ';' :
		/*                    */
		/* END of expression: */
		/*                    */
		*ee  =*e;
		pT->i=END;
		return pT->t=OPERATOR;

	case '(':
	case ')':
	case ',':		
		/*                                                 */
		/* PARENTHESES                                     */
		/* (OR COMMA USED TO SEPARATE FUNCTION PARAMETERS) */
		/*                                                 */
		pT->i=*ee++=*e++;
		*ee  ='\0';
		return pT->t=OPERATOR;

	case '[':
	case ']':
		/*                                        */
		/* BRACKETS for internal array variables: */
		/*                                        */
		pT->i=*ee++=*e++;
		*ee='\0';
		return pT->t=OPERATOR;          

	case '=':
	case '!':
	case '<':
	case '>':
		/*                       */
		/* comparison OPERATORs: */
		/*                       */
		*ee++=*e++;
		/*                                            */
		/* possibility of ==,!=,<=, and >= operators: */
		/*                                            */
		if(*e=='='){
			*ee++=*e++;
			
			switch(pT->e[0]){
			case '!':
				pT->i=NE;
				break;
			
			case '<':
				pT->i=LE;
				break;
			
			case '>':
				pT->i=GE;
				break;
				
			case '=':
				pT->i='=';
				break;
			}
		}else{
			pT->i=pT->e[0]; 
		}
		*ee='\0';
		return pT->t=OPERATOR;

	case '+':
	case '-':
	case '/':
	case '*':
	case '^':
		/*                         */
		/* mathematical OPERATORs: */
		/*                         */
		*ee++=*e++;

		/* 2000.09.23 ET ADDENDA:                     */
		/* possibility of ++,--,+=,-=      operators: */
		/*                                            */
		if(*e=='='){
			*ee++=*e++;
			
			switch(pT->e[0]){
			case '+':
				/* += assignment operator */
				pT->i=PLUS_ASSIGN;
				break;
			case '-':
				/* -= assignment operator */
				pT->i=MINUS_ASSIGN;
				break;
			case '*':
				/* *= assignment operator */
				pT->i=MULTIPLY_ASSIGN;
				break;
			case '/':
				/* /= assignment operator */
				pT->i=DIVIDE_ASSIGN;
				break;
				
			}
		}else if(*e==pT->e[0] && (*e=='+' || *e=='-')){
			
			*ee++=*e++;
			
			switch(pT->e[0]){
			case '+':
				/* increment operator */
				pT->i=INCREMENT_ASSIGN;
				break;
				
			case '-':
				/* decrement operator */
				pT->i=DECREMENT_ASSIGN;
				break;
			}
		}else{
			pT->i=pT->e[0]; 
		}
		*ee='\0';
		return pT->t=OPERATOR;

	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
		/*                        */
		/* literal numeric value: */
		/*                        */
		for(;isdigit(*e) || *e=='.';*ee++=*e++);
		*ee='\0';
		pT->i=NOINTERNALTOKEN;
		pT->v=atof(pT->e);  /* convert literal value to double */
		return pT->t=NUMERICVALUE;

	case '\"':
	case '\'':
		/*                                                           */
		/* literal string value:                                     */
		/*                                                           */
		c=*e++;    /* skip the starting delimiting quote             */
		/* gather string but only up to the end of the current line: */
		for(;*e && !(*e==c || *e==0x0D || *e==0x0A);*ee++=*e++);
		if(*e!=c) MadelineErrorReport("GetToken(): quoted string literal is missing terminating delimiter");
		e++;   /* skip over the ending delimiting quote */
		*ee='\0';
		pT->i=NOINTERNALTOKEN;
		return pT->t=STRINGVALUE;
	case '{':
		/* This can either be the beginning of a date string, in which case */
		/* the terminating "}" must appear on the SAME LINE, or it can be   */
		/* the beginning of a PROGRAM CONTROL BLOCK, in which case "{" must */
		/* be the last non-white space character on the line:               */
		
		/* First, lets deal with the easy PROGRAM CONTROL BLOCK case: */
		for(t=e+1;*t && (*t==' ' || *t=='\t' || *t=='\n' || *t=='\r');t++);
		if(!*t){
			pT->i=*ee++=*e++;
			*ee='\0';
			pT->v=0;
			return pT->t=OPERATOR;
		}
		/*                                                                */
		/* literal date value:                                            */
		/*                                                                */
		c='}';   /* set terminating delimiter                             */
		e++;    /* skip the starting delimiter                            */
		/*                                                                */
		/* gather date string but only up to the end of the current line: */
		/*                                                                */
		for(;*e && !(*e==c || *e==0x0D || *e==0x0A);*ee++=*e++);
		if(*e!=c) MadelineErrorReport("GetToken(): bracketed date literal is missing terminating delimiter");
		e++;   /* skip over the ending delimiter */
		*ee='\0';
		pT->i=NOINTERNALTOKEN;
		pT->v=(double)Read_YYYYMMDD_DateString(pT->e);
		return pT->t=DATEVALUE;
		
	case '}':
		/* PROGRAM CONTROL BLOCK TERMINATOR */
		*ee++=*e++;
		*ee='\0';
		pT->i=CTRLC;
		pT->v=0.0;
		pT->u.pvf=ExecuteEndOfControlBlock; /* Set the union pointer to the ExecuteEndOfControlBlock function */
		return pT->t=COMMAND;
		//return pT->t=PROGCTRLOPERATOR;
		
	case '.':
		/*                                             */
		/* case of a dot operator not inside a number: */
		/*                                             */
		pT->i=*ee++=*e++;
		*ee='\0';
		return pT->t=OPERATOR;
		          
	case '?':
		/*                                       */
		/* Question mark used in place of "show" */
		/* command:                              */
		/*                                       */
		*ee++=*e++;
		*ee='\0';
		pT->i=SHOWC;
		pT->v=0.0;
		pT->u.pvf=Show;        /* Set the union pointer to the Show() function */
		return pT->t=COMMAND;
		
	default:
		/*                                       */
		/* character is an alphabetic character: */
		/*                                       */

		/*                                           */
		/* a  NUMERICVARIABLE or STRINGVARIABLE      */
		/*                                           */
		/* or OPERATOR like AND, OR or NOT           */
		/*                                           */
		/* or OPERATOR that is a FUNCTION            */
		/*             like SIN,COS,SQRT, etc.       */
		/*                                           */
		/* or NUMERICVALUE    like MISSING, PI, or E */
		/*                                           */
		
		/*                                     */
		/* make an upper-case copy of a token  */
		/* consisting of letters, numbers      */
		/* or the underscore character:        */
		/*                                     */
		for(;*e && (isalnum(*e) || *e=='_' || *e=='#' || *e=='}' );*ee++=toupper(*e++));
		*ee='\0';

		/*                                */
		/* See if token is in token tree: */
		/*                                */
		if(knownTokens.lookup(pT->e,pT)){
		
			 return pT->t;
			 
		}else{
			/*                                                */
			/* not an internal command or variable or keyword */
			/*                                                */
			pT->i=NOINTERNALTOKEN;
			if(!g->db)
				MadelineErrorReport("GetToken(): \"%s\" is not a recognized token",pT->e);

			pT->f=g->db->FieldNumber(g->db,pT->e); /* get and store field number of variable */
			if(pT->f==MISSING){
				MadelineErrorReport("GetToken(): \"%s\" is not a recognized token or field name",pT->e);
			}else{
				/*                                                                 */
				/* determine if variable is a string variable or numeric variable: */
				/*                                                                 */
				
				switch( c=g->db->FieldType(g->db,pT->f) ){
				
				case 'C':
					return pT->t=STRINGVARIABLE;
				
				case 'N':
					return pT->t=NUMERICVARIABLE;
				
				case 'D':
					return pT->t=DATEVARIABLE;
				
				default:
					MadelineErrorReport("GetToken(): Operations are not available for fields of type \"%c\" (0x%4x)",c,c);
				}
			}
		}

	}
	return MISSING;
}


/*                                                        */
/*                                                        */
/* EvaluateValueOrVariable():                             */
/*                                                        */
/* Places the current value of a VARIABLE or the constant */
/* value of a VALUE token into result.                    */
/*                                                        */
/* This is the expression parser's atomic function.       */
/*                                                        */
void EvaluateValueOrVariable(PRESULT result){

	extern PCONTROL g;

	switch(pT->t){
	case NUMERICVALUE:

		result->t  =NUMERICVALUE;
		result->v  =pT->v;
		GetToken();
		break;
		
	case STRINGVALUE:

		result->t  =STRINGVALUE;
		strcpy(result->s,pT->e);
		GetToken();
		break;
	
	case DATEVALUE:
		result->t  =DATEVALUE;
		result->v  =pT->v;
		GetToken();
		break;
				
	case NUMERICVARIABLE:

		result->t  =NUMERICVALUE;
		if(g->db->cr==NO_RECORD_EXISTS){
			/*                                                          */
			/* referencing a field, but with NO record in the database: */
			/*                                                          */
			result->v=GetNumericCoreFieldValue(g->db,pT->f,g->cpd,g->cp);
		}else{
			/*              */
			/* IN database: */
			/*              */
			result->v  =g->db->ReadDouble(g->db,pT->f,g->db->cr);
			/*                                              */
			/* check for missing values: now done in ndbf.c */
			/*                                              */
			/* if(IsNMV(result->v))                         */
			/* 		result->v=DOUBLEMISSINGVALUE;              */
		}
		GetToken();
		break;

	case STRINGVARIABLE:
	
		result->t  =STRINGVALUE;
		if(g->db->cr==NO_RECORD_EXISTS){
			/*                  */
			/* NOT in database: */
			/*                  */
			strcpy(result->s,GetCharacterCoreFieldValue(g->db,pT->f,g->cpd,g->cp));
		}else{
			/*              */
			/* IN database: */
			/*              */
			strcpy(result->s,g->db->ReadString(g->db,NULL,pT->f,g->db->cr));
		}
		GetToken();
		break;

	case DATEVARIABLE:
	
		result->t  =DATEVALUE;
		if(g->db->cr==NO_RECORD_EXISTS){
			/*                                                          */
			/* referencing a field, but with NO record in the database: */
			/*                                                          */
			result->v=MISSINGDATE;
			
			/*                                                           */
			/*result->v=GetDateCoreFieldValue(g->db,pT->f,g->cpd,g->cp); */
			/*                                                           */
			
		}else{
			/*              */
			/* IN database: */
			/*              */
			result->v  =g->db->ReadDate(g->db,pT->f,g->db->cr);
		}
		GetToken();
		break;
	
	
	case INTERNALVARIABLE:
	
		/*                                                             */
		/* referencing an internal variable to find out it's contents: */
		/* (internal arrays are handled in EvaluateFunctionCallOrArray():     */
		/*                                                             */
		/* An internal variable can be of three types,                 */
		/* NUMERICVALUE,STRINGVALUE, or FIELDNAMEVALUE:                */
		/*                                                             */
		if(pT->u.v->t      ==NUMERICVALUE){
			result->t       =NUMERICVALUE;
			result->v       =pT->u.v->e.v;  /* get the value; */

		}else if(pT->u.v->t==STRINGVALUE){
			result->t       =STRINGVALUE;                   
			strcpy(result->s,pT->u.v->e.s);

		}else if(pT->u.v->t==FIELDNAMEVALUE){
			/*                                                         */
			/* There is actually no need to distinguish FIELDNAMEVALUE */
			/* from STRINGVALUE when getting the value, but this way   */
			/* simply emphasizes the distinction that a FIELDNAMEVALUE */
			/* can only be so long:                                    */
			/*                                                         */
			/* LNFIELDNAME is probably obsolete now that the database  */
			/* engine can read multiple formats, not just Foxpro:      */
			/*                                                         */
			result->t       =STRINGVALUE;
			strncpy(result->s,pT->u.v->e.s,LNFIELDNAME);
		}
		GetToken();
		break;

	case INDIVIDUALVARIABLE:
	case INDIVIDUALSTRING:
		
		/*                                                */
		/* Evaluate internal numeric or string variables: */
		/*                                                */
		EvaluateIndividualVariableReference(result);
		GetToken();
		break;

	case INDIVIDUALPOINTER:
		/*                                                               */
		/* dereference the father or mother, then evaluate the variable: */
		/* EvaluateIndividualPointerReference() ends with a GetToken()   */
		/* so we don't need one here:                                    */
		/*                                                               */
		EvaluateIndividualPointerReference(g,result);
		break;
	
	default:

		MadelineErrorReport("EvaluateValueOrVariable(): Syntax error in expression");
		break;
	}
}

/*                                                                                   */
/*                                                                                   */
/* EvaluateIndividualVariableReference():                                            */
/*                                                                                   */
/* Part of the expression parser, EvaluateIndividualVariableReference() allows       */
/* the user to query information stored in an INDIVIDUAL structure for an individual */
/* in a pedigree, such as an individual's number of offspring, number of mates,      */
/* personal or familial identifier, and so on.                                       */
/*                                                                                   */
/* Access to the individual is via the pointers in the CONTROL structure for the     */
/* current family (g->cpd) and current individual (g->cp).  These of course won't    */
/* be available if no database is opened.                                            */
/*                                                                                   */
void EvaluateIndividualVariableReference(PRESULT result){

	extern PCONTROL g;
	extern PTOKENINFO pT;
	
	/*                                                                      */
	/* This function should only be called when g->cp and g->cpd have been  */
	/* filled in correctly:                                                 */
	/*                                                                      */
	if(!g->cp)
		MadelineErrorReport("EvaluateIndividualVariableReference(): internal error: g->cp not mapped!");

	switch(pT->i){
	case INK:
		/*                                                      */
		/* return number of offspring ("nk" for "no. of kids"): */
		/*                                                      */
		result->t=NUMERICVALUE;
		result->v=(double) g->cp->nk;
		break;
		
	case INA:
		/*                                                                                  */
		/* return number of spouses ("na" for "no. of ai4 ren2" (get a Chinese dictionary): */
		/*                                                                                  */
		result->t=NUMERICVALUE;
		result->v=(double) g->cp->na;
		break;
		
	case IID:
		/*                               */
		/* return individual identifier: */
		/*                               */
		result->t=STRINGVALUE;
		strcpy(result->s,g->cp->id);
		break;
		
	case IFID:
		/*                           */
		/* return family identifier: */
		/*	                          */
		result->t=STRINGVALUE;
		strcpy(result->s,g->cpd->fid);	
		break;
	
	case INFF:
		/*                                                       */
		/* return number of founding fathers in the individual's */
		/* pedigree:                                             */
		/*                                                       */
		result->t=NUMERICVALUE;
		result->v=(double) g->cpd->nff;
		break;

	case INFM:
		/*                                                       */
		/* return number of founding mothers in the individual's */
		/* pedigree:                                             */
		/*                                                       */
		result->t=NUMERICVALUE;
		result->v=(double) g->cpd->nfm;
		break;
		
	case INIP:
		/*                                                       */
		/* return the total number of people in the individual's */
		/* pedigree:                                             */
		/*                                                       */
		result->t=NUMERICVALUE;
		result->v=(double) g->cpd->n;
		break;

	case INCSP:
		/*                                                                */
		/* return the individual's "childless spouse" boolean flag state: */
		/*	                                                               */
		result->t=NUMERICVALUE;
		result->v=(double) g->cp->csp;
		break;

	case INEXC:
		/*                                                               */
		/* returns the individual's "excluded" boolean flag state:       */
		/* This only indicates if an individual is marked for exclusion  */
		/* -- in fact, they can still be included in output even if      */
		/* marked for exclusion because they may be required to maintain */
		/* structural integrity of the pedigree                          */
		/*                                                               */
		result->t=NUMERICVALUE;
		result->v=(double) g->cp->excluded;
		break;

	case ININC:
		/*                                                        */
		/* return the individual's "included" boolean flag state: */
		/*                                                        */
		result->t=NUMERICVALUE;
		result->v=(double) g->cp->included;
		break;
		
	case INHDT:
		/*                                                         */
		/* Returns the individual's "has data" boolean flag state. */
		/* This can only be TRUE if a file has been written which  */
		/* required evaluation of the "has data" state:            */
		/*                                                         */
		result->t=NUMERICVALUE;
		result->v=(double) g->cp->hasdata;
		break;

	case INFLT:
		/*                                                            */
		/* Returns the individual's boolean "unconnected" flag state. */
		/* If unconnected==TRUE, the individual is sometimes referred */
		/* to as a FLOATER, hence "INFLT" above:                      */
		/*                                                            */
		result->t=NUMERICVALUE;
		result->v=(double) g->cp->unconnected;
		break;
		
	case INDBF:
		/*                                                            */
		/* Returns the individual's "in database" boolean flag state. */
		/* Virtual individuals return FALSE:                          */
		/*                                                            */
		result->t=NUMERICVALUE;
		result->v=g->cp->db?1.0:0.0;
		break;
		
	case INISP:
		/*                                                            */
		/* return the individual's "is a proband" boolean flag state: */
		/*                                                            */
		result->t=NUMERICVALUE;
		result->v=g->cp->proband==MISSING?DOUBLEMISSINGVALUE:(double)g->cp->proband;
		break;
		
	case INGTC:
		/*                                                           */
		/*  return genotyped count                                   */
		/*                                                           */
		result->t=NUMERICVALUE;
		result->v=(double) g->cp->genotypecount;
		break;
		
	case INGTP:
		/*                                                           */
		/*  return percent genotyped:                                */
		/*                                                           */
		result->t=NUMERICVALUE;
		result->v=(double) 100.0*g->cp->genotypecount/g->db->n.go;
		break;
		
	case IRCI:
		/*                                                            */
		/* return the individual's database record number:            */
		/*                                                            */
		result->t=NUMERICVALUE;
		result->v=g->cp->db?g->cp->rci:DOUBLEMISSINGVALUE;
		break;
		
	case INFEM:
		/*                                                            */
		/* return the individual's gender attribute:                  */
		/*                                                            */
		result->t=NUMERICVALUE;
		result->v=g->cp->female==MISSING?DOUBLEMISSINGVALUE:(double)g->cp->female;
		break;
		
	case INFND:
		/*                                                             */
		/* Return true if individual is a founder, else false:         */
		/* A founder has no parents: a non-founder has both parents.   */
		/* The case of a single parent is handled by Madeline creating */
		/* a virtual parent, so that case doesn't occur here:          */
		/*                                                             */
		result->t=NUMERICVALUE;
		result->v=(double)!(g->cp->m && g->cp->f);
		break;
		
	case INPFD:
		/*                                                             */
		/* Return true if individual is a PRIMARY founder, else false: */
		/* A primary founder is at the top of the pedigree tree.       */
		/* Algorithmically, they are the parents of people who have no */
		/* grandparents.                                               */
		/*                                                             */
		result->t=NUMERICVALUE;
		result->v=(double)g->cp->IsPrimaryFounder;
		break;
		
	case INCPX:
		/*                                                             */
		/* Returns pedigree complexity expressed as 2N-F:              */
		/*                                                             */
		result->t=NUMERICVALUE;
		result->v=(double)2.0*g->cpd->NumberOfNonFounders - g->cpd->NumberOfFounders;
		break;
		
	case INNOF:
		/*                                                             */
		/* Returns the Number Of Founders:                             */
		/*                                                             */
		result->t=NUMERICVALUE;
		result->v=(double)g->cpd->NumberOfFounders;
		break;
		
	case INNON:
		/*                                                             */
		/* Returns the Number Of NonFounders:                          */
		/*                                                             */
		result->t=NUMERICVALUE;
		result->v=(double)g->cpd->NumberOfNonFounders;
		break;
		
	case INNOA:
		/*                                                             */
		/* Returns the Number of Affecteds (_NumberAffected)           */
		/* in the pedigree:                                            */
		result->t=NUMERICVALUE;
		result->v=(double)g->cpd->NumberOfAffected;
		break;
		
	case INNOAP:
		/*                                                             */
		/* Returns the Number of Affecteds Pairs                       */
		/* (_NumberOfAffectedPairs) in the pedigree.                   */
		/* NumberOfPairs() is from dataevaluation.c                    */
		result->t=NUMERICVALUE;
		result->v=(double)NumberOfPairs(g->cpd->NumberOfAffected);
		break;
		
	case INAFF:
		/*                                                            */
		/* return the individual's AffectionStatus attribute:         */
		/*                                                            */
		result->t=NUMERICVALUE;
		result->v=g->cp->affected==MISSING?DOUBLEMISSINGVALUE:(double)g->cp->affected;
		break;
		
	case INDED:
		/*                                                            */
		/* return the individual's DeceasedStatus attribute:          */
		/*                                                            */
		result->t=NUMERICVALUE;
		result->v=g->cp->dead==MISSING?DOUBLEMISSINGVALUE:(double)g->cp->dead;
		break;
	case INCSG:
		/*                                                            */
		/* return the individual's Consanguinity attribute:           */
		/*                                                            */
		result->t=NUMERICVALUE;
		result->v=g->cp->consanguinity?1.0:0.0;
		break;
		
	case INMZT:
		/*                                                            */
		/* return true if Monozygotic twin:                           */
		/*                                                            */
		result->t=NUMERICVALUE;
		result->v=isupper(g->cp->twin)?1.0:0.0;
		break;
		
	case INDZT:
		/*                                                            */
		/* return true if Dizygotic twin:                             */
		/*                                                            */
		result->t=NUMERICVALUE;
		result->v=islower(g->cp->twin)?1.0:0.0;
		break;
		
	case INMDL:
		/*                                                            */
		/* return true if MendelianInconsistent                       */
		/*                                                            */
		result->t=NUMERICVALUE;
		result->v=(g->cp->integrity!=NULL);
		break;
		
	case INMDC:
		/*                                                            */
		/* return count of MendelianInconsistencies                   */
		/*                                                            */
		result->t=NUMERICVALUE;
		result->v=(double) CountIndividualMarkerInconsistencies(g,g->cp);
		break;
		
	case INMDP:
		/*                                                            */
		/* return Percentage of Mendelian Inconsistencies             */
		/*                                                            */
		result->t=NUMERICVALUE;
		result->v=(double) 100.0*CountIndividualMarkerInconsistencies(g,g->cp)/g->db->n.go;
		break;
		
	}
	return;
}

/*                                                                        */
/* EvaluateIndividualPointerReference(PRESULT result):                    */
/*                                                                        */
/* This function is part of the expression parser.  An INDIVIDUAL         */
/* structure maintains pointers to his or her father and mother.          */
/*                                                                        */
/* This function handles requests such as the following:                  */
/*                                                                        */
/* M>show _father.age                                                     */
/*                                                                        */
/* ... which would return the age not of the current individual, but      */
/* rather of his FATHER.                                                  */
/*                                                                        */
/* A lot of the detail in functions like this concerns returning the      */
/* correct type of missing value indicator in case the current individual */
/* does not have a father (i.e., is a founder or singleton):              */
/*                                                                        */
void EvaluateIndividualPointerReference(PCONTROL c,PRESULT result){


	PINDIVIDUAL p;  /* pointer to father or mother of the current person */
	PINDIVIDUAL cp; /* current person                                    */
	
	/*                                                          */
	/* pT refers to the operand, in this case a reference to    */
	/* the father, mother, or other relation of the individual: */
	/*                                                          */
	switch(pT->i){
	
	case IDAD:
	
		/*                                          */
		/* dereference INDIVIDUAL's father pointer: */
		/*                                          */
		/* c->cp is the current INDIVIDUAL:         */
		/*                                          */
		if(c->cp) p=c->cp->f; /* pointer to father  */
		else      p=NULL;
		break;
	
	case IMOM:
	
		/*                                          */
		/* dereference INDIVIDUAL's mother pointer: */
		/*                                          */
		/* c->cp is the current INDIVIDUAL:         */
		/*                                          */
		if(c->cp) p=c->cp->m; /* pointer to mother  */
		else      p=NULL;
		break;
	
	case IPSPS:

		/*                                                         */
		/* dereference INDIVIDUAL's first spouse                   */
		/*                                                         */
		if(c->cp && c->cp->na) p=c->cp->a[0]; /* pointer to spouse */
		else      p=NULL;
		break;

	case IPK01:

		/*                                                           */
		/* dereference INDIVIDUAL's first child:                     */
		/*                                                           */
		if(c->cp && c->cp->nk>=1) p=c->cp->k[0]; /* pointer to child */
		else      p=NULL;
		break;

	case IPK02:

		/*                                                           */
		/* dereference INDIVIDUAL's second child:                    */
		/*                                                           */
		if(c->cp && c->cp->nk>=2) p=c->cp->k[1]; /* pointer to child */
		else      p=NULL;
		break;

	case IPK03:

		/*                                                           */
		/* dereference INDIVIDUAL's third child:                     */
		/*                                                           */
		if(c->cp && c->cp->nk>=3) p=c->cp->k[2]; /* pointer to child */
		else      p=NULL;
		break;

	case IPK04:

		/*                                                           */
		/* dereference INDIVIDUAL's fourth child:                    */
		/*                                                           */
		if(c->cp && c->cp->nk>=4) p=c->cp->k[3]; /* pointer to child */
		else      p=NULL;
		break;

	case IPK05:

		/*                                                           */
		/* dereference INDIVIDUAL's fifth child:                     */
		/*                                                           */
		if(c->cp && c->cp->nk>=5) p=c->cp->k[4]; /* pointer to child */
		else      p=NULL;
		break;

	case IPK06:

		/*                                                           */
		/* dereference INDIVIDUAL's sixth child:                     */
		/*                                                           */
		if(c->cp && c->cp->nk>=6) p=c->cp->k[5]; /* pointer to child */
		else      p=NULL;
		break;

	case IPK07:

		/*                                                           */
		/* dereference INDIVIDUAL's seventh child:                   */
		/*                                                           */
		if(c->cp && c->cp->nk>=7) p=c->cp->k[6]; /* pointer to child */
		else      p=NULL;
		break;

	case IPK08:

		/*                                                           */
		/* dereference INDIVIDUAL's eighth child:                    */
		/*                                                           */
		if(c->cp && c->cp->nk>=8) p=c->cp->k[7]; /* pointer to child */
		else      p=NULL;
		break;

	case IPK09:

		/*                                                           */
		/* dereference INDIVIDUAL's ninth child:                     */
		/*                                                           */
		if(c->cp && c->cp->nk>=9) p=c->cp->k[8]; /* pointer to child */
		else      p=NULL;
		break;

	case IPK10:

		/*                                                            */
		/* dereference INDIVIDUAL's tenth child:                      */
		/*                                                            */
		if(c->cp && c->cp->nk>=10) p=c->cp->k[9]; /* pointer to child */
		else      p=NULL;
		break;
		
	case IPMGM:
		/*                                                            */
		/* Dereference INDIVIDUAL's Maternal Grandmother, if present: */
		/*                                                            */
		if(c->cp && c->cp->m) p=c->cp->m->m;
		else     p=NULL;
		break;
		
	case IPMGF:
		/*                                                            */
		/* Dereference INDIVIDUAL's Maternal Grandfather, if present: */
		/*                                                            */
		if(c->cp && c->cp->m) p=c->cp->m->f;
		else     p=NULL;
		break;
		
	case IPPGM:
		/*                                                            */
		/* Dereference INDIVIDUAL's Paternal Grandmother, if present: */
		/*                                                            */
		if(c->cp && c->cp->f) p=c->cp->f->m;
		else     p=NULL;
		break;
		
	case IPPGF:
		/*                                                            */
		/* Dereference INDIVIDUAL's Paternal Grandfather, if present: */
		/*                                                            */
		if(c->cp && c->cp->m) p=c->cp->f->f;
		else     p=NULL;
		break;
		
	case ISELF:
		/*                                              */
		/* Dereference INDIVIDUAL's self pointer:       */
		/*                                              */
		/* This, of course, seems superfluous, but      */
		/* it's actually nice to have in order to       */
		/* enforce clarity in query statements such     */
		/* as "view for _mother.age - _self.age <= 17". */
		/*                                              */
		if(c->cp) p=c->cp; /* pointer to mother  */
		else      p=NULL;
		break;
		
	default:
	
		MadelineErrorReport("EvaluateIndividualPointerReference(): unrecognized pointer reference");
		break;

	}
	
	/*                       */
	/* Get the dot operator: */
	/*                       */
	GetToken();
	/*                                    */
	/* Should now be on the "." operator: */
	/* Check to make sure it's there:     */
	/*                                    */
	if(pT->i!='.')
		MadelineErrorReport("EvaluateIndividualPointerReference(): \".\" expected");
	/*                                                                       */
	/* Get the next token which should be a field or internal variable name: */
	/*                                                                       */
	GetToken();
	
	/*                       */
	/* process for person p: */
	/*	                      */
	if(p){
		/*                                                                  */
		/* we have a valid pointer for dereferencing:                       */
		/*                                                                  */
		/* -- store record number of c->cp                                  */
		/* -- go to the record, if its in the database:                     */
		/* -- evaluate the value of the field variable or internal variable */
		/*                                                                  */
		
		/*                                                     */
		/* save current c->cp reference (this is the offspring */
		/* whose parent, p, is being dereferenced:             */
		/*                                                     */
		cp =c->cp;
	
		/*                                   */
		/* place new p into global position: */
		/*                                   */
		c->cp=p;
		/*                                        */
		/* place new current record into position */
		/*	                                       */
		if(p->db) GoTo(p->db,p->rci);			
		else      c->db->cr=NO_RECORD_EXISTS;		
		/*                                                         */
		/* Only a variable should be allowed, no direct value,     */
		/* but we have no way of checking for this with the        */
		/* current code in EvaluateValueOrVariable().  However,    */
		/* we can check for it below for the cases where           */
		/* p is NULL, so that's good enough:                       */
		/*                                                         */
		/* Why call EvaluateFunctionCallOrArray()? The reason is that     */
		/* EvaluateFunctionCallOrArray() also handles array dereferencing */
		/* and of course it's OK to have an array after the dot    */
		/* operator:                                               */
		/*                                                         */
		EvaluateFunctionCallOrArray(result);
		
		/*                                   */
		/* restore previous globals from cp: */
		/*                                   */
		c->cp=cp;
		if(cp->db) GoTo(cp->db,cp->rci);
		else       c->db->cr=NO_RECORD_EXISTS;

	}else{
		/*                                                    */
		/* can't dereference: return correct form of MISSING: */
		/*                                                    */
		
		/*                                                         */
		/* Check the token type of the variable in order to decide */
		/* which type to return:                                   */
		/*                                                         */
		switch(pT->t){
		case NUMERICVARIABLE:
		case INDIVIDUALVARIABLE:

			result->t=NUMERICVALUE;
			result->v=DOUBLEMISSINGVALUE;
			GetToken();
			break;

		case STRINGVARIABLE:
		case INDIVIDUALSTRING:
		
			result->t=STRINGVALUE;
			/* below give same result as strcpy(result->s,CHARMISSINGSTRING); */
			result->s[0]='\0';
			GetToken();
			break;
			
		case DATEVARIABLE:
			result->t=DATEVALUE;
			result->v=MISSINGDATE;
			GetToken();
			break;
			
		case INDIVIDUALARRAY:
		case INDIVIDUALPOINTER:
			/*             */
			/* Keep going: */
			/*             */
			EvaluateFunctionCallOrArray(result);
			break;

		case AFUNCTION:
			/*                      */
			/* Get the parentheses: */
			/*                      */
			GetToken();
			/*                                                   */
			/* Call EvaluateParenthesizedExpression()            */
			/* in order to put the remaining tokens onto the     */
			/* token stack -- not doing so can cause an error    */
			/* when repeating for a later individual: result is  */
			/* passed to hold the result, but we then dump the   */
			/* answer:                                           */
			/*                                                   */
			EvaluateParenthesizedExpression(result);
			/*                                                      */
			/* We already knew, without calling                     */
			/* EvaluateParenthesizedExpression, that this case must */
			/* evaluate to missing:                                 */
			/*                                                      */
			result->t=NUMERICVALUE;
			result->v=DOUBLEMISSINGVALUE;
						
			break;
						
		default:
		
			MadelineErrorReport("EvaluateIndividualPointerReference():\n\tnumeric, date or string variable expected");
			break;
		
		}
		/*                                                                         */
		/* Finally, get the next token just as EvaluateValueOrVariable() would do: */
		/*                                                                         */
		/* GetToken();                                                             */
		/*                                                                         */
		/*                                                                         */
	}
	return;
}



/*                                                                 */
/*                                                                 */
/* EvaluateParenthesizedExpression(): evaluates a parenthesized or */
/* bracketed expression, or expression delimited by commas passed  */
/* to a function.  This function is part of the expression parser. */
/*                                                                 */
void EvaluateParenthesizedExpression(PRESULT result){

	unsigned short op;
	
	if(pT->i=='(' || pT->i=='['){

		op=(pT->i=='(' ? ')' : ']' );

		GetToken();
		EvaluateJoiners(result);
		/*                                             */
		/* should have ended with closing parentheses: */
		/*                                             */
		if(pT->i!=op)
			MadelineErrorReport("EvaluateParenthesizedExpression(): missing terminating \"%c\"",op);
		/*                    */
		/* get the next token */
		/*                    */
		GetToken();
	}else
		EvaluateValueOrVariable(result);

}


/*                                                              */
/* EvaluateFunctionCallOrArray(): evaluates a function or              */
/* a bracketed internal array or a bracketed array referencing  */
/* a vector in an INDIVIDUAL (ie., and INDIVIDUALARRAY).        */
/*                                                              */
/* This function is part of the expression parser.              */
/*                                                              */
void EvaluateFunctionCallOrArray(PRESULT result){

	TOKENINFO op;
	
	RESULT temp[MAXPARAMETERS];
	int i;
	
	VARIABLE aa_key;
	PVARIABLE paa_value;
	
	extern PCONTROL g;
	

	/*strcpy(op.e,pT->e); // not necessary to have this stored */
	op.i  =pT->i;
	op.t  =pT->t;
	op.u.p=pT->u.p;
	
	if(     op.t==NFUNCTION        || 
		op.t==SFUNCTION        || 
		op.t==CFUNCTION        || 
		op.t==XFUNCTION        ||
		op.t==ICCFUNCTION      ||
		op.t==INTERNALARRAY    ||
		op.t==USERARRAY        ||
		op.t==INDIVIDUALARRAY  ||
		op.t==ASSOCIATIVEARRAY ||
		op.t==AFUNCTION        ||
		op.t==TDRNFUNCTION     ||
		op.t==TNRDFUNCTION     ||
		op.t==TVRDFUNCTION
	){

		/*                                              */
		/* Get a token and make sure it is a '(' or '[' */
		/*                                              */
		GetToken();             
		/*                                                */
		/* function call must be parenthesized            */
		/* so the new token should be left  parentheses:  */
		/* while a reference to an array must be followed */
		/* by a bracketed expression                      */
		/*                                                */
		if(op.t==INTERNALARRAY || op.t==INDIVIDUALARRAY || op.t==USERARRAY || op.t==ASSOCIATIVEARRAY){ 
			if(pT->i!='[')
				MadelineErrorReport("EvaluateFunctionCallOrArray(): Array name must be followed by bracketed expression.");
		}else if(pT->i!='('){
			MadelineErrorReport("EvaluateFunctionCallOrArray(): Function call must be followed by a parenthesized expression.");
		}
	}

	/*                                                */
	/* Special case of a multiple-parameter function: */
	/*                                                */
	switch(op.t){
	case XFUNCTION:
	case ICCFUNCTION:
		/*                                                               */
		/* Get another token in order to go past the function call's "(" */
		/* on a multiple-parameter function call:                        */
		/*                                                               */
		for(i=0;pT->i=='(' || pT->i==',';i++){
			GetToken();
			/* DEBUG: IS THIS CORRECT? WHY NOT JUST EvaluateParenthesizedExpression()? */
			EvaluateJoiners((PRESULT)temp+i);
		}
		break;
		
	case AFUNCTION:
		/*                                */
		/* Handle the aggregate function: */
		/*                                */
		EvaluateAggregateFunction(g,&op,result);
		/* DEBUG: WHERE ARE WE?                                                             */
		/* lprintf("==> DEBUGGING: After EvaluateAggregateFunction, pT->i=\"%c\"\n",pT->i); */
		break;
		
	case TVRDFUNCTION:
		
		/* These are functions taking a void parameter:              */
		/* Get the next token which should be a closing parentheses: */
		GetToken();             
		break;
		
	default:
		/*                                                                */
		/* Evaluating a single parameter is easy because we can just call */
		/* EvaluateParenthesizedExpression() to do the work:              */
		/*                                                                */
		EvaluateParenthesizedExpression(result);
		break;
	}
	
	/*                                                                */
	/* After an getting the multiple parameters of an XFUNCTION, the  */
	/* token should be a closing ')':                                 */
	/*                                                                */
	if(op.t==XFUNCTION || op.t==ICCFUNCTION || op.t==TVRDFUNCTION){
		if(pT->i!=')')
			MadelineErrorReport("EvaluateFunctionCallOrArray(): Function call not terminated with \")\".");
		/*                     */
		/* Get the next token: */
		/*                     */
		GetToken();
	}       

	/*                                     */
	/* perform evaluations, if applicable: */
	/*                                     */
	switch(op.t){
	case INTERNALARRAY:
	case USERARRAY:
	
		/*                                  */
		/* error check on array boundaries: */
		/*                                  */
		if(result->t!=NUMERICVALUE){
			MadelineErrorReport("EvaluateFunctionCallOrArray(): Array subscript must be numeric.");
		}
		if(result->v<1 || result->v>=op.u.a->n+1){
			MadelineErrorReport("EvaluateFunctionCallOrArray(): Array subscript out of bounds.\nThe first element in the array is at postion 1\nThe last element in the array is at position %i",
			op.u.a->n-1
			);
		}
		
		switch(op.u.a->t){
		case NUMERICVALUE:
			result->t=NUMERICVALUE;
			result->v=op.u.a->e[(int)result->v-1].v;
			break;
		case DATEVALUE:
			result->t=DATEVALUE;
			result->v=op.u.a->e[(int)result->v-1].v;     
			break;
		case STRINGVALUE:
			result->t=STRINGVALUE;
			strcpy(result->s,op.u.a->e[(int)result->v-1].s);
			break;
		}
		break;
		
	case ASSOCIATIVEARRAY:
		
		/* Set up the SEARCH KEY */
		aa_key.t=result->t;
		if(aa_key.t==NUMERICVALUE || aa_key.t==DATEVALUE) aa_key.e.v=result->v;
		else                                              aa_key.e.s=result->s;
		/* Get a pointer to the Node (NULL if not found) */
		paa_value=LookupValueFromKey(op.u.aa,&aa_key);
		if(paa_value){
			/* Node found */
			result->t=paa_value->t;
			switch(result->t){
			case NUMERICVALUE:
			case DATEVALUE:
				result->v=paa_value->e.v;
				break;
			case STRINGVALUE:
				strcpy(result->s,paa_value->e.s);
				break;
			}
		}else{
			MadelineErrorReport("EvaluateFunctionCallOrArray(): Node not found.\nThe specified key is not present in this associative array.");
		}
		break;
		
	case NFUNCTION:
		
		/*                                   */
		/* must have correct parameter type: */
		/*                                   */
		if(result->t!=NUMERICVALUE)
			MadelineErrorReport("EvaluateFunctionCallOrArray(): numeric parameter required.");
		
		result->t =NUMERICVALUE;        
		result->v =op.u.pnf(result->v);
		break;
		
	case TDRNFUNCTION:
		
		/* Function taking a date (TD) returning a number (RN): */
		
		/*                                   */
		/* must have correct parameter type: */
		/*                                   */
		if(result->t!=DATEVALUE)
			MadelineErrorReport("EvaluateFunctionCallOrArray(): date parameter required.");
		
		result->t =NUMERICVALUE;        
		result->v =op.u.pnf(result->v);
		break;
		
	case TNRDFUNCTION:
		
		/* Function taking a number (TN) returning a date (RD): */
		
		/*                                   */
		/* must have correct parameter type: */
		/*                                   */
		if(result->t!=NUMERICVALUE)
			MadelineErrorReport("EvaluateFunctionCallOrArray(): numeric parameter required.");
		
		result->t =DATEVALUE;        
		result->v =op.u.pnf(result->v);
		break;
		
	case TVRDFUNCTION:
		
		/* Function taking void (TV) returning a date (RD): */
		
		/*                                 */
		/* Void function returning a date: */
		/*                                 */
		result->t =DATEVALUE;
		result->v =op.u.pnf(0);
		break;
		
	case CFUNCTION:
		
		/*                                   */
		/* must have correct parameter type: */
		/*                                   */
		if(result->t!=STRINGVALUE)
			MadelineErrorReport("EvaluateFunctionCallOrArray(): character string parameter required.");
		
		result->t =NUMERICVALUE;
		result->v =op.u.pcf(result->s);
		break;
		
	case XFUNCTION:
		
		/*                                    */
		/* must have correct parameter types: */
		/*                                    */
		if(temp[0].t!=STRINGVALUE)
			MadelineErrorReport("EvaluateFunctionCallOrArray(): first parameter must be a character string.");
		if(temp[1].t!=NUMERICVALUE)
			MadelineErrorReport("EvaluateFunctionCallOrArray(): second parameter must be a numeric value.");
		if(temp[2].t!=NUMERICVALUE)
			MadelineErrorReport("EvaluateFunctionCallOrArray(): third parameter must be a numeric value.");

		result->t = STRINGVALUE;
		op.u.pxf(result->s,temp[0].s,(int)temp[1].v,(int)temp[2].v);
		break;
	
	case ICCFUNCTION:
	
		/*                                    */
		/* must have correct parameter types: */
		/*                                    */
		if(temp[0].t!=STRINGVALUE)
			MadelineErrorReport("EvaluateFunctionCallOrArray(): first parameter must be a character string.");
		if(temp[1].t!=STRINGVALUE)
			MadelineErrorReport("EvaluateFunctionCallOrArray(): second parameter must be a character string.");
		
		result->t = NUMERICVALUE;
		result->v = op.u.pif(temp[0].s,temp[1].s);
		break;
	
	case INDIVIDUALARRAY:
	
		/*                                      */
		/* Evaluate individual array reference: */
		/*                                      */
		EvaluateIndividualArrayReference(g,&op,result);	
		break;
	}
	/*                                   */
	/* get here if just passing through: */
	/*                                   */
	
}


/*                                                                                    */
/* EvaluateAggregateFunction().  This function is part of the expression              */
/* parser.  Madeline has special functions for calculating things like the            */
/* mean BMI (body mass index) of sibs in a sibship -- BMI is just an example:         */
/* you could get the mean of any numeric field variable.  EvaluateAggregateFunction() */
/* handles these sorts of functions:                                                  */
/*                                                                                    */
void EvaluateAggregateFunction(PCONTROL c,PTOKENINFO pop,PRESULT result){

	PINDIVIDUAL  p;
	PINDIVIDUAL  mom;
	int i;
	int j;
	int female; /* true if p is female */
	RESULT temp;
	PTOKENINFO StartingToken;
	double ss;           /* running sum                                              */
	int cc;              /* integer counter: count non-missing                       */
	int cm;              /* counts missing                                           */
	int ct;              /* counts true                                              */
	int cf;              /* counts false                                             */
	int RepeatStatus;    /* for saving repeat status                                 */
	double *vv=NULL;     /* for saving values for calculating std. dev. and variance */
	double dev;          /* deviation from mean                                      */
	double svar;         /* for variance (or standard deviation)                     */
	double min=DOUBLE_MAXIMUM; /* For calculating the minimum                        */
	double max=DOUBLE_MINIMUM; /* For calculating the maximum                        */

	extern PTOKENINFO pT;	

	/*                */
	/* for debugging: */
	/*                */
	if(!c->cp)
		MadelineErrorReport("EvaluateAggregateFunction():\n\tc->cp not mapped! --Probably no pedigree table is open.");
	
	if(!(pop->i==AMEAN ||
	     pop->i==ASUM  ||
	     pop->i==ACOUNT||
	     pop->i==ACTRUE|| 
	     pop->i==ACFALS|| 
	     pop->i==ACMISS|| 
	     pop->i==ASTDV || 
	     pop->i==AVAR  ||
	     pop->i==AMIN  ||
	     pop->i==AMAX  ||
	     pop->i==ACPAIR
	    )){
		MadelineErrorReport("EvaluateAggregateFunction():\n\tSorry! This aggregate function is not yet supported");
	}
	/*                                                               */
	/* Assign individual: if a male, we have to loop through all his */
	/* spouses instead of him:                                       */
	/*                                                               */
	p=c->cp;
	
	/*                                     */
	/* return missing if without children: */
	/*                                     */
	if(!p->nk){
		result->t=NUMERICVALUE;
		result->v=DOUBLEMISSINGVALUE;
		/*                                */
		/* Still have to gobble tokens:   */
		/*                                */
		/* Is this the right one to call? */
		/* EvaluateFunctionCallOrArray(&temp);   */
		EvaluateParenthesizedExpression(&temp);
		
		if(temp.t!=NUMERICVALUE)
			MadelineErrorReport("EvaluateAggregateFunction(): numeric expression expected");		
		return;
	}
	
	/*                                          */
	/* Save the position of the starting token: */
	/*                                          */
	StartingToken=pT;
	/*                         */
	/* save the repeat status: */
	/*                         */
	RepeatStatus=RepeatInProgress;
		
	/*                                                   */
	/* Loop through children, calculating the expression */
	/* for each child:                                   */
	/*                                                   */
	if(pop->i==ASTDV || pop->i==AVAR){
		/*                                         */
		/* allocate vector for remembering values: */
		/*                                         */
		vv=(double *)malloc((unsigned)sizeof(double)*p->nk);
		if(!vv) ErrorReport("EvaluateAggregateFunction(): Insufficient memory for vv vector.");
		for(i=0;i<p->nk;i++) vv[i]=0.0;
	}
	
	/*                                                                               */
	/* if male  , the outer "j" loop runs through female mates of the male:          */
	/* if female, the outer "j" loop executes just once for the mom herself          */
	/*                                                                               */
	/* The following code would look nicer if the inner "i" loop were segregated out */
	/* into a separate function call, but I got lazy when modifying the original     */
	/* code to handle males as well as females (The original code only allowed       */
	/* females with children).  Avoiding an extra function call here might actually  */
	/* result in faster code in the end analysis anyway:                             */
	/*                                                                               */
	female=(p->female==1);
	for(j=0;j<(female?1:p->na);j++){
		
		if(female) mom=p;        /* p is already the mom with children    */
		else	   mom=p->a[j];  /* run through each female mate of the male */
		
		/*                                                                */
		/* it's possible, although unlikely, that a female mate of a male */
		/* doesn't have kids:                                             */
		/*                                                                */
		if(!mom->nk) continue;
		
		for(ss=0.0,ct=cf=cm=cc=i=0;i<mom->nk;i++){
			/*                                                           */
			/* save pointer to current child in global CONTROL structure */
			/* so that it will be accessible to the parser:              */
			/*                                                           */
			c->cp=mom->k[i];
			/*                                             */
			/* save database record of child globally too: */
			/*                                             */
			if(mom->k[i]->db){
				GoTo(mom->k[i]->db,mom->k[i]->rci);
			}else{
				c->db->cr=NO_RECORD_EXISTS;
			}
			/*                                                    */
			/* recall the starting token position: remember pT is */
			/* global, so this is where                           */
			/* EvaluateParenthesizedExpression() starts:          */
			/*                                                    */
			pT=StartingToken;
			/*                                         */
			/* Evaluate expression for this offspring: */
			/*                                         */
			/* 980915 ET change:                       */
			/*                                         */
			EvaluateParenthesizedExpression(&temp);
			
			/*                           */
			/* Check validity of result: */
			/*                           */
			if(temp.t!=NUMERICVALUE){
				if(vv) free(vv);
				MadelineErrorReport("EvaluateAggregateFunction(): numeric expression expected");
			}
			
			/*                        */
			/* calculate running sum: */
			/*                        */
			if(temp.v!=DOUBLEMISSINGVALUE){
				/*                                      */
				/* for standard deviation or variance,  */
				/*                                      */
				if(pop->i==ASTDV || pop->i==AVAR) vv[cc]=temp.v;
				/*                        */
				/* running sum and count: */
				/*                        */
				ss+=temp.v;
				cc++;
				/*                   */
				/* count true/false: */
				/*                   */
				if(temp.v) ct++;
				else       cf++;
				/*                   */
				/* Track min and max */
				if(temp.v<min) min=temp.v;
				if(temp.v>max) max=temp.v;
				
			}else{
				/*                */
				/* count missing: */
				/*                */
				cm++;
			}
			
			/*                                   */
			/* Force RepeatInProgress to be TRUE */
			/* after the first loop              */
			/*                                   */
			if(!RepeatInProgress) RepeatInProgress=TRUE;

			/*                        */
			/* end of inner "i" loop: */
			/*                        */
		}
	
		/*                        */
		/* end of outer "j" loop: */
		/*                        */
	}
	
	
	/*                         */
	/* set result return type: */
	/*                         */
	result->t=NUMERICVALUE;
	/*                                   */
	/* finally, set result return value: */
	/*                                   */
	switch(pop->i){
	case AMEAN:
		if(!cc) result->v=DOUBLEMISSINGVALUE;
		else    result->v=ss/cc;
		break;
	case ASUM:
		if(!cc) result->v=DOUBLEMISSINGVALUE;
		else    result->v=ss;
		break;
	case ACOUNT:
		result->v=(double)cc;
		break;
	case ACTRUE:
		result->v=(double)ct;
		break;
	case ACPAIR:
		result->v=(double) PairCount(ct);
		break;
	case ACFALS:
		result->v=(double)cf;
		break;
	case ACMISS:
		result->v=(double)cm;
		break;
	case AMIN:
		result->v=min;
		break;
	case AMAX:
		result->v=max;
		break;
	case ASTDV:
	case AVAR:
		/*                    */
		/* need at least two: */
		/*                    */
		if(cc<=1){
			result->v=DOUBLEMISSINGVALUE;
			break;
		}
		/*       */
		/* mean: */
		/*       */
		ss/=cc;
		/*                            */
		/* sum of deviations squared: */
		/*                            */
		for(svar=0.0,i=0;i<cc;i++){
			dev=ss-vv[i];
			svar+=(dev*dev);
		}
		/*                */
		/* by definition: */
		/*                */
		svar/=(cc-1);
		if(pop->i==ASTDV)
			svar=sqrt(svar);
		/*              */
		/* fill result: */
		/*              */
		result->v=svar;
		/*          */
		/* free vv: */
		/*          */
		free(vv);
		break;
	}
	/*                                            */
	/* restore parental pointer and record index: */
	/*                                            */
	c->cp=p;
	if(p->db) GoTo(p->db,p->rci);
	else      c->db->cr=NO_RECORD_EXISTS;
	
	/*                                 */
	/* restore previous repeat status: */
	/*                                 */
	RepeatInProgress=RepeatStatus;
	/*                                                      */
	/* No need to fix up pT:                                */
	/* it should be at the right spot, so we're outta here: */
	/*                                                      */
	return;
	
}


/*                                                                         */
/*                                                                         */
/* EvaluateIndividualArrayReference().  This function is also part of      */
/* the expression parser.  An INDIVIDUAL in Madeline maintains an array of */
/* pointers to mates, and, if female, an array of pointers to offspring.   */
/* (For males, you have to access the children through the female mate(s). */
/*                                                                         */
/* This function evaluates these array references. A typical command on    */
/* the command line might be:                                              */
/*                                                                         */
/* M>show _o[1].studyage                                                   */
/*                                                                         */
/* ...this would print the study age of the individual's second offspring  */
/* (arrays are zero-offset in Madeline).                                   */
/*                                                                         */
void EvaluateIndividualArrayReference(PCONTROL c,PTOKENINFO pop,PRESULT result){

	PINDIVIDUAL  p;
	PINDIVIDUAL cp;
	int index;
	
	if(result->t!=NUMERICVALUE)
		MadelineErrorReport("EvaluateIndividualArrayReference(): a numeric subscript is required.");
	/* From the user's perspective, array indices are 1-offset instead of 0-offset: */
	index=(int)result->v;
	if(index<1)
		MadelineErrorReport("EvaluateIndividualArrayReference():\nNumeric subscript must be greater than or equal to 1.");
	index--;
	/*                                                                                                                  */
	/* for debugging:                                                                                                   */
	/*                                                                                                                  */
	/*if(!c->cp)                                                                                                        */
	/*	MadelineErrorReport("EvaluateIndividualArrayReference():\n\tc->cp not mapped! --Probably no pedigree table is open."); */
	
	/*                              */
	/* Now permit c->cp to be NULL: */
	/*                              */

	/*                                                                 */
	/* pop refers to the operand, in this case a reference to an array */
	/* internal to an INDIVIDUAL structure:                            */
	/*		                                                               */
	switch(pop->i){
	
	case IA:
	
		/*                                                                     */
		/* dereference INDIVIDUAL's mate vector:                               */
		/*                                                                     */
		/* c->cp is the current INDIVIDUAL:                                    */
		/*                                                                     */
		if(!c->cp || index>=c->cp->na) p=NULL;            /* can't dereference */
		else                 p=c->cp->a[index]; /* OK ... got a pointer        */

		break;
	
	case IK:
	
		/*                                                                                       */
		/* dereference INDIVIDUAL's offspring vector:                                            */
		/*                                                                                       */
		/* c->cp is the current INDIVIDUAL:                                                      */
		/*                                                                                       */
		/* Note that we can't dereference if no child, and we                                    */
		/* can't dereference on a male either:                                                   */
		/*                                                                                       */
		if(!c->cp || index>=c->cp->nk || !c->cp->female) p=NULL;            /* can't dereference */
		else                                   p=c->cp->k[index]; /* OK...got a pointer          */

		break;
		
	default:
	
		MadelineErrorReport("EvaluateIndividualArrayReference(): unrecognized vector");

		break;

	}

	/*                                            */
	/* Should now already be on the "." operator: */
	/* Check to make sure it's there:             */
	/*                                            */
	if(pT->i!='.')
		MadelineErrorReport("EvaluateIndividualArrayReference(): \".\" expected");
	/*                                                                       */
	/* Get the next token which should be a field or internal variable name: */
	/*                                                                       */
	GetToken();
	
	/*                       */
	/* process for person p: */
	/*	                      */
	if(p){
		/*                                                                  */
		/* we have a valid pointer for dereferencing:                       */
		/*                                                                  */
		/* -- store record number of c->cp                                  */
		/* -- go to the record, if its in the database:                     */
		/* -- evaluate the value of the field variable or internal variable */
		/*                                                                  */
		
		/*                               */
		/* save current c->cp reference: */
		/*                               */
		cp =c->cp;
	
		/*                                   */
		/* place new p into global position: */
		/*                                   */
		c->cp=p;
		/*                                        */
		/* place new current record into position */
		/*	                                       */
		if(p->db) GoTo(p->db,p->rci);			
		else      c->db->cr=NO_RECORD_EXISTS;		
		/*                                                       */
		/* Only a variable should be allowed, no direct value,   */
		/* but we have no way of checking for this with the      */
		/* current code in EvaluateValueOrVariable().  However,  */
		/* we can check for it below for the cases where         */
		/* p is NULL, so that's good enough:                     */
		/*                                                       */
		EvaluateFunctionCallOrArray(result);
		
		/*                                   */
		/* restore previous globals from cp: */
		/*                                   */
		c->cp=cp;
		if(cp->db) GoTo(cp->db,cp->rci);
		else       c->db->cr=NO_RECORD_EXISTS;

	}else{
		/*                                                    */
		/* can't dereference: return correct form of MISSING: */
		/*                                                    */
		
		/*                                                         */
		/* Check the token type of the variable in order to decide */
		/* which type to return:                                   */
		/*                                                         */
		switch(pT->t){
		case DATEVARIABLE:

			result->t=DATEVALUE;
			result->v=MISSINGDATE;
			GetToken();
			break;
			
		case NUMERICVARIABLE:
		case INDIVIDUALVARIABLE:

			result->t=NUMERICVALUE;
			result->v=DOUBLEMISSINGVALUE;
			GetToken();
			break;

		case STRINGVARIABLE:
		case INDIVIDUALSTRING:
		
			result->t=STRINGVALUE;
			result->s[0]=CHARMISSINGVALUE;
			result->s[1]='\0';
			GetToken();
			break;
			
		case INDIVIDUALPOINTER:
		case INDIVIDUALARRAY:
			/*                                         */
			/* Keep going:                             */
			/*                                         */
			/*EvaluateParenthesizedExpression(result); */
			/*                                         */
			EvaluateFunctionCallOrArray(result);
			break;

		case AFUNCTION:
			/*                      */
			/* Get the parentheses: */
			/*                      */
			GetToken();
			/*                                                   */
			/* Call EvaluateParenthesizedExpression()            */
			/* in order to put the remaining tokens onto the     */
			/* token stack -- not doing so can cause an error    */
			/* when repeating for a later individual: result is  */
			/* passed to hold the result, but we then dump the   */
			/* answer:                                           */
			/*                                                   */
			EvaluateParenthesizedExpression(result);
			/*                                                      */
			/* We already knew, without calling                     */
			/* EvaluateParenthesizedExpression, that this case must */
			/* evaluate to missing:                                 */
			/*                                                      */
			result->t=NUMERICVALUE;
			result->v=DOUBLEMISSINGVALUE;
						
			break;
						
		default:
		
			MadelineErrorReport("EvaluateIndividualArrayReference():\n\tnumeric, date or string variable expected");
			break;
		
		}

		/*                                                 */
		/* Finally, get the next token after the variable: */
		/*                                                 */
		/* GetToken();                                     */
		/*                                                 */
	}
	return;
}


/*                                                             */
/*                                                             */
/* EvaluateUnaryOperator():                                    */
/*                                                             */
/* Part of the expression parser.  Handles the unary negation  */
/* and logical NOT operators.                                  */
/*                                                             */
void EvaluateUnaryOperator(PRESULT result){

	register unsigned short op;
	
	op=0;
	
	if(pT->t==OPERATOR && (pT->i=='+' || pT->i=='-' || pT->i=='!')){
		op=pT->i;
		GetToken();
	}
	EvaluateFunctionCallOrArray(result);

	/* error checking: */
	if((op=='-' || op=='!') && result->t!=NUMERICVALUE)
		MadelineErrorReport("EvaluateUnaryOperator(): Numeric value required");
	
	/* handle MISSING: */
	if(result->v==DOUBLEMISSINGVALUE) return;

	if     (op=='-') result->v *= -1.0;            /* unary negation     */
	else if(op=='!') result->v= result->v?0.0:1.0; /* unary NOT operator */
}


/*                                                           */
/*                                                           */
/* EvaluateExponentiation().  Part of the expression parser. */
/* Handles exponentiation.                                   */
/*                                                           */
/*                                                           */
void EvaluateExponentiation(PRESULT result){

	RESULT temp;
	
	EvaluateUnaryOperator(result);  
	if(pT->i=='^'){
		GetToken();
		EvaluateExponentiation(&temp);

		/* error checking: */
		if(result->t!=NUMERICVALUE || temp.t!=NUMERICVALUE)
			MadelineErrorReport("EvaluateExponentiation(): Numeric values required");

		/* handle MISSING: */
		if(result->v==DOUBLEMISSINGVALUE) return;
		if(temp.v   ==DOUBLEMISSINGVALUE){
			result->v=DOUBLEMISSINGVALUE;
			return;
		}
		
		if(temp.t==NUMERICVALUE && temp.v==0.0){
			result->v=1.0;
			return;
		}
				
		result->v=pow(result->v,temp.v);
	}
}


/*                                                    */
/* EvaluateFactors().  Part of the expression parser. */
/* Handles multiplication and division.               */
/*                                                    */
void EvaluateFactors(PRESULT result){

	register unsigned short op;
	RESULT temp;
	
	EvaluateExponentiation(result);
	while((op=pT->i)=='*' || op=='/'){

		GetToken();
		EvaluateExponentiation(&temp);
		
		/* error checking: */
		if(result->t!=NUMERICVALUE || temp.t!=NUMERICVALUE)
			MadelineErrorReport("EvaluateFactors(): Numeric Values required");

		/*                 */
		/* handle MISSING: */
		/*                 */
		if(result->v==DOUBLEMISSINGVALUE ||
		   temp.v   ==DOUBLEMISSINGVALUE
		){
		
			result->v=DOUBLEMISSINGVALUE;
			
		}else if(op=='*'){
		
			result->v *= temp.v;
			
		}else{
			if(temp.v==0.0)
				MadelineErrorReport("EvaluateFactors(): Division by zero is not permitted.");

			result->v /= temp.v;			
		}

	}
}


/*                                                                    */
/* RoundToWhole(): Handles the Round() function which rounds a number */
/* to a whole number.                                                 */
/*                                                                    */
double RoundToWhole(double v){

	double ip;
	double fr;

	if(v==DOUBLEMISSINGVALUE) return v;
		
	fr=modf(v,&ip);
	
	if(v>=0){
		if(fr>=0.5)	ip++;
	}else{
		if(fr<=-0.5) ip--;
	}
	
	return ip;
}




/*                                                                  */
/* EvaluateTerms(): Part of the expression parser, EvaluateTerms()  */
/* handles addition and subtraction.  Note that addition is defined */
/* for both strings and dates, and subtraction for dates too:       */
/*                                                                  */
void EvaluateTerms(PRESULT result){
	
	register unsigned short op;
	RESULT temp;
	
	EvaluateFactors(result);
	while((op=pT->i)=='+' || op=='-'){
		
		GetToken();
		EvaluateFactors(&temp);
		/*                                                             */
		/* error checking: terms must be of the same type, except      */
		/* in the case of a number added/subtracted to a date value or */
		/*                                                             */

		if(op=='+'){
			/*                                                      */
			/* addition operator supported for STRING, NUMERIC, and */
			/* DATE values:                                         */
			/*                                                      */
			switch(result->t){
			case STRINGVALUE:
				if(temp.t!=result->t)
					MadelineErrorReport("EvaluateTerms(): String term expected.");
				strcat(result->s,temp.s);
				break;
			case NUMERICVALUE:
				if(temp.t!=result->t)
					MadelineErrorReport("EvaluateTerms(): Numeric term expected.");
				/*                 */
				/* handle MISSING: */
				/*                 */
				if(result->v==DOUBLEMISSINGVALUE || temp.v==DOUBLEMISSINGVALUE)
					result->v=DOUBLEMISSINGVALUE;					
				else				
					result->v += temp.v;
				break;
			case DATEVALUE:
				/*                                          */
				/* adding a number of days to a date value: */
				/* return value remains a DATEVALUE:        */
				/*                                          */
				if(temp.t!=NUMERICVALUE)
					MadelineErrorReport("EvaluateTerms(): Numeric term expected.");
				/*                 */
				/* handle MISSING: */
				/*                 */
				if(result->v==MISSINGDATE || temp.v==DOUBLEMISSINGVALUE)
					result->v=MISSINGDATE;
				else
					result->v+=RoundToWhole(temp.v);
				break;
			}
		}else{
			/*                                                    */
			/* subtraction supported for NUMERIC and DATE values: */
			/*                                                    */
			switch(result->t){
			case STRINGVALUE:
				MadelineErrorReport("EvaluateTerms(): Subtraction operator cannot be used with strings.");
				break;
			case NUMERICVALUE:
				if(temp.t!=result->t)
					MadelineErrorReport("EvaluateTerms(): Numeric term expected.");
			
				/*                 */
				/* handle MISSING: */
				/*                 */
				if(result->v==DOUBLEMISSINGVALUE || temp.v==DOUBLEMISSINGVALUE)
					result->v=DOUBLEMISSINGVALUE;
				
				else			
					result->v -= temp.v;
				break;
			case DATEVALUE:
				if(!(temp.t==NUMERICVALUE || temp.t==DATEVALUE))
					MadelineErrorReport("EvaluateTerms(): Numeric or date term expected.");
					
				if(temp.t==NUMERICVALUE){
					/*                                       */
					/* subtract a numeric value from a date: */
					/*                                       */
					if(result->v==MISSINGDATE || temp.v==DOUBLEMISSINGVALUE)
						result->v=MISSINGDATE;
					else
						result->v -= RoundToWhole(temp.v);
				}else{
					/*                                                                   */
					/* subtract one date from another date, returning the number of days */
					/* different between them:                                           */
					/*                                                                   */
					result->t=NUMERICVALUE;
					
					if(result->v==MISSINGDATE || temp.v==MISSINGDATE)
						result->v=DOUBLEMISSINGVALUE;
					else
						result->v -= temp.v;
				}
				break;
			}
		}
	}
}


/*                                                                              */
/*                                                                              */
/* EvaluateComparators().  Part of the expression parser, EvaluateComparators() */
/* handles less than, greater than, equality, etc:                              */
/*                                                                              */
/*                                                                              */
void EvaluateComparators(PRESULT result){

	register unsigned short op;
	RESULT temp;
	int differ;
	
	EvaluateTerms(result);
	if((op=pT->i)=='=' || op=='<' || op=='>'
		|| op==NE || op==LE || op==GE){
	
		GetToken();
		EvaluateTerms(&temp);
		
		/* error checking: */
		if(result->t!=temp.t)
			MadelineErrorReport("EvaluateComparators(): Only values of the same type can be compared.");
		
		/*           */
		/* Equality: */
		/*           */
		if(op=='='){
			switch(result->t){
			case STRINGVALUE:
				result->t=NUMERICVALUE;
				result->v=!strcmp(result->s,temp.s);
				break;

			case NUMERICVALUE:
				/*                                                        */
				/* Note that values CAN be compared to missing:           */
				/*                                                        */
				/* If one of the values is the missing value indicator,   */
				/* then we compare exactly.                               */
				/*                                                        */
				/* Otherwise, we count miniscule differences as not truly */
				/* different for our purposes:                            */
				/*                                                        */
				if(result->v==DOUBLEMISSINGVALUE || temp.v==DOUBLEMISSINGVALUE)
					result->v=(result->v==temp.v);
				else
					result->v=(fabs(result->v-temp.v)<=MINIMUMDIFFERENCE);
				break;
				
			case DATEVALUE:
				/*                                                           */
				/* Date values can also be compared to MISSINGDATE:          */
				/* DATEVALUES should always be integral, so exact comparison */
				/* is all we need:                                           */
				/*                                                           */
				result->t=NUMERICVALUE;
				result->v=(result->v==temp.v);
				break;
				
			}
		}else if(op==NE){
			/*                                          */
			/* Once again, comparison to MISSING is OK: */
			/*                                          */
			switch(result->t){
			case STRINGVALUE:
				result->t=NUMERICVALUE;
				result->v=strcmp(result->s,temp.s);
				break;
				
			case NUMERICVALUE:
				if(result->v==DOUBLEMISSINGVALUE || temp.v==DOUBLEMISSINGVALUE){
					result->v=(result->v!=temp.v);
				}else{
					result->v=(fabs(result->v-temp.v)>MINIMUMDIFFERENCE);				
				}
				break;
				
			case DATEVALUE:
				/*                                                    */
				/* all DATEVALUES are supposed to be integral values, */
				/* so we can do exact comparisons:                    */
				/*                                                    */
				result->t=NUMERICVALUE;
				result->v=(result->v!=temp.v);
				break;
			}
		}else{
			/*                                                   */
			/* remaining inequality comparators: < , <= , > , >= */
			/*                                                   */
			
			/*                                 */
			/* Handle NUMERIC and DATE values: */
			/*                                 */
			
			if(!(result->t==NUMERICVALUE || result->t==DATEVALUE))
				MadelineErrorReport("EvaluateComparators(): Numeric or date values required for comparison");

			switch(result->t){
			case NUMERICVALUE:
				/*                                    */
				/* comparison to MISSING not allowed: */
				/*                                    */
				if(result->v==DOUBLEMISSINGVALUE || temp.v==DOUBLEMISSINGVALUE){
					result->v=DOUBLEMISSINGVALUE;
				}else{				
					/*                                                            */
					/* are the numbers different at MINIMUMDIFFERENCE resolution? */
					/*                                                            */
					differ=fabs(result->v-temp.v)>MINIMUMDIFFERENCE;
					
					switch(op){
					case '<':             
						result->v=(differ && result->v<temp.v);
						break;
					case '>':
						result->v=(differ && result->v>temp.v);
						break;
					case LE:
						result->v=(!differ || (differ && result->v<temp.v));
						break;
					case GE:
						result->v=(!differ || (differ && result->v>temp.v));
						break;
					}
				}
				break;
					
			case DATEVALUE:
			
				/*                        */
				/* use exact comparisons: */
				/*                        */

				/*                                    */
				/* comparison to MISSING not allowed: */
				/*                                    */
				result->t=NUMERICVALUE;
				
				if(result->v==MISSINGDATE || temp.v==MISSINGDATE){
					result->v=DOUBLEMISSINGVALUE;
				}else{						
					switch(op){
					case '<':             
						result->v=(result->v <  temp.v);
						break;
					case '>':
						result->v=(result->v >  temp.v);
						break;
					case LE:
						result->v=(result->v <= temp.v);
						break;
					case GE:
						result->v=(result->v >= temp.v);
						break;
					}
				}
			
				break;
			}
		}
	}
}


/*                                                                      */
/*                                                                      */
/* EvaluateJoiners():  Part of the expression parser, EvaluateJoiners() */
/* handles the logical operators AND and OR:                            */
/*                                                                      */
void EvaluateJoiners(PRESULT result){

	register unsigned short op;
	RESULT temp;
	
	EvaluateComparators(result);
	while((op=pT->i)==AND || op==OR || op==XOR){
		GetToken();
		EvaluateComparators(&temp);
		
		
		/* error checking: */
		if(result->t!=NUMERICVALUE || temp.t!=NUMERICVALUE)
			MadelineErrorReport("EvaluateJoiners(): Numeric values required");
		

		/*                 */
		/* handle MISSING: */
		/*                 */
		if(result->v==DOUBLEMISSINGVALUE || temp.v==DOUBLEMISSINGVALUE){
			
			if(op==AND && (result->v==0 || temp.v==0)){
				/* For AND, if one of the two operands is FALSE,   */
				/* then the answer is FALSE according to the truth */
				/* table:                                          */
				result->v=0;
				
			}else if(op==OR && ( 
				            (result->v && result->v!=DOUBLEMISSINGVALUE) || 
				            (temp.v && temp.v!=DOUBLEMISSINGVALUE)
				           ) 
			){
				/* Here, we have to prove that one of the operands is not */
				/* zero and is also not MISSING                           */
				/* for OR, if one of the two operands is TRUE,            */
				/* then the answer is TRUE according to the truth         */
				/* table:                                                 */
				result->v=1;
			}else{
				/* This case covers the cases where AND and OR must      */
				/* return MISSING, and also the case of XOR which always */
				/* returns MISSING if one of the operands is MISSING:    */
				result->v=DOUBLEMISSINGVALUE;
				
			}
		}else if(op==AND){
			/* AND CASE */
			result->v=(result->v && temp.v);
			
		}else if(op==XOR){
			/* XOR CASE */
			result->v=((result->v && !temp.v) || (!result->v && temp.v)); 
		}else{
			/* OR CASE */
			result->v=(result->v || temp.v);
			
		}
	}
}


/*                                                                 */
/*                                                                 */
/* EvaluateExpression(): This is the entry point into the          */
/* expression parser.                                              */
/*                                                                 */
/* Regardless of whether repeating evaluation of an old expression */
/* or starting a new expression, pT must be reset to point one     */
/* before the starting address of Token.                           */
/*                                                                 */
/* The parser is informed of a repeat if the first character of    */
/* e is the REPEATSIGNAL, in which case RepeatInProgress is set to */
/* true (this is only used in debugging -- in a working version,   */
/* RepeatInProgress is set directly by the code when needed).      */
/*                                                                 */
/* If this is a new expression then the Token stack                */
/* must be cleared.                                                */
/*                                                                 */
void EvaluateExpression(PRESULT result){

	extern PTOKENINFO Token;

	/*                                              */
	/* set estt for benefit of MadelineErrorReport: */
	/*                                              */
	estt=e;

	/*                                                    */
	/* check that stack has been initialized (allocated): */
	/*                                                    */
	if(!Token) Token=NewTokenStack(TokenStackSize);
	/*                                                              */
	/* Point token stack at the beginning (MUST be one before Token */
	/* because GetToken() starts by pushing pT                      */
	/*                                                              */
	pT=Token;
	
	/*                                                     */
	/* clear the token stack if starting a new expression: */
	/*                                                     */
	if(!RepeatInProgress) ClearTokenStack(Token,TokenStackSize);        

	GetToken();     /* get first token                                                   */
        /*                                                                            */
	/* Allow empty strings:                                                              */
	/* if(!pT->e[0]) MadelineErrorReport("EvaluateExpression(): No expression present"); */
	EvaluateJoiners(result); /* start into recursive descent parser                      */
}



/*                                                                 */
/*                                                                 */
/* EvaluateExpressionOnStack2(): This is the entry point into the  */
/* expression parser, identical to EvaluateExpression(), but uses  */
/* Token2, the reserve token stack, instead of the current token   */
/* stack                                                           */
/*                                                                 */
/* Regardless of whether repeating evaluation of an old expression */
/* or starting a new expression, pT must be reset to point one     */
/* before the starting address of Token.                           */
/*                                                                 */
/* If this is a new expression then the Token stack                */
/* must be cleared.                                                */
/*                                                                 */
void EvaluateExpressionOnStack2(PRESULT result){

	extern PTOKENINFO Token2;

	/*                                              */
	/* set estt for benefit of MadelineErrorReport: */
	/*                                              */
	estt=e;

	/*                                                    */
	/* check that stack has been initialized (allocated): */
	/*                                                    */
	if(!Token2) Token2=NewTokenStack(TokenStack2Size);
	/*                                                              */
	/* Point token stack at the beginning (MUST be one before Token */
	/* because GetToken() starts by "pushing" pT:                   */
	/*                                                              */
	pT=Token2-1;
	
	/*                                                     */
	/* clear the token stack if starting a new expression: */
	/*                                                     */
	if(!RepeatInProgress) ClearTokenStack(Token2,TokenStack2Size);        

	GetToken();     /* get first token */
	if(!pT->e[0]) MadelineErrorReport("EvaluateExpression(): No expression present");
	EvaluateJoiners(result); /* start into recursive descent parser */
}


/* EvaluateMultipleExpressions  */
void EvaluateMultipleExpressions(int StopToken,int PrintFlag){

	extern PTOKENINFO Token2;
	extern int TokenStack2Size;
	extern char *estt, *e;
	extern PCONTROL g;
	
	RESULT result;
	/*                                              */
	/* set estt for benefit of MadelineErrorReport: */
	/*                                              */
	estt=e;

	/*                                                    */
	/* check that stack has been initialized (allocated): */
	/*                                                    */
	if(!Token2) Token2=NewTokenStack(TokenStack2Size);
	/*                                                              */
	/* Point token stack at the beginning (MUST be one before Token */
	/* because GetToken() starts by "pushing" pT:                   */
	/*                                                              */
	pT=Token2-1;
	
	/*                                                     */
	/* clear the token stack if starting a new expression: */
	/*                                                     */
	if(!RepeatInProgress) ClearTokenStack(Token2,TokenStack2Size);        

	GetToken();     /* get first token */
	
	if(!pT->e[0]) MadelineErrorReport("EvaluateExpression(): No expression present");
	
	while(!(pT->i==END || pT->i==StopToken)){
		
		EvaluateJoiners(&result); /* start into recursive descent parser */
		
		if(pT->i==',') GetToken();
		
		if(PrintFlag){
			
			switch(result.t){
			case STRINGVALUE:
			
				if(result.s[0]=='\0') dprintf(".%c",g->delimiter); 
				else dprintf("%s%c",result.s,g->delimiter);
				break;
			
			case DATEVALUE:
			
				dprintf("%s%c",DelimitedGenericDateString((long int)result.v,'-'),g->delimiter);
				break;
			
			case NUMERICVALUE:
			
				if(result.v==DOUBLEMISSINGVALUE) dprintf(".%c",g->delimiter);
				else dprintf("%g%c",result.v,g->delimiter);
				break;
			}
			
		}
		
	}
	dprintf("\n");
}



/*                                                                   */
/* Interpret() is part of the recursive descent parser.  Interpret() */
/* calls a command or performs an assignment:                        */
/*                                                                   */
void Interpret(char *expr){

	extern char *e; /* global pointer to expression */
	extern PTOKENINFO Token;
	extern int TokenStackSize;
	extern int gScanning;
	
	int tt;         /* token type */

	/*                                      */
	/* check that stack has been allocated: */
	/*                                      */
	if(!Token) Token=NewTokenStack(TokenStackSize);

	estt=e=expr;    /* point global expression pointer */
	
	/* */
	/* */
	/* */
	do{
		pT=Token;     /* put pT in right position prior to GetToken() call */
		tt=GetToken();
		
		if(gScanning){
			
			/* Execute only the           */
			/* program control functions: */
			if(pT->i==FORK || pT->i==CTRLC) pT->u.pvf();
			
		}else{
			/* Execute normally: */
			if(tt==COMMAND){
				/* call function of type void (*PVF)(void) */
				if(pT->i!=CTRLC) pT->u.pvf();
				
			}else if(tt==KEYWORD){
				MadelineErrorReport("Keyword \"%s\" must be used with a command",pT->e);
			}else{
				/*                                    */
				/* see if it is assignment statement: */
				/*                                    */
				Assignment();
			}
		}
	}while(pT->i!=END);
}


/*                                                        */
/* Show(): show the result of a calculation, or the value */
/*         of an internal variable or array by calling    */
/*         EvaluateExpression()                           */
/*                                                        */
void Show(void){

	RESULT result;
	
	extern PLANGUAGEOBJECT Language[LANGUAGE_COUNT];
	extern PCONTROL g;
	
	/*                                                                */
	/* Get references to individual just in case the                  */
	/* expression contains references to INDIVIDUAL member variables: */
	/*                                                                */
	GetReferencesToIndividual(g);
	
	/*                                                   */
	/* EvaluateExpression() starts off with a GetToken() */
	/* after clearing the token stack:                   */
	/*                                                   */
	EvaluateExpression(&result);  /* get the answer      */
	switch(result.t){
	case STRINGVALUE:

		lprintf("\"%s\"\n",result.s);
		break;
		
	case DATEVALUE:
	
		lprintf("{%s}\n",Language[g->lang]->PrintDate((long int)result.v));
		break;

	case NUMERICVALUE:	

		if(result.v==DOUBLEMISSINGVALUE)
			lprintf("#MISSING\n");
		else
			lprintf("%'12.10g\n",result.v);

		break;
	    
	}
}


/*                                                          */
/* WhatIs(): Show the result of a calculation, or the value */
/*           of an internal variable or array by calling    */
/*           EvaluateExpression().  This function is        */
/*           an alternate to the Show() command             */
/*                                                          */
void WhatIs(void){
	
	GetToken();
	if(pT->i!=ISC){
		MadelineErrorReport("WhatIs(): WHAT IS <expr>");
	}
	
	Show();
	
}


/*                                                                */
/* void GetReferencesToIndividual(PCONTROL c)                     */
/*                                                                */
/* In order to evaluate an expression containing references to an */
/* INDIVIDUAL, we need to find that individual in a pedigree and  */
/* individual tree.  This function takes care of this.            */
/*                                                                */
void GetReferencesToIndividual(PCONTROL c){

	char fid[MAXIDWIDTH];
	char iid[MAXIDWIDTH];

	/*                                            */
	/* Can't do a thing if no database is opened: */
	/*                                            */
	if(!c->db){
		c->cpd=NULL;
		c->cp =NULL;
		return;
	}
	
	if(c->db->cr==NO_RECORD_EXISTS){
		/*                                                            */
		/* Show and WhatIs() can only be used for looking at database */
		/* records, so go to a record:                                */
		/*                                                            */
		GoTo(c->db,0);
		lprintf("Madeline has reset record pointer to first record (record 0):\n");
	}
	/*                                 */
	/* Get here if database record OK: */
	/*                                 */
	
	/*                       */
	/* Read individual's ID: */
	/*                       */
	strcpy(iid,c->db->ReadString(c->db,NULL,gfnID.i,c->db->cr));
	/*                 */
	/* Read family ID: */
	/*                 */
	if(gfnFamID.i==MISSING){
		/*                         */
		/* FUSION type ID assumed: */
		/*                         */
		strncpy(fid,iid,4);
		fid[4]='\0';
	}else{
		/*                             */
		/* read fid from database too: */
		/*                             */
		strcpy(fid,c->db->ReadString(c->db,NULL,gfnFamID.i,c->db->cr));
	}
	/*                                                     */
	/* Lookup and store references to the individual in c: */
	/*                                                     */
	IndividualLookup2(c->pd,fid,iid,&c->cpd,&c->cp);
}




/*                          */
/*                          */
/* Open(): Open a database. */
/*                          */
/*                          */
/*                          */
void Open(void){

	RESULT result;
	const char *fileToOpen;
	
	clock_t stt,end;
	char h[25];
	int table_type;
	
	extern PCONTROL g;

	/*                                                */
	/* EvaluateExpression() starts with a GetToken()  */
	/* after clearing the token stack:                */
	/*                                                */
	EvaluateExpression(&result); /* get the file name */
	if(result.t==STRINGVALUE){
		
		/*                                            */
		/* SmartTableSelect figures out whether the   */
		/* user specified the .data or the .mfh file  */
		/* and checks the usability of the .mfh file. */
		/* If the .mfh does not exist, recognize() is */
		/* called to create it:                       */
		/*                                            */
		fileToOpen=SmartTableSelect(result.s);
		
		/*                                          */
		/* if a database is already open, close it: */
		/*                                          */
		if(g->db){
			
			/*                       */
			/* Close open database:  */
			/*                       */
			lprintf("Closing pedigree table \"%s\"...\n",g->db->nm);
			CloseDatabase(g->db);
			g->db=NULL;
			/*                       */
			/* Remove pedigree tree: */
			/*                       */
			lprintf("Removing old pedigrees...\n");
			FreeBinaryPedigreeTree(g->pd);
			g->pd=NULL;
			/*                         */
			/* Reset counters to zero: */
			/*                         */
			ResetCounters(g);
			/*                                                      */
			/* Remove pedigree drawing graphics set up, if present: */
			/*                                                      */
			if(gDrawing){
				FreeDrawing(gDrawing);
				gDrawing=NULL;
			}
			/*                                                     */
			/* Reset HasErrors flag, if prior database was faulty: */
			/*                                                     */
			ResetErrorCounters(g);
		}
				
		/*                                                       */
		/* open the database into g->db:                         */
		/*                                                       */
		/* This is done by first calling CharacterizeDatabase()  */
		/* to determine what type of database it is.             */
		/* Then, NewDatabase() instantiates a database object of */
		/* the correct type, file name, and open mode.           */
		/* Calling the OpenDatabase() member function for the    */
		/* object then opens and buffers the database:           */
		/*                                                       */
		table_type=CharacterizeDatabase(fileToOpen);	
		g->db=NewDatabase(fileToOpen,READONLY,table_type);
		g->db->OpenDatabase(g->db);

		/*                                                                 */
		/* Store the name of the new database file if opened successfully: */
		/*                                                                 */
		StoreString(&gDatabaseFile,fileToOpen);
		
		GoTo(g->db,0);    /* set pointer to the top */
		/*                                          */
		/* store field indexes for known fields:    */
		/*                                          */
		/*                                          */
		gfnFamID.i   =g->db->FieldNumber(g->db,gfnFamID.e.s   );
		gfnID.i      =g->db->FieldNumber(g->db,gfnID.e.s      );
		gfnFather.i  =g->db->FieldNumber(g->db,gfnFather.e.s  );
		gfnMother.i  =g->db->FieldNumber(g->db,gfnMother.e.s  );
		gfnSex.i     =g->db->FieldNumber(g->db,gfnSex.e.s     );
		gfnMZTwin.i  =g->db->FieldNumber(g->db,gfnMZTwin.e.s  );
		gfnDZTwin.i  =g->db->FieldNumber(g->db,gfnDZTwin.e.s  );
		gfnAffected.i=g->db->FieldNumber(g->db,gfnAffected.e.s);
		gfnProband.i =g->db->FieldNumber(g->db,gfnProband.e.s );
		gfnClass.i   =g->db->FieldNumber(g->db,gfnClass.e.s   );
		gfnLClass.i  =g->db->FieldNumber(g->db,gfnLClass.e.s  );
		gfnDead.i    =g->db->FieldNumber(g->db,gfnDead.e.s    );
		gfnDOB.i     =g->db->FieldNumber(g->db,gfnDOB.e.s     );
		gfnDOD.i     =g->db->FieldNumber(g->db,gfnDOD.e.s     );
		
		/*                                                      */
		/* Now set the field flags for the known set of fields: */
		/*                                                      */
		SetKnownFieldFieldFlags(g->db);
		/*                                          */
		/* Auto detect fields as being (G)enotype,  */
		/* or (P)henotype fields:                   */
		/*                                          */
		if(g->AutoDetect) SetFieldFlags(g->db);
		CountFieldFlags(g->db);

		/*                                                                */
		/* ReuseSavedMarkerVector() checks for compatability              */
		/* between the saved marker vector and the newly opened database: */
		/* If the saved can't be used, they are removed, and we have to   */
		/* calculate new frequencies.  It can, of course, also occur that */
		/* there are no saved frequencies at all:                         */
		/*
		
		//
		// OBSOLETE: THIS IF STATEMENT APPEARS OBSOLETE 2004.07.29.ET
		//                                                                */
		if(!ReuseSavedMarkerVector(g)){
			/*                                            */
			/* If there are GO fields, then calculate the */
			/* allele frequencies for them:               */
			/*                                            */
			CalculateAlleleFrequencies(g->db);
		}
			
		lprintf("Pedigree table \"%s\" opened with %s records\n",result.s,Itoa(h,g->db->rc,9));

		/*                                                   */
		/* Call function to read core information and create */
		/* binary pedigree tree:                             */
		/*                                                   */
		g->pd=ReadCoreInformation(g->pd,g->db);
				
		/*                                       */
		/* Call function to construct pedigrees: */
		/*                                       */
		/*                                       */
		
		/*                 */
		/* START STOPWATCH */
		/*                 */
		stt=clock();
		TraverseBinaryPedigreeTree(g->pd,ConstructPedigree);
		
		/*                                */
		/* Set the founding group vector: */
		/*                                */
		TraverseBinaryPedigreeTree(g->pd,SetFoundingGroups);
		
		/*                                                         */
		/* Call function to set pedigree "widths": This is part of */
		/* the preprocessing needed for pedigree drawing:          */
		/*                                                         */
		TraverseBinaryPedigreeTree(g->pd,CalculatePedigreeWidth);
		/*                                                                          */
		/* Call function to set pedigree vlevels: This is also part of              */
		/* the preprocessing needed for (as of 19990908, still future additions of) */
		/* the pedigree drawing code:                                               */
		/*                                                                          */
		TraverseBinaryPedigreeTree(g->pd,CalculatePedigreeVLevel);
			
		/*                             */
		/* auto set field order flags: */
		/*                             */
		AutoSetFieldOrdering(g,g->db);
		
		/*                                                            */
		/* Set the genotype counts based on all of the marker fields: */
		/*                                                            */
		SetGenotypeCountAttributes(g->pd);
		
		/*                                          */
		/* Calculate pedigree complexities as 2N-F: */
		/*                                          */
		CalculateComplexityOfPedigrees(g);
		
		/*                */
		/* STOP STOPWATCH */
		/*                */
		end=clock();
		lprintf("Pedigrees reconstructed in %.4lf seconds\n",(double)(end-stt)/(double)CLOCKS_PER_SEC);
		
		/*                                                             */
		/* Check inheritance, if the AutoCheckInheritance flag is set: */
		/*                                                             */
		if(g->AutoCheckInheritance){
			
			/* Clear the PMARKER inconsistency counters: */
			ClearMarkerInconsistencyCounters(g);
			
			lprintf("Checking simple Mendelian inheritance in nuclear families... :\n");
			lprintf("==============================================================\n");
			lprintf("Inheritance inconsistency:%cPEDIGREE%cMOTHER%cFATHER%cMARKER\n",
				g->delimiter,g->delimiter,g->delimiter,g->delimiter
			);
			lprintf("--------------------------%c--------%c------%c------%c------\n",
				g->delimiter,g->delimiter,g->delimiter,g->delimiter
			);
			TraverseBinaryPedigreeTree2(g,g->pd,CheckPedigreeInheritance);
			lprintf("==============================================================\n");
			
			PrintMarkerInconsistencySummary(g);
			
		}

		/*                    */
		/* Show field status: */
		/*		                  */
		ShowFieldStatusColumnwise(g->db,3,DisplayFieldInformation);

		/*                           */
		/* Show database statistics: */
		/*                           */
		ShowDatabaseStatistics(g);
	
	
	}else
		MadelineErrorReport("Open(): file name required");
}



/*                                                        */
/* System(): Call ANSI C's system() to run an OS command: */
/*                                                        */
void System(void){

	RESULT result;
	
	/*                                               */
	/* EvaluateExpression() starts with a GetToken() */
	/* after clearing the token stack:               */
	/*                                               */
	EvaluateExpression(&result); /* get the command  */
	if(result.t==STRINGVALUE){              
		if(system(result.s))
			MadelineErrorReport("System(): The operating system could not execute the command.");
	}else
		MadelineErrorReport("System(): a quoted command string to pass to the OS is required");
}



/*                                                                    */
/* SetKnownFieldFieldFlags(): Sets the field flags for the set of     */
/* internally recognized fields.  This should be called whenever      */
/* one of the known fields is reassigned.  It is called automatically */
/* whenever a new database is opened by Open().                       */
/*                                                                    */
/* The COREINSET and COREOUTSET flags are COMPLETELY OBSOLETE now     */
/* -19980622 ET                                                       */
/*                                                                    */
/* ICONSET has replaced COREINSET with a different meaning and usage  */
/* -19990324 ET                                                       */
/*                                                                    */
/* * KNOWNSET  means a CORE field.                                    */
/* * OUTPUTSET means the field is toggled on for output by default.   */
/*                                                                    */
void SetKnownFieldFieldFlags(PDBF db){
	
	/*                  */
	/* Family ID field: */
	/*                  */
	if(gfnFamID.i!=MISSING)
		db->ff[gfnFamID.i   ]=KNOWNSET|OUTPUTSET;
	/*                      */
	/* Individual ID field: */
	/*                      */
	if(gfnID.i!=MISSING)
		db->ff[gfnID.i      ]=KNOWNSET|OUTPUTSET;
	/*                  */
	/* Father ID field: */
	/*                  */
	if(gfnFather.i!=MISSING)
		db->ff[gfnFather.i  ]=KNOWNSET|OUTPUTSET;
	/*                  */
	/* Mother ID field: */
	/*                  */
	if(gfnMother.i!=MISSING)
		db->ff[gfnMother.i  ]=KNOWNSET|OUTPUTSET;
	/*                                     */
	/* Sex field (Now called GenderField): */
	/*                                     */
	if(gfnSex.i!=MISSING)
		db->ff[gfnSex.i     ]=KNOWNSET|OUTPUTSET;
	/*               */
	/* MZTWIN Field: */
	/*               */
	if(gfnMZTwin.i!=MISSING)
		db->ff[gfnMZTwin.i  ]=KNOWNSET|OUTPUTSET;

	/*               */
	/* DZTWIN Field: */
	/*               */
	if(gfnDZTwin.i!=MISSING)
		db->ff[gfnDZTwin.i  ]=KNOWNSET|OUTPUTSET;

	/*                                                       */
	/* Affected field: Known (i.e., "C" core field) and also */
	/* the default field for graphic display in the icon     */
	/*                                                       */
	if(gfnAffected.i!=MISSING)
		db->ff[gfnAffected.i]=KNOWNSET|ICONSET|OUTPUTSET;
	/*                */
	/* Proband field: */
	/*                */
	if(gfnProband.i!=MISSING)
		db->ff[gfnProband.i ]=KNOWNSET|OUTPUTSET;
	/*                                            */
	/* FUSION study CLASS field: THIS IS OBSOLETE */
	/*                                            */
	if(gfnClass.i!=MISSING)
		db->ff[gfnClass.i   ]=KNOWNSET|OUTPUTSET;
	/*                                                    */
	/* Liability Class (recognized for Genehunter format: */
	/*                                                    */
	if(gfnLClass.i!=MISSING)
		db->ff[gfnLClass.i  ]=KNOWNSET|OUTPUTSET;

	/*                   */
	/* DeathStatusField: */
	/*                   */
	if(gfnDead.i!=MISSING)
		db->ff[gfnDead.i  ]=KNOWNSET|OUTPUTSET;
		
	/*                                                                     */
	/* DateOfBirthField: Date fields are not set on for output by default: */
	/*                                                                     */
	if(gfnDOB.i!=MISSING)
		db->ff[gfnDOB.i  ]=KNOWNSET;
	/*                   */
	/* DateOfDeathField: */
	/*                   */
	if(gfnDOD.i!=MISSING)
		db->ff[gfnDOD.i  ]=KNOWNSET;
	
	/*                                                           */
	/* Since affection status is an ICONSET field, we need to    */
	/* call ManageFieldTrees() to handle field tree building and */
	/* reporting (from ndbf.c):                                  */
	/*                                                           */
	ManageFieldTrees(db);
	ManageFieldLevelsLabelsArrays(knownTokens,db);
}



/*                                       */
/* Go(): go to a record in the database. */
/*                                       */
void Go(void){

	extern PCONTROL g;
	
	RESULT result;
	
	if(!g->db) MadelineErrorReport("Toggle(): You must open a pedigree table before you can use this command.");
	
	/*                                                    */
	/* EvaluateExpression() starts with a GetToken()      */
	/* after clearing the token stack:                    */
	/*                                                    */
	EvaluateExpression(&result); /* get the record number */
	if(result.t==NUMERICVALUE){
		if(result.v<0 || result.v>=g->db->rc){
			MadelineErrorReport("Go(): No such record.  The first record is at position 0\nand the last record is at position %i.",
			g->db->rc-1
			);
		}
		GoTo(g->db,(int)result.v);
	}else
		MadelineErrorReport("Go(): numeric value required for record index");
}


/*                                                   */
/* ShowFieldStatusColumnwise(): Shows fields as well */
/* as field status flags in "columns" columns:       */
/*                                                   */
void ShowFieldStatusColumnwise(PDBF db,int columns,void (*showfunc)(PDBF db,int f)){

	int rows,remn,i,j,mm,f;

	rows=db->fc/columns;
	remn=db->fc%columns;
	
	/*                          */
	/* Print field information: */
	/*                          */
	for(i=0;i<rows+(remn?1:0);i++){
		for(mm=-1,j=0;j<(i==rows?remn:columns);j++){          
			if(mm<remn) mm++;
			f=i+j*rows+mm;
			
			showfunc(db,f);			
		}
		lprintf("\n");
	}
}


/*                                                               */
/* DisplayFieldInformation():  This is the show function to pass */
/* to ShowFieldStatusColumnWise():                               */
/*                                                               */
void DisplayFieldInformation(PDBF db,int f){

	lprintf("%3i.%-10.10s %c%c%s%c ",
                f+1,
                db->FieldName(db,f),
                db->ff[f] & KNOWNSET     ?'C':
                db->ff[f] & COVARIATESET ?'V':
                db->ff[f] & GENOTYPESET  ?'G':
                db->ff[f] & PHENOTYPESET ?'P':
                db->ff[f] & NULLCOLUMNSET?'*':' ',
                db->ff[f] & OUTPUTSET    ?'o':' ',
                UnderscoredNumber(db->fi->r[f],3),
                db->ff[f] & ICONSET      ?'+':' '
        );           
}


/*                                                             */
/*                                                             */
/* SetFieldFlags()                                             */
/*                                                             */
/* Set flags indicating field status for all fields except the */
/* known fields which are set by SetKnownFieldFieldFlags():    */
/*                                                             */
/* This function sets the the non-core fields as belonging to  */
/* either the GENOTYPESET or PHENOTYPESET:                     */
/*                                                             */
void SetFieldFlags(PDBF db){

	int j;
	
	for(j=0;j<db->fc;j++){		
		/*                                                     */
		/* skip the KNOWNSET set by SetKnownFieldFieldFlags(): */
		/*                                                     */
		if(db->ff[j] & KNOWNSET) continue;               
		/*                                */
		/* Is the field a genotype field? */
		/*                                */
		switch(IsAGenotypeField(db,j)){
			case TRUE:
				/*                         */
				/* It is a genotype field: */
				/*                         */
				db->ff[j] |= GENOTYPESET;
				db->ff[j] |= OUTPUTSET;
				break;
			case FALSE:
				/*                                */
				/* assumed to be phenotype field: */
				/*                                */
				db->ff[j] |= PHENOTYPESET;
				/*                                                */
				/* mark phenotype field for output if it is NOT a */
				/* DATE field:                                    */
				/*                                                */
				if(db->FieldType(db,j)!='D')
					db->ff[j] |= OUTPUTSET;
				break;
			case MISSING:
				/*                          */
				/* completely empty column: */
				/*                          */
				db->ff[j] |= NULLCOLUMNSET;
				/*                                  */
				/* turn off output flag if it's on: */
				/*                                  */
				if(db->ff[j] & OUTPUTSET) db->ff[j]^=OUTPUTSET;

				eprintf(WARNINGFLAG,"SetFieldFlags(): %i. %s contains no data!\n",j,db->FieldName(db,j));
				break;
		}
	}
}


/*                                                     */
/*                                                     */
/* List() command: display fields or internal arrays   */
/* or the internal chromosome marker map(s) in memory: */
/*                                                     */
void List(void){

	int i;
	RESULT result;
	
	extern PCONTROL g;

	/* get the next token: */
	
	GetToken();
	
	if(pT->i==FLDK){
		
		if(!g->db) MadelineErrorReport("List(): No pedigree table is open");
		ShowFieldStatusColumnwise(g->db,3,DisplayFieldInformation);
		
	}else if(pT->t==INTERNALARRAY || pT->t==USERARRAY){
		/*                                 */
		/* display array size information: */
		/*                                 */
		if(pT->t==USERARRAY){
			ConsoleBold(stdout);
			lprintf("%s",pT->p);
			ConsoleNormal(stdout);
			lprintf(" is a user array\n");
		}
		ConsoleBold(stdout);
		lprintf("%s",pT->p);
		ConsoleNormal(stdout);
		lprintf(" has %i elements:\n",pT->u.a->n);
		
		if(pT->u.a->t==STRINGVALUE){
			/*                                  */
			/* internal array of string values: */
			/*                                  */
			for(i=0;i<pT->u.a->n;i++){
				ConsoleBold(stdout);
				lprintf("%s",pT->p);
				ConsoleNormal(stdout);
				lprintf("[%2i]=\"%s\"\n",i+1,pT->u.a->e[i].s);
			}
		}else if(pT->u.a->t==NUMERICVALUE){
			/*                                   */
			/* internal array of numeric values: */
			/*                                   */
			for(i=0;i<pT->u.a->n;i++){
				ConsoleBold(stdout);
				lprintf("%s",pT->p);
				ConsoleNormal(stdout);
				if(pT->u.a->e[i].v==DOUBLEMISSINGVALUE)
					lprintf("[%2i]=#MISSING\n",i+1);
				else
					lprintf("[%2i]=%14.8g\n",i+1,pT->u.a->e[i].v);
			}
		}else{
			MadelineErrorReport("List(): unrecognized type of array");
		}
		
	}else if(pT->t==ASSOCIATIVEARRAY){
		
		ConsoleBold(stdout);
		lprintf("%s",pT->p);
		ConsoleNormal(stdout);
		lprintf(" has %i elements:\n",pT->u.aa->n);
		TraverseTree(pT->u.aa->h,ListKeyValuePair);
		
	}else if(pT->i==MAPK){
		/*                                                        */
		/* optional "FOR CHROMOSOME=" expression to limit output: */
		/*                                                        */
		GetToken();
		if(pT->i==FORK){
			GetToken();
			if(pT->i!=CHRMK)
				MadelineErrorReport("List(): CHROMOSOME keyword expected:\n\tLIST MAP FOR CHROMOSOME <n>");
				
			/* GetToken();                                                                         */
			/* if(pT->i!='=')                                                                      */
			/*	MadelineErrorReport("List(): \"=\" expected:\n\tLIST MAP FOR CHROMOSOME=<n>"); */
			/*                                                                                     */
			
			/*                                                         */
			/* use EvaluateExpression, which begins with a GetToken(), */
			/* to get the chromosome number:                           */
			/*                                                         */
			EvaluateExpression(&result);
			if(result.t!=NUMERICVALUE)
				MadelineErrorReport("List(): a numeric expression is expected for the chromosome number");
			if(result.v<1)
				MadelineErrorReport("List(): the chromosome number must be a positive number");
				
			/*                                             */
			/* ready to print for the specific Chromosome: */
			/*                                             */
			DisplayMarkerMapInformationHeader();
			TraverseMarkerMapByPositionForChromosome(g->mp,(int)result.v,DisplayMarkerMapInformation);
		}else{
			DisplayMarkerMapInformationHeader();
			TraverseMarkerMapByPosition(g->mp,DisplayMarkerMapInformation);
		}
		
	}else if(pT->i==STATK){
		
		ShowPedigreeStatistics(g);
		
	}else if(pT->i==ALLEK){
		
		ShowAlleleFrequencies(g);
		
	}else if(pT->i==FILEK){
		
		ListCurrentDirectory();
		
	}else{
		
		MadelineErrorReport("List(): object whose token value is \"%i\" is not recognized for this command",pT->i);
		
	}
	
}


/*                                                */
/*                                                */
/* Write(): write out data in a specified format: */
/*                                                */
/*                                                */
void Write(void){

	RESULT result;
	int KeywordSeen=0;

	void (*DataHeaderProcessor)(PCONTROL,char *fn);
	void (*OutputPedigree     )(PCONTROL,PPEDIGREE);
	void (*DataFooterProcessor)(PCONTROL);
	void (*ShowStatistics     )(PCONTROL);

	extern PCONTROL g;
	
	/*                            */
	/* a database MUST be opened: */
	/*                            */
	if(!g->db) MadelineErrorReport("Write(): no pedigree table is open");

	/*                                                                            */
	/* First keyword should be "TO", or else it can be "LOCUS" followed by "FILE" */
	/* or else it can be "PEDIGREE" followed by "FILE":                           */
	/*                                                                            */
	GetToken();
	
	if(pT->i==LOCUS){
		/*                                       */
		/* Transfer control to Locus() function: */
		/*                                       */
		Locus();
		return;
	}
	
	/*                                                                       */
	/* if we get PEDIGREE keyword, then after this must be the FILE keyword, */
	/* and then finally the TO keyword:                                      */
	/*                                                                       */
	if(pT->i==PEDIK){
		GetToken();
		if(pT->i!=FILEK)
			MadelineErrorReport("Write(): \"FILE\" keyword expected:\n\tWRITE PEDIGREE FILE TO \"filename\" IN xxx FORMAT");
		
		GetToken();
	}
	
	
	/*                                       */
	/* Get here if "TO" keyword is expected: */
	/*                                       */
	if(pT->i!=TOC)
		MadelineErrorReport("Write(): \"TO\" keyword expected:\n\tWRITE [PEDIGREE FILE] TO \"filename\" IN xxx FORMAT");

	/*                                                */
	/* Get the file name for writing:                 */
	/* EvaluateExpression starts with a GetToken():   */
	/*                                                */
	EvaluateExpression(&result);  /* get the filename */
	if(result.t!=STRINGVALUE)
		MadelineErrorReport("Write(): specify a filename to write to");
	
	/*                                                          */
	/* No need to get another token after EvaluateExpression(): */
	/*                                                          */
	if(!(pT->i==INC || pT->i==FORMC))
		MadelineErrorReport("Write(): \"IN\" or \"FORMAT\" keyword expected");
	if(pT->i==FORMC) KeywordSeen++;
	/*                         */
	/* get the format keyword: */
	/*                         */
	GetToken();

	/*                                               */
	/* store the format indicator flag in g control: */
	/*                                               */
	g->OutputFormat=pT->i;

	/*                                     */
	/* store the output function pointers: */
	/*                                     */
	if(!pT->u.m)
		MadelineErrorReport("Write(): Unrecognized format \"%s\"",pT->e);

	DataHeaderProcessor=pT->u.m->DataHeaderProcessor;
	OutputPedigree     =pT->u.m->OutputPedigree;
	DataFooterProcessor=pT->u.m->DataFooterProcessor;
	ShowStatistics     =pT->u.m->ShowStatistics;
	
	/*                                           */
	/* get the "FORMAT" keyword if not yet seen: */
	/*                                           */
	if(!KeywordSeen){
		GetToken();
		if(pT->i!=FORMC)
			MadelineErrorReport("Write(): \"FORMAT\" keyword expected:\n\tWRITE TO \"filename\" IN xxx FORMAT");
	}

	/*                                          */
	/* Check if there is really anything to do: */
	/*                                          */
	if(!(   DataHeaderProcessor
	     && OutputPedigree
	     && DataFooterProcessor
	))
		MadelineErrorReport("Write(): output routines not yet available for this format");
		  	
	/*                                                                     */
	/*                                                                     */
	/* Data Header Processing: open file(s), reset counters, write header: */
	/*                                                                     */
	/*                                                                     */
	DataHeaderProcessor(g,result.s);
	
	/*                                                       */
	/* Write data: Note that gOutputFormat is global, so the */
	/* OutputFunction can access it if it is needed:         */
	/*                                                       */
	TraverseBinaryPedigreeTree2(g,g->pd,OutputPedigree);

	/*                                                       */
	/* Data Footer Processing: write footer(s), close files: */
	/*                                                       */
	DataFooterProcessor(g);
	/*                                            */
	/* Show (method-specific) statistics to user: */
	/*                                            */
	ShowStatistics(g);
	
}



/*                               */
/*                               */
/* SiblinkDataHeaderProcessor(): */
/*                               */
/*                               */
void SiblinkDataHeaderProcessor(PCONTROL c,char *fn){

	/*                                         */
	/* a map database is required for siblink: */
	/*                                         */
	if(!c->mp)
		MadelineErrorReport("SiblinkDataHeaderProcessor(): a marker map table is required\nfor the Siblink control file");

	/*                                                 */
	/* Affection status field is required for Siblink: */
	/*                                                 */
	if(gfnAffected.i==MISSING)
		MadelineErrorReport("SiblinkDataHeaderProcessor():\naffection status field is required for the Siblink format");

	/*                                                 */
	/* For Siblink, we also need to write to a control */
	/* file = a parameter file                         */
	/*                                                 */
	StoreString(&gParameterOutputFile,fn);

	/*                                                                     */
	/* change ending to .par by overwriting last 4 characters:             */
	/*                                                                     */
	/* -981113 ET Addendum: Siblink still doesn't permit a file extension, */
	/*  so just null terminate the file name instead:                      */
	/*                                                                     */
	/*strncpy(gParameterOutputFile.e.s+strlen(gParameterOutputFile.e.s)-4, */
	/*        ".ctl",4);                                                   */
	gParameterOutputFile.e.s[strlen(gParameterOutputFile.e.s)-4]='\0';

	/*                                                                     */
	/* Then open up the .par file for use by SiblinkDataFooterProcessor(): */
	/*                                                                     */
	c->streams.fpar=fopen(gParameterOutputFile.e.s,"wt");
	if(!c->streams.fpar)
		MadelineErrorReport("SiblinkDataHeaderProcessor(): unable to open \"%s\" for output",gParameterOutputFile.e.s);

	lprintf("Creating associated SIBLINK control/parameter file called \"%s\"\n",gParameterOutputFile.e.s);
		
	/*                                                         */
	/* Call generic header processor to open main output file: */
	/*                                                         */
	GenericDataHeaderProcessor(c,fn);

	/*                                                               */
	/* Write FORTRAN read statement at top of the pedigree file:     */
	/*                                                               */
	/* The first record is for reading number of individuals,        */
	/* family id, and total number of affected sibs                  */
	/*                                                               */
	/* The second record is a description for reading the individual */
	/* information, consisting of fixed core fields followed by the  */
	/* alleles:                                                      */
	/*                                                               */
	/* The id,fid,mid,sex and twin data are all taken from the       */
	/* INDIVIDUAL and are printed in a fixed format.                 */
	/*                                                               */
	/* Note that there are allele fields equal to two times          */
	/* the number of output genotype fields (of course):             */
	/*                                                               */
	fprintf(c->streams.fp,"(I4,1X,A8,1X,I4)\n");
	fprintf(c->streams.fp,"(A8,%iX,%i(I%i))\n",
		23,
		2*c->db->n.go,
		SIBLINKALLELEFIELDWIDTH
	);
	
}


/*                                                                          */
/*                                                                          */
/* SiblinkDataFooterProcessor(): Writes the control file to c->streams.fpar */
/* and, of course, then closes streams via a call to the generic processor: */
/*                                                                          */
void SiblinkDataFooterProcessor(PCONTROL c){

	int i;
	
	/*                                         */
	/* all integers in the control file header */
	/* can be printed in I4 format:            */
	/*                                         */
	int spc;
	int gap=35;
	int isz=4;
	int fsz=8;
	
	/*            */
	/* nprob,nrep */
	/*            */
	spc=gap-isz*2;
	fprintf(c->streams.fpar,"%4i%4i%*.*snprob nrep\n",
		1,
		1,
		spc,
		spc,
		" ");

	/*                                                   */
	/* nfam, paropt, anaopt, ntloc  ,nloc,  yeswt:       */
	/*        addit.  grid   # loci # loci   1=wt.       */
	/*                       total  to anal. 2=don't wt. */
	/*                                                   */
	spc=gap-isz*7;
	/*                                                               */
	/* Differentiate between current and "future" Siblink formats:   */
	/*                                                               */
	/* -> 981023: CSIBL no longer being used: only "current" formats */
	/*    are available:                                             */
	/*                                                               */
	/* if(c->OutputFormat==CSIBL){                                   */
	/*                                                               */
	
	/*                 */
	/* Current format: */
	/*	                */
	fprintf(c->streams.fpar,"%4i%4i%4i%4i%4i%4i%4i%*.*snsibp paropt anaopt ntloc nloc yeswt outputIBDs\n",
		c->nuclear.sibp.in,
		1,
		1,
		c->db->n.go,
		c->db->n.go,
		2,
		1,
		spc,
		spc,
		" ");
		
	/*                                                                                          */
	/*}else{                                                                                    */
	/*	//                                                                                       */
	/*	// Future format: really, it's the same (to date) except for yeswt option:               */
	/*	//                                                                                       */
	/*	fprintf(c->streams.fpar,"%4i%4i%4i%4i%4i%4i%*.*snsibp paropt anaopt ntloc nloc yeswt\n", */
	/*		c->nuclear.sibp.in,                                                                     */
	/*		1,                                                                                      */
	/*		1,                                                                                      */
	/*		c->db->n.go,                                                                            */
	/*		c->db->n.go,                                                                            */
	/*		1,                                                                                      */
	/*		spc,                                                                                    */
	/*		spc,                                                                                    */
	/*		" ");	                                                                                  */
	/*}                                                                                         */
	/*                                                                                          */

	/*               */
	/* testd, dlocs: */
	/*               */
	spc=gap-fsz*2;
	fprintf(c->streams.fpar,"%8.5f%8.5f%*.*stestd dlocs\n",
		0.01*EvaluationInterval.e.v,
		0.500,
		spc,
		spc,
		" ");
			
	/*                                              */
	/* ZIBD vector for simul, not curr. functional: */
	/*                                              */
	spc=gap-fsz*3;
	fprintf(c->streams.fpar,"%8.3f%8.3f%8.3f%*.*szibd(0) zibd(1) zibd(2)\n",
		0.250,
		0.500,
		0.250,
		spc,
		spc,
		" ");
	
	/*                                                            */
	/* loop through GENOTYPE OUTPUT fields:                       */
	/* -- note: i is the rank index, not the actual field index:  */
	/*                                                            */
	/* --> OutputSiblinkMarker() contains code to distinguish the */
	/*     Current and Future Siblink formats:                    */
	/*                                                            */
	for(i=0;i<c->db->n.go;i++)
			OutputSiblinkMarker(c,i);
	/*                                                            */
	/* Final simulation parameter junk required at the end of the */
	/* Current Siblink format:                                    */
	/*                                                            */
	spc=gap-isz*6;

	/*                             */
	/* if(c->OutputFormat==CSIBL){ */
	/*                             */

		fprintf(c->streams.fpar,"%4i%4i%4i%4i%4i%4i%*.*snpeo2 npeo nsib2 nsib noff ncopy\n",
			12,
			 4,
			10,
			 2,
			 0,
			 1,
			spc,
			spc,
			" ");
	/*  */
	/*} */
	/*  */
	
	/* */
	/* */
	/* */

	/*       */
	/* done: */
	/*       */


	/*                                                    */
	/* Call the generic footer processor to close streams */
	/* and show stats:                                    */
	/*                                                    */
	GenericDataFooterProcessor(c);
}


/*                                                        */
/*                                                        */
/* Locus(): handles the WRITE LOCUS FILE TO "..." command */
/* to producd locus files by gene counting based on the   */
/* currently opened input data set:                       */
/*                                                        */
void Locus(void){

	RESULT result;
	int KeywordSeen=0;

	int i;
	
	extern PCONTROL g;
	
	void (*LocusHeaderProcessor)(PCONTROL,char *);
	void (*OutputMarker)(PCONTROL,int);
	void (*LocusFooterProcessor)(PCONTROL);
	
	/*                                                    */
	/* This function is called from Write() after Write() */
	/* detects the LOCUS keyword:                         */
	/*                                                    */

	/*                             */
	/* "FILE" keyword is expected: */
	/*                             */
	GetToken();
	if(pT->i!=FILEK)
		MadelineErrorReport("Locus(): \"FILE\" keyword expected:\n\tWRITE LOCUS FILE TO \"filename\" IN xxx FORMAT");
	/*                                       */
	/* Get here if "TO" keyword is expected: */
	/*                                       */
	GetToken();
	if(pT->i!=TOC)
		MadelineErrorReport("Locus(): \"TO\" keyword expected:\n\tWRITE LOCUS FILE TO \"filename\" IN xxx FORMAT");
	
	/*                                                */
	/* Get the file name for writing:                 */
	/* EvaluateExpression starts with a GetToken():   */
	/*                                                */
	EvaluateExpression(&result);  /* get the filename */
	if(result.t!=STRINGVALUE)
		MadelineErrorReport("Locus(): specify a filename to write to");
	
	/*                                                          */
	/* No need to get another token after EvaluateExpression(): */
	/*                                                          */
	if(!(pT->i==INC || pT->i==FORMC))
		MadelineErrorReport("Locus(): \"IN\" or \"FORMAT\" keyword expected");
	if(pT->i==FORMC) KeywordSeen++;
	/*                         */
	/* get the format keyword: */
	/*                         */
	GetToken();
	g->OutputFormat=pT->i;
	/*                               */
	/* get the output file pointers: */
	/*                               */
	if(!pT->u.m)
		MadelineErrorReport("Locus(): Unrecognized file format: %s",pT->e);
		
	LocusHeaderProcessor=pT->u.m->LocusHeaderProcessor;
	OutputMarker        =pT->u.m->OutputMarker;
	LocusFooterProcessor=pT->u.m->LocusFooterProcessor;
	
	

	/*                                           */
	/* get the "FORMAT" keyword if not yet seen: */
	/*                                           */
	if(!KeywordSeen){
		GetToken();
		if(pT->i!=FORMC)
			MadelineErrorReport("Locus(): \"FORMAT\" keyword expected: \"...IN xxxxx FORMAT\"");
	}

	/*                                              */
	/* Check that keyword is valid in this context: */
	/*                                              */
	if(!(   LocusHeaderProcessor
	     && OutputMarker
	     && LocusFooterProcessor
	))
		MadelineErrorReport("Locus(): locus file output routines not available for this format");

	/*                              */
	/* Start processing locus file: */
	/*                              */
	LocusHeaderProcessor(g,result.s);
		
	/*                                           */
	/* Loop through output genotype fields only: */
	/*                                           */
	for(i=0;i<g->db->n.go;i++){
		/*                                                      */
		/* i is now the rank index, not the actual field index: */
		/* all the OutputMarker routines now recognize this to  */
		/* figure out the actual field index:                   */
		/*                                                      */
		OutputMarker(g,i);
	}
	
	/*                               */
	/* Finish processing locus file: */
	/*                               */
	LocusFooterProcessor(g);
	/*                            */
	/* Tell user what's going on: */
	/*                            */
	lprintf("Locus file \"%s\" has been written.\n",result.s);
}





/*                                */
/* Quit() terminates the program: */
/*                                */
void Quit(void){

	extern PCONTROL g;

	/*                                                     */
	/* Print error summary, if any, and reset the counter: */
	/*                                                     */
	ResetErrorCounters(g);
		
	/*                    */
	/* Release resources: */
	/*                    */
	lprintf("Releasing resources ...\n");

	/*                                          */
	/* Release Database, marker map, pedigrees: */
	/*                                          */
	if(g->db) CloseDatabase(g->db);
	if(g->pd) FreeBinaryPedigreeTree(g->pd);
	if(g->mp) FreeMap(g->mp);


	/*                                          */
	/* Release graphics object for any pedigree */
	/* drawing that took place:                 */
	/*                                          */
	if(gDrawing){
		FreeDrawing(gDrawing);
		gDrawing=NULL;
	}


	/*                                        */
	/* Release internal variables and arrays: */
	/*                                        */
	FreeInternalVariables();

	/*                   */
	/* Release stack(s): */
	/*                   */
	if(Token)  FreeTokenStack(Token ,&TokenStackSize );
	if(Token2) FreeTokenStack(Token2,&TokenStack2Size);	
	
	
	/*                        */
	/* Close any open files:  */
	/*                        */
	if(g->streams.log    ) fclose(g->streams.log    );
	if(g->streams.detail ) fclose(g->streams.detail );
	if(g->streams.fp     ) fclose(g->streams.fp     );
	if(g->streams.fpar   ) fclose(g->streams.fpar   );
	if(g->streams.fpar2  ) fclose(g->streams.fpar2  );
	if(g->streams.prn    ) fclose(g->streams.prn    );
	if(g->streams.err    ) fclose(g->streams.err    );
	if(g->streams.command) fclose(g->streams.command);
	
	/*              */
	/* Say goodbye: */
	/*              */
	lprintf("Goodbye!\n");
	
	/*        */
	/* leave: */
	/*        */
	exit(0);
}


/*                                     */
/* double Inverse(): take the inverse: */
/*                                     */
double Inverse(double v){

	if(v==0.0)
		MadelineErrorReport("Inverse(): Division by zero is not permitted.");
		
	return (double) 1.0/v;
}


/*                                                                   */
/* NewTokenStack(): returns a token stack.  This function is called  */
/* at program start up in order to provide stacks for the expression */
/* parser:                                                           */
/*                                                                   */
PTOKENINFO NewTokenStack(int stacksize){

	int i;
	PTOKENINFO tstack;
	
	tstack=(PTOKENINFO) malloc((unsigned) stacksize*sizeof(TOKENINFO));
	if(!tstack) MadelineErrorReport("NewTokenStack(): out of memory");

	/*                            */
	/* intialize the token stack: */
	/*                            */
	for(i=0;i<stacksize;i++){
		tstack[i].e[0]='\0';
		tstack[i].i   =NOINTERNALTOKEN;
		tstack[i].t   =OPERATOR;
		tstack[i].f   =MISSING;
		tstack[i].v   =DOUBLEMISSINGVALUE;
		tstack[i].u.p =NULL;
	}
	/*                        */
	/* return tstack pointer: */
	/*                        */
	return tstack;
}


/*                                                                      */
/* IncreaseTokenStack(): increases the token stack's current allocation */
/* by one additional TOKENSTACKALLOCATIONUNIT unit.  This may not be    */
/* used currently.                                                      */
/*                                                                      */
PTOKENINFO IncreaseTokenStack(PTOKENINFO tstack,int oldsize,int newsize){

	int i;
	
	tstack=(PTOKENINFO)realloc(tstack,newsize*sizeof(TOKENINFO));
	if(!tstack) MadelineErrorReport("IncreaseTokenStack(): out of memory");
	/*                               */
	/* initialize the new part only: */
	/*                               */
	for(i=oldsize;i<newsize;i++){
		tstack[i].e[0]='\0';
		tstack[i].i   =NOINTERNALTOKEN;
		tstack[i].t   =OPERATOR;
		tstack[i].f   =MISSING;
		tstack[i].v   =DOUBLEMISSINGVALUE;
		tstack[i].u.p =NULL;
	}
	/*                          */
	/* return pointer to stack: */
	/*                          */
	return tstack;
}


/*                                           */
/* ClearTokenStack(): Clear the token stack: */
/*                                           */
void ClearTokenStack(PTOKENINFO tstack,int stacksize){

	int i;
	
	for(i=0;i<stacksize;i++){
		tstack[i].e[0]='\0';
		tstack[i].i   =NOINTERNALTOKEN;
		tstack[i].t   =OPERATOR;
		tstack[i].f   =MISSING;
		tstack[i].v   =DOUBLEMISSINGVALUE;
		tstack[i].u.p =NULL;
	}
}



/*                                                                       */
/*                                                                       */
/* InitializeInternalVariables():                                        */
/*                                                                       */
/* Initializes Cmv and Nmv, internal variables for field names, etc.     */
/*                                                                       */
void InitializeInternalVariables(){
	
	/*                                         */
	/* all gfXXXX char * are initially null:   */
	/* since these defaults can all be changed */
	/* by the program, we start off with       */
	/* dynamic allocation, so we can           */
	/* later call free()                       */
	/*                                         */
	/*                                         */
	extern PARRAY pCmv;
	extern PARRAY pNmv;
	
	/*                                              */
	/* Defaults for main databases opened via OPEN: */
	/*                                              */
	gfnFamID.e.s   =CopyString(FAMILY_ID_FIELD);
	gfnID.e.s      =CopyString(STUDY_ID_FIELD);
	gfnFather.e.s  =CopyString(FATHER_FIELD);
	gfnMother.e.s  =CopyString(MOTHER_FIELD);
	gfnSex.e.s     =CopyString(GENDER_FIELD);
	gfnMZTwin.e.s  =CopyString(MZ_TWIN_FIELD);
	gfnDZTwin.e.s  =CopyString(DZ_TWIN_FIELD);
	gfnAffected.e.s=CopyString(AFFECTION_STATUS_FIELD);
	gfnProband.e.s =CopyString(PROBAND_FIELD);
	gfnClass.e.s   =CopyString(CLASS_FIELD);
	gfnLClass.e.s  =CopyString(LIABILITY_CLASS_FIELD);
	gfnDead.e.s    =CopyString(DECEASED_STATUS_FIELD);
	gfnDOB.e.s     =CopyString(DATE_OF_BIRTH_FIELD);
	gfnDOD.e.s     =CopyString(DATE_OF_DEATH_FIELD);
	
	/*                                    */
	/* For map databases opened via LOAD: */
	/*                                    */
	gfnMapChromosome.e.s    =CopyString(MAP_CHROMOSOME_FIELD     );
	gfnMapPosition.e.s      =CopyString(MAP_POSITION_FIELD       );
	gfnMapFemalePosition.e.s=CopyString(MAP_FEMALE_POSITION_FIELD);
	gfnMapMalePosition.e.s  =CopyString(MAP_MALE_POSITION_FIELD  );
	gfnMapPositionBP.e.s    =CopyString(MAP_POSITION_BP_FIELD    );
	gfnMapOrdinal.e.s       =CopyString(MAP_ORDINAL_FIELD        );
	/*                                      */
	/* This is also used for marker tables: */
	/*                                      */
	gfnMapMarker.e.s        =CopyString(MAP_MARKERNAME_FIELD     );
	
	/*                              */
	/* For Allele Frequency Tables: */
	/*                              */
	gfnAlleleField.e.s      =CopyString(ALLELE_FIELD             );
	gfnFrequencyField.e.s   =CopyString(FREQUENCY_FIELD          );
	
	/* For graphing functionality: */
	gfnGraphPosition.e.s=CopyString(MAP_POSITION_FIELD);
	gfnGraphScore.e.s   =CopyString(GRAPH_SCORE_FIELD);
	gGraphTitle.e.s     =CopyString(GRAPH_TITLE);
	gGraphXAxisLabel.e.s=CopyString(GRAPH_X_AXIS_LABEL);
	gGraphYAxisLabel.e.s=CopyString(GRAPH_Y_AXIS_LABEL);
	/*                    */
	/* For marker tables: */
	/*                    */
	gfnAllele1.e.s   =CopyString(ALLELE_1_FIELD);
	gfnAllele2.e.s   =CopyString(ALLELE_2_FIELD);

	/*                                       */
	/* field indexes initialized to MISSING: */
	/*                                       */
	gfnID.i      =MISSING;
	gfnFather.i  =MISSING;
	gfnMother.i  =MISSING;
	gfnSex.i     =MISSING;
	gfnMZTwin.i  =MISSING;
	gfnDZTwin.i  =MISSING;
	gfnFamID.i   =MISSING;
	gfnAffected.i=MISSING;
	gfnProband.i =MISSING;
	gfnClass.i   =MISSING;
	gfnLClass.i  =MISSING;
	gfnDead.i    =MISSING;
	gfnDOB.i     =MISSING;
	gfnDOD.i     =MISSING;
	
	/*                              */
	/* defaults for the file names: */
	/*                              */
	/*                              */
	StoreString(&gAllLogFiles        ,ALL_LOGS_BASE_NAME);
	StoreString(&gDetailFile         ,DETAIL_LOG_NAME);
	StoreString(&gLogFile            ,GENERAL_LOG_NAME);
	StoreString(&gErrorFile          ,ERROR_LOG_NAME);
	StoreString(&gCommandFile        ,COMMAND_LOG_NAME);
	StoreString(&gDatabaseFile       ,DATABASE_FILE_NAME);
	StoreString(&gOutputFile         ,OUTPUT_FILE_NAME);
	StoreString(&gParameterOutputFile,PARAM_OUTPUT_FILE_NAME);
	StoreString(&gMapDatabase        ,MAP_DATABASE_FILE_NAME);

	/*                                     */
	/* Default for the PrintCommand, etc.: */
	/*                                     */
	StoreString(&gPrintCommand       ,PRINT_COMMAND);
	StoreString(&gPedigreeDrawing    ,POSTSCRIPT_PEDIGREE_NAME);
	StoreString(&gGraphDrawing       ,POSTSCRIPT_GRAPH_NAME);
	StoreString(&gEditor             ,EDIT_COMMAND);
	StoreString(&gPSViewerName       ,POSTSCRIPT_VIEWER);
	StoreString(&gWebViewerName      ,WEB_VIEWER);
	StoreString(&gWebAddress         ,WEB_ADDRESS);
	
	/*                                              */
	/* all arrays were initialized with 0 elements, */
	/* so need to increase them:                    */
	/*                                              */
	IncreaseArray(pCmv);
	IncreaseArray(pNmv);
	IncreaseArray(pCsv);
	IncreaseArray(pNsv);
	IncreaseArray(pCaff);
	IncreaseArray(pNaff);
	IncreaseArray(pCds);
	IncreaseArray(pNds);
		
	/*                              */
	/* for CharacterMissingValue[]: */
	/*                              */
	pCmv->n=5;
	pCmv->e[0].s=CopyString(CHARACTER_MISSING_VALUE_1); /* generic missing value */
	pCmv->e[1].s=CopyString(CHARACTER_MISSING_VALUE_2); /* missing genotype      */
	pCmv->e[2].s=CopyString(CHARACTER_MISSING_VALUE_3); /* missing genotype      */
	pCmv->e[3].s=CopyString(CHARACTER_MISSING_VALUE_4); /* missing genotype      */
	pCmv->e[4].s=CopyString(CHARACTER_MISSING_VALUE_5); /* missing genotype      */
	
	/*                            */
	/* for NumericMissingValue[]: */
	/*                            */
	pNmv->n=1;
	pNmv->e[0].v=NUMERIC_MISSING_VALUE_1;
	
	/*                            */
	/* for CharacterSexValue[]:   */
	/* *** THIS ARRAY IS OBSOLETE */
	/* *** see GenderStatus[] *** */
	pCsv->n=2;
	pCsv->e[FEMALE].s=CopyString(DEFAULT_CODE_FOR_FEMALE);
	pCsv->e[MALE  ].s=CopyString(DEFAULT_CODE_FOR_MALE);
	
	/*                            */
	/* for NumericSexValue[]:     */
	/* *** THIS ARRAY IS OBSOLETE */
	/* *** see GenderStatus[] *** */
	/*                            */
	pNsv->n=2;
	pNsv->e[FEMALE].v=(double)FEMALE;
	pNsv->e[MALE  ].v=(double)MALE;
	
	/*                              */
	/* for NumericAffection Status: */
	/*                              */
	pNaff->n=5;
	pNaff->e[0].v=(double) NUM_STATUS_UNAFFECTED;          /* unaffected (studied)                    */
	pNaff->e[1].v=(double) NUM_STATUS_AFFECTED;            /* affected   (studied)                    */
	pNaff->e[2].v=(double) NUM_STATUS_REPORTED_UNAFFECTED; /* reported as unaffected, but not studied */
	pNaff->e[3].v=(double) NUM_STATUS_REPORTED_AFFECTED;   /* reported as affected, but not studied   */
	pNaff->e[4].v=(double) NUM_STATUS_REPORTS_CONFLICTING;  /* conflicting reports and not studied     */

	/*                                */
	/* for CharacterAffection Status: */
	/*                                */
	pCaff->n=5;
	pCaff->e[0].s=CopyString(CHR_STATUS_UNAFFECTED         );
	pCaff->e[1].s=CopyString(CHR_STATUS_AFFECTED           );
	pCaff->e[2].s=CopyString(CHR_STATUS_REPORTED_UNAFFECTED);
	pCaff->e[3].s=CopyString(CHR_STATUS_REPORTED_AFFECTED  );
	pCaff->e[4].s=CopyString(CHR_STATUS_REPORTS_CONFLICTING);

	
	/*                                               */
	/* for CharacterDeathStatus[]:                   */
	/*                                               */
	pCds->n=2;
	pCds->e[ALIVE].s=CopyString(CHR_NOT_DECEASED); /* no, not deceased */
	pCds->e[DEAD ].s=CopyString(CHR_IS_DECEASED ); /* yes, deceased    */

	
	/*                           */
	/* for NumericDeathStatus[]: */
	/*                           */
	pNds->n=2;
	pNds->e[ALIVE].v=(double)ALIVE;  /* no, not dead */
	pNds->e[DEAD ].v=(double)DEAD;   /* yes, dead    */
	
	/*                                                                  */
	/* For pedigree drawing label and legend font sizes:                */
	/*                                                                  */
	LabelFontSize.e.v =(double) LABEL_FONT_SIZE; /* pedigree drawing label font size  */
	LegendFontSize.e.v=(double) LEGEND_FONT_SIZE; /* pedigree drawing legend font size */
		
	/*                                                                  */
	/* For evaluation interval and off-end distances, in centiMorgans:  */
	/*                                                                  */
	EvaluationInterval.e.v= EVALUATION_INTERVAL; /* evaluation interval in centiMorgans */
	OffEndDistance.e.v    = OFF_END_DISTANCE; /* distance off-end in centiMorgans    */
	
	
	gColorBlack.e.s      =CopyString(COLOR_BLACK);      
	gColorBlue.e.s       =CopyString(COLOR_BLUE );       
	gColorBrown.e.s      =CopyString(COLOR_BROWN);      
	gColorCyan.e.s       =CopyString(COLOR_CYAN );       
	gColorDarkGray.e.s   =CopyString(COLOR_DKGRY);   
	gColorForestGreen.e.s=CopyString(COLOR_FRGRN);
	gColorGray.e.s       =CopyString(COLOR_GRAY );       
	gColorGreen.e.s      =CopyString(COLOR_GREEN);      
	gColorLightGray.e.s  =CopyString(COLOR_LTGRY);  
	gColorMagenta.e.s    =CopyString(COLOR_MAGEN);    
	gColorOrange.e.s     =CopyString(COLOR_ORANG);     
	gColorPeach.e.s      =CopyString(COLOR_PEACH);      
	gColorPurple.e.s     =CopyString(COLOR_PURPL);     
	gColorRed.e.s        =CopyString(COLOR_RED  );        
	gColorWhite.e.s      =CopyString(COLOR_WHITE);      
	gColorYellow.e.s     =CopyString(COLOR_YELLW);     
	
}


/*                                                    */
/* SubString(): place a substring of "src" consisting */
/* of "howmany" characters starting from "stt" into   */
/* "dest":                                            */
/*                                                    */
void SubString(char *dest,char *src,int stt,int howmany){

	char *end;
	/*                                                                    */
	/* force the character arrays to be one-offset instead of zero-offset */
	/* from the user's perspective:                                       */
	/*                                                                    */
	if(stt<1 || howmany<1){
		MadelineErrorReport("SubString(): numeric parameters must be zero or positive");
	}

	stt--;     
	if(stt>(int)strlen(src)-1){
		/*                                                                          */
		/* In order for querying to work properly, this cannot generate             */
		/* an error and should instead return the empty string, i.e., the           */
		/* universal missing value indicator for character variables:               */
		/*                                                                          */
		/* MadelineErrorReport("SubString(): starting place beyond end of string"); */
		/*                                                                          */
		dest[0]='\0';
		return;
	}
	
	src+=stt;
	end=src+howmany;
	
	for(;*src && src<end;*dest++=*src++);
	*dest='\0';
	return;

}


/*                                                            */
/* Assignment: assign a value to a variable or array element: */
/*                                                            */
/* void Assignment(void){                                     */


/*                                                                  */
/* CommandLine():  This is Madeline's command line processor which  */
/* prints the prompt and waits for command input from the user:     */
/*                                                                  */
void CommandLine(FILE *fp){
	
	char b[COMMANDLINEWIDTH+1];
	int HasHopped;
	char *rd=NULL;
	char *prompt;
	/* char *newlinepos; */
	
	extern PCONTROL g;
	
	using_history();
	
	HasHopped=setjmp(BunnyHop);
	for(;;){
		
		
		if(fp==stdin){
			/* construct the prompt string */
			prompt=ConstructPrompt(g);
			/* use GNU's readline() --             */
			/* Thank you, Richard Stallman et al.! */
			/*                                     */
			rd=readline (prompt);
			if(!rd) break;
			if(*rd){
				/* Here we add non-null lines */
				/* to the history:            */
				add_history (rd);
			}
			/*                                                */
			/* Echo the command line to log and detail files: */
			/* Since GNU's readline() truncates the '\n', we  */
			/* need to add it back on when printing the lines */
			/* to the log files                               */
			if(g->streams.log)      fprintf(g->streams.log   ,"> %s\n",rd); 
			if(g->streams.detail)   fprintf(g->streams.detail,"> %s\n",rd);
			
			/* 2004.10.22.ET ADDENDUM:                        */
			/* Echo commands to the new command log:          */
			if(g->streams.command)  fprintf(g->streams.command,"%s\n",rd);
			
		}else{
		  /*               */
		  /* print prompt: */
		  /*               */
		  /* PrintNonInteractivePrompt(g); */
		
			/*             */
			/* get a line: */
			/*             */
			rd=fgets(b,COMMANDLINEWIDTH,fp);
			if(rd && *rd) add_history(rd);
			/*                                                */
			/* Echo the command line to log and detail files: */
			/* Note that fgets() retains the '\n' at the end: */
			if(rd && g->streams.log)      fprintf(g->streams.log   ,"> %s",rd); 
			if(rd && g->streams.detail)   fprintf(g->streams.detail,"> %s",rd);
		}
		/*                                                                */
		/* Switching from a batch file submitted on the command line back */
		/* to stdin:                                                      */
		/*                                                                */
		if(fp!=stdin && rd==NULL){
			/*                                            */
			/* restore defaults for gVerbose and gSilent: */
			/*                                            */
			g->Verbose=1;
			g->Silent =0;
							
			lprintf("NOTE: Batch file has terminated without a QUIT command.\n");
			lprintf("Madeline is now in interactive mode\n");
		
			/*                   */
			/* close batch file: */
			/*                   */
			fclose(fp);
		
			/*                  */
			/* switch to stdin: */
			/*                  */
			fp=stdin;
		
		}
		/*                                                               */
		/* echo line when reading from batch file if not in silent mode: */
		/*                                                               */
		
		if(fp!=stdin && !g->Silent) fprintf(stderr ,"%s",rd);
		
		
		/*                                                         */
		/* Interpret the command line, if it's not a comment line: */
		/* (Comment lines start with "//" as in C++)               */
		/*                                                         */
		/* if(!(b[0]=='/' && b[1]=='/')) */
		if(rd && !(rd[0]=='/' && rd[1]=='/')) Interpret(rd);                   
	}
}


/*                                                                       */
/* BatchCommandLine(): Like CommandLine(), but with slight modifications */
/*                     for supporting batch files loaded with Run()      */
/*                                                                       */
void BatchCommandLine(char *fn){

	char b[COMMANDLINEWIDTH+1];
	FILE *fp;
	char *stt="M-Batch> *** Running batch file \"%s\"... ***\n";
	char *end="M-Batch> *** Finished batch file \"%s\"... ***\n";
	char *mid="M-Batch> %s";

	extern PCONTROL g;
	/* extern int history_length; */
	
	/*                  */
	/* open batch file: */
	/*                  */
	fp=fopen(fn,"rt");
	if(!fp)
		MadelineErrorReport("BatchCommandLine(): Unable to open \"%s\"",fn);

	/*                   */
	/* show entry point: */
	/*		                 */
	fprintf(stderr,stt,fn);
	if(g->streams.log)      fprintf(g->streams.log   ,stt,fn);
	if(g->streams.detail)   fprintf(g->streams.detail,stt,fn);

	/*                     */
	/* run batch commands: */
	/*	                    */
	while(fgets(b,COMMANDLINEWIDTH,fp)){

		/*                                                               */
		/* echo line when reading from batch file if not in silent mode: */
		/*                                                               */
		if(!g->Silent) fprintf(stderr ,mid,b);
		/*                                                                 */
		/* in either case, also echo command line to log and detail files: */
		/*                                                                 */
		if(g->streams.log)      fprintf(g->streams.log   ,mid,b);
		if(g->streams.detail)   fprintf(g->streams.detail,mid,b);
		/*                                                            */
		/* Interpret the command line:                                */
		/* (Unless it is a comment line starting with "//" as in C++) */
		/*                                                            */
		if(!(b[0]=='/' && b[1]=='/')){
			add_history(b);
			Interpret(b);
		}                 
	}
	/*                                */
	/* all done: close fp and return: */
	/*                                */
	fclose(fp);
	
	/*                  */
	/* show exit point: */
	/*                  */
	fprintf(stderr,end,fn);
	if(g->streams.log)      fprintf(g->streams.log   ,end,fn);
	if(g->streams.detail)   fprintf(g->streams.detail,end,fn);
	
}


/*                         */
/* Run(): run a batch file */
/*                         */
void Run(void){

	RESULT result;
	
	/*                                                */
	/* Get the file name reading:                     */
	/* EvaluateExpression starts with a GetToken():   */
	/*                                                */
	EvaluateExpression(&result);  /* get the filename */
	if(result.t!=STRINGVALUE)
		MadelineErrorReport("Run(): specify a batch file name to run");
	
	BatchCommandLine(result.s);
	
}	


/*             */
/*             */
/* PrintPrompt */
/*             */
/*             */
void PrintPrompt(PCONTROL c,FILE *source){

	int continued=0;

	if(c->HasSyntaxErrors){
		fprintf(stderr,"%i SYNTAX ERROR%s",
			c->HasSyntaxErrors,
			c->HasSyntaxErrors==1?"":"S"
		);
		continued=1;
	}
	if(c->HasWarnings){
		fprintf(stderr,"%s%i WARNING%s",
			continued?", ":"",
			c->HasWarnings,
			c->HasWarnings==1?"":"S"
		);
		continued=1;
	}
	if(c->HasSevereWarnings){
		/* Put Severe warnings in BOLD AND RED: */
		ConsoleBold(stderr);
		ConsoleRed(stderr);
		fprintf(stderr,"%s%i SEVERE WARNING%s",
			continued?", ":"",
			c->HasSevereWarnings,
			c->HasSevereWarnings==1?"":"S"
		);
		ConsoleNormal(stderr);
		continued=1;
	}
	if(c->HasInheritanceErrors){
		fprintf(stderr,"%s%i INHERITANCE INCONSISTENC%s",
			continued?", ":"",
			c->HasInheritanceErrors,
			c->HasInheritanceErrors==1?"Y":"IES"
		);
		continued=1;
	}
	if(c->HasErrors){
		/* Put fatal errors in BOLD AND RED: */
		ConsoleBold(stderr);
		ConsoleRed(stderr);
		fprintf(stderr,"%s%i FATAL ERROR%s",
			continued?", ":"",
			c->HasErrors,
			c->HasErrors==1?"":"S"
		);
		ConsoleNormal(stderr);
		continued=1;
	}
	/*               */
	/* print prompt: */
	/*               */
	if(source==stdin){
		fprintf(stderr,"%sM>",
			continued?" ":""
		);
	}else{
		fprintf(stderr,"\n");
	}
}

/*                                           */
/* ConstructPrompt                           */
/*                                           */
/* Instead of printing the prompt on stderr  */
/* like PrintPrompt does, ConstructPrompt    */
/* returns a pointer to a static buffer      */
/* which contains the current prompt string. */
/* This is done so that the GNU readline()   */
/* function can be used.                     */
/*                                           */
char *ConstructPrompt(PCONTROL c){

	int continued=0;
	static char prompt[SMALLBUFFERSIZE];
	char hold[MINIMUMBUFFERSIZE];
	extern int gScanning;
	
	prompt[0]='\0';
	
	if(gScanning){
		
		sprintf(prompt,"SCANNING LEVEL %03i:",gScanning);
		return (char *)prompt;
		
	}
	
	/* Get here if not scanning: */
	if(c->ResetPrompt){
		
		/* Reset the prompt: */
		c->ResetPrompt=0;
		
		if(c->HasSyntaxErrors){
			sprintf(prompt,"%i SYNTAX ERROR%s",
				c->HasSyntaxErrors,
				c->HasSyntaxErrors==1?"":"S"
			);
			continued=1;
		}
		if(c->HasWarnings){
			sprintf(hold,"%s%i WARNING%s",
				continued?", ":"",
				c->HasWarnings,
				c->HasWarnings==1?"":"S"
			);
			strcat(prompt,hold);
			continued=1;
		}
		if(c->HasSevereWarnings){
			sprintf(hold,"%s%i SEVERE WARNING%s",
				continued?", ":"",
				c->HasSevereWarnings,
				c->HasSevereWarnings==1?"":"S"
			);
			strcat(prompt,hold);
			continued=1;
		}
		if(c->HasInheritanceErrors){
			sprintf(hold,"%s%i INHERITANCE INCONSISTENC%s",
				continued?", ":"",
				c->HasInheritanceErrors,
				c->HasInheritanceErrors==1?"Y":"IES"
			);
			strcat(prompt,hold);
			continued=1;
		}
		if(c->HasErrors){
			sprintf(hold,"%s%i FATAL ERROR%s",
				continued?", ":"",
				c->HasErrors,
				c->HasErrors==1?"":"S"
			);
			strcat(prompt,hold);
			continued=1;
		}
		/* Add M> prompt */
		sprintf(hold,"%sM>",continued?" ":"");
		strcat(prompt,hold);
		return (char *)prompt;
		
	}
	
	/* Default case: "M>" prompt: */
	
	sprintf(prompt,"M>");
	return (char *)prompt;
	
}


/*                                                                     */
/* PrintStatusTableLine(): Print a formatted line of the status table: */
/*                                                                     */
void PrintStatusTableLine(const char *state,const char *setting,const char *description){
	
	int i, remainingSpaces;
	
	if(state[0]=='H' && state[1]=='\0'){
		/* print horizontal table line: */
		lprintf("+-----------------------+-----------+-----------------------------------------+\n");
	}else{
		/* ... else print information line: */
		lprintf("| %-22.22s| %-10.10s|",
			state,
			setting
		);
		/* Description could be in UTF-8: */
		lprintf("%s",description);
		
		remainingSpaces = 40 - ColumnOccupancy(description);
		for(i=0;i<=remainingSpaces;i++) lprintf(" ");
		lprintf("|\n");
		
	}
}


/*                                                         */
/*                                                         */
/* Hello(): returns system status information:             */
/*                                                         */
/* --> This command is now mapped as both HELLO and STATUS */
/*                                                         */
void Hello(void){
	
	extern PLANGUAGEOBJECT Language[LANGUAGE_COUNT];
	extern PCONTROL g;
	char hold[15];
	char *setting;
	char *description;
	char *delimiter;
	
	/* Table Heading */
	PrintStatusTableLine("H",NULL,NULL);
	PrintStatusTableLine("Variable or State Flag","Setting","Description");
	/* PrintStatusTableLine("H",NULL,NULL); */

	/* EXTERNAL PROGRAM SETTINGS */
	PrintStatusTableLine("H",NULL,NULL);
	PrintStatusTableLine("EXTERNAL PROGRAMS","","");
	PrintStatusTableLine("H",NULL,NULL);
	/* File Editor */
	PrintStatusTableLine("Editor",gEditor.e.s,"Program used to edit files");
	/* PostscriptViewer */
	PrintStatusTableLine("PostscriptViewer",gPSViewerName.e.s,"Program used to view Postscript drawings");
	/* Print command */
	PrintStatusTableLine("PrintCommand",gPrintCommand.e.s,"System program used to print files");
	/* Web Browser */
	PrintStatusTableLine("WebBrowser",gWebViewerName.e.s,"Program used to view HTML documentation");

	/* EVALUATION INTERVAL AND OFF-END DISTANCE */
	PrintStatusTableLine("H",NULL,NULL);
	PrintStatusTableLine("EVALUATION SETTINGS","","");
	PrintStatusTableLine("H",NULL,NULL);
	/* Evaluation Interval */
	sprintf(hold,"%6.2f cM",EvaluationInterval.e.v);
	PrintStatusTableLine("EvaluationInterval",hold,"Value to write to control file.");
	/* OffEndDistance */
	sprintf(hold,"%6.2f cM",OffEndDistance.e.v);
	PrintStatusTableLine("OffEndDistance",hold,"Value to write to control file");

	/* PEDIGREE DRAWING SETTINGS */
	PrintStatusTableLine("H",NULL,NULL);
	PrintStatusTableLine("DRAWING SETTINGS","","");
	PrintStatusTableLine("H",NULL,NULL);
	/* Color */
	PrintStatusTableLine("Color",g->d.InColor?"ON":"OFF",g->d.InColor?  "Draw pedigrees in color":"Draw pedigrees in black and white");
	/* ReverseShading */
	PrintStatusTableLine("ReverseShading",g->d.ReverseShading?"ON":"OFF",g->d.ReverseShading?  "White is first icon shade":"Black is first icon shade");

	/* DividedDrawings */
	PrintStatusTableLine("DividedDrawings",g->d.DividedDrawings?"ON":"OFF",g->d.DividedDrawings?"Paginate drawings by founding group":"--NOT YET IMPLEMENTED--");
	/* HighlightRows */
	PrintStatusTableLine("HighlightRows",g->HighlightRows?"ON":"OFF",g->HighlightRows?  "Alternately highlight data on drawings":"Do not highlight data on drawings");
	/* LabelCreatedIndividuals */
	PrintStatusTableLine("LabelCreatedIndividuals",g->LabelCreatedVirtuals?"ON":"OFF",g->LabelCreatedVirtuals?  "Label virtuals created by Madeline":"Omit labels on created virtuals");

	/* Orientation */
	switch(g->d.Orientation){
	case AUTOMATICORIENTATION:
		setting="AUTOMATIC";
		description="Automatic based on drawing dimensions";
		break;
	case PORTRAITORIENTATION:
		setting="PORTRAIT";
		description="Drawings will be scaled to portrait page";
		break;
	case LANDSCAPEORIENTATION:
		setting="LANDSCAPE";
		description="Drawings will be scaled to landscape page";
		break;
	case MULTIPAGEORIENTATION:
		setting="MULTIPAGE";
		description="Currently identical to AUTOMATIC";
		break;
	default:
		setting=description="unknown";
		break;
	}
	PrintStatusTableLine("Orientation",setting,description);
	/* PaperMargin */
	sprintf(hold,"%.2f cm",g->d.PageMargin);
	PrintStatusTableLine("PaperMargin",hold,"Margin (in cm) on all four sides");
	/* PaperSize */
	switch(g->d.PaperSize){
	case USLETTERSIZE:
		setting="USLETTER";
		description="8.5 x 11.0 inches";
		break;
	case USLEGALSIZE:
		setting="USLEGAL";
		description="8.5 x 14.0 inches";
		break;
	case A4SIZE:
		setting="A4";
		description="210 x 297 mm";
		break;
	case A4LONGSIZE:
		setting="A4LONG";
		description="210 x 339 mm";
		break;
	case A4SUPERSIZE:
		setting="A4SUPER";
		description="9.0 x 15.0 inches";
		break;
	default:
		setting=description="unknown";
		break;
	}
	PrintStatusTableLine("PaperSize",setting,description);

	/* OTHER SETTINGS */
	PrintStatusTableLine("H",NULL,NULL);
	PrintStatusTableLine("OTHER SETTINGS","","");
	PrintStatusTableLine("H",NULL,NULL);
	/* AutoExclude */
	PrintStatusTableLine("AutoExclude",g->AutoExclude?"ON":"OFF",g->AutoExclude?"Exclude pedigrees automatically":"Include all pedigrees");
	/* AutoCheckInheritance */
	PrintStatusTableLine("AutoCheckInheritance",g->AutoCheckInheritance?"ON":"OFF",g->AutoCheckInheritance?"Check inheritance on OPEN":"Check Inheritance using CHECK");
	/* ConsoleHighlights */
	PrintStatusTableLine("ConsoleHighlights",g->ConsoleHighlights?"ON":"OFF",g->ConsoleHighlights?"Use bold/color highlights on console":"Don't use bold/color highlights");
	/* Delimiter */
	switch(g->delimiter){
	case '\t':
		delimiter="TAB";
		break;
	case ',':
		delimiter="COMMA";
		break;
	case ' ':
		delimiter="SPACE";
		break;
	default:
		sprintf(hold,"%c",g->delimiter);
		delimiter=hold;
		break;
	}
	PrintStatusTableLine("Delimiter",delimiter,"Delimiter for tables and other output.");
	/* FusionSupport */
	PrintStatusTableLine("FusionSupport",g->FusionSupport?"ON":"OFF",g->FusionSupport?"FUSION customizations enabled":"FUSION customizations disabled");
	/* HaplotypeDisplay */
	PrintStatusTableLine("HaplotypeDisplay",g->Haplotype?"ON":"OFF",g->Haplotype?"Display genotypes delimited with \"|\"":"Display genotypes delimited with \"/\"");
	PrintStatusTableLine("Language",Language[g->lang]->EnglishName,"Language convention used for date, time");
	/* MapDetails */
	PrintStatusTableLine("MapDetails",g->MapDetails?"ON":"OFF",g->MapDetails?"LIST MAP detailed display":"LIST MAP summary display");
	/* SaveAlleleFrequencies */
	PrintStatusTableLine("SaveAlleleFrequencies",g->SaveMarkerInfo?"ON":"OFF",g->SaveMarkerInfo?"Reuse allele frequencies on next OPEN":"Calculate new frequencies on next OPEN");
	/* Time (current) */
	PrintStatusTableLine("Date","",Language[g->lang]->CurrentTime());
	/* Verbosity */
	if(g->Verbose){
		setting="VERBOSE";
		description="All messages are printed to the console";
	}else if(g->Silent){
		setting="SILENT";
		description="No messages are printed to the console";
	}else{
		setting="QUIET";
		description="Summary messages are printed to console";
	}
	PrintStatusTableLine("Verbosity",setting,description);

	/* Table footer: */
	PrintStatusTableLine("H",NULL,NULL);	
}


/*                                     */
/* PromptUser(): Prompt for a response */
/*                                     */
void PromptUser(void){

	RESULT result;
	
	/*                                               */
	/* Evaluate expression begins with a GetToken(): */
	/*                                               */
	EvaluateExpression(&result);
	if(result.t!=STRINGVALUE) MadelineErrorReport("PromptUser(): Prompt string expected.");
	lprintf("%s\n",result.s);
	
}



/*                          */
/* Banner(): program banner */
/*                          */
void Banner(void){

	/* This only looks like a mess because of the escaped backslashes; it prints correctly:  */
	lprintf(" ______________________________________________________________________________\n");
	lprintf(" ______________________________________________________________________________\n");
	lprintf("  __    __       _       ______     _______   _          _   __    _   _______ \n");
	lprintf(" |  \\  /  |     / \\     |  ___  \\  |  _____| | |        | | |  \\  | | |  _____|\n");
	lprintf(" |   \\/   |    / ^ \\    | |   \\  \\ | |___    | |        | | |   \\ | | | |___   \n");
	lprintf(" | |\\  /| |   / /_\\ \\   | |    | | |  ___|   | |        | | | |\\ \\| | |  ___|  \n");
	lprintf(" | | \\/ | |  /  ___  \\  | |___/  / | |_____  | |______  | | | | \\   | | |_____ \n");
	lprintf(" |_|    |_| /__/   \\__\\ |_______/  |_______| |________| |_| |_|  \\__| |_______|\n");
	lprintf(" ______________________________________________________________________________\n");
	lprintf(" ______________________________________________________________________________\n");	
	lprintf("\n                                 Version %6.3lf\n",VERSION);
	lprintf("                           Written by Edward H. Trager\n");
	lprintf("                              <ehtrager@umich.edu>\n\n");

	lprintf("COPYRIGHT © %4i\n",COPYRIGHTYEAR); 
	lprintf("THE REGENTS OF THE UNIVERSITY OF MICHIGAN\n");
	lprintf("PORTIONS COPYRIGHT © 1995 EDWARD H. TRAGER\n");
	lprintf("ALL RIGHTS RESERVED\n\n");

	lprintf("Madeline comes with ABSOLUTELY NO WARRANTY.  This is free software and\n");
	lprintf("you are welcome to redistribute it under certain conditions.  For details,\n");
	lprintf("type \"license\"\n\n");

}


/*                                                    */
/* License() prints Copyright and terse license info. */
/*                                                    */
void License(void){

	lprintf("\nMADELINE VERSION %6.3lf\n",VERSION);
	lprintf("WRITTEN BY EDWARD H. TRAGER <ehtrager@umich.edu>\n\n");

	lprintf("COPYRIGHT © %4i\n",COPYRIGHTYEAR); 
	lprintf("THE REGENTS OF THE UNIVERSITY OF MICHIGAN\n");
	lprintf("PORTIONS COPYRIGHT © 1995 EDWARD H. TRAGER\n");
	lprintf("ALL RIGHTS RESERVED\n\n");

	lprintf("MADELINE IS FREE SOFTWARE; YOU CAN REDISTRIBUTE IT AND/OR MODIFY IT UNDER THE\n");
	lprintf("TERMS OF THE GNU GENERAL PUBLIC LICENSE AS PUBLISHED BY THE FREE SOFTWARE\n");
	lprintf("FOUNDATION, SO LONG AS THE NAME OF THE UNIVERSITY OF MICHIGAN IS NOT USED IN\n");
	lprintf("ANY ADVERTISING OR PUBLICITY PERTAINING TO THE USE OR DISTRIBUTION OF THIS\n");
	lprintf("SOFTWARE WITHOUT SPECIFIC, WRITTEN PRIOR AUTHORIZATION.\n\n");

	lprintf("THIS SOFTWARE IS DISTRIBUTED IN THE HOPE THAT IT WILL BE USEFUL, BUT IS\n");
	lprintf("PROVIDED AS IS, WITHOUT REPRESENTATION FROM THE UNIVERSITY OF MICHIGAN AS TO\n");
	lprintf("ITS FITNESS FOR ANY PURPOSE, AND WITHOUT WARRANTY BY THE UNIVERSITY OF\n"); 
	lprintf("MICHIGAN OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION\n");
	lprintf("THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR\n"); 
	lprintf("PURPOSE. THE REGENTS OF THE UNIVERSITY OF MICHIGAN SHALL NOT BE LIABLE FOR\n");
	lprintf("ANY DAMAGES, INCLUDING SPECIAL, INDIRECT, INCIDENTAL, OR CONSEQUENTIAL\n"); 
	lprintf("DAMAGES, WITH RESPECT TO ANY CLAIM ARISING OUT OF OR IN CONNECTION WITH THE\n"); 
	lprintf("USE OF THE SOFTWARE, EVEN IF IT HAS BEEN OR IS HEREAFTER ADVISED OF THE\n"); 
	lprintf("POSSIBILITY OF SUCH DAMAGES.\n\n");

	lprintf("SEE THE GNU GENERAL PUBLIC LICENSE WHICH ACCOMPANIED THIS DISTRIBUTION\n");
	lprintf("FOR MORE DETAILS.\n");

}


/*                                                                 */
/* IsAGenotypeField()                                              */
/*                                                                 */
/* "Guesses" whether a field is a genotype field                   */
/* using simple criteria:                                          */
/*                                                                 */
/* Genotype fields - are character fields                          */
/*                 - contain two numbers separated by '/'          */
/*                                                                 */
/* IsAGenotypeField checks to make sure that a character field     */
/* does indeed contain nothing other than two numbers separated by */
/* a '/' slash character.                                          */
/*                                                                 */
/* NOTE BENE: RETURN VALUES:                                       */
/*                                                                 */
/* returns: 1 for a Genotype  field                                */
/*          0 for a Phenotype field (i.e., a non-Genotype field)   */
/*         -9 (MISSING) for a completely empty field with no data: */
/*                                                                 */
int IsAGenotypeField(PDBF db,int j){
	
	UINT32 i;
	char *o;
	/* char b[SMALLBUFFERSIZE]; */

	/*                                        */
	/* genotype field must be character type: */
	/*                                        */
	if(db->FieldType(db,j)!='C') return 0;

	for(i=0;i<db->rc;i++){
		/*               */
		/* read a value: */
		/*               */
		o=db->ReadString(db,NULL,j,i);
		/*                                           */
		/* skip missing values:                      */
		/*                                           */
		/*if(IsCMV(o)) continue;                     */
		/*                                           */
		/*if(!strcmp(o,CHARMISSINGSTRING)) continue; */
		/*                                           */
		if(!o[0]) continue;
		
		/*                                       */
		/* diagnose the first non-missing entry: */
		/*                                       */
		return ContainsAGenotype(o);
	}
	/*                                       */
	/* get here if the entire column         */
	/* contained only missing values:        */
	/* -- this is basically bad news,        */
	/* indicating a completely empty column: */
	/*                                       */
	return MISSING;
}


/*                                                       */
/* ContainsAGenotype: version simplified for speed       */
/* Genotype fields must have integral numeric labels:    */
/*                                                       */
/* RETURNS:  1 if string contains number/number genotype */
/*           0 otherwise                                 */
/*                                                       */
int ContainsAGenotype(char *s){
	
	int sc=0; /* slash count */

	/*                          */
	/* skip initial whitespace: */
	/*                          */
	for(;*s && (*s==' ' || *s=='\t');s++);
	if(!*s)              return 0;
	/*                       */
	/* traverse first label: */
	/*                       */
	for(;*s && isdigit(*s);s++);
	/*                                                                 */
	/* only one numeric label, no slash, or else some other disallowed */
	/* character:                                                      */
	/*                                                                 */
	if(!*s)              return 0;

	/*                                                                   */
	/* skip middle section of white space, counting slashes, if present: */
	/*                                                                   */
	for(;*s && (*s==' ' || *s=='\t' || *s=='/');s++) if(*s=='/') sc++;
	/*                         */
	/* only one slash allowed: */
	/*                         */
	if(sc!=1)            return 0;
	if(!*s)              return 0;
	/*                                       */
	/* second numeric label is now expected: */
	/*                                       */
	for(;*s && isdigit(*s);s++);

	/*                            */
	/* skip terminal white space: */
	/*                            */
	for(;*s && (*s==' ' || *s=='\t');s++);
	/*                                    */
	/* there should not be anything else: */
	/*                                    */
	if(*s)               return 0;
	/*                     */
	/* otherwise, it's OK: */
	/*                     */
	return 1;
}



/*                                                                                    */
/*                                                                                    */
/* Toggle(): toggle field flags on/off:                                               */
/*                                                                                    */
/* -"o" output flag status is independent of phenotype, genotype, or covariate status */
/* -"P" phenotype and "G" genotype are mutually exclusive                             */
/* -"V" covariate is a subtype of "P" phenotype, and so the two flags can coexist     */
/* -"P" phenotype does not have to be "V" covariate, but you can't have "V" without   */
/*      also having the "P" flag turned on                                            */
/* -"I" icon flag can be toggled on for any numeric "C" core or "P" phenotype field   */
/*      that can be treated as a categorical variable.  If ICONSET flags are TOGGLED, */
/*      then the program calls RemoveFieldTrees() and BuildFieldTrees():              */
/*                                                                                    */
void Toggle(void){
	
	int i,stt,end;
	int flag;
	int CoreToggled;
	int force_on=0;
	int force_off=0;
	
	PTOKENINFO ptt[4];
	
	extern PCONTROL g;
	
	RESULT result;
	
	if(!g->db) MadelineErrorReport("Toggle(): You must open a pedigree table before you can use this command.");
	
	/*                                                                       */
	/* The first keyword is an optional ON or OFF directive:                 */
	/*                                                                       */
	GetToken();
	flag=pT->i;
	if(flag==ONK || flag==OFFK){
		
		if(flag==ONK) force_on++;
		else          force_off++;
		
		GetToken();
		flag=pT->i;
		
		if(flag!=OUTPK) MadelineErrorReport("Toggle(): The optional keywords \"ON\" and \"OFF\" can only be used with \"OUTPUT FLAGS\"");
		
	}
	
	/*                                                                       */
	/* First keyword should be COVARIATE,GENOTYPE,PHENOTYPE,OUTPUT, or ICON: */
	/*                                                                       */
	if(!(flag==COVAK || flag==GENEK || flag==PHENK || flag==OUTPK || flag==ICONK ))
		MadelineErrorReport("Toggle(): \"PHENOTYPE\", \"COVARIATE\", \"GENOTYPE\", \"OUTPUT\" or \"ICON\" keyword expected:\n\tTOGGLE [PHENOTYPE|COVARIATE|GENOTYPE|OUTPUT|ICON] FLAG\n\tFOR field_n,field_m,...");
	/*                                   */
	/* translate flag into bitflag mask: */
	/*                                   */
	if     (flag==COVAK) flag=COVARIATESET;
	else if(flag==GENEK) flag=GENOTYPESET;
	else if(flag==PHENK) flag=PHENOTYPESET;
	else if(flag==OUTPK) flag=OUTPUTSET;
	else if(flag==ICONK) flag=ICONSET;
	
	
	GetToken();
	if(pT->i!=FLAGK)
		MadelineErrorReport("Toggle(): \"FLAG\" keyword expected to follow the flag type");
	GetToken();
	if(pT->i!=FORK)
		MadelineErrorReport("Toggle(): \"FOR\" keyword expected to begin list of fields to toggle");

	do{
	
		end=MISSING;
		stt=MISSING;
		/*                                                             */
		/* Get field index numbers or field names separated by commas  */
		/* or possibly consisting of a series, like 21-25. Get no more */
		/* than three tokens needed for a series                       */
		/*                                                             */
		i=0;
		do{
			GetToken();
			/*                                                        */
			/* 2003.02.24.ET ADDENDUM: Allow a new command option:    */
			/*                                                        */
			/* "TOGGLE ON|OFF OUTPUT FLAGS FOR CHROMOSOME 12 MARKERS" */
			/*                                 *********************  */
			/*                                                        */
			if(pT->i==CHRMK){
				/* Make sure that a map table has been loaded: */
				if(!g->mp) MadelineErrorReport("Toggle(): a map file must be loaded in order to toggle marker fields by chromosome number.");
				/* Get the Chromosome number: */
				EvaluateExpression(&result);
				if(result.t!=NUMERICVALUE){ 
					MadelineErrorReport("Toggle(): a numeric chromosome number was expected after the keyword CHROMOSOME:\n\tTOGGLE OUTPUT FLAGS ON|OFF FOR CHROMOSOME <n> MARKERS");
				}
				
				if(pT->i!=';' && pT->i!=MARKK){
					MadelineErrorReport("Toggle(): The keyword \"MARKERS\" was expected after the chromosome number:\n\tTOGGLE OUTPUT FLAGS ON|OFF FOR CHROMOSOME <n> MARKERS");
				}
				/*                                    */
				/* Call function to handle this case: */
				/* in togglebychromosome.c:           */
				/*                                    */
				ToggleMarkersByChromosome(force_on,(int) result.v);
				/* Finish up by jumping to the FinishUp label below: */
				goto FinishUp;
			}
			/*                                                         */
			/* 2003.03.04.ET ADDENDUM: Allow a new command option:     */
			/*                                                         */
			/* TOGGLE ON|OFF OUTPUT FLAGS FOR _IsMendelianInconsistent */
			/*                            **************************** */
			/*                                                         */
			if(pT->i==INMDL){
				/*                                    */
				/* Call function to handle this case: */
				/* in togglebychromosome.c:           */
				/*                                    */
				GetToken();
				if(pT->i==ONK){ 
					GetToken();
					if(pT->i==CHRMK) EvaluateExpression(&result);
					if(result.t!=NUMERICVALUE){
						MadelineErrorReport("Toggle(): a numeric chromosome number was expected after the keyword CHROMOSOME:\n\tTOGGLE ON|OFF OUTPUT FLAGS FOR _IsMendelianInconsistent [ON CHROMOSOME <n>]");
					}else{
						ToggleMendelianInconsistentMarkersByChromosome(force_on,(int) result.v);
					}
				}else{
					ToggleMendelianInconsistentMarkers(force_on);
				}
				goto FinishUp;
			}
			
			ptt[i++]=pT;
		}while(i<4 && pT->i!=END && pT->i!=',');
		i--;
		if(i==1){

			/*                              */
			/* Single field or index number */
			/* followed by END or a comma:  */
			/*                              */
			if(ptt[0]->t==NUMERICVARIABLE || 
			   ptt[0]->t==STRINGVARIABLE  ||
			   ptt[0]->t==DATEVARIABLE
			){
				/*                             */
				/* Numeric, character, or date */
				/* field name seen:            */
				/*                             */
				end=stt=ptt[0]->f;
			}else if(ptt[0]->t==NUMERICVALUE){      
				/*                             */
				/* literal integer value seen: */
				/*                             */
				end=stt=(int)ptt[0]->v-1;
			}else
				MadelineErrorReport("Toggle(): field name or field index expected");

		}else if(i==3 && ptt[1]->i=='-'){
			/*                                       */
			/* series specified with dash separator: */
			/*                                       */
			
			/*                        */
			/* first field in series: */
			/*                        */
			if(ptt[0]->t==NUMERICVARIABLE || ptt[0]->t==STRINGVARIABLE || ptt[0]->t==DATEVARIABLE ){
				/*                  */
				/* field name seen: */
				/*                  */
				stt=ptt[0]->f;
			}else if(ptt[0]->t==NUMERICVALUE){      
				/*                             */
				/* literal integer value seen: */
				/*                             */
				stt=(int)ptt[0]->v-1;
			}else
				MadelineErrorReport("Toggle(): field name or field index expected");
			
			/*                       */
			/* last field in series: */
			/*                       */
			if(ptt[2]->t==NUMERICVARIABLE || ptt[2]->t==STRINGVARIABLE || ptt[2]->t==DATEVARIABLE ){
				/*                  */
				/* field name seen: */
				/*                  */
				end=ptt[2]->f;
			}else if(ptt[2]->t==NUMERICVALUE){      
				/*                             */
				/* literal integer value seen: */
				/*                             */
				end=(int)ptt[2]->v-1;
			}else
				MadelineErrorReport("Toggle(): field name or field index expected");
			/*                          */
			/* error checking on range: */
			/*                          */
			if(end<stt)
				MadelineErrorReport("Toggle(): specify an increasing range like 3-5,7-9, etc.");
		}else
			MadelineErrorReport("Toggle(): specify field names or indices separated by commas or dashes");
		/*                   */
		/* toggle the flags: */
		/*                   */
		CoreToggled=0;
		
		for(i=stt;i<=end;i++){
			/*                                                          */
			/* Rules:                                                   */
			/*                                                          */
			/* (1) Core variables can't be changed to be non-core.      */
			/*     However, core variables can still be turned on       */
			/*     or off -- this elicits a message because the program */
			/*     controls output of core fields most of the time.     */
			/*                                                          */
			/*     19990325 ET ADDENDUM: Core variables can also have   */
			/*     their ICON flag toggled on or off                    */
			/*                                                          */
			/* (2) Can't have overlaps between                          */
			/*     covariate,genotype, and phenotype flags:             */
			/*     these are mutually exclusive                         */
			/*                                                          */
			if(g->db->ff[i]&KNOWNSET && !(flag==OUTPUTSET || flag==ICONSET)){
				MadelineErrorReport("Toggle(): %s is a core field and cannot be toggled\nto another field type.",
				g->db->FieldName(g->db,i));
			}else{
				
				//fprintf(stdout,"%i. OFF:%i ON:%i CURRENT:%i\n",i,force_off,force_on,g->db->ff[i]&OUTPUTSET);
				
				if     (force_off){
					
					if(g->db->ff[i]&OUTPUTSET) g->db->ff[i]^=flag;
					
				}else if(force_on){
					
					if(!(g->db->ff[i]&OUTPUTSET)) g->db->ff[i]^=flag;
					
				}else{
					                            g->db->ff[i]^=flag;
					
				}
			}
			/*                                                  */
			/* message to user when turning core fields on/off: */
			/*                                                  */
			if(g->db->ff[i]&KNOWNSET){
				CoreToggled++;
			}

			if(flag==COVARIATESET && g->db->ff[i]&COVARIATESET){
				/*                                      */
				/* covariate flag turned on, so:        */
				/* - turn off genotype flag if its on   */
				/* - turn on  phenotype flag if its off */
				/*                                      */
				if(g->db->ff[i] & GENOTYPESET ) g->db->ff[i]^=GENOTYPESET;
				if(!(g->db->ff[i] & PHENOTYPESET)) g->db->ff[i]^=PHENOTYPESET;

			}else if(flag==GENOTYPESET && g->db->ff[i]&GENOTYPESET){
				/*                                                       */
				/* genotype flag turned on, so can't have "P" or "V" on: */
				/*                                                       */
				if(g->db->ff[i] & COVARIATESET) g->db->ff[i]^=COVARIATESET;
				if(g->db->ff[i] & PHENOTYPESET) g->db->ff[i]^=PHENOTYPESET;                              

			}else if(flag==PHENOTYPESET){

				if(g->db->ff[i]&PHENOTYPESET){
					/*                                                            */
					/* phenotype flag turned on, so can't have geno or covariate: */
					/*                                                            */
					if(g->db->ff[i] & GENOTYPESET ) g->db->ff[i]^=GENOTYPESET;
					if(g->db->ff[i] & COVARIATESET) g->db->ff[i]^=COVARIATESET;
				}else{
					/*                                                                 */
					/* phenotype flag just turned off, so force all "V" flags off too: */
					/*                                                                 */
					if(g->db->ff[i] & COVARIATESET) g->db->ff[i]^=COVARIATESET;
				}

			}
			
		}
		
		
	}while(pT->i==',');


	/*                                                                              */
	/* If ICONSET flags were toggled, first remove field trees from any fields      */
	/* that no longer have the flag, then build field trees for newly flagged       */
	/* fields that don't yet have trees:  This is all handled by ManageFieldTrees() */
	/*                                                                              */
	if(flag==ICONSET){
		ManageFieldTrees(g->db);
		ManageFieldLevelsLabelsArrays(knownTokens,g->db);
	}
	
	/*                                          */
	/* Note to user about toggling core fields: */
	/*                                          */
	if(CoreToggled){
		lprintf("\nNOTE: Core fields will be included in output if required by a\n");
		lprintf("specific \"write\" format regardless of toggle status.  The \n");
		lprintf("\"draw\" command, in contrast, will respect the toggle settings\n");
		lprintf("for core fields.\n");
	}
	
	/*                                       */
	/* Yes, here is a real, live GOTO label: */
	/*                                       */
	FinishUp:
	
	/*                          */
	/* Recount the field types: */
	/*                          */
	/*                          */
	CountFieldFlags(g->db);

	/*                                                  */
	/* Reset the field ordering after a toggle command: */
	/*                                                  */
	AutoSetFieldOrdering(g,g->db);

	/*                                                              */
	/* Reset the genotype counts based on all of the marker fields: */
	/*                                                              */
	SetGenotypeCountAttributes(g->pd);

	/*                                                                        */
	/* Using Toggle() will often change the set of output fields,             */
	/* making the pedigree drawing definitions in gDrawing obsolete, so       */
	/* remove the obsolete gDrawing object.  Draw() checks if gDrawing==NULL  */
	/* and defines a new object then:                                         */
	/*                                                                        */
	if(gDrawing){
		FreeDrawing(gDrawing);
		gDrawing=NULL;
	}       
}


/*                                                                     */
/* Help(): print help information on screen and in the logs.           */
/*                                                                     */
/* April, 1999:  The help facility was completely redone, resulting in */
/* a great improvement over the original.  -- ET                       */
/*                                                                     */
/* 2000 05 09                                                          */
/*                                                                     */
void Help(void){

	RESULT result;
	
	int len;
	char *ss;
	char hold[MINIMUMBUFFERSIZE];
	char *args[3];
	
	/*                                               */
	/* Evaluate expression begins with a GetToken(): */
	/*                                               */
	EvaluateExpression(&result);
	if(result.t!=STRINGVALUE)
		MadelineErrorReport("Help(): search string expected.");

	len=strlen(result.s);

	/* if(g->HtmlHelp){ -- DEPRECATED 2000 05 09 ET: HELP is now */
	/* always HTML-based help, while LOOKUP gives you the quick  */
	/* token-tree search.                                        */

	/*                                                          */
	/* convert to lower case for finding bookmark in HTML file: */
	/*                                                          */
	for(ss=result.s;*ss;ss++) *ss=tolower(*ss);
	/*                                        */
	/* set up call to world wide web browser: */
	/*                                        */
	
	/* First, let's check if a local copy of the documentation */
	/* is available in the default system location:            */
	/*                                                         */
	if(g->MadelineSystem){
		/*                                      */
		/* Use local mirror of documentation    */
		/* in Madeline system directory, "docs" */
		/* subdirectory:                        */
		/*                                      */
		strcpy(hold,"file://");
		strcat(hold,g->MadelineSystem);
		strcat(hold,DIRECTORYSEPARATOR);
		strcat(hold,"documentation");
		strcat(hold,DIRECTORYSEPARATOR);
		strcat(hold,"madeline.html");
	}else{
		strcpy(hold,"http://");
		strcat(hold,gWebAddress.e.s);
	}
	/*                        */
	/* Add specific bookmark: */
	/*                        */
	strcat(hold,"#");
	strcat(hold,result.s);

	/*               */
	/* call URL viewing application: */
	/*               */
	args[0]=gWebViewerName.e.s;
	args[1]=hold;
	args[2]=NULL;
	/* Tell the user what's going on, just in case the browser */
	/* loads slowly or not at all ...                          */
	lprintf("Calling %s with \"%s\" ...\n",gWebViewerName.e.s,hold);
	lprintf("(If you don't find what you want in the HTML documentation, try the LOOKUP command)\n");
	
	CreateChildProcess(gWebViewerName.e.s,args);
}


/*                                                                     */
/* Lookup(): Lookup tokens in the token tree - the original text-based */
/* help system is still very useful when you just can't quite remember */
/* which Madeline command or keyword to use, or how to spell it        */
/*                                                                     */
void Lookup(void){

	RESULT result;
	
	int len;
	char *ss;
	
	/*                                               */
	/* Evaluate expression begins with a GetToken(): */
	/*                                               */
	EvaluateExpression(&result);
	if(result.t!=STRINGVALUE)
		MadelineErrorReport("Lookup(): search string expected.\nBe sure to enclose search string in single or double quotes.");

	len=strlen(result.s);
	/*                                                         */
	/* convert to upper case in order to search in token tree: */
	/*                                                         */
	for(ss=result.s;*ss;ss++) *ss=toupper(*ss);
	/*                                        */
	/* Call recursive function to print help: */
	/*                                        */
	
	// knownTokens is currently global:
	knownTokens.printHelp(result.s,len);
	
}

/*                                                                     */
/* ReadCoreInformation(): read the core data from the database, db,    */
/* and construct a binary tree,pd, of families and individuals         */
/* within each family.                                                 */
/*                                                                     */
/* Note that a PEDIGREE contains a pointer to an INDIVIDUAL tree.      */
/* Pedigrees are stored in red-black binary search trees consisting of */
/* *PEDIGREE nodes, and each PEDIGREE node itself contains a tree      */
/* consisting of *INDIVIDUAL nodes.                                    */
/*                                                                     */
PPEDIGREE ReadCoreInformation(PPEDIGREE pd,PDBF db){

	UINT32 i;
	
	int HasMZTwinField;
	int HasCharacterMZTwinField;
	int HasCharacterDZTwinField;
	
	int HasNumericClassField;

	/* AFFECTION STATUS FIELD */
	int HasCharacterAffectedField;
	int HasNumericAffectedField;
	/* GENDER STATUS FIELD */
	int HasCharacterSexField;
	/* DEATH STATUS FIELD */
	int HasCharacterDeathStatusField;
	int HasNumericDeathStatusField;
	/* PROBAND FIELD */
	int HasCharacterProbandField;
	int HasNumericProbandField;
	/* LIABILITY CLASS FIELD */
	int HasCharacterLiabilityClassField;
	int HasNumericLiabilityClassField;

	int HasFamIDField;
	
	int HasDOBField;
	int HasDODField;
	
	/* clock_t stt,end; */
	char hold1[MAXIDWIDTH]; 
	char hold2[MAXIDWIDTH];
	char hold3[MAXIDWIDTH];
	char hold4[MAXIDWIDTH]; 
	char *piid;				/* pointer to individual id */
	char *pfid;				/* pointer to father id     */
	char *pmid;				/* pointer to mother id     */
	char *pfamid;				/* pointer to family id     */
	char *o;				/* generic char pointer     */
	
	char sex[MAXSHORTSTRINGCODEWIDTH];         /* sex field entry as 'M' or 'F'     */
	char dead[MAXSHORTSTRINGCODEWIDTH];        /* dead field entry as 'Y','N', etc. */
	char hold[MINIMUMBUFFERSIZE]; /* for reading anything else         */

	int  isex;
	int  idead=MISSING;

	PINDIVIDUAL p;  /* pointer to the individual */

	extern PCONTROL g;

	
	/*                                             */
	/* Emit errors if core fields are not present: */
	/*                                             */
	if(gfnID.i==MISSING){
		FreeDatabase(db);
		if(pd) FreeBinaryPedigreeTree(pd);
		g->db=NULL;
		g->pd=NULL;
		MadelineErrorReport("ReadCoreInformation(): The IndividualIDField \"%s\" is not present in the table!",gfnID.e.s);
	}
	if(gfnFather.i==MISSING){
		FreeDatabase(db);
		if(pd) FreeBinaryPedigreeTree(pd);
		g->db=NULL;
		g->pd=NULL;
		MadelineErrorReport("ReadCoreInformation(): The FatherIDField \"%s\" is not present in the table!",gfnFather.e.s);
	}
	if(gfnMother.i==MISSING){
		FreeDatabase(db);
		if(pd) FreeBinaryPedigreeTree(pd);
		g->db=NULL;
		g->pd=NULL;
		MadelineErrorReport("ReadCoreInformation(): The MotherIDField \"%s\" is not present in the table!",gfnMother.e.s);
	}
	if(gfnSex.i==MISSING){
		FreeDatabase(db);
		if(pd) FreeBinaryPedigreeTree(pd);
		g->db=NULL;
		g->pd=NULL;
		MadelineErrorReport("ReadCoreInformation(): The GenderField \"%s\" is not present in the table!",gfnSex.e.s);
	}
	/*                                             */
	/* 2004.07.29.ET ADDENDUM:                     */
	/*                                             */
	/* Since it is convenient to be able to draw   */
	/* pedigree drawings even when you don't know  */
	/* the affection status, we let the user get   */
	/* away without having AffectionStatus.  But   */
	/* we still consider it a potential problem,   */
	/* and hence produce a warning message:        */
	/*                                             */
	if(gfnAffected.i==MISSING)
		eprintf(WARNINGFLAG,"ReadCoreInformation(): The AffectionStatusField, \"%s\" is not present in the table!\n",
		gfnAffected.e.s);
	
	/*                                                         */
	/* if pd is not null (because a previous table was opened) */
	/* free the current pedigree tree:                         */
	/*                                                         */
	if(pd){
		FreeBinaryPedigreeTree(pd);
		pd=NULL;
	}
	
	/*                                                */
	/* Read individual information from the database: */
	/*                                                */
	
	/* stt=clock(); */

	/*                                    */
	/* determine field types before loop: */
	/*                                    */

	/* AFFECTION STATUS FIELD */
	HasNumericAffectedField = (gfnAffected.i!=MISSING && db->FieldType(db,gfnAffected.i)=='N');
	HasCharacterAffectedField=(gfnAffected.i!=MISSING && db->FieldType(db,gfnAffected.i)=='C');
	/* GENDER STATUS FIELD */
	HasCharacterSexField    = db->FieldType(db,gfnSex.i)=='C';
	/* DEATH STATUS FIELD */
	HasCharacterDeathStatusField=(gfnDead.i !=MISSING && db->FieldType(db,gfnDead.i)    =='C');
	HasNumericDeathStatusField  =(gfnDead.i !=MISSING && db->FieldType(db,gfnDead.i)    =='N');
	/* PROBAND FIELD */
	HasCharacterProbandField= (gfnProband.i !=MISSING && db->FieldType(db,gfnProband.i) =='C');
	HasNumericProbandField  = (gfnProband.i !=MISSING && db->FieldType(db,gfnProband.i) =='N');
	/* LIABILITY CLASS FIELD */
	HasCharacterLiabilityClassField= (gfnLClass.i !=MISSING && db->FieldType(db,gfnLClass.i) =='C');
	HasNumericLiabilityClassField  = (gfnLClass.i !=MISSING && db->FieldType(db,gfnLClass.i) =='N');
	/* MZTWIN AND DZTWIN FIELDS */
	HasMZTwinField          = (gfnMZTwin.i  !=MISSING);
	HasCharacterMZTwinField = (gfnMZTwin.i  !=MISSING && db->FieldType(db,gfnMZTwin.i)  =='C');
	HasCharacterDZTwinField = (gfnDZTwin.i  !=MISSING && db->FieldType(db,gfnDZTwin.i)  =='C');
	/* CORE FAMILY ID AND INDIVIDUAL ID FIELDS */
	HasNumericClassField    = (gfnClass.i   !=MISSING && db->FieldType(db,gfnClass.i)   =='N');
	HasFamIDField           = (gfnFamID.i   !=MISSING && db->FieldType(db,gfnFamID.i)   =='C');
	/* DATE OF BIRTH AND DATE OF DEATH FIELDS */
	HasDOBField             = (gfnDOB.i !=MISSING && db->FieldType(db,gfnDOB.i)         =='D');
	HasDODField             = (gfnDOD.i !=MISSING && db->FieldType(db,gfnDOD.i)         =='D');
	
	/*                                   */
	/* loop through records in database: */
	/*                                   */
	for(i=0;i<db->rc;i++){

		/*                                                          */
		/* get individual's id plus father's and mother's ids:      */
		/*                                                          */
		piid=db->ReadString(db,hold1,gfnID.i,i);
		pfid=db->ReadString(db,hold2,gfnFather.i,i);
		pmid=db->ReadString(db,hold3,gfnMother.i,i);

		/* GENDER STATUS */
		if(HasCharacterSexField){
			isex=ReadCharacterGenderStatus(db->ReadString(db,sex,gfnSex.i,i));                   
		}else{
			isex=ReadNumericGenderStatus(db->ReadInt32(db,gfnSex.i,i));
		}
		/* DEATH STATUS */
		if      (HasCharacterDeathStatusField){
			idead=ReadCharacterDeathStatus(db->ReadString(db,dead,gfnDead.i,i));                   
		}else if(HasNumericDeathStatusField){
			idead=ReadNumericDeathStatus(db->ReadInt32(db,gfnDead.i,i));
		}

		/*                                                               */
		/* family id: read a substring of id if famid field not present: */
		/*                                                               */
		if(HasFamIDField)
			pfamid=db->ReadString(db,hold4,gfnFamID.i,i);
		else{
			strncpy(hold4,piid,4);
			hold4[4]='\0'; /* null terminate family id */
			pfamid=hold4;
		}
		/*                                  */
		/* Add this person to the pedigree: */
		/* Add this person to the pedigree: */
		/* Add this person to the pedigree: */
		/*                                  */
		if(!pd) pd=BinaryPedigreeTreeHeadNode(pfamid,piid,db,i,&p);
		else    pd=AddIndividualToBinaryPedigreeTree(pd,pfamid,piid,db,i,&p,1);
		
		/*                                                */
		/* fill-in remaining information for this person: */
		/*                                                */
		strcpy(p->fid,pfid);
		strcpy(p->mid,pmid);
		p->female=isex;         /* sex assignment          */
		p->dead  =idead;        /* death status assignment */

		/*                                                        */
		/* Read additional information from core known field set: */
		/*                                                        */
		/*                                                        */
		

		/*                   */
		/* AFFECTION STATUS: */
		/*                   */
		if(HasNumericAffectedField){
			p->affected=ReadNumericAffectionStatus(db->ReadDouble(db,gfnAffected.i,p->rci));
		}else if(HasCharacterAffectedField){
			p->affected=ReadCharacterAffectionStatus(db->ReadString(db,hold,gfnAffected.i,p->rci));
		}
		/*                             */
		/* PROBAND: if class==PROBAND: */
		/*                             */
		if(HasNumericProbandField){
			/*                                      */
			/* ...read directly from proband field: */
			/*                                      */
			p->proband=ReadNumericProbandStatus(db->ReadDouble(db,gfnProband.i,p->rci));
			
		}else if(HasCharacterProbandField){
			
			p->proband=ReadCharacterProbandStatus(db->ReadString(db,hold,gfnProband.i,p->rci));
			
		}else if(g->FusionSupport && p->clas==PROBAND){
			/*                                           */
			/* Notice that we have to have FusionSupport */
			/* turned on to get here:                    */
			/*                                                  */
			/* assume FUSION type of id:                        */
			/*                                                  */
			/*                                                  */
			/* Well, a proband's just a sibling like any other: */
			/*                                                  */
			p->clas=SIBLING;
			/*                                  */
			/* here's where we'll mark proband: */
			/*                                  */
			p->proband=TRUE;
		}
		/*                   */
		/* LIABILITY CLASS:  */
		/*                   */
		if(HasNumericLiabilityClassField){
			p->lclass=ReadNumericLiabilityClassStatus(db->ReadDouble(db,gfnLClass.i,p->rci));
		}else if(HasCharacterLiabilityClassField){
			p->lclass=ReadCharacterLiabilityClassStatus(db->ReadString(db,hold,gfnLClass.i,p->rci));
		}
		
		/*                                                   */
		/* MZTWIN: currently assumed to be a character field */
		/*                                                   */
		if(HasMZTwinField){
			if(HasCharacterMZTwinField){
				o=db->ReadString(p->db,hold,gfnMZTwin.i,p->rci);
				/*                                                      */
				/* of course '\0' is equivalent to 0 and boolean false, */
				/* and will be explicitly used as such too:             */
				/*                                                      */
				/* if(IsCMV(hold)){                                     */
				/*                                                      */
				/* if(!strcmp(hold,CHARMISSINGSTRING)){                 */
				/*                                                      */
				if(!o[0]){
					 p->twin='\0';
				}else{
					p->twin=o[0];
					/*                                       */
					/* MONOZYGOTIC TWINS MUST BE UPPER CASE: */
					/*                                       */
					if(!isupper(p->twin)) p->twin=toupper(p->twin);
				}
			}
			/*                                   */
			/* Case of Numeric twin field ???			 */
			/*                                   */
		}else{
			/*                                 */
			/* There is NO MZTwinField at all: */
			/*                                 */
			p->twin='\0';
		}
		
		/*                                                   */
		/* DZTWIN: currently assumed to be a character field */
		/*                                                   */
		if(HasCharacterDZTwinField){
			o=db->ReadString(p->db,hold,gfnDZTwin.i,p->rci);
			/*                                                      */
			/* of course '\0' is equivalent to 0 and boolean false, */
			/* and will be explicitly used as such too:             */
			/*                                                      */
			/* if(!IsCMV(hold)):                                    */
			/*                                                      */
			/*if(strcmp(hold,CHARMISSINGSTRING)){                   */
			/*                                                      */
			
			if(o[0]){

				p->twin=o[0];
				/*                                        */
				/* DIZYGOTIC TWINS MUST BE IN LOWER CASE: */
				/*                                        */
				if(!islower(p->twin)) p->twin=tolower(p->twin);
			}
		}
		
		
		/*                                                      */
		/* CLASS: pull class info from individual id:           */
		/* -- note that this way there is no need to read class */
		/*    from a separate source field                      */
		/*                                                      */
		if(HasNumericClassField)
			/*                           */
			/* ...read from class field: */
			/*                           */
			p->clas=ReadFlagField(p->db,gfnClass.i,p->rci);
		else
			/*                                                        */
			/* ...otherwise, figure out the class if its a FUSION ID: */
			/*                                                        */
			
			p->clas=IsFusionID(p->id)?p->id[5]-'0':0;
			
		
		
		/*                */
		/* DATE OF BIRTH: */
		/*                */
		if(HasDOBField){
			p->dob=db->ReadDate(db,gfnDOB.i,p->rci);
		}
		
		/*                */
		/* DATE OF DEATH: */
		/*                */
		if(HasDODField){
			p->dod=db->ReadDate(db,gfnDOD.i,p->rci);
			/*                */
			/* check p->dead: */
			/*                */
			if(p->dod!=MISSINGDATE && p->dead!=TRUE) p->dead=TRUE;
			/*                                   */
			/* make sure death later than birth: */
			/*                                   */
			if(p->dob!=MISSINGDATE && p->dod!=MISSINGDATE && p->dob>p->dod){
				/* Warn on people born after they died -- that's scary! */
				eprintf(WARNINGFLAG,"ReadCoreInformation(): %s in pedigree %s was born on %s but died on %s!",
					p->id,
					p->pd->fid,
					DelimitedGenericDateString(p->dob,'.'),
					DelimitedGenericDateString(p->dod,'.')
				);
			}
		}
		
		/*            */
		/* That's it! */
		/*            */
	}

	/* end=clock(); */
	
	/*                                   */
	/* Display elapsed time information: */
	/*                                   */
	/* lprintf("Core information read in %g seconds\n",(double)(end-stt)/(double)CLOCKS_PER_SEC); */
	
	return pd;
}



/*                                   */
/*                                   */
/* Draw() command:                   */
/*                                   */
/* SYNTAX: DRAW PEDIGREE ID1,IDN-IDZ */
/*                                   */
void Draw(void){

	int i;
	
	PPEDIGREE pd;

	PTOKENINFO ptt[4];
	char *stt,*end;

	char *args[3];
	
	extern PCONTROL g;


		
	/*                                   */
	/* First keyword should be PEDIGREE: */
	/*                                   */
	GetToken();
	if(pT->i!=PEDIK)
		MadelineErrorReport("Draw(): \"PEDIGREE(S)\" keyword expected.");

	/*                                 */
	/* Database must have been opened: */
	/*                                 */
	if(!g->db)
		MadelineErrorReport("Draw(): No pedigree table is open.");
	
	/*                                        */
	/* Assign data to global gDrawing object: */
	/*                                        */
	if(!gDrawing){
		gDrawing=NewPedigreeDrawing(1);
		/* gDrawing->x=1.5+gDrawing->xex/2; */
		
		/*                                                        */
		/* New drawing coordinates which assume later translation */
		/* for correct centering on the page:                     */
		/*                                                        */
		gDrawing->x=0.0;
		gDrawing->y=-2.0;
		
		switch(g->d.Orientation){
		case LANDSCAPEORIENTATION:
		case AUTOMATICORIENTATION:
		case MULTIPAGEORIENTATION:
		/*	gDrawing->y=18.5; */
			break;
		case PORTRAITORIENTATION:
		/*	gDrawing->y=24.5; */
			break;
		}

		/*                                                          */
		/* Assign the title cell: the null pointer will be replaced */
		/* in DrawPedigree:                                         */
		/*                                                          */
		gDrawing->Title=NewAssignedCell(CM,0.0,0.0,5.0,1.5,0.1,
		      NORMLINE,NORMLINE,NORMLINE,NORMLINE,
		      TCWHITE,55,CENTER,ARIAL,
		      3,"",BOLD,16,"",BOLD,10,"",BOLD,10);
		/*                            */
		/* Assign data to the legend: */
		/*                            */
		gDrawing->Legend=NewPedigreeLegendCell(NORMAL,(int)LegendFontSize.e.v);
		/*                      */
		/* Assign drawing data: */
		/*                      */
		gDrawing->Individual=NewPedigreeDrawingCell(NORMAL,(int)LabelFontSize.e.v);
	}
	
	/*                            */
	/* Reset page number to zero: */
	/*                            */
	gDrawing->page=0;
	
	/*                                              */
	/* Open print device (i.e., the "DrawingFile"): */
	/*                                              */
	g->streams.prn=fopen(gPedigreeDrawing.e.s,"wb");
	if(!g->streams.prn) MadelineErrorReport("Draw(): unable to open %s for drawing.",gPedigreeDrawing.e.s);
	/*                                                                */
	/* Download Madeline's core PostScript pedigree drawing routines: */
	/*                                                                */
	SetPostScriptEnvironment(g,(int)LegendFontSize.e.v,(int)LabelFontSize.e.v);

	/*                                                       */
	/* loop through specified pedigrees and pedigree ranges: */
	/*                                                       */
	do{
		/*                                                                     */
		/* Get pedigree IDs separated by commas                                */
		/* or possibly consisting of a series, like "0021"-"0025". Get no more */
		/* than three tokens needed for a series                               */
		/*                                                                     */
		i=0;
		stt=NULL;
		end=NULL;
		do{
			GetToken();
			/*                                                        */
			/* If first token after "PEDIGREES" is the "FOR" keyword, */
			/* Then transfer processing to DrawFor() routine:         */
			/*                                                        */
			if(pT->i==FORK){
				DrawFor(g);
				return;
			}
			ptt[i++]=pT;
		}while(i<4 && pT->i!=END && pT->i!=',');
		i--;
		if(i==1){
			/*                             */
			/* Single pd->fid:             */
			/* followed by END or a comma: */
			/*                             */
			if(ptt[0]->t==STRINGVALUE){
				stt=end=ptt[0]->e;
			}else{
				fclose(g->streams.prn);			
				g->streams.prn=NULL;			
				MadelineErrorReport("Draw(): quoted string literal like \"0035\" expected");
			}

		}else if(i==3 && ptt[1]->i=='-'){
			/*                                       */
			/* series specified with dash separator: */
			/*                                       */
			
			/*                     */
			/* first id in series: */
			/*                     */
			if(ptt[0]->t==STRINGVALUE){
				stt=ptt[0]->e;                  
			}else{
				fclose(g->streams.prn);			
				g->streams.prn=NULL;			
				MadelineErrorReport("Draw(): quoted string literal like \"0035\" expected");
			}
			/*                    */
			/* last id in series: */
			/*                    */
			if(ptt[2]->t==STRINGVALUE){
				end=ptt[2]->e;
			}else{
				fclose(g->streams.prn);			
				g->streams.prn=NULL;			
				MadelineErrorReport("Draw(): quoted string literal like \"0035\" expected");
			}
			/*                          */
			/* error checking on range: */
			/*                          */
			if(strcmp(end,stt)<0)
				MadelineErrorReport("Draw(): specify an increasing range like \"0003\"-\"0009\"");
			/*                               */
			/* Draw pedigrees for the range: */
			/*                               */
		}else{
			fclose(g->streams.prn);			
			g->streams.prn=NULL;			
			MadelineErrorReport("Draw(): specify pedigree IDs in quotes separated by commas or dashes");
		}

		/*                */
		/* Process range: */
		/*                */
		
		
		if(stt==end){
			pd=SeekPedigree(g->pd,stt);
			if(pd){
				/*                       */
				/* Draw single pedigree: */
				/*                       */
				DrawPedigree(pd);
			}else{
				/*                     */
				/* close print device: */
				/*                     */
				fclose(g->streams.prn);
				g->streams.prn=NULL;			
				MadelineErrorReport("Draw(): pedigree \"%s\" not found",stt);
			}
		}else{
			if(!SeekPedigree(g->pd,stt)){
				/*                     */
				/* close print device: */
				/*                     */
				fclose(g->streams.prn);
				g->streams.prn=NULL;
				MadelineErrorReport("Draw(): pedigree \"%s\" not found",stt);
			}
			if(!SeekPedigree(g->pd,end)){
				/*                     */
				/* close print device: */
				/*                     */
				fclose(g->streams.prn);
				g->streams.prn=NULL;
				MadelineErrorReport("Draw(): pedigree \"%s\" not found",end);
			}
			TraverseRangeInBinaryPedigreeTree(g->pd,stt,end,DrawPedigree);
		}               

	}while(pT->i==',');     

	/*                     */
	/* close print device: */
	/*                     */
	fclose(g->streams.prn);
	g->streams.prn=NULL;
	/*              */
	/* open viewer: */
	/*              */
	if(gPSViewerName.e.s[0]){
	
		args[0]=gPSViewerName.e.s;
		args[1]=gPedigreeDrawing.e.s;
		args[2]=NULL;		
		CreateChildProcess(gPSViewerName.e.s,args);				
		
	}


}


/*              */
/* DrawPedigree */
/*              */
void DrawPedigree(PPEDIGREE pd){

	int i;
	char hold[MINIMUMBUFFERSIZE];
	
	switch(pd->nfm+pd->nff){
	case 0:
	case 1:
		/*                                                            */
		/* something is messed up if there are fewer than 2 founders: */
		/*                                                            */
		MadelineErrorReport("Can't draw pedigree %s: lacking %i founders",
			pd->fid,
			2-pd->nfm-pd->nff
		);
		break;
	default:
		/*                                 */
		/* One or multiple founding pairs: */
		/*                                 */
		for(i=0;i<pd->nfg;i++){
			/*                             */
			/* Tell user what's happening: */
			/*                             */
			dprintf("Drawing pedigree %s, %s's subtree (subtree %i of %i) ...\n",
				pd->fid,
				pd->fg[i].f->id,
				i+1,
				pd->nfg
			);

			/*                                            */
			/* put the second title line into the report: */
			/*                                            */
			sprintf(hold,"Page %i of %i",i+1,pd->nfg);
			gDrawing->Title->t[1].t=hold;
			/*                       */
			/* Call drawing routine: */
			/*                       */
			DrawSingleFoundingGroupPedigree(pd,i);

		}
		/*                                                     */
		/* NewPrintPedigreeDrawing rotates, translates, scales */
		/* and even prints to multiple pages, as required:     */
		/*                                                     */
		/*NewPrintPedigreeDrawing(g,pd); */
		
		break;
	}
	/*                                       */
	/* clear the p->drawn and p->page flags: */
	/*                                       */
	TraverseIndividualBST(pd->h,ClearIndividualDrawFlags);

}


/*                          */
/* ClearIndividualDrawFlags */
/*                          */
void ClearIndividualDrawFlags(PINDIVIDUAL p){

	if(p->drawn){
		p->page=p->drawn=0;
		p->x=p->y=0.0;
	}

}


/*                                                                         */
/* DrawSingleFoundingGroupPedigree():                                      */
/*                                                                         */
/* A single founding group consists of an ultimate ancestor and his or     */
/* her one or multiple spouses.  DrawSingleFoundingGroupPedigree() draws   */
/* the complete single founding group subtree originating with the founder */
/* of the ith founding group in the pedigree:                              */
/*                                                                         */
void DrawSingleFoundingGroupPedigree(PPEDIGREE pd,int i){


	char hold[MINIMUMBUFFERSIZE];
	char ProcName[MINIMUMBUFFERSIZE];
	double xorg;
	PINDIVIDUAL founder;
	/* double scaled; */
	
	extern PLANGUAGEOBJECT Language[LANGUAGE_COUNT];
	extern PCONTROL g;
	extern PPEDIGREEDRAWING gDrawing;
	
	if(!g->streams.prn) MadelineErrorReport("DrawPedigree(): printer not open");

	/*                                                 */
	/* Set pointer to founder:                         */
	/* This is done as a convenience for code clarity: */
	/*                                                 */
	founder=pd->fg[i].f;
				
	/*                                       */
	/* Supply the title in first title line: */
	/*                                       */
	sprintf(hold,"Pedigree %s",pd->fid);
	gDrawing->Title->t[0].t=hold;
	/*                                         */
	/* Supply the date as title 3rd subheader: */
	/*                                         */
	gDrawing->Title->t[2].t=Language[g->lang]->CurrentDate();
	
	/*                                                        */
	/* Increment page counter, then                           */
	/* set PostScript page environment with the drawing title */
	/* (in hold) and the PostScript page number:              */
	/*                                                        */
	gDrawing->page++;
	SetPostScriptPageEnvironment(g,hold,pd->fid,gDrawing->page);

	/*                                                                 */
	/* Set drawing up as a postscript procedure: procedure name        */
	/* is based on family id and page ordinal:                         */
	/*                                                                 */
	/* Note that drawing page is the same as the founding group number */

	/* OBSOLETE: sprintf(ProcName,"Pedigree%s_%05i",pd->fid,gDrawing->page); */

	/* Name the Postscript procedure after the pedigree family ID and the */
	/* founding group number:                                             */
	sprintf(ProcName,PS_PEDIGREE_PROC_NAME_FMT,pd->fid,i);

	/*                                        */
	/* Start Postscript procedure definition: */
	/*                                        */
	fprintf(g->streams.prn,"/%s {\n",ProcName);

	/*             */
	/* Draw title: */
	/*             */
	PrintIconCell(g->streams.prn,gDrawing->Title,NULL);
	/*                  */
	/* Draw the legend: */
	/*                  */
	PrintIconCell(g->streams.prn,gDrawing->Legend,NULL);
	/*                       */
	/* Draw the icon legend: */
	/*                       */
	PrintIconLegend(
		g->streams.prn,
		gDrawing->Legend->x+3.0,
		gDrawing->Legend->y,
		g->db
	);

	/*                     */
	/* Start with founder: */
	/*                     */
	if(founder->na>1){
		xorg=gDrawing->x-founder->nw*g->d.HalfHZInterval+IndividualLeftSubtreeWidth(founder);
	}else{
		xorg=gDrawing->x-g->d.HalfHZInterval;
	}
	
	/*                                                                            */
	/* DrawFamily() actually does most of the work of drawing the single founding */
	/* group tree:                                                                */
	/*	                                                                           */
	DrawFamily(g->streams.prn,founder,xorg,gDrawing->y,gDrawing->Individual,pd,i);
	/*                                      */
	/* End Postscript procedure definition: */
	/*                                      */
	fprintf(g->streams.prn,"}def\n");
	
	/*                                                  */
	/* PrintPedigreeDrawing rotates, translates, scales */
	/* and even prints to multiple pages, as required:  */
	/*                                                  */
	PrintPedigreeDrawing(g,pd,i,ProcName);
	
}


/*                          */
/* IndividualXOriginShift() */
/*                          */
double IndividualXOriginShift(PINDIVIDUAL p){

	int naleft;
	/* int nwleft; */
	int i;
	int sumleftnw;
	/* double halflxu; */

	extern PCONTROL g;
			
	switch(p->na){
	case 0:
		/*                                               */
		/* Individual has no spouse and no children,     */
		/* so draw him or her in place without shifting: */
		/*                                               */
		return 0.0;
	
	case 1:
		/*                                                   */
		/* Individual has one spouse and the original        */
		/* code draws one spouse to the right side of the    */
		/* originating individual, so we want to shift the   */
		/* drawing position of the originating individual    */
		/* to the left by one half of the horizontal spacing */
		/* unit for individuals                              */
		/*                                                   */
		return g->d.HalfHZInterval;
	
	default:
		/*                                                            */
		/* Multiple spouses.  By "default," as it were, the code      */
		/* assumes that exactly half of the spouses and children      */
		/* can be drawn on the left side of the individual, and the   */
		/* other half on the right side.  But, of course in reality   */
		/* there may be an odd number of spouses, and not all spouses */
		/* have the same number of children.  So, we need to correct  */
		/* the xorg position of the originating individual by the     */
		/* amount of imbalance between his left and right subtrees:   */
		/*                                                            */
		
		/*                                                   */
		/* number of spouses that will be drawn to the left: */
		/*                                                   */
		naleft=p->na>>1;
		/*                                                           */
		/* get the left-side actual nw width based on these spouses: */
		/*                                                           */
		for(sumleftnw=i=0;i<naleft;i++) sumleftnw+=p->a[i]->nw;
		/*                                                         */
		/* return the difference of the actual                     */
		/* left-side width minus half the originating individual's */
		/* full/combined subtree width:                            */
		/*                                                         */
		/* return ((double)sumleftnw-p->nw/2.0)*lxu;               */
		/*                                                         */
		return ((double)sumleftnw - p->nw/2.0)*g->d.HorizontalInterval;
		
	}
}


/*                                                                             */
/* DrawFamily(): Draws a family consisting of individual and his or            */
/* her spouses, and then calls DrawOffspring() to draw the offspring.          */
/*                                                                             */
/* DrawOffspring() and the twin drawing routines make                          */
/* recursive calls back up to DrawFamily() in order to draw the entire         */
/* tree.                                                                       */
/*                                                                             */
/* Note that DrawFamily draws a single founding group tree.  A single founding */
/* group consists of an individual and his or her one or multiple spouses.     */
/*                                                                             */
/* PINDIVIDUAL p:     Individual whose spouse(s) and offspring will be drawn   */
/* doubel xorg, yorg: Coordinates at which to start drawing p's tree           */
/* PCELL c:           The CELL defining which data of the individual to print  */
/* PPEDIGREE pd:      The individual's PEDIGREE, needed in order to be able to */
/*                    trace to which founding group a spouse of an individual  */
/*                    belongs                                                  */
/* int ith:           The current ith founding group tree to which p belongs   */
/*                                                                             */
/*                                                                             */
void DrawFamily(FILE *fp,PINDIVIDUAL p,double xorg,double yorg,PCELL c,PPEDIGREE pd,int ith){

	PINDIVIDUAL mom;
	PINDIVIDUAL dad;
	PINDIVIDUAL a[MAXSPOUSES];  /* vector of spouses */
	double      sttx;
	double      deltax;
	double      xend;
	
	int naleft;
	int naright;
	/* int sumnwleft; */
	/* int sumnwright; */
	int i;
	int j;

	/* char hold[MINIMUMBUFFERSIZE];*/
		
	extern PCONTROL g;


	/* switch the number of mates */
	switch(p->na){
	case 0:
	case 1:
		/*                                                        */
		/* below is just the old code for handling 0 or 1 spouse: */
		/*                                                        */

		/*                   */
		/* Draw this person: */
		/*                   */
		DrawIndividual(fp,p,xorg,yorg,c,g,pd,ith);
		/*                      */
		/* return if no spouse: */
		/*                      */
		if(!p->na) return;
		/*                                               */
		/* Draw line connecting individual to spouse:    */
		/*                                               */
		/* Note one thing about Madeline's original old  */
		/* and simplistic pedigree drawing algorithm:    */
		/* A single spouse was always just drawn         */
		/* to the right of the current individual.       */
		/* But, things have become more complicated with */
		/* consanguinous and multiple spouses ...        */
		/*                                               */
		if(p->consanguinity && p->a[0]->consanguinity){
			/*                       */
			/* CASE OF CONSANGUINITY */
			/*                       */
			if(p->a[0]->drawn){
				/* The spouse has been drawn.  This means that the offspring have also        */
				/* been drawn over to the left or up above somewhere (if on the same page),   */
				/* so we just need to draw a connecting line from this individual             */
				/* over to where the spouse would normally have been drawn had it not been a  */
				/* case of consanguinity ...                                                  */
				/*                                                                            */
				/*  *** JUST DRAW CONNECTOR ***                                               */
				xend=p->a[0]->x+g->d.HorizontalInterval;
				DrawConsanguinousConnector(fp,xorg,yorg,xend,p->a[0]->y);
				
			}else{
				/* The spouse has not yet been drawn.  This means that the offspring have also */
				/* not yet been drawn.  Therefore, draw the horizontal line to the right of    */
				/* the current individual (as if we were going to draw the spouse) but really  */
				/* to serve as a bar from which we can hang the children (unless there are no  */
				/* children.  Currently (2001.12.14), Madeline still doesn't handle the case   */
				/* of childless spouses very thoroughly, so this isn't too likely, but we'll   */
				/* check for the no-kids case anyway:                                          */
				
				/*                         */
				/* if no kids, we're done: */
				/*                         */
				if(!p->nk) return;
				
				/* DoubleHorizontalLine(fp,xorg+g->d.IconRadius,yorg,g->d.HorizontalInterval-g->d.IconDiameter); */
				
				/* This line stretches all the way to the center of where the spouse would normally be drawn */
				DoubleHorizontalLine(fp,xorg+g->d.IconRadius,yorg,g->d.HorizontalInterval-g->d.IconRadius);
				
				/* Now draw the kids */
				if(p->female){
					mom=p;
					dad=p->a[0];
				}else{
					mom=p->a[0];
					dad=p;
				}
				DrawOffspring(fp,dad,mom,xorg+g->d.HalfHZInterval,yorg,c,pd,ith);
			}
		}else{
			/*                                */
			/* NORMAL, NON-CONSANGUINOUS CASE */
			/*                                */
			HorizontalLine(fp,xorg+g->d.IconRadius,yorg,g->d.HorizontalInterval-g->d.IconDiameter);
			/*                                                             */
			/* Draw 1st (and only) spouse:                                 */
			DrawIndividual(fp,p->a[0],xorg+g->d.HorizontalInterval,yorg,c,g,pd,ith);
			/*                         */
			/* if no kids, we're done: */
			/*                         */
			if(!p->nk) return;
			
			/* Draw the offspring */
			if(p->female){
				mom=p;
				dad=p->a[0];
			}else{
				mom=p->a[0];
				dad=p;
			}
			DrawOffspring(fp,dad,mom,xorg+g->d.HalfHZInterval,yorg,c,pd,ith);
		}
		break;
		
	default:
		/*                                             */
		/* multiple spouses: copy spouses to a vector: */
		/*                                             */
		for(i=0;i<p->na && i<MAXSPOUSES;i++) a[i]=p->a[i];
		/*                                        */
		/* sort the spouses by the sort criteria: */
		/*		                                      */
		if(g->sort) SortIndividualVector(a,p->na,g);
		/*                                                       */
		/* draw half the spouses to the left, half to the right: */
		/*                                                       */
		naleft=p->na>>1;
		naright=p->na-naleft;

		if(p->female==MISSING)
			MadelineErrorReport("DrawFamily(): Madeline needs to know the sex of %s.",p->id);

		/*                                      */
		/* ORIGIN: draw the originating person: */
		/*                                      */
		DrawIndividual(fp,p,xorg,yorg,c,g,pd,ith);
		
		/*            */
		/* LEFT SIDE: */
		/*            */
		
		sttx=xorg-IndividualLeftSubtreeWidth(p)+IndividualLeftSubtreeWidth(a[0]);
		
		for(j=naleft-1,i=0;i<naleft;i++,j--){
			/*              */
			/* Draw spouse: */
			/*              */
			DrawIndividual(fp,a[i],sttx,yorg,c,g,pd,ith);
			/*                                   */
			/* horizontal line back to origin p: */
			/*			                                */
			HorizontalLineToSpouse(fp,xorg,yorg,sttx,j,naleft-1,0);
			
			if(p->female){
				mom=p;
				dad=a[i];
			}else{
				mom=a[i];
				dad=p;
			}
			DrawOffspring(fp,dad,mom,sttx+g->d.HalfHZInterval,yorg,c,pd,ith);

			/*                                          */
			/* Calculate start position of next spouse: */
			/*                                          */
			if(i<naleft-1){
				deltax=IndividualRightSubtreeWidth(a[i])+IndividualLeftSubtreeWidth(a[i+1]);
				sttx+=deltax;
			}
		}
		/*             */
		/* RIGHT SIDE: */
		/*             */
		
		/*                                                                 */
		/* note that naleft actually indexes the first indv. on the right: */
		/* -add lxu to account for originating individual                  */
		/* -add halflxu to account for backset                             */
		/*                                                                 */
		sttx=xorg+g->d.HorizontalInterval+g->d.HalfHZInterval+IndividualLeftSubtreeWidth(a[naleft]);
		
		for(j=0,i=naleft;i<p->na;i++,j++){
			/*              */
			/* Draw spouse: */
			/*              */
			DrawIndividual(fp,a[i],sttx,yorg,c,g,pd,ith);
			/*                                   */
			/* horizontal line back to origin p: */
			/*                                   */
			HorizontalLineToSpouse(fp,xorg,yorg,sttx,j,naright-1,0);
			
			if(p->female){
				mom=p;
				dad=a[i];
			}else{
				mom=a[i];
				dad=p;
			}
			DrawOffspring(fp,dad,mom,sttx-g->d.HalfHZInterval,yorg,c,pd,ith);
			
			/*                                          */
			/* Calculate start position of next spouse: */
			/*                                          */
			if(i<p->na-1){
				deltax=IndividualRightSubtreeWidth(a[i])+IndividualLeftSubtreeWidth(a[i+1]);
				sttx+=deltax;
			}
		}

		break;

	}
	
}

/*                                                                                     */
/* DrawConsanguinousConnector(): draws a double bridging line connecting consanguinous */
/* spouses anywhere on the same page                                                   */
/*                                                                                     */
void DrawConsanguinousConnector(FILE *fp,double xorg,double yorg,double xdest,double ydest){
	
	extern PCONTROL g;

	double xstt;
	double xoffset=0.1;
	double ytick =0.2;
	double yoffset=g->d.IconRadius+ytick;

	
	if(xdest<xorg){
		/* Already-drawn spouse is somewhere to the LEFT */
		/* Draw little horizontal bar sticking out from LEFT side of the icon */
		xstt=xorg-g->d.IconRadius;
		fprintf(fp,"newpath %.2f cm %.2f cm moveto %.2f cm 0 rlineto ",
		xstt,
		yorg,
		-xoffset
		);
		/* Now draw a straight line connecting the two points */
		/*fprintf(fp,"%.2f cm %.2f cm lineto ",               */
		/*	xdest,                                        */
		/*	ydest                                         */
		/* );                                                 */
		
		/* Use only vertical and horizontal paths to get to the */
		/* destination:                                         */
		xstt-=xoffset;
		fprintf(fp,"0 %.2f cm rlineto %.2f cm 0 rlineto 0 %.2f cm rlineto ",
			ydest-yorg+yoffset,
			xdest-xstt,
			-yoffset
		);
		/* continue drawing a horizontal line back to the original spouse: */
		fprintf(fp,"%.2f cm 0 rlineto DoubleBridge\n",
			-g->d.HorizontalInterval+g->d.IconRadius
		);
	}else{
		/* Already-drawn spouse is somewhere to the RIGHT */
		/* Draw little horizontal bar sticking out from RIGHT side of the icon */
		xstt=xorg+g->d.IconRadius;
		fprintf(fp,"newpath %.2f cm %.2f cm moveto %.2f cm 0 rlineto ",
		xstt,
		yorg,
		xoffset
		);
		/* Now draw a straight line connecting the two points */
		/* fprintf(fp,"%.2f cm %.2f cm lineto ",              */
		/*	xdest,                                        */
		/*	ydest                                         */
		/*);                                                  */

		/* Use only vertical and horizontal paths to get to the */
		/* destination:                                         */
		xstt+=xoffset;
		fprintf(fp,"0 %.2f cm rlineto %.2f cm 0 rlineto 0 %.2f cm rlineto ",
			ydest-yorg+yoffset,
			xdest-xstt,
			-yoffset
		);
		
		/* continue drawing a horizontal line forward to the original spouse: */
		fprintf(fp,"%.2f cm 0 rlineto DoubleBridge\n",
			g->d.HorizontalInterval-g->d.IconRadius
		);
	}
}


/*                                                                             */
/* HorizontalLineToSpouse(): draws a line connecting spouses by "jumping       */
/* over" any spouses in the way along the path:                                */
/*                                                                             */
/* fp   - print destination file/device                                        */
/* xorg - x origin of originating individual                                   */
/* yorg - y origin of originating individual                                   */
/* xend - x origin of destination spouse.  Madeline looks at xend to           */
/*        figure out whether the spouse is to the left or right of the         */
/*        originating individual                                               */
/* idx  - index number of the spouse to the right or left of the originating   */
/*        individual, counting using zero offset so that 0 is the first to the */
/*        right or left, 1 the second, and so forth.  This info. is necessary  */
/*        so Madeline can draw pretty step-up lines that run parallel to each  */
/*        other in succession.                                                 */
/* maxidx Maximum spouse index to the right or left, zero-offset               */
/*                                                                             */
void HorizontalLineToSpouse(FILE *fp,double xorg,double yorg,double xend,int idx,int maxidx,int consanguinous){

	double stepincr;
	double hhop;
	double vorg;
	double vhop;
	double hdist;
	int rightside;
	double ThisDistance;
	double ThisOrigin;

	extern PCONTROL g;
	
	
	/*                           */
	/* calculate step increment: */
	/*                           */
	stepincr=g->d.IconDiameter/MAXSPOUSES;


	/*                                                  */
	/* Drawing a line to a spouse on the right or left? */
	/*                                                  */
	rightside=(xend>xorg);
	
	/*                                                */
	/* do easy case when idx==0: just a straight line */
	/*                                                */
	if(idx==0){
		if(rightside){
			ThisDistance=xend-xorg-g->d.IconDiameter;
			ThisOrigin  =xorg+g->d.IconRadius;
		}else{
			ThisDistance=xend-xorg+g->d.IconDiameter;
			ThisOrigin  =xorg-g->d.IconRadius;
		}
		fprintf(fp,"newpath %.2f cm %.2f cm moveto %.2f cm 0 rlineto ",
			ThisOrigin,
			yorg,
			ThisDistance
		);
		/*                  */
		/* Stroke the path: */
		/*                  */
		if(consanguinous)
			fprintf(fp,"DoubleBridge\n");
		else
			fprintf(fp,"stroke\n");
		/*                          */
		/* All done with easy case: */
		/*                          */
		return;
	}

	/*                           */
	/* Get here in general case: */
	/*	                          */

	/*                                                            */
	/* horizontal push off distance, vertical origin:             */
	/* plus 2 instead of plus 1 gives it a little extra distance: */
	/*                                                            */
	hhop=(maxidx-idx+2)*stepincr;
	vhop=stepincr*idx;
	vorg=yorg+vhop;

	if(rightside){
		xorg+=g->d.IconRadius;
		hdist=xend-xorg-g->d.HorizontalInterval-hhop;
	}else{
		xorg-=g->d.IconRadius;
		hdist=xorg-xend-g->d.HorizontalInterval-hhop;
	}
		
	/*                      */
	/* horizontal push off: */
	/*		                    */
	fprintf(fp,"newpath %.2f cm %.2f cm moveto %.2f cm 0 rlineto ",
		xorg,
		vorg,
		rightside?hhop:-hhop
	);

	/*                                                                 */
	/* vertical riser is IconRadius tall to clear head of next symbol: */
	/*                                                                 */
	fprintf(fp,"0 %.2f cm rlineto ",g->d.IconRadius);
	
	/*                                                             */
	/* horizontal jump to HorizontalInterval distance before xend: */
	/*                                                             */
	fprintf(fp,"%.2f cm 0 rlineto ",
		rightside?hdist:-hdist
	);
		
	/*                                                            */
	/* vertical drop back down to yorg:                           */
	/*                                                            */
	/* VerticalLine(fp,xend-lxu,vorg-grad,grad+vhop,CENTIMETERS); */
	/*                                                            */
	fprintf(fp,"0 %.2f cm rlineto ",- g->d.IconRadius - vhop);
		
	/*                                                        */
	/* final horizontal line to individual:                   */
	/*                                                        */
	/* HorizontalLine(fp,xend-lxu,yorg,lxu-grad,CENTIMETERS); */
	/*                                                        */
	ThisDistance=g->d.HorizontalInterval-g->d.IconRadius;		
	fprintf(fp,"%.2f cm 0 rlineto\n",
		rightside?ThisDistance:-ThisDistance			
	);		

	/*                  */
	/* Stroke the path: */
	/*                  */
	if(consanguinous)
		fprintf(fp,"DoubleBridge\n");
	else
		fprintf(fp,"stroke\n");
					
}



/*                                                                             */
/* Draw the offspring of the father (dad) and mother (mom) pair:               */
/*                                                                             */
/* FILE *fp:             Print file                                            */
/* PINDIVIDUAL dad, mom: Self-explanatory                                      */
/* double xorg,yorg:     Starting coordinates                                  */
/* PCELL c:              CELL which holds information about what data to print */
/* PPEDIGREE pd:         PEDIGREE which is being drawn                         */
/* int ith:              Current "ith" founding group which is being drawn     */
/*                                                                             */
void DrawOffspring(FILE *fp,PINDIVIDUAL dad,PINDIVIDUAL mom,double xorg,double yorg,PCELL c,PPEDIGREE pd,int ith){

	
	PINDIVIDUAL kid[MAXKIDS];
	PINDIVIDUAL o;             /* the first kid, either twin or non-twin */
	int i,j;
	PTWINGROUP t=NULL;
	int sumnw;                 /* sum of subtree widths for the pair's offspring */
	int OffspringDrawn=0;      /* set to 1 if non-twin offspring have been drawn */

	double sttx;
	double OffspringLineY;     /* y-coordinate of the offspring line */
	double deltax=0.0;
	/* double halflxu; */      /* half the horizontal unit distance between people */
	/* double thisxshift; */   /* xorg shift of this individual                    */

	/* PTOKENINFO pLastToken; */     /* last token in current token stack */

	/*                                                                                                        */
	/* get halflxu:                                                                                           */
	/*                                                                                                        */
	/* halflxu=g->d.HorizontalInterval/2.0;     // half unit horizontal distance between individual's centers */
	
	OffspringLineY= yorg+g->d.VerticalDrop1;  /* where horizontal line will be drawn */


	/*                                                                                  */
	/* The female's k vector has ALL of her children -including those that might not be */
	/* from other matings                                                               */
	/*                                                                                  */
	
	/*                                                                    */
	/* Store pointers to the pair's non-twin offspring in kid vector.     */
	/* Otherwise, for twins, build a TWINGROUP BST and store the pointers */
	/* for the twins there:                                               */
	/*                                                                    */
	for(sumnw=j=i=0;i<mom->nk && i<MAXKIDS;i++){
		/*                              */
		/* kids sired by this dad only: */
		/*                              */
		if(mom->k[i]->f==dad){
			/*                                                      */
			/* sum up offspring nw widths for the pair's offspring, */
			/* including twin's widths:                             */
			/*                                                      */
			sumnw+=mom->k[i]->nw;
			
			if(mom->k[i]->twin){
				/*        */
				/* twins: */
				/*        */
				t=InsertTwinGroup(t,mom->k[i]->twin,mom->k[i]);
			}else{
				/*            */
				/* non-twins: */
				/*            */
				 kid[j++]=mom->k[i];
		 	}		 	
		}
	}
	
	/*                             */
	/* return if no kids or twins: */
	/*                             */
	if(!(j || t)) return;	
	
	/*                                             */
	/* Draw all the non-twins first, using what is */
	/* basically the original code to do this:     */
	/*                                             */
	
	/*                                                                   */
	/* if sorting is in effect, sort the kid array with j non-twin kids: */
	/*                                                                   */
	if(g->sort){
		/*                  */
		/* non-twin vector: */
		/*                  */
		if(j>1) SortIndividualVector(kid,j,g);
		/*               */
		/* twin vectors: */
		/*               */
		if(t) TraverseTwinGroup(t,SortTwinGroup);

	}

	
	/*                                                        */
	/* VERTICAL LINE from parent line down to offspring line: */
	/*                                                        */
	VerticalDrop1(fp,xorg,yorg);
		
	/*                                                                                  */
	/* Redefine yorg to be the offspring symbol center:                                 */
	/*                                                                                  */
	yorg=OffspringLineY + g->d.VerticalDrop2;  /* where offspring symbol center will be */

	/*                                                          */
	/* From xorg, go back half the sumnw width,                 */
	/* then add half the first offspring's nw width.            */
	/*                                                          */
	/* Note that the very first offspring could be a twin.      */
	/* So, if j is non-zero, then we take the width of the      */
	/* first non-twin, otherwise we take the width of the first */
	/* twin:                                                    */
	/*                                                          */
	
	/*                                                                     */
	/* base left subtree width on that mate with only                      */
	/* one spouse:                                                         */
	/*                                                                     */
	o=j?kid[0]:t->t[0];  /* the very first kid, either in kid[] or in t->t */
	sttx=xorg-(sumnw)*g->d.HalfHZInterval+IndividualLeftSubtreeWidth(o);

	/*                                                */
	/*                                                */
	/* Loop through non-twin kids in the kid vector:  */
	/*                                                */
	for(i=0;i<j;i++){
		/*                                                  */
		/* Draw horizontal segment to start of next person: */
		/*		                                                */
		if(i<j-1){
			/*                        */
			/* deltax to next person: */
			/*                        */
			deltax=IndividualRightSubtreeWidth(kid[i])+IndividualLeftSubtreeWidth(kid[i+1]);
			/*                              */
			/* draw line to next offspring: */
			/*                              */
			HorizontalLine(fp,sttx,OffspringLineY,deltax);
		}else{
			/*                                                */
			/* draw a line to first twin group individual, if */
			/* there are twins:                               */
			/*                                                */
			if(t){
				/*                       */
				/* deltax to first twin: */
				/*                       */
				deltax=IndividualRightSubtreeWidth(kid[i])+IndividualLeftSubtreeWidth(t->t[0]);
				/*                              */
				/* draw line to next offspring: */
				/*                              */
				HorizontalLine(fp,sttx,OffspringLineY,deltax);
			}
		}				
		/*                                       */
		/* draw vertical line down to offspring: */
		/*                                       */
		VerticalDrop2MinusIconRadius(fp,sttx,OffspringLineY);
		
		/*                                           */
		/* draw offspring family via recursive call: */
		/*                                           */
		DrawFamily(fp,kid[i],sttx,yorg,c,pd,ith);

		/*                                                            */
		/* increment sttx for next kid only when there is a next kid: */
		/*                                                            */
		if(i<j-1 || t) sttx+=deltax;

		/*                            */
		/* note drawing of offspring: */
		/*                            */
		if(!OffspringDrawn) OffspringDrawn++;
	}
	
	/*                                                                  */
	/* draw connector to xorg if missing: this occurs when there is     */
	/* only one offspring and that offspring is married, so there's     */
	/* a halflxu backshift, but no subsequent kid so no horizontal line */
	/* is ever drawn:                                                   */
	/*                                                                  */
	if(!t && sttx<xorg){
		deltax=xorg-sttx;
		HorizontalLine(fp,sttx,OffspringLineY,deltax);
	}

	/*                                         */
	/* Handle twins at the end of the sibship: */
	/*                                         */
	if(!t) return;

	/*                                                */
	/* Get here if there are twin groups, so traverse */
	/* them with the supplied function:               */
	/*                                                */
	TraverseTwinGroupToDrawSibship(fp,mom,t,sttx,OffspringLineY,c,OffspringDrawn,xorg,pd,ith,DrawTwinSibship);
}



/*                                                                  */
/* SortTwinGroup() is a wrapper around SortIndividualVector() which */
/* can be called as the visit function for TraverseTwinGroup():     */
/*                                                                  */
void SortTwinGroup(PTWINGROUP t){

	extern PCONTROL g;
	SortIndividualVector(t->t,t->n,g);

}


/*                                                                  */
/* SortIndividualVector(): sort a vector of pointers to individuals */
/*                                                                  */
void SortIndividualVector(PPINDIVIDUAL v,int n,PCONTROL c){

	PTOKENINFO pLastToken;
	
	extern PTOKENINFO pT;

	if(!n) return;
	/*                 */
	/* turn repeat on: */
	/*                 */
	RepeatInProgress=TRUE;
	/*                                      */
	/* save current pT (which is on Token): */
	/*                                      */
	pLastToken=pT;
	/* Call QuickSort with the Compare function which looks at     */
	/* an expression on Token2: g->sort specifies either ascending */
	/* or descending: pass 1 for ascending, 0 for descending       */
	/*                                                             */
			
	/*                                           */
	/* Sort the non-twin offspring "kid" vector: */
	/*                                           */
	QuickSortPointerVector((void **)v,n,(c->sort==SASCK),CompareExpression);
	/*                  */
	/* turn repeat off: */
	/*                  */
	RepeatInProgress=FALSE;
	/*                                                                 */
	/* restore pT so that it points to the last token on "Token" stack */
	/* (pT is currently on the last token on "Token2" stack            */
	pT=pLastToken;
}
		

/*                                                                       */
/* TraverseTwinGroupToDrawSibship(): Traverse a TWINGROUP BST:           */
/*                                                                       */
/* FILE *fp      : File to print to                                      */
/* PINDIVIDUAL f : mother of the twins                                   */
/* PTWINGROUP t  : a bst of the twin groups                              */
/* double sttx,liney,xorg: drawing coordinates                           */
/* PCELL c       : contains data to print for the individuals            */
/* int OffspringDrawn: flag indicating if other sibs (twin or otherwise) */
/*                     have been drawn already or not                    */
/* PPEDIGREE pd  : Pedigree which is being drawn                         */
/* ith           : ith founding group tree which is being drawn          */
/* DrawingFunction: Function which draws a single twin group sibship     */
/*                                                                       */
/* Note that this function is really just a basic recursive bst          */
/* traversal function:                                                   */
/*                                                                       */
/*                                                                       */
double TraverseTwinGroupToDrawSibship(FILE *fp,PINDIVIDUAL f,PTWINGROUP t,
    double sttx,double liney,PCELL c,int OffspringDrawn,double xorg,
    PPEDIGREE pd,int ith,
    double (*DrawFunction)(FILE *,PINDIVIDUAL,PTWINGROUP,double,double,PCELL,int,double,PPEDIGREE,int)
    ){

	if(!t) return sttx;
	/*            */
	/* left node: */
	/*            */
	if(t->l){
		sttx=TraverseTwinGroupToDrawSibship(fp,f,t->l,sttx,liney,c,OffspringDrawn,xorg,pd,ith,DrawFunction);
		OffspringDrawn++;
	}
	/*               */
	/* current node: */
	/*               */
	sttx=DrawFunction(fp,f,t,sttx,liney,c,OffspringDrawn,xorg,pd,ith);
	OffspringDrawn++;
	/*             */
	/* right node: */
	/*             */
	if(t->r){
		sttx=TraverseTwinGroupToDrawSibship(fp,f,t->r,sttx,liney,c,OffspringDrawn,xorg,pd,ith,DrawFunction);
	}
	/*                      */
	/* Return updated sttx: */
	/*                      */
	return sttx;
}

/*                                                                                   */
/* DrawTwinSibship(): Draw a sibship of twins:                                       */
/* -f is a female individual, the mother of the twinship:                            */
/* -sttx is the starting x position of the first twin in the twin group              */
/* -xorg is the x-coordinate of the vertical drop-down offspring line.  This is      */
/*  needed so that in the rare case where there are multiple twin groups and no non- */
/*  twin offspring, a connecting horizontal line can be drawn from xanchor to sttx.  */
/* -liney is the vertical position of the horizontal bar connecting sibs, not yorg   */
/* -xorg is the original xanchor of the entire sibship, twins and otherwise          */
/* -pd is the pedigree being drawn                                                   */
/* -ith is the ith founding group tree being drawn                                   */
/*                                                                                   */
double DrawTwinSibship(FILE *fp,PINDIVIDUAL f,PTWINGROUP t,double sttx,double liney,PCELL c,int OffspringDrawn,double xorg,PPEDIGREE pd,int ith){

	int i;
	/* int j; */
	int sumnw;             /* sum of nw for twinship */
	/* int ni; */          /* next index             */
	double deltax;
	double xanchor;
	double yticklen;
	double deltay;
	double yorg;
	/* double halflxu; */
	
	extern PCONTROL g;
	
	
	int type;      /* 0 for confirmed and unconfirmed DZ, 1 for confirmed MZ, 2 for unconfirmed possible MZ */
	
	/*                                                                                       */
	/* determine type based on key:                                                          */
	/*                                                                                       */
	if     (t->key>='A' && t->key<='Z') type=1; /* confirmed MZ                              */
	else if(t->key>='1' && t->key<='9') type=2; /* unconfirmed, could be MZ because same sex */
	else                                type=0; /* known to be DZ because sex different      */

	/*                                                */
	/* determine xanchor based on sumnw for twinship: */
	/*                                                */
	for(sumnw=i=0;i<t->n;i++) sumnw+=t->t[i]->nw;

	/*                                                */
	/* Anchor point is sttx plus the summed nw widths */
	/* divided by 2:                                  */
	/*                                                */
	deltax=g->d.HalfHZInterval*(sumnw)-IndividualLeftSubtreeWidth(t->t[0]);
	xanchor=sttx+deltax;	

	/*                                                                     */
	/* Draw horizontal segment from xorg to xanchor:                       */
	/* This fixes a couple of things:                                      */
	/* (1) in the rare case where sttx is less than xorg, a line would     */
	/*     not otherwise be drawn                                          */
	/* (2) in the case that a left TWINGROUP node is drawn, a connecting   */
	/*     line would not be drawn before the function exits and goes back */
	/*     up to the parent node.                                          */
	/*                                                                     */
	
	HorizontalLine(fp,xanchor,liney,xorg-xanchor);

	/*                                                     */
	/* draw horizontal segment from sttx to anchor point,  */
	/* but only if there                                   */
	/* were predecessors already drawn (either non-twin    */
	/* siblings, or even a previous twinship group:        */
	/*                                                     */

	if(OffspringDrawn)
		HorizontalLine(fp,sttx,liney,deltax);

	/*                                                              */
	/* yticklen puts little vertical tick segments before and       */
	/* after the diagonal segment, which leads to a more attractive */
	/* drawing:                                                     */
	/*                                                              */
	/* NOTE BENE: deltay is ALREADY NEGATIVE because VerticalDrop2  */
	/* is negative.  So, yticklen is ALSO ALREADY NEGATIVE.         */
	/* Don't forget this when looking at the drawing code!          */
	/*                                                              */
	deltay  =g->d.VerticalDrop2+g->d.IconRadius;
	yticklen=deltay/5;
	yorg    =liney+g->d.VerticalDrop2;
	/*                                               */
	/* vertical tick hanging down from anchor point: */
	/*                                               */
	VerticalLine(fp,xanchor,liney,yticklen);	

	/*                                                         */
	/* Now draw diagonals and vertical dropping ticks to sibs, */
	/* then call DrawFamily recursively to draw sibs and their */
	/* families:                                               */
	/*                                                         */
	for(i=0;i<t->n;i++){
		/*                                    */
		/* Draw diagonal segment from xanchor */
		/* to the start of this person:       */
		/*                                    */
		AbsoluteLine(fp,xanchor,liney+yticklen,sttx,liney+deltay-yticklen);
		/*                           */
		/* final vertical drop tick: */
		/*                           */
		VerticalLine(fp,sttx,liney+deltay-yticklen,yticklen);
		/*                                           */
		/* draw offspring family via recursive call: */
		/*                                           */
		DrawFamily(fp,t->t[i],sttx,yorg,c,pd,ith);

		/*                                                   */
		/* Figure out what deltax is to position next kid:   */
		/*                                                   */
		/* (1) deltax to the NEXT kid in CURRENT TWINGROUP   */
		/*                                                   */
		/* OR:                                               */
		/*                                                   */
		/* (2) deltax to the FIRST kid in the NEXT TWINGROUP */
		/*                                                   */
		if(i<t->n-1 || t->r){

			if(i<t->n-1){
				/*                                          */
				/* there's still another kid in this group: */
				/*                                          */

				deltax=IndividualRightSubtreeWidth(t->t[i])+IndividualLeftSubtreeWidth(t->t[i+1]);
				
			}else{
				/*                                              */
				/* case of a new twin group coming up:          */
				/*                                              */
				/* deltax=halflxu*(t->t[i]->nw+t->r->t[0]->nw); */

				deltax=IndividualRightSubtreeWidth(t->t[i])+IndividualLeftSubtreeWidth(t->r->t[0]);
				
				/*                                          */
				/* draw horizontal line to next twin group: */
				/*                                          */
				HorizontalLine(fp,xanchor,liney,sttx-xanchor+deltax);
			}
		}
		
		/*                                            */
		/* Draw horizontal line to show monozygotic   */
		/* twinship.  Draw dashed line with ? mark to */
		/* show possible but unconfirmed MZ twinship: */
		/*                                            */
		if(i<t->n-1 && type){
			switch(type){
			case 1:
				/*                */
				/* Confirmed MZs: */
				/*                */
				HorizontalLine(fp,sttx,liney+deltay-yticklen,deltax);
				break;
			case 2:
				/*                                                      */
				/* same sex twins, could be MZ or DZ, MZ not confirmed: */

				/*                                            */
				/* Print 20% gray density line in the middle: */
				/*                                            */
				HorizontalGrayLine(fp,sttx,liney+deltay-yticklen,deltax,NORMLINE,CENTIMETERS,0.20);
				/*                                                  */
				/* Put a question mark down the middle of the line: */
				/*                                                  */
				QuickLabel(fp,sttx+deltax/2.0,liney+deltay,0.3,NOLINE,1,"?");
				break;
			}
		}

		/*                              */
		/* increment sttx for next kid: */
		/*                              */
		sttx+=deltax;

	}
	
	/*              */
	/* return sttx: */
	/*              */
	return sttx;
}



/*                                                                */
/*                                                                */
/* NewPedigreeDrawingCell():                                      */
/* Fields in the global database, gdb, currenty flagged as output */
/* fields, are assigned to the CELL for output                    */
/* int s and int z are text style and size, respectively          */
/*                                                                */
/*                                                                */
#define LINESPACING 0.85
#define INTOCM 3.54

PCELL NewPedigreeDrawingCell(int s,int z){

	int i,j;
	int prec;
	int ii;     /* actual field index:            */
	double CMFontSize; /* FontSize in centimeters */
	
	PCELL c;
	
	extern PCONTROL g;

	c=NewCell(1);
	/*                         */
	/* Assign basic cell data: */
	/*                         */
	c->u=CENTIMETERS;
	/*                                                         */
	/* xex default is set: will be changed later if necessary: */
	/*                                                         */
	c->xex=g->d.CellWidth;
	for(i=0;i<MAXBORDERS;i++)
		c->b[i]=NOLINE;         
	c->tc=TCBLACK;
	c->bc=BCWHITE;
	c->j =CENTER;
	c->f =ARIAL;

	
	/*                                                      */
	/* Number of output fields is number of text labels for */
	/* the cell:                                            */
	/*                                                      */
	c->n=g->db->n.o;
	c->t=NewTextLabel(c->n);
	/*                                                                            */
	/* yex is set based on number of lines, i.e., no. of fields                   */
	/* multiplied by the font height (1 point is 1/72nd of an inch)               */
	/* multiplied by a line spacing factor:                                       */
	/*                                                                            */
	/*                                                                            */
	/* yex is FontSize*(LineSpacing=1.1)*(NumberOfLines)+2*CellMargin+FontSize/2: */
	/*                                                                            */
	CMFontSize=2.54/72.0*(double)z;
	c->yex=CMFontSize*g->db->n.o+0.2+CMFontSize/2.0;
	
	/*                                                                 */
	/* set g->d.VerticalDrop1 based on c->yex:                         */
	/*                                                                 */
	/* Remember that VerticalDrop1 and VerticalDrop2 are always        */
	/* NEGATIVE.  VerticalDrop2 is greater in magnitude than           */
	/* IconDiameter, so the addition here gives a little extra spacing */
	/* to the vertical drop:                                           */
	/*                                                                 */
	g->d.VerticalDrop1=-c->yex-g->d.IconRadius+g->d.VerticalDrop2;
	
	/*                                                              */
	/* Assign text data:                                            */
	/* i indexes the database field while j indexes the text label: */
	/*                                                              */
	
	/*                                       */
	/* The ordering of the fields is C->P->G */
	/*                                       */
	
	/*                 */
	/* C FIELDS first: */
	/*                 */
	for(j=i=0;i<g->db->n.co;i++){
		/*                      */
		/* get the field index: */
		/*                      */
		ii=g->db->fi->c[i];
		/*                                      */
		/* fixed text pointer is assigned NULL: */
		/*                                      */
		c->t[j].t=NULL;
		c->t[j].s=s;
		c->t[j].z=z;
		switch(g->db->FieldType(g->db,ii)){
		case 'C':
			c->t[j].d=NewDataSource(g->db,ii,TEXTDATA,NULL);
			break;
		case 'D':
			c->t[j].d=NewDataSource(g->db,ii,DATEDATA,NULL);
			break;
		case 'N':
			prec=g->db->FieldPrecision(g->db,ii);
			if(prec>5) prec=5; /* don't print too many decimal places */
			c->t[j].d=NewDataSource(g->db,ii,prec,NULL);                   
			break;
		}
		j++;
	}
	/*                */
	/* P FIELDS next: */
	/*                */
	for(i=0;i<g->db->n.po;i++){
		/*                      */
		/* get the field index: */
		/*                      */
		ii=g->db->fi->p[i];
		/*                                      */
		/* fixed text pointer is assigned NULL: */
		/*                                      */
		c->t[j].t=NULL;
		c->t[j].s=s;
		c->t[j].z=z;
		switch(g->db->FieldType(g->db,ii)){
		case 'C':
			c->t[j].d=NewDataSource(g->db,ii,TEXTDATA,NULL);
			break;
		case 'D':
			c->t[j].d=NewDataSource(g->db,ii,DATEDATA,NULL);
			break;
		case 'N':
			prec=g->db->FieldPrecision(g->db,ii);
			if(prec>5) prec=5; /* don't print too many decimal places */
			c->t[j].d=NewDataSource(g->db,ii,prec,NULL);                   
			break;
		}
		j++;
	}
	/*                                                 */
	/* G FIELDS last: G fields can only be of type 'C' */
	/*                                                 */
	for(i=0;i<g->db->n.go;i++){
		/*                      */
		/* get the field index: */
		/*                      */
		ii=g->db->fi->g[i];
		/*                                      */
		/* fixed text pointer is assigned NULL: */
		/*                                      */
		c->t[j].t=NULL;
		c->t[j].s=s;
		c->t[j].z=z;
		c->t[j].d=NewDataSource(g->db,ii,GENOTYPEDATA,NULL);
		j++;
	}	
	return c;
}




/*                                                                */
/*                                                                */
/* NewPedigreeLegendCell():                                       */
/* Fields in the global database, gdb, currenty flagged as output */
/* fields, are assigned to the CELL for output                    */
/* int s and int z are text style and size, respectively          */
/*                                                                */
/*                                                                */
PCELL NewPedigreeLegendCell(int s,int z){

	int i,j,ii;	
	PCELL c;
	extern PCONTROL g;

	c=NewCell(1);

	
	/*                         */
	/* Assign basic cell data: */
	/*                         */
	c->u=CENTIMETERS;
	/*c->x=4.0;                                               */
	c->x= -8.0; /* This works for both Portrait and Landscape */
	c->y=  0.0;
	/*                                    */
	/* y-position depends on orientation: */
	/*                                    */
	switch(g->d.Orientation){
	case LANDSCAPEORIENTATION:
	case AUTOMATICORIENTATION:
	case MULTIPAGEORIENTATION:
	/*	c->y=20.5; */
		break;
	case PORTRAITORIENTATION:
	/*	c->y=27.0; */
		break;
	}

	c->xex=2.2;
	for(i=0;i<MAXBORDERS;i++)
		c->b[i]=THINLINE;         
	c->tc=TCBLACK;
	c->bc=BCWHITE;
	c->j =CENTER;
	c->f =ARIAL;
		
	/*                                                      */
	/* Number of output fields is number of text labels for */
	/* the cell:                                            */
	/*                                                      */
	c->n=g->db->n.o;
	c->t=NewTextLabel(c->n);
	/*                                                              */
	/* yex is set based on number of lines, i.e., no. of fields     */
	/* multiplied by the font height (1 point is 1/72nd of an inch) */
	/* multiplied by a line spacing factor:                         */
	/*                                                              */
	c->yex=z*g->db->n.o*LINESPACING*3.54/72.0;
	
	/*                                                              */
	/* Assign text data:                                            */
	/* i indexes the database field while j indexes the text label: */
	/*                                                              */
	
	/*                 */
	/* C FIELDS first: */
	/*                 */
	for(j=i=0;i<g->db->n.co;i++){          
		/*                  */
		/* Get field index: */
		/*                  */
		ii=g->db->fi->c[i];
		/*                               */
		/* Assign the field name as text */
		/*                               */
		c->t[j].t=g->db->FieldName(g->db,ii);
		c->t[j].s=s;
		c->t[j].z=z;
		j++;
	}
	/*                */
	/* P FIELDS NEXT: */
	/*                */
	for(i=0;i<g->db->n.po;i++){          
		/*                  */
		/* Get field index: */
		/*                  */
		ii=g->db->fi->p[i];
		/*                               */
		/* Assign the field name as text */
		/*                               */
		c->t[j].t=g->db->FieldName(g->db,ii);
		c->t[j].s=s;
		c->t[j].z=z;
		j++;
	}
	/*                */
	/* G FIELDS last: */
	/*                */
	for(i=0;i<g->db->n.go;i++){          
		/*                  */
		/* Get field index: */
		/*                  */
		ii=g->db->fi->g[i];
		/*                               */
		/* Assign the field name as text */
		/*                               */
		c->t[j].t=g->db->FieldName(g->db,ii);
		c->t[j].s=s;
		c->t[j].z=z;
		j++;
	}
	
	return c;
}


/*                                                      */
/*                                                      */
/* FisherDataHeaderProcessor(): Used for Generic Fisher */
/*                                                      */
void FisherDataHeaderProcessor(PCONTROL c, char *fn){
	

	int i,ii;
	int w; /* output field width: adjusted, if necessary */


	/*                                                                    */
	/* call generic to open output stream, store file name, reset control */
	/* counters, and inform user:                                         */
	/*                                                                    */
	GenericDataHeaderProcessor(c,fn);
		
	fprintf(c->streams.fp,"(I3,1X,I3,1X,A8)\n");
	fprintf(c->streams.fp,"(3(A8,1X),2(A1,1X)");

	/*                                                                  */
	/* phenotype fields first: these will be output with individualized */
	/* widths                                                           */
	/*                                                                  */
	for(i=0;i<c->db->n.po;i++){
		/*                          */
		/* Get the true field index */
		/*                          */
		ii=c->db->fi->p[i];
		/*                                                     */
		/* Fix width if it exceeds MENDEL's 8-character width: */
		/*                                                     */
		if((w=c->db->FieldLength(c->db,ii))>MAXIMUMMENDELFIELDWIDTH)
			w=MAXIMUMMENDELFIELDWIDTH;
		/*                                     */
		/* print the FORTRAN format specifier: */
		/*                                     */
		fprintf(c->streams.fp,",A%i,1X",w);
	}
	/*                                                     */
	/* Now genotype fields: all fixed width xxx/xxx A7,1X: */
	/*                                                     */
	fprintf(c->streams.fp,",%i(A%i,1X)",c->db->n.go,MENDELGENOTYPEFIELDWIDTH);
 
 	/*                                                       */
	/* terminate the FORTRAN record with closing parentheses  */
	/* and a carriage return:                                 */
	/*                                                        */
	fprintf(c->streams.fp,")\n");
}



/*                                                                                       */
/*                                                                                       */
/* UserM13DataHeaderProcessor(): Used for Mendel-based programs: Mendel, UserM13,Fisher, */
/*                              & SolarFisher                                            */
/*                                                                                       */
void UserM13DataHeaderProcessor(PCONTROL c, char *fn){
	

	/* int i,ii; */
	/* int w;    */     /* output field width: adjusted, if necessary */


	/*                                                                    */
	/* call generic to open output stream, store file name, reset control */
	/* counters, and inform user:                                         */
	/*                                                                    */
	GenericDataHeaderProcessor(c,fn);
		
	fprintf(c->streams.fp,"(I3,1X,A8)\n");
	fprintf(c->streams.fp,"(3(A8,1X),2(A1,1X)");


	
	 /*phenotype fields first: these will be output with individualized */
	 /*widths                                                           */
	/*                                                                  */
	/*for(i=0;i<c->db->n.po;i++){                                       */
	/*	//                                                               */
	/*	// Get the true field index                                      */
	/*	//                                                               */
	/*	ii=c->db->fi->p[i];                                              */
	/*	//                                                               */
	/*	// Fix width if it exceeds MENDEL's 8-character width:           */
	/*	//                                                               */
	/*	if((w=c->db->FieldLength(c->db,ii))>MAXIMUMMENDELFIELDWIDTH)     */
	/*		w=MAXIMUMMENDELFIELDWIDTH;                                      */
	/*	//                                                               */
	/*	// print the FORTRAN format specifier:                           */
	/*	//                                                               */
	/*	fprintf(c->streams.fp,",A%i,1X",w);                              */
	/*}                                                                 */
	/*                                                                  */
	
	
	/*                                                 */
	/* Genotype fields: all fixed width xxx/xxx A7,1X: */
	/*                                                 */
	fprintf(c->streams.fp,",%i(A%i,1X)",c->db->n.go,MENDELGENOTYPEFIELDWIDTH);
	
	/*                                                          */
	/* extra final column of weights for MENDEL USERM13 module: */
	/*                                                          */
	fprintf(c->streams.fp,",A3");
 
 	/*                                                       */
	/* terminate the FORTRAN record with closing parentheses  */
	/* and a carriage return:                                 */
	/*                                                        */
	fprintf(c->streams.fp,")\n");
}


/*                              */
/*                              */
/* CrimapDataHeaderProcessor(): */
/*                              */
/*                              */
void CrimapDataHeaderProcessor(PCONTROL c,char *fn){

	int i,ii;

	
	/*                                                             */
	/* call generic or "base class" function to open stream, etc.: */
	/*                                                             */
	GenericDataHeaderProcessor(c,fn);
	
	/*                                              */
	/*                                              */
	/* The number of families output for Crimap     */
	/* must be added on after all the families have */
	/* been output and counted, but it's not a big  */
	/* deal:                                        */
	/*                                              */
	/* fprintf(c->streams.fp,"%i\n",c->p.in);       */
	/*                                              */


	/* So, this is actually the second item in the Crimap */
	/* file header:                                       */
	/* number of genotype output fields:                  */
	/*                                                    */
	fprintf(c->streams.fp,"%i\n",c->db->n.go);

	/*                                         */
	/* print the names of the genotype fields: */
	/*                                         */
	for(i=0;i<c->db->n.go;i++){
		/*                  */
		/* get field index: */
		/*                  */
		ii=c->db->fi->g[i];
		fprintf(c->streams.fp,"%s ",c->db->FieldName(c->db,ii));
	}
					
	fprintf(c->streams.fp,"\n\n");
	/*                                     */
	/* Crimap header ends here, so return: */
	/*                                     */
	return;
}


/*                              */
/*                              */
/* CrimapDataFooterProcessor(): */
/*                              */
/*                              */
void CrimapDataFooterProcessor(PCONTROL c){

	char *b;
	
	/*                               */
	/* First close main output file: */
	/*                               */
	fclose(c->streams.fp);
	/*                                                    */
	/* put the whole output file into a buffer in memory: */
	/* note that OpenAndBufferFile() closes the file once */
	/* it has read it all:                                */
	/*                                                    */
	lprintf("Inserting CRIMAP file header...\n");
	
	b=OpenAndBufferFile(gOutputFile.e.s);
	/*                                 */
	/* write over the old output file: */
	/*                                 */
	c->streams.fp=fopen(gOutputFile.e.s,"wb");
	/*                                               */
	/* print that one missing number, the number of  */
	/* families that must be at the top of the file, */
	/* followed by the whole file:                   */
	/*                                               */
	fprintf(c->streams.fp,"%i\n%s",c->counters.pedg.in,b);
	free(b);
	/*                                                       */
	/* Call generic processor to close files and show stats: */
	/*                                                       */
	GenericDataFooterProcessor(c);
}



/*                           */
/*                           */
/* FSPDataHeaderProcessor(): */
/*                           */
/*                           */
void FSPDataHeaderProcessor(PCONTROL c,char *fn){

	int i,ii;
	int charcount;
	int rowcount=1;  /* the first row is just the core data. */
	int chars;
	int lastcount;
	
	/*                                  */
	/* For FSP, we also write to a .par */
	/* parameter file:                  */
	/*                                  */
	StoreString(&gParameterOutputFile,fn);

	/*                                                         */
	/* change ending to .par by overwriting last 4 characters: */
	/*                                                         */
	strncpy(gParameterOutputFile.e.s+strlen(gParameterOutputFile.e.s)-4,
	        ".par",4);

	/*                                                       */
	/* Then open up the .par file for use by SibpalOutput(): */
	/*                                                       */
	c->streams.fpar=fopen(gParameterOutputFile.e.s,"wt");
	if(!c->streams.fpar)
		MadelineErrorReport("FSPDataHeaderProcessor(): unable to open \"%s\" for output",gParameterOutputFile.e.s);

	lprintf("Creating associated FSP parameter file called \"%s\"\n",gParameterOutputFile.e.s);
		
	/*                                                         */
	/* Call generic header processor to open main output file: */
	/*                                                         */
	GenericDataHeaderProcessor(c,fn);

	/*                          */
	/* SAGE package FSP format: */
	/*                          */
	if(c->OutputFormat==FSPK){
		/*                                                           */
		/* Figure out how many records there must be per individual: */
		/*                                                           */
		for(charcount=i=0;i<c->db->n.po;i++){
			/*                  */
			/* get field index: */
			/*                  */
			ii=c->db->fi->p[i];
			charcount+=c->db->FieldLength(c->db,ii); /* field width    */
			charcount++;                             /* plus 1X spacer */
			if(charcount>=FSPMAXRECLENGTH-12){
				charcount=0;
				rowcount++;
			}
		}
		for(i=0;i<c->db->n.go;i++){
			/*                                                                                */
			/* get field index:                                                               */
			/*                                                                                */
			/* ii=c->db->fi->g[i];                                                            */
			/*                                                                                */
			charcount+=SIBPALGENOTYPEFIELDWIDTH;     /* field width fixed for genotype fields */
			charcount++;                             /* plus 1X spacer                        */
			if(charcount>=FSPMAXRECLENGTH-12){
				charcount=0;
				rowcount++;
			}
		}
		/*             */
		/* final line? */
		/*             */
		if(charcount) rowcount++;	
	}else{
		/*               */
		/* FSP0K format: */
		/*               */
		rowcount=1;
	}
	
	fprintf(c->streams.fpar,"Input for FSP created by Madeline for %-41.41s\n",c->db->nm);
	/*                                                                               */
	/* If FSP0 format, then you definitely don't need the segregation analysis file, */
	/* If FSP  format, then you probably do want it, hence the defaults below:       */
	/*                                                                               */
	fprintf(c->streams.fpar,"    1    %c         0    %iMF\n",
		c->OutputFormat==FSP0K?'0':'1',
		rowcount);
	/*                                                        */
	/* These fields are study identifier, family id, studyid, */
	/* parent 1's id, parent 2's id, and sex, respectively:   */
	/*                                                        */
	fprintf(c->streams.fpar,"(A5,1X,I5,1X,A8,1X,A8,1X,A8,1X,A1");

	if(c->OutputFormat==FSPK){
		/*                                                      */
		/* write FORTRAN statement for any phenotype variables: */
		/*                                                      */

		/*                                                */
		/* Now do FORTRAN statement for remaining fields: */
		/*                                                */
		for(charcount=i=0;i<c->db->n.po;i++){
			/*                      */
			/* get the field index: */
			/*                      */
			ii=c->db->fi->p[i];
			chars=c->db->FieldLength(c->db,ii);       /* field width ...                   */
			/*                                                                             */
			/* note that line break, "/", is shown first, commas thereafter:               */
			/*                                                                             */
			fprintf(c->streams.fpar,"%cA%i,1X",(i==0?'/':','),chars); /* ... printed here  */
			chars++;                                  /* add 1X spacer width               */
			charcount+=chars;                         /* running sum                       */
			if(charcount>=FSPMAXRECLENGTH-12){        /* time to break a line              */
				fprintf(c->streams.fpar,"/");         /* show line break in FORTRAN statement */
				charcount=0;                          /* reset for next line                  */
			}

		}	
		/*                                               */
		/* Now do FORTRAN statement for genotype fields: */
		/*                                               */
		for(lastcount=i=0;i<c->db->n.go;i++){
			/*                                                                          */
			/* get the field index:                                                     */
			/*                                                                          */
			/* ii=c->db->fi->g[i];                                                      */
			/*                                                                          */
			chars=SIBPALGENOTYPEFIELDWIDTH;           /* fixed genotype field width ... */
			chars++;                                  /* add 1X spacer width            */
			charcount+=chars;                         /* running sum                    */
			if(charcount>=FSPMAXRECLENGTH-12){        /* time to break a line           */
			    /*                                                                      */
			    /* show comma or line break:                                            */
			    /*                                                                      */
				fprintf(c->streams.fpar,"%c",lastcount==0?',':'/');
				/*                                                                 */
				/* print a condensed statement for the number of genotypes so far: */
				/*                                                                 */
			    fprintf(c->streams.fpar,"%i(A%i,1X)",i+1-lastcount,SIBPALGENOTYPEFIELDWIDTH);
				charcount=0;                          /* reset for next line */
				lastcount=i+1;
			}

		}
		if(charcount){
		    /*                                                                         */
		    /* show final break:                                                       */
		    /*                                                                         */
			fprintf(c->streams.fpar,"/");         /* show line break in FORTRAN statement */
			/*                                                                            */
			/* need to print last condensed statement:                                    */
			/* -->i is already one past the last, so don't need to add 1:                 */
			/*                                                                            */
		    fprintf(c->streams.fpar,"%i(A%i,1X)",i-lastcount,SIBPALGENOTYPEFIELDWIDTH);
		}
	}
	
	fprintf(c->streams.fpar,")\n");
	/*                                        */
	/* done with parameter file, so close it: */
	/*                                        */
	fclose(c->streams.fpar);
	c->streams.fpar=NULL;
}


/*                               */
/*                               */
/* SIBPAL data header processor: */
/*                               */
/*                               */
/*                               */
void SibpalDataHeaderProcessor(PCONTROL c,char *fn){

	/*                                                   */
	/* verify that at least one trait variable is marked */
	/* for output: there can also be covariate variables */
	/* marked:                                           */
	/*                                                   */
	if((   g->OutputFormat==SIBP1
	    || g->OutputFormat==SIBP2
	    || g->OutputFormat==SIBP3) 
	   && c->db->n.po==0)
		MadelineErrorReport("SibpalDataHeaderProcessor():\n\tSibpal analyses type 1,2 and 3 require at least one phenotype trait");

	/*                                                    */
	/* For SIBPAL1,2,3,4 and FSP, we also write to a .par */
	/* parameter file:                                    */
	/*                                                    */
	StoreString(&gParameterOutputFile,fn);

	/*                                                         */
	/* change ending to .par by overwriting last 4 characters: */
	/*                                                         */
	strncpy(gParameterOutputFile.e.s+strlen(gParameterOutputFile.e.s)-4,
	        ".par",4);

	/*                                                       */
	/* Then open up the .par file for use by SibpalOutput(): */
	/*                                                       */
	c->streams.fpar=fopen(gParameterOutputFile.e.s,"wt");
	if(!c->streams.fpar)
		MadelineErrorReport("SibpalDataHeaderProcessor(): unable to open \"%s\" for output",gParameterOutputFile.e.s);

	lprintf("Creating associated SIBPAL parameter file called \"%s\"\n",gParameterOutputFile.e.s);

	/*                                                         */
	/* Call generic header processor to open main output file: */
	/*                                                         */
	GenericDataHeaderProcessor(c,fn);
	
	/*                                      */
	/* Call Sibpal specific header routine: */
	/*                                      */
	SibpalHeader(c);
}


/*                                           */
/*                                           */
/* SibpalHeader(): Writes the sibpal header: */
/*                                           */
/*                                           */
void SibpalHeader(PCONTROL c){

	int i,ii,j;
	int type;
	int ty;
	int ncv;
	int ntr;
	int width,prec;
	
	extern PARRAY pNmv;

	switch(c->OutputFormat){
		case SIBP1:
			type=1;
			break;
		case SIBP2:
			type=2;
			break;
		case SIBP3:
			type=3;
			break;
		case SIBP4:
			type=4;
			break;
	}
	/*                                                                */
	/* determine number of covariates and number of phenotype traits: */
	/*                                                                */
	ncv=c->db->n.vo;
	ntr=c->db->n.po-ncv;

	/*                                  */
	/* Record 1: Problem identification */
	/*                                  */
	fprintf(c->streams.fpar,"Input type %1i for SIBPAL created by Madeline for %-36.36s\n",type,c->db->nm);
	/*                                                */
	/* Record 2: Printing options: defaults are zero: */
	/*                                                */
	fprintf(c->streams.fpar,"   10   10   10    0    1\n");
	/*                                                                              */
	/* Record 3: Type of analysis: type of analysis, no. of marker loci,            */
	/* no. of traits, options 2=perform SLR and also MLR on                         */
	/* the pi-hats of all loci,0=no weights are used,                               */
	/* 1=plot y vs. pi-hat if sign.,                                                */
	/* and finally, the number of covariates (subtract one if age of onset option): */
	/*                                                                              */
	fprintf(c->streams.fpar,"%5i%5i%5i    1    0    0%5i    0\n",
		type,
		c->db->n.go,
		ntr,
		c->OutputFormat==SIBP3?ncv-1:ncv);
	
	/*                                                                          */
	/* Record 4: covariates: repeat this record for each covariate:             */
	/* but if the analysis is SIBP3, then don't print a record for the          */
	/* very first covariate, which is assumed to be the age of onset covariate: */
	/*                                                                          */
	for(j=i=0;j<ncv && i<c->db->n.po;i++){
		/*                  */
		/* get field index: */
		/*                  */
		ii=c->db->fi->p[i];
		/*                            */
		/* limit to covariate fields: */
		/*		                          */
		if(c->db->ff[ii]&COVARIATESET){
			/*                                                 */
			/* if analysis type 3, skip the first covar. which */
			/* is supposed to be the age of onset covar which  */
			/* is treated separately:                          */
			/*                                                 */
			if(c->OutputFormat==SIBP3 && j==0){
				j++;
				continue;
			}
		
			/*                                                             */
			/* use value in cell [1] of NMV[] for missing value indicator: */
			/*                                                             */
			fprintf(c->streams.fpar,"%-10.10s%5i%s\n",
				c->db->FieldName(c->db,ii),
				1,
				FormatNumber(pNmv->e[1].v,NULL,5,0));
			j++;
		}
	}
	/*                                                        */
	/* Record 5: repeat this record for each phenotype trait: */
	/*                                                        */
	/*                                                        */
	if(type!=4){
		for(j=i=0;j<ntr && i<c->db->n.po;i++){
			/*                  */
			/* get field index: */
			/*                  */
			ii=c->db->fi->p[i];
			/*                                                 */
			/* limit to just phenotype, not covariate, fields: */
			/*                                                 */
			if(   c->db->ff[ii]&PHENOTYPESET 
			   && !(c->db->ff[ii]&COVARIATESET)
			){
				/*                                                            */
				/* print out the following: name of trait,transformation      */
				/* (0=none, 1=log, 2=sqrt),missing value code,                */
				/* affection code=1 by default,                               */
				/* not affected code=0 by default                             */
				/*                                                            */
				/*                                                            */
				/* use the value in cell [1] of NMV[] for missing value code: */
				/*                                                            */
				if(type==1){
					fprintf(c->streams.fpar,"%-10.10s%5i%s\n",
				      c->db->FieldName(c->db,ii),
				      0,
				      FormatNumber(pNmv->e[1].v,NULL,5,0));
				}else{
					fprintf(c->streams.fpar,"%-10.10s%5i%s%5.0f%5.0f\n",
				      c->db->FieldName(c->db,ii),
				      0,
				      FormatNumber(pNmv->e[1].v,NULL,5,0),
				      1.0,
				      0.0);
				}
				j++;
			}
		}
	}
	/*                                                                */
	/* The following two records are included only if analysis type 3 */
	/* is performed:                                                  */
	/*                                                                */
	if(c->OutputFormat==SIBP3){
		/*                                 */
		/* Record 6.1: Age of onset option */
		/*                                 */
		/*                                 */
		
		/*                                                              */
		/* again use value in cell [1] of NMV[] for missing value code: */
		/*                                                              */
		fprintf(c->streams.fpar,  "%s%10.0f%10.4f%10.4f\n",
		      FormatNumber(pNmv->e[1].v,NULL,10,0),
		      1.0,
		      0.0,
		      1.0);
				
		/*                                                 */
		/* Record 6.2: Susceptibilities for 1st-6th class: */
		/*                                                 */
		for(i=0;i<6;i++)
			fprintf(c->streams.fpar,"%10.4f",1.0);
		fprintf(c->streams.fpar,"\n");
	}
	/*                                                       */
	/* Record 7: Format for the family Data file records:    */
	/*                                                       */
	/* fixed-fields are studycode, family id, individual id: */
	/*                                                       */
	fprintf(c->streams.fpar,"(A5,1X,I5,1X,A8,1X");
	/*                                                                  */
	/* now output FORTRAN formats for all phenotype fields:             */
	/* (both traits and covariates, including ago of onset, if present) */
	/* --but only do this for analyses 1-3: not for type 4:             */
	/*                                                                  */
	if(c->OutputFormat!=SIBP4){
		/*                              */
		/* phenotype traits loop first: */
		/*                              */
		for(j=i=0;j<ntr && i<c->db->n.po;i++){
			/*                  */
			/* get field index: */
			/*                  */
			ii=c->db->fi->p[i];
			
			if(   c->db->ff[ii]&PHENOTYPESET
			   && !(c->db->ff[ii]&COVARIATESET)
			){
				/*                                     */
				/* print the FORTRAN format specifier: */
				/*                                     */
				width=c->db->FieldLength(c->db,ii);
				prec =c->db->FieldPrecision(c->db,ii);
				if((ty=c->db->FieldType(c->db,ii))=='C'){
					/*                                                      */
					/* actually, I don't believe "C" fields are allowed by  */
					/* SIBPAL as trait fields!:                             */
					/*                                                      */
					fprintf(c->streams.fpar,",A%i,1X",width);
				}else if(ty=='N'){
					fprintf(c->streams.fpar,",F%i.%i,1X",width,prec);
				}else{
					/*                                                 */
					/* Call generic footer processor to close streams: */
					/*                                                 */
					GenericDataFooterProcessor(c);
					MadelineErrorReport("SibpalHeader(): %s is %c: only C and N field types are currently supported",
						c->db->FieldName(c->db,i),ty);
				}
				j++;
			}
		}
		/*                                                   */
		/* covariates loop follows after phenotype loop:     */
		/* this includes the age of onset, if specified. ncv */
		/* properly counts this:                             */
		/*                                                   */
		for(j=i=0;j<ncv && i<c->db->n.po;i++){
			/*                  */
			/* get field index: */
			/*                  */
			ii=c->db->fi->p[i];
			/*                  */
			/* covariates only: */
			/*                  */
			if(c->db->ff[ii]&COVARIATESET){
				/*                                     */
				/* print the FORTRAN format specifier: */
				/*                                     */
				width=c->db->FieldLength(c->db,ii);
				prec =c->db->FieldPrecision(c->db,ii);
				if((ty=c->db->FieldType(c->db,ii))=='C'){
					/*                                                      */
					/* actually, I don't believe "C" fields are allowed by  */
					/* SIBPAL as trait fields!:                             */
					/*                                                      */
					fprintf(c->streams.fpar,",A%i,1X",width);
				}else if(ty=='N'){
					fprintf(c->streams.fpar,",F%i.%i,1X",width,prec);
				}else{
					/*                                                 */
					/* Call generic footer processor to close streams: */
					/*                                                 */
					GenericDataFooterProcessor(c);
					MadelineErrorReport("SibpalHeader(): %s is %c: only C and N field types are currently supported",
						c->db->FieldName(c->db,i),ty);
				}
				j++;
			}
		}
	}                       
	/*                                                           */
	/* now output FORTRAN formats for all genotype fields:       */
	/*                                                           */
	/*                                                           */
	/* Since it is known that all GENOTYPE fields are character, */
	/* and since we know that all genotypes will print in A9,    */
	/* we just output a condensed FORTRAN statement: this solves */
	/* the problem that SIBPAL doesn't like a FORTRAN statement  */
	/* that exceeds 80 character line, or that exceeds three     */
	/* such 80-char. lines:                                      */
	/*                                                           */
	fprintf(c->streams.fpar,",%i(A9,1X)",c->db->n.go);
		
	/*                                              */
	/* terminating parentheses and carriage return: */
	/*                                              */
	fprintf(c->streams.fpar,")\n");
	/*                            */
	/* close the parameters file: */
	/*                            */
	fclose(c->streams.fpar);
	c->streams.fpar=NULL;
}




/*                                                              */
/*                                                              */
/* OutputUserM13IndividualRecord()                              */
/*                                                              */
/* -> Can be used for UserM13                                   */
/* -> Identical to OutputMendelIndividualRecord() except that   */
/*    unconnecteds and childless spouses will also be processed */
/*    (they will be skipped by OutputMendelIndividualRecord())  */
/*                                                              */
void OutputUserM13IndividualRecord(PCONTROL c,PINDIVIDUAL p){

	extern char gMendelOutputMissingValue[];
	
	int i,ii;
		

	if(ExcludedForUserM13(p)) return;	

	/*                                       */
	/* Write core fixed fields (COREOUTSET): */
	/*                                       */
	fprintf(c->streams.fp,"%-8.8s %-8.8s %-8.8s %c %c ",
		p->id,
		p->fid[0]==CHARMISSINGVALUE?gMendelOutputMissingValue:p->fid,
		p->mid[0]==CHARMISSINGVALUE?gMendelOutputMissingValue:p->mid,
		(p->female==MISSING?' ':(p->female?'F':'M')),
		p->twin && isupper(p->twin)?p->twin:' '
	);
	
	/*                                     */
	/* now loop and print fields:          */
	/* note that there will be NO data for */
	/* the inserted fathers and mothers,   */
	/* hence need to check db for NULL:    */
	/*                                     */
		
	/*                  */
	/* Genotype fields: */
	/*                  */
	for(i=0;i<c->db->n.go;i++){
		/*                      */
		/* Get the field index: */
		/*                      */
		ii=c->db->fi->g[i];
		OutputMendelField(c,p,ii,1);
	}
			
	/*                                                  */
	/* UserM13 format specialization:                   */
	/*                                                  */
	/*                                                  */
	/* print weighting column: "1." for each individual */
	/* in I3 format:                                    */
	/*                                                  */
	fprintf(c->streams.fp," 1.");                                       

	/*                              */
	/* Terminating carriage return: */
	/*                              */
	fprintf(c->streams.fp,"\n");
	/*                            */
	/* Mark individual as output: */
	/*                            */
	p->included++;
}



/*                                                              */
/*                                                              */
/* OutputRelpairIndividualRecord()                              */
/*                                                              */
/* -> Used for Relpair                                          */
/* -> Identical to OutputMendelIndividualRecord() except that   */
/*    unconnecteds and childless spouses will also be processed */
/*    (they will be skipped by OutputMendelIndividualRecord())  */
/*                                                              */
void OutputRelpairIndividualRecord(PCONTROL c,PINDIVIDUAL p){
	
	extern char gMendelOutputMissingValue[];
	
	int i,ii;
	
	
	if(ExcludedForUserM13(p)) return;	

	/*                                       */
	/* Write core fixed fields (COREOUTSET): */
	/*                                       */
	fprintf(c->streams.fp,"%-8.8s %-8.8s %-8.8s %c %c ",
		p->id,
		p->fid[0]==CHARMISSINGVALUE?gMendelOutputMissingValue:p->fid,
		p->mid[0]==CHARMISSINGVALUE?gMendelOutputMissingValue:p->mid,
		(p->female==MISSING?' ':(p->female?'F':'M')),
		p->twin && isupper(p->twin)?p->twin:' '
	);
	
	/*                                     */
	/* now loop and print fields:          */
	/* note that there will be NO data for */
	/* the inserted fathers and mothers,   */
	/* hence need to check db for NULL:    */
	/*                                     */
		
	/*                  */
	/* Genotype fields: */
	/*                  */
	for(i=0;i<c->db->n.go;i++){
		/*                      */
		/* Get the field index: */
		/*                      */
		ii=c->db->fi->g[i];
		OutputMendelField(c,p,ii,1);
	}
			
	/*                              */
	/* Terminating carriage return: */
	/*                              */
	fprintf(c->streams.fp,"\n");
	/*                            */
	/* Mark individual as output: */
	/*                            */
	p->included++;
}




/*                                                 */
/*                                                 */
/* FisherOutput(): outputs data for Mendel FISHER: */
/*                                                 */
/* - Used for FISHER output:                       */
/*                                                 */
void FisherOutput(PCONTROL c,PPEDIGREE pd){

	int noutput;
	int nprobands;


	/*                                           */
	/* Clear p->hasdata flags:                   */
	/*                                           */
	/*TraverseIndividualBSTClearDataFlag(pd->h); */

	/*                                                            */
	/* Check all individuals for data under the current criteria: */
	/*                                                            */
	TraverseIndividualBSTMarkDataFlag(pd->h,MendelIndividualHasData);


	noutput=FisherPedigreeHasData(c,pd);	
	if(noutput){
	
		if(c->OutputFormat==FISH1){
			/*                                                         */
			/* For Fisher with ascertainment correction, we want to    */
			/* note each pedigree that has probands, and how many      */
			/* probands there are (see code for PedigreeHasProband()); */
			/*                                                         */
			nprobands=PedigreeHasProband(pd);
		}else{
			nprobands=0;
		}	
		fprintf(c->streams.fp,"%3i %3i %-8.8s\n",
			nprobands,
			noutput,
			pd->fid);
			
		/*                   */
		/* Visit proband(s): */
		/*                   */
		TraverseIndividualBSTVisitProband(c,pd->h,OutputMendelIndividualRecord);
		/*                       */
		/* Visit non-proband(s): */
		/*                       */
		TraverseIndividualBSTVisitNonProband(c,pd->h,OutputMendelIndividualRecord);
		
	}
		
}



/*                                            */
/*                                            */
/* UserM13Output(): outputs data for UserM13: */
/*                                            */
/* - Used for USERM13 output                  */
/*                                            */
void UserM13Output(PCONTROL c,PPEDIGREE pd){

	int noutput;	

	/*                                           */
	/* Clear p->hasdata flags:                   */
	/*                                           */
	/*TraverseIndividualBSTClearDataFlag(pd->h); */

	/*                                                            */
	/* Check all individuals for data under the current criteria: */
	/*                                                            */
	TraverseIndividualBSTMarkDataFlag(pd->h,UserM13IndividualHasData);

	noutput=UserM13PedigreeHasData(c,pd);

	if(noutput){
		fprintf(c->streams.fp,"%3i %-8.8s\n",noutput,pd->fid);
		TraverseIndividualBST2(c,pd->h,OutputUserM13IndividualRecord);
	}
		
}


/*                                                              */
/*                                                              */
/* RelpairOutput(): outputs data for Relpair:                   */
/*                                                              */
/* - Like USERM13, Relpair can process EVERYBODY in a pedigree, */
/*   including unattached and childless spouses                 */
/*                                                              */
/* - The data format is identical with MENDEL                   */
/*                                                              */
void RelpairOutput(PCONTROL c,PPEDIGREE pd){

	int noutput;	

	/*                                                                  */
	/* Use UserM13 data criteria routines to figure out how many people */
	/* will be output, since the criteria are identical:                */
	/*                                                                  */

	/*                                                            */
	/* Check all individuals for data under the current criteria: */
	/*                                                            */
	TraverseIndividualBSTMarkDataFlag(pd->h,UserM13IndividualHasData);


	noutput=UserM13PedigreeHasData(c,pd);

	if(noutput){
		fprintf(c->streams.fp,"%3i %-8.8s\n",noutput,pd->fid);
		TraverseIndividualBST2(c,pd->h,OutputRelpairIndividualRecord);
	}

}

/*                       */
/* PedigreeHasProband(): */
/*                       */
/*                       */
int PedigreeHasProband(PPEDIGREE pd){

	/*                                   */
	/* look for proband (or index case): */
	/*                                   */
	return TraverseIndividualBST7(pd->h,IndividualIsProband);
}







/*                                                            */
/* FSPOutput(): writes the data file which serves as input to */
/* the SAGE FSP module                                        */
/*                                                            */
void FSPOutput(PCONTROL c,PPEDIGREE pd){

	int i;
	
	/*                                           */
	/* Clear hasdata flags:                      */
	/*                                           */
	/*TraverseIndividualBSTClearDataFlag(pd->h); */

	/*                                                            */
	/* Check all individuals for data under the current criteria: */
	/*                                                            */
	TraverseIndividualBSTMarkDataFlag(pd->h,GeneralCaseIndividualHasData);


	/*                                                             */
	/* return if sufficient data is not available in the pedigree: */
	/*	                                                            */
	if(!GeneralCasePedigreeHasData(c,pd)) return;
		
	/*                          */
	/* Output founding fathers: */
	/*                          */
	for(i=0;i<pd->nff;i++)
		OutputFSPIndividualRecord(c,pd,pd->ff[i]);
	/*                          */
	/* Output founding mothers: */
	/*                          */
	for(i=0;i<pd->nfm;i++)
		OutputFSPIndividualRecord(c,pd,pd->fm[i]);
		
	for(i=0;i<pd->nfm;i++){
		TraverseOffspring2(c,pd,pd->fm[i],OutputFSPIndividualRecord);
	}

}

/*                              */
/* OutputFSPIndividualRecord(): */
/*                              */
void OutputFSPIndividualRecord(PCONTROL c,PPEDIGREE pd,PINDIVIDUAL p){
	
	extern char gMendelOutputMissingValue[];
	
	int i,ii;
	char hold[MINIMUMBUFFERSIZE];
	int charcount=0,rowcount=0;
	
	if(ExcludedForGeneralCase(p)) return;
	
	/*                                                             */
	/* Core information in record 1:                               */
	/*                                                             */
	/* Study ID(A5),FamilyID(I5),Indiv. ID,Father ID, MotherID,Sex */
	/*                                                             */
	fprintf(c->streams.fp,"FSP   %-5.5s %-8.8s %-8.8s %-8.8s %c \n",
		NumerizeID(hold,pd->fid),
		p->id,
		p->fid[0]==CHARMISSINGVALUE?gMendelOutputMissingValue:p->fid,
		p->mid[0]==CHARMISSINGVALUE?gMendelOutputMissingValue:p->mid,
		p->female==MISSING?'?':p->female?'F':'M');

	/*                                                    */
	/* return if FSP0 format (only core fields required): */
	/*                                                    */
	if(c->OutputFormat==FSP0K) return;
	/*                                              */
	/* return if only the core fields are involved: */
	/*                                              */
	if(!(c->db->n.po || c->db->n.go)) return;
	/*                                                          */
	/* Additional individual information in subsequent records: */
	/*                                                          */

	/*                                               */
	/* first loop through the PHENOTYPE trait fields */
	/*                                               */
	for(i=0;i<c->db->n.po;i++){
		/*                  */
		/* Get field index: */
		/*                  */
		ii=c->db->fi->p[i];
		/*                    */
		/* Output field data: */
		/*		                  */
		charcount+=OutputSibpalField(c,p,ii,0);
		if(charcount>=FSPMAXRECLENGTH-12){
			fprintf(c->streams.fp,"\n");
			charcount=0;
			rowcount++;
		}
	}

	/*                  */
	/* genotype fields: */
	/*                  */
	for(i=0;i<c->db->n.go;i++){
		/*                  */
		/* Get field index: */
		/*                  */
		ii=c->db->fi->g[i];
		/*                    */
		/* Output field data: */
		/*                    */
		charcount+=OutputSibpalField(c,p,ii,1);
		if(charcount>=FSPMAXRECLENGTH-12){
			fprintf(c->streams.fp,"\n");
			charcount=0;
			rowcount++;
		}
	}
	/*                            */
	/* Limit of six rows on data: */
	/*                            */
	if(rowcount>5) lprintf("OutputFSPIndividualRecord(): Data will not fit within six-record limit!!!\n");
	
	/*                                        */
	/* terminate record with carriage return: */
	/*                                        */
	if(charcount<FSPMAXRECLENGTH-12)
		fprintf(c->streams.fp,"\n");
	/*                            */
	/* Mark individual as output: */
	/*                            */
	p->included++;		
}


/*                 */
/* SibpalOutput(): */
/*                 */
void SibpalOutput(PCONTROL c,PPEDIGREE pd){
	int i;
		

	/*                                           */
	/* clear hasdata flags:                      */
	/*                                           */
	/*TraverseIndividualBSTClearDataFlag(pd->h); */

	/*                                                            */
	/* Check all individuals for data under the current criteria: */
	/*                                                            */
	TraverseIndividualBSTMarkDataFlag(pd->h,GeneralCaseIndividualHasData);


	/*                                                             */
	/* return if sufficient data is not available in the pedigree: */
	/*	                                                            */
	if(!GeneralCasePedigreeHasData(c,pd)) return;
	
		
	/*                          */
	/* Output founding fathers: */
	/*                          */
	for(i=0;i<pd->nff;i++)
		OutputSibpalIndividualRecord(c,pd,pd->ff[i]);
	/*                          */
	/* Output founding mothers: */
	/*                          */
	for(i=0;i<pd->nfm;i++)
		OutputSibpalIndividualRecord(c,pd,pd->fm[i]);
		
	for(i=0;i<pd->nfm;i++){
		TraverseOffspring2(c,pd,pd->fm[i],OutputSibpalIndividualRecord);
	}
	
}


/*                                */
/* OutputSibpalIndividualRecord() */
/*                                */
void OutputSibpalIndividualRecord(PCONTROL c,PPEDIGREE pd,PINDIVIDUAL p){


	char hold[MINIMUMBUFFERSIZE];
	int i;
	int ii;
	int j;
	int ntr,ncv;


	if(ExcludedForGeneralCase(p)) return;


	/*                                      */
	/* Fixed fields:                        */
	/* Study ID(A5),FamilyID(I5),Indiv. ID: */
	/*                                      */
	fprintf(c->streams.fp,"SIBPL %-5.5s %-8.8s ",
		NumerizeID(hold,pd->fid),
		p->id);
	
	/*                                                               */
	/* phenotype fields: output for Sibpal analyses type 1-3, not 4: */
	/*                                                               */
	if(c->OutputFormat!=SIBP4){
		ncv=c->db->n.vo;
		ntr=c->db->n.po-ncv;

		/*                                               */
		/* first loop through the PHENOTYPE trait fields */
		/*                                               */
		for(j=i=0;j<ntr && i<c->db->n.po;i++){
			/*                  */
			/* get field index: */
			/*                  */
			ii=c->db->fi->p[i];
			
			if(  c->db->ff[ii]&PHENOTYPESET &&
			   !(c->db->ff[ii]&COVARIATESET)
			){
				OutputSibpalField(c,p,ii,0);
				j++;
			}
		}
		/*                                             */
		/* now, loop only through the COVARIATE fields */
		/*                                             */
		for(j=i=0;j<ncv && i<c->db->n.po;i++){
			/*                  */
			/* get field index: */
			/*                  */
			ii=c->db->fi->p[i];
			
			if(c->db->ff[ii]&OUTPUTSET    &&
			   c->db->ff[ii]&COVARIATESET
			){
				OutputSibpalField(c,p,ii,0);
				j++;
			}
		}

	}

	/*                  */
	/* genotype fields: */
	/*                  */
	for(i=0;i<c->db->n.go;i++){
		/*                  */
		/* Get field index: */
		/*                  */
		ii=c->db->fi->g[i];
		/*                        */
		/* Output the field data: */
		/*                        */
		OutputSibpalField(c,p,ii,1);
	}       
	/*                                        */
	/* terminate record with carriage return: */
	/*                                        */
	fprintf(c->streams.fp,"\n");
	/*                            */
	/* Mark individual as output: */
	/*                            */
	p->included++;
	
}

/*                                                                   */
/* OutputSibpalField():                                              */
/*                                                                   */
/* p: individual                                                     */
/* i: field index                                                    */
/* j: justification flag: 1 for right-justifying the genotype fields */
/*    otherwise zero                                                 */
/*                                                                   */
int OutputSibpalField(PCONTROL c,PINDIVIDUAL p,int i,int j){
	
	extern char gMendelOutputMissingValue[];
	
	char hold[MINIMUMBUFFERSIZE];
	char  *o;
	double v;
	int ty,wide,prec;
	
	extern PARRAY pNmv;
	

	wide=j?SIBPALGENOTYPEFIELDWIDTH:c->db->FieldLength(c->db,i);
	prec=c->db->FieldPrecision(c->db,i);
	ty  =c->db->FieldType(c->db,i);
	
	if(p->db && !p->excluded){      		
		if(ty=='C'){
			/*                             */
			/* read character string data: */
			/*                             */
			o=p->db->ReadString(p->db,hold,i,p->rci);
			/*                                                               */
			/* check for missing values:                                     */
			/*                                                               */
			/* if(IsCMV(o)) o=gMendelOutputMissingValue;                     */
			/*                                                               */
			/* if(!strcmp(o,CHARMISSINGSTRING)) o=gMendelOutputMissingValue; */
			/*                                                               */
			if(!o[0]) o=gMendelOutputMissingValue;
			
			/*                                                */
			/* write to output file: make the genotype fields */
			/* a fixed A7 and see what happens:               */
			/*                                                */
			if(j) fprintf(c->streams.fp,"%*.*s ",wide,wide,o);
			else  fprintf(c->streams.fp,"%-*.*s ",wide,wide,o);
						
		}else if(ty=='N'){
			/*                    */
			/* read numeric data: */
			/*                    */
			v=p->db->ReadDouble(p->db,i,p->rci);
			/*                                           */
			/* check for missing values and standardize: */
			/*                                           */
			
			/*			                           */
			/* if(IsNMV(v)) v=pNmv->e[1].v; */
			/*                              */
			if(v==DOUBLEMISSINGVALUE) v=pNmv->e[1].v;
			
			/*                                */
			/* print out the formatted value: */
			/*                                */
			fprintf(c->streams.fp,"%s ",FormatNumber(v,NULL,wide,prec));
		}
	}else{
		/*                                                                         */
		/* person is dummied-in or is non-terminal excluded                        */
		/* not actually in the database: access to the database header is via gdb: */
		/*                                                                         */
		/*                                                                         */
		/* o and v are set to the missing value indicators:                        */
		/*                                                                         */
		o=gMendelOutputMissingValue;
		v=pNmv->e[1].v;

		/*                 */
		/* output by type: */
		/*                 */
		if(ty=='C'){
			fprintf(c->streams.fp,"%-*.*s ",wide,wide,o);                                             
		}else if(ty=='N'){
			/*                                */
			/* print out the formatted value: */
			/*                                */
			fprintf(c->streams.fp,"%s ",FormatNumber(v,NULL,wide,prec));
		}               
	}
	/*                                          */
	/* return the width plus the extra spacing: */
	/*                                          */
	return wide+1;
}


	
/*                                                                  */
/* NumerizeID(): "Numerizes" an ID by changing any non-numeric      */
/* characters into their ASCII equivalent.  The result thus retains */
/* the unique character of the original ID, but of course the ID    */
/* may now be longer.                                               */
/*                                                                  */
char *NumerizeID(char *b,char *s){

	char *bb,*ss;
	int i;
	
	for(bb=b,ss=s;*ss;){
		if(isdigit(*ss)) *bb++=*ss++;
		else{
			/* use the ASCII value of the character: */
			i=sprintf(bb,"%i",*ss++);
			bb+=i;          
		}
	}
	/*                               */
	/* don't forget null terminator: */
	/*                               */
	*bb=*ss;
	return b;
}


/*                             */
/* TypeIndividual(PINDIVIDUAL) */
/*                             */
//void TypeIndividual(PINDIVIDUAL p){
//
//	p->typed=HasGenotypeData(p);
//
//}


/*                                             */
/*                                             */
/* CrimapOutput(): added to parser.c on 970915 */
/*                                             */
/*                                             */
void CrimapOutput(PCONTROL c,PPEDIGREE pd){

	/* int i,j; */
	/* PINDIVIDUAL p; */
	int noutput;
	char hold[SMALLBUFFERSIZE];

	/*                             */
	/* Mark Individuals with data: */
	/*                             */
	TraverseIndividualBSTMarkDataFlag(pd->h,GeneralCaseIndividualHasData);
	
	/*                                                       */
	/* Check all individuals for data under Crimap criteria: */
	/*                                                       */
	noutput=CrimapPedigreeHasData(c,pd);
	/*                                 */
	/* Get here if pedigree is usable: */
	/*                                 */
	if(noutput){
		/*               */
		/* print header: */
		/*               */
		fprintf(c->streams.fp,"%s %i\n",NumerizeID(hold,pd->fid),noutput);
		TraverseIndividualBST2(c,pd->h,OutputCrimapIndividualRecord);
	}
}




/*                      */
/* TraverseOffspring(): */
/*                      */
void TraverseOffspring(PCONTROL c,PINDIVIDUAL pm,void (*func)(PCONTROL c,PINDIVIDUAL)){

	int i,j;
	PINDIVIDUAL p;

	/*                                                     */
	/* pm must be female so we can traverse her offspring: */
	/*                                                     */
	if(!pm->female)
		MadelineErrorReport("TraverseOffspring(): non-female \"mom\" encountered: %s\n",pm->id);
	/*                                     */
	/* nothing to do if there are no kids: */
	/*                                     */
	if(pm->nk<1) return;
	/*                                                         */
	/* first loop: traverse kids, calling the                  */
	/* supplied function to operate on them and their spouses: */
	/*                                                         */
	for(i=0;i<pm->nk;i++){
		p=pm->k[i];
		/*                        */
		/* operate on the person: */
		/*                        */
		func(c,p);
		/*                                 */
		/* now on spouse(s) who have kids: */
		/*                                 */
		for(j=0;j<p->na;j++)
			if(p->a[j]->nk)
				func(c,p->a[j]);  
	}
	/*                               */
	/* second loop: recursive calls: */
	/*                               */
	for(i=0;i<pm->nk;i++){
		p=pm->k[i];
		/*                                 */
		/* only operate if they have kids: */
		/*                                 */
		if(p->nk>0){
			/*                                                 */
			/* if not female, run through all of male's mates: */
			/*                                                 */
			if(!p->female){
				for(j=0;j<p->na;j++){
					TraverseOffspring(c,p->a[j],func);                                
				}
			}else{
				TraverseOffspring(c,p,func);
			}
		}
	}       
}



/*                                                                               */
/* TraverseOffspring2(): same as TraverseOffspring, but with PPEDIGREE pd added: */
/*                                                                               */
void TraverseOffspring2(PCONTROL c,PPEDIGREE pd,PINDIVIDUAL pm,void (*func)(PCONTROL,PPEDIGREE,PINDIVIDUAL)){

	int i,j;
	PINDIVIDUAL p;

	/*                                                     */
	/* pm must be female so we can traverse her offspring: */
	/*                                                     */
	if(!pm->female)
		MadelineErrorReport("TraverseOffspring2(): non-female \"mom\" encountered: %s\n",pm->id);
	/*                                     */
	/* nothing to do if there are no kids: */
	/*                                     */
	if(pm->nk<1) return;
	/*                                                         */
	/* first loop: traverse kids, calling the                  */
	/* supplied function to operate on them and their spouses: */
	/*                                                         */
	for(i=0;i<pm->nk;i++){
		p=pm->k[i];
		/*                        */
		/* operate on the person: */
		/*                        */
		func(c,pd,p);
		/*                                     */
		/* now on the spouse(s) who have kids: */
		/*                                     */
		for(j=0;j<p->na;j++)
			if(p->a[j]->nk)
				func(c,pd,p->a[j]);  
	}
	/*                               */
	/* second loop: recursive calls: */
	/*                               */
	for(i=0;i<pm->nk;i++){
		p=pm->k[i];
		/*                                 */
		/* only operate if they have kids: */
		/*                                 */
		if(p->nk>0){
			/*                                                 */
			/* if not female, run through all of male's mates: */
			/*                                                 */
			if(!p->female){
				for(j=0;j<p->na;j++){
					TraverseOffspring2(c,pd,p->a[j],func);                                
				}
			}else{
				TraverseOffspring2(c,pd,p,func);
			}
		}
	}       
}



/*                                                                              */
/* TraverseOffspring3(): same as TraverseOffspring2,                            */
/* but applies the visit function, func(), to ALL spouses, even spouses without */
/* children.                                                                    */
/*                                                                              */
void TraverseOffspring3(PCONTROL c,PPEDIGREE pd,PINDIVIDUAL pm,void (*func)(PCONTROL,PPEDIGREE,PINDIVIDUAL)){

	int i,j;
	PINDIVIDUAL p;

	/*                                                     */
	/* pm must be female so we can traverse her offspring: */
	/*                                                     */
	if(!pm->female)
		MadelineErrorReport("TraverseOffspring2(): non-female \"mom\" encountered: %s\n",pm->id);
	/*                                     */
	/* nothing to do if there are no kids: */
	/*                                     */
	if(pm->nk<1) return;
	/*                                                         */
	/* first loop: traverse kids, calling the                  */
	/* supplied function to operate on them and their spouses: */
	/*                                                         */
	for(i=0;i<pm->nk;i++){
		p=pm->k[i];
		/*                        */
		/* operate on the person: */
		/*                        */
		func(c,pd,p);
		/*                                      */
		/* now on the spouse(s)                 */
		/* regardless of whether they have kids */
		/* or not:                              */
		/*                                      */
		for(j=0;j<p->na;j++)
				func(c,pd,p->a[j]);  
	}
	/*                               */
	/* second loop: recursive calls: */
	/*                               */
	for(i=0;i<pm->nk;i++){
		p=pm->k[i];
		/*                                 */
		/* only operate if they have kids: */
		/*                                 */
		if(p->nk>0){
			/*                                                 */
			/* if not female, run through all of male's mates: */
			/*                                                 */
			if(!p->female){
				for(j=0;j<p->na;j++){
					TraverseOffspring3(c,pd,p->a[j],func);                                
				}
			}else{
				TraverseOffspring3(c,pd,p,func);
			}
		}
	}       
}


/*                                */
/*                                */
/* OutputCrimapIndividualRecord() */
/*                                */
/*                                */
void OutputCrimapIndividualRecord(PCONTROL c,PINDIVIDUAL p){

	int i,ii;
	char hold[SMALLBUFFERSIZE];
	
	char *o;
	char *allele1;
	char *allele2;

	/*                                */
	/* Don't do anything if excluded: */
	/*                                */
	if(ExcludedForCrimap(p)) return;
	
	/*                                                                   */
	/* first output id,mother's id,father's id, and sex as 0 for female, */
	/* 1 for male, and 3 if unknown:                                     */
	/*                                                                   */
	
	/*             */
	/* individual: */
	/*             */
	fprintf(c->streams.fp,"%s ",NumerizeID(hold,p->id));
	/*         */
	/* mother: */
	/*         */
	if(!p->mid[0] || p->mid[0]==CHARMISSINGVALUE)
		fprintf(c->streams.fp,"0 ");
	else
		fprintf(c->streams.fp,"%s ",NumerizeID(hold,p->mid));     /* mother */
	/*                                                                   */
	/* father:                                                           */
	/*                                                                   */
	if(!p->fid[0] || p->fid[0]==CHARMISSINGVALUE)
		fprintf(c->streams.fp,"0 ");
	else
		fprintf(c->streams.fp,"%s ",NumerizeID(hold,p->fid));     /* father */

	/*                                                             */
	/* sex, followed by a return to make the output more readable: */
	/*                                                             */
	fprintf(c->streams.fp,"%c\n",p->female==MISSING?'3':(p->female?'0':'1'));


	/*                                  */
	/* Now do just the genotype fields: */
	/*                                  */
	if(p->db && !p->excluded){
		/*                                                  */
		/* print out only genotype fields in natural order: */
		/*                                                  */
		for(i=0;i<p->db->n.go;i++){
			/*                  */
			/* Get field index: */
			/*                  */
			ii=p->db->fi->g[i];
			/*                          */
			/* read the genotype field: */
			/*                          */
			o=p->db->ReadString(p->db,hold,ii,p->rci);
			/*                                           */
			/* check for missing values:                 */
			/*                                           */
			/* if(IsCMV(o)) o="0/0";                     */
			/*                                           */
			/* if(!strcmp(o,CHARMISSINGSTRING)) o="0/0"; */
			/*                                           */
			if(!o[0]){
				allele1=allele2="0";
			}else{			
				/*                                     */
				/* get allele1 and allele2 substrings: */
				/*                                     */
				for(allele1=allele2=o;*allele2 && *allele2!='/';allele2++);
				*allele2++='\0';
			}
			fprintf(c->streams.fp,"%*.*s %*.*s ",
				CRIMAPALLELEFIELDWIDTH,
				CRIMAPALLELEFIELDWIDTH,
				allele1,
				CRIMAPALLELEFIELDWIDTH,
				CRIMAPALLELEFIELDWIDTH,
				allele2
			);
		}       
	}else{
		/*                                                      */
		/* print out zeros for both alleles of genotype fields: */
		/*                                                      */
		allele1=allele2="0";
		for(i=0;i<c->db->n.go;i++){
			fprintf(c->streams.fp,"%*.*s %*.*s ",
				CRIMAPALLELEFIELDWIDTH,
				CRIMAPALLELEFIELDWIDTH,
				allele1,
				CRIMAPALLELEFIELDWIDTH,
				CRIMAPALLELEFIELDWIDTH,
				allele2
			);
		}
	}
	fprintf(c->streams.fp,"\n");
	/*                            */
	/* Mark individual as output: */
	/*                            */
	p->included++;
	
}




/*                                                */
/* ReadFlagField(): Reads a field from the        */
/* database as a flag which can have              */
/* the values of 1 (TRUE), 0 (FALSE), or MISSING: */
/*                                                */
signed char ReadFlagField(PDBF db,int i,UINT32 rci){

	INT32 iv;

	iv=db->ReadInt32(db,i,rci);
	/*                                        */
	/*if(iv==INT32MISSINGVALUE || IsNMV(iv))  */
	/*                                        */
	if(iv==INT32MISSINGVALUE)
		return (signed char) MISSING;
	else
		return (signed char) iv;
}


/*            */
/* Verbose(): */
/*            */
void Verbose(void){

	extern PCONTROL g;
	
	g->Verbose=1;
	g->Silent =0;
	lprintf("Madeline is now in verbose mode.\n");
}

/*          */
/* Quiet(): */
/*          */
void Quiet(void){

	extern PCONTROL g;
	
	g->Verbose=0;
	g->Silent =0;
}

/*            */
/* Silence(): */
/*            */
void Silence(void){

	extern PCONTROL g;
	
	g->Verbose=0;
	g->Silent =1;
}



/*                                       */
/* void AssignSequentialID(PINDIVIDUAL): */
/*                                       */
void AssignSequentialID(PCONTROL c,PINDIVIDUAL p){
	
	/*                                                       */
	/* Assign the ID, then increment global seq. id counter: */
	/*                                                       */
	p->sid=c->isid++;
};


/*                                          */
/* void OutputSolarRelateIndividualRecord() */
/*                                          */
void OutputSolarRelateIndividualRecord(PCONTROL c,PPEDIGREE pd,PINDIVIDUAL p){

	fprintf(c->streams.fp,"%5i%5i%5i%5i %1i\n",
		p->sid,
		pd->sid,
		p->f?p->f->sid:0,
		p->m?p->m->sid:0,
		p->twin && isupper(p->twin)?1:0);
	/*                                     */
	/* place map into detail file for now: */
	/*                                     */
	dprintf("Individual %s in pedigree %s has been mapped to ped. # %5i seq. id# %5i\n",
		p->id,
		p->pd->fid,
		pd->sid,
		p->sid);
	/*                            */
	/* Mark individual as output: */
	/*                            */
	p->included++;

}


/*                     */
/*                     */
/* SolarRelateOutput() */
/*                     */
void SolarRelateOutput(PCONTROL c,PPEDIGREE pd){

	int i;

	/*                                                             */
	/* return if sufficient data is not available in the pedigree: */
	/*	                                                            */
	if(!GeneralCasePedigreeHasData(c,pd)) return;
	
	/*                               */
	/* Step 1: assign sequential ids */
	/*                               */
	
	/*                         */
	/* Assign the pedigree id: */
	/*                         */
	pd->sid=c->psid++;
	
	/*                                                    */
	/* Assign sequential ids to founders of the pedigree: */
	/*                                                    */
	for(i=0;i<pd->nff;i++)
		AssignSequentialID(c,pd->ff[i]);

	for(i=0;i<pd->nfm;i++)
		AssignSequentialID(c,pd->fm[i]);
	/*                                                          */
	/* Now traverse offspring of founding mothers, recursively, */
	/* and assign sequential ids to them:                       */
	/*                                                          */
	for(i=0;i<pd->nfm;i++)
		TraverseOffspring(c,pd->fm[i],AssignSequentialID);

	/*                                                     */
	/* Step 2: output everyone in sorted order:            */
	/* (actually, it would probably be OK to output people */
	/* unsorted, but it might be hard to read the file, so */
	/* we'll do it the nice way) :                         */
	/*                                                     */
	
	/*                      */
	/* Output the founders: */
	/*                      */
	for(i=0;i<pd->nff;i++)
		OutputSolarRelateIndividualRecord(c,pd,pd->ff[i]);	
	for(i=0;i<pd->nfm;i++)
		OutputSolarRelateIndividualRecord(c,pd,pd->fm[i]);
	/*                                             */
	/* Traverse Offspring to output everyone else: */
	/*                                             */
	for(i=0;i<pd->nfm;i++)
		TraverseOffspring2(c,pd,pd->fm[i],OutputSolarRelateIndividualRecord);
	/*       */
	/* done: */
	/*       */
	return;	
}



/*                             */
/*                             */
/* SolarFisherOutput()         */
/* OBOLETE - SHOULD BE REMOVED */
/*                             */
void SolarFisherOutput(PCONTROL c,PPEDIGREE pd){

	int i;
	
	/*                                                             */
	/* return if sufficient data is not available in the pedigree: */
	/*	                                                            */
	if(!GeneralCasePedigreeHasData(c,pd)) return;
	
	/*                               */
	/* Step 1: assign sequential ids */
	/*                               */
	
	/*                         */
	/* Assign the pedigree id: */
	/*                         */
	pd->sid=c->psid++;
	
	/*                                                    */
	/* Assign sequential ids to founders of the pedigree: */
	/*                                                    */
	for(i=0;i<pd->nff;i++)
		AssignSequentialID(c,pd->ff[i]);

	for(i=0;i<pd->nfm;i++)
		AssignSequentialID(c,pd->fm[i]);
	/*                                                          */
	/* Now traverse offspring of founding mothers, recursively, */
	/* and assign sequential ids to them:                       */
	/*                                                          */
	for(i=0;i<pd->nfm;i++)
		TraverseOffspring(c,pd->fm[i],AssignSequentialID);

	/*                                                     */
	/* Step 2: output everyone in sorted order:            */
	/* (actually, it would probably be OK to output people */
	/* unsorted, but it might be hard to read the file, so */
	/* we'll do it the nice way) :                         */
	/*                                                     */

	/*                                       */
	/* Fisher 1 format: ascertain on proband */
	/*                                       */
	/*                                       */
	if(c->OutputFormat==SFIS1 && PedigreeHasProband(pd))
		/*                              */
		/* Fisher 1 ascertained format: */
		/*                              */
		fprintf(c->streams.fp,"%3i %3i %-8.8s\n",1,pd->n-pd->ex,pd->fid);
	else
		/*                                  */
		/* Fisher 0 non-ascertained format: */
		/*                                  */
		fprintf(c->streams.fp,"%3i %3i %-8.8s\n",0,pd->n-pd->ex,pd->fid);

	/*                                              */
	/* Output the proband:                          */
	/* Put out proband first (required by FISHER):  */
	/*                                              */
	/* HACK:                                        */
	/* This occurs automatically with FUSION IDs in */
	/* an PINDIVIDUAL BST                           */
	/*                                              */
	/* OBSOLETE CODE: SHOULD BE REMOVED !!!         */
	/*                                              */
	TraverseIndividualBST2(c,pd->h,OutputSolarFisherIndividualRecord);
}


/*                                                                   */
/* void OutputSolarFisherIndividualRecord()                          */
/*                                                                   */
/* THE CODE HERE FOR OutputSolarFisherIndividualRecord() is OBSOLETE */
/*                                                                   */
void OutputSolarFisherIndividualRecord(PCONTROL c,PINDIVIDUAL p){
	
	extern char gMendelOutputMissingValue[];
	
	int wide;
	int prec;
	char hold[MINIMUMBUFFERSIZE];
	char *o;
	char ty;
	double v;
	int j,l;
	
	/*                                          */
	/* studyid,fatherid,motherid,               */
	/* egoid,father ego, mother ego, sex, twin: */
	/*	                                         */
	fprintf(c->streams.fp,"%8.8s %8.8s %8.8s %5i%5i%5i %c %c ",
		p->id,
		p->fid[0]==CHARMISSINGVALUE?gMendelOutputMissingValue:p->fid,
		p->mid[0]==CHARMISSINGVALUE?gMendelOutputMissingValue:p->mid,
		p->sid,
		p->f?p->f->sid:0,
		p->m?p->m->sid:0,
		p->female==MISSING?' ':p->female?'F':'M',
		p->twin && isupper(p->twin)?p->twin:' ');
	/*                                                               */
	/* now, run through whatever fields are marked as output fields: */
	/*                                                               */

	/*                                     */
	/* now loop and print fields:          */
	/* note that there will be NO data for */
	/* the inserted fathers and mothers,   */
	/* hence need to check db for NULL:    */
	/*                                     */
	if(p->db){
		/*                      */
		/* loop through fields: */
		/*                      */
		for(j=0;j< p->db->fc;j++){
			/*                 */
			/* skip COREOUTSET */
			/*                 */
			if(p->db->ff[j] & COREOUTSET) continue;
			/*                         */
			/* skip deselected fields: */
			/*                         */
			if(!(p->db->ff[j] & OUTPUTSET)) continue;
			/*                            */
			/* restrict width for MENDEL: */
			/*                            */
			if((wide=p->db->FieldLength(p->db,j))>MAXIMUMMENDELFIELDWIDTH)
				wide=MAXIMUMMENDELFIELDWIDTH;
			/*                       */
			/* read data:            */
			/*                       */
			if((ty=p->db->FieldType(p->db,j))=='C'){
				/*                             */
				/* read character string data: */
				/*                             */
				o=p->db->ReadString(p->db,hold,j,p->rci);
				/*                                                               */
				/* check for missing values:                                     */
				/*                                                               */
				/* if(IsCMV(o)) o=gMendelOutputMissingValue;                     */
				/*                                                               */
				/* if(!strcmp(o,CHARMISSINGSTRING)) o=gMendelOutputMissingValue; */
				/*                                                               */
				if(!o[0]) o=gMendelOutputMissingValue;
				
				/*                       */
				/* write to output file: */
				/*                       */
				fprintf(c->streams.fp,"%-*.*s ",wide,wide,o);
				
			}else if(ty=='N'){
				/*                    */
				/* read numeric data: */
				/*                    */
				v=p->db->ReadDouble(p->db,j,p->rci);
				/*                           */
				/* check for missing values: */
				/*                           */
				/* if(IsNMV(v)){             */
				/*                           */
				if(v==DOUBLEMISSINGVALUE){				
					fprintf(c->streams.fp,"%-*.*s ",wide,wide,gMendelOutputMissingValue);
				}else{
					/*                                       */
					/* check precision (adjust if too wide): */
					/*                                       */
					prec=p->db->FieldPrecision(p->db,j);
					if(wide==MAXIMUMMENDELFIELDWIDTH && prec>HALFMAXMENDELFIELDWIDTH)
						prec=HALFMAXMENDELFIELDWIDTH;
						
					fprintf(c->streams.fp,"%s ",FormatNumber(v,NULL,wide,prec));
				}
			}
		}
	}else{
		/*                                                                           */
		/* person is dummied-in,                                                     */
		/* not actually in the database: access to the database header is via gdb:   */
		/*                                                                           */
		/* Some formats, like basic MENDL, can just terminate here with a CR.        */
		/* However, others, like UM13, require an additional column after everything */
		/* else, so we have to write out blanks for each field:                      */
		/*                                                                           */
		if(c->OutputFormat==UM13 || c->OutputFormat==SFIS0 || c->OutputFormat==SFIS1){
			for(j=0;j<c->db->fc;j++){
				/*                 */
				/* skip COREOUTSET */
				/*                 */
				if(c->db->ff[j] & COREOUTSET) continue;
				/*                         */
				/* skip deselected fields: */
				/*                         */
				if(!(c->db->ff[j] & OUTPUTSET)) continue;
				/*                                               */
				/* check, and if necessary adjust, field width:  */
				/*                                               */
				if((wide=c->db->FieldLength(c->db,j))>MAXIMUMMENDELFIELDWIDTH)
					wide=MAXIMUMMENDELFIELDWIDTH;
				/*                                              */
				/* spit out spaces for all the missing fields   */
				/* of the dummied-in record, plus the one extra */
				/* spacer field :                               */
				/*                                              */
				for(l=0;l<=wide;l++) fputc(' ',c->streams.fp);                            
			}
		}
	}
		
	/*                                                      */
	/* finish up with reduplicated ego ids (who knows why?) */
	/* and the carriage return:                             */
	/*                                                      */
	fprintf(c->streams.fp,"%5i.%5i.\n",p->sid,p->sid);
	/*                            */
	/* Mark individual as output: */
	/*                            */
	p->included++;

}


/*                                                              */
/* Turn():  This is the entry point for all of the TURN and SET */
/* commands.  The list of these keeps growing as the program    */
/* becomes more sophisticated -- see the latest documentation   */
/* for a rundown on all of the various settings and options.    */
/*                                                              */
void Turn(void){

	register unsigned short op;
	RESULT result;
		
	extern PCONTROL g;
	
	/*                */
	/* First keyword  */
	/*                */
	GetToken();

	op=pT->i;

	/*                 */
	/* Field ordering: */
	/*                 */
	if(op==FLDK){
		/*                              */
		/* Call UserSetFieldOrdering(): */
		/*                              */
		UserSetFieldOrdering(g,g->db);
		return;
	}
	/*                    */
	/* Language settings: */
	/*                    */
	if(op==LANGK){
		/*                         */
		/* Call UserSetLanguage(): */
		/*                         */
		UserSetLanguage(g);
		/*                                      */
		/* Run Hello() only if in verbose mode: */
		/*                                      */
		if(g->Verbose) Hello();
		return;
	}
	/*                                       */
	/* Pedigree drawing orientation options: */
	/*                                       */
	if(op==ORIEK){
		/*                          */
		/* Set drawing orientation: */
		/*                          */
		GetToken();
		if(pT->i!=TOC) MadelineErrorReport("Turn(): TO keyword expected.");
		GetToken();
		op=pT->i;
		switch(op){
		case AUTPK:
			g->d.Orientation=AUTOMATICORIENTATION;
			break;
		case LANDK:
			g->d.Orientation=LANDSCAPEORIENTATION;
			break;
		case PORTK:
			g->d.Orientation=PORTRAITORIENTATION;
			break;
		case MLTPK:
			g->d.Orientation=MULTIPAGEORIENTATION;
			break;
		default:
			MadelineErrorReport("Turn(): AUTOMATIC, LANDSCAPE, PORTRAIT, or MULTIPAGE keyword expected");
			break;
		}
		/*                                */
		/* TEMPORARY SITUATION:           */
		/*                                */
		/* This is a temporary situation: */
		/*                                */
		if(gDrawing){
			FreeDrawing(gDrawing);
			gDrawing=NULL;
		}
				
		/*   */
		/*   */
		/*		 */
		if(g->Verbose) Hello();
		return;
	}
	/*                      */
	/* Paper size settings: */
	/*                      */
	if(op==SZPAK){
		GetToken();
		if(pT->i!=TOC) MadelineErrorReport("Turn(): TO keyword expected.");
		GetToken();
		op=pT->i;
		switch(op){
		case SZLTK:
			g->d.PaperSize = USLETTERSIZE;
			g->d.PageWidth = 21.59;
			g->d.PageHeight= 27.94;
			break;
		case SZLGK:
			g->d.PaperSize=USLEGALSIZE;
			g->d.PageWidth = 21.59;
			g->d.PageHeight= 35.56;
			break;
		case SZA4K:
			g->d.PaperSize=A4SIZE;
			g->d.PageWidth = 21.0;
			g->d.PageHeight= 29.7;
			break;
		case SZALK:
			g->d.PaperSize=A4LONGSIZE;
			g->d.PageWidth = 21.0;
			g->d.PageHeight= 33.9;
			break;
		case SZASK:
			g->d.PaperSize=A4SUPERSIZE;
			g->d.PageWidth = 22.86;
			g->d.PageHeight= 38.10;
			break;
		default:
			MadelineErrorReport("Turn(): USLETTER, USLEGAL, A4, A4LONG, or A4SUPER keyword expected");
			break;
		}
		/*                                */
		/* TEMPORARY SITUATION:           */
		/*                                */
		/* This is a temporary situation: */
		/*		                              */
		if(gDrawing){
			FreeDrawing(gDrawing);
			gDrawing=NULL;
		}
		if(g->Verbose) Hello();
		return;	
	}
	/*                       */
	/* Paper margin setting: */
	/*                       */
	if(op==PMRGK){
		/*                   */
		/* Get "TO" keyword: */
		/*                   */
		GetToken();
		if(pT->i!=TOC) MadelineErrorReport("Turn(): TO keyword expected.");
		/*                                                           */
		/* get value: EvaluateExpression() starts with a GetToken(): */
		/*                                                           */
		EvaluateExpression(&result);
		if(result.t!=NUMERICVALUE)
			MadelineErrorReport("Numeric expression or value expected after TO.");
		if(result.v>=1.0 && result.v<g->d.PageWidth/2.0)
			g->d.PageMargin=result.v;
		else
			MadelineErrorReport("Value for margin is out of range.");

		/*                                */
		/* TEMPORARY SITUATION:           */
		/*                                */
		/* This is a temporary situation: */
		/*                                */
		if(gDrawing){
			FreeDrawing(gDrawing);
			gDrawing=NULL;
		}
		if(g->Verbose) Hello();
		return;
	}
	if(op==HELPC){
		/*                           */
		/* set help to ... TEXT|HTML */
		/*                           */
		
		/*                   */
		/* Get "TO" keyword: */
		/*                   */
		GetToken();
		if(pT->i!=TOC) MadelineErrorReport("Turn(): TO keyword expected.");
		/*                               */
		/* Get "HTML" OR "TEXT" keyword: */
		/*                               */
		GetToken();
		op=pT->i;
		switch(op){
		case HTMLK:
			g->HtmlHelp=TRUE;
			break;
		case TEXTK:
			g->HtmlHelp=FALSE;
			break;
		default:
			MadelineErrorReport("Turn(): HTML or TEXT keywords expected");
			break;
		}
		if(g->Verbose) Hello();
		return;
	}	
	if(op==DLMBK){
		/*                           */
		/* set delimiter to ...      */
		/*                           */
		
		/*                   */
		/* Get "TO" keyword: */
		/*                   */
		GetToken();
		if(pT->i!=TOC) MadelineErrorReport("Turn(): TO keyword expected.");
		/*                               */
		/* Get Keyword or string:        */
		/*                               */
		GetToken();
		op=pT->i;
		switch(op){
		case SPCAK:
			g->delimiter=' ';
			break;
		case TABAK:
			g->delimiter='\t';
			break;
		case CMMAK:
			g->delimiter=',';
			break;
		default:
			g->delimiter=pT->e[0];
			break;
		}
		if(g->Verbose) Hello();
		return;
	}
	
	/*                            */
	/* remaining ON/OFF settings: */
	/*                            */
	
	GetToken();
	if(!(pT->i==ONK || pT->i==OFFK))
		MadelineErrorReport("Turn(): ON or OFF keyword expected");
	
	switch(op){
	case AUTOK:
		g->AutoDetect =(pT->i==ONK);
		break;
	case AXCLK:
		g->AutoExclude=(pT->i==ONK);
		break;
	case COLRK:
		g->d.InColor=(pT->i==ONK);
		break;
	case RSHDK:
		g->d.ReverseShading=(pT->i==ONK);
		break;
	case SVAFK:
		g->SaveMarkerInfo=(pT->i==ONK);
		/*                                                             */
		/* database must be opened in order to save the marker vector: */
		/*                                                             */
		if(g->SaveMarkerInfo && !g->db){
			g->SaveMarkerInfo=0;
			MadelineErrorReport("Turn(): a pedigree table must be opened in order to save allele frequency information");
		}
		break;
	case DIVDK:
		g->d.DividedDrawings=(pT->i==ONK);
		break;
	case HAPLK:
		g->Haplotype=(pT->i==ONK);
		break;
	case FUSNK:
		g->FusionSupport=(pT->i==ONK);
		break;
	case LBLVI:
		g->LabelCreatedVirtuals=(pT->i==ONK);
		break;
	case HIGHL:
		g->HighlightRows=(pT->i==ONK);
		break;
	case AINHK:
		g->AutoCheckInheritance=(pT->i==ONK);
		break;
	case CNSLK:
		g->ConsoleHighlights=(pT->i==ONK);
		break;
	case MAPDT:
		g->MapDetails=(pT->i==ONK);
		break;
	default:
		
		MadelineErrorReport("Turn(): the keyword \"%s\" is not currently recognized by this command.",pT->e);
		break;
	}
	/*                   */
	/* Show flag status: */
	/*                   */
	Hello();
}



/*                                                  */
/* SiblinkOutput(): Entry point for Siblink Output: */
/*                                                  */
void SiblinkOutput(PCONTROL c,PPEDIGREE pd){

	int i,j;
	
	/*                                                        */
	/* Check all individuals for data under current criteria: */
	/*                                                        */
	TraverseIndividualBSTMarkDataFlag(pd->h,SiblinkIndividualHasData);

	/*                                                       */
	/* Count any unattached individuals or childless spouses */
	/* before anything else:                                 */
	/*                                                       */
	TraverseIndividualBST2(c,pd->h,CountExcludedUnattachedIndividual);

	/*                                                             */
	/* use c->psid to count nuclear blocks as is done in UserFQTL: */
	/* reset to zero here:                                         */
	/*                                                             */
	c->psid=0;
			
	/*                                                                    */
	/* proceed to output individual family blocks:                        */
	/* Loop over each founding mom, and each spouse of each founding mom: */
	/*                                                                    */
	for(i=0;i<pd->nfm;i++){
		for(j=0;j<pd->fm[i]->na;j++){
			SiblinkTraverseNuclearBlock(c,pd,pd->fm[i],pd->fm[i]->a[j],SiblinkSibPairBlockOutput);
		}
	}
		
	/*                                                                */
	/* if c->psid is now greater than zero, then at least one nuclear */
	/* block contained a sib pair that was output, so increment the   */
	/* full pedigree counter.  Otherwise, we can say that the entire  */
	/* full pedigree was excluded:                                    */
	/*                                                                */
	if(c->psid)
		c->counters.pedg.in++;
	else
		c->counters.pedg.ex++;		
}

/*                                                                           */
/*                                                                           */
/* SiblinkTraverseNuclearBlock():                                            */
/*                                                                           */
/* Traverses all sib pairs in a sibship                                      */
/* sharing the same mom, pm, and then does a recursive descent               */
/* on each sib or sib spouse, whichever is female.  A function pointer       */
/* of type void (*func)(PINDIVIDUAL,PINDIVIDUAL,int) must be passed          */
/* as the second parameter to operate on the pairs.  This is usually         */
/* an output function for printing out the affected sib pairs, etc.          */
/* Note that TraverseSibPairs() goes over all sib pairs, affected or not.    */
/*                                                                           */
/* If pm points to a founding mother, then all sib pairs from all successive */
/* generations will be operated on by the function passed via its pointer.   */
/*                                                                           */
void SiblinkTraverseNuclearBlock(PCONTROL c,PPEDIGREE pd,PINDIVIDUAL s1,PINDIVIDUAL s2,void (*func)(PCONTROL c,PPEDIGREE pd,PINDIVIDUAL p1,PINDIVIDUAL p2,int sss)){

	int i,j;
	int asc;        /* counter for no. of affected sibs in sibship */
	/* PINDIVIDUAL p; */
	PINDIVIDUAL mom;
	PINDIVIDUAL dad;

	/*                                                      */
	/* return if no children present: not a nuclear family: */
	/*                                                      */
	if(!s1->nk) return;

	/*                             */
	/* get pointer to mom and dad: */
	/*                             */
	if(s1->female==1){
		mom=s1;
		dad=s2;
	}else{
		mom=s2;
		dad=s1;
	}


	/*                                                                               */
	/* SiblinkNuclearFamilyHasData() returns 0 if the nuclear pedigree               */
	/* doesn't have data:                                                            */
	/* * When SiblinkAffected   is used, asc is the number of affected sibs...       */
	/* * When SiblinkUnaffected is used, asc is no. of unaffecteds...                */
	/* * When SiblinkAllPairs   is used, asc is no. of affect. and unaffect. sibs... */
	/* * When SiblinkDiscordant is used, asc is no. of affect. and unaffect. sibs... */
	/* The return                                                                    */
	/* value of any xxxxxxxHasData() function is the number of individuals,          */
	/* including the parents, so we just subtract off the two parents in order       */
	/* to get the number of affected sibs:	                                          */
	/*                                                                               */
	asc=SiblinkNuclearFamilyHasData(c,pd,mom,dad)-2;	
	
	/*                                                                 */
	/* First loop: run through sib pairs, if any present               */
	/*                                                                 */
	/* Note that the function "func" has access to all information     */
	/* in the pedigree, via pd, the first sib in the pair, pm->k[i],   */
	/* the second sib in the pair, pm->k[j], and to the total affected */
	/* sib count, asc:                                                 */
	/*                                                                 */
	if(asc>0){
		for(i=0;i<mom->nk;i++){
			/*                                 */
			/* limit to offspring of the pair: */
			/*                                 */
			if(mom->k[i]->f!=dad) continue;

			for(j=i+1;j<mom->nk;j++){
				/*                                 */
				/* limit to offspring of the pair: */
				/*                                 */
				if(mom->k[j]->f!=dad) continue;

				/*                                    */
				/* Call function to operate on pairs: */
				/*                                    */
				(*func)(c,pd,mom->k[i],mom->k[j],asc);
			}
		}
	}


	/*                                                       */
	/* Second loop: recursive descent for offspring of sibs: */
	/*                                                       */
	for(i=0;i<mom->nk;i++){
		/*                                */
		/* limit to offspring of the pair */
		/*                                */
		if(mom->k[i]->f!=dad) continue;

		/*                                                     */
		/* second loop to traverse all mates of the offspring: */
		/*                                                     */
		for(j=0;j<mom->k[i]->na;j++){
			SiblinkTraverseNuclearBlock(c,pd,mom->k[i],mom->k[i]->a[j],func);
		}
	}
}


/*                                                                                */
/* SiblinkNuclearFamilyHasData(): s1 and s2 are the nuclear family parental pair: */
/*                                                                                */
/*                                                                                */
int SiblinkNuclearFamilyHasData(PCONTROL c,PPEDIGREE pd,PINDIVIDUAL s1,PINDIVIDUAL s2){

	PINDIVIDUAL mom;
	PINDIVIDUAL dad;
	
	int i;
	int nio; /* number of offspring who will be included: */

	if(s1->female){
		mom=s1;
		dad=s2;
	}else{
		mom=s2;
		dad=s1;
	}
	
	
	/*                                                                        */
	/* For Siblink, at least ONE PAIR of offspring with data must be present: */
	/*                                                                        */
	for(nio=i=0;i<mom->nk;i++){
		if(mom->k[i]->f==dad && mom->k[i]->hasdata) nio++;
	}
	
	if(nio>=2){

		return CountIncludedNuclearPedigree(c,mom,dad);
						
	}else{
			
		nio=CountExcludedNuclearPedigree(c,mom,dad);
		dprintf("SiblinkNuclearFamilyHasData():\n\tExcluding %i individuals in pedigree %s in nuclear pedigree formed by %s and %s:\n\tno affected sib pair with genotype data\n",
			nio+2,
			pd->fid,
			mom->id,
			dad->id
		);
		return 0;
	}
}


/*                                                                            */
/* SiblinkSibPairBlockOutput(): Outputs nuclear family blocks consisting      */
/* of the parents of individuals p1 and p2, followed by p1 and p2 themselves  */
/* in Siblink format                                                          */
/*                                                                            */
/* famid: family ID                                                           */
/* p1,p2: the pair to output                                                  */
/* sss  : the size of the sibship, being the weighting factor used by Siblink */
/*                                                                            */
void SiblinkSibPairBlockOutput(PCONTROL c,PPEDIGREE pd,PINDIVIDUAL p1,PINDIVIDUAL p2,int sss){


	/*                                               */
	/* Don't output if no data for one or the other: */
	/*                                               */
	if(!(p1->hasdata && p2->hasdata)) return;
	/*                                              */
	/* In the discordant case, don't output if      */
	/* the two sibs have the same affection status: */
	/*                                              */
	if(c->OutputFormat==SIBLKD &&
		p1->affected==p2->affected) return;

	
	/*                                                         */
	/* Get here if pedigree included, so increment c->psid     */
	/* to indicate the inclusion of sibpairs from the pedigree */
	/*                                                         */
	c->psid++;

	/*                                                                           */
	/* Print FORTRAN family record information:                                  */
	/*                                                                           */
	/* Print sss for all formats except the discordant case, where we print zero */
	/* for the time being (981023):                                              */
	/*                                                                           */
	fprintf(c->streams.fp,"%4i %-8.8s %4i\n",4,pd->fid,c->OutputFormat==SIBLKD?0:sss);
	/*                        */
	/* Output father, mother: */
	/*                        */
	OutputSiblinkIndividualRecord(c,p1->f);
	OutputSiblinkIndividualRecord(c,p1->m);
	/*                      */
	/* Output the sib pair: */
	/*                      */
	OutputSiblinkIndividualRecord(c,p1);
	OutputSiblinkIndividualRecord(c,p2);
	
	/*                                                  */
	/* keep track of pedigrees that have been included  */
	/* in the output set:                               */
	/*                                                  */
	if(!pd->fl) pd->fl++;	
}


/*                                                          */
/* OutputSiblinkIndividualRecord():                         */
/*                                                          */
/* After the fixed set of fields (STUDYID, FATHER, MOTHER,  */
/* SEX, and MZTWIN status are output), the remaining        */
/* GENOTYPE OUTPUT set of fields are printed using ranks    */
/* in place of the allele labels: this routine is thus very */
/* similar to the GENEHUNTER output routine.                */
/*                                                          */
void OutputSiblinkIndividualRecord(PCONTROL c,PINDIVIDUAL p){
	
	extern char gMendelOutputMissingValue[];
	
	int   j,ii;
	char *allele1;
	char *allele2;
	int   nallele1;
	int   nallele2;
	char  hold[SMALLBUFFERSIZE];
	char *o;
	
	hold[0]='\0';


	/*                                                         */
	/* Print core fixed fields first: id, fid, mid, sex, twin: */
	/*                                                         */
	fprintf(c->streams.fp,"%-8.8s %-8.8s %-8.8s %c %c ",
		p->id,
		p->fid[0]==CHARMISSINGVALUE?gMendelOutputMissingValue:p->fid,
		p->mid[0]==CHARMISSINGVALUE?gMendelOutputMissingValue:p->mid,
		(p->female==MISSING?' ':(p->female?'F':'M')),
		p->twin && isupper(p->twin)?p->twin:' '
	);

	/*                                  */
	/* Now do just the genotype fields: */
	/*                                  */
	if(p->db){
		/*                                              */
		/* print out only genotype fields in map order: */
		/*                                              */
		for(j=0;j<p->db->n.go;j++){
			/*                  */
			/* Get field index: */
			/*                  */
			ii=p->db->fi->g[j];
			/*                          */
			/* read the genotype field: */
			/*                          */
			o=p->db->ReadString(p->db,hold,ii,p->rci);
			
			/*                                   */
			/* check for missing values:         */
			/*                                   */
			/* if(IsCMV(o)){                     */
			/*                                   */
			/* if(!strcmp(o,CHARMISSINGSTRING)){ */
			
			if(!o[0]){
				/*                                 */
				/* Print zeros for missing values: */
				/*                                 */
				fprintf(c->streams.fp,"%*i%*i",
					SIBLINKALLELEFIELDWIDTH,
					0,
					SIBLINKALLELEFIELDWIDTH,
					0
				);
				
			}else{
				/*                                     */
				/* get allele1 and allele2 substrings: */
				/*                                     */
				for(allele1=allele2=o;*allele2 && *allele2!='/';allele2++);
				*allele2++='\0';
			
				/*                                                  */
				/* Convert the alleles to their corresponding ranks */
				/*                                                  */
				nallele1=atoi(allele1);
				nallele2=atoi(allele2);
				if(nallele1==nallele2){
					nallele2=nallele1=GetAlleleRank(p->db->m+ii,nallele1);
				}else{
					nallele1=GetAlleleRank(p->db->m+ii,nallele1);
					nallele2=GetAlleleRank(p->db->m+ii,nallele2);
				}
				/*            */
				/* and print: */
				/*            */
				fprintf(c->streams.fp,"%*i%*i",
					SIBLINKALLELEFIELDWIDTH,
					nallele1,
					SIBLINKALLELEFIELDWIDTH,
					nallele2
				);
			}
		}       
	}else{
		/*                                                      */
		/* print out zeros for both alleles of genotype fields: */
		/*                                                      */
		allele1=allele2="0";
		for(j=0;j<c->db->n.go;j++){
			/*                                      */
			/* process only output genotype fields: */
			/*                                      */
			fprintf(c->streams.fp,"%*.*s%*.*s",
				SIBLINKALLELEFIELDWIDTH,
				SIBLINKALLELEFIELDWIDTH,
				allele1,
				SIBLINKALLELEFIELDWIDTH,
				SIBLINKALLELEFIELDWIDTH,
				allele2);
		}
	}
	fprintf(c->streams.fp,"\n");
	/*                            */
	/* Mark individual as output: */
	/*                            */
	p->included++;
}



/*                   */
/* InitializeControl */
/*                   */
void InitializeControl(PCONTROL c){

	c->db    =NULL; /* database      */
	c->pd    =NULL; /* pedigree tree */
	
	c->graphdb=NULL; /* graph database */
	
	c->cpd   =NULL; /* current pedigree   -- not always used */
	c->pd    =NULL; /* current individual -- not always used */

	c->m     =NULL; /* used for saving a previous PMARKER vector */
	c->ms    =0;    /* used for saving a previous PMARKER vector */

	c->mdb   =NULL; /* map database   */
	c->mp    =NULL; /* marker map BST */
	
	c->streams.fp    =NULL;  /* main output file for WRITE formats        */
	c->streams.fpar  =NULL;  /* associated parameter file                 */
	c->streams.fpar2 =NULL;  /* additional param. or ctl. file, if needed */
	c->streams.log   =NULL;  /* log file                                  */
	c->streams.detail=NULL;  /* detail file                               */
	c->streams.prn   =NULL;  /* Postscript pedigree drawing stream file   */
	c->streams.graph =NULL;  /* Postscript graph drawing stream file      */
	c->streams.err   =NULL;  /* serious data errors log file              */
	c->streams.command=NULL; /* Commands entered by the user log file     */
	
	/*                                               */
	/* Call ResetCounters to reset counters to zero, */
	/* sequential identifiers to 1:                  */
	/*                                               */
	ResetCounters(c);
	
	c->OutputFormat   = 0;     /* output format                                                         */
	c->AutoDetect     = TRUE;  /* do automatic field type detection: MUST BE ALWAYS TRUE in v. 1.0++    */
	c->AutoExclude    = TRUE;  /* automatic exclusions                                                  */
	c->Eliminate      = FALSE; /* FALSE= don't eliminate floaters: MUST BE ALWAYS FALSE IN v. 1.0++ !!! */
	c->Silent         = FALSE; /* don't be silent                                                       */
	c->Verbose        = TRUE;  /* do be verbose (as opposed to quiet)                                   */
	c->SaveMarkerInfo = FALSE; /* don't save marker information between Open()s.                        */
	c->HtmlHelp       = TRUE;  /* use HTML help                                                         */
	c->LabelCreatedVirtuals = TRUE; /* Label virtual individuals having random IDs created by Madeline  */
	c->HighlightRows  = TRUE;  /* Highlight alternating groups of 5 rows on pedigree drawings           */
	c->Haplotype      = FALSE; /* print genotypes the regular way, not as haplotypes                    */
	c->FusionSupport  = FALSE; /* FALSE= don't support FUSION customizations by default                 */
	c->AutoCheckInheritance = TRUE; /* Automatically check inheritance on OPENing a pedigree table      */
	c->ConsoleHighlights = TRUE;    /* Console highlighting (bold, color) on or off (default ON)        */
	c->MapDetails     = FALSE; /* FALSE= don't display map details (inter-marker distance, recom. fr)   */
	
	/*                                                                                                  */
	/* Call ResetErrorCounters() to reset the error and warning message                                 */
	/* counters:                                                                                        */
	/*                                                                                                  */
	ResetErrorCounters(c);
	
	 /* This defaults to American English for non-UTF-8 locales: */
	c->lang       =GetLanguageConventionFromLocale();
	
	c->sort       =0;       /* no sort expression present */
	
	c->delimiter  ='\t';     /* Use the tab character as the default delimiter */
	/*                             */
	/* Default drawing parameters: */
	/* Defines are now in config.h */
	/*                             */
	c->d.UnitOfMeasure      = CENTIMETERS;
	c->d.IconDiameter       = DP_ICON_DIAMETER;
	c->d.IconRadius         = DP_ICON_RADIUS;
	c->d.HorizontalInterval = DP_HZ_INTERVAL; 
	c->d.HalfHZInterval     = c->d.HorizontalInterval/2.0;
	c->d.CellWidth          = c->d.HorizontalInterval-DP_CELL_SPACING;
	c->d.TickLength         = DP_TICK_LENGTH;
	/*                                                                   */
	/* NOTE BENE: VerticalDrop1 and VerticalDrop2 are NEGATIVE:          */
	/* Don't forget this when looking at the drawing code!               */
	/*                                                                   */
	c->d.VerticalDrop1      = DP_VT_DROP1;  /* this depends on number of labels */
	c->d.VerticalDrop2      = DP_VT_DROP2;
	c->d.Orientation        = AUTOMATICORIENTATION;
	c->d.InColor            = TRUE;
	c->d.ReverseShading     = FALSE;
	c->d.DividedDrawings    = TRUE;
	c->d.CrossOverTags      = FALSE; /* Only used for haplotype displays */
	/*                                */
	/* Paper settings (ALWAYS IN CM): */
	/*                                */
	c->d.PaperSize  = USLETTERSIZE;
	c->d.PageHeight = 28.0;
	c->d.PageWidth  = 21.5;
	c->d.PageMargin = 1.0;
	
	/*                                                                   */
	/* 19990630 ET ADDENDUM:                                             */
	/*                                                                   */
	/* Assign gfn global variables to an indexable vector in the control */
	/* structure to simplify global access to these variables:           */
	/*                                                                   */
	c->ivar[AFFECTIONSTATUSFIELD_IVI]=&gfnAffected;
	c->ivar[CHROMOSOMEFIELD_IVI]     =&gfnMapChromosome;
	c->ivar[DATEOFBIRTHFIELD_IVI]    =&gfnDOB;
	c->ivar[DATEOFDEATHFIELD_IVI]    =&gfnDOD;
	c->ivar[DEATHSTATUSFIELD_IVI]    =&gfnDead;
	c->ivar[DZTWINFIELD_IVI]         =&gfnDZTwin;
	c->ivar[FAMILYIDFIELD_IVI]       =&gfnFamID;
	c->ivar[FATHERIDFIELD_IVI]       =&gfnFather;
	c->ivar[GENDERFIELD_IVI]         =&gfnSex;
	c->ivar[INDEXCASEFIELD_IVI]      =&gfnProband;
	c->ivar[INDIVIDUALIDFIELD_IVI]   =&gfnID;
	c->ivar[LIABILITYCLASSFIELD_IVI] =&gfnLClass;
	c->ivar[MARKERFIELD_IVI]         =&gfnMapMarker;
	c->ivar[MOTHERIDFIELD_IVI]       =&gfnMother;
	c->ivar[MZTWINFIELD_IVI]         =&gfnMZTwin;
	c->ivar[ORDINALFIELD_IVI]        =&gfnMapOrdinal;
	c->ivar[POSITIONFIELD_IVI]       =&gfnMapPosition;
	c->ivar[ALLELE1FIELD_IVI]        =&gfnAllele1;
	c->ivar[ALLELE2FIELD_IVI]        =&gfnAllele2;
	c->ivar[FEMALEPOSITIONFIELD_IVI] =&gfnMapFemalePosition;
	c->ivar[MALEPOSITIONFIELD_IVI]   =&gfnMapMalePosition;
	c->ivar[TERMINATOR_IVI]          =(PVARIABLE)NULL;

	/*                                              */
	/* Get the MADELINESYSTEM environment variable: */
	/*                                              */
	c->MadelineSystem=getenv("MADELINESYSTEM");
	/*                                              */
	/* 2000 05 09 ET ADDENDUM: If the environment   */
	/* variable MADELINESYSTEM does not exist, then */
	/* let's just check if things were installed in */
	/* the default location under                   */ 
	/* "/usr/local/madeline-v.vvv/" (where v.vvv is */
	/* the version number):                         */
	/*                                              */
	if(!c->MadelineSystem){
		char path[MINIMUMBUFFERSIZE];
		char *terminate;
		/* 2001.12.03 ET Addendum: Use path from AUTOCONF's config.h */
		sprintf(path,"%sdocumentation%smadeline.html",
			SHARE_PATH,
			DIRECTORYSEPARATOR
		);
		
		if(File(path)){
			/* Leave only the system path */
			/* without the final "/"      */
			terminate=Find("documentation",path);
			*(--terminate)='\0';
			c->MadelineSystem=CopyString(path);
		}
	}
}




/*                                                                 */
/* TraverseBinaryPedigreeTree2():                                  */
/*                                                                 */
/* Traverses a binary PEDIGREE tree, applying a user-supplied      */
/* function, func, to each node.  This function passes the control */
/* structure along to the user-supplied function                   */
/*                                                                 */
void TraverseBinaryPedigreeTree2(PCONTROL c,PPEDIGREE pd,void (*func)(PCONTROL,PPEDIGREE)){

	if(pd){
		if(pd->l) TraverseBinaryPedigreeTree2(c,pd->l,func);
		(*func)(c,pd);
		if(pd->r) TraverseBinaryPedigreeTree2(c,pd->r,func);
	}
}



/*                                                    */
/*                                                    */
/* OutputSiblinkMarker()                              */
/*                                                    */
/* NOTE: i is the rank index, not the field index.    */
/*       Recall that i is zero-offset, so need to add */
/*       one when writing rank information:           */
/*                                                    */
void OutputSiblinkMarker(PCONTROL c,int i){

	PMARKERMAP p;
	int ii;         /* the actual field index */
	char *fn;
	/*                      */
	/* Get the field index: */
	/*                      */
	ii=c->db->fi->g[i];
	/*                     */
	/* now get field name: */
	/*                     */
	fn=c->db->FieldName(c->db,ii);
	/*                                          */
	/* get pointer to map info for this marker: */
	/*                                          */
	p=LookupMarkerMap(c->mp,fn);
	
	if(!p){
		/*                   */
		/* clean up streams: */
		/*                   */
		GenericDataFooterProcessor(c);
		MadelineErrorReport("Can't find marker %s in map",c->db->FieldName(c->db,ii));
	}
	/*                                                                              */
	/* Print the marker's map order number, i, adding one because i is zero-offset: */
	/*                                                                              */
	fprintf(c->streams.fpar,"%4i\n",i+1);
	
	/*                                                                  */
	/* Distinguish between CurrentSiblink and (future) Siblink formats: */
	/*                                                                  */
	/*if(c->OutputFormat==CSIBL){                                       */
	/*                                                                  */
	
	
	/* 981023: No longer doing "future" format: just current: */
	
	/*                 */
	/* Current format: */
	/*                 */

	/*                                                              */
	/* Print the marker's map position in Morgans after the regular */
	/* MENDEL-style header:                                         */
	/*                                                              */
	fprintf(c->streams.fpar,"%-8.8sAUTOSOME%2i %7.5f\n",
		fn,
		c->db->m[ii].n,
		0.01*p->ps
		);
	/*                               */
	/* Reverse-traverse allele tree: */
	/*		                             */
	ReverseTraverseAllele2(c->streams.fpar,c->db->m[ii].a,c->db->m[ii].s,OutputCurrentFormatSiblinkMarkerAllele);

	/*	                                                                                          */
	/*}else{                                                                                     */
	/*	//                                                                                        */
	/*	// future format:                                                                         */
	/*	//                                                                                        */
	/*	                                                                                          */
	/*	//                                                                                        */
	/*	// Print the marker's map position in centiMorgans after the regular                      */
	/*	// MENDEL-style header:                                                                   */
	/*	//                                                                                        */
	/*	fprintf(c->streams.fpar,"%-8.8sAUTOSOME%3i %8.4lf\n",                                     */
	/*		fn,                                                                                      */
	/*		c->db->m[ii].n,                                                                          */
	/*		p->ps                                                                                    */
	/*		);                                                                                       */
	/*	                                                                                          */
	/*	TraverseAllele2(c->streams.fpar,c->db->m[ii].a,c->db->m[ii].s,OutputSiblinkMarkerAllele); */
	/*}                                                                                          */
	/*                                                                                           */
}

/*                                                   */
/*                                                   */
/* OutputSiblinkMarkerAllele: future proposed format */
/*                                                   */
void OutputSiblinkMarkerAllele(FILE *fp,PALLELE a,int s){

	fprintf(fp,"%3i     %6.4f                     <<< %3i\n",
		a->i,
		(double)a->n/(double)s,
		a->v);

}


/*                                                                           */
/*                                                                           */
/* OutputCurrentFormatSiblinkMarkerAllele: current actual format (980915 ET) */
/*                                                                           */
void OutputCurrentFormatSiblinkMarkerAllele(FILE *fp,PALLELE a,int s){

	fprintf(fp,"%2i     %6.4f                      <<< %3i\n",
		a->i,
		(double)a->n/(double)s,
		a->v);

}




/*                                */
/*                                */
/* OutputSageMarker()             */
/*                                */
/* - i is now just the rank index */
/*                                */
void OutputSageMarker(PCONTROL c,int i){

	int ii;
	/*                      */
	/* Get the field index: */
	/*                      */
	ii=c->db->fi->g[i];
	
	fprintf(c->streams.fp,"%-8.8s\n",c->db->FieldName(c->db,ii));
	
	TraverseAllele3(c->streams.fp,c->db->m+ii,c->db->m[ii].a,c->db->m[ii].s,OutputSageMarkerAllele);

}

/*                                        */
/*                                        */
/* OutputSageMarkerAllele: generic format */
/*                                        */
void OutputSageMarkerAllele(FILE *fp,PMARKER m,PALLELE a,int s){

	/*                                                               */
	/* Note that m->r is always set to the "next" rank, so it will   */
	/* be one beyond the highest rank encountered in a->i, hence we  */
	/* subtract one to see if a is the last allele, and if it is, we */
	/* print the terminating semicolon sequence.  Otherwise, just a  */
	/* plain newline character is printed:                           */
	/*                                                               */
	fprintf(fp,"%3i    =%6.4f%s",
		a->v,
		(double)a->n/(double)s,
		(m->r-1==a->i?";\n;\n":"\n")
		);
}

/*                                                           */
/* ConstructPedigree(): constructs the pedigree by           */
/* joining an individual to his/her parents, parents to one  */
/* another, and parents to children.  After that's done, the */
/* function determines the founding pair.                    */
/*                                                           */
void ConstructPedigree(PPEDIGREE pd){


	extern PCONTROL g;

	/*                                              */
	/* loop through all the people in the pedigree: */
	/*                                              */
	TraverseIndividualBST5(pd,pd->h,ConnectIndividual);

	/*                                              */
	/* Reconnect as yet unconnected FUSION spouses: */
	/*                                              */
	TraverseIndividualBST5(pd,pd->h,JoinFloatingSpouses);
	/*                                                  */
	/* Log counts of unconnected and childless spouses: */
	/*                                                  */
	TraverseIndividualBST5(pd,pd->h,CountAffectedsVirtualsAndUnconnecteds);
	
	/*                                                     */
	/* Now that pedigrees have been constructed, do        */
	/* bookkeeping on number of pedigrees and individuals: */
	/*                                                     */
	/*                                                     */
	PedigreeTally(g,pd);
	
		
	/*                     */
	/* Determine founders: */
	/*                     */
	TraverseIndividualBST5(pd,pd->h,DetermineFounders);
	
	/*                                                      */
	/* Erase flags indicating presence of singleton "twins" */
	/* and also check for unconfirmed twins, confirmed      */
	/* twins with different birth dates, etc.               */
	/*                                                      */
	CheckTwins(pd);

	/*                                                  */
	/* Report on pedigrees with UNCONNECTED INDIVIDUALS */
	/* and CHILDLESS SPOUSES                            */
	/*                                                  */
	if(pd->nun || pd->ncsp){
		dprintf("NOTE: Pedigree %s has ",pd->fid);
		if(pd->nun)  dprintf("%i unconnected individual%s",
			pd->nun,
			pd->nun>1?"s":"");
		if(pd->ncsp) dprintf("%s %i childless spouse%s",
			pd->nun?" and":"",
			pd->ncsp,
			pd->ncsp>1?"s":"");
		dprintf(".\n");		
	}
	
	/*                          */
	/* Check for consanguinity: */
	/*                          */
	CheckConsanguinity(pd);
			
}




/*                                                         */
/* void JoinFloatingSpouses(): connect unconnected spouses */
/* of childless couples:                                   */
/*                                                         */
/* This function is only good for FUSION-type IDs          */
/* where it is known that '500' and '700'-series IDs       */
/* are spouses of probands and proband sibs, respectively: */
/*                                                         */
void JoinFloatingSpouses(PPEDIGREE pd,PINDIVIDUAL p){


	PINDIVIDUAL s;           /* the spouse      */
	char spid[MAXIDWIDTH+1]; /* for spouse ID;	 */
	
	/*                                            */
	/* examine only those completely unconnected  */
	/* people who, assuming FUSION IDs, are known */
	/* to be spouses:                             */
	/*                                            */
	if(    IsFusionID(p->id) 
	   && !(p->f || p->m || p->na || p->nk) 
	   &&  (p->id[5]=='5' || p->id[5]=='7')
	){
		/*                   */
		/* create spouse ID: */
		/*                   */
		strcpy(spid,p->id);
		spid[5]=(p->id[5]=='5'?'1':'4');
		spid[8]='\0';
		/*                                          */
		/* attach their spouse, if found:           */
		/* (if not, IndividualLookup returns NULL): */
		/*                                          */
		s=IndividualLookup(pd->h,spid);
		/*                                 */
		/* if found, make all connections: */
		/*                                 */
		if(s){
			p->a[p->na]=s;
			p->na++;
			s->a[s->na]=p;
			s->na++;
			/*                            */
			/* Fail on same sex partners: */
			/*                            */
			if(p->female==s->female)
				PDErrorReport("JoinFloatingSpouses(): %s and %s appear to both be %s.\n",
					p->id,
					s->id,
					p->female==MISSING?"missing sex":p->female?"female":"male"
				);
		}
	}
}


/*                                                                      */
/* CountAffectedsVirtualsAndUnconnecteds():  2003.10.22.ET Addenda      */
/*                                                                      */
/* (1) Sum up the number of affecteds in each pedigree                  */
/* (2) Mark and count unconnected individuals                           */
/* (3) Sum up the number of inserted individuals                        */
/* (4) Also sum up the number of childless spouses                      */
/*                                                                      */
void CountAffectedsVirtualsAndUnconnecteds(PPEDIGREE pd,PINDIVIDUAL p){

	/*                           */
	/* Completely unconnecteds:  */
	/*                           */
	if(!(p->f || p->m || p->na || p->nk)){
		p->unconnected++;
		pd->nun++;
	}
	
	/*                                         */
	/* Connected but childless FUSION spouses: */
	/*                                         */
	if(!(p->f || p->m || p->nk) && p->na){
		p->csp++;
		pd->ncsp++;
	}
	
	/*                                                                   */
	/* Also note number of inserted virtual individuals in the pedigree: */
	/*                                                                   */
	if(!p->db) pd->nvi++;
	
	/*                                                                   */
	/* Also note number of affecteds:                                    */
	/*                                                                   */
	if(p->affected==1) pd->NumberOfAffected++;
	
}



/*                            */
/*                            */
/* GetMapData(): get map data */
/*                            */
/*                            */
void GetMapData(PCONTROL c,const char *fn){

	UINT32 i;

	if(!File(fn))
		MadelineErrorReport("GetMapData(): can't open \"%s\" for reading.\nPerhaps the path or file name is incorrect.",fn);
	/*           */
	/* if a map: */
	/*           */
	if(c->mp){
		/*                     */
		/* clean up old map:		 */
		/*                     */
		FreeMap(c->mp);
		lprintf("Removing old map data based on %s\n",gMapDatabase.e.s);
		c->mp=NULL;
	}
				
	/*                                                       */
	/* open the map database into g->mdb:                    */
	/*                                                       */
	/* This is done by first calling CharacterizeDatabase()  */
	/* to determine what type of database it is.             */
	/* Then, NewDatabase() instantiates a database object of */
	/* the correct type, file name, and open mode.           */
	/* Calling the OpenDatabase() member function for the    */
	/* object then opens and buffers the database:           */
	/*                                                       */
	c->mdb=NewDatabase(fn,READONLY,CharacterizeDatabase(fn));
	c->mdb->OpenDatabase(g->mdb);
	
	GoTo(c->mdb,0);    /* set pointer to the top */
	/*                                           */
	/* store field indexes for the known fields: */
	/*                                           */
	gfnMapChromosome.i     =c->mdb->FieldNumber(c->mdb,gfnMapChromosome.e.s    );
	gfnMapPosition.i       =c->mdb->FieldNumber(c->mdb,gfnMapPosition.e.s      );
	gfnMapPositionBP.i     =c->mdb->FieldNumber(c->mdb,gfnMapPositionBP.e.s    ); /* What is this really used for? Anything? Probably should remove it. -2003.01.30.ET */
	gfnMapOrdinal.i        =c->mdb->FieldNumber(c->mdb,gfnMapOrdinal.e.s       );
	gfnMapMarker.i         =c->mdb->FieldNumber(c->mdb,gfnMapMarker.e.s        );
	gfnMapFemalePosition.i =c->mdb->FieldNumber(c->mdb,gfnMapFemalePosition.e.s); /* 2003.01.30.ET addendum */
	gfnMapMalePosition.i   =c->mdb->FieldNumber(c->mdb,gfnMapMalePosition.e.s  ); /* 2003.01.30.ET addendum */
	
	/*                                      */
	/* Error message if fields not present: */
	/*                                      */
	if(gfnMapChromosome.i==MISSING){
		CloseDatabase(c->mdb);
		c->mdb=NULL;
		MadelineErrorReport("GetMapData(): Chromosome field \"%s\" not found in marker map table",gfnMapChromosome.e.s);
	}
	if(gfnMapPosition.i==MISSING){
		CloseDatabase(c->mdb);
		c->mdb=NULL;
		MadelineErrorReport("GetMapData(): Position field \"%s\" not found in marker map table",gfnMapChromosome.e.s);
	}
	if(gfnMapOrdinal.i==MISSING){
		CloseDatabase(c->mdb);
		c->mdb=NULL;
		MadelineErrorReport("GetMapData(): Ordinal (marker order) field \"%s\" not found in marker map table",gfnMapOrdinal.e.s);
	}
	if(gfnMapMarker.i==MISSING){
		CloseDatabase(c->mdb);
		c->mdb=NULL;
		MadelineErrorReport("GetMapData(): Marker name field \"%s\" not found in marker map table",gfnMapMarker.e.s);
	}
	
	/*                                                  */
	/* WARNING if only ONE sex-specific map is present: */
	/*                                                  */
	if( (gfnMapFemalePosition.i!=MISSING && gfnMapMalePosition.i==MISSING) ||
	    (gfnMapFemalePosition.i==MISSING && gfnMapMalePosition.i!=MISSING)
	){
		
		eprintf(WARNINGFLAG,"It appears that you are trying to load gender-specific maps, but only the %s map was found!\n",(gfnMapMalePosition.i==MISSING?"female":"male"));
		                                
	}
	
	if(gfnMapFemalePosition.i!=MISSING && gfnMapMalePosition.i!=MISSING){
		/*                                                     */
		/* Load all three maps: sex-averaged, female, and male */
		/*                                                     */
		for(i=0;i<c->mdb->rc;i++){
			
			c->mp=InsertMarkerMap(
				c->mp,
				c->mdb->ReadString(c->mdb,NULL,gfnMapMarker.i,i),
				c->mdb->ReadInt32(c->mdb,gfnMapChromosome.i,i),
				c->mdb->ReadInt32(c->mdb,gfnMapOrdinal.i,i),
				c->mdb->ReadDouble(c->mdb,gfnMapPosition.i,i),
				c->mdb->ReadDouble(c->mdb,gfnMapFemalePosition.i,i),
				c->mdb->ReadDouble(c->mdb,gfnMapMalePosition.i,i)
			);
		}
		
	}else{
		/*                                                     */
		/* Load only the sex-averaged map:                     */
		/*                                                     */
		for(i=0;i<c->mdb->rc;i++){
			
			c->mp=InsertMarkerMap(
				c->mp,
				c->mdb->ReadString(c->mdb,NULL,gfnMapMarker.i,i),
				c->mdb->ReadInt32(c->mdb,gfnMapChromosome.i,i),
				c->mdb->ReadInt32(c->mdb,gfnMapOrdinal.i,i),
				c->mdb->ReadDouble(c->mdb,gfnMapPosition.i,i),
				DOUBLEMISSINGVALUE,
				DOUBLEMISSINGVALUE
			);
		}
		
	}
	
	/*                                                      */
	/* get here if there were no errors and everything was  */
	/* successfully constructed:                            */
	/*                                                      */
	StoreString(&gMapDatabase,fn);
	lprintf("Marker maps based on %s are now installed.\n",gMapDatabase.e.s);
	
	if(c->db){
		AutoSetFieldOrdering(c,c->db);
	}
	/*                                                       */
	/* No reason to keep the map database open, so close it: */
	/*                                                       */
	CloseDatabase(c->mdb);
	c->mdb=NULL;
	
}



/*                                         */
/* Load: load a map of markers into memory */
/*                                         */
void Load(void){

	RESULT result;
	extern PCONTROL g;
	const char *fileToOpen;
	
	/*                                                */
	/* Get the file name for writing:                 */
	/* EvaluateExpression starts with a GetToken():   */
	/*                                                */
	EvaluateExpression(&result);  /* get the filename */
	
	if(result.t!=STRINGVALUE)
		MadelineErrorReport("Load(): specify a marker map table to load");
	
	/*                                            */
	/* SmartTableSelect figures out whether the   */
	/* user specified the .data or the .mfh file  */
	/* and checks the usability of the .mfh file. */
	/* If the .mfh does not exist, recognize() is */
	/* called to create it:                       */
	/*                                            */
	fileToOpen=SmartTableSelect(result.s);
	GetMapData(g,fileToOpen);
	
}	

/*                                                               */
/* DisplayMarkerMapInformationHeader(): print the heading on the */
/* table displaying map information:                             */
/*                                                               */
void DisplayMarkerMapInformationHeader(void){
	
	extern PCONTROL g;;
	
	if(g->MapDetails){
		lprintf("\n                   Sex-averaged Map   Female-specific Map  Male-specific Map\n");
		lprintf("                  ------------------- ------------------- -------------------\n");
		lprintf("Ch Or Marker Name Kosambi   Dist (RF) Kosambi   Dist (RF) Kosambi   Dist (RF)\n");
		lprintf("-- -- ----------- --------- --------- --------- --------- --------- ---------\n");     
	}else{
		lprintf("\n                    Map Position (Kosambi cM)\n");
		lprintf("                  -----------------------------\n");
		lprintf("Ch Or Marker Name Sex-avg.   Female     Male\n");
		lprintf("-- -- ----------- --------- --------- ---------\n");
	}
}

/*                                                                    */
/* DisplayMarkerMapInformation(): display the map info for one marker */
/*                                                                    */
void DisplayMarkerMapInformation(PMARKERMAP p){
	
	extern PCONTROL g;
	
	static int    last_chr=0;
	static double last_ps =0.0;
	static double last_psf=0.0;
	static double last_psm=0.0;
	register double distance,distance_f=0.0,distance_m=0.0;
	
	char ExtraSpace[]="         ";
	
	if(g->MapDetails && p->ch!=last_chr){
		
		lprintf("\n--------------\n");
		lprintf("Chromosome %2i\n",p->ch);
		lprintf("--------------\n");
		last_chr=p->ch;
		last_ps=last_psf=last_psm=0.0;
		
	}
	
	/*                                                           */
	/* DISPLAY INTER-MARKER DISTANCES AND RECOMB. FRAC. (THETA): */
	/*                                                           */
	if(g->MapDetails && p->ps>0.0){
		
		/* FIRST print inter-marker distances: */
		distance=p->ps-last_ps;
		if(distance){
			lprintf("                            %9.4lf ",distance);
		}else{
			lprintf("                                .     ",distance);
		}
		if(p->psf!=DOUBLEMISSINGVALUE){
			distance_f=p->psf-last_psf;
			if(distance_f){
				lprintf("         %9.4lf ",distance_f);
			}else{
				lprintf("             .     ");
			}
		}
		if(p->psm!=DOUBLEMISSINGVALUE){
			distance_m=p->psm-last_psm;
			if(distance_m){
				lprintf("         %9.4lf ",distance_m);
			}else{
				lprintf("             .     ");
			}
		}
		lprintf("\n");
		
		/* THEN print recombination fractions: */
		if(distance){
			lprintf("                              (%6.4lf)",Kosambi_cM_To_Theta(distance));
		}else{
			lprintf("                                .     ",distance);
		}
		if(p->psf!=DOUBLEMISSINGVALUE){
			if(distance_f){
				lprintf("           (%6.4lf)",Kosambi_cM_To_Theta(distance_f));
			}else{
				lprintf("             .     ");
			}
		}
		if(p->psm!=DOUBLEMISSINGVALUE){
			if(distance_m){
				lprintf("           (%6.4lf)",Kosambi_cM_To_Theta(distance));
			}else{
				lprintf("             .     ");
			}
		}
		lprintf("\n");
		
	}
	
	/*                        */
	/* DISPLAY MAP POSITIONS: */
	/*                        */
	lprintf("%2i %2i %-11.11s %9.4lf",
		p->ch,
		p->i,
		p->nm,
		p->ps
	);
	/* Save last position for next iteration: */
	last_ps=p->ps;
	
	if(g->MapDetails) lprintf(ExtraSpace);
	
	/* Print female-specific map position: */
	if(p->psf==DOUBLEMISSINGVALUE){
		lprintf("     .    ");
	}else{
		lprintf(" %9.4lf",p->psf);
		last_psf=p->psf;
	}
	
	if(g->MapDetails) lprintf(ExtraSpace);
	
	/* Print male-specific   map position: */
	if(p->psm==DOUBLEMISSINGVALUE){
		lprintf("     .    ");
	}else{
		lprintf(" %9.4lf",p->psm);
		last_psm=p->psm;
	}
	
	lprintf("\n");
	
}

/*                                                            */
/*                                                            */
/* DelimitedOutput(): Generic comma-, space- or tab-delimited */
/* output for Madeline.  The core fields are, as always,      */
/* required, but now you must specifically set the output     */
/* flags to have these fields included or excluded, and       */
/* the order of the output will be entirely based on the      */
/* field order in the database                                */
/*                                                            */
/* -> Updated 981215 by ET                                    */
/*                                                            */
void DelimitedOutput(PCONTROL c,PPEDIGREE pd){

	/* int i; */

	
	/*                                 */
	/* Check all individuals for data: */
	/*                                 */
	TraverseIndividualBSTMarkDataFlag(pd->h,GeneralCaseIndividualHasData);
		
	/*                                                             */
	/* return if sufficient data is not available in the pedigree: */
	/*	                                                            */
	if(!GeneralCasePedigreeHasData(c,pd)) return;

	/*                  */
	/* Output the data: */
	/*                  */
	TraverseIndividualBST4(c,pd,pd->h,OutputDelimitedIndividualRecord);

}


/*                                                                */
/*                                                                */
/* OutputDelimitedIndividualRecord(): Output delimited individual */
/* record with space or commas:                                   */
/*                                                                */
/* -> Updated 981215 by ET                                        */
/*                                                                */
void OutputDelimitedIndividualRecord(PCONTROL c,PPEDIGREE pd,PINDIVIDUAL p){

	int i,ii;
	
	/*                     */
	/* Return if excluded: */
	/*                     */
	if(ExcludedForGeneralCase(p)) return;
	
	/*              */
	/* Core Fields: */
	/*              */
	for(i=0;i<c->db->n.co;i++){
		/*                  */
		/* get field index: */
		/*                  */
		ii=c->db->fi->c[i];
		fprintf(c->streams.fp,"%s%c",OutputGenericField(c,pd,p,ii),c->delimiter);
	}
	
	
	/*                   */
	/* Phenotype Fields: */
	/*                   */
	for(i=0;i<c->db->n.po;i++){
		/*                  */
		/* get field index: */
		/*                  */
		ii=c->db->fi->p[i];
		fprintf(c->streams.fp,"%s%c",OutputGenericField(c,pd,p,ii),c->delimiter);
	}
	
	/*                  */
	/* Genotype fields: */
	/*                  */
	for(i=0;i<c->db->n.go;i++){
		/*                  */
		/* get field index: */
		/*                  */
		ii=c->db->fi->g[i];
		if(i<c->db->n.go-1) fprintf(c->streams.fp,"%s%c",OutputGenericField(c,pd,p,ii),c->delimiter);
		else                fprintf(c->streams.fp,"%s",OutputGenericField(c,pd,p,ii));
	}
	/*                              */
	/* terminating carriage return: */
	/*                              */
	fprintf(c->streams.fp,"\n");	

	/*                            */
	/* Mark individual as output: */
	/*                            */
	p->included++;
	
}


/*                                                            */
/* OutputGenericField(): returns a pointer to formatted field */
/*                       data.                                */
/*                                                            */
/* PCONTROL c:      pointer to control structure              */
/* PPEDIGREE pd:    pointer to individual's pedigree          */
/* PINDIVIDUAL p:   pointer to this individual                */
/* int i:           actual field index                        */
/*                                                            */
char *OutputGenericField(PCONTROL c,PPEDIGREE pd,PINDIVIDUAL p,int i){

	static char rbuf[MINIMUMBUFFERSIZE];
	char *o;		
	char type;
	int width;
	int precision;
	double v;       /* numeric value */
	long d;         /* date          */
	
	/*                                                  */
	/* Obtain field parameters: type, width, precision: */
	/*                                                  */
	type     =c->db->FieldType(c->db,i);
	width    =c->db->FieldLength(c->db,i);
	precision=c->db->FieldPrecision(c->db,i);

	/*                       */
	/* switch based on type: */
	/*	                      */
	switch(type){
		case 'C':
			/*                                                          */
			/* Character field: can be a CORE field or a GENOTYPE field */
			/* or a PHENOTYPE field.  We need to know                   */
			/* if it is a CORE field:                                   */
			/*                                                          */
			if(c->db->ff[i]&KNOWNSET){
				/*                                */
				/* format value based on p or pd: */
				/*                                */
				o=GetCharacterCoreFieldValue(c->db,i,pd,p);
				/* if missing use external CMV indicator: */
				if(!o[0]) o=pCmv->e[0].s;
			}else{
				/*                                                  */
				/* read value from database, or else return missing */
				/* if not in database, or excluded:                 */
				/*                                                  */
				if(p->db && !p->excluded){
					/*                                        */
					/* in database, not excluded: read value: */
					/*                                        */
					o=p->db->ReadString(p->db,NULL,i,p->rci);
					/*                                         */
					/* if missing, use external CMV indicator: */
					/*                                         */
					if(!o[0]) o=pCmv->e[0].s;
				}else{
					/* if missing use external CMV indicator */
					o=pCmv->e[0].s;
				}
			}
			/*               */
			/* print output: */
			/*               */
			switch(c->OutputFormat){
				case CDLMK:
				case TDLMK:
					sprintf(rbuf,"%s",o);								
					break;
				case SDLMK:
				default:
					sprintf(rbuf,"%-*.*s",width,width,o);				
					break;
			}			
			break;
		
		case 'N':
			/*                                                          */
			/* Numeric field: can be a CORE field or a PHENOTYPE field. */
			/* Again, we need to know if it is a CORE field:            */
			/*                                                          */
			if(c->db->ff[i]&KNOWNSET){
				/*                                */
				/* format value based on p or pd: */
				/*	                               */
				v=GetNumericCoreFieldValue(c->db,i,pd,p);
				/* if missing use external NMV indicator: */
				if(v==DOUBLEMISSINGVALUE) v=pNmv->e[0].v;
			}else{
				/*                                                  */
				/* read value from database, or else return missing */
				/* if not in database, or excluded:                 */
				/*                                                  */
				if(p->db && !p->excluded){
					/*                                        */
					/* in database, not excluded: read value: */
					/*		                                      */
					v=p->db->ReadDouble(p->db,i,p->rci);
					/*                                                   */
					/* if missing, use external missing value indicator: */
					/*                                                   */
					if(v==DOUBLEMISSINGVALUE) v=pNmv->e[0].v;
				}else{
					/* if missing use external NMV indicator: */
					v=pNmv->e[0].v;
				}
			}
			/*               */
			/* print output: */
			/*               */
			sprintf(rbuf,"%s",FormatNumber(v,NULL,width,precision));
			break;
		
		case 'D':
			/*                                                                    */
			/* Date format:                                                       */
			/* For dates, we currently don't differentiate the handling of "core" */
			/* dates (like DOB) vs. "non-core" (i.e., phenotype "P") dates        */
			/*                                                  */
			/* read value from database, or else return missing */
			/* if not in database, or excluded:                 */
			/*                                                  */
			if(p->db && !p->excluded){
				/*                                        */
				/* in database, not excluded: read value: */
				/*                                        */
				d=p->db->ReadDate(p->db,i,p->rci);
			}else{
				/* if missing use missing indicator: */
				d=MISSINGDATE;
			}
			/*               */
			/* print output: */
			/*               */
			sprintf(rbuf,"%s",DelimitedGenericDateString(d,'-'));
			break;
	}
	return (char *) rbuf;
}	



/*                                                            */
/* GetNumericCoreFieldValue(): for numeric core "cio" fields: */
/*                                                            */
double GetNumericCoreFieldValue(PDBF db,int i,PPEDIGREE pd,PINDIVIDUAL p){

	char *fnm;
	
	/*                                */
	/* get field name for comparison: */
	/*	                               */
	fnm=db->FieldName(db,i);

	/*                                                     */
	/* for sex, return the values defined in Nsv (or Nmv): */
	/*		                                                   */
	if(!strcmp(fnm,gfnSex.e.s)){
		if(p->female==MISSING) return DOUBLEMISSINGVALUE;
		if(p->female)          return pNsv->e[FEMALE].v;
		else                   return pNsv->e[  MALE].v;
	}

	/*                  */
	/* Affection status */
	/*                  */
	if(!strcmp(fnm,gfnAffected.e.s)){
		if(p->affected==MISSING) return DOUBLEMISSINGVALUE;
		return (double) p->affected;
	}
	
	/*          */
	/* Proband: */
	/*          */
	if(!strcmp(fnm,gfnProband.e.s)){
		if(p->proband==MISSING) return DOUBLEMISSINGVALUE;
		return (double) p->proband;
	}
	
	/*       */
	/* Dead: */
	/*		     */
	if(!strcmp(fnm,gfnDead.e.s)){
		if(p->dead==MISSING) return DOUBLEMISSINGVALUE;
		return (double) p->dead;
	}

	/*        */
	/* Class: */
	/*        */
	if(!strcmp(fnm,gfnClass.e.s)){
		if(p->clas==MISSING) return DOUBLEMISSINGVALUE;
		return (double) p->clas;
	}

	/*            */
	/* Otherwise: */
	/*            */
	return DOUBLEMISSINGVALUE;
}


/*                                                           */
/* GetCharacterCoreFieldValue(): for character "cio" fields: */
/*                                                           */
char * GetCharacterCoreFieldValue(PDBF db,int i,PPEDIGREE pd,PINDIVIDUAL p){

	static char buf[MAXIDWIDTH]="\0";
	static char yes[]="Y";
	static char no[]= "N";
	
	char *fnm;
	

	/*                                */
	/* get field name for comparison: */
	/*	                               */
	fnm=db->FieldName(db,i);

	/*                                  */
	/* run through core fields in turn: */
	/*	                                 */
	
	/*           */
	/* Study ID: */
	/*           */
	if(!strcmp(fnm,gfnID.e.s))
		return p->id;
	
	/*         */
	/* Father: */
	/*         */
	if(!strcmp(fnm,gfnFather.e.s))
		return p->fid;

	/*         */
	/* Mother: */
	/*         */
	if(!strcmp(fnm,gfnMother.e.s))
		return p->mid;

	/*                                                     */
	/* for sex, return the values defined in Csv (or Cmv): */
	/*		                                                   */
	if(!strcmp(fnm,gfnSex.e.s)){
		if(p->female==MISSING) return CHARMISSINGSTRING;
		if(p->female)          return pCsv->e[FEMALE].s;
		else                   return pCsv->e[  MALE].s;
	}

	/*                                                     */
	/* for MZTwin, just copy value to a string and return: */
	/*	                                                    */
	if(!strcmp(fnm,gfnMZTwin.e.s)){
		if(isupper(p->twin)){
			buf[0]=p->twin;
			buf[1]='\0';
			return buf;
		}else 
			return CHARMISSINGSTRING;
	}

	/*                                                     */
	/* for DZTwin, just copy value to a string and return: */
	/*	                                                    */
	if(!strcmp(fnm,gfnDZTwin.e.s)){
		if(islower(p->twin)){
			buf[0]=toupper(p->twin);
			buf[1]='\0';
			return buf;
		}else
			return CHARMISSINGSTRING;
	}
	
	/*            */
	/* Family ID: */
	/*            */
	if(!strcmp(fnm,gfnFamID.e.s))
		return pd->fid;
	
	/*                  */
	/* affection field: */
	/*	                 */
	if(!strcmp(fnm,gfnAffected.e.s)){
		if(p->affected==MISSING) return CHARMISSINGSTRING;
		return p->affected?yes:no;
	}

	/*                */
	/* proband field: */
	/*                */
	if(!strcmp(fnm,gfnProband.e.s)){
		if(p->proband==MISSING) return CHARMISSINGSTRING;
		return p->proband?yes:no;
	}
	
	/*                     */
	/* death status field: */
	/*		                   */
	if(!strcmp(fnm,gfnDead.e.s)){
		if(p->dead==MISSING) return CHARMISSINGSTRING;
		return p->dead?yes:no;
	}
	return CHARMISSINGSTRING;
}


/*                                               */
/* Exclude(): exclude individuals and pedigrees: */
/*                                               */
void Exclude(void){

	RESULT result;
	int recurse=0;
	
	
	extern PCONTROL g;

	/*                                      */
	/* Check for presence of open database: */
	/*                                      */
	if(!g->db)
		MadelineErrorReport("Exclude(): no pedigree table is currently open");

	/*                                                                       */
	/* Get next token:                                                       */
	/* if the token is "family", then exclude descendants and spouse as well */
	/*                                                                       */
	GetToken();
	if(pT->i==FAMK){
		recurse=1;	
		/*                    */
		/* get "for" keyword: */
		/*                    */
		GetToken();
	}else if(pT->i==PEDIK){
		recurse=2;	/* flag for excluding entire pedigrees */
		/*                                                */
		/* get "for" keyword:                             */
		/*                                                */
		GetToken();
	}
	/*               */
	/* syntax check: */
	/*               */
	if(pT->i!=FORK)
		MadelineErrorReport("Exclude(): FOR keyword expected: EXCLUDE FOR <expr>");

	/*                                    */
	/* get here if there is a FOR clause: */
	/*                                    */

	/*                                      */
	/* Check for presence of open database: */
	/*                                      */
	if(!g->db)
		MadelineErrorReport("Exclude(): no pedigree table is currently open");

	
	/*                                                          */
	/* set globals to avoid error when dereferencing internals: */
	/*                                                          */
	g->cpd=g->pd;
	g->cp =g->pd->h;
	
		
	/*                                        */
	/* Go to the first record of the database */
	/* -- this record is as good as any other */
	/*    for the first evaluation of the     */
	/*    expression:                         */
	/*                                        */
	GoTo(g->db,0);


	/*                                               */
	/* EvaluateExpression() starts with a GetToken() */
	/* after clearing the token stack:               */
	/*                                               */

	/*                                                    */
	/* The first time of evaluating the expression        */
	/* is done to make sure the expression is error-free: */
	/*                                                    */
	EvaluateExpression(&result);
	/*                                                  */
	/* Set the expression repeat flag so that           */
	/* GetToken() will simply pull tokens off the token */
	/* stack instead of reparsing the expression:       */
	/*                                                  */
	RepeatInProgress=1;
	/*                        */
	/* reset tallies to zero: */
	/*                        */
	ResetTallyCounters(g);
	
	/*                                            */
	/* Traverse pedigrees with supplied function: */
	/*                                            */
	if(recurse==1){
		TraverseBinaryPedigreeTree2(g,g->pd,SetExcludeFlagsRecursively);
	/*else if(recurse==2)  */
		/* Exclude pedigrees: */
	}else{
		TraverseBinaryPedigreeTree2(g,g->pd,SetExcludeFlags);
	}
	/*                              */
	/* Done: reset RepeatInProgress */
	/*                              */
	RepeatInProgress=0;
	
	/*                                  */
	/* Recalculate pedigree complexity: */
	/*                                  */
	CalculateComplexityOfPedigrees(g);
	
	/*             */
	/* Show tally: */
	/*             */
	ShowTally(g,"marked for exclusion");
}

/*                                                         */
/* SetExcludeFlags(): Set the exclude flags of individuals */
/* in a pedigree                                           */
/*                                                         */
void SetExcludeFlags(PCONTROL c,PPEDIGREE pd){
	
	TraverseIndividualBST4(c,pd,pd->h,SetIndividualExcludeFlag);
}


/*                                                                    */
/* SetExcludeFlagsRecursively(): Set the exclude flags of individuals */
/* in a pedigree, and then recurse their descendants:                 */
/*                                                                    */
void SetExcludeFlagsRecursively(PCONTROL c,PPEDIGREE pd){
	
	TraverseIndividualBST4(c,pd,pd->h,SetIndividualExcludeFlagRecursively);
}


/*                                                     */
/* SetIndividualExcludeFlag(): set the exclude flag of */
/* one individual                                      */
/*                                                     */
void SetIndividualExcludeFlag(PCONTROL c,PPEDIGREE pd,PINDIVIDUAL p){

	RESULT result;

	/*                                                                 */
	/* always set c->cpd and c->cp so that internals can be evaluated: */
	/*                                                                 */
	c->cpd=pd;
	c->cp=p;
	
	/*                                */
	/* Handle dummied in individuals: */
	/*                                */
	if(!p->db){
		/*                      */
		/* no record available: */
		/*                      */
		c->db->cr=NO_RECORD_EXISTS;
	}else{
		/*                                 */
		/* Go to this individual's record: */
		/*                                 */
		GoTo(p->db,p->rci);
	}
	/*                                                    */
	/* evaluate the expression already on                 */
	/* the token stack (assuming that RepeatInProgress is */
	/* set to TRUE, which it should be):                  */
	/*                                                    */
	EvaluateExpression(&result);
	/*                                      */
	/* set the individual's exclusion flag: */
	/*                                      */
	if(result.v && result.v!=DOUBLEMISSINGVALUE){
		ExcludeIndividual(c,pd,p);
	}
}


/*                                                                */
/* SetIndividualExcludeFlagRecursively(): set the exclude flag of */
/* one individual                                                 */
/*                                                                */
void SetIndividualExcludeFlagRecursively(PCONTROL c,PPEDIGREE pd,PINDIVIDUAL p){

	RESULT result;
	int i;

	/*                                                                 */
	/* always set c->cpd and c->cp so that internals can be evaluated: */
	/*                                                                 */
	c->cpd=pd;
	c->cp=p;
	
	/*                                */
	/* Handle dummied in individuals: */
	/*                                */
	if(!p->db){
		/*                      */
		/* no record available: */
		/*                      */
		c->db->cr=NO_RECORD_EXISTS;
	}else{
		/*                                 */
		/* Go to this individual's record: */
		/*                                 */
		GoTo(p->db,p->rci);
	}

	/*                                                    */
	/* evaluate the expression already on                 */
	/* the token stack (assuming that RepeatInProgress is */
	/* set to TRUE, which it should be):                  */
	/*                                                    */
	EvaluateExpression(&result);
	/*                                      */
	/* set the individual's exclusion flag: */
	/*                                      */
	if(result.v && result.v!=DOUBLEMISSINGVALUE){
		/*                     */
		/* Exclude individual: */
		/*                     */
		ExcludeIndividual(c,pd,p);
		/*                               */
		/* Exclude spouse(s), if present */
		/*                               */
		if(!p->na) return;
		/*                                                    */
		/* If gender missing, then should not have spouses    */
		/* anyway, and shouldn't have kids either, so return: */
		/*                                                    */
		if(p->female==MISSING) return;

		/*                  */
		/* Exclude spouses: */
		/*                  */
		for(i=0;i<p->na;i++){
			ExcludeIndividual(c,pd,p->a[i]);
		}
		/*                    */
		/* recurse on female: */
		/*                    */
		if(p->female){
			TraverseOffspring3(c,pd,p,ExcludeIndividual);
		}else{
			/*                                          */
			/* For a male, recurse on his female mates: */
			/*                                          */
			for(i=0;i<p->na;i++){
				TraverseOffspring3(c,pd,p->a[i],ExcludeIndividual);
			}
		}
	}
}


/*                    */
/* ExcludeIndividual: */
/*                    */
void ExcludeIndividual(PCONTROL c,PPEDIGREE pd,PINDIVIDUAL p){
	/*                                    */
	/* if not yet excluded, then exclude: */
	/*                                    */
	if(!p->excluded){		
		p->excluded=1;
		pd->ex++;
		dprintf("Individual %s in pedigree %s has been marked for exclusion\n",p->id,p->pd->fid);

		/*                                               */
		/* increment counters: vir,flt,csp, and din      */
		/* categories are used: dno and dex categories   */
		/* don't apply here.  Note that we are           */
		/* making a tally of how many individuals and    */
		/* pedigrees are "included" in the exclusions -- */
		/* thus were going to always use "in" instead of */
		/* "ex" for the tally counts:                    */
		/*                                               */
		if     (p->csp        ) c->tally.csp.in++;
		else if(p->unconnected) c->tally.flt.in++;
		else if(!p->db        ) c->tally.vir.in++;
		else                    c->tally.din.in++;
		
		/*                     */
		/* has lastpd changed? */
		/*                     */
		if(c->lastpd!=pd){
			c->lastpd=pd;
			c->tally.pedg.in++;
		}

	} else {
		/*                   */
		/* already excluded: */
		/*                   */
		dprintf("Individual %s in pedigree %s has already been marked for exclusion\n",p->id,p->pd->fid);
	}
}


/*                                              */
/* Unexclude(): unexclude specific individuals: */
/*                                              */
void Unexclude(void){

	RESULT result;
	
	int recurse=0;
	
	extern PCONTROL g;


	/*                                      */
	/* Check for presence of open database: */
	/*                                      */
	if(!g->db)
		MadelineErrorReport("Unexclude(): no pedigree table is currently open");

	/*                                                                       */
	/* Get next token:                                                       */
	/* if the token is "family", then exclude descendants and spouse as well */
	/*                                                                       */
	GetToken();
	if(pT->i==FAMK){
		recurse=1;	
		/*                    */
		/* get "for" keyword: */
		/*                    */
		GetToken();
	}
	/*               */
	/* syntax check: */
	/*               */
	if(pT->i!=FORK)
		MadelineErrorReport("Unexclude(): FOR keyword expected: UNEXCLUDE FOR <expr>");

	/*                                    */
	/* get here if there is a FOR clause: */
	/*                                    */

	/*                                      */
	/* Check for presence of open database: */
	/*                                      */
	if(!g->db)
		MadelineErrorReport("Unexclude(): no pedigree table is currently open");
	
	/*                             */
	/* set globals to avoid error: */
	/*                             */
	g->cpd=g->pd;
	g->cp =g->pd->h;
	
		
	/*                                        */
	/* Go to the first record of the database */
	/* -- this record is as good as any other */
	/*    for the first evaluation of the     */
	/*    expression:                         */
	/*                                        */
	GoTo(g->db,0);

	/*                                               */
	/* EvaluateExpression() starts with a GetToken() */
	/* after clearing the token stack:               */
	/*                                               */

	/*                                                    */
	/* The first time of evaluating the expression        */
	/* is done to make sure the expression is error-free: */
	/*                                                    */
	EvaluateExpression(&result);
	/*                                                  */
	/* Set the expression repeat flag so that           */
	/* GetToken() will simply pull tokens off the token */
	/* stack instead of reparsing the expression:       */
	/*                                                  */
	RepeatInProgress=1;
	/*                        */
	/* reset tallies to zero: */
	/*                        */
	ResetTallyCounters(g);

	/*                                            */
	/* Traverse pedigrees with supplied function: */
	/*                                            */
	if(recurse)
		TraverseBinaryPedigreeTree2(g,g->pd,UnsetExcludeFlagsRecursively);
	else
		TraverseBinaryPedigreeTree2(g,g->pd,UnsetExcludeFlags);
	/*                              */
	/* Done: reset RepeatInProgress */
	/*                              */
	RepeatInProgress=0;
	
	/*                                  */
	/* Recalculate pedigree complexity: */
	/*                                  */
	CalculateComplexityOfPedigrees(g);
	
	/*             */
	/* Show tally: */
	/*             */
	ShowTally(g,"recalled from exclusion");
}


/*                                                           */
/* UnsetExcludeFlags(): Set the exclude flags of individuals */
/* in a pedigree back to zero if they are set to one:        */
/*                                                           */
void UnsetExcludeFlags(PCONTROL c,PPEDIGREE pd){

	TraverseIndividualBST4(c,pd,pd->h,UnsetIndividualExcludeFlag);
}

/*                                                                        */
/* UnsetExcludeFlagsRecursively(): Unset the exclude flags of individuals */
/* in a pedigree, and then recurse their descendants:                     */
/*                                                                        */
void UnsetExcludeFlagsRecursively(PCONTROL c,PPEDIGREE pd){
	
	TraverseIndividualBST4(c,pd,pd->h,UnsetIndividualExcludeFlagRecursively);
}

/*                                                            */
/* void UnsetIndividualExcludeFlag(): set the exclude flag of */
/* one individual back to zero                                */
/*                                                            */
void UnsetIndividualExcludeFlag(PCONTROL c,PPEDIGREE pd,PINDIVIDUAL p){

	RESULT result;


	/*                                                                 */
	/* always set c->cpd and c->cp so that internals can be evaluated: */
	/*                                                                 */
	c->cpd=pd;
	c->cp=p;
	
	/*                                */
	/* Handle dummied in individuals: */
	/*                                */
	if(!p->db){
		/*                      */
		/* no record available: */
		/*                      */
		c->db->cr=NO_RECORD_EXISTS;
	}else{
		/*                                 */
		/* Go to this individual's record: */
		/*                                 */
		GoTo(p->db,p->rci);
	}
	
	/*                                                    */
	/* evaluate the expression already on                 */
	/* the token stack (assuming that RepeatInProgress is */
	/* set to TRUE, which it should be):                  */
	/*                                                    */
	EvaluateExpression(&result);
	/*                                                              */
	/* Unset the (presumably excluded) individual's exclusion flag: */
	/*                                                              */
	if(result.v && result.v!=DOUBLEMISSINGVALUE)
		UnexcludeIndividual(c,pd,p);
}


/*                                                                    */
/* UnsetIndividualExcludeFlagRecursively(): Unset the exclude flag of */
/* one individual, and then recurse their descendants                 */
/*                                                                    */
void UnsetIndividualExcludeFlagRecursively(PCONTROL c,PPEDIGREE pd,PINDIVIDUAL p){

	RESULT result;
	int i;

	/*                                                                 */
	/* always set c->cpd and c->cp so that internals can be evaluated: */
	/*                                                                 */
	c->cpd=pd;
	c->cp=p;
	
	/*                                */
	/* Handle dummied in individuals: */
	/*                                */
	if(!p->db){
		/*                      */
		/* no record available: */
		/*                      */
		c->db->cr=NO_RECORD_EXISTS;
	}else{
		/*                                 */
		/* Go to this individual's record: */
		/*                                 */
		GoTo(p->db,p->rci);
	}
	/*                                                    */
	/* evaluate the expression already on                 */
	/* the token stack (assuming that RepeatInProgress is */
	/* set to TRUE, which it should be):                  */
	/*                                                    */
	EvaluateExpression(&result);
	/*                                      */
	/* set the individual's exclusion flag: */
	/*                                      */
	if(result.v){
		/*                       */
		/* Unexclude individual: */
		/*                       */
		UnexcludeIndividual(c,pd,p);
		/*                                 */
		/* Unexclude spouse(s), if present */
		/*                                 */
		if(!p->na) return;
		for(i=0;i<p->na;i++)
			UnexcludeIndividual(c,pd,p->a[i]);
		/*                    */
		/* recurse on female: */
		/*                    */
		if(p->female){
			TraverseOffspring3(c,pd,p,UnexcludeIndividual);
		}else{
			for(i=0;i<p->na;i++)
				TraverseOffspring3(c,pd,p->a[i],UnexcludeIndividual);
		}
	}
}



/*                                                                        */
/* void UnexcludeIndividual(): unexclude a previously excluded individual */
/*                                                                        */
/*                                                                        */
void UnexcludeIndividual(PCONTROL c,PPEDIGREE pd,PINDIVIDUAL p){
	if(p->excluded){
		p->excluded=0;
		pd->ex--;
		dprintf("Individual %s in pedigree %s is no longer excluded\n",p->id,p->pd->fid);

		/*                                               */
		/* increment counters: vir,flt,csp, and din      */
		/* categories are used: dno and dex categories   */
		/* don't apply here.  Note that we are           */
		/* making a tally of how many individuals and    */
		/* pedigrees are "included" in the exclusions -- */
		/* thus were going to always use "in" instead of */
		/* "ex" for the tally counts:                    */
		/*                                               */
		if     (p->csp        ) c->tally.csp.in++;
		else if(p->unconnected) c->tally.flt.in++;
		else if(!p->db        ) c->tally.vir.in++;
		else                    c->tally.din.in++;
		/*                     */
		/* has lastpd changed? */
		/*                     */
		if(c->lastpd!=pd){
			c->lastpd=pd;
			c->tally.pedg.in++;			
		}

	}else{
		dprintf("Individual %s in pedigree %s is already included\n",p->id,p->pd->fid);
	}
}


/*                                                               */
/* Convert(): Convert a comma-, tab- or other-delimited file     */
/*            to an ASCII rectangular array format that Madeline */
/*            can recognize directly                             */
/*                                                               */
void Convert(void){

	RESULT result;
	RESULT result2;
	char dlm;      /* delimiter */
	int n;
	
	char *OutputFileName; /* output file name */

	/* extern PCONTROL g; */

	/*                                                                                        */
	/* First keyword after CONVERT can be COMMA or TAB or other delimiter enclosed in quotes: */
	/*                                                                                        */
	GetToken();

	if     (pT->i==CMMAK){
		dlm=',';
	}else if(pT->i==TABAK){
		dlm='\t';
	}else if(pT->i==SPCAK){
		dlm=' ';
	}else if(pT->i==MAPK || pT->i==CRIMK || pT->i==MARSH || pT->i==SIMWK || pT->i==WEBRK){
		/* Call function to handle map and other file conversions: */
		ConvertMapAndOtherFileTypes(pT->i);
		return;
	}else{
		if(pT->t!=STRINGVALUE)
			MadelineErrorReport("Convert(): COMMA, SPACE, TAB or a string denoting the delimiter expected.");
		dlm=pT->e[0];
	}
	
	
	/*                                   */
	/* Get the words DELIMITED and FILE: */
	/*                                   */
	GetToken();
	if(pT->i!=DLMAK) MadelineErrorReport("Convert(): DELIMITED keyword expected");	
	GetToken();
	if(pT->i!=FILEK) MadelineErrorReport("Convert(): FILE keyword expected");	
	/*                                       */
	/* Get the name of the file for reading: */
	/*                                       */
	EvaluateExpression(&result);
	if(result.t!=STRINGVALUE) MadelineErrorReport("Convert(): Name of file to read expected");
		
	/* No need to get another token after EvaluateExpression(): see if */
	/* keyword "TO" is also present:                                   */
	/*                                                                 */
	if(pT->i==TOC){
		/*                                                                */
		/* If TO is also present, get the optional file name for writing: */
		/*                                                                */
		EvaluateExpression(&result2);  /* get the filename                */
		if(result2.t!=STRINGVALUE)
			MadelineErrorReport("Convert(): name of file to write expected after TO");
		
		OutputFileName=result2.s;
	}else{
		OutputFileName=ModifyFileName(result.s,".mod");
	}

	/*                               */
	/* Make sure file can be opened: */
	/*                               */
	if(!File(result.s))
		MadelineErrorReport("Convert(): Unable to open \"%s\".: \nPerhaps the name or path is incorrect.",result.s);	
	
	/*                                  */
	/* ConvertFile() is from cnvtdlm.c: */
	/*                                  */
	lprintf("Converting file \"%s\" to \"%s\" ...\n",result.s,OutputFileName);
	/*                                                             */
	/* ConvertFile(): returns number of records written to output: */
	/*                                                             */
	n=ConvertFile(result.s,OutputFileName,dlm);
	if(n) lprintf("Done: %i lines were written to %s\n",n,OutputFileName);
	else  lprintf("No lines were written!\n");
}

/*                                */
/* ConvertMapAndOtherFileTypes(): */
/*                                */
void ConvertMapAndOtherFileTypes(int filetype){
	
	RESULT result;
	RESULT result2;
	char *OutputFileName;                  /* map or result file output name   */
	char *MfhOutputFileName;               /* mfh output file name             */
	char *SimwalkMapFileName;              /* simwalk map file name            */
	
	GetToken();
	if(pT->i!=FILEK) MadelineErrorReport("ConvertMapAndOtherFileTypes(): FILE keyword expected: CONVERT MAP|CRIMAP|MARSHFIELD|SIMWALK FILE \"INPUT_FILE_NAME\" TO \"CONVERTED_FILE\"");
	/*                                       */
	/* Get the name of the file for reading: */
	/*                                       */
	EvaluateExpression(&result);
	if(result.t!=STRINGVALUE) MadelineErrorReport("ConvertMapAndOtherFileTypes(): Name of file to read expected");
		
	/* No need to get another token after EvaluateExpression(): see if */
	/* keyword "TO" is also present:                                   */
	/*                                                                 */
	if(pT->i==TOC){
		/*                                                                */
		/* If TO is also present, get the optional file name for writing: */
		/*                                                                */
		EvaluateExpression(&result2);  /* get the filename                */
		if(result2.t!=STRINGVALUE)
			MadelineErrorReport("ConvertMapAndOtherFileTypes(): name of file to write expected after TO: CONVERT MAP|CRIMAP|MARSHFIELD|SIMWALK FILE \"CRIMAP_FILE_NAME\" TO \"MAP_FILE\"");
		
		OutputFileName=CopyString(result2.s);
		if(filetype==SIMWK){
			SimwalkMapFileName=CopyString(ModifyFileName(OutputFileName,MAP_FILE_EXTENSION));
		}
		MfhOutputFileName=CopyString(ModifyFileName(OutputFileName,HEADER_FILE_EXTENSION));
		
	}else{
		
		if(filetype==SIMWK){
			OutputFileName    =CopyString(ModifyFileName(result.s,GRAPH_FILE_EXTENSION));
			SimwalkMapFileName=CopyString(ModifyFileName(OutputFileName,MAP_FILE_EXTENSION));
		}else if(filetype==WEBRK){
			OutputFileName    =CopyString(ModifyFileName(result.s,DATA_FILE_EXTENSION));
		}else{
			OutputFileName=CopyString(ModifyFileName(result.s,MAP_FILE_EXTENSION));
		}
		MfhOutputFileName=CopyString(ModifyFileName(OutputFileName,HEADER_FILE_EXTENSION));
	}
	
	lprintf("Converting input file \"%s\"\nto Madeline-formatted output files,\n\"%s\" and \"%s\" ...\n",
		result.s,
		OutputFileName,
		MfhOutputFileName
	);
	
	lprintf("================\n");
	lprintf("Converting file\n");
	lprintf("================\n");
	
	if(filetype==MAPK || filetype==CRIMK){
		
		/* RecognizeCrimapFile() is in recognize.c: */
		RecognizeCrimapFile(result.s,OutputFileName);
		
	}else if(filetype==WEBRK){
		
		/*                                                                                 */
		/* The last parameter is a flag indicating whether we wish to decompose the table: */
		/*                                                                                 */
		RecognizeWeberLabFile(result.s,OutputFileName,0);
		
		
	}else if(filetype==MARSH){
		
		/* RecognizeMarshfieldFile() is in recognize.c:                    */
		/* This function is very similar to RecognizeCrimapFile(), but the */
		/* files need to be parsed differently.                            */
		RecognizeMarshfieldFile(result.s,OutputFileName);
		
	}else if(filetype==SIMWK){
		/* There are TWO files to recognize here: both the graph DATA and the graph MAP file: */
		RecognizeSimwalkResultFile(result.s,OutputFileName);
		
	}else{
		MadelineErrorReport("Unsupported file type!");
	}
	/*                         */
	/* Create the .mfh header: */
	/*                         */
	lprintf("================%c\n",filetype==SIMWK?'=':' ');
	lprintf("Recognizing file%c\n",filetype==SIMWK?'s':' ');
	lprintf("================%c\n",filetype==SIMWK?'=':' ');
	
	RecognizeFile(OutputFileName,g->lang,g->ivar,0);
	if(filetype==SIMWK){
		lprintf("--------------------\n");
		lprintf("Associated map file:\n");
		lprintf("--------------------\n");
		RecognizeFile(SimwalkMapFileName,g->lang,g->ivar,0);
	}
	
	free(OutputFileName);
	free(MfhOutputFileName);
	
}

/*                                                                             */
/* Rectify(): Rectify a rectangular ASCII data file containing embedded tabs   */
/*            or uneven line lengths so that it is in the format that Madeline */
/*            can recognize directly.                                          */
/*                                                                             */
void Rectify(void){

	RESULT result;
	RESULT result2;
	int n;

	char *OutputFileName;
	
	/* extern PCONTROL g; */

	/*                                       */
	/* Get the name of the file for reading: */
	/*                                       */
	EvaluateExpression(&result);
	if(result.t!=STRINGVALUE) MadelineErrorReport("Rectify(): Name of file to read expected");
		
	/* No need to get another token after EvaluateExpression(): see if */
	/* keyword "TO" is also present:                                   */
	/*                                                                 */
	if(pT->i==TOC){
		/*                                                                */
		/* If TO is also present, get the optional file name for writing: */
		/*                                                                */
		EvaluateExpression(&result2);  /* get the filename                */
		if(result2.t!=STRINGVALUE)
			MadelineErrorReport("Rectify(): name of file to write expected after TO");
		OutputFileName=result2.s;
	}else{
		OutputFileName=ModifyFileName(result.s,".mod");
	}

	/*                               */
	/* Make sure file can be opened: */
	/*                               */
	if(!File(result.s))
		MadelineErrorReport("Rectify(): Unable to open \"%s\".:\nPerhaps the name or path is incorrect.",result.s);
	
	/*                                   */
	/* RectifyFile() is from recogniz.c: */
	/*                                   */
	lprintf("Rectifying file \"%s\" to \"%s\" ...\n",result.s,OutputFileName);
	/*                                                             */
	/* Note bene: because RectifyFile() now trims out empty lines, */
	/* the return value no longer represents the actual number of  */
	/* "lines" written to the output file:                         */
	/*                                                             */
	n=RectifyFile(result.s,OutputFileName);
	/*                        */
	/* Inform user of result: */
	/*                        */
	if(n) lprintf("Rectified file \"%s\" has been written.\n",OutputFileName);
	else  lprintf("File %s does not require rectification.\n",result.s);
}


/*                                                                     */
/* Recognize():  Recognize an ASCII space-delimited, column-aligned    */
/* rectangular array of data as a database via a Madeline MBASE header */
/* file which recognize will create:                                   */
/*                                                                     */
void Recognize(void){

	RESULT result;
	RESULT result2;
	int n;

	char *OutputFileName;
	
	extern PCONTROL g;

	/*                                       */
	/* Get the name of the file for reading: */
	/*                                       */
	EvaluateExpression(&result);
	if(result.t!=STRINGVALUE) MadelineErrorReport("Recognize(): Name of file to read expected");
	/*                                                                 */
	/* No need to get another token after EvaluateExpression(): see if */
	/* keyword "TO" is also present:                                   */
	/*                                                                 */
	if(pT->i==TOC){
		/*                                                               */
		/* If TO is also present, get the optional file name for writing */
		/* the .mfh header:                                              */
		/*                                                               */
		EvaluateExpression(&result2);  /* get the filename               */
		if(result2.t!=STRINGVALUE)
			MadelineErrorReport("Recognize(): name of file to write expected after TO (should end in \"%s\")",HEADER_FILE_EXTENSION);
		
		OutputFileName=result2.s;
	}else{
		OutputFileName=ModifyFileName(result.s,HEADER_FILE_EXTENSION);
	}

	/*                               */
	/* Make sure file can be opened: */
	/*                               */
	if(!File(result.s))
		MadelineErrorReport("Recognize(): Unable to open \"%s\". \nPerhaps the name or path is incorrect.",result.s);	
	
	/*                                     */
	/* RecognizeFile() is from recogniz.c: */
	/*                                     */
	lprintf("Starting to recognize file \"%s\" to \"%s\" ...\n",result.s,OutputFileName);

	/*                                                                           */
	/* RecognizeFile() now makes complete use of the field names in c->ivar:     */
	/*                                                                           */
	/* NOTE: The last parameter is a 0 or a 1 indicating whether to also create  */
	/* the complementary "run" file                                              */
	n=RecognizeFile(result.s,g->lang,g->ivar,0);

	if(n){
		/*                                                                                                               */
		/* file was recognized, so report creation of .mfh and .run files:                                               */
		/* --> Note: messages now printed directly from RecognizeFile() function call                                    */
		/*lprintf("Recognition header file \"%s\" has been created.\nEdit .run command file to open.\n",OutputFileName); */
		/*                                                                                                               */
	}else{
		/*                          */
		/* file was not recognized: */
		/*                          */
		lprintf("No \"%s\" or \"%s\" files were created\n",HEADER_FILE_EXTENSION,RUN_FILE_EXTENSION);
	}
}


/*                                 */
/*                                 */
/* UserFQTLLocusHeaderProcessor(): */
/*                                 */
/*                                 */
void UserFQTLLocusHeaderProcessor(PCONTROL c,char *fn){

	/*                                                     */
	/* a map database is required for UserFQTL locus file: */
	/*                                                     */
	if(!c->mp)
		MadelineErrorReport("UserFQTLLocusHeaderProcessor(): a marker map table is required\nfor the Siblink control file");
	/*                                                       */
	/* Call generic locus header processor to open the file: */
	/*                                                       */
	GenericLocusHeaderProcessor(c,fn);
}


/*                                            */
/*                                            */
/* OutputUserFQTLMarker()                     */
/*                                            */
/* -i is the rank index, not the field index: */
/*                                            */
void OutputUserFQTLMarker(PCONTROL c,int i){

	PMARKERMAP p;
	int ii;
	
	/*                  */
	/* Get field index: */
	/*                  */
	ii=c->db->fi->g[i];
	
	/*                               */
	/* Lookup the marker in the map: */
	/*                               */
	p=LookupMarkerMap(c->mp,c->db->FieldName(c->db,ii));
	
	if(!p){
		/*                   */
		/* clean up streams: */
		/*                   */
		GenericDataFooterProcessor(c);
		MadelineErrorReport("OutputUserFQTLMarker(): Can't find marker %s in map",c->db->FieldName(c->db,ii));
	}
	/*                                           */
	/* Print the marker's chromosome and         */
	/* map position in Morgans after the regular */
	/* MENDEL-style header:                      */
	/*                                           */
	fprintf(c->streams.fp,"%-8.8sAUTOSOME%2i 0%4i%8.5f\n",
		c->db->FieldName(c->db,ii),
		c->db->m[ii].n,
		p->ch,
		p->ps/100
		);
	
	TraverseAllele2(c->streams.fp,c->db->m[ii].a,c->db->m[ii].s,OutputUserFQTLMarkerAllele);
}

/*                             */
/*                             */
/* OutputUserFQTLMarkerAllele: */
/*                             */
void OutputUserFQTLMarkerAllele(FILE *fp,PALLELE a,int s){

	fprintf(fp,"%3i     %6.4f\n",
		a->v,
		(double)a->n/(double)s
	);
}


/*                                */
/* UserFQTLDataHeaderProcessor(): */
/*                                */
/*                                */
void UserFQTLDataHeaderProcessor(PCONTROL c,char *fn){

	char hold[MINIMUMBUFFERSIZE];
	char *h,*s;
	int i,ii;
	/* int j; */
	int w; /* output field width: adjusted, if necessary */
	int PedigreePositionIndex=1;
	int TraitIndex=2;
	int AgeIndex=3;
	int BmiIndex=4;

	/*                         */
	/* Store output file name: */
	/*                         */
	StoreString(&gOutputFile,fn);
	/*                                       */
	/* we also write to a .ctl control file: */
	/*                                       */
	StoreString(&gParameterOutputFile,fn);

	/*                                                         */
	/* change ending to .ctl by overwriting last 4 characters: */
	/*                                                         */
	strncpy(gParameterOutputFile.e.s+strlen(gParameterOutputFile.e.s)-4,
	        ".ctl",4);

	/*                                                                    */
	/* UserFQTL, in its current incarnation,                              */
	/* can only have one trait variable and zero to two covariates:       */
	/* if covariates, the assumption is that the first is age, the second */
	/* is bmi:                                                            */
	/*                                                                    */
	if(c->db->n.po-c->db->n.vo!=1 || c->db->n.vo>2)
		MadelineErrorReport("UserFQTLDataHeaderProcessor():\n\tUserFQTL currently only models a single trait variable\n\tand up to two covariates");
		
	/*                                         */
	/* Then open up the .ctl file for writing: */
	/*                                         */
	c->streams.fpar=fopen(gParameterOutputFile.e.s,"wt");
	if(!c->streams.fpar)
		MadelineErrorReport("UserFQTLDataHeaderProcessor(): unable to open \"%s\" for output",gParameterOutputFile.e.s);

	lprintf("Creating associated UserFQTL control file called \"%s\"\n",gParameterOutputFile.e.s);

	/*                                         */
	/* get the core file name minus extension: */
	/*                                         */
	for(h=hold,s=gOutputFile.e.s;*s && *s!='.';*h++=*s++);
	*h++='.';
	*h='\0';

	/*                                                                                     */
	/* Write defaults                                                                      */
	/* for the parameter file:                                                             */
	/*                                                                                     */
	fprintf(c->streams.fpar,"%sloc\n",hold);  /* 1. locus file                             */
	fprintf(c->streams.fpar,"%sped\n",hold);  /* 2. pedigree file                          */
	fprintf(c->streams.fpar,"%sopd\n",hold);  /* 3. output pedigree file (for simulations) */
	fprintf(c->streams.fpar,"testrnd.lst\n"); /* 4. random number list                     */

	fprintf(c->streams.fpar,"UserFQTL %s %s\n",
		c->OutputFormat==UFNAK?"ALL":
		c->OutputFormat==UFNOK?"OFFSP":
		"FNDRS",
		DelimitedGenericDateString(JulianToday(),'.')
	);                  /* 5. title of problem */

	fprintf(c->streams.fpar,"     0     1\n");	/* 6. no. of simulations, autosomal vs. X-linked	 */
	fprintf(c->streams.fpar,"%sout\n",hold);    /* 7. general output file name                   */
	fprintf(c->streams.fpar,"%slnk\n",hold);    /* 8. linkage output file name                   */
	fprintf(c->streams.fpar,"%splt\n",hold);    /* 9. plot file output file name                 */

	fprintf(c->streams.fpar,"%4i%4i%4i%4i%4i%4i%4i\n",
		1+c->db->n.po,
		c->db->n.vo==0?2:c->db->n.vo==1?3:8,
		TraitIndex,
		PedigreePositionIndex,
		c->db->n.vo?AgeIndex:0,
		c->db->n.vo==2?BmiIndex:0,
		0
	);	                                        /* 10. NVAR, MODELM,IVTRT,IVPOS,IVAGE,IVBMI,IECHO	 */
	
	fprintf(c->streams.fpar,"FM\n");            /* 11. Female, male symbols */

	fprintf(c->streams.fpar," %7.5f %7.5f\n", /* 12. Flanking distance, test increment */
		0.01*OffEndDistance.e.v,
		0.01*EvaluationInterval.e.v		
	);

	/*                                                                    */
	/* call generic to open output stream, store file name, reset control */
	/* counters, and inform user:                                         */
	/*                                                                    */
	GenericDataHeaderProcessor(c,fn);
		
	/*                                              */
	/* FORTRAN read format for:                     */
	/*                                              */
	/*  -number of probands in Integer format       */
	/*  -number of family members in Integer format */
	/*  -family id in character format              */
	/*                                              */
	fprintf(c->streams.fp,"(I3,1X,I3,1X,A8)\n");
	/*                                                                 */
	/* regular MENDEL, FISHER core fields, plus extra sequence number: */
	/*                                                                 */
	/*  -individual id                                                 */
	/*  -parent 1 id                                                   */
	/*  -parent 2 id                                                   */
	/*  -sex                                                           */
	/*  -twin status                                                   */
	/*  -sequence number                                               */
	/*                                                                 */
	fprintf(c->streams.fp,"(A8,1X,A8,1X,A8,1X,A1,1X,A1,1X,A3,1X");
	/*                                           */
	/* remaining fields are grouped as follows:  */
	/*                                           */
	/*  -1. phenotype variable(s)                */
	/*  -2. covariate(s)                         */
	/*  -3. marker genotypes                     */
	/*                                           */
	
	/*                                  */
	/* Group 1  -  phenotype variables: */
	/*                                  */
	/*                                  */
	for(i=0;i<c->db->n.po;i++){
		/*                  */
		/* get field index: */
		/*                  */
		ii=c->db->fi->p[i];
		/*                                                  */
		/* choose only the "PO" phenotype output variables: */
		/*                                                  */
		if(     c->db->ff[ii]&PHENOTYPESET 
		   && !(c->db->ff[ii]&COVARIATESET)){
		
			/*                                                     */
			/* Fix width if it exceeds MENDEL's 8-character width: */
			/*                                                     */
			w=c->db->FieldLength(c->db,ii);
			if(w>MAXIMUMMENDELFIELDWIDTH)
				w=MAXIMUMMENDELFIELDWIDTH;
			/*                                     */
			/* print the FORTRAN format specifier: */
			/*                                     */
			fprintf(c->streams.fp,",A%i,1X",w);
		}		
	}


	/*                                  */
	/* Group 2  -  covariate variables: */
	/*                                  */
	for(i=0;i<c->db->n.po;i++){
		/*                  */
		/* get field index: */
		/*                  */
		ii=c->db->fi->p[i];
		/*                                                  */
		/* choose only the "VO" covariate output variables: */
		/*                                                  */
		if(     c->db->ff[ii]&COVARIATESET 
		   &&   c->db->ff[ii]&OUTPUTSET
		){
		
			/*                                                     */
			/* Fix width if it exceeds MENDEL's 8-character width: */
			/*                                                     */
			w=c->db->FieldLength(c->db,ii);
			if(w>MAXIMUMMENDELFIELDWIDTH)
				w=MAXIMUMMENDELFIELDWIDTH;
			/*                                     */
			/* print the FORTRAN format specifier: */
			/*                                     */
			fprintf(c->streams.fp,",A%i,1X",w);
		}		
	}


	/*                                                           */
	/* Group 3  -  genotype variables:                           */
	/*                                                           */
	/*	                                                          */
	/*                                                           */
	/* Print a shortened format assuming a fixed width of eight: */
	/*                                                           */
	/*                                                           */
	fprintf(c->streams.fp,",%i(A8,1X)",c->db->n.go);
	
	
	/*                                                         */
	/* terminate the FORTRAN record with a closing parenthesis */
	/* and a carriage return:                                  */
	/*                                                         */
	fprintf(c->streams.fp,")\n");
}


/*                                                                  */
/*                                                                  */
/* OutputUserFQTLIndividualRecord():                                */
/*                                                                  */
/* NOTE: set "parent" to 1 (TRUE) for parental records, so that the */
/*       father and mother id fields will be blanked out.           */
/*                                                                  */
/*                                                                  */
void OutputUserFQTLIndividualRecord(PCONTROL c,PINDIVIDUAL p, int parent){
	
	extern char gMendelOutputMissingValue[];
	
	int i,ii;
	/* int j; */
	
	/*                                                            */
	/* a non-parent is a terminal individual in a nuclear family: */
	/* if s/he hasn't data, don't include:                        */
	/*                                                            */
	if(!(parent || p->hasdata)) return;		

	/*                                                                  */
	/* Write core fixed fields (COREOUTSET), including                  */
	/* the sequential individual ID:                                    */
	/*                                                                  */
	/* NOTE BENE: For a twin individual to be marked as a twin, he or   */
	/*            she must not be a parent in the current nuclear block */
	/*            output.  If a parent, they are marked as if not being */
	/*            a twin.                                               */
	/*                                                                  */
	fprintf(c->streams.fp,"%-8.8s %-8.8s %-8.8s %c %c %2i. ",
		p->id,
		(p->fid[0]==CHARMISSINGVALUE || parent)?gMendelOutputMissingValue:p->fid,
		(p->mid[0]==CHARMISSINGVALUE || parent)?gMendelOutputMissingValue:p->mid,
		(p->female==MISSING?' ':(p->female?'F':'M')),
		(p->twin && isupper(p->twin) && !parent)?p->twin:' ',
		c->isid
	);
	
	/*                                                */
	/* increment the individual sequential id counter */
	/* for the next round:                            */
	/*                                                */
	c->isid++;


	/*                                                               */
	/* output phenotype fields first: "fixedwidth" is treated        */
	/* as FALSE when set to zero in the call to OutputUserFQTLField: */
	/*                                                               */
	for(i=0;i<c->db->n.po;i++){
		/*                  */
		/* get field index: */
		/*                  */
		ii=c->db->fi->p[i];
		/*                                                  */
		/* choose only the "PO" phenotype output variables: */
		/*                                                  */
		if(!(c->db->ff[ii]&COVARIATESET)){

			OutputUserFQTLField(c,p,ii,0);

		}
	}
	/*                          */
	/* Now do covariate fields: */
	/*                          */
	for(i=0;i<c->db->n.po;i++){
		/*                  */
		/* get field index: */
		/*                  */
		ii=c->db->fi->p[i];
		/*                                                  */
		/* choose only the "VO" covariate output variables: */
		/*                                                  */
		if(c->db->ff[ii]&COVARIATESET){
		
			OutputUserFQTLField(c,p,ii,0);

		}
	}
	
	/*                                                       */
	/* now do genotype fields:                               */
	/*                                                       */
	/* NOTE that fixedwidth is now set to 8, so all genotype */
	/* fields will be output with this fixed width:          */
	/*                                                       */
	for(i=0;i<c->db->n.go;i++){
		/*                  */
		/* get field index: */
		/*                  */
		ii=c->db->fi->g[i];
		OutputUserFQTLField(c,p,ii,8);
	}       
	/*                                        */
	/* terminate record with carriage return: */
	/*                                        */
	fprintf(c->streams.fp,"\n");
	/*                            */
	/* Mark individual as output: */
	/*                            */
	p->included++;
}



/*                                                             */
/*                                                             */
/* OutputUserFQTLField():                                      */
/*                                                             */
/*  c : Madeline CONTROL structure for access to streams, etc. */
/*  p : the individual whose data is being output              */
/*  i : the database field index                               */
/*                                                             */
void OutputUserFQTLField(PCONTROL c,PINDIVIDUAL p,int i,int fixedwidth){
	
	extern char gMendelOutputMissingValue[];
	
	char hold[MINIMUMBUFFERSIZE];
	char  *o;
	double v;
	int ty,wide,prec;
	
	if(fixedwidth) wide=fixedwidth;
	else           wide=c->db->FieldLength(c->db,i);
	
	prec=c->db->FieldPrecision(c->db,i);
	ty  =c->db->FieldType(c->db,i);

	/*                                          */
	/* Automatic correction of width, precision */
	/* when they exceed what is acceptable to   */
	/* Mendel:                                  */
	/*                                          */
	if(wide>MAXIMUMMENDELFIELDWIDTH){
			wide=MAXIMUMMENDELFIELDWIDTH;
			if(prec>HALFMAXMENDELFIELDWIDTH)
				prec=HALFMAXMENDELFIELDWIDTH;
	}


	if(p->db){      

		if(ty=='C'){
			/*                             */
			/* read character string data: */
			/*                             */
			o=p->db->ReadString(p->db,hold,i,p->rci);
			/*                                                               */
			/* check for missing values:                                     */
			/*                                                               */
			/* if(IsCMV(o))                                                  */
			/*                                                               */
			/* if(!strcmp(o,CHARMISSINGSTRING)) o=gMendelOutputMissingValue; */
			/*                                                               */
			if(!o[0]) o=gMendelOutputMissingValue;
			
			/*                                               */
			/* fprintf(c->streams.fp,"%-*.*s ",wide,wide,o); */
			fprintf(c->streams.fp,"%*.*s ",wide,wide,o);
						
		}else if(ty=='N'){
			/*                    */
			/* read numeric data: */
			/*                    */
			v=p->db->ReadDouble(p->db,i,p->rci);
			/*                                           */
			/* check for missing values and standardize: */
			/*			                                        */
			/* if(IsNMV(v)){                             */
			/*                                           */
			if(v==DOUBLEMISSINGVALUE){
				/*                                               */
				/* numeric missing value in Mendel is also blank */
				/* because records are all read in character     */
				/* format:                                       */
				o=gMendelOutputMissingValue;
	 			fprintf(c->streams.fp,"%-*.*s ",wide,wide,o);
	 			
			}else{
				/*                                */
				/* print out the formatted value: */
				/*                                */
				fprintf(c->streams.fp,"%s ",FormatNumber(v,NULL,wide,prec));
				
			}
		}

	}else{
		/*                                                                         */
		/* person is dummied-in,                                                   */
		/* not actually in the database: access to the database header is via gdb: */
		/*                                                                         */

		/*                                                            */
		/* output by type: in Mendel, both character and numeric      */
		/* missing values are represented by blanks in the data file: */
		/*                                                            */

		o=gMendelOutputMissingValue;
		fprintf(c->streams.fp,"%-*.*s ",wide,wide,o);

	}
}



/*                             */
/*                             */
/* UserFQTLNuclearBlockOutput: */
/*                             */
/*                             */
void UserFQTLNuclearBlockOutput(PCONTROL c,PPEDIGREE pd){

	int i,j;

	/*                                                            */
	/* Check all individuals for data under the current criteria: */
	/*                                                            */
	TraverseIndividualBSTMarkDataFlag(pd->h,UserFQTLIndividualHasData);

	/*                                                       */
	/* Count any unattached individuals or childless spouses */
	/* before anything else:                                 */
	/*                                                       */
	TraverseIndividualBST2(c,pd->h,CountExcludedUnattachedIndividual);
		
	/*                                                   */
	/* reset pedigree sequential id: this is used to     */
	/* count nuclear blocks rather than actual pedigrees */
	/*                                                   */
	c->psid=1;
	/*                                                                   */
	/* Loop over each founding mom and each spouse of each founding mom: */
	/*                                                                   */
	for(i=0;i<pd->nfm;i++){
		for(j=0;j<pd->fm[i]->na;j++){
			/*                                                                    */
			/* parameters are the CONTROL structure, the founding mom, pd->fm[i], */
			/* and her jth spouse, pd->fm[i]->a[j]:                               */
			/*                                                                    */
			TraverseUserFQTLNuclearBlock(c,pd,pd->fm[i],pd->fm[i]->a[j]);
			
		}
	}
		
	/*                                                          */
	/* if c->psid is now greater than one, that means at        */
	/* least one nuclear family from this pedigree was included */
	/* --otherwise, no nuclear families were included, so we    */
	/*   can say that the whole pedigree was excluded:          */
	/*                                                          */
	if(c->psid>1)
		c->counters.pedg.in++;
	else
		c->counters.pedg.ex++;
		
}

/*                              */
/* UserFQTLIndividualHasData(): */
/*                              */
int UserFQTLIndividualHasData(PINDIVIDUAL p){

	extern PCONTROL g;
	
	/*                                                */
	/* Person "has data" (in a logical sense only) if */
	/* "AutoExclude" is off:                          */
	/*                                                */
	if(!g->AutoExclude) return 1;
	/*                                                             */
	/* Person definitely doesn't have data if not in the database: */
	/*                                                             */
	if(!p->db) return 0;
	
	/*                                                                 */
	/* Person doesn't "have data" if they are supposed to be excluded: */
	/*                                                                 */
	if(p->excluded) return 0;

	/*                                                             */
	/* Person doesn't "have data" if they're a childless spouse or */
	/* otherwise unnattached who has to be excluded anyway:        */
	/*                                                             */
	if(p->csp || p->unconnected) return 0;

	/*                                                            */
	/* No data if there are no output fields above and beyond the */
	/* core output field set:                                     */
	/*                                                            */
	if(p->db->n.o==p->db->n.co) return 0;           

	/*                                              */
	/* Check for phenotype and genotype information */
	/* and mark flags:                              */
	/*                                              */
	
	
	// 2004.07.09.ET CHANGES:
	// p->haspdata=HasPhenotypeData(p);
	p->hasgdata=HasGenotypeData(p);
	
	// 2004.07.09.ET CHANGES:
	//if(p->haspdata || p->hasgdata){
	//	return 1;
	//}else{	
	//	/*                                                              */
	//	/* Inform user when a person in the database lacks information: */
	//	/*                                                              */
	//	dprintf("UserFQTLIndividualHasData(): Individual %s in pedigree %s has no genotype information\n",p->id,p->pd->fid); 
	//	dprintf("                             and has a missing value for one or more phenotype variables\n");
	//	return 0;
	//}
	
	if(p->hasgdata){
		return 1;
	}else{
		dprintf("UserFQTLIndividualHasData(): Individual %s in pedigree %s has no genotype information\n",p->id,p->pd->fid);
		return 0;
		
	}
}


/*                                 */
/* TraverseUserFQTLNuclearBlock(): */
/*                                 */
void TraverseUserFQTLNuclearBlock(PCONTROL c,PPEDIGREE pd,PINDIVIDUAL s1,PINDIVIDUAL s2){

	int i;
	int j;
	int ni;
	PINDIVIDUAL mom,dad;
	char hold[MINIMUMBUFFERSIZE];

	/*                                                      */
	/* return if no children present: not a nuclear family: */
	/*                                                      */
	if(!s1->nk) return;


	/*                                                 */
	/* Get pointer to the female in the parental pair: */
	/*                                                 */
	if(s1->female==1){
		mom=s1;
		dad=s2;
	}else{
		mom=s2;
		dad=s1;
	}
			
	/*                                                              */
	/* Check for data present: ni is number of included individuals */
	/* in the nuclear family:                                       */
	/*                                                              */
	ni=UserFQTLNuclearFamilyHasData(c,pd,mom,dad);
	
	if(ni){
		
		/*                                */
		/* construct a nuclear family id: */
		/*                                */
		hold[0]='\0';
		sprintf(hold,"%s.%i",pd->fid,c->psid);
	
		/*                                                            */
		/* increment sequential nuclear block counter for next round: */
		/*                                                            */
		c->psid++;
		
		/*		                                                                 */
		/* Print number of individuals in I2 format, family ID in A8 format: */
		/*                                                                   */
		fprintf(c->streams.fp,"%3i %3i %-8.8s\n",
			0,
			ni,
			hold
		);
		
		/*                                                      */
		/* reset the individual sequential ID: this is used     */
		/* and incremented by OutputUserFQTLIndividualRecord(): */
		/*                                                      */
		c->isid=1;
	
		/*                                         */
		/* Output parents first:                   */
		/* -- Note that "parent" flag is set to 1: */
		/*	                                        */
		OutputUserFQTLIndividualRecord(c,mom,1);
		OutputUserFQTLIndividualRecord(c,dad,1);
		/*                                               */
		/* alternate parental ordering used for testing: */
		/*                                               */
		/*OutputUserFQTLIndividualRecord(c,dad,1);       */
		/*OutputUserFQTLIndividualRecord(c,mom,1);       */
		
		/*                                          */
		/* then kids: "parent" flag is set to zero: */
		/*                                          */
		/* --> output only kids by this mating:     */
		/*                                          */
		for(i=0;i<mom->nk;i++){
			if(mom->k[i]->f==dad && mom->k[i]->hasdata){
				OutputUserFQTLIndividualRecord(c,mom->k[i],0);
			}
		}
	}
	/*                                       */
	/* Recursive calls for offspring of pair */
	/*                                       */
	/*		                                     */

	for(i=0;i<mom->nk;i++){
		/*                                    */
		/* limit to offspring of this mating: */
		/*                                    */
		if(mom->k[i]->f!=dad) continue;
		/*                                       */
		/* loop over all mates of the offspring: */
		/*                                       */
		for(j=0;j<mom->k[i]->na;j++){
			TraverseUserFQTLNuclearBlock(c,pd,mom->k[i],mom->k[i]->a[j]);
		}
	}	

}





/*                           */
/* IndividualGenderMissing() */
/*                           */
int IndividualGenderMissing(PINDIVIDUAL p){

	if(p->female==MISSING){	
		dprintf("Sex of individual %s in pedigree %s is missing.\n",p->id,p->pd->fid);
		return 1;
	}else
		return 0;
}

/*                                                                   */
/* View(): View IDs (or records) that meet a user-defined condition: */
/*                                                                   */
void View(void){

	extern PCONTROL g;
	
	RESULT result;
	int ViewRecordFlag=0;
	int ViewDataFlag  =0;

	/*                                     */
	/* Get keyword "for" or keyword "file" */
	/*                                     */
	GetToken();

	/*                                      */
	/* Check for presence of open database: */
	/*                                      */
	if(!g->db){
		MadelineErrorReport("View(): no pedigree table is currently open");
	}

	if(pT->i==RECORK){
		
		/*                                    */
		/* see if FOR clause is also present: */
		/*                                    */
		GetToken();
		if(pT->i==FORK){ 
			ViewRecordFlag++;
		}else{
			
			/*                               */
			/* view just the current record: */
			/*                               */
			ViewIndividualRecord(g->db);
			return;
		}
		
	}else if(pT->i==UNIQK){
		/*                                       */
		/* Process VIEW DISTINCT|UNIQUE command: */
		/*                                       */
		ViewDistinct(g);
		return;
		
	}else if(pT->i==DATAK){
		
		/* Set ViewDataFlag so we can clear out the reserve token stack   */
		/* after we are finished:                                         */
		ViewDataFlag++;
		
		/* Put tokens onto TokenStack2, the reserve stack, until we reach the FOR keyword: */
		
		/*                                                            */
		/* set globals to avoid error when evaluating the expression: */
		/*                                                            */
		g->cpd=g->pd;
		g->cp =g->pd->h;
			
		/*                                        */
		/* Go to the first record of the database */
		/* -- this record is as good as any other */
		/*    for the first evaluation of the     */
		/*    expression:                         */
		/*                                        */
		GoTo(g->db,0);
		
		/*                                                               */
		/* EvaluateExpression() starts with a GetToken()                 */
		/* after clearing the token stack:                               */
		/*                                                               */
		/* The expression is evaluated here for two reasons:             */
		/* 1. To make sure the expression is error-free.                 */
		/* 2. To place the tokens in internal form onto the token stack. */
		/*    It is evaluated on the reserve token stack where the       */
		/* parsed version is then stored for use by other functions:     */
		/*                                                               */
		EvaluateMultipleExpressions(FORK,0);
		
		if(pT->i!=FORK){
			/*                                           */
			/* Show results just for the current record: */
			/*                                           */
			EvaluateMultipleExpressions(END,1);
			ClearTokenStack(Token2,TokenStack2Size);
			pT=Token;
			return;
		}
		/* Switch pT back to the non-reserve token stack    */
		/* in order to pick up the FOR expression and place */ 
		/* that expression on the regular token stack:      */
		pT=Token;
		
	}else if(pT->i!=FORK){
		
		/* Clear the reserve stack: */
		MadelineErrorReport("View(): FILE or FOR or RECORD keyword expected:\nVIEW FILE <filename> or VIEW FOR <expr> or VIEW RECORD FOR <expr>");
		
	}

	/*                                    */
	/* get here if there is a FOR clause: */
	/*                                    */
	
	
	/*                             */
	/* set globals to avoid error: */
	/*                             */
	g->cpd=g->pd;
	g->cp =g->pd->h;
	
	
	/*                                        */
	/* Go to the first record of the database */
	/* -- this record is as good as any other */
	/*    for the first evaluation of the     */
	/*    expression:                         */
	/*                                        */
	GoTo(g->db,0);

	/*                                                    */
	/* EvaluateExpression() starts with a GetToken()      */
	/* after clearing the token stack:                    */
	/*                                                    */
	/* The first time of evaluating the expression        */
	/* is done to make sure the expression is error-free: */
	/*                                                    */
	EvaluateExpression(&result);

	/*                                              */
	/* The result must be a NUMERICVALUE to qualify */
	/* as a logical true/false result for the       */
	/* FOR expression                               */
	if(result.t!=NUMERICVALUE)
		MadelineErrorReport("View(): a numeric expression is expected.");
	
	/*                                                  */
	/* Set the expression repeat flag so that           */
	/* GetToken() will simply pull tokens off the token */
	/* stack instead of reparsing the expression:       */
	/*                                                  */
	/* NOTE: This flag is checked when processing both  */
	/* the reserve stack which can contain a list of    */
	/* data fields or attributes, as well as by the     */
	/* regular token stack which can contain the FOR    */
	/* expression.                                      */
	/*                                                  */
	RepeatInProgress=1;
	/*                                                                 */
	/* Traverse pedigrees with supplied function, depending on option: */
	/*                                                                 */

	/*                */
	/* Reset tallies: */
	/*                */
	ResetTallyCounters(g);	
	
	if(ViewRecordFlag){
		TraverseBinaryPedigreeTree2(g,g->pd,ViewIndividualsInDetail);
	}else if(ViewDataFlag){
		TraverseBinaryPedigreeTree2(g,g->pd,ViewIndividualsData);
	}else{
		TraverseBinaryPedigreeTree2(g,g->pd,ViewIndividuals);
	}
	/*                              */
	/* Done: reset RepeatInProgress */
	/*                              */
	RepeatInProgress=0;
	/*                                                     */
	/* clear the token stack if starting a new expression: */
	/*                                                     */
	if(ViewDataFlag) ClearTokenStack(Token2,TokenStack2Size);        
	
	ShowTally(g,"matched");
	
}



void ShowTally(PCONTROL c,char *message){

	char h1[10];
	char h2[10];

	int total;
	
	char *trim1;
	char *trim2;

	/*                              */
	/* total number of individuals: */
	/*                              */
	total=c->tally.din.in+
		  c->tally.csp.in+
		  c->tally.flt.in+
		  c->tally.vir.in;

	/*                                                    */
	/* Format total and no. of pedigrees for presentation */
	/* in table:                                          */
	/*                                                    */
	Itoa(h1,total,9);
	Itoa(h2,c->tally.pedg.in,9);
	/*                                                        */
	/* Trim the strings for presentation in the lead-in line: */
	/*                                                        */
	for(trim1=h1;*trim1==' ';trim1++);
	for(trim2=h2;*trim2==' ';trim2++);
		  
	lprintf("\n%s individual%s in %s pedigree%s %s as follows:\n\n",
		trim1,
		total==1?"":"s",
		trim2,
		c->tally.pedg.in==1?"":"s",
		message
	);
		
	lprintf("Individuals ..............  %s\n",
		h1
	);
	lprintf(" + In database ...........  %s\n",
		Itoa(h2,c->tally.din.in+
		        c->tally.csp.in+
		        c->tally.flt.in,9)
	);
	lprintf(" |  + Attached ...........  %s\n",
		Itoa(h2,c->tally.din.in,9)
	);
	lprintf(" |  + Childless spouses ..  %s\n",
		Itoa(h2,c->tally.csp.in,9)
	);
	lprintf(" |  + Unattached .........  %s\n",
		Itoa(h2,c->tally.flt.in,9)
	);
	lprintf(" + Not in database .......  %s\n",
		Itoa(h2,c->tally.vir.in,9)
	);
}



/*                 */
/* ViewIndividuals */
/*                 */
void ViewIndividuals(PCONTROL c,PPEDIGREE pd){
	
	TraverseIndividualBST4(c,pd,pd->h,ViewIndividual);
}

/*                 */
/* ViewIndividuals */
/*                 */
void ViewIndividualsData(PCONTROL c,PPEDIGREE pd){
	
	TraverseIndividualBST4(c,pd,pd->h,ViewIndividualData);
}


/*                                                */
/* ViewIndividualData(): View a single individual */
/*                                                */
void ViewIndividualData(PCONTROL c,PPEDIGREE pd,PINDIVIDUAL p){

	RESULT result;

	extern PCONTROL g;
	
	/*                                                               */
	/* store current pedigree and person in globally accessible      */
	/* control, c, so that recursive descent parser can access       */
	/* this information.  This allows the parser to access variables */
	/* internal to an INDIVIDUAL structure, or to evaluate the       */
	/* data of an INDIVIDUAL who's been dummied in and doesn't have  */
	/* a record in the database:                                     */
	/*                                                               */
	c->cpd=pd;
	c->cp=p;

	
	/*                                */
	/* Handle dummied in individuals: */
	/*                                */
	if(!p->db){
		/*                      */
		/* no record available: */
		/*                      */
		c->db->cr=NO_RECORD_EXISTS;
	}else{
		/*                                 */
		/* Go to this individual's record: */
		/*                                 */
		GoTo(p->db,p->rci);
	}

	/*                                                    */
	/* evaluate the expression already on                 */
	/* the token stack (assuming that RepeatInProgress is */
	/* set to TRUE, which it should be):                  */
	/*                                                    */
	EvaluateExpression(&result);
	/*                        */
	/* if true, show results: */
	/*                        */
	if(result.v && result.v!=DOUBLEMISSINGVALUE){
		/*                                          */
		/* Print Family and Individual identifiers: */
		/*                                          */
		dprintf("%s%c%s%c",pd->fid,g->delimiter,p->id,g->delimiter);
		
		/*                              */
		/* EvaluateMultipleExpressions: */
		/*                              */
		EvaluateMultipleExpressions(FORK,1);
		
		
		/*                                               */
		/* increment counters: vir,flt,csp, and din      */
		/* categories are used: dno and dex categories   */
		/* don't apply here.  Note that we are           */
		/* making a tally of how many individuals and    */
		/* pedigrees are "included" --                   */
		/* thus were going to always use "in" instead of */
		/* "ex" for the tally counts:                    */
		/*                                               */
		if     (p->csp        ) c->tally.csp.in++;
		else if(p->unconnected) c->tally.flt.in++;
		else if(!p->db        ) c->tally.vir.in++;
		else                    c->tally.din.in++;
		/*                     */
		/* has lastpd changed? */
		/*                     */
		if(c->lastpd!=pd){
			c->lastpd=pd;
			c->tally.pedg.in++;			
		}
	}
}

/*                                            */
/* ViewIndividual(): View a single individual */
/*                                            */
void ViewIndividual(PCONTROL c,PPEDIGREE pd,PINDIVIDUAL p){

	RESULT result;
	
	/*                                                               */
	/* store current pedigree and person in globally accessible      */
	/* control, c, so that recursive descent parser can access       */
	/* this information.  This allows the parser to access variables */
	/* internal to an INDIVIDUAL structure, or to evaluate the       */
	/* data of an INDIVIDUAL who's been dummied in and doesn't have  */
	/* a record in the database:                                     */
	/*                                                               */
	c->cpd=pd;
	c->cp=p;

	
	/*                                */
	/* Handle dummied in individuals: */
	/*                                */
	if(!p->db){
		/*                      */
		/* no record available: */
		/*                      */
		c->db->cr=NO_RECORD_EXISTS;
	}else{
		/*                                 */
		/* Go to this individual's record: */
		/*                                 */
		GoTo(p->db,p->rci);
	}

	/*                                                    */
	/* evaluate the expression already on                 */
	/* the token stack (assuming that RepeatInProgress is */
	/* set to TRUE, which it should be):                  */
	/*                                                    */
	EvaluateExpression(&result);
	/*                        */
	/* if true, show results: */
	/*                        */
	if(result.v && result.v!=DOUBLEMISSINGVALUE){
		if(p->db){
			dprintf("%s in %s (rec. no. %5i) %s\n",
				p->id,
				pd->fid,
				p->rci,
				p->unconnected?"* unattached *":
				p->csp        ?"* childless spouse *":
				"");
		}else{
			dprintf("%s in %s (virtual individual not in pedigree table)\n",p->id,pd->fid);
		}
		
		/*                                               */
		/* increment counters: vir,flt,csp, and din      */
		/* categories are used: dno and dex categories   */
		/* don't apply here.  Note that we are           */
		/* making a tally of how many individuals and    */
		/* pedigrees are "included" --                   */
		/* thus were going to always use "in" instead of */
		/* "ex" for the tally counts:                    */
		/*                                               */
		if     (p->csp        ) c->tally.csp.in++;
		else if(p->unconnected) c->tally.flt.in++;
		else if(!p->db        ) c->tally.vir.in++;
		else                    c->tally.din.in++;
		/*                     */
		/* has lastpd changed? */
		/*                     */
		if(c->lastpd!=pd){
			c->lastpd=pd;
			c->tally.pedg.in++;			
		}
	}
}




/*                         */
/* ViewIndividualsInDetail */
/*                         */
void ViewIndividualsInDetail(PCONTROL c,PPEDIGREE pd){
	
	TraverseIndividualBST4(c,pd,pd->h,ViewIndividualInDetail);
}



/*                                                              */
/* ViewIndividualInDetail(): View a single individual in detail */
/*                                                              */
void ViewIndividualInDetail(PCONTROL c,PPEDIGREE pd,PINDIVIDUAL p){

	RESULT result;


	/*                                                               */
	/* store current pedigree and person in globally accessible      */
	/* control, c, so that recursive descent parser can access       */
	/* this information.  This allows the parser to access variables */
	/* internal to an INDIVIDUAL structure, or to evaluate the       */
	/* data of an INDIVIDUAL who's been dummied in and doesn't have  */
	/* a record in the database:                                     */
	/*                                                               */
	c->cpd=pd;
	c->cp=p;

	
	/*                                */
	/* Handle dummied in individuals: */
	/*                                */
	if(!p->db){
		/*                      */
		/* no record available: */
		/*                      */
		c->db->cr=NO_RECORD_EXISTS;
	}else{
		/*                                 */
		/* Go to this individual's record: */
		/*                                 */
		GoTo(p->db,p->rci);
	}

	/*                                                    */
	/* evaluate the expression already on                 */
	/* the token stack (assuming that RepeatInProgress is */
	/* set to TRUE, which it should be):                  */
	/*                                                    */
	EvaluateExpression(&result);
	/*                        */
	/* if true, show results: */
	/*                        */
	if(result.v && result.v!=DOUBLEMISSINGVALUE){
		if(p->db)
			dprintf("%s in %s (rec. no. %5i) %s\n",
				p->id,
				pd->fid,
				p->rci,
				p->unconnected?"* unconnected *":
				p->csp        ?"* childless spouse *":
				"");
		else
			dprintf("%s in %s (virtual individual not in pedigree table)\n",p->id,pd->fid);			
		ViewIndividualRecord(c->db);

		/*                                               */
		/* increment counters: vir,flt,csp, and din      */
		/* categories are used: dno and dex categories   */
		/* don't apply here.  Note that we are           */
		/* making a tally of how many individuals and    */
		/* pedigrees are "included" --                   */
		/* thus were going to always use "in" instead of */
		/* "ex" for the tally counts:                    */
		/*                                               */
		if     (p->csp        ) c->tally.csp.in++;
		else if(p->unconnected) c->tally.flt.in++;
		else if(!p->db        ) c->tally.vir.in++;
		else                    c->tally.din.in++;
		/*                     */
		/* has lastpd changed? */
		/*                     */
		if(c->lastpd!=pd){
			c->lastpd=pd;
			c->tally.pedg.in++;			
		}
	}
}



/*                                */
/* Clear(): Clears all exclusions */
/*                                */
void Clear(void){

	
	extern PCONTROL g;


	/*                                      */
	/* Check for presence of open database: */
	/*                                      */
	if(!g->db)
		MadelineErrorReport("Clear(): no pedigree table is currently open");


	/*                   */
	/* Get "exclusions": */
	/*                   */
	GetToken();
	if(pT->i!=EXCLK)
		MadelineErrorReport("Clear(): EXCLUSIONS keyword expected: CLEAR EXCLUSIONS");
	
	/*                                            */
	/* Traverse pedigrees with supplied function: */
	/*                                            */
	TraverseBinaryPedigreeTree(g->pd,ClearPedigreeExclusions);

}


/*                                                                   */
/* ClearPedigreeExclusions(): Clear the exclusion flags of pedigrees */
/*                                                                   */
void ClearPedigreeExclusions(PPEDIGREE pd){
	/*                       */
	/* reset pd->ex to zero: */
	/*                       */
	if(pd->ex){
		pd->ex=0;
		TraverseIndividualBST(pd->h,ClearExcludeFlag);
	}
}

/*                    */
/* ClearExcludeFlag() */
/*                    */
void ClearExcludeFlag(PINDIVIDUAL p){
		
	if(p->excluded) p->excluded=0;
}



/*                              */
/*                              */
/* Aspex data header processor: */
/*                              */
/*                              */
/*                              */
void AspexDataHeaderProcessor(PCONTROL c,char *fn){

	int i,ii;
	double lpos=0.0; /*inter-marker distances */
	PMARKERMAP mp;


	/*                                          */
	/* a map database is required for the Aspex */
	/* control file:                            */
	/*                                          */
	if((c->OutputFormat==SIBIBD || c->OutputFormat==SIBPHS) && !c->mp)
		MadelineErrorReport("AspexDataHeaderProcessor(): a marker map table is required\nfor the sib_ibd and sib_phase control files");

	/*                                                 */
	/* Affection status field is optional for Aspex:   */
	/* When not present, the programs make assumptions */
	/* about affection status, so it's a good idea to  */
	/* have it in the database:                        */
	/*                                                 */
	if(gfnAffected.i==MISSING)
		eprintf(WARNINGFLAG,"AspexDataHeaderProcessor():\naffection status field is not present in pedigree table");
	/*                         */
	/* we also write to a .par */
	/* parameter file:         */
	/*                         */
	StoreString(&gParameterOutputFile,fn);

	/*                                                         */
	/* change ending to .tcl by overwriting last 4 characters: */
	/*                                                         */
	strncpy(gParameterOutputFile.e.s+strlen(gParameterOutputFile.e.s)-4,
	        ".tcl",4);

	/*                                     */
	/* Then open up the .tcl file for use: */
	/*                                     */
	c->streams.fpar=fopen(gParameterOutputFile.e.s,"wt");
	if(!c->streams.fpar)
		MadelineErrorReport("AspexDataHeaderProcessor(): unable to open \"%s\" for output",gParameterOutputFile.e.s);

	lprintf("Creating associated Aspex TCL parameter file called \"%s\"\n",gParameterOutputFile.e.s);

	/*                               */
	/* Write well-commented defaults */
	/* for the parameter file:       */
	/*                               */
	
	/*                                                         */
	/* parameters common to all programs in the Aspex package: */
	/*                                                         */
	fprintf(c->streams.fpar,"#\n");
	fprintf(c->streams.fpar,"# Aspex Parameter file generated by Madeline on %s\n",DelimitedGenericDateString(JulianToday(),'.'));
	fprintf(c->streams.fpar,"#\n");
	fprintf(c->streams.fpar,"# Parameters common to all Aspex programs:\n");
	fprintf(c->streams.fpar,"#\n\n");
	
	fprintf(c->streams.fpar,"# The number of marker loci\n");
	fprintf(c->streams.fpar,"set nloc %i\n",c->db->n.go);

	fprintf(c->streams.fpar,"# Marker names\n");
	fprintf(c->streams.fpar,"set loc { \n");
	/*                                     */
	/* print out the names of the markers: */
	/*                                     */
	for(i=0;i<c->db->n.go;i++){
		/*                       */
		/* Get true field index: */
		/*                       */
		ii=c->db->fi->g[i];
		/*                             */
		/* print out the marker names: */
		/*	                            */
		fprintf(c->streams.fpar,"\"%s\"%s",
			c->db->FieldName(c->db,ii),
			(i+1)%5==0?" \n":" ");
	}
	fprintf(c->streams.fpar,"\n}\n");

	fprintf(c->streams.fpar,"# Allele identifier for missing data\n");
	fprintf(c->streams.fpar,"set blank \"0\"\n");

	fprintf(c->streams.fpar,"# Width of family identifiers\n");
	fprintf(c->streams.fpar,"set fid_width 8\n");

	fprintf(c->streams.fpar,"# Width of person identifiers\n");
	fprintf(c->streams.fpar,"set pid_width 8\n");

	fprintf(c->streams.fpar,"# Allele name widths\n");
	fprintf(c->streams.fpar,"set allele_width 8\n");

	fprintf(c->streams.fpar,"# Discard partially typed genotypes?\n");
	fprintf(c->streams.fpar,"set discard_partial true\n");

	fprintf(c->streams.fpar,"# Is data for the sex chromosomes?\n");
	fprintf(c->streams.fpar,"set sex_linked false\n");

	if(c->OutputFormat==SIBIBD || c->OutputFormat==SIBPHS){
		/*                                             */
		/* specific to sib_ibd and sib_phase programs: */
		/*                                             */
		fprintf(c->streams.fpar,"#\n\n");
		fprintf(c->streams.fpar,"# Parameters specific to sib_ibd and sib_phase programs:\n");
		fprintf(c->streams.fpar,"#\n\n");	

		/*                                                              */
		/* First and last distances are dummied-in distances out to the */
		/* telomeres:                                                   */
		/*                                                              */
		fprintf(c->streams.fpar,"# Distances between markers in Morgans\n");
		fprintf(c->streams.fpar,"set dist { \n0.10 \n");

		/*                                              */
		/* Print out inter-marker distances in Morgans: */
		/*                                              */
		for(i=0;i<c->db->n.go;i++){
			/*                       */
			/* Get true field index: */
			/*                       */
			ii=c->db->fi->g[i];
			/*                     */
			/* get the marker map: */
			/*                     */
			mp=LookupMarkerMap(c->mp,c->db->FieldName(c->db,ii));
			/*                     */
			/* check against NULL: */
			/*                     */
			if(!mp){
				GenericLocusFooterProcessor(c);
				MadelineErrorReport("AspexDataHeaderProcessor():\n\tNo map information available for %s",c->db->FieldName(c->db,ii));
			}
			/*                                         */
			/* print out intermarker distances as soon */
			/* as the second marker map is seen:       */
			/*                                         */
			if(i){
				fprintf(c->streams.fpar,"%7.5f%s",
					(mp->ps-lpos)/100,
					i%5?" ":" \n");
			}
			/*                                */
			/* save the most recent position: */
			/*                                */
			lpos=mp->ps;
		}
		fprintf(c->streams.fpar,"\n0.10 \n}\n");
		/*                                            */
		/* (dist_xx and dist_xy are not yet included) */
		/*                                            */
		fprintf(c->streams.fpar,"# Mapping function for recomb. fractions\n");
		fprintf(c->streams.fpar,"set mapping Haldane\n");

		/*                        */
		/* recurrence risk ratio: */
		/*                        */
		fprintf(c->streams.fpar,"# Sibling recurrence risk ratio:\n");
		fprintf(c->streams.fpar,"set risk 1.32\n");
				
		fprintf(c->streams.fpar,"# Identity by descent probabilities\n");
		fprintf(c->streams.fpar,"set z \"[expr 0.25/$risk] 0.50 [expr 0.50-0.25/$risk]\"\n");
	
		fprintf(c->streams.fpar,"# Maximum gap to leave between data points in lod map in Morgans\n");
		fprintf(c->streams.fpar,"set max_step 0.01\n");
	
		fprintf(c->streams.fpar,"# Should gap size between map points be fixed?\n");
		fprintf(c->streams.fpar,"set fix_step false\n");
	
		fprintf(c->streams.fpar,"# Should max likelihood for sharing be done at each locus\n");
		fprintf(c->streams.fpar,"set most_likely false\n");
		
		fprintf(c->streams.fpar,"# Fit to a linear model?\n");
		fprintf(c->streams.fpar,"set linear_model true\n");
	
		fprintf(c->streams.fpar,"# No dominance variance in linear model?\n");
		fprintf(c->streams.fpar,"set no_Dv false\n");
	
		fprintf(c->streams.fpar,"# If most_likely=true, then this indicates that\n");
		fprintf(c->streams.fpar,"# sharing should be at least 50%% for likelihood maxim.:\n");	
		fprintf(c->streams.fpar,"set truncate_sharing true\n");
	
		fprintf(c->streams.fpar,"# Should only strictly independent sibs be counted?\n");
		fprintf(c->streams.fpar,"set count_once false\n");
	
		fprintf(c->streams.fpar,"# Should only first appropriate sib be counted?\n");
		fprintf(c->streams.fpar,"set first_pair false\n");
	
		fprintf(c->streams.fpar,"# Should pair be counted where first sib is unaffected?\n");
		fprintf(c->streams.fpar,"set count_unaffected false\n");
	
		fprintf(c->streams.fpar,"# Should disease status of sibs in a pair be discordant?\n");
		fprintf(c->streams.fpar,"set count_discordant false\n");
	
		fprintf(c->streams.fpar,"# Error frequency at which to identify likely typing errors\n");
		fprintf(c->streams.fpar,"set error_freq 0\n");
	}
	/*                                           */
	/* parameters specific to specific programs: */
	/*                                           */
	switch(c->OutputFormat){
	case SIBIBD:
		fprintf(c->streams.fpar,"#\n# Parameters specific to sib_ibd program:\n#\n");
		
		fprintf(c->streams.fpar,"# Restrict parent reconstructions to only sibs that are not part of counted pairs?\n");
		fprintf(c->streams.fpar,"set limit_build false\n");

		fprintf(c->streams.fpar,"# Sort sibs by number of typed loci?\n");
		fprintf(c->streams.fpar,"set best_order false\n");			

		fprintf(c->streams.fpar,"# Calculate separate LOD scores for paternal and maternal sharing?\n");
		fprintf(c->streams.fpar,"set sex_split false\n");

		break;

	case SIBPHS:
		fprintf(c->streams.fpar,"#\n# Parameters specific to sib_phase program:\n#\n");

		fprintf(c->streams.fpar,"# Use allele frequencies to estimate IBD probabilities when parents missing?\n");
		fprintf(c->streams.fpar,"set use_allele_freq true\n");

		fprintf(c->streams.fpar,"# Calculate separate LOD scores for paternal and maternal sharing?\n");
		fprintf(c->streams.fpar,"set sex_split false\n");

		fprintf(c->streams.fpar,"# Allele freq. specified in file?\n");
		fprintf(c->streams.fpar,"set fixed_freq true\n");

		for(i=0;i<c->db->n.go;i++){
			/*                                           */
			/* i is the rank index, not the field index: */
			/*                                           */
			OutputAspexMarker(c,i);

		}				
	
		break;
	case SIBMAP:
		fprintf(c->streams.fpar,"#\n# Parameters specific to sib_map program:\n#\n");

		fprintf(c->streams.fpar,"# LOD score cutoff for support intervals\n");
		fprintf(c->streams.fpar,"set support 1.0\n");

		fprintf(c->streams.fpar,"# Convergence criterion for iterative distance refinement routines\n");
		fprintf(c->streams.fpar,"set epsilon 0.00001\n");
	
		fprintf(c->streams.fpar,"# Perform map shuffling?\n");
		fprintf(c->streams.fpar,"set do_shuffle false\n");

		fprintf(c->streams.fpar,"# Calculate separate maternal and paternal recombination maps?\n");
		fprintf(c->streams.fpar,"set sex_split false\n");

		break;
	case SIBTDT:
		fprintf(c->streams.fpar,"#\n# Parameters specific to sib_tdt program:\n#\n");

		fprintf(c->streams.fpar,"# ->There are no sib_tdt-specific parameters\n");
	
		break;
	case KINSHP:
		fprintf(c->streams.fpar,"#\n# Parameters specific to kinship program:\n#\n");

		fprintf(c->streams.fpar,"# Report IBS for relative pairs with putative parents?\n");
		fprintf(c->streams.fpar,"set count_parents false\n");
	
		fprintf(c->streams.fpar,"# Compare likelihoods for full & half sibs, cousins, & 2nd cousins?\n");
		fprintf(c->streams.fpar,"set four_way false\n");

		break;
	}
	
	
	/*                                                         */
	/* Call generic header processor to open main output file: */
	/*                                                         */
	GenericDataHeaderProcessor(c,fn);
	/*                                                    */
	/* write the names of the markers as the header line: */
	/*                                                    */
	/*                                                    */
	/* print out the names of the markers:                */
	/*                                                    */
	for(i=0;i<c->db->n.go;i++){
		/*                  */
		/* Get field index: */
		/*                  */
		ii=c->db->fi->g[i];
		/*                             */
		/* print out the marker names: */
		/*	                            */
		fprintf(c->streams.fp,"%s ",c->db->FieldName(c->db,ii));
	}
	/*                                 */
	/* carriage return at end of line: */
	/*                                 */
	fprintf(c->streams.fp,"\n");
	
	/*                                                    */
	/* done: data are output by AspexNuclearBlockOutput() */
	/*                                                    */
}


/*                                                                  */
/*                                                                  */
/* OutputAspexIndividualRecord():                                   */
/*                                                                  */
/* NOTE: set "parent" to 1 (TRUE) for parental records, so that the */
/*       father and mother id fields will be blanked out.           */
/*                                                                  */
/*                                                                  */
void OutputAspexIndividualRecord(PCONTROL c,char *famid,PINDIVIDUAL p, int parent){

	int i,ii;
	char *o,*allele1,*allele2;
		
	/*                                                */
	/* Write core fixed fields: famid, individual id, */
	/*                                                */
	/* the sequential individual ID:                  */
	/*                                                */
	fprintf(c->streams.fp,"%-8.8s %-8.8s %-8.8s %-8.8s %c ",
		famid,
		p->id,
		(p->fid[0]==CHARMISSINGVALUE || parent)?"0":p->fid,
		(p->mid[0]==CHARMISSINGVALUE || parent)?"0":p->mid,
		(p->female==MISSING?'0':(p->female?'f':'m'))
	);
	
	/*                                     */
	/* Write affection status, if present: */
	/*                                     */
	if(gfnAffected.i!=MISSING)
		fprintf(c->streams.fp,"%c ",p->affected==MISSING?'u':(p->affected?'y':'n'));
	
	/*                         */
	/* Output genotype fields: */
	/*                         */
	if(p->db){
		/*                                                  */
		/* print out only genotype fields in natural order: */
		/*                                                  */
		for(i=0;i<p->db->n.go;i++){
			/*                  */
			/* Get field index: */
			/*                  */
			ii=p->db->fi->g[i];
			/*                          */
			/* read the genotype field: */
			/*                          */
			o=p->db->ReadString(p->db,NULL,ii,p->rci);
			/*                                          */
			/* check for missing values:                */
			/*                                          */
			/* if(IsCMV(o)) o="0/0";                    */
			/*                                          */
			/*if(!strcmp(o,CHARMISSINGSTRING)) o="0/0"; */
			/*                                          */
			if(!o[0]){
				allele1=allele2="0";
			}else{
				/*                                     */
				/* get allele1 and allele2 substrings: */
				/*                                     */
				for(allele1=allele2=o;*allele2 && *allele2!='/';allele2++);
				*allele2++='\0';
			}
			fprintf(c->streams.fp,"%*.*s %*.*s  ",
				CRIMAPALLELEFIELDWIDTH,
				CRIMAPALLELEFIELDWIDTH,
				allele1,
				CRIMAPALLELEFIELDWIDTH,
				CRIMAPALLELEFIELDWIDTH,
				allele2
			);
		}       
	}else{
		/*                                                      */
		/* print out zeros for both alleles of genotype fields: */
		/*                                                      */
		allele1=allele2="0";
		for(i=0;i<c->db->n.go;i++){
			fprintf(c->streams.fp,"%*.*s %*.*s  ",
				CRIMAPALLELEFIELDWIDTH,
				CRIMAPALLELEFIELDWIDTH,
				allele1,
				CRIMAPALLELEFIELDWIDTH,
				CRIMAPALLELEFIELDWIDTH,
				allele2
			);
		}
	}
	fprintf(c->streams.fp,"\n");
	/*                            */
	/* Mark individual as output: */
	/*                            */
	p->included++;
}


/*                          */
/*                          */
/* AspexNuclearBlockOutput: */
/*                          */
/*                          */
void AspexNuclearBlockOutput(PCONTROL c,PPEDIGREE pd){

	int i,j;

	/*                                                        */
	/* Check all individuals for data under current criteria: */
	/*                                                        */
	TraverseIndividualBSTMarkDataFlag(pd->h,GeneralCaseIndividualHasData);
	
	/*                                                              */
	/* Count any unattached indivdiuals or childless spouses before */
	/* anything else:                                               */
	/*                                                              */
	TraverseIndividualBST2(c,pd->h,CountExcludedUnattachedIndividual);
	
	/*                                                   */
	/* reset pedigree sequential id: this is used to     */
	/* count nuclear blocks rather than actual pedigrees */
	/*                                                   */
	c->psid=1;
	/*                                                                   */
	/* Loop over each founding mom and each spouse of each founding mom: */
	/*                                                                   */
	for(i=0;i<pd->nfm;i++){
		for(j=0;j<pd->fm[i]->na;j++){
			/*                                                      */
			/* parameters are CONTROL structure c, the PEDIGREE pd, */
			/* the founding female, pd->fm[i], and her jth spouse   */
			/* pd->fm[i]->a[j]                                      */
			/*                                                      */
			TraverseAspexNuclearBlock(c,pd,pd->fm[i],pd->fm[i]->a[j]);
		}		
	}
		
	/*                                                          */
	/* if c->psid is now greater than one, that means at        */
	/* least one nuclear family from this pedigree was included */
	/* --otherwise, no nuclear families were included, so we    */
	/*   can say that the whole pedigree was excluded:          */
	/*                                                          */
	if(c->psid>1)
		c->counters.pedg.in++;
	else
		c->counters.pedg.ex++;
		
}


/*                                                     */
/* TraverseAspexNuclearBlock():                        */
/* -->s1 and s2 are the nuclear founding parental pair */
/*                                                     */
void TraverseAspexNuclearBlock(PCONTROL c,PPEDIGREE pd,PINDIVIDUAL s1,PINDIVIDUAL s2){

	int i,j;
	PINDIVIDUAL mom,dad;
	char hold[MINIMUMBUFFERSIZE];
	int ninc;

	/*                                                      */
	/* return if no children present: not a nuclear family: */
	/*                                                      */
	if(!s1->nk) return;
	
	/*                                                 */
	/* Get pointer to the female in the parental pair: */
	/*                                                 */
	if(s1->female==1){
		mom=s1;
		dad=s2;	
	}else{
		mom=s2;
		dad=s1;
	}
		
	/*                         */
	/* Check for data present: */
	/*                         */
	ninc=AspexNuclearFamilyHasData(c,mom,dad);
	
	if(ninc){
		/*                                */
		/* construct a nuclear family id: */
		/*                                */
		hold[0]='\0';
		sprintf(hold,"%s-%i",pd->fid,c->psid);

		/*                                                            */
		/* increment sequential nuclear block counter for next round: */
		/*                                                            */
		c->psid++;
		
		/*                                         */
		/* Output parents first:                   */
		/* -- Note that "parent" flag is set to 1: */
		/*	                                        */
		OutputAspexIndividualRecord(c,hold,mom,1);
		OutputAspexIndividualRecord(c,hold,dad,1);
		/*                                          */
		/* then kids: "parent" flag is set to zero: */
		/*                                          */
		for(i=0;i<mom->nk;i++){
			if(mom->k[i]->f==dad && mom->k[i]->hasdata){
				OutputAspexIndividualRecord(c,hold,mom->k[i],0);
			}
		}
	}
	
	/*                                                                */
	/* Recursive calls for printing nuclear blocks for the offspring: */
	/* --> limit to offspring of the pair, without regard to          */
	/*     the presence or absence of data:                           */

	/*                            */
	/* loop over mom's offspring: */
	/*                            */
	for(i=0;i<mom->nk;i++){
		if(mom->k[i]->f!=dad) continue;
		/*                                       */
		/* loop over all mates of the offspring: */
		/*                                       */
		for(j=0;j<mom->k[i]->na;j++){
			TraverseAspexNuclearBlock(c,pd,mom->k[i],mom->k[i]->a[j]);
		}
	}
	
}


/*                                                                    */
/* AspexNuclearFamilyHasData():                                       */
/*                                                                    */
/* - For Aspex, we check if the individuals have                      */
/*   genotype data.                                                   */
/*                                                                    */
/*   Note: included==1 if previous level was included, 0 if excluded. */
/*         --We need this information in order to count included and  */
/*           excluded individuals exactly without double counting     */
/*           (we also do double counting in nind)                     */
/*                                                                    */
int AspexNuclearFamilyHasData(PCONTROL c,PINDIVIDUAL s1,PINDIVIDUAL s2){

	/* int i; */
	PINDIVIDUAL mom,dad;
	int nio;

	if(s1->female==1){
		mom=s1;
		dad=s2;
	}else{
		mom=s2;
		dad=s1;
	}


	if(mom->hasdata ||
	   dad->hasdata ||
	   OffspringOfPairHasData(mom,dad)
	){
		return CountIncludedNuclearPedigree(c,mom,dad);
	}else{
		nio=CountExcludedNuclearPedigree(c,mom,dad);
		dprintf("AspexNuclearFamilyHasData():\nExcluding %i individuals in pedigree %s in nuclear pedigree formed by %s and %s:\n\tno individual with genotype data\n",
			nio,
			mom->pd->fid,
			mom->id,
			dad->id
		);
		return 0;
	}
}


/*                                             */
/*                                             */
/* OutputAspexMarker()                         */
/*                                             */
/* - i is the rank index, not the field index: */
/*                                             */
void OutputAspexMarker(PCONTROL c,int i){

	int ii;

	/*                       */
	/* Get true field index: */
	/*                       */
	ii=c->db->fi->g[i];

	fprintf(c->streams.fpar,"freq \"%s\" {\n",c->db->FieldName(c->db,ii));
	
	TraverseAllele3(c->streams.fpar,c->db->m+ii,c->db->m[ii].a,c->db->m[ii].s,OutputAspexMarkerAllele);
	
	fprintf(c->streams.fpar,"\n}\n");
}

/*                                         */
/*                                         */
/* OutputAspexMarkerAllele: generic format */
/*                                         */
void OutputAspexMarkerAllele(FILE *fp,PMARKER m,PALLELE a,int s){

	fprintf(fp,"\"%i\" %6.4f%c",
		a->v,
		(double)a->n/(double)s,
		a->i%4==0?'\n':' '
	);
}


/*                                                               */
/* CheckTwins(): Check for unpaired individuals marked as twins: */
/*                                                               */
void CheckTwins(PPEDIGREE pd){

	int i;
	/*                                                          */
	/* loop through all founding moms:                          */
	/*                                                          */
	/* CheckTwinStatus() works recursively through family tree: */
	/*	                                                         */
	for(i=0;i<pd->nfm;i++)
		CheckTwinStatus(pd->fm[i]);
}



/*                     */
/* CheckTwinStatus():  */
/*                     */
void CheckTwinStatus(PINDIVIDUAL p){
	
	int i;
	
	/*                                                          */
	/* Note that UnmarkSingletonTwins() calls CheckTwinStatus() */
	/* to do recursion on the next generation of offspring:     */
	/*                                                          */
	
	if(p->female){
		UnmarkSingletonTwins(p);
	}else{
		/*                              */
		/* loop through female spouses: */
		/*                              */
		for(i=0;i<p->na;i++){
			UnmarkSingletonTwins(p->a[i]);
		}
	}
}


/*                                                              */
/* UnmarkSingletonTwins(): Unmark any stray single individuals  */
/* who remain marked as part of a twin group, presumably due to */
/* elimination of other individuals in the sibship:             */
/*                                                              */
/* f - female with children                                     */
/*                                                              */
void UnmarkSingletonTwins(PINDIVIDUAL f){

	PTWINGROUP t=NULL;
	PTWINGROUP w;
	int i,j;
	
	char MaxDZMark='a'-1;
	char MaxTWMark='1'-1;	
	char c;
	
	/*                                  */
	/* return if there are no children: */
	/*                                  */
	if(!f->nk) return;
	
	/*                                                             */
	/* Determine if twin groups are present                        */
	/* and store the twin identifier key(s) and monozygote counts. */
	/* Usually there will only be a single twin pair,              */
	/* but the code supports __ANY__ number of                     */
	/* twin groups via the BST with root node t:                   */
	/*                                                             */
	for(i=0;i<f->nk;i++){
		c=f->k[i]->twin; /* a letter, or else '\0' (0) */
		if(c){
			t=InsertTwinGroup(t,c,f->k[i]);
			/*                                               */
			/* keep track of the maximum symbol used for DZ: */
			/* (no point in doing this for MZ)               */
			/*                                               */
			if(islower(c)){
				/*               */
				/* confirmed DZ: */
				/*               */
				if(c>MaxDZMark) MaxDZMark=c;				
			}
		}
	}
	/*                                                 */
	/* Now, unmark "twin" flags for any stray singles: */
	/*                                                 */
	for(i=0;i<f->nk;i++){
		if(f->k[i]->twin){
			w=LookupTwinGroup(t,f->k[i]->twin);
			if(w->n==1){
				/*                                 */
				/* This is a singleton, so unmark: */
				/*                                 */
				dprintf("%s's twin \"%c\" siblings in pedigree %s not present in database: twin flag removed.\n",
					f->k[i]->id,
					f->k[i]->pd->fid,
					f->k[i]->twin
				);
				f->k[i]->twin='\0';
			}
		}
	}
	
	/*                                                    */
	/* Now check for TWIN DISAGREEMENT OR UNMARKED TWINS: */
	/* by going over all sibs pairwise:                   */
	/*                                                    */
	for(i=0;i<f->nk;i++){
		for(j=i+1;j<f->nk;j++){
			/*                           */
			/* check for unmarked twins: */
			/*                           */
			if(f->k[i]->dob!=MISSINGDATE && f->k[i]->dob==f->k[j]->dob){
				/*                                      */
				/* same birth dates, so apparent TWINS: */
				/*                                      */

				if(!(f->k[i]->twin || f->k[j]->twin)){
					/*                                  */
					/* One or other not marked as twin: */
					/* get a symbol:                    */
					/*                                  */
					if     (f->k[i]->twin) c=f->k[i]->twin;
					else if(f->k[j]->twin) c=f->k[j]->twin;
					else{
						/*                         */
						/* neither marked as twin: */
						/*                         */
						if( f->k[i]->female==MISSING ||
							f->k[j]->female==MISSING ||
							f->k[i]->female==f->k[j]->female
						){
							/*                                       */
							/* same sex or else sex of one or both   */
							/* is missing, so we                     */
							/* can't say for sure what kind of twin: */
							/*                                       */
							c=++MaxTWMark;
						}else{
							/*                                   */
							/* different sex, sex of both known: */
							/*                                   */
							c=++MaxDZMark;
						}
					}
					/*             */
					/* Mark twins: */
					/*             */
					f->k[i]->twin=c;
					f->k[j]->twin=c;
					/*                 */
					/* Report marking: */
					/*                 */
					lprintf("NOTE: %s and %s in pedigree %s now marked with \"%c\" indicating\nan apparent %s twinship.\n",
						f->k[i]->id,
						f->k[j]->id,
						f->k[i]->pd->fid,
						c,
						(c>='1'&&c<='9')?"MZ/DZ indeterminate":"dizygotic");
						
				}else if(f->k[i]->twin!=f->k[j]->twin){
					/*                                        */
					/* twins mismarked: must have same symbol */
					/*                                        */
					ErrorReport("The twinship between %s (%c) and %s (%c) in pedigree %s must be\nidentified by a single symbol.",
						f->k[i]->id,
						f->k[i]->twin,
						f->k[j]->id,
						f->k[j]->twin,
						f->k[i]->pd->fid);
				}
			}else{
				/*                                                */
				/* make sure different birth dates are not twins: */
				/*                                                */
				if(f->k[i]->twin && f->k[i]->twin==f->k[j]->twin){
					if(f->k[i]->dob==MISSINGDATE){
						lprintf("NOTE: Dates of birth missing for twins %s and %s in pedigree %s.\n",
							f->k[i]->id,
							f->k[j]->id,
							f->k[i]->pd->fid);
					}else{
						eprintf(WARNINGFLAG,"WARNING: Date of births disagree for twins %s and %s in pedigree %s.\n",
							f->k[i]->id,
							f->k[j]->id,
							f->k[i]->pd->fid);
					}
				}
			}		
		}
	}
	

	/*         */
	/* Free t: */
	/*         */
	if(t) FreeTwinGroup(t);

		
	/*                    */
	/* Recurse offspring: */
	/*                    */
	for(i=0;i<f->nk;i++)
		CheckTwinStatus(f->k[i]);
		
}


/*                                */
/*                                */
/* RelpairLocusHeaderProcessor(): */
/*                                */
/*                                */
void RelpairLocusHeaderProcessor(PCONTROL c,char *fn){

	/*                                                    */
	/* a map database is required for Relpair locus file: */
	/*                                                    */
	if(!c->mp)
		MadelineErrorReport("RelpairLocusHeaderProcessor(): a marker map table is required\nfor the locus file");
	/*                                                       */
	/* Call generic locus header processor to open the file: */
	/*                                                       */
	GenericLocusHeaderProcessor(c,fn);
}


/*                                            */
/*                                            */
/* OutputRelpairMarker()                      */
/*                                            */
/* -i is the rank index, not the field index: */
/*                                            */
void OutputRelpairMarker(PCONTROL c,int i){

	PMARKERMAP p;
	int ii;
	
	/*                  */
	/* Get field index: */
	/*                  */
	ii=c->db->fi->g[i];
	/*                               */
	/* Lookup the marker in the map: */
	/*                               */
	p=LookupMarkerMap(c->mp,c->db->FieldName(c->db,ii));
	
	if(!p){
		/*                   */
		/* clean up streams: */
		/*                   */
		GenericDataFooterProcessor(c);
		MadelineErrorReport("OutputRelpairMarker(): Can't find marker %s in map",c->db->FieldName(c->db,ii));
	}
	/*                                           */
	/* Print the marker's chromosome and         */
	/* map position in Morgans after the regular */
	/* MENDEL-style header:                      */
	/*                                           */
	fprintf(c->streams.fp,"%-8.8sAUTOSOME%2i 0%4i%8.5f\n",
		c->db->FieldName(c->db,ii),
		c->db->m[ii].n,
		p->ch,
		p->ps/100
		);
	
	TraverseAllele2(c->streams.fp,c->db->m[ii].a,c->db->m[ii].s,OutputRelpairMarkerAllele);
}


/*                            */
/*                            */
/* OutputRelpairMarkerAllele: */
/*                            */
void OutputRelpairMarkerAllele(FILE *fp,PALLELE a,int s){

	fprintf(fp,"%3i     %8.5f\n",
		a->v,
		(double)a->n/(double)s
	);
}


/*                               */
/* RelpairDataHeaderProcessor(): */
/*                               */
/*                               */
void RelpairDataHeaderProcessor(PCONTROL c,char *fn){

	char hold[MINIMUMBUFFERSIZE];
	char *h,*s;
	int i,ii;
	int w; /* output field width: adjusted, if necessary */
	int minmarks;

	/*                         */
	/* Store output file name: */
	/*                         */
	StoreString(&gOutputFile,fn);
	/*                                       */
	/* we also write to a .ctl control file: */
	/*                                       */
	StoreString(&gParameterOutputFile,fn);

	/*                                                         */
	/* change ending to .ctl by overwriting last 4 characters: */
	/*                                                         */
	strncpy(gParameterOutputFile.e.s+strlen(gParameterOutputFile.e.s)-4,
	        ".ctl",4);
		
	/*                                         */
	/* Then open up the .ctl file for writing: */
	/*                                         */
	c->streams.fpar=fopen(gParameterOutputFile.e.s,"wt");
	if(!c->streams.fpar)
		MadelineErrorReport("RelpairDataHeaderProcessor(): unable to open \"%s\" for output",gParameterOutputFile.e.s);

	lprintf("Creating associated Relpair control file called \"%s\"\n",gParameterOutputFile.e.s);

	/*                                         */
	/* get the core file name minus extension: */
	/*                                         */
	for(h=hold,s=gOutputFile.e.s;*s && *s!='.';*h++=*s++);
	*h++='.';
	*h='\0';

	/*                         */
	/* Write defaults          */
	/* for the parameter file: */
	/*                         */
	fprintf(c->streams.fpar,"%sloc\n",hold);
	fprintf(c->streams.fpar,"%sped\n",hold);
	fprintf(c->streams.fpar,"%sout\n",hold);
	/*                                      */
	/* options here are "family" vs. "all": */
	/*                                      */
	fprintf(c->streams.fpar,"all\n");
	/*                                                                  */
	/* The following options are:                                       */
	/*                                                                  */
	/* echo marker map info to output file? y/n                         */
	/* echo pedigree data to output file? y/n                           */
	/* female symbol in data file: F                                    */
	/* male symbol in data file: M                                      */
	/* minimum number of markers typed and shared to analyze a pair: 50 */
	/*                                                                  */
	/* For minimum number of markers, take 80% of marker count if fewer */
	/* than 50, otherwise, take 50:                                     */
	/*                                                                  */
	minmarks=(int)(c->db->n.go*0.25);
	if(minmarks<1) minmarks=1;
	
	fprintf(c->streams.fpar,"y\nn\nF\nM\n%i\n",minmarks);

	/*                                                                    */
	/* call generic to open output stream, store file name, reset control */
	/* counters, and inform user:                                         */
	/*                                                                    */
	GenericDataHeaderProcessor(c,fn);
		
	/*                                                  */
	/* MENDEL family header record FORTRAN read format: */
	/*                                                  */
	fprintf(c->streams.fp,"(I3,1X,A8)\n");	
	/*                                             */
	/* for id,fid,mid,sex,mzstatus, and 1X spacer: */
	/* (core output set):                          */
	/*                                             */
	fprintf(c->streams.fp,"(3(A8,1X),2(A1,1X)");
	/*                                                                  */
	/* phenotype fields first: these will be output with individualized */
	/* widths - really should not have any phenotype fields for Relpair */
	/*                                                                  */
	for(i=0;i<c->db->n.po;i++){
		/*                  */
		/* get field index: */
		/*                  */
		ii=c->db->fi->p[i];

		/*                                                     */
		/* Fix width if it exceeds MENDEL's 8-character width: */
		/*                                                     */
		if((w=c->db->FieldLength(c->db,ii))>MAXIMUMMENDELFIELDWIDTH)
			w=MAXIMUMMENDELFIELDWIDTH;
		/*                                     */
		/* print the FORTRAN format specifier: */
		/*                                     */
		fprintf(c->streams.fp,",A%i,1X",w);
	}
	/*                                                     */
	/* Now genotype fields: all fixed width xxx/xxx A7,1X: */
	/*                                                     */
	fprintf(c->streams.fp,",%i(A%i,1X)",c->db->n.go,MENDELGENOTYPEFIELDWIDTH);
	/*                                                        */
	/* terminate the FORTRAN record with closing parentheses  */
	/* and a carriage return:                                 */
	/*                                                        */
	fprintf(c->streams.fp,")\n");
}


/*                         */
/*                         */
/* AutoSetFieldOrdering(): */
/*                         */
/*                         */
void AutoSetFieldOrdering(PCONTROL c,PDBF db){
	
	int i;
	int cc;
	int pp;
	int gg;
	
	/*                                                    */
	/* "c"ore      field pointer starts at zero:          */
	/* "p"henotype field pointer starts after end of "c": */
	/* "g"enotype  field pointer starts after end of "p": */
	/*                                                    */
	db->fi->c=db->fi->i;
	db->fi->p=db->fi->i+db->n.c;
	db->fi->g=db->fi->i+db->n.c+db->n.p;
	/*  */
	/*  */
	/*  */
	for(cc=pp=gg=i=0;i<db->fc;i++){
		/*                                      */
		/* only assign orders to output fields: */
		/*                                      */
        if(db->ff[i]&OUTPUTSET){
			/*                                                         */
			/* assign orders by field type, known (core), phenotype or */
			/* genotype set:                                           */
			/*                                                         */
			if      (db->ff[i]&KNOWNSET    ){
				db->fi->c[cc++]=i;      /* store actual field index */
				db->fi->r[i   ]=cc;     /* store rank of this field */
			}else if(db->ff[i]&PHENOTYPESET){
				db->fi->p[pp++]=i;      /* store actual field index */
				db->fi->r[i   ]=pp;     /* store rank of this field */
			}else if(db->ff[i]&GENOTYPESET ){
				db->fi->g[gg++]=i;      /* store actual field index */
				db->fi->r[i   ]=gg;     /* store rank of this field */
			}
		}else{
			db->fi->r[i]=0;             /* reset rank to zero */
		}
	}
	/*                                                  */
	/* reset genotype field ordering based on a map, if */
	/* a map has been loaded:                           */
	/*                                                  */
	if(c->mp){
		/*                                      */
		/* try ordering according to map order: */
		/*                                      */
		
		/*                                                          */
		/* reset cc,pp,gg, then traverse map and set field indices: */
		/*                                                          */
		cc=pp=gg=0;
		TraverseMarkerMapByPosition3(c->mp,c->db,&cc,&pp,&gg,SetFieldIndex);

		/*                                                                    */
		/* Warn user if the number of markers in the map table is fewer than  */
		/* the number of markers in the pedigree table:  This is not an error */
		/* per se, because the user might toggle some of the markers off:     */
		/*                                                                    */
		if(gg<db->n.go){
			/*                                                 */
			/* reset the order to straight numerical ordering: */
			/*                                                 */
			for(gg=i=0;i<db->fc;i++){
				/*                                      */
				/* only assign orders to output fields: */
				/*                                      */
		        if(db->ff[i]&OUTPUTSET && db->ff[i]&GENOTYPESET){
					db->fi->g[gg++]=i;      /* store actual field index */
					db->fi->r[i   ]=gg;     /* store rank of this field */
				}
			}		
			eprintf(WARNINGFLAG,"AutoSetFieldOrdering() WARNING: \nCould not order genotype fields according to map: \nOne or more markers toggled for output could not be located in the map table.\n");

		}else{

			/*                                             */
			/* It's nice to tell the user what's happened: */
			/*                                             */
			lprintf("Genotype fields now ordered according to current map\n");
		}
		
	}	
}


/*                                          */
/* db: pointer to the database              */
/* nm: field name                           */
/* r : current ordinal (rank) for the field */
/*                                          */
void SetFieldIndex(PDBF db,char *nm,int *cc,int *pp,int *gg){

	int i;
	
	i=db->FieldNumber(db,nm);
	if(i==MISSING) eprintf(WARNINGFLAG,"SetFieldIndex(): Field \"%s\" not found in database.\n",nm);
	if(i!=MISSING && db->ff[i]&OUTPUTSET){
		if(db->ff[i]&KNOWNSET){
			db->fi->c[*cc]=i;
			db->fi->r[i]=++(*cc);
		}else if(db->ff[i]&PHENOTYPESET){
			db->fi->p[*pp]=i;
			db->fi->r[i]=++(*pp);
		}else if(db->ff[i]&GENOTYPESET){
			db->fi->g[*gg]=i;
			db->fi->r[i]=++(*gg);
		}
	}
}


/*                                                                             */
/* TraverseMarkerMapByPosition3(): Traverse a MARKERMAP BST, as in             */
/*                       TraverseMarkerMap() function, but apply               */
/*                       the visit function only if the chromosome             */
/*                       matches chr.  Traversal is by chromosome and position */
/*                                                                             */
void TraverseMarkerMapByPosition3(PMARKERMAP p,PDBF db,int *cc,int *pp,int *gg,void (*visit)(PDBF,char *,int *,int *,int *)){

	if(!p) return;
	TraverseMarkerMapByPosition3(p->ll,db,cc,pp,gg,visit);
	visit(db,p->nm,cc,pp,gg);
	TraverseMarkerMapByPosition3(p->rr,db,cc,pp,gg,visit);
	
}



/*                                                           */
/*                                                           */
/* UserSetFieldOrdering(): SET FIELD ORDER TO A,B,C,D-F      */
/*                                                           */
/* -> This function will be called from the SET/TURN command */
/*    after "SET FIELD" has been entered, so the next token  */
/*    expected is "ORDER", then "TO":                        */
/*                                                           */
void UserSetFieldOrdering(PCONTROL c,PDBF db){

	int i;
	int stt;
	int end;
	int cc;
	int gg;
	int pp;
	int CoreReordered;
	
	PTOKENINFO ptt[4];
	char *changed;     /* boolean vector showing which fields were changed by the user */
                
	/*                                                                             */
	/* the "FIELD" keyword will have already been seen by Turn(), which will have: */
	/* called UserSetFieldOrdering(), so the next keyword must be "ORDER":         */
	/*                                                                             */
	GetToken();
        if(pT->i!=ORDERK)
		MadelineErrorReport("UserSetFieldOrdering(): \"ORDER\" keyword expected:\n\tSET FIELD ORDER TO <F1>,<F2>,<F3>,...");
	GetToken();
	/*                                                           */
	/* set field order automatically, or manually accd. to user: */
	/*                                                           */
	if(pT->i==AUTOMK){
		/*                                */
		/* set field order automatically: */
		/*                                */
		AutoSetFieldOrdering(c,db);
		return;		
	}else if(pT->i!=TOC)
		MadelineErrorReport("UserSetFieldOrdering(): \"TO\" or \"AUTOMATICALLY\" keyword expected:\n\tSET FIELD ORDER [AUTOMATICALLY|TO <F1>[,<F2>[,<F3>,...]]]");
	
	/*                                           */
	/* allocate changed vector and set to zero:  */
	/* Also set cc,gg and pp equal to zero here: */
	/*                                           */
	changed=(char *)malloc((unsigned)db->fc);
	if(!changed) MadelineErrorReport("UserSetFieldOrdering(): out of memory.");
	for(cc=gg=pp=i=0;i<db->fc;i++) changed[i]=0;

	/*                                                                   */
	/* reset all output fields to zero to start with so that after the   */
	/* subset of fields mentioned by the user has been processed, we can */
	/* still go back and set the remainder __not__ mentioned by the user */
	/* by detecting those output fields set to zero in "r".  We also set */
	/* cc,gg, and pp to zero here:                                       */
	/*                                                                   */
	/* for(cc=gg=pp=i=0;i<db->fc;i++) db->fi->r[i]=db->fi->i[i]=0;       */

	/*                                                        */
	/* Get list of field indices by index number, or by name: */
	/*                                                        */
	do{	
		end=MISSING;
		stt=MISSING;
		i=0;
		/*                                                             */
		/* Get field index numbers or field names separated by commas  */
		/* or possibly consisting of a series, like 21-25. Get no more */
		/* than three tokens needed for a series                       */
		/*                                                             */
		do{
			GetToken();
			ptt[i++]=pT;
		}while(i<4 && pT->i!=END && pT->i!=',');
		i--;
		if(i==1){

			/*                              */
			/* Single field or index number */
			/* followed by END or a comma:  */
			/*                              */
			if(ptt[0]->t==NUMERICVARIABLE || ptt[0]->t==STRINGVARIABLE || ptt[0]->t==DATEVARIABLE){
				/*                  */
				/* field name seen: */
				/*                  */
				end=stt=ptt[0]->f;
			}else if(ptt[0]->t==NUMERICVALUE){      
				/*                             */
				/* literal integer value seen: */
				/*                             */
				end=stt=(int)ptt[0]->v-1;
			}else
				MadelineErrorReport("UserSetFieldOrdering(): field name or field index expected");

		}else if(i==3 && ptt[1]->i=='-'){
			/*                                       */
			/* series specified with dash separator: */
			/*                                       */
			
			/*                        */
			/* first field in series: */
			/*                        */
			if(ptt[0]->t==NUMERICVARIABLE || ptt[0]->t==STRINGVARIABLE){
				/*                  */
				/* field name seen: */
				/*                  */
				stt=ptt[0]->f;
			}else if(ptt[0]->t==NUMERICVALUE){      
				/*                             */
				/* literal integer value seen: */
				/*                             */
				stt=(int)ptt[0]->v-1;
			}else
				MadelineErrorReport("UserSetFieldOrdering(): field name or field index expected");
			
			/*                       */
			/* last field in series: */
			/*                       */
			if(ptt[2]->t==NUMERICVARIABLE || ptt[2]->t==STRINGVARIABLE){
				/*                  */
				/* field name seen: */
				/*                  */
				end=ptt[2]->f;
			}else if(ptt[2]->t==NUMERICVALUE){      
				/*                             */
				/* literal integer value seen: */
				/*                             */
				end=(int)ptt[2]->v-1;
			}else
				MadelineErrorReport("UserSetFieldOrdering(): field name or field index expected");
			/*                          */
			/* error checking on range: */
			/*                          */
			if(end<stt)
				MadelineErrorReport("UserSetFieldOrdering(): specify an increasing range like 3-5,7-9, etc.");
		}else
			MadelineErrorReport("UserSetFieldOrdering(): specify field names or indices separated by commas or dashes");

		/*                  */
		/* set field order: */
		/*                  */
		CoreReordered=0;

		for(i=stt;i<=end;i++){
			/*                                      */
			/* only assign orders to output fields: */
			/*                                      */
	        if(db->ff[i]&OUTPUTSET){
				/*                                                         */
				/* assign orders by field type, known (core), phenotype or */
				/* genotype set:                                           */
				/*                                                         */
				if      (db->ff[i]&KNOWNSET    ){
					CoreReordered++;
					db->fi->c[cc++]=i;      /* store actual field index */
					db->fi->r[i   ]=cc;     /* store rank of this field */
				}else if(db->ff[i]&PHENOTYPESET){
					db->fi->p[pp++]=i;      /* store actual field index */
					db->fi->r[i   ]=pp;     /* store rank of this field */
				}else if(db->ff[i]&GENOTYPESET ){
					db->fi->g[gg++]=i;      /* store actual field index */
					db->fi->r[i   ]=gg;     /* store rank of this field */
				}
				/*                  */
				/* mark as changed: */
				/*                  */
				changed[i]++;
			}
		}
				
	}while(pT->i==',');

	/*                                               */
	/* Note to user about reordering of core fields: */
	/*                                               */
	if(CoreReordered)
		lprintf("NOTE:\tSettings for the core fields you reordered\n\tmay be selectively ignored by Madeline.\n");
	
	/*                                                                 */
	/* set order of any remaining output fields __not__                */
	/* mentioned explicitly by the user: these get subsequent          */
	/* cc,pp, or gg ordinals.  But __ONLY__ do this for the field      */
	/* sets (C,P, or G) whose order has been modified by the user      */
	/* -- retain the order of the undisturbed sets, if any, unchanged. */
	/* We know which sets were changed -- just see if cc,pp, or gg     */
	/* are greater than zero:                                          */
	/*                                                                 */
	for(i=0;i<db->fc;i++){
		if(db->ff[i]&OUTPUTSET && !changed[i]){
			/*                                                          */
			/* Assign orders by field type, known (core), phenotype or  */
			/* genotype set.  But, __ONLY__ do this if some other       */
			/* fields in the same set were changed, i.e., if cc, pp, or */
			/* gg are already greater than zero:                        */
			/*                                                          */
			if      (cc && db->ff[i]&KNOWNSET    ){
				db->fi->c[cc++]=i;      /* store actual field index */
				db->fi->r[i   ]=cc;     /* store rank of this field */
			}else if(pp && db->ff[i]&PHENOTYPESET){
				db->fi->p[pp++]=i;      /* store actual field index */
				db->fi->r[i   ]=pp;     /* store rank of this field */
			}else if(gg && db->ff[i]&GENOTYPESET ){
				db->fi->g[gg++]=i;      /* store actual field index */
				db->fi->r[i   ]=gg;     /* store rank of this field */
			}		
		}
	}
	
	/*                                                            */
	/* any set field order command must invalidate the graphics   */
	/* report set up for drawing pedigrees.  The drawing routines */
	/* will automatically set up a new report if there is none:   */
	/*                                                            */
	if(gDrawing){
		FreeDrawing(gDrawing);
		gDrawing=NULL;	
	}
	
	/*                           */
	/* release "changed" vector: */
	/*                           */
	free(changed);
}



/*                                                   */
/* ViewIndividualRecord(): show one complete record: */
/*                                                   */
void ViewIndividualRecord(PDBF db){

	int    i;  /* loop var.                                                   */
	int    j;  /* number of characters printed (for conditional line returns) */

	if(db->cr==NO_RECORD_EXISTS){
		dprintf("-- not in pedigree table --\n");
		return;
	}
	/*                          */
	/* get here if in database: */
	/*                          */
	
	/*              */
	/* CORE FIELDS: */
	/*              */
	dprintf("CORE FIELDS:\n");
	for(j=i=0;i<db->n.co;i++){
		PrintViewRecordField(db,db->fi->c[i],&j);
	}

	/*                   */
	/* PHENOTYPE FIELDS: */
	/*                   */
	if(db->n.po){
		dprintf("\nPHENOTYPE FIELDS:\n");
		for(j=i=0;i<db->n.po;i++){
			PrintViewRecordField(db,db->fi->p[i],&j);
		}
	}

	/*                  */
	/* GENOTYPE FIELDS: */
	/*                  */
	if(db->n.go){
		dprintf("\nGENOTYPE FIELDS:\n");
		for(j=i=0;i<db->n.go;i++){
			PrintViewRecordField(db,db->fi->g[i],&j);
		}
	}
	if(j>0) lprintf("\n");	
}


/*                        */
/* PrintViewRecordField() */
/*                        */
/*                        */
#define PRINTBRIEFDATEWIDTH 10
void PrintViewRecordField(PDBF db,int i,int *j){

	int   ty;  /* field type                                                       */
	char  *o;  /* for string field values                                          */
	double v;  /* for numeric field values                                         */
	int    w;  /* width                                                            */
	int    p;  /* precision                                                        */
	long   d;  /* date field values                                                */
	char missing[]="........................"; /* what to print for missing fields */
	
	ty=db->FieldType(db,i);
	if(ty=='D') w=PRINTBRIEFDATEWIDTH;
	else        w=db->FieldLength(db,i);
	/*                                                          */
	/* put line breaks in nice places:                          */
	/*                                                          */
	(*j)+=w; /* add the width of the next field to print        */
	(*j)++;  /* add the extra space after each field is printed */
	
	/*                                                   */
	/* LINEWIDTH-1 leaves room for that carriage return: */
	/*                                                   */
	if(*j>LINEWIDTH-1){
		*j=w;
		dprintf("\n");
	}

	switch(ty){
	case 'C':
		/*                             */
		/* read character string data: */
		/*                             */
		o=db->ReadString(db,NULL,i,db->cr);
		/*                                            */
		/* check for missing values:                  */
		/*                                            */
		/*if(IsCMV(o))                                */
		/*                                            */
		/*if(!strcmp(o,CHARMISSINGSTRING)) o=missing; */
		
		if(!o[0]) o=missing;
		
		/*                       */
		/* write to output file: */
		/*                       */
		dprintf("%-*.*s ",w,w,o);
		break;
		
	case 'N':
		/*                    */
		/* read numeric data: */
		/*                    */
		v=db->ReadDouble(db,i,db->cr);
		/*                           */
		/* check for missing values: */
		/*                           */
		/*if(IsNMV(v)){              */
		/*                           */
		if(v==DOUBLEMISSINGVALUE){
			dprintf("%-*.*s ",w,w,missing);
		}else{
			/*                */
			/* get precision: */
			/*                */
			p=db->FieldPrecision(db,i);					
			dprintf("%s ",FormatNumber(v,NULL,w,p));
		}
		break;
		
	case 'D':
			d=db->ReadDate(db,i,db->cr);
			if(d==MISSINGDATE){
				dprintf("{%-*.*s} ",w,w,missing);
			}else{
				dprintf("{%-*.*s} ",w,w,DelimitedGenericDateString(d,'.'));
			}
		break;
		
	default:
		MadelineErrorReport("Field type \"%c\" not supported (%s)",ty,db->FieldName(db,i));
		break;
		
	}
}


/*                    */
/* UserSetLanguage(): */
/*                    */
void UserSetLanguage(PCONTROL c){

	
                
	/*                                                               */
	/* the "LANGUAGE" keyword will have already been seen by Turn(), */
	/* so the next keyword must be "TO"                              */
	/*                                                               */
	GetToken();
	if(pT->i!=TOC)
		MadelineErrorReport("UserSetLanguage(): \"TO\" keyword expected:\n\tSET LANGUAGE TO <LANGUAGE_OR_LOCALE_NAME>");
	GetToken();
	switch(pT->i){
	case LAMER:
		c->lang=AMERICAN_ENGLISH;
		break;
	case LBRIT:
		c->lang=BRITISH_ENGLISH;
		break;
	case LARAB:
		c->lang=ARABIC;
		break;
	case LCHIN:
		c->lang=CHINESE;
		break;
	case LFREN:
		c->lang=FRENCH;
		break;
	case LGERM:
		c->lang=GERMAN;
		break;
	case LGREK:
		c->lang=GREEK;
		break;
	case LJAPN:
		c->lang=JAPANESE;
		break;
	case LITAL:
		c->lang=ITALIAN;
		break;
	case LPORT:
		c->lang=PORTUGESE;
		break;
	case LRUSS:
		c->lang=RUSSIAN;
		break;
	case LSUOM:
		c->lang=FINNISH;
		break;
	case LSPAN:
		c->lang=SPANISH;
		break;
	case LTHAI:
		c->lang=THAI;
		break;
	default:
		MadelineErrorReport("UserSetLanguage(): unexpected language keyword");
		break;
	}
	
	return;
}


/*                                                              */
/* Sort(): set the global sort expression for commands, such as */
/* Draw(), that recognize different sort orders                 */
/*                                                              */
void Sort(void){

	extern PCONTROL g;
	RESULT result;
	
	GetToken();

	if(!g->db)
		MadelineErrorReport("Sort(): No pedigree table is currently open");

	if(!(pT->i==ONK || pT->i==OFFK)){
		MadelineErrorReport("Sort(): SORT OFF|ON <expr>");
	}
		
	if(pT->i==OFFK){
		g->sort=FALSE;
		return;
	}
	
	/*                                     */
	/* get here if SORT ON <expr> command: */
	/*                                     */
	

	/*                                                            */
	/* set globals to avoid error when evaluating the expression: */
	/*                                                            */
	g->cpd=g->pd;
	g->cp =g->pd->h;
			
	/*                                        */
	/* Go to the first record of the database */
	/* -- this record is as good as any other */
	/*    for the first evaluation of the     */
	/*    expression:                         */
	/*                                        */
	GoTo(g->db,0);

	/*                                                           */
	/* EvaluateExpression() starts with a GetToken()             */
	/* after clearing the token stack:                           */
	/*                                                           */
	/* The expression is only evaluated to                       */
	/* make sure the expression is error-free. Note that         */
	/* it is evaluated on the reserve token stack where the      */
	/* parsed version is then stored for use by other functions: */
	/*                                                           */
	EvaluateExpressionOnStack2(&result);

	/*                                                                      */
	/* Check for ascending/descending keyword present after the expression: */
	/*                                                                      */
		
	switch(pT->i){
	case SDESK:
		g->sort=SDESK; /* descending */
		break;
	case SASCK:
	case END:
		g->sort=SASCK; /* ascending */
		break;
	default:
		MadelineErrorReport("Sort(): ASCENDING or DESCENDING keyword expected");
	}
}	


/*                                                                                */
/* CompareExpression(): Evaluates the current expression                          */
/* on the RESERVE token stack (Token2) for two individuals and returns the result */
/* of the comparison                                                              */
/*                                                                                */
int CompareExpression(void *aa,void *bb,int ascend){

	extern PCONTROL g;

	PINDIVIDUAL a;
	PINDIVIDUAL b;
	
	RESULT ResultA;
	RESULT ResultB;
	
	a=(PINDIVIDUAL) aa;
	b=(PINDIVIDUAL) bb;
		
	/*                                       */
	/* Evaluate the expression for PERSON A: */
	/*                                       */
	
	/*                                                           */
	/* Person's pedigree is presumably already stored in c->cpd: */
	/* store person a:                                           */
	/*                                                           */
	/* g->cpd=pd;                                                */
	/*                                                           */
	g->cp=a;
	/*                               */
	/* Handle dummied in individual: */
	/*                               */
	if(!a->db){
		/*                      */
		/* no record available: */
		/*                      */
		g->db->cr=NO_RECORD_EXISTS;
	}else{
		/*                                 */
		/* Go to this individual's record: */
		/*                                 */
		GoTo(a->db,a->rci);
	}
	/*                                                 */
	/* evaluate the expression already on              */
	/* the reserve token stack (RepeatInProgress=TRUE) */
	/*                                                 */
	EvaluateExpressionOnStack2(&ResultA);

	/*                                     */
	/* Now do the same thing for PERSON B: */
	/*                                     */
	g->cp=b;
	/*                               */
	/* Handle dummied in individual: */
	/*                               */
	if(!b->db){
		/*                      */
		/* no record available: */
		/*                      */
		g->db->cr=NO_RECORD_EXISTS;
	}else{
		/*                                 */
		/* Go to this individual's record: */
		/*                                 */
		GoTo(b->db,b->rci);
	}
	/*                                                    */
	/* evaluate the expression already on                 */
	/* the token stack (assuming that RepeatInProgress is */
	/* set to TRUE, which it should be):                  */
	/*                                                    */
	EvaluateExpressionOnStack2(&ResultB);
	
	/*                               */
	/* Finally, make the comparison: */
	/*                               */
	switch(ResultA.t){
	case STRINGVALUE:
		if(ascend)
			return strcmp(ResultB.s,ResultA.s);
		else
			return strcmp(ResultA.s,ResultB.s);
	
	case NUMERICVALUE:	
	case DATEVALUE:

		return ascend?(ResultB.v>ResultA.v):(ResultA.v>ResultB.v);

	}
	return 0;
}




/*                          */
/*                          */
/* FreeInternalVariables(): */
/*                          */
/*                          */
void FreeInternalVariables(void){
	
	
	extern PARRAY pCmv;
	extern PARRAY pNmv;
	
	/* int i; */
	
	free(gfnID.e.s);
	free(gfnFather.e.s);
	free(gfnMother.e.s);
	free(gfnSex.e.s);
	free(gfnMZTwin.e.s);
	free(gfnDZTwin.e.s);
	free(gfnFamID.e.s);
	free(gfnAffected.e.s);
	free(gfnProband.e.s);
	free(gfnClass.e.s);
	free(gfnLClass.e.s);
	free(gfnDead.e.s);
	free(gfnDOB.e.s);
	free(gfnDOD.e.s);

	free(gfnMapChromosome.e.s);
	free(gfnMapPosition.e.s);
	free(gfnMapOrdinal.e.s);
	free(gfnMapMarker.e.s);


	free(gfnAllele1.e.s);
	free(gfnAllele2.e.s);
	

	free(gDatabaseFile.e.s);
	free(gDetailFile.e.s);
	free(gLogFile.e.s);
	free(gOutputFile.e.s);
	free(gParameterOutputFile.e.s);
	free(gMapDatabase.e.s);
	free(gErrorFile.e.s);

	free(gPrintCommand.e.s);
	free(gPedigreeDrawing.e.s);
	free(gEditor.e.s);
	free(gPSViewerName.e.s);

	free(gWebViewerName.e.s);
	free(gWebAddress.e.s);
	
	FreeArray(pCmv );
	FreeArray(pNmv );
	FreeArray(pCsv );
	FreeArray(pNsv );
	FreeArray(pCaff);
	FreeArray(pNaff);
	FreeArray(pCds );
	FreeArray(pNds );
	
}






/*                   */
/* FreeTokenStack(): */
/*                   */
PTOKENINFO FreeTokenStack(PTOKENINFO tstack,int *stacksize){

	free(tstack);
	*stacksize=0;
	return tstack=NULL;
}

/*                                                                                */
/* ViewDistinct(): called from View() command after the VIEW and DISTINCT         */
/* tokens have been seen, this function processes the VIEW DISTINCT VALUES OF ... */
/* command:                                                                       */
/*                                                                                */
/* 19990327 (SAT) ET ADDENDUM:                                                    */
/* 20020301 (FRI) ET ADDENDUM: Added Percentages                                  */
/*                                                                                */
void ViewDistinct(PCONTROL c){

	int i,stt,end;
	PTOKENINFO ptt[4];
	int fw,fp; /* field width and precision */
	UINT32 nonmissing;
	double percent;

	/*//////////////////////// */
	/*                         */
	/* Get remaining keywords: */
	/*                         */
	/*//////////////////////// */
	/*                         */
	/* Get VALUES keyword:     */
	/*                         */
	GetToken();
	if(pT->i!=VALUK)
		MadelineErrorReport("ViewDistinct(): VALUES keyword expected: VIEW DISTINCT VALUES OF ...");
	/*                 */
	/* Get OF keyword: */
	/*                 */
	GetToken();
	if(pT->i!=OF__K)
		MadelineErrorReport("ViewDistinct(): OF keyword expected: VIEW DISTINCT VALUES OF ...");


	/*//////////////////////////////////////////////////////// */
	/*                                                         */
	/* Process field list in same manner as in TOGGLE command: */
	/*                                                         */
	/*//////////////////////////////////////////////////////// */

	do{
	
		end=MISSING;
		stt=MISSING;
		/*                                                             */
		/* Get field index numbers or field names separated by commas  */
		/* or possibly consisting of a series, like 21-25. Get no more */
		/* than three tokens needed for a series:                      */
		/*                                                             */
		i=0;
		do{
			GetToken();
			ptt[i++]=pT;
		}while(i<4 && pT->i!=END && pT->i!=',');
		i--;
		if(i==1){

			/*                              */
			/* Single field or index number */
			/* followed by END or a comma:  */
			/*                              */
			if(ptt[0]->t==NUMERICVARIABLE || 
			   ptt[0]->t==STRINGVARIABLE  ||
			   ptt[0]->t==DATEVARIABLE
			){
				/*                             */
				/* Numeric, character, or date */
				/* field name seen:            */
				/*                             */
				end=stt=ptt[0]->f;
			}else if(ptt[0]->t==NUMERICVALUE){      
				/*                             */
				/* literal integer value seen: */
				/*                             */
				end=stt=(int)ptt[0]->v-1;
			}else
				MadelineErrorReport("ViewDistinct(): field name or field index expected");

		}else if(i==3 && ptt[1]->i=='-'){
			/*                                       */
			/* series specified with dash separator: */
			/*                                       */
			
			/*                        */
			/* first field in series: */
			/*                        */
			if(ptt[0]->t==NUMERICVARIABLE || ptt[0]->t==STRINGVARIABLE || ptt[0]->t==DATEVARIABLE){
				/*                  */
				/* field name seen: */
				/*                  */
				stt=ptt[0]->f;
			}else if(ptt[0]->t==NUMERICVALUE){      
				/*                             */
				/* literal integer value seen: */
				/*                             */
				stt=(int)ptt[0]->v-1;
			}else
				MadelineErrorReport("ViewDistinct(): field name or field index expected");
			
			/*                       */
			/* last field in series: */
			/*                       */
			if(ptt[2]->t==NUMERICVARIABLE || ptt[2]->t==STRINGVARIABLE || ptt[2]->t==DATEVARIABLE){
				/*                  */
				/* field name seen: */
				/*                  */
				end=ptt[2]->f;
			}else if(ptt[2]->t==NUMERICVALUE){      
				/*                             */
				/* literal integer value seen: */
				/*                             */
				end=(int)ptt[2]->v-1;
			}else
				MadelineErrorReport("ViewDistinct(): field name or field index expected");
			/*                          */
			/* error checking on range: */
			/*                          */
			if(end<stt)
				MadelineErrorReport("ViewDistinct(): specify an increasing range like 3-5,7-9, etc.");
		}else
			MadelineErrorReport("ViewDistinct(): specify field names or indices separated by commas or dashes");

		/*////////////////////////////////// */
		/*                                   */
		/* Process the field or field range: */
		/*                                   */
		/*////////////////////////////////// */
		for(i=stt;i<=end;i++){
			/*                                                          */
			/* Here we build a field tree and attach it to db->ftree[i] */
			/* (even though the ICONSET flag is not set):               */
			/*                                                          */
			if(!c->db->ftree[i]){
				/*                            */
				/* build tree for this field: */
				/*                            */
				BuildFieldTree(c->db,i);
			}
			/*                                          */
			/* Get here if tree has just been built, or */
			/* already exists: display results:         */
			/*                                          */
			
			/*              */
			/* field width: */
			/*              */
			if(c->db->FieldType(c->db,i)=='D')
				fw=10; /* print date using PrintBriefDate() with width of 10 */
			else
				fw=c->db->FieldLength(c->db,i);
			/*                  */
			/* field precision: */
			/*                  */
			fp=c->db->FieldPrecision(c->db,i);

			lprintf("\n%i. %s has %i level%s:\n\n",
				i+1,
				c->db->FieldName(c->db,i),
				c->db->levels[i],
				c->db->levels[i]==1?"":"s"
			);
			lprintf("Level  %-*.*s Cases Percent\n------ %-*.*s ----- -------\n",
				fw,
				fw,
				"Value",
				fw,
				fw,
				"-----"
			);
			
			/*                                                */
			/* Call display function:                         */
			/* ShowFieldTreeData() is in ndbf.c:              */
			/* Note that c->lang is also passed to specify    */
			/* the language in which dates are to be printed: */
			/*                                                */
			ShowFieldTreeData(c->db->ftree[i],c->db,fw,fp,c->lang);
			/*                                           */
			/* Finally, display totals                   */
			/* for non-missing and missing value counts: */
			/*                                           */
			lprintf("------ %-*.*s ----- -------\n",fw,fw,"-----");

			nonmissing = c->db->rc - c->db->missingcount[i];
			percent    = 100.0*(double)nonmissing/(double)c->db->rc;
			lprintf("    .. %-*.*s %5i  %5.1f   non-missing value%s in database\n",
				fw,
				fw,
				"....................",
				nonmissing,
				percent,
				nonmissing==1?"":"s"
			);

			percent    = 100.0*(double)c->db->missingcount[i]/(double)c->db->rc;
			lprintf("    .. %-*.*s %5i  %5.1f       missing value%s in database\n",
				fw,
				fw,
				"....................",
				c->db->missingcount[i],
				percent,
				c->db->missingcount[i]==1?"":"s"
			);
		}		
		/*                    */
		/* end of while loop: */
		/*                    */
	}while(pT->i==',');

}




/*                                                                    */
/*                                                                    */
/* Edit(): Edit a file using the edit program specified in FileEditor */
/*                                                                    */
/*                                                                    */
void Edit(void){

	RESULT result;
	
	char *commandString;
	
	/* extern PCONTROL g; */
	
	
	/*                                                */
	/* Get the file name for writing:                 */
	/* EvaluateExpression starts with a GetToken():   */
	/*                                                */
	EvaluateExpression(&result);  /* get the filename */
	if(result.t!=STRINGVALUE)
		MadelineErrorReport("Edit(): specify a file name to edit");

	/*                                                */
	/* edit the file using the user-specified editor: */
	/*                                                */
	commandString=Concatenate(" ",gEditor.e.s,result.s,"");
	if(system(commandString)){
		
		MadelineErrorReport("Edit(): The operating system could not execute the command.");
		
	}
	
}

/*                                                                                                   */
/* Compose():                                                                                        */
/*                                                                                                   */
/* NOTE BENE: This function was previously called Transpose() and attached to the command TRANSPOSE. */
/*            But now we have a new command called DECOMPOSE which is clearly the complement of      */
/*            COMPOSE.  And the action of this function is indeed quite analogous to the idea of     */
/*            *composing* type for printing, while the action of the function DECOMPOSE is quite     */
/*            like the decomposition of type.  Hence the name change.                                */
/*                                                                                                   */
void Compose(void){

	RESULT result;
	RESULT result2;
	/* int n; */
	
	PDBF sdb;

	//char *OutputFileName;

	//
	// std::string is now used here to avoid memory management and other problems
	// caused by the fact that ModifyFileName returns a pointer to a static buffer
	// which can get overwritten by the call to SmartTableSelect():
	//
	std::string OutputFileName;
	std::string fileToOpen;
	
	/* extern PCONTROL g; */
	extern VARIABLE gfnFamID;
	extern VARIABLE gfnID;
	extern VARIABLE gfnMapMarker;
	extern VARIABLE gfnAllele1;
	extern VARIABLE gfnAllele2;


	/*                                       */
	/* Get the name of the file for reading: */
	/*                                       */
	EvaluateExpression(&result);
	if(result.t!=STRINGVALUE) MadelineErrorReport("Compose(): Name of file to read expected");
		
	/* No need to get another token after EvaluateExpression(): see if */
	/* keyword "TO" is also present:                                   */
	/*                                                                 */
	if(pT->i==TOC){
		/*                                                                */
		/* If TO is also present, get the optional file name for writing: */
		/*                                                                */
		EvaluateExpression(&result2);  /* get the filename                */
		if(result2.t!=STRINGVALUE)
			MadelineErrorReport("Compose(): name of file to write expected after TO");
		
		OutputFileName=result2.s;		
	}else{
		OutputFileName=ModifyFileName(result.s,COMPOSED_FILE_EXTENSION);
	}

	/* Use SmartTableSelect() to get the .mfh file: */
	fileToOpen=SmartTableSelect(result.s);

	/*                                     */
	/* Make sure file can be opened first: */
	/*                                     */
	//if(!File(result.s))
	//	MadelineErrorReport("Compose(): can't open \"%s\" for reading.\nPerhaps the path or file name is incorrect.",result.s);

	/*                    */
	/* OpenDatabase(sdb); */
	/*                    */
	sdb=NewDatabase(fileToOpen.c_str(),READONLY,CharacterizeDatabase(fileToOpen.c_str()));
	sdb->OpenDatabase(sdb);
	/*                    */
	/* set field indices: */
	/*                    */
	gfnFamID.i    =sdb->FieldNumber(sdb,gfnFamID.e.s);
	gfnID.i       =sdb->FieldNumber(sdb,gfnID.e.s);
	gfnMapMarker.i=sdb->FieldNumber(sdb,gfnMapMarker.e.s);
	gfnAllele1.i  =sdb->FieldNumber(sdb,gfnAllele1.e.s);
	gfnAllele2.i  =sdb->FieldNumber(sdb,gfnAllele2.e.s);
	/*                                                   */
	/* Verify that all five required fields are present: */
	/*                                                   */
	if(gfnID.i==MISSING){
		CloseDatabase(sdb);
		MadelineErrorReport("Compose(): Individual ID field \"%s\" not found in %s",gfnID.e.s,result.s);
	}
	if(gfnFamID.i==MISSING){
		CloseDatabase(sdb);
		MadelineErrorReport("Compose(): Family ID field \"%s\" not found in %s",gfnFamID.e.s,result.s);
	}
	if(gfnMapMarker.i==MISSING){
		CloseDatabase(sdb);
		MadelineErrorReport("Compose(): Marker name field \"%s\" not found in %s",gfnMapMarker.e.s,result.s);
	}
	if(gfnAllele1.i==MISSING){
		CloseDatabase(sdb);
		MadelineErrorReport("Compose(): Allele 1 field \"%s\" not found in %s",gfnAllele1.e.s,result.s);
	}
	if(gfnAllele2.i==MISSING){
		CloseDatabase(sdb);
		MadelineErrorReport("Compose(): Allele 2 field \"%s\" not found in %s",gfnAllele2.e.s,result.s);
	}

	/*            */
	/* Tell user: */
	/*            */
	lprintf("Composing \"%s\" to \"%s\" ...\n",result.s,OutputFileName.c_str());
	/*                                                     */
	/* Call function to transpose database from ndbf.c:    */
	/* TransposeMarkerDatabase() takes care of closing sdb */
	/* regardless of success or failure:                   */
	/*                                                     */
	if(TransposeMarkerDatabase(
		sdb,
		gfnFamID.i,
		gfnID.i,
		gfnMapMarker.i,
		gfnAllele1.i,
		gfnAllele2.i,
		OutputFileName.c_str(),
		MBASE
	)){
		lprintf("Composed file has been created\n");
		lprintf("(Remember to specify the Madeline \"%s\" files when merging composed\ntables with family structure or other tables)\n",HEADER_FILE_EXTENSION);
	}else{
		lprintf("No composed file was created\n");	
	}

}


/*                                                         */
/* Merge(): Merge n tables into one: see details in ndbf.c */
/*                                                         */
#define MAXMERGETABLECOUNT 50
void Merge(void){

	RESULT result[MAXMERGETABLECOUNT];
	RESULT result2;
	
	char *OutputFileName;
	PPDBF SourceTable;
	int i;
	int TableCount;
	clock_t stt,end;
	int MergeOption=MERGEALPHA;
	char *FieldList=NULL;

	/* extern PCONTROL g; */
	extern VARIABLE gfnFamID;
	extern VARIABLE gfnID;

	/*                                     */
	/* get file names separated by commas: */
	/*                                     */
	i=0;
	do{
		/*                                */
		/* Get the name of files to read: */
		/*                                */
		EvaluateExpression(&result[i]);
		if(result[i].t!=STRINGVALUE) MadelineErrorReport("Merge(): Name of a file to merge expected");
		i++;
	}while(pT->i==',' && i<MAXMERGETABLECOUNT);
	/*                                                                         */
	/* Check for rare event: comma would occur if exceeded MAXMERGETABLECOUNT: */
	/*                                                                         */
	if(pT->i==',')
		MadelineErrorReport("Merge(): Attempt to process more than %i tables at once",MAXMERGETABLECOUNT);
	/*                   */
	/* Number of tables: */
	/*                   */
	TableCount=i;
		
	/*		                                                               */
	/* No need to get another token after EvaluateExpression(): see if */
	/* keyword "TO" is also present:                                   */
	/*                                                                 */
	if(pT->i==TOC){
		/*                                                                */
		/* If TO is also present, get the optional file name for writing: */
		/*                                                                */
		EvaluateExpression(&result2);  /* get the filename                */
		if(result2.t!=STRINGVALUE)
			MadelineErrorReport("Merge(): name of file to write expected after TO");

		OutputFileName=result2.s;
	}else{
		OutputFileName=ModifyFileName(result[0].s,".mrg");
	}

	/*                   */
	/* Get merge option: */
	/*                   */
	if(pT->i!=INC) GetToken();

	if(pT->i==INC){
		GetToken();
		switch(pT->i){
		case ALPHAK:
			lprintf("Alphabetic order specified for merging fields ...\n");
			MergeOption=MERGEALPHA;
			break;
		case PHYSIK:
			lprintf("Physical order specified for merging fields ...\n");
			MergeOption=MERGEPHYSICAL;
			break;
		case NOINTERNALTOKEN:
			/*                                                           */
			/* a file name is expected for the user-defined field order: */
			/*                                                           */
			if(pT->t!=STRINGVALUE)
				MadelineErrorReport("Merge(): keyword ALPHA or PHYSICAL or else a file name is expected: ... IN [ALPHA|PHYSICAL|<FileName>] ORDER");
			lprintf("User defined order specified for merging fields ...\n");
			/*                               */
			/* make sure file can be opened: */
			/*                               */
			if(!File(pT->e))
				MadelineErrorReport("Merge(): can't open \"%s\" for reading.\nPerhaps the path or file name is incorrect.",pT->e);
			/*                                      */
			/* Place field list into string buffer: */
			/*                                      */
			FieldList=OpenAndBufferFile(pT->e);
			MergeOption=MERGEUSER;						
			break;
		default:
			MadelineErrorReport("Merge(): keyword ALPHA or PHYSICAL or else a file name is expected: ... IN [ALPHA|PHYSICAL|<FileName>] ORDER");
			break;
		}
		GetToken();
		if(pT->i!=ORDERK)
			MadelineErrorReport("Merge(): keyword ORDER expected: ... IN [ALPHA|PHYSICAL] ORDER");
	
	}else{
		/*                         */
		/* default to alpha order: */
		/*                         */
		lprintf("Defaulting to alphabetic order for merging fields ...\n");
		MergeOption=MERGEALPHA;	
	}
	

	/*                                          */
	/* Make sure all input files can be opened: */
	/*                                          */
	for(i=0;i<TableCount;i++){
		if(!File(result[i].s))
		MadelineErrorReport("Merge(): can't open \"%s\" for reading.\nPerhaps the path or file name is incorrect.",result[i].s);
	}
	/*                 */
	/* Open databases: */
	/*                 */
	SourceTable=(PPDBF)malloc((unsigned)TableCount*sizeof(PDBF));
	if(!SourceTable) MadelineErrorReport("Merge(): unable to allocate PPDBF SourceTable vector");
	
	for(i=0;i<TableCount;i++){

		SourceTable[i]=NewDatabase(result[i].s,READONLY,CharacterizeDatabase(result[i].s));
		SourceTable[i]->OpenDatabase(SourceTable[i]);
		/*                                          */
		/* Set known field flags:                   */
		/*                                          */
		/* SetKnownFieldFieldFlags(SourceTable[i]); */
		/* SetFieldFlags(SourceTable[i]);           */
		/* CountFieldFlags(SourceTable[i]);         */
		/*                                          */
		
	}

	/*                                                   */
	/* Call MergeDatabases() from ndbf.c to do the work: */
	/*                                                   */
	/* Key1=gfnFamID                                     */
	/* Key2=gfnID                                        */
	/*                                                   */

	lprintf("Merging %i tables to \"%s\" ...\n",
		TableCount,
		OutputFileName
	);
	
	/*                                                                        */
	/* MergeDatabases() takes care of closing databases regardless of success */
	/* or failure:                                                            */
	/*                                                                        */
	stt=clock();
	if(MergeDatabases(SourceTable,TableCount,gfnFamID.e.s,gfnID.e.s,OutputFileName,MBASE,MergeOption,FieldList)){
		end=clock();
		lprintf("%i table%s merged to %s in %8.2lf seconds\n",
			TableCount,
			TableCount==1?"":"s",
			OutputFileName,
			(double)(end-stt)/(double)CLOCKS_PER_SEC
		);
		lprintf("(Remember to use the Madeline \"%s\" file when opening the merged table)\n",HEADER_FILE_EXTENSION);
	}else{
		lprintf("No tables merged\n");
	}
}





/*                                                                   */
/* RemoveHelpTextDelimiters(): Removes the "~" squiggle delimiters   */
/* and replaces them with '\0' string terminators so that the entire */
/* text buffer is divided up into help topic strings:                */
/*                                                                   */
void RemoveHelpTextDelimiters(char *HelpTextBuffer){

	char *ss;

	if(!HelpTextBuffer) return;
	/*                                                        */
	/* Get rid of previously mentioned squiggles in HelpText: */
	/*                                                        */
	for(ss=HelpTextBuffer;*ss;ss++) if(*ss=='~') *ss='\0';
}

/*                                                                  */
/* Debug(): The command DEBUG calls this function which can         */
/*          serve as a wrapper for any debugging function you want. */
/*                                                                  */
void Debug(void){
	
	lprintf("The DEBUG command doesn't do anything in a production release of the program\n");
	lprintf("The Madeline system path is: %s\n",g->MadelineSystem);
	return;
	
}

/* *** &&& *** */

/*                                 */
/* OutputPedCheckLocusFileHeader() */
/*                                 */
void OutputPedCheckLocusFileHeader(PCONTROL c,char *fn){

	int i;

	/*                   */
	/* open file stream: */
	/*                   */
	GenericLocusHeaderProcessor(c,fn);

	/*                    */
	/*                    */
	/* Number of markers: */
	/*                    */
	/* -981104 ET         */
	/*                    */
	
	/*                                       */
	/* Add one to account for disease locus: */
	/*                                       */
	fprintf(c->streams.fp,"%3i 0 0 5\n",c->db->n.go+1);
	fprintf(c->streams.fp,"0 0.0 0.0 0\n");
	/*                                                         */
	/* Add one to db->n.go count to account for disease locus: */
	/*                                                         */
	/* We add one so that the numbering goes from 1 to n where */
	/* n is the number of markers:                             */
	/*                                                         */
	for(i=0;i<c->db->n.go+1;i++){
		fprintf(c->streams.fp,"%3i ",i+1);
	}
	/*                              */
	/* terminating carriage return: */
	/*                              */
	fprintf(c->streams.fp,"\n");

	/*                                                                    */
	/* print out the default normal and disease allele frequencies:       */
	/* this is just made up for the user to edit: "1" is for normals,     */
	/* "2" for disease.  Who knows if this is really needed for PEDCHECK? */
	/* I don't, but seems to work fine with it present:                   */
	/*                                                                    */
	fprintf(c->streams.fp,"1 2\n0.99 0.01\n");
	/*                                                                   */
	/* print out the default liability class /penetrance function        */
	/* information: here we just default to one class.  Again, who knows */
	/* if this is really needed for PEDCHECK, but it seems to work fine  */
	/* with it present:                                                  */
	/*                                                                   */
	fprintf(c->streams.fp,"1\n0.050 0.950 0.950\n");
	



	/*                                                                   */
	/* that's it! -- next come the marker information                    */
	/* (Which is identical to the Genehunter format, so we'll call those */
	/* functions).                                                       */
	/*                                                                   */
}


/*                 */
/* PedCheckOutput: */
/*                 */
void PedCheckOutput(PCONTROL c,PPEDIGREE pd){

	/*                                                            */
	/* Check all individuals for data under the current criteria: */
	/*                                                            */
	TraverseIndividualBSTMarkDataFlag(pd->h,GeneralCaseIndividualHasData);

	/*                                                             */
	/* return if sufficient data is not available in the pedigree: */
	/*                                                             */
	/*                                                             */
	if(!GeneralCasePedigreeHasData(c,pd)) return;
	
	/*                                                               */
	/* Output the data: As long as only genotype fields are present, */
	/* the PEDCHECK format is IDENTICAL to the GENEHUNTER format,    */
	/* so we just call OutputGenehunterIndividualRecord():           */
	/*                                                               */
	TraverseIndividualBST4(c,pd,pd->h,OutputGenehunterIndividualRecord);
	
}


/*                                 */
/* PedCheck data header processor: */
/*                                 */
void PedCheckDataHeaderProcessor(PCONTROL c,char *fn){

	/*                                                             */
	/* Affection status field is required for Genehunter routines: */
	/*                                                             */
	if(gfnAffected.i==MISSING)
		MadelineErrorReport("PedCheckDataHeaderProcessor():\naffection status field is required.");

	/*                                  */
	/* Call the "base class" processor: */
	/*                                  */
	GenericDataHeaderProcessor(c,fn);
}



/*                                  */
/* OutputPedCheckLocusFileFooter(): */
/*                                  */
void OutputPedCheckLocusFileFooter(PCONTROL c){

	int i,ii;
	/* int j; */
	PMARKERMAP mp;
	double lpos;    /* inter-marker distances */

	/*                                                 */
	/* Print map information ONLY if a map is present: */
	/*                                                 */
	if(c->mp){
		/*                                                         */
		/* Let user know that map information is being included in */
		/* the locus file:                                         */
		/*                                                         */
		lprintf("A marker map is loaded: inter-marker distance information \nis being included in locus file.\n");
		/*                                                                           */
		/* The "0 0" apparently flags the upcoming intermarker distance information: */
		/*                                                                           */
		fprintf(c->streams.fp,"0 0\n");
		/*                                                   */
		/* Print out inter-marker distances in CentiMorgans: */
		/*                                                   */
		for(lpos=0.0,i=0;i<c->db->n.go;i++){
			/*                      */
			/* Get the field index: */
			/*                      */
			ii=c->db->fi->g[i];
			/*                      */
			/* Lookup map position: */
			/*                      */
			mp=LookupMarkerMap(c->mp,c->db->FieldName(c->db,ii));
			/*                     */
			/* check against NULL: */
			/*                     */
			if(!mp){
				GenericLocusFooterProcessor(c);
				MadelineErrorReport("OutputPedCheckLocusFileFooter():\n\tNo map information available for %s",c->db->FieldName(c->db,ii));
			}
			/*                                         */
			/* print out intermarker distances as soon */
			/* as the second marker map is seen:       */
			/*                                         */
			if(i){
				/*                                                      */
				/* now printing distances with greater precision:       */
				/* This is being done so that markers which map with no */
				/* recombination distance between them can be marked    */
				/* as having a very small distance between them:        */
				/*                                                      */
				/* -- previously had 6.2lf:                             */
				/*                                                      */
				fprintf(c->streams.fp,"%7.3f ",
					mp->ps-lpos);
			}
			/*                                */
			/* save the most recent position: */
			/*                                */
			lpos=mp->ps;
		}
	}		
	/*                                                                          */
	/* note carriage return, then magic numbers:                                */
	/*                                                                          */
	fprintf(c->streams.fp,"\n1 0.1 0.45\n");         /* dummied who knows what? */

	/*                    */
	/* close file stream: */
	/*                    */
	GenericLocusFooterProcessor(c);
}


/*                                                                                 */
/* MarkTrioOffspringHavingGenotypeData()                                           */
/*                                                                                 */
/* This functions marks p->hasgdata if the trio of a                               */
/* non-excluded connected individual and his parents                               */
/* are all in the database and typed for one or more markers.                      */
/*                                                                                 */
/* The value stored in p->hasgdata in this case is the number of markers for which */
/* the individual and his parents are all typed and not all heterozygous           */
/* for the same alleles (phase is ambiguous when all three are heterozygous        */
/* for the same allele so this case does not contribute to the score).             */
/*                                                                                 */
/* p->hasgdata is not marked if the above criteria are not met.                    */
/*                                                                                 */
int MarkTrioOffspringHavingGenotypeData(PINDIVIDUAL p){

	int i,ii;
	/* int j; */
	
	extern PCONTROL g;
	char *o; /* pointer to genotype of individual, father, mother resp. */
	char *allele1,*allele2;
	int nallele1,nallele2;
	int a1[3],a2[3];

	/*                                                                   */
	/* The assumption upon entering this function is that p->hasdata and */
	/* p->hasgdata are already set or reset to 0:                        */
	/*	                                                                  */
	
	/*                                                             */
	/* Person definitely doesn't have data if not in the database: */
	/*                                                             */
	if(!p->db) return 0;
	/*                                                                 */
	/* Person doesn't "have data" if they are supposed to be excluded: */
	/* -> This has to apply to the parents as well: see below          */
	/*                                                                 */
	if(p->excluded) return 0;
	/*                                                             */
	/* Person doesn't "have data" if they're a childless spouse or */
	/* otherwise unnattached who has to be excluded anyway:        */
	/*                                                             */
	if(p->csp || p->unconnected) return 0;
	/*                                                            */
	/* No data if there are no output fields above and beyond the */
	/* core output field set:                                     */
	/*                                                            */
	if(!p->db->n.go) return 0;
	/*                                                                */
	/* No data if missing one or the other parent, or parents are not */
	/* in the database:                                               */
	/*                                                                */
	if(!(p->f && p->f->db && p->m && p->m->db)) return 0;
	/*                            */
	/* check if parents excluded: */
	/*                            */
	if(p->f->excluded || p->m->excluded) return 0;

	/*                                                        */
	/* For LDE2K format, We restrict to trios that have       */
	/* one affected parent and one unaffected parent, without */
	/* regard for the affection status of the offspring:      */
	/*                                                        */

	/*if(g->OutputFormat==LDE2K &&                                */
	/*   !(p->f->affected!=MISSING && p->m->affected!=MISSING &&  */
	/*     p->f->affected ^ p->m->affected )                      */
	/*) return 0;                                                 */
	if(g->OutputFormat==LDE2K && 
		!((p->f->affected==1 || p->m->affected==1) &&
		 p->f->affected != p->m->affected)
	) return 0;

	/*                                                            */
	/* For LDE3K format, We restrict to trios where the offspring */
	/* is affected without regard to the affection status of the  */
	/* parents:                                                   */
	/*                                                            */
	if(g->OutputFormat==LDE3K && p->affected!=1) return 0;

	/*                                              */
	/* Get here if we can look at the trio:         */
	/* Note we are going to keep a running sum of   */
	/* the number of "good" markers in p->hasgdata: */
	/*                                              */
	for(p->hasgdata=i=0;i<p->db->n.go;i++){
		/*                  */
		/* get field index: */
		/*                  */
		ii=p->db->fi->g[i];
		
		/*                                            */
		/* for efficiency, code has been inlined for  */
		/* the three individuals in the trio:         */
		/*                                            */

		/*//////////////////////////////////////////// */
		/*                                             */
		/* read the genotype field for the INDIVIDUAL: */
		/*                                             */
		/*//////////////////////////////////////////// */
		o=p->db->ReadString(p->db,NULL,ii,p->rci);
		/*                                                    */
		/* go on to next marker if this individual not typed: */
		/*                                                    */
		if(!o[0]) continue;
		/*                                     */
		/* get allele1 and allele2 substrings: */
		/*                                     */
		for(allele1=allele2=o;*allele2 && *allele2!='/';allele2++);
		*allele2++='\0';
		/*                              */
		/* Get and store allele values: */
		/*                              */
		nallele1=atoi(allele1);
		nallele2=atoi(allele2);
		/*                                   */
		/* Can't use half-typed individuals: */
		/*                                   */
		if(!(nallele1 && nallele2)) continue;
		if(nallele2>nallele1){
			a1[0]=nallele1;
			a2[0]=nallele2;
		}else{
			a1[0]=nallele2;
			a2[0]=nallele1;
		}
		
		/*//////////////////////////////////////////// */
		/*                                             */
		/* read the genotype field for the FATHER:     */
		/*                                             */
		/*//////////////////////////////////////////// */
		o=p->db->ReadString(p->db,NULL,ii,p->f->rci);
		/*                                                */
		/* go on to next marker if this father not typed: */
		/*                                                */
		if(!o[0]) continue;
		/*                                     */
		/* get allele1 and allele2 substrings: */
		/*                                     */
		for(allele1=allele2=o;*allele2 && *allele2!='/';allele2++);
		*allele2++='\0';
		/*                              */
		/* Get and store allele values: */
		/*                              */
		nallele1=atoi(allele1);
		nallele2=atoi(allele2);
		/*                                   */
		/* Can't use half-typed individuals: */
		/*                                   */
		if(!(nallele1 && nallele2)) continue;
		if(nallele2>nallele1){
			a1[1]=nallele1;
			a2[1]=nallele2;
		}else{
			a1[1]=nallele2;
			a2[1]=nallele1;
		}

		/*//////////////////////////////////////////// */
		/*                                             */
		/* read the genotype field for the MOTHER:     */
		/*                                             */
		/*//////////////////////////////////////////// */
		o=p->db->ReadString(p->db,NULL,ii,p->m->rci);
		/*                                                */
		/* go on to next marker if this MOTHER not typed: */
		/*                                                */
		if(!o[0]) continue;
		/*                                     */
		/* get allele1 and allele2 substrings: */
		/*                                     */
		for(allele1=allele2=o;*allele2 && *allele2!='/';allele2++);
		*allele2++='\0';
		/*                              */
		/* Get and store allele values: */
		/*                              */
		nallele1=atoi(allele1);
		nallele2=atoi(allele2);
		/*                                   */
		/* Can't use half-typed individuals: */
		/*                                   */
		if(!(nallele1 && nallele2)) continue;
		if(nallele2>nallele1){
			a1[2]=nallele1;
			a2[2]=nallele2;
		}else{
			a1[2]=nallele2;
			a2[2]=nallele1;
		}

		/*////////////////////////////////// */
		/*                                   */
		/* Evaluate the trio at this marker: */
		/*                                   */
		/*////////////////////////////////// */
		
		/*                                                   */
		/* if not all identical heterozygotes, then it's OK: */
		/* (we're not checking for non-Mendelians here:      */
		/*                                                   */
		if( ! (a1[0]!=a2[0] 
		   &&  a1[0]==a1[1] && a1[0]==a1[2]
		   &&  a2[0]==a2[1] && a2[0]==a2[2] )) p->hasgdata++;		
	}	

	/*         */
	/* return: */
	/*         */
	return p->hasgdata;
}

/*                */
/* LDEQHOutput(): */
/*                */
void LDEQHOutput(PCONTROL c,PPEDIGREE pd){

	/* int i; */
	PINDIVIDUAL TrioOffspringIndividual;

	/*                                                            */
	/* Because only a trio of individuals is output from each     */
	/* pedigree, the bookkeeping required differs from the normal */
	/* model seen in routines such as MendelOutput()              */
	/* or GenehunterOutput():                                     */
	/*                                                            */
	
	/*                   */
	/* Clear data flags: */
	/*                   */
	TraverseIndividualBSTClearDataFlag(pd->h);

	/*                                                                                            */
	/* Check all individuals for data: TrioHasGenotypeData()                                      */
	/* marks the p->hasgdata "has genotype" data flags with the number of good analyzable markers */
	/* for the trio in which p is the offspring.                                                  */
	/*                                                                                            */
	if(TraverseIndividualBST7(pd->h,MarkTrioOffspringHavingGenotypeData)){
		/*                                                               */
		/* Get here if there are one or more good trios in the pedigree: */
		/* so data are available:                                        */

		/*                                                                */
		/* Find the trio with the most data:  The "1" flag forces a reset */
		/* of the static MaximizedIndividual pointer:                     */
		/*                                                                */
		TrioOffspringIndividual=LDEQHTraverseToFindBestTrio(pd->h,1);
		/*                                                             */
		/* The "has data" flag of the individual and his parents are   */
		/* not yet marked (only the p->hasgdata flag of the individual */
		/* was marked), so we do that now:                             */
		/*                                                             */
		/*                                                             */
		TrioOffspringIndividual->hasdata++;
		TrioOffspringIndividual->f->hasdata++;
		TrioOffspringIndividual->m->hasdata++;

		/*                  */
		/* Output the trio: */
		/*                  */
	    OutputLDEQHIndividualRecord(c,pd,TrioOffspringIndividual->f,1);
	    OutputLDEQHIndividualRecord(c,pd,TrioOffspringIndividual->m,1);
	    OutputLDEQHIndividualRecord(c,pd,TrioOffspringIndividual,0);
	}
	/*                                                   */
	/* We still need to do bookkeeping on this pedigree: */
	/*	                                                  */
	LDEQHPedigreeHasDataBookkeeping(c,pd);

}


/*                                    */
/* LDEQHPedigreeHasDataBookkeeping(): */
/*                                    */
void LDEQHPedigreeHasDataBookkeeping(PCONTROL c,PPEDIGREE pd){

	/*                                                                  */
	/* Recall that LDEQH always puts out just one trio from the family: */
	/* and that the p->hasdata flags have only been set for the single  */
	/* optimal trio selected:                                           */
	/*                                                                  */
	if(TraverseIndividualBSTUntilDataFound(pd->h)){
		/*                             */
		/* increment include counters: */
		/*                             */
		c->counters.pedg.in++;
		TraverseIndividualBST6(c,pd->h,LDEQHCountIncludedPedigreeIndividual);

	}else{	
		/*                             */
		/* no data: excluded pedigree: */
		/*                             */
		dprintf("LDEQHPedigreeHasData(): excluding pedigree %s: lacks a trio with data.\n",pd->fid);
		c->counters.pedg.ex++;
		TraverseIndividualBST2(c,pd->h,GeneralCaseCountExcludedPedigreeIndividual);
	}
}


/*                                                           */
/* LDEQHTraverseToFindBestTrio(): Stores the pointer to the  */
/* offspring individual of the trio that has the most data   */
/* available in the static MaximizedDataIndividual. When all */
/* done, this is the return value:                           */
/*                                                           */
PINDIVIDUAL LDEQHTraverseToFindBestTrio(PINDIVIDUAL p,int reset){

	static PINDIVIDUAL MaximizedDataIndividual=NULL;

	
	/*                                                     */
	/* Since MaximizedDataIndividual is static, we need to */
	/* reset it to NULL whenever the pedigree changes:     */
	/*                                                     */
	if(reset) MaximizedDataIndividual=NULL;

	/*                      */
	/* return if NULL node: */
	/*	                     */
	if(!p) return MaximizedDataIndividual;

	/*                      */
	/* Left node recursion: */
	/*	                     */
	LDEQHTraverseToFindBestTrio(p->l,0);	
	/*               */
	/* Current node: */
	/*               */
	if(p->hasgdata){
		if(MaximizedDataIndividual==NULL){
			/*                                          */
			/* First individual with data, so keep him: */
			/*                                          */
			MaximizedDataIndividual=p;
		}else{
			/*                                                       */
			/* Keep the individual with the most analyzable markers: */
			/*                                                       */
			if(p->hasgdata>MaximizedDataIndividual->hasgdata) MaximizedDataIndividual=p;
		}
	}
	/*                       */
	/* Right node recursion: */
	/*		                     */
	LDEQHTraverseToFindBestTrio(p->r,0);

	/*               */
	/* Return value: */
	/*               */
	return MaximizedDataIndividual;
}



/*                                                                    */
/*                                                                    */
/* OutputLDEQHIndividualRecord(): Output LDEQ individual              */
/* record with spaces:                                                */
/*                                                                    */
/* -> If IsParent is true, then the individual's father and mother    */
/*    fields are written using the MISSING value indicator from pCmv: */
/*                                                                    */
void OutputLDEQHIndividualRecord(PCONTROL c,PPEDIGREE pd,PINDIVIDUAL p,int IsParent){

	int i,ii;
	int width;		
	static char spc[]=" ";
	char *dlm;
	
	extern VARIABLE gfnFather,gfnMother; /* father and mother ID Field name vars. */
	extern PARRAY pCmv;                  /* CharacterMissingValue array           */

	/*                           */
	/* Use spaces as delimiters: */
	/*                           */
	dlm=spc;		

	/*              */
	/* Core Fields: */
	/*              */
	for(i=0;i<c->db->n.co;i++){
		/*                  */
		/* get field index: */
		/*                  */
		ii=c->db->fi->c[i];
		if(IsParent && (ii==gfnFather.i || ii==gfnMother.i)){
			width=p->db->FieldLength(p->db,ii);
			fprintf(c->streams.fp,"%-*.*s%s",width,width,pCmv->e[0].s,dlm);
		}else{
			fprintf(c->streams.fp,"%s%s",OutputGenericField(c,pd,p,ii),dlm);
		}
	}
		
	/*                  */
	/* Genotype fields: */
	/*                  */
	for(i=0;i<c->db->n.go;i++){
		/*                  */
		/* get field index: */
		/*                  */
		ii=c->db->fi->g[i];
		fprintf(c->streams.fp,"%s%s",OutputGenericField(c,pd,p,ii),i<c->db->n.go-1?dlm:"");
	}
	/*                              */
	/* terminating carriage return: */
	/*                              */
	fprintf(c->streams.fp,"\n");	

	/*                            */
	/* Mark individual as output: */
	/*                            */
	p->included++;	
}


/*                                                               */
/* LDEQHCountIncludedPedigreeIndividual()                        */
/*                                                               */
/*                                                               */
/* --> This function returns 1 for each __included__ individual, */
/*     zero for each __excluded__ individual.  Note that book-   */
/*     keeping on individuals is broken down by category in the  */
/*     CONTROL structure.                                        */
/*                                                               */
/* --> This function requires that p->hasdata has been set:      */
/*                                                               */
int LDEQHCountIncludedPedigreeIndividual(PCONTROL c,PINDIVIDUAL p){

	if(p->db){
		/*              */
		/* in database: */
		/*              */
		if(p->unconnected){
			/*                                                         */
			/* In the general case, unconnected individuals are        */
			/* not included in output, regardless of the status of the */
			/* p->excluded flag:                                       */
			/*                                                         */
			c->counters.flt.ex++;
			return 0;
		}else if(p->csp){
			/*                                                         */
			/* In the general case, childless spouses are              */
			/* not included in output, regardless of the status of the */
			/* p->excluded flag:                                       */
			/*                                                         */
			c->counters.csp.ex++;
			return 0;
		}else{
			/*                                                */
			/* normally connected individual in the database: */
			/*                                                */
			if(p->excluded){
				/*                                                */
				/* An excluded person cannot be included for LDEQ */
				/* formats under any circumstances:               */
				/*                                                */
				c->counters.dex.ex++;
				return 0;			
			}else{
				/*                                            */
				/* not an excluded individual: a regular,     */
				/* individual: classify based on whether they */
				/* have data or not:                          */
				/*                                            */
				if(p->hasdata){
					c->counters.din.in++;
					return 1;
				}else{
					/*                                    */
					/* if they dont "have data", they are */
					/* never included for LDEQ formats:   */
					c->counters.dno.ex++;
					return 1;
				}
			}
		}
	}else{
		/*                                                         */
		/* virtual individual, not in database.  These individuals */
		/* are never included for LDEQ formats:                    */
		/*                                                         */
		c->counters.vir.ex++;
		return 0;
	}
}


/*                                */
/* IndividualLeftSubtreeWidth():  */
/*                                */
double IndividualLeftSubtreeWidth(PINDIVIDUAL p){

	int nleft;      /* number of people to loop through */
	int sumleftnw;
	/* double halflxu; */
	int i;
	/* PINDIVIDUAL f; */ /* female individual                */
	PINDIVIDUAL v[MAXKIDS];   /* vector of individuals */
	
	extern PCONTROL g;
		
	
	switch(p->na){
	case 0:
		/*                           */
		/* no spouse and no children */
		/*                           */
		return g->d.HalfHZInterval;

	case 1:
		/*             */
		/* one spouse: */
		/*             */
		
		/*                                                                   */
		/* just take subtree width times half the unit horizontal distance:  */
		/* - subtract another half unit distance to account for shift of the */
		/* originating individual back 1/2 to make room for mate:            */
		/*                                                                   */
		return p->nw*g->d.HalfHZInterval - g->d.HalfHZInterval;

	default:
		/*                       */
		/* more than one spouse: */
		/*                       */
		/*                       */

		/*                                                  */
		/* copy mates to v[] vector and sort, if necessary: */
		/*                                                  */
		for(i=0;i<p->na && i<MAXSPOUSES;i++) v[i]=p->a[i];
		if(g->sort) SortIndividualVector(v,p->na,g);
		
		
		/*                                                   */
		/* number of spouses that will be drawn to the left: */
		/*                                                   */
		nleft=p->na>>1;
		/*                                                           */
		/* get the left-side actual nw width based on these spouses: */
		/*                                                           */
		for(sumleftnw=i=0;i<nleft;i++) sumleftnw+=v[i]->nw;		
		/*                                                          */
		/* add a half to account for the drawing of the originating */
		/* individual dead center on virtual centering line:        */
		/*                                                          */
		return sumleftnw*g->d.HorizontalInterval + g->d.HalfHZInterval;
	}
}


/*                                 */
/* IndividualRightSubtreeWidth():  */
/*                                 */
double IndividualRightSubtreeWidth(PINDIVIDUAL p){

	int nleft;      /* number of people to loop through on left */
	int sumrightnw;
	/* double halflxu; */
	int i;
	/* PINDIVIDUAL f; */
	PINDIVIDUAL v[MAXKIDS];
		
	
	switch(p->na){
	case 0:
		/*                           */
		/* no spouse and no children */
		/*                           */
		return g->d.HalfHZInterval;

	case 1:
		/*                                                        */
		/* one spouse:                                            */
		/*                                                        */
		/*                                                        */
		/* just take subtree width times unit lxu divided by two  */
		/* -- add halflxu to account for offset due to mate pair: */
		/*                                                        */
		return p->nw*g->d.HalfHZInterval + g->d.HalfHZInterval;
		
	default:
		/*                                                   */
		/* more than one spouse:                             */
		/*                                                   */
		/*                                                   */
		/* number of spouses that will be drawn to the left: */
		/*                                                   */
		nleft=p->na>>1;

		/*                                                  */
		/* copy mates to v[] vector and sort, if necessary: */
		/*                                                  */
		for(i=0;i<p->na && i<MAXSPOUSES;i++) v[i]=p->a[i];
		if(g->sort) SortIndividualVector(v,p->na,g);

		/*                                     */
		/* get the right-side actual nw width: */
		/*                                     */
		for(sumrightnw=0,i=nleft;i<p->na;i++) sumrightnw+=v[i]->nw;		
		/*                                                          */
		/* add a half to account for the drawing of the originating */
		/* individual dead center on virtual centering line:        */
		/*                                                          */
		return sumrightnw*g->d.HorizontalInterval + g->d.HalfHZInterval;
	}
}




/*                                                                             */
/* DrawFor(): Draws only pedigrees meeting the criteria of a                   */
/* logical expression which follows: DRAW PEDIGREES FOR <LOGICAL EXPRESSION>.  */
/*                                                                             */
/* This routine is called from Draw() if a FOR clause is present:              */
/* Since it is called from Draw(), the drawing object is already established,  */
/* the print file is already opened, and the Postscript routines have already  */
/* been downloaded.  So, all that's really needed is to call the routines that */
/* draw the pedigrees for the subset for which the expression is true for one  */
/* or more individuals in the pedigree:                                        */
/*                                                                             */
void DrawFor(PCONTROL c){

	RESULT result;

	char *args[3];
	
	/*                                      */
	/* Check for presence of open database: */
	/*                                      */
	if(!g->db){
		fclose(c->streams.prn);
		MadelineErrorReport("DrawFor(): no pedigree table is currently open");
	}
	
	/*                             */
	/* set globals to avoid error: */
	/*                             */
	c->cpd=c->pd;
	c->cp =c->pd->h;
			
	/*                                        */
	/* Go to the first record of the database */
	/* -- this record is as good as any other */
	/*    for the first evaluation of the     */
	/*    expression:                         */
	/*                                        */
	GoTo(c->db,0);

	/*                                                    */
	/* EvaluateExpression() starts with a GetToken()      */
	/* after clearing the token stack:                    */
	/*                                                    */
	/* The first time of evaluating the expression        */
	/* is done to make sure the expression is error-free: */
	/*                                                    */
	EvaluateExpression(&result);

	/*                                              */
	/* The result must be a NUMERICVALUE to qualify */
	/* as a logical true/false result:              */
	/*                                              */
	if(result.t!=NUMERICVALUE){
		MadelineErrorReport("View(): a numeric expression is expected.");
	}
	
	/*                                                  */
	/* Set the expression repeat flag so that           */
	/* GetToken() will simply pull tokens off the token */
	/* stack instead of reparsing the expression:       */
	/*                                                  */
	RepeatInProgress=1;
	/*                */
	/* Reset tallies: */
	/*                */
	ResetTallyCounters(c);	
	
	/*                                                                 */
	/* Draw pedigree if expression becomes true for one individual in  */
	/* a pedigree:                                                     */
	/*                                                                 */
	TraverseBinaryPedigreeTree2(c,c->pd,DrawPedigreeIfTrueForOneIndividual);
	
	/*                              */
	/* Done: reset RepeatInProgress */
	/*                              */
	RepeatInProgress=0;

	/*                                                                         */
	/* Show abbreviated tally (i.e., only show count of number of pedigrees,   */
	/* as detail regarding number and type of individuals is not recorded      */
	/* for this type of query (the reason is that not all INDIVIDUAL nodes     */
	/* are traversed.  If we find one INDIVIDUAL who meets the query criteria, */
	/* then the pedigree is drawn:                                             */
	/*                                                                         */
	lprintf("\n%i pedigree%s in result set.\n\n",
		c->tally.pedg.in,
		c->tally.pedg.in==1?"":"s"
	);
	
	/*                     */
	/* close print device: */
	/*                     */
	fclose(c->streams.prn);
	c->streams.prn=NULL;
	/*              */
	/* open viewer: */
	/*              */
	if(gPSViewerName.e.s[0]){

		args[0]=gPSViewerName.e.s;
		args[1]=gPedigreeDrawing.e.s;
		args[2]=NULL;		
		CreateChildProcess(gPSViewerName.e.s,args);

	}
	
}


/*                                                                  */
/* DrawPedigreeIfTrueForOneIndividual() is basically just a wrapper */
/* that can be called by the pedigree traversal function:           */
/*                                                                  */
void DrawPedigreeIfTrueForOneIndividual(PCONTROL c,PPEDIGREE pd){

	TraverseIndividualBSTUntilTrue2(c,pd,pd->h,DrawPedigreeIfTrueForThisIndividual);

}


/*                                                                        */
/* DrawPedigreeIfTrueForThisIndividual(): Draws pedigree of an individual */
/* who matches true in a DRAW FOR ... query:                              */
/*                                                                        */
/* Returns 0 if expression evaluates to false for individual,             */
/*         1 if expression evaluates to true and the pedigree is drawn    */
/*                                                                        */
int DrawPedigreeIfTrueForThisIndividual(PCONTROL c,PPEDIGREE pd,PINDIVIDUAL p){

	RESULT result;

	/*                                                               */
	/* store current pedigree and person in globally accessible      */
	/* control, c, so that recursive descent parser can access       */
	/* this information.  This allows the parser to access variables */
	/* internal to an INDIVIDUAL structure, or to evaluate the       */
	/* data of an INDIVIDUAL who's been dummied in and doesn't have  */
	/* a record in the database:                                     */
	/*                                                               */
	c->cpd=pd;
	c->cp=p;

	
	/*                                */
	/* Handle dummied in individuals: */
	/*                                */
	if(!p->db){
		/*                      */
		/* no record available: */
		/*                      */
		c->db->cr=NO_RECORD_EXISTS;
	}else{
		/*                                 */
		/* Go to this individual's record: */
		/*                                 */
		GoTo(p->db,p->rci);
	}

	/*                                                    */
	/* evaluate the expression already on                 */
	/* the token stack (assuming that RepeatInProgress is */
	/* set to TRUE, which it should be):                  */
	/*                                                    */
	EvaluateExpression(&result);
	/*                                                */
	/* if true, draw the pedigree of this individual: */
	/*                                                */
	if(result.v && result.v!=DOUBLEMISSINGVALUE){
	
		/*                                                          */
		/* If it is a singleton or some sort of messed up pedigree, */
		/* don't draw it:                                           */
		if(c->cpd->n==1 || c->cpd->nff+c->cpd->nfm<2){
			/*              */
			/* Inform user: */
			/*              */
			lprintf("Individual %s matches query criteria but pedigree %s %s\n",
				c->cp->id,
				c->cpd->fid,
				c->cpd->n==1?"is a singleton pedigree":"lacks two founding individuals"
			);
			
		}else{
			/*                    */
			/* Draw the pedigree: */
			/*                    */
			DrawPedigree(pd);
		}
		/*                                     */
		/* Tally number of pedigrees included: */
		/*                                     */
		c->tally.pedg.in++;			
			
		/*                                                            */
		/* If returning true, we can skip the rest of the individuals */
		/* in this pedigree and go on to check the next pedigree:     */
		/*                                                            */
		return 1;

	}else{

		return 0;
	}

}



/*                                                                      */
/* FieldValuesTooLongForLabel(): If a value has a string length>2, then */
/* it is considered as being too long to use directly as a label on an  */
/* icon on the pedigree drawing.                                        */
/*                                                                      */
int FieldValuesTooLongForLabel(PNODE node){

	PVARIABLE data;
	/*                   */
	/* Return if at end: */
	/*                   */
	if(!node) return 0;

	/*                    */
	/* Process left node: */
	/*                    */
	if(node->l && FieldValuesTooLongForLabel(node->l)) return 1;
	

	/*////////////////////// */
	/*                       */
	/* Process current node: */
	/*                       */
	/*////////////////////// */
	data=(PVARIABLE) node->o;
	switch(data->t){
	case STRINGVALUE:
		if(strlen(data->e.s)>2) return 1;
		break;
	case NUMERICVALUE:
		/*                                            */
		/* Return 1 if value is greater than 10 or if */
		/* value is not an integer:                   */
		/*                                            */
		if(fabs(data->e.v)>10 || floor(data->e.v)!=data->e.v) return 1;
		break;
	case DATEVALUE:
		/*                               */
		/* All data values are too long: */
		/*                               */
		return 1;
	}

	/*                     */
	/* Process right node: */
	/*                     */
	if(node->l && FieldValuesTooLongForLabel(node->l)) return 1;
	
	/*                 */
	/* Get here if OK: */
	/*                 */
	return 0;
}




/*                   */
/* PrintIconLegend() */
/*                   */
void PrintIconLegend(FILE *fp,double cx,double cy,PDBF db){

	int i;

	for(i=0;i<db->fc;i++){
		if(db->ff[i]&ICONSET){
			/*                                                    */
			/* start list of parameters for Postscript procedure: */
			/*                                                    */
			fprintf(fp,"%.2f cm %.2f cm (%s) %i [",
				cx,
				cy,
				db->FieldName(db,i),
				db->levels[i]
			);
			/*                                                     */
			/* Call PrintIconLegendLineItem() to set up Postscript */
			/* array:                                              */
			/*                                                     */
			PrintIconLegendLineItem(fp,db,i,db->fllaa[i]->h,0);
			
			/*                            */
			/* Call Postscript Procedure: */
			/*                            */
			fprintf(fp,"] DisplayLegendVariable\n");
			/*                                              */
			/* Increment cy for next round, if present:     */
			/* On the legend, each line item takes up 4 mm. */
			/* The header also uses 4 mm.  Add extra 2 mm.  */
			/* between categorical variable entries:        */
			/*                                              */
			cy-=(((double)db->levels[i]+1)*0.4+0.2);
		}
	}
}


/*                            */
/* PrintIconLegendLineItem(): */
/*                            */
int PrintIconLegendLineItem(FILE *fp,PDBF db,int fieldIndex,PNODE node,int levelIndex){
	
	PKEYVALUEPAIR pkvp;
	PVARIABLE pColor;
	PVARIABLE pGray;
	
	static VARIABLE lookupFailure;
	
	/*                                                                                 */
	/* When the user creates a new entry in a Label array (often because of a typo),   */
	/* there will be no corresponding Color or Gray array entry, so we have this       */
	/* lookupFailure VARIABLE to handle this case and we set the colors to light gray: */
	/*                                                                                 */
	lookupFailure.t=0;
	lookupFailure.e.v=0.85;
	lookupFailure.e.s="0.85 0.85 0.85";
	
	/* char hold[25]; */ /* for holding converted numbers */
	/*                                               */
	/* Return if at end:                             */
	/*                                               */
	if(!node) return levelIndex;
	
	/*                    */
	/* Process left node: */
	/*                    */
	if(node->l) levelIndex=PrintIconLegendLineItem(fp,db,fieldIndex,node->l,levelIndex);
	
	/*//////////////////////                                        */
	/*                                                              */
	/* Process current node:                                        */
	/*                                                              */
	/*//////////////////////                                        */
	/*                                                              */
	/* Set up Postscript array containing the current level (idx),  */
	/* the icon label (a->e[idx.s), and the formatted value for the */
	/* level (data->e.v or data->e.s):                              */
	/*                                                              */
	pkvp = (PKEYVALUEPAIR) node->o;
	pColor = LookupValueFromKey(db->flcaa[fieldIndex],&pkvp->key);
	pGray  = LookupValueFromKey(db->flgaa[fieldIndex],&pkvp->key);
	/*                                             */
	/* Handle the case where the key               */
	/* does not exist in the Color or Gray arrays: */
	/*                                             */
	if(!pColor) pColor = &lookupFailure;
	if(!pGray ) pGray  = &lookupFailure;
	
	switch(pkvp->key.t){
	case STRINGVALUE:
		/* pkvp->value.e.s is the LABEL for display on the icon */
		/* pkvp->key.e.s   is the LEVEL which is the key        */
		fprintf(fp,"[%i(%s)(%s) %4.2f %s ]",
			levelIndex+1,
			pkvp->value.e.s,
			pkvp->key.e.s,
			pGray->e.v,
			pColor->e.s
		);
		break;
	case NUMERICVALUE:
		fprintf(fp,"[%i(%s)(%i) %4.2f %s ]",
			levelIndex+1,
			pkvp->value.e.s,
			(int) pkvp->key.e.v,
			pGray->e.v,
			pColor->e.s
		);
		break;
	case DATEVALUE:
		fprintf(fp,"[%i(%s)(%s) %4.2f %s ]",
			levelIndex+1,
			pkvp->value.e.s,
			DelimitedGenericDateString((long)pkvp->key.e.v,DEFAULT_ISO_DATE_DELIMITER),
			pGray->e.v,
			pColor->e.s
		);
		break;
	}
	/*                               */
	/* increment idx for next round: */
	/*                               */
	levelIndex++;
	
	/*                     */
	/* Process right node: */
	/*                     */
	if(node->r) levelIndex=PrintIconLegendLineItem(fp,db,fieldIndex,node->r,levelIndex);
	/*             */
	/* return idx: */
	/*             */
	return levelIndex;
}


/*                                                                                                   */
/* Decompose():                                                                                      */
/*                                                                                                   */
/* This function is the complement (opposite of) Compose().                                          */
/*                                                                                                   */
void Decompose(void){

	RESULT result;
	RESULT result2;
	
	int filetype;
	
	/* PDBF sdb; */
	
	std::string OutputFileName;

	GetToken();
	filetype=pT->i;
	if(!(filetype==WEBRK || filetype==MADLN)) MadelineErrorReport("Decompose(): Keyword specifying the file type expected: DECOMPOSE MADELINE|WEBER FILE \"FILE_NAME\" [TO \"OUTPUT_NAME\"]");
	GetToken();
	if(pT->i!=FILEK) MadelineErrorReport("Decompose(): Keyword \"FILE\" expected: DECOMPOSE MADELINE|WEBER FILE \"FILE_NAME\" [TO \"OUTPUT_NAME\"]");
	/*                                       */
	/* Get the name of the file for reading: */
	/*                                       */
	EvaluateExpression(&result);
	if(result.t!=STRINGVALUE) MadelineErrorReport("Decompose(): Name of file to read expected.");
	
	/* No need to get another token after EvaluateExpression(): see if */
	/* keyword "TO" is also present:                                   */
	/*                                                                 */
	if(pT->i==TOC){
		/*                                                                */
		/* If TO is also present, get the optional file name for writing: */
		/*                                                                */
		EvaluateExpression(&result2);  /* get the filename                */
		if(result2.t!=STRINGVALUE)
			MadelineErrorReport("Decompose(): name of file to write expected after TO: DECOMPOSE WEBER FILE \"FILE_NAME\" [TO \"OUTPUT_NAME\"]");
		
		OutputFileName=result2.s;
		
	}else{
		
		OutputFileName=ModifyFileName(result.s,DECOMPOSED_FILE_EXTENSION);
		
	}
	
	lprintf("Decomposing input file \"%s\"\nto \"%s\"\n",
		result.s,
		OutputFileName.c_str()
	);
	
	
	/*                                                                                 */
	/* The last parameter is a flag indicating whether we wish to decompose the table: */
	/* (yes, in this case)                                                             */
	
	switch(filetype){
	case MADLN:
		lprintf("Decompose(): Sorry, the MADELINE format is not yet supported for this command!\n");
		break;
	case WEBRK:
		RecognizeWeberLabFile(result.s,OutputFileName.c_str(),1);
		lprintf("File written.\n");
		break;
	default:
		lprintf("Decompose(): Sorry, this format is not yet supported for this command!\n");
		break;
	}
	
}

/*                                                                     */
/* Square(): Square a non-rectangular data file                        */
/*                                                                     */
/*                                                                     */
void Square(void){

	RESULT result;
	RESULT result2;
	
	char *OutputFileName;
	
	/*                                       */
	/* Get the name of the file for reading: */
	/*                                       */
	EvaluateExpression(&result);
	if(result.t!=STRINGVALUE) MadelineErrorReport("Square(): Name of file to read expected");
	/*                                                                 */
	/* No need to get another token after EvaluateExpression(): see if */
	/* keyword "TO" is also present:                                   */
	/*                                                                 */
	if(pT->i==TOC){
		/*                                                               */
		/* If TO is also present, get the optional file name for writing */
		/* the .mfh header:                                              */
		/*                                                               */
		EvaluateExpression(&result2);  /* get the filename               */
		if(result2.t!=STRINGVALUE)
			MadelineErrorReport("Square(): name of file to write expected after TO (should end in \"%s\")",HEADER_FILE_EXTENSION);
		
		OutputFileName=result2.s;
	}else{
		OutputFileName=ModifyFileName(result.s,MODIFIED_FILE_EXTENSION);
	}

	/*                               */
	/* Make sure file can be opened: */
	/*                               */
	if(!File(result.s))
		MadelineErrorReport("Square(): Unable to open \"%s\". \nPerhaps the name or path is incorrect.",result.s);
	
	/*                                     */
	/* SquareDataGrid() is in recogniz.c:  */
	/*                                     */
	
	lprintf("Squaring file \"%s\" to \"%s\" ...\n",result.s,OutputFileName);
	
	SquareDataGrid(result.s,OutputFileName);
	
}

/*                               */
/* This is the END of madeline.c */
/*                               */

