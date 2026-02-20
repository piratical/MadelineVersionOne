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
/* "recognize.h" is part of the Madeline software distribution.                          */
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
/* recognize.h           */
/*                       */
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "tools.h"

#ifndef NDBF_INCLUDED
#include "ndbf.h"
#endif

#ifndef IDTREE_INCLUDED
#include "idtree.h"
#endif

#ifndef LOGS_INCLUDED
#include "logs.h"
#endif

#ifndef TABLETYPE_INCLUDED
#include "tabletypes.h"
#endif

#define HOLDBUFFERSIZE 25

#define INTEGERNUMBER       1
#define FLOATINGPOINTNUMBER 2

#ifndef MALESEX
#define MALESEX 0
#endif

#ifndef FEMALESEX
#define FEMALESEX 1
#endif

#define MAXTABEXPANSION 8
/*                        */
/* STRUCTURE DEFINITIONS: */
/*                        */

/*           */
/* LINEINFO: */
/*           */
typedef struct sLINEINFO{
	
	int  ln; /* line length in bytes                                                                                          */
	char ty; /* 'D' for data, 'H' for header, '\0' for empty lines at bottom, 'T' if line contains tab characters (forbidden) */
	char em; /* 1 for empty lines, else zero                                                                                  */
	char tb; /* 1 if line contains tabs, else zero                                                                            */
	char *b; /* pointer to the beginning of each line                                                                         */
	
}LINEINFO,*PLINEINFO;

/*             */
/* LINEINDICES */
/*             */
typedef struct sBLOCKINDICES{
	
	int headerStart;
	int headerEnd;
	int dataStart;
	int dataEnd;
	int maxDataLineLength;
	int maxDataLineTabCount;
	
}BLOCKINDICES,*PBLOCKINDICES;

/*                 */
/* PARSEDFIELDINFO */
/*                 */
typedef struct sPARSEFIELDINFO{

	char nm[MAXMBASEFIELDNAMELENGTH];
	int stt; /* starting column: equals offset to field */
	int end; /* ending column                           */
	int len; /* field lenth/width                       */
	int spc; /* terminal spacing before next field      */
	int prc; /* decimal precision                       */
	
	char typ; /* field type (D=Date,G=Genotype (character),C=Character,N=Numeric,*=Missing) */

	int character; /* no. of times this field appears to be a char. field                 */
	int numeric;   /* no. of times this field appears to be a numeric field               */
	int genotype;  /* no. of times this field appears to be a genotype field              */
	int date;      /* no. of times this field appears to be a date field                  */
	int missing;   /* no. of times this field appears to be missing                       */
	int gender;    /* no. of times this field appears to contain "M" or "F" or "m" or "f" */

}PARSEDFIELDINFO,*PPARSEDFIELDINFO;

/*                            */
/* MAPTOKENS                  */
/* Tokens read from a line of */
/* a Crimap genetic map file  */
typedef struct sMAPTOKENS{
	
	char *number;
	char *marshfieldname;
	char *markername;
	char *position;   /* used for sex-averaged map */
	char *position1;  /* used on sex-specific maps */
	char *position2;  /* used on sex-specific maps */
	char *theta;
	char *theta1;     /* ... as above ... */
	char *theta2;     /* ... as above ... */
	char *distance;
	char *distance1;
	char *distance2;
	
}MAPTOKENS,*PMAPTOKENS;


/*                      */
/* FUNCTION PROTOTYPES: */
/*                      */
int main(int argc,char *argv[]);
PLINEINFO GetLineInfo(char *b,int linecount);
PLINEINFO NewLineInfo(int linecount);
int GetLineCount(char *b);
void AssignRecordTypes(int blockCount,PLINEINFO p,int lineCount,PBLOCKINDICES pBlockIndices);
void ShowLineInfo(PLINEINFO p,int linecount);
int  GetHeaderAndRecordInfo(PLINEINFO p,int linecount,int *TotalHeaderLines,int *HeaderDataLines,int *DataLines,int *RecordLength);

void RecognizeFlatFileHeader(char *HeaderStart,char *HeaderEnd,PPARSEDFIELDINFO f,int FieldCount);
char *ReadFieldName(char *Beginning,char *HeaderEnd,char *FieldName,int MaxFieldNameLength);
char *ReadFieldType(char *Beginning,char *HeaderEnd,char *type);

int *NewIntVector(int n);
PPARSEDFIELDINFO NewParsedFieldInfo(int n);
PPARSEDFIELDINFO ParseDatabase(PLINEINFO p,int TotalHeaderLines,int DataLines,int RecordLength,int *FieldCount);
void DetermineFieldBoundaries(int *filled,PPARSEDFIELDINFO f,int RecordLength);
void DetermineFieldContents(PLINEINFO p,int DataLines,PPARSEDFIELDINFO f,int FieldCount);
void DetermineNumericPrecision(int *dots,PPARSEDFIELDINFO f,int FieldCount);
int  IsEmptyOrDot(char *p,int stt,int end);
int  IsAGenotype(char *p,int stt,int end);
int  IsADate(char *p,int stt,int end);
int  IsNumeric(char *p,int stt,int end);

int  IsGenderCode(char *p,int stt,int end);
int  GetGenderFieldIndex(PPARSEDFIELDINFO f,int FieldCount);
void AssignDefaultFieldNames(PPARSEDFIELDINFO f,int FieldCount);
int  ReadGenderCode(char *p,int stt,int end);
void ReadCharacterData(char *dest,char *p,int stt,int end);

int DetermineCoreRelationFields(PPARSEDFIELDINFO f,int FieldCount,PLINEINFO p,int DataLines,int GenderFieldIndex,int *IndividualFieldIndex,int *FatherFieldIndex,int *MotherFieldIndex);
int FindIndividualIDField(int FieldLengthGroup,PPARSEDFIELDINFO f,int FieldCount,PLINEINFO p,int DataLines,int GenderFieldIndex,int *FatherFieldIndex,int *MotherFieldIndex);
PNODE BuildIDGenderTree(int IDFieldIndex,PPARSEDFIELDINFO f,PLINEINFO p,int DataLines,int GenderFieldIndex);
int SeekParentField(PNODE tree,int IDFieldIndex,int FieldLengthGroup,PPARSEDFIELDINFO f,int FieldCount,PLINEINFO p,int DataLines,int female);
int TestCandidateParentField(PNODE tree,int TestFieldIndex,PPARSEDFIELDINFO f,PLINEINFO p,int DataLines,int female);
void AssignDefaultCoreFieldNames(PPARSEDFIELDINFO f,int iidx,int fidx,int midx);

/*                                                        */
/* Functions added to support file format rectification   */
/* in cases where files contain tab characters and uneven */
/* line lengths:                                          */
/*                                                        */
int DataBlockContainsTabs(PLINEINFO p,PBLOCKINDICES pBlockIndices);
int DataBlockLinesUneven(PLINEINFO p,PBLOCKINDICES pBlockIndices);
void TrimRightEdges(PLINEINFO p,int startingLine,int endingLine);
int DetermineTabExpansion(PLINEINFO p,PBLOCKINDICES pBlockIndices,int *ExpandedLength);
void WriteTabExpandedFile(PLINEINFO p,PBLOCKINDICES pBlockIndices,int tabsize,int linelength,const char *fn);
void WritePaddedLineFile(PLINEINFO p,PBLOCKINDICES pBlockIndices,const char *fn);
int RectifyFile(const char *fin,const char *fout);
int RecognizeFile(const char *fin,int lang,PPVARIABLE ivar,int CreateRunFile);
/*                                                */
/* Function to write the open batch command file: */
/*                                                */
void CreateOpenCommandFile(char *bfn,char *hfn,PPARSEDFIELDINFO f,int FieldCount,int iid, int fid, int mid,int gid,char *FemaleAndMaleValues,int lang,PPVARIABLE ivar);
char *GetFemaleAndMaleValues(int GenderFieldIndex,PPARSEDFIELDINFO f,PLINEINFO p,int DataLines);

/*                                                                       */
/* 19990419 ET ADDENDA: Functions to deal with allele field conversions: */
/*                                                                       */
int ConvertAlleleFieldsToGenotypeFields(PPARSEDFIELDINFO f,int FieldCount);

/*                                             */
/* 2003.01.28 Addenda & 2003.01.31.ET Addenda: */
/*                                             */

/* Function to recognize & convert Crimap sex-averaged and sex-specific map files */
int RecognizeCrimapFile(char *fin,char *fmap);
PMAPTOKENS NewMapTokensArray(int n);
/* Function to recognize & convert Marshfield online map builder map format files */
int RecognizeMarshfieldFile(char *fin,char *fmap);
/* Function to recognize & convert Simwalk result files */
int RecognizeSimwalkResultFile(char *fin,char *fout);
int RecognizeSimwalkParametricFile(PLINEINFO p,int startline,int lines,char *fout,int chromosome);
int RecognizeSimwalkNPLFile(PLINEINFO p,int startline,int lines,char *fout,int chromosome);
int RecognizeWeberLabFile(const char *fin,const char *fout,int decompose);
void SquareDataGrid(char *fin,char *fout);
int HasFieldName(PPARSEDFIELDINFO f,int fc,const char *nm);
enum TableType GuessTableType(PPARSEDFIELDINFO f,int fc);
void ReportTableType(enum TableType,const char *nm);
/* 2004.04.20.ET Addendum: */
int DetermineNumberOfBlocks(const PLINEINFO p,int lineCount);

