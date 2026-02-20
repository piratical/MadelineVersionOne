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
/* "ndbf.h" is part of the Madeline software distribution.                           */
/* Written by Edward H. Trager.                                                      */
/*                                                                                   */
/* COPYRIGHT (C) 2000 THE REGENTS OF THE UNIVERSITY OF MICHIGAN,                     */
/* Ann Arbor, Michigan, USA.  ALL RIGHTS RESERVED.                                   */
/*                                                                                   */
/* Copyright (C) 1996-2000 by Edward H. Trager and the FUSION Study Group            */
/* at the University of Michigan School of Public Health in Ann Arbor,               */
/* Michigan, USA.                                                                    */
/*                                                                                   */
/* Portions of this code were originally copyright (c) 1995 by Edward H. Trager      */
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
/*
	ndbf.h
	Originally (c) 1995 by Ed Trager.
	
	Header file for library to handle manipulation
	of standard dBase/Xbase, Foxpro and Visual Foxpro
	databases.
	
	Author:    Ed Trager, Ann Arbor, MI 48198
	Initiated: 951020
	Updated: 960911
	Updated: 961216

	Extensively revised for Madeline: 971212
		
	Revisions statement:
	
	dbf.h and dbf.c have been revised to allow reading of other
	types of databases, specifically SAS transport files and
	flat files accessed via MCOREHEADER and MFIELD structures.
	The revised code is called ndbf.h and ndbf.c.
	
	Rather than build an object-oriented database engine from scratch,
	the original dbf.c was extensively revised using object-oriented
	programming principles.  References to a DBF structure now contain
	function pointers which resolve to the appropriate functions depending
	on the type of table being used.
	
*/

/*	Includes     */

/*////////////////// */
/*                   */
/* ANSI C INCLUDES:  */
/*                   */
/*////////////////// */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>
#include <stdarg.h>
#include <limits.h>
#include <float.h>


/*//////////////////// */
/*                     */
/* MADELINE INCLUDES:  */
/*                     */
/*//////////////////// */
#include "tools.h"

#ifndef DATE_INCLUDED
#include "date.h"
#endif

#ifndef VTREE_INCLUDED
#include "vtree.h"
#endif

#ifndef FIELDFLAGS_INCLUDED
#include "fieldflags.h"
#endif

#ifndef CONFIG_INCLUDED
#include "config.h"
#endif

/*///////////////////////////////////  */
/*                                     */
/* INCLUDE BASIC DATABASE STRUCTURES:  */
/*                                     */
/* dbf.h includes:                     */
/*                                     */
/*      - portable.h                   */
/*      - marker.h                     */
/*      - variablesandarrays.h         */
/*                                     */
/*///////////////////////////////////  */
#ifndef DBF_INCLUDED
#include "dbf.h"
#endif

/* 
   Constants to be used as 3rd argument for "fseek" function
*/
#ifndef SEEK_CUR
#define SEEK_CUR    1
#endif
#ifndef SEEK_END
#define SEEK_END    2
#endif
#ifndef SEEK_SET
#define SEEK_SET    0
#endif


/*                                                                 */
/* When a record for an individual (i.e., a dummied-in individual) */
/* is unavailable:                                                 */
/*                                                                 */
#define NO_RECORD_EXISTS UINT32MISSINGVALUE

/* Constants for MergeOption: */
#define MERGEALPHA    1
#define MERGEPHYSICAL 2
#define MERGEUSER     3



/* ///////////////////////// */
/*                           */
/*    FUNCTION PROTOTYPES    */
/*                           */
/* ///////////////////////// */

void   ErrorReport(char *s,...);
UFLAG8 Endian(void);
/*                                                   */
/* CharacterizeDatabase() determines the type of the */
/* database (i.e., table) file:                      */
/*                                                   */
UFLAG8 CharacterizeDatabase(const char *nm);
/*                                                         */
/*     NewDatabase() allocates storage for the object,     */
/*     and sets the database type and correct function     */
/*     pointers when type-specific functions are required: */
/*                                                         */
PDBF   NewDatabase(const char *nm,UFLAG8 op,UFLAG8 ty);
/*                                                                         */
/*     NewNullInfoVector() is only used for Visual Foxpro XBASE databases: */
/*                                                                         */
PNULLINFO NewNullInfoVector(UINT16 n);
/*                                                           */
/*     OpenXbaseDatabase is specific to xbase derived class: */
/*     SetNullInfoVector is also specific to xbase class:    */
/*                                                           */
PDBF   OpenXbaseDatabase(PDBF db);
unsigned char *NewFieldFlag(int n);

void   SetNullInfoVector(PDBF db);
/*                                                              */
/*     The following base-class functions work for all database */
/*     types so far:                                            */
/*                                                              */
void   SetBuffer(PDBF db);
void   SetBufferToSize(PDBF db, UINT32 nr);
void   FreeBuffer(PDBF db);
UINT32 FillBuffer(PDBF db,UINT32 fr,UINT32 nr);
int    GoTo(PDBF db, UINT32 recno);
void   FreeDatabase(PDBF db);
int    Skip(PDBF db, INT32 howmany);
/*                                                            */
/*     FieldNumber() functions for all database types so far: */
/*                                                            */
int    XbaseFieldNumber(PDBF db,const char *s);
int    MbaseFieldNumber(PDBF db,const char *s);
int    SasXportFieldNumber(PDBF db,const char *s);
/*                                                                      */
/*     Base-class implementation of CloseDatabase works for all so far: */
/*                                                                      */
void   CloseDatabase(PDBF db);
/*                                                                */
/*     aton() and atoni() are used to read directly from the data */
/*     buffers of a flat file or xbase database with "N" or "F"   */
/*     field type: all blanks or just a single "." with no digits */
/*     are cases that are treated as missing values:              */
/*                                                                */
double aton(char *s,int ln);
INT32  atoni(char *s,int ln);
/*                           */
/*     Xbase read functions: */
/*                           */
INT32  ReadXbaseInt32(PDBF db,UINT16 fn,UINT32 rn);
double ReadXbaseDouble(PDBF db,UINT16 fn,UINT32 rn);
char * ReadXbaseString(PDBF db,char *hext,UINT16 fn,UINT32 rn);
long   ReadXbaseDate(PDBF db,UINT16 fn,UINT32 rn);

PSASXPORTFIELD NewSasXportField(int n);
PDBF           OpenSasXportDatabase(PDBF db);
INT32  ReadSasXportInt32(PDBF db,UINT16 fn,UINT32 rn);
double ReadSasXportDouble(PDBF db,UINT16 fn,UINT32 rn);
char * ReadSasXportString(PDBF db,char *hext,UINT16 fn,UINT32 rn);
long   ReadSasXportDate(PDBF db,UINT16 fn,UINT32 rn);

PDBF   OpenMbaseDatabase(PDBF db);
int    CheckMBaseRecordLength(PDBF db);
INT32  ReadMbaseInt32(PDBF db,UINT16 fn,UINT32 rn);
double ReadMbaseDouble(PDBF db,UINT16 fn,UINT32 rn);
char * ReadMbaseString(PDBF db,char *hext,UINT16 fn,UINT32 rn);
long   ReadMbaseDate(PDBF db,UINT16 fn,UINT32 rn);

double SwapEightBytes(double uia);
UINT32 SwapFourBytes(UINT32 uia);
INT32  SignedSwapFourBytes(INT32 uia);
UINT16 SwapTwoBytes(UINT16 uia);

char  *XbaseFieldName(PDBF db,UINT16 idx);
char   XbaseFieldType(PDBF db,UINT16 idx);
UINT16 XbaseFieldLength(PDBF db,UINT16 idx);
UINT8  XbaseFieldPrecision(PDBF db,UINT16 idx);

char  *MbaseFieldName(PDBF db,UINT16 idx);
char   MbaseFieldType(PDBF db,UINT16 idx);
UINT16 MbaseFieldLength(PDBF db,UINT16 idx);
UINT8  MbaseFieldPrecision(PDBF db,UINT16 idx);

char  *SasXportFieldName(PDBF db,UINT16 idx);
char   SasXportFieldType(PDBF db,UINT16 idx);
UINT16 SasXportFieldLength(PDBF db,UINT16 idx);
UINT8  SasXportFieldPrecision(PDBF db,UINT16 idx);

/* Needed for SAS transport format */
double IBMToIEEE754(double ibmv);

void FreeMarkerVector(PDBF db);
void FreeAllele(PALLELE a);

/*                                       */
/* Support for FIELDINDEX manipulations: */
/*                                       */
PFIELDINDEX NewFieldIndex(int i);
void FreeFieldIndex(PDBF db);

/*                                                                         */
/* 19990326 ET ADDENDA: Functions to manage BSTs built for any given field */
/*                      variable:                                          */
/*                                                                         */
PPNODE NewFieldTree(int n);
void  RemoveFieldTrees(PDBF db, int RemoveAll);
void  FreeFieldTreeVector(PDBF db);
void  BuildFieldTrees(PDBF db);           /* Build trees for all fields that need them */
void  BuildFieldTree(PDBF db, int index); /* Build the tree for one field              */
int  *NewLevelsVector(int n);
void  ManageFieldTrees(PDBF db);
void  ShowFieldTreeData(PNODE pn,PDBF db,int fwidth,int fprec,int lang);
PNODE BuildTwoKeyFieldTree(PNODE tree,PDBF db,int idx1, int idx2);
/*                                                         */
/* 19990401 & 4/02 ET Addenda: Functions to support        */
/* "transposing" a marker database and merging two tables: */
/*                                                         */
int  TransposeMarkerDatabase(PDBF sdb, int fidx, int iidx, int midx, int a1idx, int a2idx,const char *tdbname,int tdbtype);
int  CopyMbaseFieldNamesFromVariableTree(PNODE pn,int FieldIndex,PMBASEFIELD fd);
void WriteMbaseString(PDBF db,char *s,UINT16 fn,UINT32 rn);
void AddBlankMbaseRecords(PDBF db,UINT32 fr,UINT32 nr);
void WriteKeysFromTwoKeyTreeToFields(PDBF dest,PNODE key,int k1idx,int k2idx);
void WriteTransposedRecords(PDBF sdb,PDBF ddb,PNODE keytree,PNODE markertree, int fidx, int iidx, int midx, int a1idx, int a2idx);
void WriteRecordBufferToDisk(PDBF db);  /*  */
int  MergeDatabases(PPDBF SourceTable,int TableCount,char *Key1FieldName,char *Key2FieldName,char *OutputTableName,int OutputTableType,int MergeOption,char *UserFieldList);
void FreeMergeDatabasesFunctionResources(int TableCount,PPDBF SourceTable,PPNODE SourceFieldTree,PPNODE SourceKeyTree,PNODE CombinedFieldTree,PNODE CombinedKeyTree,int *Key1Index,int *Key2Index,PDBF CombinedTable);
void WriteMbaseMergedRecords(PDBF CombinedTable,PPDBF SourceTable,int TableCount,PPNODE SourceKeyTree,PPNODE SourceFieldTree,int Key1Index,int Key2Index);
void WriteMbaseDouble(PDBF db,double dv,UINT16 fn,UINT32 rn);
void WriteMbaseInt32(PDBF db,INT32 iv,UINT16 fn,UINT32 rn);
void WriteMbaseDate(PDBF db,long julian,UINT16 fn,UINT32 rn);
/*                               */
/* 19991101,19991103 ET Addenda: */
/*                               */
int CopyMbaseFieldNamesFromSourceTables(PMBASEFIELD fd,int OutputTableFieldCount,PPDBF SourceTable,int TableCount,int *Key1Index,int *Key2Index);
int CopyMbaseFieldNamesFromBuffer(PMBASEFIELD fd,int OutputTableFieldCount,char *FieldList);
/* 2003.06.05.ET xfr from madeline.c */
void CountFieldFlags(PDBF db);

#define NDBF_INCLUDED


