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
/* "ndbf.c" is part of the Madeline software distribution.                           */
/* Written by Edward H. Trager.                                                      */
/*                                                                                   */
/* COPYRIGHT (C) 2000 THE REGENTS OF THE UNIVERSITY OF MICHIGAN,                     */
/* Ann Arbor, Michigan, USA.  ALL RIGHTS RESERVED.                                   */
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
/* General Public License for more details.                                  */
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
	NDBF.C

	Original Version
	----------------        
	Author:  (c) 1995 by Ed Trager, Ann Arbor, MI 48198
	Initiated: 951020
		
	Very Modified Version
	---------------------
	Author: Ed Trager, Ann Arbor, MI 48198
	Initiated: 971212

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

/*//////////////// */
/*                 */
/* INCLUDE ndbf.h: */
/*                 */
/*//////////////// */
#include "ndbf.h"

#include "logs.h"      /* so we can use lprintf, etc... */
#include "languageconventions.h"
#include "utf8routines.h"

/*
	Database file type marker array
*/
#define MAXFILEMARKS 11
static unsigned char DbfFileMark[]={
	NOTADATABASE,
	FOXBASE,
	DBASEIII,    
	VFP,   
	DBASEIVSQL,
	DBASEIVSYS, 
	DBASEIIIMEMO,
	DBASEIVMEMO,
	DBASEIVSQLMEMO,
	FOXPROMEMO,
	FOXBASE2
};

/* This is not currently used - ET 2000 03 02 in Madeline */
/* static char *DbfFileType[]={
	"Undocumented file type",
	"FoxBase",
	"dBase III Plus",
	"Visual FoxPro",
	"dBase IV SQL",
	"dBase IV system SQL",
	"dBase III Plus with memo",
	"dBase IV with memo",
	"dBase IV SQL with memo",
	"FoxPro 2.x with memo",
	"FoxBase"
}; */

/*
	Database Field Type Array
*/
/* This is not currently used - ET 2000 03 02 in Madeline */
/* static unsigned char DbfFieldMark[]={
	FMCHARACTER,
	FMCURRENCY, 
	FMNUMERIC,  
	FMFLOAT,         
	FMDATE,          
	FMDATETIME,      
	FMDOUBLE,        
	FMINTEGER,       
	FMLOGICAL,       
	FMMEMO,          
	FMGENERAL,       
	FMPICTURE,       
	FMOBJECT,        
	FMSCREEN,
	FMUNDEFINED
}; */

/*
	Database field type descriptions
	Note that this array can be
	directly indexed with the proper
	offset, i.e.:
	
	char *desc=DbfFieldType[FieldMark-'A'];
	
	... as long as the index is in range
*/
/* This is not currently used - ET 2000 03 02 in Madeline */
/* static char * DbfFieldType[]={
	"unknown type \"A\"",
	"double",
	"character",
	"date",
	"unknown type \"E\"",
	"float",
	"general",
	"unknown type \"H\"",
	"integer",
	"unknown type \"J\"",
	"unknown type \"K\"",
	"logical",
	"memo",
	"numeric",
	"object",
	"picture",
	"unknown type \"Q\"",
	"unknown type \"R\"",
	"screen",
	"dateTime",
	"undefined type \"U\"",
	"unknown type \"V\"",
	"unknown type \"W\"",
	"unknown type \"X\"",
	"currency",
	"unknown type \"Z\""
}; */


/*                       */
/* Global Null String    */
/*                       */
const char NULLSTRING[]="\0";


/*                                                                         */
/* global flag set to M_BIG_ENDIAN, M_LITTLE_ENDIAN, or M_PERVERSE_ENDIAN: */
/*                                                                         */
unsigned char gle=0;


/*                       */
/*      FUNCTIONS        */
/*                       */


/*                                       */
/*      ErrorReport                      */
/*                                       */
/*      Simple error reporting mechanism */
/*      used in prototyping DBF.C        */
/*                                       */
void ErrorReport(char *s,...){
	
	va_list argptr;

	fprintf(stderr,"\nDatabase processor report:\n");
	va_start(argptr,s);
	vfprintf(stderr,s,argptr);
	va_end(argptr);        
	fprintf(stderr,"\n");
	exit(1);

}

/*                                                                  */
/* Endian(): determines if system is big-endian, little endian, or  */
/* perverse-endian (i.e. where successively significant             */
/* bytes in a long int are stored non-sequentially, like 1-3-2-4 or */
/* something.  Perverse-endian would include the PDP scheme: it's   */
/* not supported:                                                   */
/*                                                                  */
UFLAG8 Endian(void){

	unsigned long tv=0x0055AA81;
	unsigned char *t=(unsigned char *) &tv;

	if(t[0]==0x00 && t[1]==0x55 && t[2]==0xAA) return (UFLAG8) M_BIG_ENDIAN;
	if(t[0]==0x81 && t[1]==0xAA && t[2]==0x55) return (UFLAG8) M_LITTLE_ENDIAN;
	/*                                             */
	/* Get here only if system is perverse-endian: */
	/*                                             */
	eprintf(FATALFLAG,"ndbf.c: Endian(): This system is perverse-endian.\nMadeline will not run correctly on this platform.");
	return (UFLAG8) M_PERVERSE_ENDIAN; 
}


/*                                                   */
/*                                                   */
/* CharacterizeDatabase() ... as being XBASE, MBASE, */
/* never-opened flat file, or SAS XPORT dataset.     */
/* In the future, more file types may be added:      */
/*                                                   */
#define CDBUFFERSIZE 51
UFLAG8 CharacterizeDatabase(const char *nm){

	FILE *fp;
	char b[CDBUFFERSIZE], *bufferStart=b;
	char *bb;
	
	/*                                                                    */
	/* 2003.01.23.ET:                                                     */
	/*                                                                    */
	/* According to Daniel Feenberg at the National Bureau of Economic    */
	/* Research (http://www.nber.org/data/sasport.html), the first 16     */
	/* bytes of different types of SAS files look like this:              */
	/*                                                                    */
	/*               type        first 16 bytes                           */
	/*                                                                    */
	/*               system     'SAS     6.03    '                        */
	/*               XPORT      'HEADER RECORD***'                        */
	/*               CPORT      'LIB CONTROL PC D'                        */
	/*               CPORT      '**COMPRESSED** *'                        */
	/*                                                                    */
	/* ... So now we can have Madeline detect these different types       */
	/* even though Madeline can still only handle the XPORT type.         */
	/*                                                                    */
	char sas_xport_header[]  ="HEADER RECORD*******LIBRARY HEADER RECORD!!!!!!!";
	char sas_system_header[] ="SAS";         /* Anything beyond the word "SAS" could differ by version and platform!    */
	char sas_cport_header_1[]="LIB CONTROL"; /* The letters after LIB CONTROL differ depending on version and platform! */
	char sas_cport_header_2[]="**COMPRESSED**"; /* Again, we'll try matching on as little as necessary to avoid unkowns */
	char mdb[]="MadelineDatabase";
	char mfh[]="MadelineFlatHead";
	int i;
	
	/*                        */
	/* open file for reading: */
	/*                        */
	fp=fopen(nm,"rb");
	if(!fp)
		MadelineErrorReport("CharacterizeDatabase(): Unable to open %s.  Perhaps the file name is incorrect?",nm);
		
	if(fread(b,CDBUFFERSIZE-1,1L,fp)!=1){
		fclose(fp);
		MadelineErrorReport("CharacterizeDatabase(): Error reading from %s.  Perhaps the file is corrupt?",nm);
	}
	/*             */
	/* close file: */
	/*             */
	fclose(fp);
	
	b[CDBUFFERSIZE-1]='\0';
	
	/*                 */
	/* Check if MBASE: */
	/*                 */
	if(strncmp(b,mdb,strlen(mdb))==0) return MBASE;
	if(strncmp(b,mfh,strlen(mfh))==0) return MBASE;
	/*                     */
	/* Check if SAS XPORT: */
	/*                     */
	if(strncmp(b,sas_xport_header,strlen(sas_xport_header))==0) return SASXPORT;
	
	/*                                                           */
	/* 2004.08.12.ET: CHECK IF IT IS AN ASCII OR UTF-8 FLAT FILE */
	/*                                                           */
	//
	// First, trim off the last UTF-8 sequence which may be truncated
	// so that isLegalUTF8String won't return false on a truncated legal
	// fragment:
	// 0xC0 is the bit pattern for the beginning byte of a UTF-8 sequence
	// 0x80 is the bit pattern for a continuation byte of a UTF-8 sequence
	//
	// i is used to track how many bytes we strip off:
	// 	
	for(i=0,bb= bufferStart+CDBUFFERSIZE-2; bb>bufferStart && (*bb & 0xC0) == 0x80 ; bb--,i++){
		*bb='\0';
	}
	// Should now be on the first byte of a UTF-8 sequence, or else an ASCII character:
	if((*bb & 0xC0) == 0xC0 ){
		*bb='\0';
		i++;
	}
	//
	// If it is an ASCII or UTF-8 string, it must be full-length:
	// (That is, a long as it should be after stripping off possibly one incomplete UTF-8
	// sequence at the end of the test fragment:
	//
	if(strlen(b)<(unsigned)CDBUFFERSIZE-1-i){
		lprintf("Test string is too short! This file is probably in an unsupported binary format ...\n");
		return 0;
	}
	if(isLegalUTF8String((const UTF8*)b)){
		return FLATFILE;
	}
	
	/*                                */
	/* Catch unsupported SAS formats: */
	/*                                */
	if(strncmp(b,sas_system_header,strlen(sas_system_header))==0) 
		MadelineErrorReport("CharacterizeDatabase(): This file looks like a SAS system file.  Madeline only supports the SAS XPORT file format.");
	if(strncmp(b,sas_cport_header_1,strlen(sas_cport_header_1))==0) 
		MadelineErrorReport("CharacterizeDatabase(): This file looks like a SAS CPORT file.  Madeline only supports the SAS XPORT file format.");
	if(strncmp(b,sas_cport_header_2,strlen(sas_cport_header_2))==0) 
		MadelineErrorReport("CharacterizeDatabase(): This file looks like a SAS compressed CPORT file.  Madeline only supports the SAS XPORT file format.");
		
	/* 2004.10.22.ET CHANGE: The single-byte XBASE checks have      */
	/* been moved to the end here, so that XBASE is, given its very */
	/* old legacy status, checked LAST.  This was necessary be-     */
	/* cause the "magic number" in the first byte of a              */
	/* DBASEIVSQL is 0x43, i.e. ASCII LATIN CAPITAL LETTER "C"      */
	/* and for DBASEIVSYS it is 0x63, ASCII LATIN SMALL LETTER "c"  */
	/* -- which on *rare* but *extant* occasions will conflict with */
	/* a simple ASCII or UTF-8 data table -- Aarrgghhhh! Also note  */
	/* that the magic number file used by the Linux "file" utility  */
	/* does not provide any help in finding a better way to detect  */
	/* these old file formats.                                      */
	/*                                                              */
	/* Check for XBASE as a last resort strategy:                   */
	/*                                                              */
	for(i=1;i<=10;i++) if(b[0]==DbfFileMark[i]) return XBASE;
	
	/*                                   */
	/* Other unknown types: return zero: */
	/*                                   */
	return 0;
}



/*                                                                              */
/*      NewDatabase(): Allocates storage,initializes                            */
/*                     data, and sets function pointers                         */
/*                     for a new database object                                */
/*                 having file name nm, to be opened in                         */
/*                 op mode, and being of type ty (XBASE,MBASE,SASXPORT, etc...) */
PDBF NewDatabase(const char *nm,UFLAG8 op,UFLAG8 ty){
	
	PDBF db;
	
	db=(PDBF) malloc((unsigned) sizeof(DBF));
	if(db==NULL) eprintf(FATALFLAG,"NewDatabase(): unable to allocate memory for database");
	/*                                                                      */
	/* initializations:                                                     */
	/*                                                                      */
	strcpy(db->nm,nm);    /* assign file name                               */
	db->op=op;            /* assign file open mode as READWRITE or READONLY */
	/*                                                                      */
	/* Set database type: sub-types are, however, determined                */
	/* when the file is opened:                                             */
	/*                                                                      */
	db->ty     =ty;
	db->subtype=GENERICSUBTYPE;
	
	db->fp  =NULL;   /* not yet opened                */
	db->fp2 =NULL;   /* associated file also not open */
	db->rc  =0;      /* record count                  */
	db->cr  =0;      /* current record index          */
	db->rl  =0;      /* record length                 */
	db->fc  =0;      /* field count                   */
	db->swap=0;      /* defaults to no swapping       */

	db->rc  =0;       /* record count                                        */
	db->rl  =0;       /* record length                                       */
	db->of  =0;       /* offset                                              */
	db->cr  =0;       /* current record --can't use missing on unsigned long */
	db->fc  =0;       /* field count                                         */

	/*                               */
	/* field subheader is a union of */
	/* pointers, so all types can be */
	/* initialized here:             */
	/*                               */
	db->fd.x=NULL;
	db->fd.m=NULL;
	db->fd.s=NULL;

	/*                                      */
	/* marker fields and information:       */
	/*                                      */
	db->m =NULL; /* assume no marker fields */
	db->mc=0;   

	/*                     */
	/* Buffer information: */
	/*                     */
	db->b.r  =NULL;
	db->b.cap=0;
	db->b.nr =0;
	db->b.fr =0; /* can't use MISSING on unsigned long */
	db->b.lr =0; /* can't use MISSING on unsigned long */
	

			
	/*                                */
	/* Type-specific initializations: */
	/*                                */
	if(db->ty==XBASE){

		/*              */
		/* core header: */
		/*              */
		db->hd.x.ty=0;
		db->hd.x.yr=0;
		db->hd.x.mn=0;
		db->hd.x.dy=0;
		db->hd.x.rc=0;
		db->hd.x.of=0;
		db->hd.x.ln=0;
		db->hd.x.rs[0]='\0';
		db->hd.x.fl=0;
		db->hd.x.cp=0;
		db->hd.x.r2[0]='\0';
		/*                              */
		/* function pointer assignment: */
		/*                              */
		db->OpenDatabase =OpenXbaseDatabase;
		db->ReadInt32    =ReadXbaseInt32;
		db->ReadDouble   =ReadXbaseDouble;
		db->ReadString   =ReadXbaseString;
		db->ReadDate     =ReadXbaseDate;
		db->FieldNumber  =XbaseFieldNumber;
		db->FieldName     =XbaseFieldName;
		db->FieldType     =XbaseFieldType;
		db->FieldLength   =XbaseFieldLength;
		db->FieldPrecision=XbaseFieldPrecision;
		
	}else if(db->ty==MBASE){
	
		db->hd.m.mtype[0] ='\0';
		db->hd.m.ffn[0]   ='\0';
		db->hd.m.crd[0]   ='\0';
		db->hd.m.lmd[0]   ='\0';
		db->hd.m.endian[0]='\0';
		db->hd.m.fc       =0;
		db->hd.m.ln       =0;
		db->hd.m.nmv[0]   ='\0';
		db->hd.m.cmv[0]   ='\0';
		db->hd.m.sk       =0;

		/*                              */
		/* function pointer assignment: */
		/*                              */
		db->OpenDatabase =OpenMbaseDatabase;
		db->ReadInt32    =ReadMbaseInt32;
		db->ReadDouble   =ReadMbaseDouble;
		db->ReadString   =ReadMbaseString;
		db->ReadDate     =ReadMbaseDate;
		db->FieldNumber  =MbaseFieldNumber;
		db->FieldName     =MbaseFieldName;
		db->FieldType     =MbaseFieldType;
		db->FieldLength   =MbaseFieldLength;
		db->FieldPrecision=MbaseFieldPrecision;
			
	}else if(db->ty==SASXPORT){

		/*                                            */
		/* This is more than enough initialization    */
		/* to stop most functions if they encountered */
		/* an unopened object:                        */
		/*                                            */
		db->hd.s.h1.s[0]   ='\0';
		db->hd.s.h2.sas1[0]='\0';
		db->hd.s.h2.sas2[0]='\0';
		db->hd.s.h3.md[0]  ='\0';
		db->hd.s.mh1.sz[0] ='\0';
		db->hd.s.mh2.pd[0] ='\0';
		db->hd.s.rh1.sas[0]='\0';
		db->hd.s.rh2.md[0] ='\0';
		db->hd.s.rh3.vc[0] ='\0';
	
		/*                              */
		/* function pointer assignment: */
		/*                              */
		db->OpenDatabase =OpenSasXportDatabase;
		db->ReadInt32    =ReadSasXportInt32;
		db->ReadDouble   =ReadSasXportDouble;
		db->ReadString   =ReadSasXportString;
		db->ReadDate     =ReadSasXportDate;
		db->FieldNumber  =SasXportFieldNumber;
		db->FieldName     =SasXportFieldName;
		db->FieldType     =SasXportFieldType;
		db->FieldLength   =SasXportFieldLength;
		db->FieldPrecision=SasXportFieldPrecision;
		
	}
	
	
	/*                                   */
	/* Field flag and field type counter */
	/* initialization:                   */
	/*                                   */
	db->ff   =NULL;
	db->n.c  =0;    /* core known set          */
	db->n.ci =0;    /* core in set             */
	db->n.co =0;    /* core out set            */
	db->n.p  =0;    /* phenotype set           */
	db->n.po =0;    /* phenotype output set    */
	db->n.g  =0;    /* genotype set            */
	db->n.go =0;    /* genotype output set     */
	db->n.v  =0;    /* covariate subset        */
	db->n.vo =0;    /* covariate output subset */
	db->n.o  =0;    /* total output set        */

	/*                           */
	/* FIELDINDEX pointer: NULL; */
	/*                           */
	db->fi=NULL;
	
	/*                                                                */
	/* Field tree node pointer: NULL;                                 */
	/*                                                                */
	db->ftree=NULL;  /* NULL tree vector                              */
	db->levels=NULL; /* NULL vector of number of non-missing levels   */
	db->missingcount=NULL; /* NULL vector of count of missing entries */
	db->fllaa=NULL; /* NULL vector of PASSOCARRAY label arrays        */
	db->flcaa=NULL; /* NULL vector of PASSOCARRAY color arrays        */
	db->flgaa=NULL; /* NULL vector of PASSOCARRAY gray  arrays        */
	/*                                                                   */
	/* NULLINFO pointer initialization:                                  */
	/*                                                                   */
	db->nfi  =MISSING; /* _NullFlags field index, if VFP type with nulls */
	db->ni   =NULL;
	
	return db;
}


/*                                                      */
/* NewNullInfoVector(): allocates a new NULLINFO vector */
/*                                                      */
PNULLINFO NewNullInfoVector(UINT16 n){

	int i;
	PNULLINFO pn;
	
	pn=(PNULLINFO)malloc(n*sizeof(NULLINFO));
	if(!pn) eprintf(FATALFLAG,"NewNullInfoVector(): out of memory allocating vector");
	
	for(i=0;i<n;i++){
		pn[i].hasnulls=(UFLAG8) 0;
		pn[i].nth     =(UINT16) 0;
		pn[i].of      =(UINT8)  0;
		pn[i].mask    =(UINT8)  0;
	
	}
	return pn;
}



/*                                              */
/* OpenXbaseDatabase(): open an x-base database */
/*                                              */
PDBF   OpenXbaseDatabase(PDBF db){

	int  i;
	int  LnBackLinkInfo;
	char ReadOnly[]="rb";
	char ReadWrite[]="rb+";

	
	/*            */
	/* open file: */
	/*            */
	if(db->ty!=XBASE) eprintf(FATALFLAG,"OpenXbaseDatabase(): Flag not set to XBASE!");    
	db->fp=fopen(db->nm,db->op==READONLY?ReadOnly:ReadWrite);
	if(db->fp==NULL)
		eprintf(FATALFLAG,"OpenXbaseDatabase(): Unable to open file %s",db->nm);

	/*                                          */
	/* read XBASECOREHEADER structure directly: */
	/*                                          */
	if(fread(&db->hd.x,sizeof(XBASECOREHEADER),1L,db->fp)!=1L)
		eprintf(FATALFLAG,"OpenXbaseDatabase(): Unable to read XBASECOREHEADER in %s",db->nm);

	/*                                             */
	/* loop through possible values in DbfFileMark */
	/* to determine database type:                 */
	/*                                             */
	for(i=0;i<MAXFILEMARKS && db->hd.x.ty!=DbfFileMark[i];i++);
	if(i==0 || i==MAXFILEMARKS)
		eprintf(FATALFLAG,"OpenXbaseDatabase(): unknown file type 0x%4x",db->hd.x.ty);

	/*                                                        */
	/* make a note if the file uses the Visual FoxPro format: */
	/*                                                        */
	if(db->hd.x.ty==VFP){
		db->subtype=VFPSUBTYPE;
		LnBackLinkInfo=LNBACKLINKINFO;  /*set LnBackLinkInfo length for VFP type */
	}else
		LnBackLinkInfo=0;              /*zero length back link for a generic xbase type */


	/*                                  */
	/* swap bytes on big-endian systems */
	/*                                  */
	if(!gle) gle=Endian();
	if(gle==M_BIG_ENDIAN){
		db->hd.x.rc=SwapFourBytes(db->hd.x.rc);
		db->hd.x.of=SwapTwoBytes(db->hd.x.of);
		db->hd.x.ln=SwapTwoBytes(db->hd.x.ln);
	}

	/*                                                     */
	/* place record count, offset to first record,         */
	/* and record length into generically-accessible spot: */
	/* for "base class" function access:                   */
	/*                                                     */
	db->rl=db->hd.x.ln;
	db->rc=db->hd.x.rc;
	db->of=db->hd.x.of;
	

	/*                             */
	/* calculate number of FIELDS: */
	/*                             */
	db->fc=(db->hd.x.of-LNMAINHEADER-LNHEADERTERMINATOR-LnBackLinkInfo)/LNFIELDSUBHEADER;
	if(db->fc==0)
		eprintf(FATALFLAG,"OpenXbaseDatabase(): No fields present");
	
	/*                              */
	/* allocate storage for fields: */
	/*                              */
	db->fd.x=(PXBASEFIELD)malloc((unsigned)sizeof(XBASEFIELD)*db->fc);
	if(db->fd.x==NULL)
		eprintf(FATALFLAG,"OpenXbaseDatabase(): No memory for XBASEFIELDs");

	/*                                  */
	/* directly read field information: */
	/*                                  */
	if(fread(db->fd.x,sizeof(XBASEFIELD),db->fc,db->fp)!=(size_t)db->fc)
		eprintf(FATALFLAG,"OpenXbaseDatabase(): Error reading FIELD subheaders");

	/*                                           */
	/* swap the field subheader offset integers: */
	/*                                           */
	if(gle==M_BIG_ENDIAN)
		for(i=0;i<db->fc;i++)
			db->fd.x[i].of=SwapFourBytes(db->fd.x[i].of);
	/*                                                            */
	/* should be directly on the 0x0D original header terminator: */
	/*                                                            */
	if(fgetc(db->fp)!=HEADERTERMINATOR)
		eprintf(FATALFLAG,"OpenXbaseDatabase(): HEADER terminator incorrect");

	/*                                   */
	/* allocate storage for field flags: */
	/*                                   */
	db->ff=NewFieldFlag(db->fc);
	
	/*                                            */
	/* allocate storage for FIELDINDEX structure: */
	/*                                            */
	db->fi=NewFieldIndex(db->fc);
	
	/*                                              */
	/* allocate storage for field PNODE head nodes: */
	/*                                              */
	db->ftree=NewFieldTree(db->fc);
	/*                                           */
	/* allocate storage for field levels vector: */
	/*                                           */
	db->levels=NewLevelsVector(db->fc);
	/*                                                 */
	/* allocate storage for field missingcount vector: */
	/*                                                 */
	db->missingcount=NewLevelsVector(db->fc);

	/*                                                 */
	/* Allocate storage for field labels array vector: */
	/*                                                 */
	db->fllaa=NewPointerToAssociativeArrayVector(db->fc);
	db->flcaa=NewPointerToAssociativeArrayVector(db->fc);
	db->flgaa=NewPointerToAssociativeArrayVector(db->fc);
	/*                                                         */
	/* Check for and then set up NULLINFO vector, if required: */
	/*                                                         */
	if(db->subtype==VFPSUBTYPE) SetNullInfoVector(db);

	/*                                                */
	/* set up buffer via call to base class function: */
	/*                                                */
	SetBuffer(db);          
	/*                                                            */
	/* Fill the buffer up to its capacity, or the whole database, */
	/* whichever is smaller:                                      */
	/*                                                            */
	FillBuffer(db,0,db->b.cap);
	
	return db;
}


/*                */
/* NewFieldFlag() */
/*                */
unsigned char *NewFieldFlag(int n){

	int i;
	unsigned char *ff;      

	ff=(unsigned char *) malloc((unsigned) sizeof(unsigned char)*n);
	if(!ff)
		eprintf(FATALFLAG,"NewFieldFlag(): No memory for field flag allocation");

	for(i=0;i<n;i++) ff[i]='\0';
	
	return ff;
}



/*                                                                */
/* NewFieldTree(): Allocate vector of PNODE pointers set to NULL: */
/*                                                                */
/* 19990326 ET ADDENDUM                                           */
/*                                                                */
PPNODE NewFieldTree(int n){

	int i;
	PPNODE ppn;      

	ppn=(PPNODE) malloc((unsigned) sizeof(PNODE)*n);
	if(!ppn)
		eprintf(FATALFLAG,"NewFieldTree(): out of memory allocating PPNODE vector");

	for(i=0;i<n;i++) ppn[i]=NULL;
	
	return ppn;
}


/*                                                                               */
/* RemoveFieldTrees(): Removes field trees that have been built on               */
/* the db->ftree vector for any fields:                                          */
/*                                                                               */
/* PDBF db: the database                                                         */
/*                                                                               */
/* int RemoveAll: if true, removes all field trees.                              */
/*                If false, only those fields no longer marked with ICONSET flag */
/*                are removed                                                    */
/*                                                                               */
/* 19990326 ET ADDENDUM                                                          */
/*                                                                               */
void RemoveFieldTrees(PDBF db, int RemoveAll){

	int i;
	
	for(i=0;i<db->fc;i++){
		/*                                      */
		/* free any trees that have been built: */
		/*                                      */
		/* FreeTree() is from tree.c:           */
		/* FreeVariableObject is from vtree.c:  */
		/*                                      */
		if(db->ftree[i]){
			/*                                                      */
			/* Remove an allocated tree if:                         */
			/*     (1) RemoveAll flag is true                       */
			/* or: (2) ICONSET flag is no longer set for this field */
			/*                                                      */
			if(RemoveAll || !(db->ff[i]&ICONSET) ){
				FreeTree(db->ftree[i],FreeVariableObject);
				db->ftree[i]=NULL;
			}
			/*                                                   */
			/* Since the field tree has been removed, also reset */
			/* the missing counter to zero:                      */
			/*                                                   */
			db->missingcount[i]=0;			
		}
	}
}


/*                                                       */
/* BuildFieldTrees(): Builds a field tree for any        */
/* field marked with the ICONSET flag which does not yet */
/* have a tree built:                                    */
/*                                                       */
/* 19990326 ET ADDENDUM:                                 */
/*                                                       */
void BuildFieldTrees(PDBF db){

	int i;

	/*                                                            */
	/* Look for ICONSET fields which don't yet have a tree built: */
	/*                                                            */
	for(i=0;i<db->fc;i++){
	
		if(db->ff[i]&ICONSET && !db->ftree[i]){
			/*                            */
			/* build tree for this field: */
			/*                            */
			BuildFieldTree(db,i);
		}
	}
}

/*                                                  */
/* BuildFieldTree():                                */
/*                                                  */
/* Builds a BST tree on the values                  */
/* for a given field specified by index.  The head  */
/* node is stored in db->ftree[index]:              */
/*                                                  */
/* The count of missing values is stored separately */
/* on db->missingcount[index]:                      */
/*                                                  */
/* 19990326 ET ADDENDUM:                            */
/*                                                  */
void BuildFieldTree(PDBF db, int index){

	VARIABLE d,*data=&d;
	PNODE tree=NULL;
	UINT32 i;
	int type;

	
	/*                       */
	/* Determine Field type: */
	/*                       */
	type=db->FieldType(db,index);
	
	switch(type){
	case 'N':
		/*                */
		/* set data type: */
		/*                */
		data->t=NUMERICVALUE;
		data->i=MISSING;
		/*                      */
		/* Read numeric values: */
		/*                      */
		for(i=0;i<db->rc;i++){
			/*                    */
			/* read numeric data: */
			/*                    */
			data->e.v=db->ReadDouble(db,index,i);
			/*                                               */
			/* Build BST tree using functions from tree.c:   */
			/* Do not include the missing value indicator in */
			/* the tree:                                     */
			/*                                               */
			if(data->e.v!=DOUBLEMISSINGVALUE){
				if(!tree) tree=HeadNode((void *)data,CopyVariableObject);
				else      tree=AddNode(tree,(void *)data,CompareVariableObjects,CopyVariableObject,1);
			}else{
				/* Record count of missing values: */
				db->missingcount[index]++;
			}			
		}
		break;
	case 'D':
		/*                */
		/* set data type: */
		/*                */
		data->t=DATEVALUE;
		data->i=MISSING;
		/*             */
		/* Read dates: */
		/*             */
		for(i=0;i<db->rc;i++){
			/*                 */
			/* read date data: */
			/*                 */
			data->e.v=db->ReadDate(db,index,i);
			/*                                               */
			/* Build BST tree using functions from tree.c:   */
			/* Do not include the missing value indicator in */
			/* the tree:                                     */
			/*                                               */
			if(data->e.v!=MISSINGDATE){
				if(!tree) tree=HeadNode((void *)data,CopyVariableObject);
				else      tree=AddNode(tree,(void *)data,CompareVariableObjects,CopyVariableObject,1);
			}else{
				/* Record count of missing values: */
				db->missingcount[index]++;
			}
		}
		break;
	case 'C':
		/*                */
		/* set data type: */
		/*                */
		data->t=STRINGVALUE;
		data->i=MISSING;
		/*               */
		/* Read strings: */
		/*               */
		for(i=0;i<db->rc;i++){
			/*                   */
			/* read string data: */
			/*                   */
			data->e.s=db->ReadString(db,NULL,index,i);
			/*                                                     */
			/* Build BST tree using functions from tree.c:         */
			/* Do not include the missing value indicator in       */
			/* the tree: CHARMISSINGSTRING is the NULLSTRING,      */
			/* so we need only check that it's not the NULLSTRING: */
			/*                                                     */
			if(data->e.s[0]){
				if(!tree) tree=HeadNode((void *)data,CopyVariableObject);
				else      tree=AddNode(tree,(void *)data,CompareVariableObjects,CopyVariableObject,1);
			}else{
				/* Record count of missing values: */
				db->missingcount[index]++;
			}
		}
		break;
	}
	if(tree){
		/*                                    */
		/* save pointer to head node of tree: */
		/*                                    */
		db->ftree[index]=tree;
		/*                                                               */
		/* Calculate ranks and total number of unique, non-missing keys: */
		/*                                                               */
		db->levels[index]=SetNodeRanks(db->ftree[index],0);
	}
}


/*                                                       */
/* FreeFieldTreeVector(): Frees vector of PNODE pointers */
/*                                                       */
/* 19990326 ET ADDENDUM                                  */
/*                                                       */
void FreeFieldTreeVector(PDBF db){
	/*                                       */
	/* Remove trees allocated on the vector: */
	/* The flag 1 means clear out all trees: */
	/*                                       */
	RemoveFieldTrees(db,1);
	/*                                        */
	/* free the database's field tree vector: */
	/*                                        */
	free(db->ftree);
}

/*                                                    */
/* ShowFieldTreeData(): This recursive function       */
/* lists out the data in a field tree in order:       */
/*                                                    */
/* int fwidth and fprec specify the formatting width  */
/* and precision (ignored if not applicable) for the  */
/* values field.                                      */
/*                                                    */
/* int lang -> specifies language and formatting      */
/* conventions for dates                              */
/*                                                    */
/* 19990327 ET ADDENDUM                               */
/*                                                    */
void ShowFieldTreeData(PNODE pn,PDBF db,int fwidth,int fprec,int lang){

	/* int type; */
	PVARIABLE pv;
	double percent;
	int i;
	
	/*                 */
	/* Stop if at end: */
	/*                 */
	if(!pn) return;

	/*                    */
	/* recurse left node: */
	/*                    */
	ShowFieldTreeData(pn->l,db,fwidth,fprec,lang);

	/*/////////////////////////                                           */
	/*                                                                    */
	/* Process Current Node:                                              */
	/*                                                                    */
	/*/////////////////////////                                           */
	pv=(PVARIABLE)pn->o; /* set the VARIABLE pointer to the node's object */
	/*                                                                    */
	/* Display depends on the type of the VARIABLE:                       */
	/*                                                                    */
	percent=100.0*(double)pn->n/(double)db->rc;
	switch(pv->t){
	case NUMERICVALUE:
		lprintf("%5i. %*.*lf %5i  %5.1f  ",
			pn->rank,
			fwidth,
			fprec,
			pv->e.v,
			pn->n,
			percent
		);
		break;
	case DATEVALUE:
		lprintf("%5i. %-*.*s %5i  %5.1f  ",
			pn->rank,
			fwidth,
			fwidth,
			DelimitedGenericDateString((long)pv->e.v,'.'),
			pn->n,
			percent
		);	
		break;
	case STRINGVALUE:
		lprintf("%5i. %-*.*s %5i  %5.1f  ",
			pn->rank,
			fwidth,
			fwidth,
			pv->e.s,
			pn->n,
			percent
		);
		break;
	}
	percent/=5.0;
	percent=floor(percent);
	for(i=0;i<(int)percent;i++) lprintf("%c",i%2?'+':'-');
	lprintf("\n");
	/*                     */
	/* Recurse right node: */
	/*                     */
	ShowFieldTreeData(pn->r,db,fwidth,fprec,lang);
}



/*                                                  */
/* SetNullInfoVector(): Set up the null information */
/* vector required for Visual FoxPro databases:     */
/*                                                  */
void SetNullInfoVector(PDBF db){

	UINT16 nth     =0;
	UINT8  byteof  =0;
	UINT8  start;
	UINT8  mask    =0;
	UINT8  filled;
	int i;
	UFLAG8 HasNulls=0;
	/*                                       */
	/* check if any null fields are present: */
	/*                                       */
	for(i=0;i<db->fc;i++){
		if(db->fd.x[i].fl & FFNULLSOK){
			HasNulls++;
			break;
		}
	}
	
	if(HasNulls){
		/*                                       */
		/* get here if there are such fields, so */
		/* set nfi and allocate NULLINFO vector: */
		/*                                       */
	
		db->nfi=db->FieldNumber(db,"_NullFlags");
		if(db->nfi==MISSING) eprintf(FATALFLAG,"Can't find _NullFlags field in %s",db->nm);
	
		db->ni=NewNullInfoVector(db->fc);
		/*                                          */
		/* Loop through fields now and set ni data: */
		/*                                          */
		mask=start=~(~mask<<1);
		filled=~(~mask>>1);
		for(i=0;i<db->fc;i++){
			if(db->fd.x[i].fl && FFNULLSOK){
				db->ni[i].hasnulls=1;
				db->ni[i].nth     =++nth;
				db->ni[i].of      =byteof;
				db->ni[i].mask    =mask;
			
				if(mask & filled){
					byteof++;
					mask=start;
				}else{
					mask<<=1;
				}
			}                       
		}
	}
}


/*                                                                */
/*  SetBuffer                                                     */
/*                                                                */
/*  Sets the buffer size based on how many records will fit       */
/*  into buffer.                                                  */
/*  memset is used to fill the buffer with ASCII space character, */
/*  0x20, so that blank records can be added.                     */
/*                                                                */
void SetBuffer(PDBF db){

	UINT32 bytes;

	/*                                          */
	/* how many records will fit into a buffer? */
	/* --make this the buffer capacity:         */
	/*                                          */
	db->b.cap=MAXBUFSIZE/db->rl;
	
	/*                                                          */
	/* adjust the buffer size capacity if actual record count   */
	/* is less than the buffer capacity and this is a read only */
	/* situation where no expansion of buffer is needed:        */
	/*                                                          */
	if(db->op==READONLY && db->rc<db->b.cap) db->b.cap=db->rc;
	/*                                      */
	/* now allocate or re-allocate storage: */
	/*                                      */
	bytes=db->b.cap*db->rl;
	if(!db->b.r) db->b.r=(char *)malloc(bytes);
	else         db->b.r=(char *)realloc(db->b.r,bytes);
	if(!db->b.r) eprintf(FATALFLAG,"SetBuffer() allocation failed");
	memset(db->b.r,' ',bytes);
}

/*                                                                    */
/*                                                                    */
/* SetBufferToSize:                                                   */
/*                                                                    */
/* Sets the buffer size explicitly to hold nr records, if they'll fit */
/*                                                                    */
void SetBufferToSize(PDBF db, UINT32 nr){

	UINT32 bytes;

	/*                                          */
	/* how many records will fit into a buffer? */
	/* --make this the buffer capacity:         */
	/*                                          */
	db->b.cap=MAXBUFSIZE/db->rl;

	if(nr>db->b.cap) eprintf(FATALFLAG,"SetBufferToSize(): too many records for buffer");
	/*                        */
	/* set number of records: */
	/*                        */
	db->b.nr=nr;
	/*                                      */
	/* now allocate or re-allocate storage: */
	/*                                      */
	bytes=db->b.nr*db->rl;
	db->b.r=(char *)realloc(db->b.r,bytes);
	if(db->b.r==NULL) eprintf(FATALFLAG,"SetBuffer() allocation failed");
	memset(db->b.r,' ',bytes);
}



/*
  FreeBuffer
  
  Frees record buffer previously allocated by SetBuffer
  and re-initializes other buffer variables to zero
  
*/
void FreeBuffer(PDBF db){

	/*                                              */
	/* free buffer block only if already allocated: */
	/*                                              */
	if(db->b.r!=NULL) free(db->b.r);
	db->b.cap=0;
	db->b.nr =0;
	db->b.fr =0;
	db->b.lr =0;
	return;
}


/*                                               */
/*  FillBuffer                                   */
/*                                               */
/*  Fills the record buffer with records.        */
/*  The operation implicitly moves the record    */
/*  pointer to one beyond the last record in the */
/*  buffer.                                      */
/*                                               */
/*  Returns number of records read which can be  */
/*  less than requested, depending on number of  */
/*  records still remaining before file end      */
/*                                               */
UINT32 FillBuffer(PDBF db,UINT32 fr,UINT32 nr){

	/*                                */
	/* Call base class GoTo function: */
	/*                                */
	GoTo(db,fr);
	
	db->b.nr=nr;                    /* desired number of records   */
	if(db->cr+db->b.nr > db->rc)    /* requested nr is too big ... */
		db->b.nr=db->rc-db->cr;     /* ... so adjust                  */

	if(fread(db->b.r,db->rl,db->b.nr,db->fp)!=db->b.nr) 
		eprintf(FATALFLAG,"FillBuffer() read error");
		
	db->cr=db->b.fr=fr;             /* first record in buffer */
	db->b.lr=db->b.fr+db->b.nr-1;   /* last record in buffer  */
	return db->b.nr;
}



/*                                                            */
/* GoTo(): BaseClass function:                                */
/*                                                            */
/* Go to the nth record: remember that the c-code counts from */
/* record "0" to n-1                                          */
/*                                                            */
int GoTo(PDBF db, UINT32 recno){

	UINT32 bytes;
	
	if(recno>=db->rc)
		eprintf(FATALFLAG,"GoTo(): attempt to go past end");
	bytes=db->rl*recno;
	fseek(db->fp,db->of+bytes,SEEK_SET);
	if(ftell(db->fp)-db->of==bytes){
		db->cr=recno;
		return 1;
	}else {
		eprintf(FATALFLAG,"Goto() failed");
		return 0;
	}
}


/*                                           */
/*      FreeDatabase(): Base class function: */
/*                                           */
/*      Releases storage for a DBF instance  */
/*      Works for all database types so far  */
/*                                           */
void FreeDatabase(PDBF db){

	/*                                       */
	/* Free the field subheader allocations: */
	/*                                       */
	switch(db->ty){
	case XBASE:
		if(db->fd.x) free(db->fd.x);
		break;
	case MBASE:
		if(db->fd.m) free(db->fd.m);
		break;
	case SASXPORT:
		if(db->fd.s) free(db->fd.s);
		break;
	}
	
	/*                             */
	/* free the buffer allocation: */
	/*                             */
	if(db->b.r) free(db->b.r);
	/*                                 */
	/* free the field flag allocation: */
	/*                                 */
	if(db->ff) free(db->ff);

	/*                             */
	/* free the field tree vector: */
	/*                             */
	if(db->ftree) FreeFieldTreeVector(db);
	/*                         */
	/* free the levels vector: */
	/*                         */
	if(db->levels) free(db->levels);
	/*                               */
	/* free the missingcount vector: */
	/*                               */
	if(db->missingcount) free(db->missingcount);

	/*                                                               */
	/* free the vector of PARRAY pointers to field label arrays:     */
	/* (The arrays themselves cannot be freed here because they      */
	/* are user arrays and can be referenced through the token tree) */
	/*                                                               */
	/* Hmmm ... this sounds like a trouble spot for memory errors!   */
	/* The right answer would be to remove the associative arrays    */
	/* themselves from the token tree ...                            */
	/*                                                               */
	/* FIX ME SOMEDAY * FIX THIS SOMEDAY *                           */
	/*                                                               */
	if(db->fllaa) free(db->fllaa); /* labels array         */
	if(db->flcaa) free(db->flcaa); /* colors array         */
	if(db->flgaa) free(db->flgaa); /* shades of gray array */
	
	/*                                 */
	/* free the FIELDINDEX allocation: */
	/*                                 */
	if(db->fi) FreeFieldIndex(db);
	
	/*                                     */
	/* free the marker allocation, if any: */
	/*                                     */
	if(db->m) FreeMarkerVector(db);
	
	/*                                       */
	/* free the NULLINFO vector, if present: */
	/*                                       */
	if(db->ni) free(db->ni);
	
	free(db);
}


/*                                                               */
/*      Skip() increments or decrements db->cr and automatically */
/*      refills the record buffer if the new record position is  */
/*      not in the buffer                                        */
/*                                                               */
int Skip(PDBF db, INT32 howmany){
	
	UINT32 newrec,maxsttrec;
	
		
	/*                    */
	/* record range check */
	/*                    */
	if(howmany<0 && (unsigned)(int)abs((int)howmany)>db->cr)
		eprintf(FATALFLAG,"Skip(): attempt to skip before beginning");
	newrec=db->cr+howmany;  
	if(newrec>db->rc-1)
		eprintf(FATALFLAG,"Skip(): attempt past end");
	/*                                           */
	/* automatically refill the buffer if needed */
	/*                                           */
	if(newrec<db->b.fr || newrec>db->b.lr){
		/*                                    */
		/* keep buffer as filled as possible: */
		/*                                    */
		maxsttrec=db->rc-db->b.cap;
		FillBuffer(db,(newrec>maxsttrec?maxsttrec:newrec),db->b.cap);
	}
	
	/* change db->cr: */
	db->cr=newrec;
	return 1;
}


/*                                             */
/*  XbaseFieldNumber                           */
/*                                             */
/*  Returns the number of the field whose name */
/*  matches the string parameter               */
/*  or MISSING if there was no match           */
/*                                             */
int XbaseFieldNumber(PDBF db,const char *s){

	int i;
	if(!db) 
		eprintf(FATALFLAG,"XbaseFieldNumber(): database must be opened prior to referencing field \"%s\"",s);
	for(i=0;i<db->fc;i++)
		if(strcmp(s,db->fd.x[i].nm)==0) return i;
	/*            */
	/* not found: */
	/*            */
	return MISSING;
}



/*                                             */
/*  MbaseFieldNumber                           */
/*                                             */
/*  Returns the number of the field whose name */
/*  matches the string parameter               */
/*  or MISSING if there was no match           */
/*                                             */
int MbaseFieldNumber(PDBF db,const char *s){

	int i;
	if(!db) 
		eprintf(FATALFLAG,"MbaseFieldNumber(): database must be opened prior to referencing field \"%s\"",s);
	for(i=0;i<db->fc;i++)
		if(strcmp(s,db->fd.m[i].nm)==0) return i;
	/*            */
	/* not found: */
	/*            */
	return MISSING;
}



/*                                             */
/*  SasXportFieldNumber                        */
/*                                             */
/*  Returns the number of the field whose name */
/*  matches the string parameter               */
/*  or MISSING if there was no match           */
/*                                             */
int SasXportFieldNumber(PDBF db,const char *s){

	int i;
	if(!db) 
		eprintf(FATALFLAG,"SasXportFieldNumber(): database must be opened prior to referencing field \"%s\"",s);
	for(i=0;i<db->fc;i++)
		if(strncmp(s,db->fd.s[i].nm,8)==0) return i;
	/*            */
	/* not found: */
	/*            */
	return MISSING;
}

/*                                                 */
/* CloseDatabase(): base-class implementation      */
/* works for all types of databases so far:        */
/*                                                 */
/* Closes the database file and frees allocations: */
/*                                                 */
void CloseDatabase(PDBF db){
	 if(db->fp) fclose(db->fp);
	 FreeDatabase(db);
}



/*                                               */
/* aton(): convert ln characters from s into     */
/*         a double.  This function allows one   */
/*         to pull data straight out of a flat   */
/*         file or xbase database buffer without */
/*         first copying n characters into a     */
/*         null-terminated string                */
/*                                               */
double aton(char *s,int ln){

	double dv;
	double mv;
	double sn;
	char *stt;
	char *dot;
	char *end;
	
	

	dv=0.0;
	mv=1.0;
	sn=0.0;
	stt=s;
	end=stt+ln-1;
	for(;stt<=end && *stt==' ';stt++);               /* skip white space */
	if(stt>end) return (double) DOUBLEMISSINGVALUE;  /* field empty      */

	for(;end>=stt && *end==' ';end--);               /* skip terminal white space     */
	for(dot=end;dot>=stt && *dot!='.';dot--,mv*=10); /* determine divisor             */
	if(*dot!='.' || dot<stt) mv=0;                  /* don't divide if no decimals    */
	if(dot==stt) return (double) DOUBLEMISSINGVALUE; /* nothing but a dot is present! */
	/*                                                                                */
	/* get value:                                                                     */
	/*                                                                                */
	if(*stt=='-') sn=-1.0;
	for(;stt<=end;stt++){
		if(*stt=='.' || *stt=='-' || *stt=='+') continue;
		dv=dv*10.0+(*stt-'0');
	}
	if(sn) dv*=sn;
	if(mv) dv/=mv;
	return dv;
}



/*                                                          */
/* atoni(): convert ln characters from s, which must be     */
/*          known to contain an integer without digits      */
/*          after a decimal point, to a 4-byte integer.     */
/*          (If s contains a decimal point, just use aton() */
/*          instead).                                       */
/*          This function allows one                        */
/*          to pull data straight out of a flat             */
/*          file or xbase database buffer without           */
/*          first copying n characters into a               */
/*          null-terminated string.                         */
/*                                                          */
INT32 atoni(char *s,int ln){

	INT32 iv;
	INT32 sn;

	char *stt;
	char *end;
	
	iv=0;
	sn=0;
	stt=s;
	end=stt+ln-1;
	for(;stt<=end && *stt==' ';stt++);                  /* skip white space */
	if(stt>end) return INT32MISSINGVALUE;               /* field empty      */

	for(;end>=stt && *end==' ';end--);                  /* skip terminal white space */
	if(*end=='.' && end==stt) return INT32MISSINGVALUE; /* nothing but a dot present */
	/*                                                                               */
	/* get value:                                                                    */
	/*                                                                               */
	if(*stt=='-') sn=-1;
	for(;stt<=end;stt++){
		if(*stt=='.' || *stt=='-' || *stt=='+') continue;
		iv=iv*10+(*stt-'0');
	}
	if(sn) iv*=sn;
	return iv;
}



/*                                                                     */
/*                                                                     */
/*      ReadXbaseInt32(): read a numeric field value as a long integer */
/*  db: the database                                                   */
/*  fn: the field number (0 -> N-1)                                    */
/*  rn: the record number                                              */
/*                                                                     */
INT32 ReadXbaseInt32(PDBF db,UINT16 fn,UINT32 rn){

	char   *ro;       /* offset into buffer to beginning of record            */
	char   *stt;      /* start of the field                                   */
	char   *nul;      /* start of the null information field in VFP databases */

	UINT32 msr;       /* maximum starting record for buffer */
	INT32  v;         /* value to return                    */
	double dv;        /* double value                       */
	
	
	/*                                        */
	/* Refill buffer if record not in range:  */
	/*                                        */
	if(rn<db->b.fr || rn>db->b.lr){
		/*                                     */
		/* keep buffer as filled as possible:  */
		/*                                     */
		msr=db->rc-db->b.cap;
		FillBuffer(db,(rn>msr?msr:rn),db->b.cap);
	}

	/*                                                                */
	/* ro    is the record offset into the buffer to the start of the */
	/*       record                                                   */
	/* stt   marks where the data start                               */
	/* nul   marks where the null information, if present, starts     */
	/*                                                                */
	ro=db->b.r+(rn-db->b.fr)*db->rl;

	/*                                         */
	/* Check for null value in a VFP database: */
	/*                                         */
	if(db->ni && db->ni[fn].hasnulls){
		nul=ro+db->fd.x[db->nfi].of+db->ni[fn].of;
		if(*nul & db->ni[fn].mask) return (INT32) INT32MISSINGVALUE;
	}

	/*             */
	/* read value: */
	/*             */
	stt=ro+db->fd.x[fn].of;

	switch(db->fd.x[fn].ty){
	
		case FMNUMERIC:
			if(db->fd.x[fn].pr){
				dv=aton(stt,db->fd.x[fn].ln);
				if(dv==DOUBLEMISSINGVALUE) v=INT32MISSINGVALUE;
				else v=(INT32)dv;
			}else{
				v=atoni(stt,db->fd.x[fn].ln);
			}
			break;
			
		case FMFLOAT:
			dv=aton(stt,db->fd.x[fn].ln);
			if(dv==DOUBLEMISSINGVALUE) v=INT32MISSINGVALUE;
			else v=(INT32)dv;
			break;
			
		case FMINTEGER:
			v=(INT32)*(INT32 *)stt;
			if(gle==M_BIG_ENDIAN) v=SignedSwapFourBytes(v);
			break;
		
		case FMDOUBLE:
			dv=*(double *)stt;
			if(gle==M_BIG_ENDIAN) dv=SwapEightBytes(dv);
			v=(INT32)dv;
			break;
			
		default:
			eprintf(FATALFLAG,"ReadXbaseInt32(): Unrecognized field type %c",db->fd.x[fn].ty);
		break;
	}
	return IsNMV((double)v)?INT32MISSINGVALUE:v;
}


/*                                                           */
/*      ReadDouble(): read a numeric field value as a double */
/*                                                           */
double ReadXbaseDouble(PDBF db,UINT16 fn,UINT32 rn){

	char   *ro;       /* offset into buffer to beginning of record            */
	char   *stt;      /* start of the field                                   */
	char   *nul;      /* start of the null information field in VFP databases */

	UINT32 msr;       /* maximum starting record for buffer */
	INT32  v;         /* int32 value                        */
	double dv;        /* double value                       */
	
	

	/*                                       */
	/* Refill buffer if record not in range: */
	/*                                       */
	if(rn<db->b.fr || rn>db->b.lr){
		/*                                    */
		/* keep buffer as filled as possible: */
		/*                                    */
		msr=db->rc-db->b.cap;
		FillBuffer(db,(rn>msr?msr:rn),db->b.cap);
	}

	/*                                                                */
	/* ro    is the record offset into the buffer to the start of the */
	/*       record                                                   */
	/* stt   marks where the data start                               */
	/* nul   marks where the null information, if present, starts     */
	/*                                                                */
	ro=db->b.r+(rn-db->b.fr)*db->rl;

	/*                                         */
	/* Check for null value in a VFP database: */
	/*                                         */
	if(db->ni && db->ni[fn].hasnulls){
		nul=ro+db->fd.x[db->nfi].of+db->ni[fn].of;
		if(*nul & db->ni[fn].mask) return (double) DOUBLEMISSINGVALUE;
	}

	/*             */
	/* read value: */
	/*             */
	stt=ro+db->fd.x[fn].of;

	switch(db->fd.x[fn].ty){
	
		case FMNUMERIC:
			dv=aton(stt,db->fd.x[fn].ln);
			break;
			
		case FMFLOAT:
			dv=aton(stt,db->fd.x[fn].ln);
			break;
			
		case FMINTEGER:
			v=(INT32)*(INT32 *)stt;
			if(gle==M_BIG_ENDIAN) v=SignedSwapFourBytes(v);
			dv=(double)v;
			break;
		
		case FMDOUBLE:
			dv=*(double *)stt;
			if(gle==M_BIG_ENDIAN) dv=SwapEightBytes(dv);
			break;

		default:
			eprintf(FATALFLAG,"ReadXbaseDouble(): Unrecognized field type %c",db->fd.x[fn].ty);
			break;
	}
	return IsNMV(dv)?DOUBLEMISSINGVALUE:dv; 
}


/*                                                            */
/*      ReadXbaseString(): read a character field and returns */
/*      the trimmed string in hold                            */
/*                                                            */
char *ReadXbaseString(PDBF db,char *hext,UINT16 fn,UINT32 rn){

	char   *hh;
	char   *rr;
	char   *stt;
	char   *end;
	UINT32 msr;
	char   *hold;
	static char hint[LOCALSTATICBUFFERSIZE];
	char   *ro;
	char   *nul;

	/*                                               */
	/* use local static storage if external is null: */
	/*                                               */
	hold=hext?hext:hint;

	/*                                            */
	/*      Refill buffer if record not in range: */
	/*                                            */
	if(rn<db->b.fr || rn>db->b.lr){
		/*                                    */
		/* keep buffer as filled as possible: */
		/*                                    */
		msr=db->rc-db->b.cap;
		FillBuffer(db,(rn>msr?msr:rn),db->b.cap);
	}

	/*                                                                */
	/* ro    is the record offset into the buffer to the start of the */
	/*       record                                                   */
	/* stt   marks where the data start                               */
	/* nul   marks where the null information, if present, starts     */
	/*                                                                */
	ro=db->b.r+(rn-db->b.fr)*db->rl;

	/*                                         */
	/* Check for null value in a VFP database: */
	/*                                         */
	if(db->ni && db->ni[fn].hasnulls){
		nul=ro+db->fd.x[db->nfi].of+db->ni[fn].of;
		if(*nul & db->ni[fn].mask){
			/*                                                            */
			/* below gives same result as strcpy(hold,CHARMISSINGSTRING); */
			/*                                                            */
			hold[0]='\0'; /* empty string                                 */
			return hold;
		}
	}

	/*             */
	/* read value: */
	/*             */
	stt=ro+db->fd.x[fn].of;
	end=stt+db->fd.x[fn].ln;

	/*                                           */
	/* left-trim string by skipping white space: */
	/*                                           */
	for(rr=stt;rr<end && *rr==' ';rr++);
	/*              */
	/* copy string: */
	/*              */
	for(hh=hold;rr<end;*hh++=*rr++);
	*hh='\0';
	if(hh>hold){
		/*                                               */   
		/* right trim the string above (the -- decrement */
		/* above insures we start at last letter,        */
		/* not at the null terminator)                   */
		/*                                               */   
		for(hh--;hh>hold && *hh==' ';*hh-- ='\0');
	}
	return IsCMV(hold)?(char *)CHARMISSINGSTRING:hold;
}


/*                                                                      */
/*      ReadXbaseDate(): read a database DATE field                     */
/*      and returns the date as a long int representing the Julian Day. */
/*      Note: this function is similar to a ReadString() function.      */
/*                                                                      */
/*                                                                      */
long ReadXbaseDate(PDBF db,UINT16 fn,UINT32 rn){

	char   *hh;
	char   *rr;
	char   *stt;
	char   *end;
	UINT32  msr;
	char    hold[9];  /* 8 characters in yyyymmdd form, plus terminator */
	char   *ro;
	char   *nul;
	int     dd;
	int     mm;
	int     yyyy;

	/*                                            */
	/*      Refill buffer if record not in range: */
	/*                                            */
	if(rn<db->b.fr || rn>db->b.lr){
		/*                                    */
		/* keep buffer as filled as possible: */
		/*                                    */
		msr=db->rc-db->b.cap;
		FillBuffer(db,(rn>msr?msr:rn),db->b.cap);
	}

	/*                                                                */
	/* ro    is the record offset into the buffer to the start of the */
	/*       record                                                   */
	/* stt   marks where the data start                               */
	/* nul   marks where the null information, if present, starts     */
	/*                                                                */
	ro=db->b.r+(rn-db->b.fr)*db->rl;

	/*                                         */
	/* Check for null value in a VFP database: */
	/*                                         */
	if(db->ni && db->ni[fn].hasnulls){
		nul=ro+db->fd.x[db->nfi].of+db->ni[fn].of;
		if(*nul & db->ni[fn].mask){
			return (long)MISSINGDATE;
		}
	}

	/*             */
	/* read value: */
	/*             */
	stt=ro+db->fd.x[fn].of;
	end=stt+db->fd.x[fn].ln;

	/*                                           */
	/* left-trim string by skipping white space: */
	/*                                           */
	for(rr=stt;rr<end && *rr==' ';rr++);
	/*              */
	/* copy string: */
	/*              */
	for(hh=hold;rr<end;*hh++=*rr++);
	*hh--='\0';
	/*                                               */
	/* right trim the string above (the -- decrement */
	/* above insures we start at last letter,        */
	/* not at the null terminator)                   */
	/*                                               */
	for(;hh>=hold && *hh==' ';*hh-- ='\0');
	/*                 */
	/* Check if empty: */
	/*                 */
	if(!*hold) return MISSINGDATE;
	/*                 */
	/* read day first: */
	/*                 */
	dd=atoi(hold+6);
	/*                                      */
	/* "knock off" day and then read month: */
	/*                                      */
	hold[6]='\0';
	mm=atoi(hold+4);
	/*                                            */
	/* "knock off" month too, and then read year: */
	/*                                            */
	hold[4]='\0';
	yyyy=atoi(hold);
	/*                                   */
	/* convert to Julian Day and return: */
	/*                                   */
	return JulianDay(mm,dd,yyyy);
}


/*                                                                      */
/*      ReadMbaseDate(): read an MBase DATE field                       */
/*      and returns the date as a long int representing the Julian Day. */
/*      Note: this function is similar to a ReadString() function.      */
/*                                                                      */
long ReadMbaseDate(PDBF db,UINT16 fn,UINT32 rn){

	extern PCONTROL g;
	
	char   *hh;
	char   *rr;
	char   *stt;
	char   *end;
	UINT32  msr;
	char    hold[11];  /* "DD.MM.YYYY" or similar format plus null terminator */
	char   *ro;
	int     dd;
	int     mm;
	int     yyyy;

	/*                                            */
	/*      Refill buffer if record not in range: */
	/*                                            */
	if(rn<db->b.fr || rn>db->b.lr){
		/*                                    */
		/* keep buffer as filled as possible: */
		/*                                    */
		msr=db->rc-db->b.cap;
		FillBuffer(db,(rn>msr?msr:rn),db->b.cap);
	}

	/*                                                                */
	/* ro    is the record offset into the buffer to the start of the */
	/*       record                                                   */
	/* stt   marks where the data start                               */
	/* nul   marks where the null information, if present, starts     */
	/*                                                                */
	ro=db->b.r+(rn-db->b.fr)*db->rl;

	/*             */
	/* read value: */
	/*             */
	stt=ro+db->fd.m[fn].of;
	end=stt+db->fd.m[fn].ln;

	/*                                            */
	/* left-trim string by skipping white space:  */
	/*                                            */
	for(rr=stt;rr<end && *rr==' ';rr++);
	/*              */
	/* copy string: */
	/*              */
	for(hh=hold;rr<end;*hh++=*rr++);
	*hh--='\0';
	/*                                               */
	/* right trim the string above (the -- decrement */
	/* above insures we start at last letter,        */
	/* not at the null terminator)                   */
	/*                                               */
	for(;hh>=hold && *hh==' ';*hh-- ='\0');
	/*                 */
	/* Check if empty: */
	/*                 */
	if(!*hold) return MISSINGDATE;
	/*                                                     */
	/* Interpretation of date depends on position of       */
	/* delimiters, if present, and language setting.       */
	/* If delimiter not present, then YYYYMMDD format      */
	/* is assumed; if delimiter is in 5th position,        */
	/* then YYYY/MM/DD format is assumed. Otherwise,       */
	/* if delimiter is in 3rd position, then the language  */
	/* setting determines whether to read as DD/MM/YYYY    */
	/* or MM/DD/YYYY format. When delimited, we always     */
	/* permit a wide range of delimiters:                  */
	/*                                                     */
	for(hh=hold;*hh && !(*hh=='/' || *hh==',' || *hh=='.' || *hh==' ' || *hh=='-');hh++);
	if(!*hh){
		if(hh-hold==8){
			/* This is the no-delimiter, "YYYYMMDD" case */
			/* Currently (20000507 ET), the Recognize    */
			/*                 */
			/* read day first: */
			/*                 */
			dd=atoi(hold+6);
			/*                                      */
			/* "knock off" day and then read month: */
			/*                                      */
			hold[6]='\0';
			mm=atoi(hold+4);
			/*                                            */
			/* "knock off" month too, and then read year: */
			/*                                            */
			hold[4]='\0';
			yyyy=atoi(hold);
		}else{
			return MISSINGDATE;
		}
	}else if(hh-hold==4){
		/* Assume "YYYY.MM.DD" or "YYYY/MM/DD" format   */
		/* We're going to allow a wide set of potential */
		/* delimiters:                                  */
		*hh='\0';
		yyyy=atoi(hold); /* grab year */
		rr=++hh;       /* go to start of month */
		for(;*hh && !(*hh=='/' || *hh==',' || *hh=='.' || *hh==' ' || *hh=='-');hh++);
		*hh='\0';
		mm=atoi(rr);
		rr=++hh;
		dd=atoi(rr);
	}else{
		/* "MM/DD/YYYY" or "DD.MM.YYYY": depends on language setting */
		if(g->lang==AMERICAN_ENGLISH){ /* AMERICAN assumes MM/DD/YYYY */
			*hh='\0';
			mm=atoi(hold);
			rr=++hh;       /* go to start of day */
			for(;*hh && !(*hh=='/' || *hh==',' || *hh=='.' || *hh==' ' || *hh=='-');hh++);
			*hh='\0';
			dd=atoi(rr);
		}else{
			*hh='\0';
			dd=atoi(hold);
			rr=++hh;       /* go to start of month */
			for(;*hh && !(*hh=='/' || *hh==',' || *hh=='.' || *hh==' ' || *hh=='-');hh++);
			*hh='\0';
			mm=atoi(rr);
		}
		rr=++hh;
		yyyy=atoi(rr);
	}
	/*                                         */
	/* Sanity checks on the dates:             */
	/* Actually, this is still too simplistic: */
	/*                                         */
	if(mm<1 || mm>12 || dd<1 || dd>31){
		return MISSINGDATE;
	}else{
		/*                                   */
		/* convert to Julian Day and return: */
		/*                                   */
		return JulianDay(mm,dd,yyyy);
	}
}


/*                                                                      */
/*      ReadSasXportDate(): read a database DATE field                  */
/*      and returns the date as a long int representing the Julian Day. */
/*      Note: this function is similar to a ReadString() function.      */
/*                                                                      */
long ReadSasXportDate(PDBF db,UINT16 fn,UINT32 rn){

	char   *ro;       /* offset into buffer to beginning of record */
	char   *stt;      /* start of the field                        */

	UINT32 msr;       /* maximum starting record for buffer */
	double dv;        /* double value                       */


	/*                                            */
	/* Refill buffer if record not in range:      */
	/*                                            */
	if(rn<db->b.fr || rn>db->b.lr){
		/*                                     */
		/* keep buffer as filled as possible:  */
		/*                                     */
		msr=db->rc-db->b.cap;
		FillBuffer(db,(rn>msr?msr:rn),db->b.cap);
	}

	/*                                                                */
	/* ro    is the record offset into the buffer to the start of the */
	/*       record                                                   */
	/* stt   marks where the data start                               */
	/* nul   marks where the null information, if present, starts     */
	/*                                                                */
	ro=db->b.r+(rn-db->b.fr)*db->rl;

	/*                                       */
	/* here's where the numeric value starts */
	/*                                       */
	stt=ro+db->fd.s[fn].of;
	/*                                                             */
	/* Check for missing values: not only must the first byte      */
	/* be one of the allowed SAS missing values, but the remaining */
	/* bytes must all be zero:                                     */
	/*                                                             */
	if( stt[0]==0x2E || 
	    stt[0]==0x5F || 
	   (stt[0]>=0x41 && stt[0]<=0x5A) &&
		stt[1]==0x00 &&
		stt[2]==0x00 &&
		stt[3]==0x00 &&
		stt[4]==0x00 &&
		stt[5]==0x00 &&
		stt[6]==0x00 &&
		stt[7]==0x00) return (INT32) INT32MISSINGVALUE;
	/*                      */
	/* read value directly: */
	/*                      */
	memcpy(&dv,stt,8);
	
	/*                                                         */
	/* Convert IBM mainframe floating point double to IEEE754: */
	/*                                                         */
	dv=IBMToIEEE754(dv);
	/*                                                         */
	/* Convert SAS Date to Julian day by adding the Julian day */
	/* for January 1, 1960 to the SAS date:                    */
	/*                                                         */
	dv+=2436935;
	/*                                */
	/* return value cast to long int: */
	/*                                */
	return (long int) dv;
}


/*                                                        */
/* NewSasXportField(): allocates vector of SASXPORTFIELDs */
/*                                                        */
PSASXPORTFIELD NewSasXportField(int n){

	PSASXPORTFIELD fd;
	
	fd=(PSASXPORTFIELD)malloc((unsigned)n*sizeof(SASXPORTFIELD));
	if(!fd) eprintf(FATALFLAG,"NewSasXportField(): Unable to allocate vector of %i field headers.",n);
	return fd;      
}


/*                                                       */
/* OpenSasXportDatabase(): Open a SAS TRANSPORT database */
/*                                                       */
#define SASFIELDNAMELENGTH 8
PDBF OpenSasXportDatabase(PDBF db){

	char ReadOnly[]="rb";
	char ReadWrite[]="rb+";
	
	PSASXPORTFIELD ff;      /* temporary pointer for reading subheaders */
	int         hs;
	int         bb;
	int i;
	OBSERVATIONHEADER oh;
	char *hh;
	
	long end;

	/*                                    */
	/* Data integrity comparison strings: */
	/*                                    */
	char h1s[]="HEADER RECORD*******LIBRARY HEADER RECORD!!!!!!!";
	char sas[]="SAS     ";
	char lib[]="SASLIB  ";
	char dat[]="SASDATA ";
	char mh1[]="HEADER RECORD*******MEMBER  HEADER RECORD!!!!!!!";
	char mh2[]="HEADER RECORD*******DSCRPTR HEADER RECORD!!!!!!!";
	char ohd[]="HEADER RECORD*******OBS     HEADER RECORD!!!!!!!";



	/*            */
	/* open file: */
	/*            */
	if(db->ty!=SASXPORT) eprintf(FATALFLAG,"OpenSasXportDatabase(): Flag not set to SASXPORT");   
	db->fp=fopen(db->nm,db->op==READONLY?ReadOnly:ReadWrite);
	if(!db->fp)
		eprintf(FATALFLAG,"OpenSasXportDatabase(): Unable to open %s.",db->nm);
	/*                                             */
	/* Read SASXPORTCOREHEADER structure directly: */
	/*                                             */
	if(fread(&db->hd.s,sizeof(SASXPORTCOREHEADER),1L,db->fp)!=1L)
		eprintf(FATALFLAG,"OpenSasXportDatabase(): Unable to read SASXPORTCOREHEADER in %s.",db->nm);
	
	/*                                    */
	/*                                    */
	/* Tests of transport file integrity: */
	/*                                    */
	/*                                    */
	if(strncmp(db->hd.s.h1.s,h1s,strlen(h1s)))
		eprintf(FATALFLAG,"OpensSasXportDatabase(): Inconsistency in %s which has:\n%-50s\n.\nA standard file has %s",db->nm,db->hd.s.h1.s,h1s);
	if(strncmp(db->hd.s.h2.sas1,sas,strlen(sas)))
		eprintf(FATALFLAG,"OpensSasXportDatabase(): Inconsistency in %s which has:\n%-50s\n.\nA standard file has %s",db->nm,db->hd.s.h2.sas1,sas);
	if(strncmp(db->hd.s.h2.sas2,sas,strlen(sas)))
		eprintf(FATALFLAG,"OpensSasXportDatabase(): Inconsistency in %s which has:\n%-50s\n.\nA standard file has %s",db->nm,db->hd.s.h2.sas2,sas);
	if(strncmp(db->hd.s.h2.lib,lib,strlen(lib)))
		eprintf(FATALFLAG,"OpensSasXportDatabase(): Inconsistency in %s which has:\n%-50s\n.\nA standard file has %s",db->nm,db->hd.s.h2.lib,lib);
	if(strncmp(db->hd.s.rh1.sas,sas,strlen(sas)))
		eprintf(FATALFLAG,"OpensSasXportDatabase(): Inconsistency in %s which has:\n:%-50s\n.\nA standard file has %s",db->nm,db->hd.s.rh1.sas,sas);
	if(strncmp(db->hd.s.rh1.sasdata,dat,strlen(dat)))
		eprintf(FATALFLAG,"OpensSasXportDatabase(): Inconsistency in %s which has:\n%-50s\n.\nA standard file has %s",db->nm,db->hd.s.rh1.sasdata,dat);
	if(strncmp(db->hd.s.mh1.pd,mh1,strlen(mh1)))
		eprintf(FATALFLAG,"OpensSasXportDatabase(): Inconsistency in %s which has:\n%-50s\n.\nA standard file has %s",db->nm,db->hd.s.mh1.pd,mh1);
	if(strncmp(db->hd.s.mh2.pd,mh2,strlen(mh2)))
		eprintf(FATALFLAG,"OpensSasXportDatabase(): Inconsistency in %s which has:\n%-50s\n.\nA standard file has %s",db->nm,db->hd.s.mh2.pd,mh2);
	
	
	/*                                                        */
	/* set the file subtype as being VAX/VMS if OS string     */
	/* makes reference to VAX or VMS:  Having not seen a SAS  */
	/* Transport file from VAX or VMS, I'm not sure what this */
	/* string really looks like:                              */
	/*                                                        */
	if(strncmp(db->hd.s.h2.os,"VAX/VMS ",8)==0) db->subtype=VMSSUBTYPE;
	
	/*                                                     */
	/* get variable (field) count: in order to read vc     */
	/* as a string, we plop down a null terminator in the  */
	/* fifth position of vc so we can read it as a string  */
	/*                                                     */
	db->hd.s.rh3.vc[4]='\0';
	db->fc=(unsigned short) atoi((char *)db->hd.s.rh3.vc);

	/*                                                               */
	/* allocate vector for field (NAMESTR==SASXPORTFIELD) subheaders */
	/*                                                               */
	/*                                                               */
	/* REGULAR IBM PC-type:                                          */
	/*                                                               */
	db->fd.s=NewSasXportField(db->fc);

	if(db->subtype==VMSSUBTYPE){
		/*                                                           */
		/* This read for VAX/VMS SAS transport files is untested:    */
		/* On VAX/VMS, the header is 4 bytes shorter, so we read 136 */
		/* bytes per instance into our 140 byte header, the last 4   */
		/* bytes being just filler anyway:                           */
		/*                                                           */
		for(i=0,ff=db->fd.s;i<db->fc;i++,ff++)
			if(fread(ff,sizeof(SASXPORTFIELD)-4,1L,db->fp)!=1L)
				eprintf(FATALFLAG,"OpensSasXportDatabase(): Error reading %ith VAX/VMS field subheader of %s.",i,db->nm);     

	}else{
		/*                                                                */
		/* For regular 140-byte header, heck, you don't need a loop, just */
		/* read in the whole chunk in one fell swoop ... swoosh !         */
		/*                                                                */
		if(fread(db->fd.s,(unsigned long)db->fc*sizeof(SASXPORTFIELD),1L,db->fp)!=1L)
			eprintf(FATALFLAG,"OpensSasXportDatabase(): Error reading %i regular field subheaders of %s.",db->fc,db->nm);
	}

	/*                                                                    */
	/* SAS transport files appear to always have ints and longs stored in */
	/* big-endian format.  Therefore, swapping is required if run on a    */
	/* little-endian system like an Intel PC:                             */
	/*                                                                    */
	/* byte swap all ints and longs in the headers if file came from      */
	/*                                                                    */
	if(!gle) gle=Endian();
	if(gle==M_LITTLE_ENDIAN){
		/*                                                           */
		/* system is little-endian, so need to swap those big-endian */
		/* ints and longs in the transport files:                    */
		/*                                                           */
		for(i=0;i<db->fc;i++){
			db->fd.s[i].ty   =SwapTwoBytes(db->fd.s[i].ty    );
			db->fd.s[i].ln   =SwapTwoBytes(db->fd.s[i].ln    );
			db->fd.s[i].hs   =SwapTwoBytes(db->fd.s[i].hs    );
			db->fd.s[i].fmtl =SwapTwoBytes(db->fd.s[i].fmtl  );
			db->fd.s[i].fmtp =SwapTwoBytes(db->fd.s[i].fmtp  );
			db->fd.s[i].fmtj =SwapTwoBytes(db->fd.s[i].fmtj  );
			db->fd.s[i].ifmtl=SwapTwoBytes(db->fd.s[i].ifmtl );
			db->fd.s[i].ifmtp=SwapTwoBytes(db->fd.s[i].ifmtp );
			db->fd.s[i].of   =SwapFourBytes(db->fd.s[i].of   );          
		}
	}

	/*                                                                   */
	/* The field names in SAS Transport field sub-headers are padded     */
	/* out to eight characters with ASCII 0x20 spaces, which we here     */
	/* change into null characters, '\0', in order to simplify           */
	/* string comparison in the SasXportFieldNumber() function.          */
	/*                                                                   */
	/* Of course, it is entirely possible, and in fact quite likely,     */
	/* that the name field, nm, will be completely filled out to the     */
	/* eighth character, leaving no room for a null terminator.  So,     */
	/* we also "cheat" big-time by stuffing a null terminator, '\0',     */
	/* into the first byte of the SUBSEQUENT field, which is the label   */
	/* field, lb, that won't be used in Madeline anyway.  Then, whenever */
	/* we cast nm to a character pointer, it all works fine.  I know     */
	/* it sounds bad, but everyone has to be bad once in a while!        */
	/*                                                                   */
	for(i=0;i<db->fc;i++){

		db->fd.s[i].lb[0]='\0'; 
		for(hh=db->fd.s[i].nm+SASFIELDNAMELENGTH-1;hh>=db->fd.s[i].nm && *hh==' ';*hh--='\0');

	}
		

	/*                                                           */
	/* the NAMESTR (i.e., SASXPORTFIELD) records                 */
	/* are terminally padded out to the end of                   */
	/* the next 80-byte segment into which the records have been */
	/* streamed, so here we read and discard any blanks:         */
	/*                                                           */
	hs=db->fc*sizeof(SASXPORTFIELD);      
	for(bb=0;bb<hs;bb+=80);
	bb-=hs;
	for(;bb;bb--) fgetc(db->fp);

	/*                                */
	/* next, read observation header: */
	/*                                */
	
	if(fread(&oh,sizeof(OBSERVATIONHEADER),1L,db->fp)!=1L)
		eprintf(FATALFLAG,"OpenSasTransportDatabase(): Error reading observation header in %s",db->nm);
	/*                                                          */
	/* one more header, so we can check integrity ad infinatum: */
	/*                                                          */
	if(strncmp(oh.s,ohd,strlen(ohd)))
		eprintf(FATALFLAG,"OpensSasXportDatabase(): Inconsistency in %s which has:\n%-50s\n.\nA standard file has %s",db->nm,oh.s,ohd);

	/*             */
	/* get db->of: */
	/*             */
	db->of=ftell(db->fp);               
	/*             */
	/* get db->rl: */
	/*             */
	db->rl=0;
	for(i=0;i<db->fc;i++) db->rl+=db->fd.s[i].ln;
	/*                                                                 */
	/* get db->rc:                                                     */
	/* Amazingly enough, the number of records does not appear         */
	/* to be stored anywhere in the header of a SAS transport file,    */
	/* so we use ftell() to find out the offset to the end of the      */
	/* file which, by the way, is for some reason padded out to make   */
	/* an integral number of 80-byte records, even though the actual   */
	/* data records can be of any length and are streamed together.    */
	/* Anyway, when we divide (offset_to_end - db->of) by db->rl,      */
	/* the integral result should equal the number of records present: */
	/*                                                                 */
	fseek(db->fp,0L,SEEK_END);
	/*                           */
	/* save the ending position: */
	/*                           */
	end=ftell(db->fp);
	/*                   */
	/* get record count: */
	/*                   */
	db->rc=(end-db->of)/db->rl;
		
	GoTo(db,0);
	
	/*                                   */
	/* allocate storage for field flags: */
	/*                                   */
	db->ff=NewFieldFlag(db->fc);

	/*                                            */
	/* allocate storage for FIELDINDEX structure: */
	/*                                            */
	db->fi=NewFieldIndex(db->fc);

	/*                                              */
	/* allocate storage for field PNODE head nodes: */
	/*                                              */
	db->ftree=NewFieldTree(db->fc);
	/*                                           */
	/* allocate storage for field levels vector: */
	/*                                           */
	db->levels=NewLevelsVector(db->fc);
	/*                                                 */
	/* allocate storage for field missingcount vector: */
	/*                                                 */
	db->missingcount=NewLevelsVector(db->fc);
	/*                                                 */
	/* Allocate storage for field labels array vector: */
	/*                                                 */
	db->fllaa=NewPointerToAssociativeArrayVector(db->fc);
	db->flcaa=NewPointerToAssociativeArrayVector(db->fc);
	db->flgaa=NewPointerToAssociativeArrayVector(db->fc);
	
	/*                                             */
	/* Now we're ready to buffer the data records: */
	/*                                             */
	SetBuffer(db);
	
	
	/*                                                                  */
	/* There ARE cases where the extra padding SAS throws onto the      */
	/* end of the file exceeds the record length, so we have to back    */
	/* track from the end to make sure we haven't accidently calculated */
	/* more records than there really are:                              */
	/*                                                                  */
	for(;;){
		/*                                      */
		/* put the last record into the buffer: */
		/*                                      */
		FillBuffer(db,db->rc-1,1);
		/*                   */
		/* is it all blanks? */
		/*                   */
		for(i=0;i<db->rl && db->b.r[i]==' ';i++);
		if(i==db->rl){
			db->rc--;
		}else{
			break;
		}
	}
	
	
	/*                                     */
	/* Fill the buffer up to its capacity, */
	/* or the whole database, whichever is */
	/* smaller:                            */
	/*                                     */
	/*FillBuffer(db,0,db->b.cap);	       */
	
	return db;
}


/*                                                                  */
/* ReadSasXportInt32(): read a numeric variable as an INT32 integer */
/*                                                                  */
INT32  ReadSasXportInt32(PDBF db,UINT16 fn,UINT32 rn){

	char   *ro;       /* offset into buffer to beginning of record */
	char   *stt;      /* start of the field                        */

	UINT32 msr;       /* maximum starting record for buffer */
	double dv;        /* double value                       */


	/*                                            */
	/* Refill buffer if record not in range:      */
	/*                                            */
	if(rn<db->b.fr || rn>db->b.lr){
		/*                                     */
		/* keep buffer as filled as possible:  */
		/*                                     */
		msr=db->rc-db->b.cap;
		FillBuffer(db,(rn>msr?msr:rn),db->b.cap);
	}

	/*                                                                */
	/* ro    is the record offset into the buffer to the start of the */
	/*       record                                                   */
	/* stt   marks where the data start                               */
	/* nul   marks where the null information, if present, starts     */
	/*                                                                */
	ro=db->b.r+(rn-db->b.fr)*db->rl;

	/*                                       */
	/* here's where the numeric value starts */
	/*                                       */
	stt=ro+db->fd.s[fn].of;
	/*                                                             */
	/* Check for missing values: not only must the first byte      */
	/* be one of the allowed SAS missing values, but the remaining */
	/* bytes must all be zero:                                     */
	/*                                                             */
	if( stt[0]==0x2E || 
	    stt[0]==0x5F || 
	   (stt[0]>=0x41 && stt[0]<=0x5A) &&
		stt[1]==0x00 &&
		stt[2]==0x00 &&
		stt[3]==0x00 &&
		stt[4]==0x00 &&
		stt[5]==0x00 &&
		stt[6]==0x00 &&
		stt[7]==0x00) return (INT32) INT32MISSINGVALUE;
	/*                      */
	/* read value directly: */
	/*                      */
	memcpy(&dv,stt,8);
	
	/*                                                         */
	/* Convert IBM mainframe floating point double to IEEE754: */
	/*                                                         */
	dv=IBMToIEEE754(dv);
	
	return IsNMV(dv)?INT32MISSINGVALUE:(INT32)dv;
	
}


/*                                                         */
/* ReadSasXportDouble(): read a numeric variable as double */
/*                                                         */
double ReadSasXportDouble(PDBF db,UINT16 fn,UINT32 rn){

	char   *ro;       /* offset into buffer to beginning of record */
	char   *stt;      /* start of the field                        */

	UINT32 msr;       /* maximum starting record for buffer */
	double dv;        /* double value                       */


	/*                                        */
	/* Refill buffer if record not in range:  */
	/*                                        */
	if(rn<db->b.fr || rn>db->b.lr){
		/*                                     */
		/* keep buffer as filled as possible:  */
		/*                                     */
		msr=db->rc-db->b.cap;
		FillBuffer(db,(rn>msr?msr:rn),db->b.cap);
	}

	/*                                                                */
	/* ro    is the record offset into the buffer to the start of the */
	/*       record                                                   */
	/* stt   marks where the data start                               */
	/* nul   marks where the null information, if present, starts     */
	/*                                                                */
	ro=db->b.r+(rn-db->b.fr)*db->rl;

	/*                                       */
	/* here's where the numeric value starts */
	/*                                       */
	stt=ro+db->fd.s[fn].of;
	/*                                                             */
	/* Check for missing values: not only must the first byte      */
	/* be one of the allowed SAS missing values, but the remaining */
	/* bytes must all be zero:                                     */
	/*                                                             */
	if( stt[0]==0x2E || 
	    stt[0]==0x5F || 
	   (stt[0]>=0x41 && stt[0]<=0x5A) &&
		stt[1]==0x00 &&
		stt[2]==0x00 &&
		stt[3]==0x00 &&
		stt[4]==0x00 &&
		stt[5]==0x00 &&
		stt[6]==0x00 &&
		stt[7]==0x00) return (double) DOUBLEMISSINGVALUE;
	/*                      */
	/* read value directly: */
	/*                      */
	memcpy(&dv,stt,8);
	
	/*                                                         */
	/* Convert IBM mainframe floating point double to IEEE754: */
	/*                                                         */
	dv=IBMToIEEE754(dv);
	
	return IsNMV(dv)?DOUBLEMISSINGVALUE:dv;
	
}


/*                                                            */
/* ReadSasXportString(): read a string (character) variable   */
/* from a SAS transport file.  If hext is not NULL, then      */
/* the characters are copied into the hext external buffer    */
/* and null-terminated.  Otherwise, hint, the internal buffer */
/* is used.                                                   */
/*                                                            */
char * ReadSasXportString(PDBF db,char *hext,UINT16 fn,UINT32 rn){

	char   *hh;
	char   *rr;
	char   *stt;
	char   *end;
	UINT32 msr;
	char   *hold;
	static char hint[LOCALSTATICBUFFERSIZE];
	char   *ro;

	/*                                               */
	/* use local static storage if external is null: */
	/*                                               */
	hold=hext?hext:hint;

	/*                                            */
	/*      Refill buffer if record not in range: */
	/*                                            */
	if(rn<db->b.fr || rn>db->b.lr){
		/*                                    */
		/* keep buffer as filled as possible: */
		/*                                    */
		msr=db->rc-db->b.cap;
		FillBuffer(db,(rn>msr?msr:rn),db->b.cap);
	}

	/*                                                                */
	/* ro    is the record offset into the buffer to the start of the */
	/*       record                                                   */
	/* stt   marks where the data start                               */
	/* nul   marks where the null information, if present, starts     */
	/*                                                                */
	ro=db->b.r+(rn-db->b.fr)*db->rl;

	/*             */
	/* read value: */
	/*             */
	stt=ro+db->fd.s[fn].of;
	end=stt+db->fd.s[fn].ln;

	/* left-trim string by skipping white space: */
	for(rr=stt;rr<end && *rr==' ';rr++);
	/*              */
	/* copy string: */
	/*              */
	for(hh=hold;rr<end;*hh++=*rr++);
	*hh='\0';
	if(hh>hold){
		/* right trim the string above (the -- decrement */
		/* above insures we start at last letter,        */
		/* not at the null terminator)                   */
		for(hh--;hh>hold && *hh==' ';*hh-- ='\0');
	}
	return IsCMV(hold)?(char *)CHARMISSINGSTRING:hold;
}


/*                      */
/*                      */
/* OpenMbaseDatabase(): */
/*                      */
/*                      */
PDBF   OpenMbaseDatabase(PDBF db){

	int  i;
	char ReadOnly[]="rb";
	char ReadWrite[]="rb+";
	UFLAG8 fendian=0;
	
	/*            */
	/* open file: */
	/*            */
	if(db->ty!=MBASE) eprintf(FATALFLAG,"OpenMbaseDatabase(): Flag not set to MBASE");    
	db->fp=fopen(db->nm,db->op==READONLY?ReadOnly:ReadWrite);
	if(db->fp==NULL)
		eprintf(FATALFLAG,"OpenMbaseDatabase(): Unable to open file %s",db->nm);

	/*                                          */
	/* read MBASECOREHEADER structure directly: */
	/*                                          */
	if(fread(&db->hd.m,sizeof(MBASECOREHEADER),1L,db->fp)!=1L)
		eprintf(FATALFLAG,"OpenMbaseDatabase(): Unable to read MBASECOREHEADER in %s",db->nm);


	/*                                                       */
	/* Determine if it is an Mbase flat file header subtype: */
	/*                                                       */
	if(strcmp(db->hd.m.mtype,MFLATSTR)==0){
		db->subtype=MFFSUBTYPE;
		/*                                                   */
		/* See if ffn, the attached flat file, is available  */
		/* and can be opened:                                */
		/*                                                   */
		db->fp2=fopen(db->hd.m.ffn,db->op==READONLY?ReadOnly:ReadWrite);
		if(!db->fp2)
			eprintf(FATALFLAG,"OpenMbaseDatabase(): Unable to open associated flat file %s",db->hd.m.ffn);
		
	}else if(strcmp(db->hd.m.mtype,MBASESTR)!=0)
		eprintf(FATALFLAG,"OpenMbaseDatabase(): The \"mtype\" of this Madeline database, %s, is corrupt",db->nm);
	
	/*                                                  */
	/* Determine endianness of file relative to system: */
	/*                                                  */
	if(!gle) gle=Endian();
	if(strcmp(db->hd.m.endian,LILENDIANSTR)==0) fendian=M_LITTLE_ENDIAN;
	else if(strcmp(db->hd.m.endian,BIGENDIANSTR)==0) fendian=M_BIG_ENDIAN;
	else
		eprintf(FATALFLAG,"Can't determine endianess of data file, %s",db->nm);
	/*                                                           */
	/* set swapper flag if file was produced on different-endian */
	/* operating system, and swap core header elements:          */
	/*                                                           */
	if(gle!=fendian){
		db->swap=1;
		db->hd.m.fc=SwapTwoBytes(db->hd.m.fc);
		db->hd.m.ln=SwapTwoBytes(db->hd.m.ln);
		db->hd.m.rc=SwapFourBytes(db->hd.m.rc);
		db->hd.m.of=SwapTwoBytes(db->hd.m.of);
	}

	/*                                                          */
	/* place record count, offset to first record, field count, */
	/* and record length into generically-accessible spot:      */
	/* for "base class" function access:                        */
	/*                                                          */
	db->fc=db->hd.m.fc;
	db->rl=db->hd.m.ln;
	db->rc=db->hd.m.rc;
	db->of=db->hd.m.of;
	
	/*                              */
	/* allocate storage for fields: */
	/*                              */
	db->fd.m=(PMBASEFIELD)malloc((unsigned)sizeof(MBASEFIELD)*db->fc);
	if(db->fd.m==NULL)
		eprintf(FATALFLAG,"OpenMbaseDatabase(): No memory for MBASEFIELDs");

	/*                                  */
	/* directly read field information: */
	/*                                  */
	if(fread(db->fd.m,sizeof(MBASEFIELD),db->fc,db->fp)!=(size_t)db->fc)
		eprintf(FATALFLAG,"OpenMbaseDatabase(): Error reading MFIELD subheaders");

	/*                                                  */
	/* swap the field subheader integers, if necessary: */
	/*                                                  */
	if(db->swap){
		for(i=0;i<db->fc;i++){
			db->fd.m[i].of=SwapFourBytes(db->fd.m[i].of);
			db->fd.m[i].ln=SwapTwoBytes(db->fd.m[i].ln);
		}
	}
	
	/*                                   */
	/* allocate storage for field flags: */
	/*                                   */
	db->ff=NewFieldFlag(db->fc);

	/*                                            */
	/* allocate storage for FIELDINDEX structure: */
	/*                                            */
	db->fi=NewFieldIndex(db->fc);

	/*                                              */
	/* allocate storage for field PNODE head nodes: */
	/*                                              */
	db->ftree=NewFieldTree(db->fc);
	/*                                           */
	/* allocate storage for field levels vector: */
	/*                                           */
	db->levels=NewLevelsVector(db->fc);
	/*                                                 */
	/* allocate storage for field missingcount vector: */
	/*                                                 */
	db->missingcount=NewLevelsVector(db->fc);
	/*                                                 */
	/* Allocate storage for field labels array vector: */
	/*                                                 */
	db->fllaa=NewPointerToAssociativeArrayVector(db->fc);
	db->flcaa=NewPointerToAssociativeArrayVector(db->fc);
	db->flgaa=NewPointerToAssociativeArrayVector(db->fc);
	
	/*                                                             */
	/* For the flat file subtype, we need to close the .mfh header */
	/* file, and use db->fp2 as the file pointer from which to     */
	/* fill up the database buffer:                                */
	/*                                                             */
	if(db->subtype==MFFSUBTYPE){
		fclose(db->fp);
		db->fp=db->fp2;
	}
	/*                                                                  */
	/* If this is the MBASE FLAT FILE subtype, we need to               */
	/* check, and possibly adjust the record length, taking into        */
	/* consideration that some systems use 2 bytes "\r\n" to indicate   */
	/* EOL (i.e., DOS), while others use just one "\n" (i.e., Unix)     */
	/* -- this depends on the system on which the flat file was         */
	/* created, but could easily change as the flat file gets FTPed     */
	/* onto other systems, etc.  Note we definitely must do this before */
	/* setting up the buffer and filling it!                            */
	/*                                                                  */
	if(db->subtype==MFFSUBTYPE) CheckMBaseRecordLength(db);
	
	/*                                                */
	/* set up buffer via call to base class function: */
	/*                                                */
	SetBuffer(db);          
	/*                                                            */
	/* Fill the buffer up to its capacity, or the whole database, */
	/* whichever is smaller:                                      */
	/*                                                            */
	FillBuffer(db,0,db->b.cap);
	
	return db;
	
}



/*                   */
/*                   */
/* ReadMbaseDouble() */
/*                   */
/*                   */
double ReadMbaseDouble(PDBF db,UINT16 fn,UINT32 rn){

	char   *ro;       /* offset into buffer to beginning of record */
	char   *stt;      /* start of the field                        */

	UINT32 msr;       /* maximum starting record for buffer */
	INT32  iv;        /* integer value                      */
	double dv;        /* double value                       */
	
	/*                                        */
	/* Refill buffer if record not in range:  */
	/*                                        */
	if(rn<db->b.fr || rn>db->b.lr){
		/*                                     */
		/* keep buffer as filled as possible:  */
		/*                                     */
		msr=db->rc-db->b.cap;
		FillBuffer(db,(rn>msr?msr:rn),db->b.cap);
	}

	/*                                                                */
	/* ro    is the record offset into the buffer to the start of the */
	/*       record                                                   */
	/* stt   marks where the data start                               */
	/* nul   marks where the null information, if present, starts     */
	/*                                                                */
	ro=db->b.r+(rn-db->b.fr)*db->rl;


	/*             */
	/* read value: */
	/*             */
	stt=ro+db->fd.m[fn].of;

	switch(db->fd.m[fn].ty){
	

		case MDOUBLE:
			dv= *(double *)stt;
			if(db->swap) dv=SwapEightBytes(iv);
			break;

		case MFLATDBL:
			dv=aton(stt,db->fd.m[fn].ln);
			break;
			
		case MINT32:
			iv= *(INT32 *)stt;
			if(db->swap) iv=SignedSwapFourBytes(iv);
			dv=(double)iv;
			break;
			
		case MFLATINT:
			iv=atoni(stt,db->fd.m[fn].ln);
			if(iv==INT32MISSINGVALUE) dv=DOUBLEMISSINGVALUE;
			else dv=(double)iv;
			break;
			
		default:
			eprintf(FATALFLAG,"ReadMbaseDouble(): Field type %4x not recognized by this function",db->fd.m[fn].ty);
			break;
	}
	return IsNMV(dv)?DOUBLEMISSINGVALUE:dv;
}

/*                   */
/*                   */
/* ReadMbaseInt32(): */
/*                   */
/*                   */
INT32  ReadMbaseInt32(PDBF db,UINT16 fn,UINT32 rn){

	char   *ro;       /* offset into buffer to beginning of record */
	char   *stt;      /* start of the field                        */

	UINT32 msr;       /* maximum starting record for buffer */
	INT32  iv;        /* integer value                      */
	double dv;        /* double value                       */
	
	/*                                            */
	/* Refill buffer if record not in range:      */
	/*                                            */
	if(rn<db->b.fr || rn>db->b.lr){
		/*                                     */
		/* keep buffer as filled as possible:  */
		/*                                     */
		msr=db->rc-db->b.cap;
		FillBuffer(db,(rn>msr?msr:rn),db->b.cap);
	}

	/*                                                                */
	/* ro    is the record offset into the buffer to the start of the */
	/*       record                                                   */
	/* stt   marks where the data start                               */
	/* nul   marks where the null information, if present, starts     */
	/*                                                                */
	ro=db->b.r+(rn-db->b.fr)*db->rl;


	/*             */
	/* read value: */
	/*             */
	stt=ro+db->fd.m[fn].of;

	switch(db->fd.m[fn].ty){
	

		case MINT32:
			iv= *(INT32 *)stt;
			if(db->swap) iv=SignedSwapFourBytes(iv);
			break;

		case MFLATINT:
			iv=atoni(stt,db->fd.m[fn].ln);
			break;
			
		case MDOUBLE:
			dv= *(double *)stt;
			if(db->swap) dv=SwapEightBytes(iv);
			iv=(INT32) dv;
			break;

		case MFLATDBL:
			dv=aton(stt,db->fd.x[fn].ln);
			if(dv==DOUBLEMISSINGVALUE) iv=INT32MISSINGVALUE;
			else iv=(INT32)dv;
			break;
			
		default:
			eprintf(FATALFLAG,"ReadMbaseInt32(): Field type %4x not recognized by this function",db->fd.m[fn].ty);
			break;
	}
	return IsNMV(iv)?INT32MISSINGVALUE:iv;
}


/*                    */
/*                    */
/* ReadMbaseString(): */
/*                    */
/*                    */
char * ReadMbaseString(PDBF db,char *hext,UINT16 fn,UINT32 rn){

	char   *hh;
	char   *rr;
	char   *stt;
	char   *end;
	UINT32 msr;
	char   *hold;
	static char hint[LOCALSTATICBUFFERSIZE];
	char   *ro;

	/*                                               */
	/* use local static storage if external is null: */
	/*                                               */
	hold=hext?hext:hint;

	/*                                            */
	/*      Refill buffer if record not in range: */
	/*                                            */
	if(rn<db->b.fr || rn>db->b.lr){
		/*                                    */
		/* keep buffer as filled as possible: */
		/*                                    */
		msr=db->rc-db->b.cap;
		FillBuffer(db,(rn>msr?msr:rn),db->b.cap);
	}

	/*                                                                */
	/* ro    is the record offset into the buffer to the start of the */
	/*       record                                                   */
	/* stt   marks where the data start                               */
	/* nul   marks where the null information, if present, starts     */
	/*                                                                */
	ro=db->b.r+(rn-db->b.fr)*db->rl;

	/*             */
	/* read value: */
	/*             */
	stt=ro+db->fd.m[fn].of;
	end=stt+db->fd.m[fn].ln;

	/* left-trim string by skipping white space: */
	for(rr=stt;rr<end && *rr==' ';rr++);
	/*              */
	/* copy string: */
	/*              */
	for(hh=hold;rr<end;*hh++=*rr++);
	*hh='\0';
	if(hh>hold){
		/* right trim the string above (the -- decrement */
		/* above insures we start at last letter,        */
		/* not at the null terminator)                   */
		for(hh--;hh>hold && *hh==' ';*hh-- ='\0');
	}
	/*                                                        */
	/* Special case where field type is 'C' and extended type */
	/* is 'G': place a slash if a slash is not present:       */
	/*                                                        */
	/* When a slash is not present, the two numeric allele    */
	/* labels are assumed to be separated by one or more      */
	/* spaces.  A slash is added, and the extra spaces are    */
	/* closed up.                                             */
	/*                                                        */
	if(db->fd.m[fn].et=='G'){
		/*                        */
		/* search for '/' in hold */
		/*                        */
		for(hh=hold;*hh && *hh!='/';hh++);
		if(*hh!='/'){
			/*                                             */
			/* we assume that there are two numbers        */
			/* separated by spaces: add slash after first: */
			/*                                             */
			for(hh=hold;*hh && isdigit(*hh);hh++);
			if(*hh==' ') *hh++='/';
			/*                                                         */
			/* now trim any other spaces on right numeric allele label */
			/*                                                         */
			for(rr=hh;*rr && *rr==' ';rr++);
			for(;*rr;*hh++=*rr++);
			*hh='\0';
		}
	}
	return IsCMV(hold)?(char *)CHARMISSINGSTRING:hold;
}


/*                                    */
/* SwapEightBytes(): swap eight bytes */
/*                                    */
/*                                    */
double SwapEightBytes(double uia){

	double uib=0;
	char *a,*b;
	int i,size=8;

	/*                                                                                                          */
	/*if((size=sizeof(uia))!=8) eprintf(FATALFLAG,"SwapEightBytes(): code must be modified for this platform"); */
	/*                                                                                                          */
	a=(char *)&uia;
	b=(char *)&uib;
	b+=(size-1);
	for(i=0;i<size;i++) *b--=*a++;
	return uib;
}



/*                                  */
/* SwapFourBytes() Swaps four bytes */
/*                                  */
UINT32 SwapFourBytes(UINT32 uia){

	UINT32 uib=0;
	char *a,*b;
	int i,size=4;

	/*                                                                                                          */
	/* if((size=sizeof(uib))!=4) eprintf(FATALFLAG,"SwapFourBytes(): code must be modified for this platform"); */
	/*                                                                                                          */
	a=(char *)&uia;
	b=(char *)&uib;
	b+=(size-1);
	for(i=0;i<size;i++) *b--=*a++;
	return uib;
	
}



/*                                        */
/* SignedSwapFourBytes() Swaps four bytes */
/*                                        */
INT32 SignedSwapFourBytes(INT32 uia){

	INT32 uib=0;
	char *a,*b;
	int i,size=4;

	/*                                                                                                          */
	/* if((size=sizeof(uib))!=4) eprintf(FATALFLAG,"SwapFourBytes(): code must be modified for this platform"); */
	/*                                                                                                          */
	a=(char *)&uia;
	b=(char *)&uib;
	b+=(size-1);
	for(i=0;i<size;i++) *b--=*a++;
	return uib;
	
}



/*                */
/* Swap Two Bytes */
/*                */
UINT16 SwapTwoBytes(UINT16 uia){

	UINT16 uib=0;
	char *a,*b;
	int i,size=2;

	/*                                                                                                         */
	/* if((size=sizeof(uib))!=2) eprintf(FATALFLAG,"SwapTwoBytes(): code must be modified for this platform"); */
	/*                                                                                                         */
	a=(char *)&uia;
	b=(char *)&uib;
	b+=(size-1);
	for(i=0;i<size;i++) *b--=*a++;
	return uib;
	
}


/*                  */
/* XbaseFieldName() */
/*                  */
char *XbaseFieldName(PDBF db,UINT16 idx){

	return db->fd.x[idx].nm;

}

/*                 */
/* XbaseFieldType: */
/*                 */
char XbaseFieldType(PDBF db,UINT16 idx){

	switch(db->fd.x[idx].ty){

		case 'C':
			return 'C';

		case 'N':
		case 'F':
		case 'I':
		case 'B':
			return 'N';
		
		case 'D':
			return 'D';
		
		case '0':
			/*                                                         */
			/* This is the Visual Foxpro field flags field which       */
			/* encodes bit flags for fields that are null.  In order   */
			/* to prevent problems elsewhere in Madeline, this field   */
			/* is temporarily treated as a 'C' field, but users should */
			/* not perform operations on this field:                   */
			/*                                                         */
			return 'C';

		default:
			eprintf(FATALFLAG,"XbaseFieldType(): Operations on field type \"%c\" (0x%4x) are not supported.",db->fd.x[idx].ty,db->fd.x[idx].ty);
			break;
	}
	return '\0';
}

/*                     */
/* XbaseFieldLength(): */
/*                     */
UINT16 XbaseFieldLength(PDBF db,UINT16 idx){

	return db->fd.x[idx].ln;

}

/*                        */
/* XbaseFieldPrecision(): */
/*                        */
UINT8 XbaseFieldPrecision(PDBF db,UINT16 idx){

	return db->fd.x[idx].pr;
	
}


/*                   */
/* MbaseFieldName(): */
/*                   */
char *MbaseFieldName(PDBF db,UINT16 idx){

	return db->fd.m[idx].nm;

}

/*                   */
/* MbaseFieldType(): */
/*                   */
char MbaseFieldType(PDBF db,UINT16 idx){

	switch(db->fd.m[idx].ty){

		case MSTRING:
			return 'C';

		case MINT32:
		case MDOUBLE:
		case MFLATINT:
		case MFLATDBL:
			return 'N';
		
		case MDATE:
			return 'D';

		default:
			eprintf(FATALFLAG,"MbaseFieldType(): Operations on field type \"%c\" (0x%4x) are not supported.",db->fd.m[idx].ty,db->fd.m[idx].ty);  
			break;
	}
	return '\0';
}


/*                     */
/* MbaseFieldLength(): */
/*                     */
UINT16 MbaseFieldLength(PDBF db,UINT16 idx){

	return db->fd.m[idx].ln;

}


/*                        */
/* MbaseFieldPrecision(): */
/*                        */
UINT8 MbaseFieldPrecision(PDBF db,UINT16 idx){

	return db->fd.m[idx].pr;
	
}


/*                                                                        */
/* SasXportFieldName(): The 'nm' element                                  */
/* in the SAS header is not really a NULL-terminated string,              */
/* but we cheat by putting a '\0' null termination character into         */
/* the first byte of the subsequent field in the SASXPORTFIELD structure. */
/* That field is the lb, or label field, which we are not going to use    */
/* in Madeline anyway, so this works great:                               */
/*                                                                        */
/*                                                                        */
char *SasXportFieldName(PDBF db,UINT16 idx){
	
	return (char *) db->fd.s[idx].nm;
	
}

/*                      */
/* SasXportFieldType(): */
/*                      */
char SasXportFieldType(PDBF db,UINT16 idx){

	switch(db->fd.s[idx].ty){

		case SASXPORTCHARACTER:
			return 'C';

		case SASXPORTNUMERIC:
			/*                                                        */
			/* Madeline breaks out DATE variables as being a distinct */
			/* type from other numeric variables:                     */
			/*                                                        */
			if(!strncmp(db->fd.s[idx].fmt,"DATE    ",8)){
				/*              */
				/* Date format: */
				/*              */
				return 'D';
			}else
				/*                  */
				/* generic numeric: */
				/*                  */
				return 'N';
		
		default:
			eprintf(FATALFLAG,"SasXportFieldType(): Operations on field type \"%c\" (0x%4x) are not supported.",db->fd.s[idx].ty,db->fd.s[idx].ty);       
			break;
	}
	return '\0';
}


/*                        */
/* SasXportFieldLength(): */
/*                        */
UINT16 SasXportFieldLength(PDBF db,UINT16 idx){
	
	if(db->fd.s[idx].ty==SASXPORTCHARACTER){
		return db->fd.s[idx].ln;

	}else{
		/*                */
		/* numeric field: */
		/*                */
		if(db->fd.s[idx].fmtl)
			return db->fd.s[idx].fmtl;
		else
			return (UINT16) SASNUMERICDEFAULTLENGTH;
	
	}
}


/*                           */
/* SasXportFieldPrecision(): */
/*                           */
UINT8 SasXportFieldPrecision(PDBF db,UINT16 idx){

	/*                                                             */
	/* if "fmtl" field length is specified, then                   */
	/* return field precision, which could, of course, be          */
	/* zero.  Otherwise, return the default precision which        */
	/* is paired with the default length returned by the preceding */
	/* SasXportFieldLength() function:                             */
	/*                                                             */
	if(db->fd.s[idx].fmtl)
		return db->fd.s[idx].fmtp;
	else
		return (UINT16) SASNUMERICDEFAULTPRECISION;
		
}

/*                                                             */
/*                                                             */
/* IBMToIEEE754(): Convert IBM mainframe floating point double */
/*                 to IEEE754 floating point double            */
/*                                                             */
double IBMToIEEE754(double ibmv){

	double ieee=0.0;

	UCHAR8 *p =(UCHAR8 *)&ibmv;

	char sigm=0x80;   /* 1000 0000 sign bit mask         */
	char expm=0x7F;   /* 0111 1111 exponent integer mask */
	
	char lnm =0xF0;   /* 1111 0000 left nibble mask  */
	char rnm =0x0F;   /* 0000 1111 right nibble mask */
	
	int    bexp=-1;     /* base exponent                            */
	int    aexp;        /* absolute value of exponent               */
	int    sexp;        /* sign of exponent (1=negative,0=positive) */
	UCHAR8 nv;          /* nibble value                             */
	double bmult=1.0;   /* base multiplier                          */
	int i;
	
	sigm &= p[0];     /* get sign          */
	expm &= p[0];     /* get exponent+64   */
	expm -= 64;       /* adjusted exponent */
	
	
	bexp+=expm;       /* calculate base exponent */
	if(bexp<0){
		sexp=1;
		aexp=bexp*-1;
	}else{
		sexp=0;
		aexp=bexp;
	}
	
	for(i=0;i<aexp;i++) bmult*=16;  /* calculate base multiplier */
	
	for(i=1;i<8;i++){

		/*                         */
		/* operate on left nibble: */
		/*                         */
		
		nv=(p[i]&lnm)>>4;          /* left nibble value */

		if(nv)
			ieee+=sexp?nv/bmult:nv*bmult;   /* accrue sum; */

		if(sexp){
			/*                                                   */
			/* negative exponent, so increase base to divide by: */
			/*                                                   */
			bmult*=16.0;  /* adjust multiplier                   */
		}else{
			/*                                                     */
			/* positive exponent, so decrease base to multiply by: */
			/*                                                     */
			bmult/=16.0;
			if(bmult<1){
				sexp=1;
				bmult=16.0;
			}
		}
		
		/*                          */
		/* operate on right nibble: */
		/*                          */
		
		nv=p[i]&rnm;      /* right nibble value */

		if(nv)
			ieee+=(double)sexp?nv/(double)bmult:nv*bmult;   /* accrue sum; */
			
		if(sexp){
			/*                                                   */
			/* negative exponent, so increase base to divide by: */
			/*                                                   */
			bmult*=16.0;    /* adjust multiplier                 */
		}else{
			/*                                                     */
			/* positive exponent, so decrease base to multiply by: */
			/*                                                     */
			bmult/=16;
			if(bmult<1){
				sexp=1;
				bmult=16.0;
			}
		}
		
		
	}


	return sigm?-1.0*ieee:ieee;
}

/*                                                           */
/* FreeMarkerVector(db): Frees the db->m vector, if present: */
/*                                                           */
void FreeMarkerVector(PDBF db){

	int i;
	
	if(!db->m) return;
	/*                                             */
	/* free all of the allocated allele BST trees: */
	/*                                             */
	for(i=0;i<db->fc;i++)
		if(db->m[i].a) FreeAllele(db->m[i].a);
	/*                               */
	/* free the db->m vector itself: */
	/*                               */
	free(db->m);
}

/*                                              */
/* FreeAllele: recursively frees an ALLELE BST: */
/*                                              */
void FreeAllele(PALLELE a){

	/*                         */
	/* leave if nothing to do: */
	/*                         */
	if(!a) return;
	/*                                        */
	/* free left and right nodes recursively: */
	/*                                        */
	if(a->l) FreeAllele(a->l);
	if(a->r) FreeAllele(a->r);
	/*                            */
	/* finally free current node: */
	/*                            */
	free(a);
}


/*                                                               */
/* CheckMBaseRecordLength(): needs to be called each time        */
/* an MBase flat file-type database is opened in order to        */
/* deal with the fact that some systems have two bytes marking   */
/* EOL and some have just one.  The solution here is portable,   */
/* since the check is always done, regardless of what system the */
/* file was created on or where or how it was FTPed across       */
/* platforms                                                     */
/*                                                               */
int CheckMBaseRecordLength(PDBF db){

	int crc; /* carriage return byte count */
	int ch;
	
	/*                                                      */
	/* Go to the __UNADJUSTED__ start of the second record: */
	/*                                                      */
	/* and keep a running sum for each '\r' or '\n' found:  */
	/*                                                      */
	GoTo(db,1);
	/*                                                               */
	/* Note that crc will normally be 1 for the single '\n'          */
	/* found on Unix flat files, and 2 for the "\r\n" combination    */
	/* normally found on DOS flat files.  But even this can be       */
	/* messed up by certain FTP or Web transfers across incompatible */
	/* systems.  With the method employed here, it should always     */
	/* work:                                                         */
	/*                                                               */
	for(crc=0;(ch=fgetc(db->fp))=='\r' || ch=='\n';crc++);	
	/*                                                                */
	/* adjust db->rl and db->hd.m.ln which always store record length */
	/* __WITHOUT__ counting the carriage return:                      */
	/*                                                                */
	db->hd.m.ln+=crc;
	db->rl=db->hd.m.ln;
	
	/*                                                     */
	/* Call base class GoTo function to reset to the first */
	/* record:                                             */
	/*                                                     */
	GoTo(db,0);
	/*             */
	/* return crc: */
	/*             */
	return crc;
}


/*                                                         */
/* Support routines for FIELDINDEX structure manipulation: */
/*                                                         */
/*                                                         */
/*                                                         */

/*                                                                      */
/* NewFieldIndex: Allocates and initializes a FIELDINDEX for a database */
/*                                                                      */
/* i represents field count:                                            */
/*                                                                      */
PFIELDINDEX NewFieldIndex(int i){

	PFIELDINDEX fi;
        int ii;

	fi=(PFIELDINDEX)malloc((unsigned)sizeof(FIELDINDEX));
	if(!fi) eprintf(FATALFLAG,"NewFieldIndex(): out of memory allocating new FIELDINDEX structure");
	
        fi->i=(int *)malloc((unsigned)i*sizeof(int));
	if(!fi->i) eprintf(FATALFLAG,"NewFieldIndex(): out of memory allocating index vector");

        fi->r=(int *)malloc((unsigned)i*sizeof(int));
	if(!fi->r) eprintf(FATALFLAG,"NewFieldIndex(): out of memory allocating rank vector");
	/*                         */
	/* c,p,g not yet assigned: */
	/*                         */
	fi->c=fi->p=fi->g=NULL;
	/*                    */
	/* initialize vector: */
	/*                    */
        for(ii=0;ii<i;ii++) fi->i[ii]=fi->r[ii]=0;
	
	return fi;
}

/*                                                   */
/* FreeFieldIndex(): Frees the FIELDINDEX allocation */
/*                                                   */
void FreeFieldIndex(PDBF db){

	free(db->fi->i);
	free(db->fi->r);
	free(db->fi);
	db->fi=NULL;

}

/*                                                             */
/* NewLevelsVector(): Used to allocate the db->levels vector:  */
/* --> Also, now used to allocate the missingcount vector too. */
/*                                                             */
int *NewLevelsVector(int n){

	int *p;
	int i;
	
	p=(int *)malloc((unsigned)sizeof(int)*n);
	if(!p) eprintf(FATALFLAG,"NewLevelsVector(): out of memory allocating vector of ints");
	
	for(i=0;i<n;i++) p[i]=0;
	
	return p;
}



/*                                                                 */
/* ManageFieldTrees(): Manages the db->ftree vector of field trees */
/*                                                                 */
void ManageFieldTrees(PDBF db){

	int i;

	RemoveFieldTrees(db,0); /* The 0 flag means remove trees only for fields no longer bearing ICONSET flag */
	BuildFieldTrees(db);    /* Build any new trees as required                                              */
	/*                                                                                                      */
	/* Tell user how many levels in each field flagged with ICONSET:                                        */
	/*                                                                                                      */
	for(i=0;i<db->fc;i++){
		if(db->ff[i]&ICONSET){
			lprintf("%3i. %s has %i levels.\n",
				i+1,
				db->FieldName(db,i),
				db->levels[i]
			);
		}
	}
}


/*                                                                   */
/* BuildTwoKeyFieldTree(): This function builds a tree where the     */
/* key is the concatenation of two character fields. A '~' delimits  */
/* the two keys.  Note that the two key fields MUST be character     */
/* fields.  (While it would be easy to write a more generic function */
/* if it were needed, it wasn't needed when this was written).       */
/*                                                                   */
/* If tree already exists, the tree is expanded.  If tree==NULL, a   */
/* new tree is built:                                                */
/*                                                                   */
PNODE BuildTwoKeyFieldTree(PNODE tree,PDBF db,int idx1, int idx2){

	VARIABLE d,*data=&d;
	UINT32 i;
	char key1[MINIMUMBUFFERSIZE];
	char key2[MINIMUMBUFFERSIZE];

	/*                                      */
	/* A little error checking never hurts: */
	/*                                      */
	if(db->FieldType(db,idx1)!='C' || db->FieldType(db,idx2)!='C')
		eprintf(FATALFLAG,"BuildTwoKeyFieldTree(): Both fields must be character fields");

	/*                */
	/* set data type: */
	/*                */
	data->t=STRINGVALUE;
	/*                                        */
	/* set data->e.s to point to key1 buffer: */
	/* we'll concatenate key2 onto the end of */
	/* key1:                                  */
	/*                                        */
	data->e.s=key1;
	/*           */
	/* read data */
	/*           */
	for(i=0;i<db->rc;i++){
		/*                                                           */
		/* read string data: remember that data->e.s points to key1: */
		/*                                                           */
		db->ReadString(db,key1,idx1,i);
		db->ReadString(db,key2,idx2,i);
		strcat(key1,KEYSEPARATORSTRING);  /* key separator from tree.h */
		strcat(key1,key2);
		/*                                                            */
		/* The index data->i is borrowed here to store the record     */
		/* number of the data value.  Note that data->i is an int,    */
		/* not a UINT32, but this will still work for all practical   */
		/* databases.  If a key repeats, then of course only the      */
		/* record number of the last occurrence of the key is stored: */
		/*                                                            */
		data->i=(int) i; /* store record number                       */

		
		/*                                                     */
		/* Build BST tree using functions from tree.c:         */
		/* Do not include the missing value indicator in       */
		/* the tree: CHARMISSINGSTRING is the NULLSTRING,      */
		/* so we need only check that it's not the NULLSTRING: */
		/*                                                     */
		if(data->e.s[0]){
			if(!tree) tree=HeadNode((void *)data,CopyVariableObject);
			else      tree=AddNode(tree,(void *)data,CompareVariableObjects,CopyVariableObject,1);
		}
	}
	/*                              */
	/* return pointer to head node: */
	/*                              */
	return tree;
}







/*                                                                               */
/* TransposeMarkerDatabase():                                                    */
/*                                                                               */
/* This function is quite specific to Madeline.                                  */
/* An index of unique values is created from the Family, fidx,                   */
/* and Individual, iidx, fields of the source database, sdb.                     */
/* The unique values in the Marker name field, midx,                             */
/* instead become the __names__ of marker fields                                 */
/* in the new transposed data base.  Values from the Allele1 and                 */
/* Allele2 fields, a1idx, and a2idx, are concatenated with a slash '/'           */
/* character to become the values in the marker field.                           */
/*                                                                               */
/* tdbname: file name to give the transposed database                            */
/* tdbtype: file type to give the transposed database (MBASE,XBASE,SASTRANSPORT) */
/*          (NOTE BENE: support may not be available                             */
/*           for __writing__ to all database types!!!!)                          */
/*                                                                               */
/* Return value: 1 if successful, 0 if failed:                                   */
/*                                                                               */
int TransposeMarkerDatabase(PDBF sdb, int fidx, int iidx, int midx, int a1idx, int a2idx,const char *tdbname,int tdbtype){

	PNODE ktree;          /* key tree built from FamilyIDField and IndividualIDField        */
	int KeyRecordCount;   /* total number of keys = total number of records in new database */
	int MarkerFieldCount; /* total number of marker fields                                  */
	PDBF tdb;             /* "transposed" database                                          */
	int i;
	int FamilyIDFieldWidth;     /* Width of the FamilyIDField                  */
	int IndividualIDFieldWidth; /* Width of the IndividualIDField              */
	int GenotypeFieldWidth=9;   /* Width of one marker (genotype) field        */
	int SpaceAfterWidth=1;      /* Width of space column after each data field */
	int TerminatorWidth=1;      /* Width of record terminator ('\n')           */
	int offset;
	char *MFHFileName;          /* Name of MFH Binary header file */

	/*                                                */
	/* Currently, only MBASE database can be created: */
	/*                                                */
	if(tdbtype!=MBASE)
		eprintf(FATALFLAG,"TransposeMarkerDatabase(): Currently only MBASE type supported for table creation");


	/*//////////////////////////////// */
	/*                                 */
	/* STEP 1: Build needed BST trees: */
	/*                                 */
	/*//////////////////////////////// */

	
	/*                                               */
	/* Build the key FamilyID and IndividualID tree: */
	/*                                               */
	ktree=NULL;
	ktree=BuildTwoKeyFieldTree(ktree,sdb,fidx,iidx);
	/*                                                             */
	/* Rank (i.e., index) the keys, and obtain the total number of */
	/* keys (which will be the total number of records in the new  */
	/* database).                                                  */
	/*                                                             */
	KeyRecordCount=SetNodeRanks(ktree,0);
	/*                                                              */
	/* The marker name tree head node is stored on db->ftree[midx]: */
	/* The number of unique non-missing markers is stored on        */
	/* db->levels[midx]:                                            */
	/*                                                              */
	if(!sdb->ftree[midx]) BuildFieldTree(sdb,midx);


	/*///////////////////////////////////////////// */
	/*                                              */
	/* Step 2: Fill in database header information: */
	/*                                              */
	/*///////////////////////////////////////////// */


	/*                                       */
	/* how many marker fields will there be? */
	/*                                       */
	MarkerFieldCount=sdb->levels[midx];
	/*                                                    */
	/* Width of Family and Individual ID fields from sdb: */
	/*                                                    */
	FamilyIDFieldWidth    =sdb->FieldLength(sdb,fidx);
	IndividualIDFieldWidth=sdb->FieldLength(sdb,iidx);


	/*//////////////////////////////////////////////               */
	/*                                                             */
	/* NOTE BENE: Currently only MBASE                             */
	/* table creation is supported.  Not only that,                */
	/* only the FLAT FILE sub-type of MBASE is                     */
	/* supported:                                                  */
	/*                                                             */
	/*//////////////////////////////////////////////               */
	/*                                                             */
	/* Create a new database: Currently, only MBASE type supported */
	/* for creating a new database:                                */
	/*                                                             */
	tdb=NewDatabase(tdbname,READWRITE,tdbtype);
	/*                                    */
	/* Set the MBASE-specific parameters: */
	/*                                    */
	strcpy(tdb->hd.m.mtype,MFLATSTR);
	strcpy(tdb->hd.m.ffn,tdbname); /* flat file table name                                                */
	strcpy(tdb->hd.m.crd,GenericDateString(JulianToday())); /* creation date                              */
	strcpy(tdb->hd.m.lmd,tdb->hd.m.crd);         /* last modified date                                    */
	strcpy(tdb->hd.m.endian,(Endian()==M_BIG_ENDIAN)?BIGENDIANSTR:LILENDIANSTR); /* endianess             */
	/*                                                                                                    */
	/* field count will be two (for Family and Individual )                                               */
	/* plus the number of marker fields:                                                                  */
	/*                                                                                                    */
	tdb->fc=tdb->hd.m.fc=2+MarkerFieldCount; /* the field count                                           */
	tdb->hd.m.nmv[0]='\0';     /* numeric missing value indicator -- not used: all fields are char fields */
	tdb->hd.m.cmv[0]='.';      /* character missing value indicator: use "."                              */
	tdb->hd.m.cmv[1]='\0';     /*                                                                         */
	/*                                                                                                    */
	/* We will write one blank line to start off the header,                                              */
	/* then write the names of all the fields at the top of the file,                                     */
	/* then write one blank line to close the header.  So the number                                      */
	/* of lines to skip is two plus the field count:                                                      */
	/*                                                                                                    */
	tdb->hd.m.sk=2+tdb->fc;     /* number of lines to skip at top of file                                 */
	/*                                                                                                    */
	/* The number of records is the same as the total number of keys                                      */
	/* in the tree, ktree:                                                                                */
	/*                                                                                                    */
	tdb->rc=tdb->hd.m.rc=KeyRecordCount; /* number of data records                                        */
	/*                                                                                                    */
	/* Record length in bytes is simply the width of all the fields                                       */
	/* plus the width of all the spacing columns (there are one fewer                                     */
	/* spacing columns than there are fields) plus the record terminator                                  */
	/* which can be some combination of CR plus LF (on PCs), but to make                                  */
	/* life simpler, we'll just write CR ('\n') for an MBASE flat file                                    */
	/* (pretending that we're always on a UNIX system):                                                   */
	/*                                                                                                    */
	/* Note however that when we write an MBASE header file to disk,                                      */
	/* we DO NOT count the terminator width:                                                              */
	/*                                                                                                    */
	
	tdb->rl=tdb->hd.m.ln =
		FamilyIDFieldWidth                  +
		IndividualIDFieldWidth              +
		GenotypeFieldWidth*MarkerFieldCount +
		SpaceAfterWidth*(tdb->fc-1)         +
		TerminatorWidth;
		
	/*                                                         */
	/* This is a good time to check whether we are going to be */
	/* able to fit all the records in the memory buffer:       */
	/*                                                         */
	tdb->b.cap=MAXBUFSIZE/tdb->rl;
	if(tdb->b.cap<tdb->rc){
		/*                                                     */
		/* Can't transpose file: TOO BIG: advise user          */
		/* to break file into smaller pieces, then concatenate */
		/* later:                                              */
		
		/*                                   */
		/* Free resources:                   */
		/*                                   */
		/* FreeTree is in tree.c:            */
		/* FreeVariableObject is in vtree.c: */
		/*                                   */
		FreeTree(sdb->ftree[midx],FreeVariableObject);
		sdb->ftree[midx]=NULL;
		FreeTree(ktree,FreeVariableObject);
		/* FreeDatabase() is used here because tdb only exists in memory: */
		/* -- tdb has not been written out to disk yet: */
		FreeDatabase(tdb);
		CloseDatabase(sdb);		
		eprintf(WARNINGFLAG,"File is too large to transpose in memory.  Try dividing \nit into several files which can be transposed individually.\n");
		return 0;
			
	}

		
	/*/////////////////////////////////////////////// */
	/*                                                */
	/* Step 3: Allocate and fill in Field Subheaders: */
	/*                                                */
	/*/////////////////////////////////////////////// */
	
	
	tdb->fd.m=(PMBASEFIELD)malloc((unsigned)sizeof(MBASEFIELD)*tdb->fc);
	if(tdb->fd.m==NULL) eprintf(FATALFLAG,"TransposeMarkerDatabase(): Unable to allocate memory for field subheaders in new table");
	/*                                */
	/* Family ID Field will be first: */
	/*                                */
	offset=0;
	strcpy(tdb->fd.m[0].nm,sdb->FieldName(sdb,fidx));
	tdb->fd.m[0].ty=MSTRING;
	tdb->fd.m[0].of=offset;
	offset+= ( tdb->fd.m[0].ln=sdb->FieldLength(sdb,fidx) );
	offset+= ( tdb->fd.m[0].sp=SpaceAfterWidth            );
	tdb->fd.m[0].pr=0;
	tdb->fd.m[0].et='C'; /* character field */
	/*                                      */
	/* Individual ID Field will be next:    */
	/*                                      */
	strcpy(tdb->fd.m[1].nm,sdb->FieldName(sdb,iidx));
	tdb->fd.m[1].ty=MSTRING;
	tdb->fd.m[1].of=offset;
	offset+= ( tdb->fd.m[1].ln=sdb->FieldLength(sdb,iidx) );
	offset+= ( tdb->fd.m[1].sp=SpaceAfterWidth            );
	tdb->fd.m[1].pr=0;
	tdb->fd.m[1].et='C'; /* character field                   */
	/*                                                        */
	/* Field width, offset, length, and type of the genotype  */
	/* fields is all fixed, so we can just loop to set these: */
	/*                                                        */
	for(i=2;i<tdb->fc;i++){
		tdb->fd.m[i].ty=MSTRING;
		tdb->fd.m[i].of=offset;
		offset+= ( tdb->fd.m[i].ln=GenotypeFieldWidth     );
		offset+= ( tdb->fd.m[i].sp=SpaceAfterWidth        );
		tdb->fd.m[i].pr=0;
		tdb->fd.m[i].et='G'; /* genotype field */
	}
	/*                                                     */
	/* Still need to get the genotype field names from     */
	/* the tree.  The first Genotype field is field no. 2: */
	/*                                                     */
	CopyMbaseFieldNamesFromVariableTree(sdb->ftree[midx],2,tdb->fd.m);
	/*                                                   */
	/* The genotype field names, being the marker names, */
	/* may not yet be capitalized:                       */
	/*                                                   */
	for(i=2;i<tdb->fc;i++) ToUpper(tdb->fd.m[i].nm);
						
	/*                                                  */
	/* (The only piece of information not yet known     */
	/* is the offset to the first record: It's easier   */
	/* to find this out as we write out the data header */
	/* of the flat file in STEP 4 below).               */
	/*                                                  */


	/*/////////////////////////////////////////////////// */
	/*                                                    */
	/* STEP 4: Open the output flat file                  */
	/* and write the flat file header:                    */
	/*                                                    */
	/*/////////////////////////////////////////////////// */

	
	/* */
	/* */
	/* */
	tdb->fp=fopen(tdbname,"wb");
	if(!tdb->fp){
		FreeTree(sdb->ftree[midx],FreeVariableObject);
		sdb->ftree[midx]=NULL;
		FreeTree(ktree,FreeVariableObject);
		/* FreeDatabase() is used here because tdb only exists in memory: */
		/* -- tdb has not been written out to disk yet: */
		FreeDatabase(tdb);
		CloseDatabase(sdb);
		eprintf(WARNINGFLAG,"Unable to open %s for writing.",tdbname);
		return 0;
	}
	/*                                             */
	/* start flat file header with one blank line: */
	/*                                             */
	offset=0;
	offset+=fprintf(tdb->fp,"\n");
	/*                                                           */
	/* print field names and generic field types, one to a line: */
	/*                                                           */
	for(i=0;i<tdb->fc;i++){
		offset+=fprintf(tdb->fp,
			"%s %c\n",
			tdb->FieldName(tdb,i),
			tdb->FieldType(tdb,i)
		);		
	}
	/*                                           */
	/* end flat file header with one blank line: */
	/*                                           */
	offset+=fprintf(tdb->fp,"\n");
	/*                                                  */
	/* store the total byte offset to the first record: */
	/*                                                  */
	tdb->of=tdb->hd.m.of=offset;
	
	
	/*//////////////////////////////////////////////////// */
	/*                                                     */
	/* STEP 5: Write the transposed records to the output: */
	/*         The records are first written to a buffer   */
	/*         in memory which must, therefore, be big     */
	/*         enough to hold all records at once:         */
	/*                                                     */
	/*//////////////////////////////////////////////////// */


	/*                                                 */
	/* Create a buffer in memory and add the required  */
	/* number of blank records to it:                  */
	/*                                                 */
	AddBlankMbaseRecords(tdb,0,tdb->rc);
	/*                                                        */
	/* Write the Family and Individual IDs to the new records */
	/* -- these are fields 0 and 1 in the new table:          */
	/*                                                        */
	WriteKeysFromTwoKeyTreeToFields(tdb,ktree,0,1);
	/*                             */
	/* Write alleles as genotypes: */
	/*                             */
	WriteTransposedRecords(sdb,tdb,ktree,sdb->ftree[midx],fidx,iidx,midx,a1idx,a2idx);
	/*                                                                              */
	/* Write database buffer to the flat file: GoTo() and WriteRecordBufferToDisk() */
	/* access the file on db->fp:                                                   */
	/*                                                                              */
	WriteRecordBufferToDisk(tdb);
	/*                  */
	/* close flat file: */
	/*                  */
	fclose(tdb->fp);
	tdb->fp=NULL;

	/*//////////////////////////////////////////////////  */
	/*                                                    */
	/* STEP 6: Write the Madeline Binary MFH header file: */
	/*                                                    */
	/*/////////////////////////////////////////////////   */


	/*                                                                        */
	/* Open and write completed MFH file:                                     */
	/* Note that before we write the .mfh header, we adjust the record length */
	/* so that the record terminator is not counted:                          */
	/*                                                                        */
	/*                                                                        */
	tdb->rl=(tdb->hd.m.ln-=TerminatorWidth);
	/*                        */
	/* Create .mfh file name: */
	/*                        */
	MFHFileName=ModifyFileName(tdbname,HEADER_FILE_EXTENSION);
	if(File(MFHFileName)) {
		MFHFileName=ModifyFileName(tdbname,ALTERNATE_HEADER_EXTEN);
		lprintf("NOTE: Because a \"%s\" file already exists, the name\n",HEADER_FILE_EXTENSION);
		lprintf("\"%s\" will be used for the binary header file.\n",MFHFileName);
	}

	tdb->fp=fopen(MFHFileName,"wb");
	if(!tdb->fp){
		FreeTree(sdb->ftree[midx],FreeVariableObject);
		sdb->ftree[midx]=NULL;
		FreeTree(ktree,FreeVariableObject);
		FreeDatabase(tdb);
		CloseDatabase(sdb);
		eprintf(WARNINGFLAG,"Unable to open %s for writing",MFHFileName);
		return 0;
	}
	/*                        */
	/* write database header: */
	/*                        */
	fwrite(&tdb->hd.m,sizeof(MBASECOREHEADER),1L,tdb->fp);
	/*                                                           */
	/* write field sub-header vector, then close database header */
	/*                                                           */
	fwrite(tdb->fd.m,sizeof(MBASEFIELD),tdb->fc,tdb->fp);
	fclose(tdb->fp);
	tdb->fp=NULL;

	/*                    */
	/* Release resources: */
	/*                    */
	FreeTree(sdb->ftree[midx],FreeVariableObject);
	sdb->ftree[midx]=NULL;
	FreeTree(ktree,FreeVariableObject);
	FreeDatabase(tdb);
	CloseDatabase(sdb);
		
	/*                */
	/* return success */
	/*                */
	return 1;	
}


/*                                     */
/* CopyMbaseFieldNamesFromVariableTree */
/*                                     */
int CopyMbaseFieldNamesFromVariableTree(PNODE pn,int FieldIndex,PMBASEFIELD fd){

	if(pn){
		if(pn->l) FieldIndex=CopyMbaseFieldNamesFromVariableTree(pn->l,FieldIndex,fd);
		/*                                    */
		/* Copy the field name from the tree: */
		/*                                    */
		strcpy(fd[FieldIndex].nm,((PVARIABLE) pn->o)->e.s);
		FieldIndex++;
		if(pn->r) FieldIndex=CopyMbaseFieldNamesFromVariableTree(pn->r,FieldIndex,fd);
	}
	return FieldIndex;
}


/*                                     */
/* CopyMbaseFieldNamesFromSourceTables */
/*                                     */
int CopyMbaseFieldNamesFromSourceTables(PMBASEFIELD fd,int OutputTableFieldCount,PPDBF SourceTable,int TableCount,int *Key1Index,int *Key2Index){

	int i,j,FieldIndex,sum;

	/*                                                                    */
	/* Check if OutputTableFieldCount and source table field counts mesh: */
	/* If not, this won't work, so return 0:                              */
	/*                                                                    */
	for(sum=i=0;i<TableCount;i++) sum+=SourceTable[i]->fc;
	/*                                                             */
	/* The two key fields are present in every table, but included */
	/* in output just once, so we subtract off 2 for every table   */
	/* except the first one:                                       */
	/*                                                             */
	if(sum-2*(TableCount-1)!=OutputTableFieldCount) return 0;
	
	for(FieldIndex=0,i=0;i<TableCount;i++){		
		for(j=0;j<SourceTable[i]->fc;j++){
			/*                                            */
			/* skip the key fields after the first table: */
			/*                                            */
			if(i>0 && (j==Key1Index[i] || j==Key2Index[i])) continue;
			/*                                                 */
			/* Otherwise, copy field name from source table to */
			/* the output table's field subheader array:       */
			/*                                                 */
			strcpy(fd[FieldIndex].nm,SourceTable[i]->FieldName(SourceTable[i],j));
			FieldIndex++;
		}	
	}
	/*     */
	/* Ok: */
	/*     */
	return FieldIndex;
}

/*                               */
/* CopyMbaseFieldNamesFromBuffer */
/*                               */
int CopyMbaseFieldNamesFromBuffer(PMBASEFIELD fd,int OutputTableFieldCount,char *FieldList){

	int j,FieldIndex;
	char *bb;

	for(FieldIndex=0,bb=FieldList;*bb;){
		
		/* skip any initial white space: */
		for(;*bb && *bb==' ' || *bb=='\t' || *bb=='\n' || *bb=='\r';bb++);
		
		/* Copy field label to fd[<index>].nm: Note capitalization: */
		for(j=0;*bb && j<LNFIELDNAME-1 && !(*bb==' ' || *bb=='\t' || *bb=='\n' || *bb=='\r');j++){
			fd[FieldIndex].nm[j]=islower(*bb)?toupper(*bb):*bb;		
			bb++;
		}

		/* NULL terminate: */		
		fd[FieldIndex].nm[j]='\0';
				
		/* increment FieldIndex counter */
		FieldIndex++;
	}
	/*                               */
	/* Decrement field index by one: */
	/*                               */
	FieldIndex--;
	
	/*                                                           */
	/* Check if OutputTableFieldCount and FieldList counts mesh: */
	/* If not, this won't work, so return 0:                     */
	/*                                                           */
	return (OutputTableFieldCount==FieldIndex)?FieldIndex:0;
}


/*                                                                      */
/*                                                                      */
/* WriteMbaseString(): Writes data to a field of a record in the buffer */
/*                                                                      */
/* db: the database                                                     */
/* s : string to write into field                                       */
/* fn: field number                                                     */
/* rn: record number                                                    */
/*                                                                      */
void WriteMbaseString(PDBF db,char *s,UINT16 fn,UINT32 rn){

	char *ro;    /* record offset                */
	char   *stt; /* where field starts in buffer */
	char   *end; /* one past end of data field   */

	if(rn<db->b.fr || rn>db->b.lr){
		/*                                       */
		/* Can't write to a record out of range: */
		/*                                       */
		eprintf(FATALFLAG,"WriteMbaseString(): Record %i not in buffer",rn);		
	}

	/*                                                                */
	/* ro    is the record offset into the buffer to the start of the */
	/*       record                                                   */
	/* stt   marks where the data start                               */
	/*                                                                */
	ro=db->b.r+(rn-db->b.fr)*db->rl;

	/*                                    */
	/* write left-justified string value: */
	/*                                    */
	stt=ro+db->fd.m[fn].of;
	end=stt+db->fd.m[fn].ln;
	/*                                         */
	/* Write a dot for missing character data: */
	/*                                         */
	if(IsCMV(s)) *stt++='.';	
	else for(;stt<end && *s;*stt++=*s++);
	/*                                         */
	/* pad right margin with spaces if needed: */
	/*                                         */
	for(;stt<end;*stt++=' ');
}

/*                                                       */
/* AddBlankMbaseRecords(): Add nr blank records starting */
/* with first record, fr,.  Records are added in memory  */
/* only:                                                 */
/*                                                       */
void AddBlankMbaseRecords(PDBF db,UINT32 fr,UINT32 nr){

	UINT32 i;
	UINT32 offset;
			
	/*                                                    */
	/* allocate buffer in memory: SetBufferToSize() calls */
	/* memset() to fill the buffer with space characters: */
	/*                                                    */
	SetBufferToSize(db,nr);
	/*                                                  */
	/* add Mbase record terminators ('\n') to structure */
	/* the blank records:                               */
	/*                                                  */
	offset=db->rl-1;
	for(i=0;i<db->b.nr;i++,offset+=db->rl) db->b.r[offset]='\n';
	/*                                                       */
	/* set database parameters:                              */
	/*                                                       */
	db->cr=db->b.fr=fr;           /* first record in buffer  */
	db->b.lr=db->b.fr+db->b.nr-1; /* last record in buffer   */
}

/*                                    */
/* WriteKeysFromTwoKeyTreeToFields(): */
/*                                    */
void WriteKeysFromTwoKeyTreeToFields(PDBF dest,PNODE key,int k1idx,int k2idx){

	char key1[MINIMUMBUFFERSIZE];
	char *key2;
	int recno;

	if(key){
		/*                    */
		/* process left node: */
		/*                    */
		if(key->l) WriteKeysFromTwoKeyTreeToFields(dest,key->l,k1idx,k2idx);

		/*///////////     */
		/*                */
		/* this node:     */
		/*                */
		/*///////////     */
		/*                */
		/* get both keys: */
		/*                */
		strcpy(key1,((PVARIABLE)key->o)->e.s);
		for(key2=key1;*key2 && *key2!=KEYSEPARATOR;key2++);
		*key2++='\0'; /* null terminate key1, key2 now points to 2nd key */
		/*                                                               */
		/* write the keys into destination table:                        */
		/* the node rank provides the record number (subtract one        */
		/* because records are zero-offset:                              */
		/*                                                               */
		WriteMbaseString(dest,key1,k1idx,recno=key->rank-1);
		WriteMbaseString(dest,key2,k2idx,recno);
			
		/*                     */
		/* process right node: */
		/*                     */
		if(key->r) WriteKeysFromTwoKeyTreeToFields(dest,key->r,k1idx,k2idx);
	}
}



/*                                                                 */
/* WriteTransposedRecords(): Reads allele1 and allele2 of a marker */
/* from source database, src, and writes a corresponding genotype  */
/* into the appropriate marker field in destination table, dest:   */
/* sdb: source database                                            */
/* ddb: destination database                                       */
/*                                                                 */
void WriteTransposedRecords(PDBF sdb,PDBF ddb,PNODE keytree,PNODE markertree, int fidx, int iidx, int midx, int a1idx, int a2idx){

	char key1[MINIMUMBUFFERSIZE];
	char key2[MINIMUMBUFFERSIZE];
	VARIABLE v;
	PNODE pn_record;
	UINT32 record;
	PNODE pn_field;
	int field;
	UINT32 i;
	
	/*                                                */
	/* set variable which will be passed to SeekNode: */
	/*                                                */
	v.t=STRINGVALUE;
	v.e.s=key1; /* points to key1 */
	v.i=MISSING;
	/*                                                   */
	/* read from source sdb and write transposed records */
	/* to destination tdb:                               */
	/*                                                   */
	for(i=0;i<sdb->rc;i++){
		/*            */
		/* read keys: */
		/*            */
		sdb->ReadString(sdb,key1,fidx,i);
		sdb->ReadString(sdb,key2,iidx,i);
		
		/*                                                   */
		/* It is a BIG problem if the keys are empty/missing */
		/*                                                   */
		if(!key1[0] || !key2[0]){
			if(!key1[0]) eprintf(SEVEREWARNINGFLAG,"Key 1 (family identifier) is missing for record %i in the source table!\n",i+1);
			if(!key2[0]) eprintf(SEVEREWARNINGFLAG,"Key 2 (indiv. identifier) is missing for record %i in the source table!\n",i+1);
			eprintf(FATALFLAG,"Your source table may contain blank records or \nelse the family and/or individual identifier \ncolumns are empty for one or more records. \nTranspose requires unique non-missing family \nand individual identifiers for every record.\n");
		}

		strcat(key1,KEYSEPARATORSTRING);
		strcat(key1,key2);
		/*                                                         */
		/* Look up record number from node's rank (minus 1 because */
		/* records are zero-offset:                                */
		/*                                                         */
		pn_record=SeekNode(keytree,(void *)&v,CompareVariableObjects);
		record=pn_record->rank-1;
		/*                                                     */
		/* Look up marker field index from node's rank (plus   */
		/* 2 to account for family and individual fields, then */
		/* subtract 1 because fields are zero-offset too:      */
		/*                                                     */
		sdb->ReadString(sdb,key1,midx,i);
		pn_field=SeekNode(markertree,(void *)&v,CompareVariableObjects);
		field=pn_field->rank+1;
		/*                                                                  */
		/* Get allele1 and allele2 data and concatenate with '/' separator: */
		/*                                                                  */
		sdb->ReadString(sdb,key1,a1idx,i);
		sdb->ReadString(sdb,key2,a2idx,i);
		strcat(key1,"/");
		strcat(key1,key2);
		/*                                 */
		/* Finally, write to new database: */
		/*                                 */
		WriteMbaseString(ddb,key1,field,record);	
	}
}


/*                           */
/* WriteRecordBufferToDisk() */
/*                           */
void WriteRecordBufferToDisk(PDBF db){

	GoTo(db,db->b.fr);	
	fwrite(db->b.r,db->rl,db->b.nr,db->fp);

}


/*                                                                         */
/* MergeDatabases():                                                       */
/*                                                                         */
/* src[] is a vector of pointers to the source tables to be merged         */
/* TableCount is the count of tables in src[]                              */
/* Key1FieldName is the name of the first key field  (Family ID field)     */
/* Key2FieldName is the name of the second key field (Individual ID field) */
/*                                                                         */
int MergeDatabases(PPDBF SourceTable,int TableCount,char *Key1FieldName,char *Key2FieldName,char *OutputTableName,int OutputTableType,int MergeOption,char *UserFieldList){


	PNODE  CombinedFieldTree=NULL;     /* combined field name tree                    */
	PPNODE SourceFieldTree  =NULL;     /* vector of field trees for each source table */
	PNODE  lookup;                     /* lookup node                                 */
	int    i,j;
	VARIABLE dd,*data=&dd;             /* VARIABLE object                                                  */
	char   b[MAXIMUMFIELDNAMELENGTH];  /* buffer for field names                                           */
	int    *Key1Index       =NULL;     /* vector of key1 indices                                           */
	int    *Key2Index       =NULL;     /* vector of key2 indices                                           */
	int    CombinedKey1Index;          /* key1 index in CombinedTable                                      */
	int    CombinedKey2Index;          /* key2 index in CombinedTable                                      */
	PNODE  CombinedKeyTree  =NULL;     /* key tree used for merged (destination) table: contains all keys  */
	PPNODE SourceKeyTree    =NULL;     /* vector of PNODE trees: one key tree for each source table        */
	int    CombinedRecordCount;        /* total number of records in merged table                          */
	int    CombinedFieldCount;         /* total count of field in the combined table                       */
	PDBF   CombinedTable    =NULL;     /* output table                                                     */
	int    offset;                     /* byte offset from beginning of table, or from beginning of record */
	/*char   *FieldName; */            /* field name to search                                             */
	int    FieldIndex;                 /* index of found field                                             */
	int    SpaceAfterWidth=1;          /* spacing between field columns in MBASE output table              */
	int    FieldType;                  /* field type of found field                                        */
	char   *MFHFileName;               /* Madeline MBASE binary header file name                           */


	/*                                 */
	/* Only MBASE currently supported: */
	/*                                 */
	if(OutputTableType!=MBASE)
		MadelineErrorReport("MergeDatabases(): Currently only MBASE output is supported");
		

	/*///////////////////////////////////////////////        */
	/*                                                       */
	/* Step 1: Build CombinedFieldTree containing the union  */
	/*         set of fields from ALL source tables without  */
	/*         duplications.                                 */
	/*                                                       */
	/*         Also build SourceFieldTree[i] trees of fields */
	/*         from EACH table for use later.  The data->i   */
	/*         index in these trees will contain the field   */
	/*         index for that field from that source table.  */
	/*                                                       */
	/*///////////////////////////////////////////////        */
	/*                                                       */
	/* Tell user what's going on:                            */
	/*                                                       */
	lprintf("Building field and record trees ...\n");
	
	/*                         */
	/* allocate PPNODE vector: */
	/*                         */
	SourceFieldTree=(PPNODE) malloc((unsigned)TableCount*sizeof(PNODE));
	if(!SourceFieldTree){
		FreeMergeDatabasesFunctionResources(
			TableCount,
			SourceTable,
			SourceFieldTree,
			SourceKeyTree,
			CombinedFieldTree,
			CombinedKeyTree,
			Key1Index,
			Key2Index,
			CombinedTable
		);
		MadelineErrorReport("MergeDatabases(): unable to allocate memory for PPNODE SourceFieldTree");
	}
	for(i=0;i<TableCount;i++) SourceFieldTree[i]=NULL;

	/*                                          */
	/* Build a tree containing the field names  */
	/* from all input sets:                     */
	/*                                          */
	data->t=STRINGVALUE;
	data->e.s=b; /* point to string buffer */
	
	/*                            */
	/* Loop through input tables: */
	/*                            */
	for(i=0;i<TableCount;i++){
		/*                                          */
		/* Loop through fields in each input table: */
		/*                                          */
		for(j=0;j<SourceTable[i]->fc;j++){
			/*                                   */
			/* Copy Field name to data VARIABLE: */
			/*                                   */
			strcpy(data->e.s,SourceTable[i]->FieldName(SourceTable[i],j));
			/*                                                             */
			/* Also keep track of the field index for individual trees for */
			/* each source table:                                          */
			/*                                                             */
			data->i=j;
			/*                                              */
			/* Build BST trees using functions from tree.c: */
			/*                                              */
			if(!CombinedFieldTree ) CombinedFieldTree =HeadNode((void *)data,CopyVariableObject);
			else                    CombinedFieldTree =AddNode(CombinedFieldTree,(void *)data,CompareVariableObjects,CopyVariableObject,1);
			if(!SourceFieldTree[i]) SourceFieldTree[i]=HeadNode((void *)data,CopyVariableObject);
			else                    SourceFieldTree[i]=AddNode(SourceFieldTree[i],(void *)data,CompareVariableObjects,CopyVariableObject,1);
		}
	}
	/*                                                          */
	/* Double check that the two key fields, Key1FieldName, and */
	/* Key2FieldName, are present in ALL input tables:          */
	/*                                                          */
	data->e.s=Key1FieldName;
	lookup=SeekNode(CombinedFieldTree,(void *)data,CompareVariableObjects);
	if(!lookup){
		FreeMergeDatabasesFunctionResources(
			TableCount,
			SourceTable,
			SourceFieldTree,
			SourceKeyTree,
			CombinedFieldTree,
			CombinedKeyTree,
			Key1Index,
			Key2Index,
			CombinedTable
		);
		MadelineErrorReport("MergeDatabases(): Key field %s is not present in any input table",Key1FieldName);
	}else if(lookup->n!=TableCount){
		FreeMergeDatabasesFunctionResources(
			TableCount,
			SourceTable,
			SourceFieldTree,
			SourceKeyTree,
			CombinedFieldTree,
			CombinedKeyTree,
			Key1Index,
			Key2Index,
			CombinedTable
		);
		MadelineErrorReport("MergeDatabases(): Key field %s is not present in all %i input tables",Key1FieldName,TableCount);
	}
	data->e.s=Key2FieldName;
	lookup=SeekNode(CombinedFieldTree,(void *)data,CompareVariableObjects);
	if(!lookup){
		FreeMergeDatabasesFunctionResources(
			TableCount,
			SourceTable,
			SourceFieldTree,
			SourceKeyTree,
			CombinedFieldTree,
			CombinedKeyTree,
			Key1Index,
			Key2Index,
			CombinedTable
		);
		MadelineErrorReport("MergeDatabases(): Key field %s is not present in any input table",Key2FieldName);
	}else if(lookup->n!=TableCount){
		FreeMergeDatabasesFunctionResources(
			TableCount,
			SourceTable,
			SourceFieldTree,
			SourceKeyTree,
			CombinedFieldTree,
			CombinedKeyTree,
			Key1Index,
			Key2Index,
			CombinedTable
		);
		MadelineErrorReport("MergeDatabases(): Key field %s is not present in %s %i input table%s",
			Key2FieldName,
			TableCount==1?"":"all",
			TableCount,
			TableCount==1?"":"s"
		);
	}
	/*                                                             */
	/* Rank (i.e., index) the keys, and obtain the total number of */
	/* keys (which will be the total number of fields in the new   */
	/* database):                                                  */
	/*                                                             */
	CombinedFieldCount=SetNodeRanks(CombinedFieldTree,0);
	
	
	/*////////////////////////////////////////////////////// */
	/*                                                       */
	/* Step 2: Build key trees:  Build a CombinedKeyTree     */
	/*         which will contain non-duplicate keys from    */
	/*         ALL source tables.                            */
	/*                                                       */
	/*         ALSO build individual key trees for EACH      */
	/*         source table -- these key trees will contain  */
	/*         the record indices for each key from each     */
	/*         source table.                                 */
	/*                                                       */
	/*/////////////////////////////////////////////////////  */

	/*                                                         */
	/* allocate PPNODE SourceKeyTree vector and Key1Index and  */
	/* Key2Index vectors:                                      */
	/*                                                         */
	SourceKeyTree=(PPNODE) malloc((unsigned)TableCount*sizeof(PNODE));
	if(!SourceKeyTree){
		FreeMergeDatabasesFunctionResources(
			TableCount,
			SourceTable,
			SourceFieldTree,
			SourceKeyTree,
			CombinedFieldTree,
			CombinedKeyTree,
			Key1Index,
			Key2Index,
			CombinedTable
		);
	 	MadelineErrorReport("MergeDatabases(): unable to allocate PPNODE SourceKeyTree vector");
	}
	Key1Index    =(int *)  malloc((unsigned)TableCount*sizeof(int  ));
	if(!Key1Index    ){
		FreeMergeDatabasesFunctionResources(
			TableCount,
			SourceTable,
			SourceFieldTree,
			SourceKeyTree,
			CombinedFieldTree,
			CombinedKeyTree,
			Key1Index,
			Key2Index,
			CombinedTable
		);	
		MadelineErrorReport("MergeDatabases(): unable to allocate int *Key1Index vector");
	}
	Key2Index    =(int *)  malloc((unsigned)TableCount*sizeof(int  ));
	if(!Key2Index    ){
		FreeMergeDatabasesFunctionResources(
			TableCount,
			SourceTable,
			SourceFieldTree,
			SourceKeyTree,
			CombinedFieldTree,
			CombinedKeyTree,
			Key1Index,
			Key2Index,
			CombinedTable
		);
		MadelineErrorReport("MergeDatabases(): unable to allocate int *Key2Index vector");
	}
	/*                           */
	/* initialize these vectors: */
	/*                           */
	for(i=0;i<TableCount;i++){
		 SourceKeyTree[i]=NULL;
		 Key1Index[i]=SourceTable[i]->FieldNumber(SourceTable[i],Key1FieldName);
		 Key2Index[i]=SourceTable[i]->FieldNumber(SourceTable[i],Key2FieldName);
	}
	/*                                               */
	/* Loop through all input tables to build trees: */
	/*                                               */
	for(i=0;i<TableCount;i++){
		CombinedKeyTree =BuildTwoKeyFieldTree(CombinedKeyTree ,SourceTable[i],Key1Index[i],Key2Index[i]);
		SourceKeyTree[i]=BuildTwoKeyFieldTree(SourceKeyTree[i],SourceTable[i],Key1Index[i],Key2Index[i]);
	}
	/*                                                             */
	/* Rank (i.e., index) the keys, and obtain the total number of */
	/* keys (which will be the total number of records in the new  */
	/* database).                                                  */
	/*                                                             */
	CombinedRecordCount=SetNodeRanks(CombinedKeyTree,0);


	/*///////////////////////////////////  */
	/*                                     */
	/* Step 3: Set up the output database, */
	/*         CombinedTable:              */
	/*                                     */
	/*///////////////////////////////////  */

	/*                                                             */
	/* Create a new database: Currently, only MBASE type supported */
	/* for creating a new database:                                */
	/*                                                             */
	CombinedTable=NewDatabase(OutputTableName,READWRITE,OutputTableType);
	/*                                                                                                  */
	/* Set the MBASE-specific parameters:                                                               */
	/*                                                                                                  */
	strcpy(CombinedTable->hd.m.mtype,MFLATSTR);                       /* MBASE FLAT type of database    */
	strcpy(CombinedTable->hd.m.ffn,OutputTableName);                  /* flat file table name           */
	strcpy(CombinedTable->hd.m.crd,GenericDateString(JulianToday())); /* creation date                  */
	strcpy(CombinedTable->hd.m.lmd,CombinedTable->hd.m.crd);          /* last modified date             */
	strcpy(CombinedTable->hd.m.endian,(Endian()==M_BIG_ENDIAN)?BIGENDIANSTR:LILENDIANSTR); /* endianess */
	/*                                                                                                  */
	/* field count:                                                                                     */
	/*                                                                                                  */
	CombinedTable->fc=CombinedTable->hd.m.fc=CombinedFieldCount; /* the field count                     */
	CombinedTable->hd.m.nmv[0]='\0';         /* numeric missing value                                   */
	CombinedTable->hd.m.cmv[0]='.';          /* character missing value indicator: use "."              */
	CombinedTable->hd.m.cmv[1]='\0';         /*                                                         */
	/*                                                                                                  */
	/* We will write one blank line to start off the ASCII header,                                      */
	/* then write the names of all the fields at the top of the file,                                   */
	/* then write one blank line to close the ASCII header.  So the number                              */
	/* of lines to skip is two plus the field count:                                                    */
	/*                                                                                                  */
	CombinedTable->hd.m.sk=2+CombinedTable->fc;     /* number of lines to skip at top of file           */
	/*                                                                                                  */
	/* The number of records is the same as the total number of keys                                    */
	/* in the CombinedKeyTree:                                                                          */
	/*                                                                                                  */
	CombinedTable->rc=CombinedTable->hd.m.rc=CombinedRecordCount; /* number of data records             */
	
	
	/*/////////////////////////////////////////////// */
	/*                                                */
	/* Step 4: Allocate and fill field sub-headers:   */
	/*                                                */
	/*/////////////////////////////////////////////// */

	
	CombinedTable->fd.m=(PMBASEFIELD)malloc((unsigned)sizeof(MBASEFIELD)*CombinedTable->fc);
	if(CombinedTable->fd.m==NULL) eprintf(FATALFLAG,"MergeDatabases(): Unable to allocate memory for field subheaders in new table");

	/*                                              */
	/* Field ordering is determined by MergeOption: */
	/*                                              */
	switch(MergeOption){
	case MERGEALPHA:
		/*                                                                 */
		/* Get the field names from the field tree which is in alpha order */
		/* by field name across all source tables:                         */
		/*                                                                 */
		CopyMbaseFieldNamesFromVariableTree(CombinedFieldTree,0,CombinedTable->fd.m);
		break;

	case MERGEPHYSICAL:
		/*                                                                   */
		/* Get the field names in the physical order in which they appear in */
		/* the source tables.  This function knows not to duplicate the      */
		/* two key field names:                                              */
		/*                                                                   */
		if(CopyMbaseFieldNamesFromSourceTables(CombinedTable->fd.m,CombinedTable->fc,SourceTable,TableCount,Key1Index,Key2Index)==0){
			FreeMergeDatabasesFunctionResources(
				TableCount,
				SourceTable,
				SourceFieldTree,
				SourceKeyTree,
				CombinedFieldTree,
				CombinedKeyTree,
				Key1Index,
				Key2Index,
				CombinedTable
			);
			MadelineErrorReport("MergeDatabases(): Wrong number of fields in source table.  Check for duplicated fields in source tables.");
		}
		break;

	case MERGEUSER:
		/*                                                        */
		/* Get the field names in the user defined order in which */
		/* they appear in a string buffer:                        */
		/*                                                        */
		if(CopyMbaseFieldNamesFromBuffer(CombinedTable->fd.m,CombinedTable->fc,UserFieldList)==0){
			FreeMergeDatabasesFunctionResources(
				TableCount,
				SourceTable,
				SourceFieldTree,
				SourceKeyTree,
				CombinedFieldTree,
				CombinedKeyTree,
				Key1Index,
				Key2Index,
				CombinedTable
			);
			MadelineErrorReport("MergeDatabases(): Wrong number of fields in field list.  Check list.");
		}
		break;
	}


	/*                                                                           */
	/* With field names in hand, we need to now fill in field type,              */
	/* field width, and precision.                                               */
	/*                                                                           */
	/* In MergeDatabases(), we now make the assumption that any given field      */
	/* beyond the key fields may also appear in any or even all source tables.   */
	/* Fields are merged side-by-side, but also different subsets of records     */
	/* sharing the same fields may also be merged into the single CombinedTable. */
	/*                                                                           */
	/* As a result of this very real possibility, we simply pull the width and   */
	/* precision information from the first table in which a field appears:      */
	/*                                                                           */
	
	/*                                                     */
	/* Loop and set parameters of fields in CombinedTable: */
	/*	                                                    */
	offset=0;
	for(j=0;j<CombinedTable->fc;j++){

		data->e.s=CombinedTable->FieldName(CombinedTable,j);
		/*                                               */
		/* Find first table in which this field appears: */
		/* Field cannot not be found:                    */
		/*                                               */
		for(i=0;i<TableCount;i++){
			lookup=SeekNode(SourceFieldTree[i],(void *)data,CompareVariableObjects);
			if(lookup){
				FieldIndex=((PVARIABLE)lookup->o)->i; /* index was stored in data->i */
				break;
			}
		}
		/*                                 */
		/* Offset to this field, in bytes: */
		/*                                 */
		CombinedTable->fd.m[j].of=offset;
		/*                  */
		/* Field precision: */
		/*                  */
		CombinedTable->fd.m[j].pr=SourceTable[i]->FieldPrecision(SourceTable[i],FieldIndex);
		/*             */
		/* Field type: */
		/*             */
		FieldType=SourceTable[i]->FieldType(SourceTable[i],FieldIndex);
		switch(FieldType){
		case 'C':
			CombinedTable->fd.m[j].ty=MSTRING;
			/*               */
			/* Field length: */
			/*               */
			offset+= ( CombinedTable->fd.m[j].ln=SourceTable[i]->FieldLength(SourceTable[i],FieldIndex) );
			break;
		case 'D':
			CombinedTable->fd.m[j].ty=MDATE;
			/*                                               */
			/* Field length: fixed at 8 in MBASE (YYYYMMDD): */
			/*                                               */
			offset+= ( CombinedTable->fd.m[j].ln=8);
			break;
		case 'N':
			if(CombinedTable->fd.m[j].pr) CombinedTable->fd.m[j].ty=MFLATDBL;
			else                          CombinedTable->fd.m[j].ty=MFLATINT;
			/*               */
			/* Field length: */
			/*               */
			offset+= ( CombinedTable->fd.m[j].ln=SourceTable[i]->FieldLength(SourceTable[i],FieldIndex) );
			break;
		}
		/*                                     */
		/* Add spacing after column to offset: */
		/*                                     */
		offset+= ( CombinedTable->fd.m[j].sp=SpaceAfterWidth);		
		/*                                                */
		/* If source is also MTYPE, fill in extended type */
		/* information:                                   */
		/*                                                */
		if(SourceTable[i]->ty==MBASE) CombinedTable->fd.m[j].et=SourceTable[i]->fd.m[FieldIndex].et;
	}
	/*                                                                      */
	/* Offset now indicates record length (not counting record terminator): */
	/*                                                                      */
	CombinedTable->rl=CombinedTable->hd.m.ln=offset;

	
	/*/////////////////////////////////////////////////// */
	/*                                                    */
	/* STEP 5: Open the output flat file                  */
	/* and write the flat file header:                    */
	/*                                                    */
	/*/////////////////////////////////////////////////// */


	CombinedTable->fp=fopen(OutputTableName,"wb");
	if(!CombinedTable->fp){
		FreeMergeDatabasesFunctionResources(
			TableCount,
			SourceTable,
			SourceFieldTree,
			SourceKeyTree,
			CombinedFieldTree,
			CombinedKeyTree,
			Key1Index,
			Key2Index,
			CombinedTable
		);
	 	MadelineErrorReport("MergeDatabases(): unable to open %s for writing",OutputTableName);
	}
	/*                                             */
	/* start flat file header with one blank line: */
	/*                                             */
	offset=0;
	offset+=fprintf(CombinedTable->fp,"\n");
	/*                                                           */
	/* print field names and generic field types, one to a line: */
	/*                                                           */
	for(j=0;j<CombinedTable->fc;j++){
		offset+=fprintf(CombinedTable->fp,
			"%s %c\n",
			CombinedTable->FieldName(CombinedTable,j),
			CombinedTable->FieldType(CombinedTable,j)
		);		
	}
	/*                                           */
	/* end flat file header with one blank line: */
	/*                                           */
	offset+=fprintf(CombinedTable->fp,"\n");
	/*                                                  */
	/* store the total byte offset to the first record: */
	/*                                                  */
	CombinedTable->of=CombinedTable->hd.m.of=offset;
	
	
	/*//////////////////////////////////////////////////// */
	/*                                                     */
	/* STEP 6: Write the records to the output:            */
	/*         The records are first written to a buffer   */
	/*         in memory which must, therefore, be big     */
	/*         enough to hold all records at once:         */
	/*                                                     */
	/*//////////////////////////////////////////////////// */


	/*                                                 */
	/* Create a buffer in memory and add the required  */
	/* number of blank records to it:                  */
	/*                                                 */
	AddBlankMbaseRecords(CombinedTable,0,CombinedTable->rc);

	/*                                               */
	/* Get the key field indices from CombinedTable: */
	/*                                               */
	CombinedKey1Index=CombinedTable->FieldNumber(CombinedTable,Key1FieldName);
	CombinedKey2Index=CombinedTable->FieldNumber(CombinedTable,Key2FieldName);

	/*                            */
	/* Tell user what's going on: */
	/*                            */
	lprintf("Writing %i records to %s ...\n",CombinedTable->rc,OutputTableName);

	/*                                                                       */
	/* First write keys from combined tree in sorted order to CombinedTable: */
	/*                                                                       */
	WriteKeysFromTwoKeyTreeToFields(CombinedTable,CombinedKeyTree,CombinedKey1Index,CombinedKey2Index);
		
	/*                                                              */
	/* Now write all the remaining non-key data from the respective */
	/* source tables into CombinedTable:                            */
	/*                                                              */
	WriteMbaseMergedRecords(CombinedTable,SourceTable,TableCount,SourceKeyTree,SourceFieldTree,CombinedKey1Index,CombinedKey2Index);
	

	/*                                                                              */
	/* Write database buffer to the flat file: GoTo() and WriteRecordBufferToDisk() */
	/* access the file on db->fp:                                                   */
	/*                                                                              */
	WriteRecordBufferToDisk(CombinedTable);
	/*                  */
	/* close flat file: */
	/*                  */
	fclose(CombinedTable->fp);
	CombinedTable->fp=NULL;

	/*//////////////////////////////////////////////////  */
	/*                                                    */
	/* STEP 7: Write the Madeline Binary MFH header file: */
	/*                                                    */
	/*//////////////////////////////////////////////////  */


	/*                                    */
	/* Open and write completed MFH file: */
	/*                                    */
	MFHFileName=ModifyFileName(OutputTableName,HEADER_FILE_EXTENSION);
	if(File(MFHFileName)) MFHFileName=ModifyFileName(OutputTableName,ALTERNATE_HEADER_EXTEN);
	lprintf("Writing Madeline binary header file \"%s\" ...\n",MFHFileName);

	CombinedTable->fp=fopen(MFHFileName,"wb");
	if(!CombinedTable->fp){
		FreeMergeDatabasesFunctionResources(
			TableCount,
			SourceTable,
			SourceFieldTree,
			SourceKeyTree,
			CombinedFieldTree,
			CombinedKeyTree,
			Key1Index,
			Key2Index,
			CombinedTable
		);
		MadelineErrorReport("MergeDatabases(): Unable to open %s for writing",MFHFileName);
	}
	/*                        */
	/* write database header: */
	/*                        */
	fwrite(&CombinedTable->hd.m,sizeof(MBASECOREHEADER),1L,CombinedTable->fp);
	/*                                                           */
	/* write field sub-header vector, then close database header */
	/*                                                           */
	fwrite(CombinedTable->fd.m,sizeof(MBASEFIELD),CombinedTable->fc,CombinedTable->fp);
	fclose(CombinedTable->fp);
	CombinedTable->fp=NULL;

	/*                 */
	/* Free resources: */
	/*                 */
	FreeMergeDatabasesFunctionResources(
		TableCount,
		SourceTable,
		SourceFieldTree,
		SourceKeyTree,
		CombinedFieldTree,
		CombinedKeyTree,
		Key1Index,
		Key2Index,
		CombinedTable
	);
	
	/*         */
	/* return: */
	/*         */
	return 1;
}



/*                                                                   */
/* WriteMbaseMergedRecords                                           */
/*                                                                   */
/* src[i] provides pointer to the source database                    */
/* idx[i] provides field index to the source database                */
/*                                                                   */
/* mdb is the Mbase output database to which records will            */
/* be written                                                        */
/*                                                                   */
/* -->              (0) MUST FIRST WRITE KEYS ONLY                   */
/*                  (1) THEN MUST FIND CORRECT RECORD IN A OR B      */
/* -->              (2) MUST CHECK FOR MISSING VALUES BEFORE WRITING */
/*                  (3) MUST HAVE WriteXXX() FUNCTIONS for MBASE     */
/*                                                                   */
void WriteMbaseMergedRecords(PDBF CombinedTable,PPDBF SourceTable,int TableCount,PPNODE SourceKeyTree,PPNODE SourceFieldTree,int Key1Index,int Key2Index){


	UINT32 i;
	UINT16 j;
	int k;
	VARIABLE d,*KeyData=&d;
	VARIABLE f,*FieldData=&f;
	char *string;
	long julian;
	double dv;
	INT32  iv;
	
	char key1[MINIMUMBUFFERSIZE];
	char key2[MINIMUMBUFFERSIZE];
	
	PNODE pn_record;
	UINT32 record;
	PNODE pn_field;
	int field;


	/*                                    */
	/* set KeyData->e.s to point to key1: */
	/*                                    */
	KeyData->t=STRINGVALUE;
	KeyData->i=MISSING;
	KeyData->e.s=key1;        /* string buffer */
	FieldData->t=STRINGVALUE;
	FieldData->i=MISSING;
	/*                          */
	/* Loop through records, i: */
	/*                          */
	for(i=0;i<CombinedTable->rc;i++){
		/*                   */
		/* retrieve the key: */
		/*                   */
		CombinedTable->ReadString(CombinedTable,key1,Key1Index,i);
		CombinedTable->ReadString(CombinedTable,key2,Key2Index,i);
		strcat(key1,KEYSEPARATORSTRING);
		strcat(key1,key2);  /* remember that KeyData->.e.s points to key1 */
		/*                                                                */
		/* Loop through fields, j, excepting the two key fields           */
		/* which have already been written:                               */
		/*                                                                */
		for(j=0;j<CombinedTable->fc;j++){
			/*                           */
			/* don't process key fields: */
			/*                           */
			if(j==Key1Index || j==Key2Index) continue;

			/*                                            */
			/* Get here if processing this key and field: */
			/*                                            */
			
			/*                          */
			/* retrieve the field name: */
			/*                          */
			FieldData->e.s=CombinedTable->FieldName(CombinedTable,j);

			/*                                               */
			/* Find out which source table has data for this */
			/* key and field:                                */
			/*                                               */
			for(k=0;k<TableCount;k++){
				/*                                                     */
				/* look for key and field in one of the source tables: */
				/*                                                     */
				pn_record=SeekNode(SourceKeyTree[k]  ,(void *)KeyData  ,CompareVariableObjects);
				if(!pn_record) continue;
				pn_field =SeekNode(SourceFieldTree[k],(void *)FieldData,CompareVariableObjects);
				if(!pn_field)  continue;
				/*                                                                           */
				/* get here if found:                                                        */
				/*                                                                           */
				record=((PVARIABLE)pn_record->o)->i; /* this is the record number in table k */
				field =((PVARIABLE)pn_field->o)->i;  /* this is the field number in table k  */
								
				
				switch(SourceTable[k]->FieldType(SourceTable[k],field)){
				case 'C':
					string=SourceTable[k]->ReadString(SourceTable[k],NULL,field,record);
					WriteMbaseString(CombinedTable,string,j,i);			
					break;
				case 'D':
					julian=SourceTable[k]->ReadDate(SourceTable[k],field,record);
					WriteMbaseDate(CombinedTable,julian,j,i);
					break;
				case 'N':
					if(SourceTable[k]->FieldPrecision(SourceTable[k],field)){
						dv=SourceTable[k]->ReadDouble(SourceTable[k],field,record);
						WriteMbaseDouble(CombinedTable,dv,j,i);
					}else{
						iv=SourceTable[k]->ReadInt32(SourceTable[k],field,record);
						WriteMbaseInt32(CombinedTable,iv,j,i);
					}
					break;
				}	
				
				
				
				/*                                                     */
				/* Since table k with record and field have been found */
				/* and processed, break out of this loop:              */
				/*                                                     */
				break;
			}   /* END of k loop - source tables                   */
		}       /* END of j loop - fields in combined table     */
	}           /* END of i loop - records in combined table */
	
}


/*                                                                */
/* FreeMergeDatabasesFunctionResources()                          */
/*                                                                */
/* MergeDatabases() uses a lot of resources, so it needs it's own */
/* "destructor" function:                                         */
/*                                                                */
void FreeMergeDatabasesFunctionResources(int TableCount,PPDBF SourceTable,PPNODE SourceFieldTree,PPNODE SourceKeyTree,PNODE CombinedFieldTree,PNODE CombinedKeyTree,int *Key1Index,int *Key2Index,PDBF CombinedTable){

	int i;
	
	/*                      */
	/* Close source tables: */
	/*                      */
	if(SourceTable){
		for(i=0;i<TableCount;i++)
			if(SourceTable[i])
				CloseDatabase(SourceTable[i]);
		free(SourceTable);
	}

	/*                          */
	/* Close destination table: */
	/*                          */
	if(CombinedTable){
		/*                                                         */
		/* CloseDatabase() close db->fp, then calls FreeDatabase() */
		/* So, here we just want to check fp2 too:                 */
		/*                                                         */
		if(CombinedTable->fp2) fclose(CombinedTable->fp2);
		CloseDatabase(CombinedTable);
	}
	
	/*                          */
	/* Free source table trees: */
	/*                          */
	if(SourceFieldTree){
		for(i=0;i<TableCount;i++){	
			if(SourceFieldTree[i]) FreeTree(SourceFieldTree[i],FreeVariableObject);
		}
		free(SourceFieldTree);
	}
	if(SourceKeyTree){
		for(i=0;i<TableCount;i++){	
			if(SourceKeyTree[i])   FreeTree(SourceKeyTree[i]  ,FreeVariableObject);	
		}
		free(SourceKeyTree  );
	}
	
	/*                              */
	/* Free destination table trees */
	/*                              */
	if(CombinedFieldTree) FreeTree(CombinedFieldTree,FreeVariableObject);
	if(CombinedKeyTree  ) FreeTree(CombinedKeyTree  ,FreeVariableObject);
	
	/*                   */
	/* Free int vectors: */
	/*                   */
	if(Key1Index) free(Key1Index);	
	if(Key2Index) free(Key2Index);	
}


/*                                                                      */
/*                                                                      */
/* WriteMbaseDouble(): Writes data to a field of a record in the buffer */
/*                                                                      */
/* db: the database                                                     */
/* dv: double value to write into field                                 */
/* fn: field number                                                     */
/* rn: record number                                                    */
/*                                                                      */
void WriteMbaseDouble(PDBF db,double dv,UINT16 fn,UINT32 rn){

	char   *ro;  /* record offset                */
	char   *stt; /* where field starts in buffer */
	char   *end; /* one past end of data field   */
	char   b[MINIMUMBUFFERSIZE];
	int    result,diff;
	/* int    width; */
	int    prec;
	char   *ss,*bb;

	if(rn<db->b.fr || rn>db->b.lr){
		/*                                       */
		/* Can't write to a record out of range: */
		/*                                       */
		eprintf(FATALFLAG,"WriteMbaseDouble(): Record %i not in buffer",rn);
	}

	/*                                                                */
	/* ro    is the record offset into the buffer to the start of the */
	/*       record                                                   */
	/* stt   marks where the data start                               */
	/* end   marks one past the true end                              */
	/*                                                                */
	ro =db->b.r+(rn-db->b.fr)*db->rl;
	stt=ro+db->fd.m[fn].of;
	end=stt+db->fd.m[fn].ln;
	
	if(dv==DOUBLEMISSINGVALUE){
		/*                                  */
		/* write spaces, then a single dot: */
		/*                                  */
		for(;stt<end-1;*stt++=' ');
		*stt='.';
		return;
	}

	result=sprintf(b,"%*.*f",
		db->fd.m[fn].ln,
		db->fd.m[fn].pr,
		dv
	);
	if(result>db->fd.m[fn].ln){
		diff=result-db->fd.m[fn].ln;
		if(diff>prec)
			MadelineErrorReport("WriteMbaseDouble(): %f can't be printed with a width of only %i",dv,db->fd.m[fn].ln);
		/*                                                                   */
		/* Get here if we can squish the remaining number into format space: */
		/*                                                                   */
		eprintf(WARNINGFLAG,"WriteMbaseDouble(): WARNING: loss of %i digit%s of requested (%i.%i) precision printing %lf\n",
			diff,
			diff>1?"s":"",
			db->fd.m[fn].ln,
			db->fd.m[fn].pr,
			dv
		);
		/*                                               */
		/* reformat with loss of precision and rounding: */
		/*                                               */
		sprintf(b,"%*.*f",db->fd.m[fn].ln,db->fd.m[fn].pr-diff,dv);
		
	}
	/*                                   */
	/* copy from b into database buffer: */
	/*                                   */
	for(ss=stt,bb=b;*bb;*ss++=*bb++);	
}



/*                                                                     */
/*                                                                     */
/* WriteMbaseInt32(): Writes data to a field of a record in the buffer */
/*                                                                     */
/* db: the database                                                    */
/* iv: int value to write into field                                   */
/* fn: field number                                                    */
/* rn: record number                                                   */
/*                                                                     */
void WriteMbaseInt32(PDBF db,INT32 iv,UINT16 fn,UINT32 rn){

	char   *ro;  /* record offset                */
	char   *stt; /* where field starts in buffer */
	char   *end; /* one past end of data field   */
	char   b[MINIMUMBUFFERSIZE];
	int    result;
	/* int diff;  */
	/* int width; */
	/* int prec;  */
	char   *ss,*bb;

	if(rn<db->b.fr || rn>db->b.lr){
		/*                                       */
		/* Can't write to a record out of range: */
		/*                                       */
		eprintf(FATALFLAG,"WriteMbaseDouble(): Record %i not in buffer",rn);		
	}

	/*                                                                */
	/* ro    is the record offset into the buffer to the start of the */
	/*       record                                                   */
	/* stt   marks where the data start                               */
	/* end   marks one past the true end                              */
	/*                                                                */
	ro =db->b.r+(rn-db->b.fr)*db->rl;
	stt=ro+db->fd.m[fn].of;
	end=stt+db->fd.m[fn].ln;


	if(iv==INT32MISSINGVALUE){
		/*                                  */
		/* write spaces, then a single dot: */
		/*                                  */
		for(;stt<end-1;*stt++=' ');
		*stt='.';
		return;
	}
	/*                          */
	/* Get here if not missing: */
	/*                          */
	result=sprintf(b,"%*i",
		db->fd.m[fn].ln,
		iv);
		
	if(result>db->fd.m[fn].ln)
		MadelineErrorReport("WriteMbaseINT32(): %li can't be printed with a width of only %i",iv,db->fd.m[fn].ln);
	/*                                   */
	/* copy from b into database buffer: */
	/*                                   */
	for(ss=stt,bb=b;*bb;*ss++=*bb++);	
}


/*                                                                    */
/*                                                                    */
/* WriteMbaseDate(): Writes data to a field of a record in the buffer */
/*                                                                    */
/* db: the database                                                   */
/* julian: julian date value to write into field                      */
/* fn: field number                                                   */
/* rn: record number                                                  */
/*                                                                    */
void WriteMbaseDate(PDBF db,long julian,UINT16 fn,UINT32 rn){

	char   *ro;  /* record offset                */
	char   *stt; /* where field starts in buffer */
	char   b[MINIMUMBUFFERSIZE];
	/* int    width; */
	/* int    prec; */
	char   *ss,*bb;
	int    mm,dd,yyyy;  /* month, day, year */

	if(rn<db->b.fr || rn>db->b.lr){
		/*                                       */
		/* Can't write to a record out of range: */
		/*                                       */
		eprintf(FATALFLAG,"WriteMbaseDouble(): Record %i not in buffer",rn);		
	}

	/*                                                                */
	/* ro    is the record offset into the buffer to the start of the */
	/*       record                                                   */
	/* stt   marks where the data start                               */
	/* end   marks one past the true end                              */
	/*                                                                */
	ro =db->b.r+(rn-db->b.fr)*db->rl;
	stt=ro+db->fd.m[fn].of;
	/*end=stt+db->fd.m[fn].ln; */

	/*                                     */
	/* convert to calendar representation: */
	/*                                     */
	CalendarDate(julian,&mm,&dd,&yyyy);
	sprintf(b,"%4i%2i%2i",
		yyyy,
		mm,
		dd
	);	
	/*                                   */
	/* copy from b into database buffer: */
	/*                                   */
	for(ss=stt,bb=b;*bb;*ss++=*bb++);
	
}


/*                                                                             */
/*                                                                             */
/* CountFieldFlags(): counts the number of fields in each field flag category: */
/* info is stored in the FIELDCOUNTER object of the database                   */
/*                                                                             */
void CountFieldFlags(PDBF db){

	int i;

	/*                         */
	/* Zero the current count: */
	/*                         */
	
	db->n.c =0;
	db->n.ci=0;
	db->n.co=0;
	db->n.p =0;
	db->n.po=0;
	db->n.g =0;
	db->n.go=0;
	db->n.v =0;
	db->n.vo=0;
	db->n.o =0;
	
	/*          */
	/* recount: */
	/*          */
	
	for(i=0;i<db->fc;i++){
		if(db->ff[i] & KNOWNSET)             db->n.c++;
		if(db->ff[i] & ICONSET)              db->n.ci++;
		if(db->ff[i] & COVARIATESET)         db->n.v++;
		if(db->ff[i] & GENOTYPESET)          db->n.g++;
		if(db->ff[i] & PHENOTYPESET)         db->n.p++;
		if(db->ff[i] & OUTPUTSET){          
			db->n.o++;
			if(db->ff[i] & KNOWNSET    ) db->n.co++;
			if(db->ff[i] & COVARIATESET) db->n.vo++;  
			if(db->ff[i] & GENOTYPESET ) db->n.go++;
			if(db->ff[i] & PHENOTYPESET) db->n.po++;
		}
	}
}

/*                                  */
/*                                  */
/*  end of ndbf.c                   */
/*                                  */
/*                                  */

