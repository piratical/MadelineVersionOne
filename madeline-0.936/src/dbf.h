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
/* "dbf.h" is part of the Madeline software distribution.                            */
/* Written by Edward H. Trager.                                                      */
/*                                                                                   */
/* COPYRIGHT (C) 2000 THE REGENTS OF THE UNIVERSITY OF MICHIGAN,                     */
/* Ann Arbor, Michigan, USA.  ALL RIGHTS RESERVED.                                   */
/*                                                                                   */
/* Portions of dbf.h originally copyright (c) 1995 by Ed Trager                      */
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

/*                              */
/* dbf.h                        */
/*                              */
/* (c) 1999 by Edward H. Trager */
/*                              */
/* Revised version for Madeline */
/* initiated 19990316 by ET     */
/*                              */
/* -> defines DBF structure     */
/*                              */

/*//////////////////////// */
/*                         */
/* ANSI INCLUDES:          */
/*                         */
/*//////////////////////// */
#include <stdio.h>

/*//////////////////////// */
/*                         */
/* INCLUDE portable.h:     */
/*                         */
/*//////////////////////// */
#ifndef PORTABLE_INCLUDED
#include "portable.h"
#endif

/*//////////////////////// */
/*                         */
/* INCLUDE marker.h:       */
/*                         */
/*//////////////////////// */
#ifndef MARKER_INCLUDED
#include "marker.h"
#endif

/*///////////////////////// */
/*                          */
/* INCLUDE tree.h:          */
/*                          */
/*///////////////////////// */
#ifndef TREE_INCLUDED
#include "tree.h"
#endif

/*///////////////////////////// */
/*                              */
/* INCLUDE variablesandarrays.h */
/*                              */
/*///////////////////////////// */
#ifndef VAR_AND_ARR_INCLUDED
#include "variablesandarrays.h"
#endif

/*///////////////////////////// */
/*                              */
/* INCLUDE associativearrays.h */
/*                              */
/*///////////////////////////// */
#ifndef ASSOCIATIVEARRAY_INCLUDED
#include "associativearrays.h"
#endif


/*                                                                               */
/* NOTE BENE: A LOT of these #defines APPLY TO XBASE databases.  Madeline        */
/* now also handles SAS TRANSPORT and MBASE ASCII formats, but that wasn't the   */
/* case when this was originally written. So, there are, unfortunately, a number */
/* of #defines like MAXFIELDS which should really be called XBASE_MAXFIELDS, and */
/* so on, to make it clear what we are talking about.                            */
/*                                                                               */
/* Fortunately, for the SAS and MBASE definitions, a prefix is IS included,      */
/* so you'll see SASXPORTNUMERIC, and MFLATINT, etc., which tell you that they   */
/* apply to SAS or Madeline's MBASE format, respectively.  So, those #defines    */
/* WITHOUT a prefix often are specific to the xbase format. -ET 2000 02 25       */
/*                                                                               */

/*                                                                             */
/* DEFINES:                                                                    */
/*                                                                             */
#define LNFIELDNAME  DBASEFIELDNAMELENGTH /* This INCLUDES the NULL terminator */
#define MAXFIELDS   255
#define LNPATH      255
#define NORMALMODE  0

#define LNSASXPORTFIELDNAME 8
#define SASXPORTNUMERIC     0x01
#define SASXPORTCHARACTER   0x02
#define SASNUMERICDEFAULTLENGTH    14 
#define SASNUMERICDEFAULTPRECISION  6

/*                                          */
/* Maximum DATABASE BUFFER SIZE             */
/*                                          */
/* #define MAXBUFSIZE 8388608               */
/*                                          */
/* Database buffer alone can now be 32 MB:  */
/* (Tables can still be larger than this:   */
/* this just limits how much can get dumped */
/* into memory at one time):                */
/*                                          */
#define MAXBUFSIZE 33554432

/*
	Byte #28 Flag constants
	to indicate:
	-Presence of .CDX or .MDX production index files
	-Presence of memos
	-File is a Visual Foxpro database container (.DBC)
*/
#define HASCDX         0x01
#define HASMEMO        0X02
#define ISDBFCONTAINER 0x04


/*
	Database file type markers
	in file core header byte 0
*/
#define NOTADATABASE   0x00
#define FOXBASE        0x02
#define DBASEIII       0x03
#define VFP            0x30
#define DBASEIVSQL     0x43
#define DBASEIVSYS     0x63
#define DBASEIIIMEMO   0x83
#define DBASEIVMEMO    0x8B
#define DBASEIVSQLMEMO 0xCB
#define FOXPROMEMO     0xF5
#define FOXBASE2       0xFB


/*
	Traditional Database header termination byte
	Note that the header is not really "terminated"
	by this byte in Visual Foxpro because of the
	extended back link path information in the subsequent
	263 bytes.
*/
#define HEADERTERMINATOR 0x0D

/*                                                                  */
/* Field Marks (types).  Madeline really only supports              */
/* handles FMCHARACTER, FMNUMERIC and FMFLOAT, FMDATE, and FMDOUBLE */
/*                                                                  */
#define FMCHARACTER     'C'
#define FMCURRENCY      'Y'
#define FMNUMERIC       'N'
#define FMFLOAT         'F'
#define FMDATE          'D'
#define FMDATETIME      'T'
#define FMDOUBLE        'B'
#define FMINTEGER       'I'
#define FMLOGICAL       'L'
#define FMMEMO          'M'
#define FMGENERAL       'G'
#define FMPICTURE       'P'
#define FMOBJECT        'O'
#define FMSCREEN        'S'
#define FMUNDEFINED     'U'


/*
	Field Flags in byte 18
	of field subheaders
*/
#define FFSYSTEMCOLUMN 0x01
#define FFNULLSOK      0x02
#define FFBINARY       0x04


/*
	Constants used to calculate
	the number of fields in the
	database based on the length
	of the header structure. All
	constants take the form of
	"LNxxxxxx..."
*/
#define LNMAINHEADER       32
#define LNHEADERTERMINATOR 1
#define LNBACKLINKINFO     263
#define LNFIELDSUBHEADER   32

/*
   Generic missing indicator used if field or variable is
   normally zero or positive value is missing:
*/
#ifndef MISSING
#define MISSING -9
#endif


/*                                          */
/* TYPEDEFS for XBASE and GENERAL DATABASES */
/*                                          */


/*                                           */
/* Now recognized open option flags:         */
/*                                           */
/* READONLY: read an extant file             */
/* READWRITE: read and update an extant file */
/*                                           */
#define READONLY  1
#define READWRITE 2


/*                                         */
/* Now-recognized (971212) database types: */
/*                                         */
#define XBASE       1
#define SASXPORT    2
#define MBASE       3
#define FLATFILE    4 /* 2004.08.12.ET: ASCII or UTF-8 FLATFILE */

/*                                                                */
/* Recognized sub-types:                                          */
/*                                                                */
#define GENERICSUBTYPE 0  /* not a special subtype                */
#define VFPSUBTYPE     1  /* Visual Foxpro subtype of XBASE       */
#define VMSSUBTYPE     2  /* VMS/VAX subtype of SAS TRANSPORT     */
#define MFFSUBTYPE     3  /* Madeline MBASE flat-file header file */

/*                                                                   */
/* Recognized Endians:                                               */
/*                                                                   */
/* The "M_" prefix has been added in order to avoid conflicting with */
/* the defines in GNU endian.h.  Note that Madeline does not support */
/* the PDP endian scheme.  Only big and little endian are supported. */
/*                                                                   */
#define M_UNKNOWN_ENDIAN  0
#define M_BIG_ENDIAN      1
#define M_LITTLE_ENDIAN   2
#define M_PERVERSE_ENDIAN 3

/*                                 */
/* Defines for MBASE field types:  */
/*                                 */
/* NOTE BENE: The endianess of em- */
/* bedded UNICODE UTF-16 can be    */
/* deciphered either from a U-FEFF */
/* zero-width no-break space, if   */
/* present, or from the MBASE      */
/* header that specifies the       */
/* endianess of the OS             */
/*                                 */

#define MINT32         'i'
#define MDOUBLE        'd'
#define MSTRING        's'
#define MDATE          'T'
#define MFLATINT       'I'
#define MFLATDBL       'D'
#define MUTF16         'u'  /* UNICODE Uncompressed v.3.0 UTF-16 text - 2000 02 25 ET -- Added for future support */
#define MUTF8          'U'  /* UNICODE UTF-8. NOTE: UCS-16 or UCS-32 formats may not ever really be supported, but UTF-8 will */
#define MBASESTR       "MadelineDatabase"
#define MFLATSTR       "MadelineFlatHead"
#define LILENDIANSTR   "LilEndian"
#define BIGENDIANSTR   "BigEndian"
#define MAXMBASEFIELDNAMELENGTH MAXIMUMFIELDNAMELENGTH
/*                                                         */
/* Size of local static buffer in ReadString() functions   */
/* (you can always use a non-null external buffer instead) */
/*                                                         */
#define LOCALSTATICBUFFERSIZE 254



/*////////////////////// */
/*                       */
/* Structure Definitions */
/*                       */
/*////////////////////// */



/*                           */
/*      XBASEFIELD structure */
/*                           */

#define LNFIELDRESERVE 13

typedef struct sXBASEFIELD{
	char    nm[LNFIELDNAME];                        /* name                            */
	UMARK8  ty;                                     /* type                            */
	UINT32  of;                                     /* offset from beginning of record */
	UINT8   ln;                                     /* length                          */
	UINT8   pr;                                     /* precision (decimal places)      */
	UFLAG8  fl;                                     /* flags                           */
	char    rs[LNFIELDRESERVE];                     /* reserved                        */
}XBASEFIELD,*PXBASEFIELD;



/*                                            */
/* NULLINFO structure to support null support */
/* in the Visual Foxpro format:               */

typedef struct sNULLINFO{

	UFLAG8 hasnulls;    /* 1 if field supports nulls, else 0                       */
	UINT16 nth;         /* nth field in series of fields supporting nulls          */
	UINT8  of;          /* byte offset into _NullFlags field                       */
	UINT8  mask;        /* bit mask for determining if the field is currently null */

}NULLINFO,*PNULLINFO;



/*
	HEADER structure
*/
/*
   Xbase core header structure:
*/
#define LNHEADERRESERVE 16
#define LNHEADERRESERVE2 2
typedef struct sXBASECOREHEADER{

	UMARK8 ty;                                              /* type                                  */
	UCHAR8 yr;                                              /* year                                  */
	UCHAR8 mn;                                              /* month                                 */
	UCHAR8 dy;                                              /* day                                   */
	UINT32 rc;                                              /* record count                          */
	UINT16 of;                                              /* offset to beginning of data           */
	UINT16 ln;                                              /* length of one record (w/ delete flag) */
	char   rs[LNHEADERRESERVE];             /* reserved                                              */
	UFLAG8 fl;                                              /* table flags                           */
	UMARK8 cp;                                              /* code page mark                        */
	char   r2[LNHEADERRESERVE2];    /* reserved;                                                     */

}XBASECOREHEADER,*PXBASECOREHEADER;



/*                                                                   */
/*   MBASECOREHEADER : Madeline's flat file and "native" core header */
/*                                                                   */

typedef struct sMBASECOREHEADER{

	char   mtype[17];     /* "MadelineDatabase\0" or "MadelineFlatHead\0"        */
	char   ffn[LNPATH];   /* name of flat file which .mfh file references        */
	char   crd[9];        /* creation  date yyyymmdd                             */
	char   lmd[9];        /* last modi date yyyymmdd                             */
	char   endian[10];    /* "lilendian\0" vs. "bigendian\0"                     */
	UINT16 fc;            /* field count                                         */
	UINT16 ln;            /* length of one record                                */
	char   nmv[LNPATH];   /* missing numeric value indicators, sep. by spaces    */
	char   cmv[LNPATH];   /* missing character value indicatores, sep. by spaces */
	UINT8  sk;            /* number of records at top of file to skip            */
	UINT32 rc;            /* record count                                        */
	UINT16 of;            /* offset to first data record                         */

}MBASECOREHEADER,*PMBASECOREHEADER;


/*                                                                           */
/* MBASEFIELD field subheader format for Madeline .mfh files and .mdb files: */
/*                                                                           */

typedef struct sMBASEFIELD{

	char   nm[MAXMBASEFIELDNAMELENGTH]; /* field name                                                                             */
	UFLAG8 ty;     /* 0=INT32,1=DOUBLE,2=STRING,3=DATE,4=FLATINT,5=FLATFLOAT                                                      */
	UINT32 of;     /* offset                                                                                                      */
	UINT16 ln;     /* length, not including terminal spacing columns in a flat file                                               */
    UINT8  sp;     /* white space columns after this field (0 in .mdb files)                                                   */
    UINT8  pr;     /* decimal precision (0 in .mdb files)                                                                      */
    UFLAG8 et;     /* extended type information: D (date),G (genotype),C (char) ,N (numeric),X (sex/gender), * (indeterminate) */
    
}MBASEFIELD,*PMBASEFIELD;



/*                                                */
/*  BUFFER structure contains                     */
/*  pointer to record buffer, r                   */
/*  and auxilliary variables indicating capacity, */
/*  number of records, first and last records in  */
/*  the buffer                                    */
/*                                                */

typedef struct sBUFFER{

	char   *r;                      /* pointer to records: size to hold n<=65 KB of data */
	UINT32 cap;                     /* capacity of buffer "r" in records                 */
	UINT32 nr;                      /* number of records currently in buffer             */
	UINT32 fr;                      /* first record in buffer                            */
	UINT32 lr;                      /* last record in buffer                             */

} BUFFER, *PBUFFER;



/*                                                           */
/*  FIELDCOUNTER: integers for counting and keeping tabs on  */
/*  various field types used in Madeline                     */
/*                                                           */

typedef struct sFIELDCOUNTER{

	UINT16 c;  /* core fields counter                                     */
	UINT16 ci; /* core input fields counter                               */
	UINT16 co; /* core output fields counter                              */
	UINT16 p;  /* counter of phenotype fields present                     */
	UINT16 po; /* counter of phenotype fields selected for output         */
	UINT16 g;  /* counter of genotype fields present                      */
	UINT16 go; /* counter of genotype fields selected for output          */
	UINT16 v;  /* counter of covariate subtype fields                     */
	UINT16 vo; /* counter of covariate subtype fields selected for output */
	UINT16 o;  /* counter of combined output field set                    */

} FIELDCOUNTER, *PFIELDCOUNTER; 



/*                                    */
/* TYPEDEFS FOR SAS TRANSPORT FORMAT: */
/*                                    */


/*                                                         */
/*  There are a lot of wasted bytes                        */
/*  laying around in a SAS transport file header.          */
/*  Nested in there are some useful tidbits.               */
/*  Some of these can be used to check for file integrity. */
/*  Some of it is used to check whether byte-swapping is   */
/*  required depending on source OS of the file.           */
/*                                                         */


/*         */
/* HEADER1 */
/*         */
typedef struct sHEADER1{
	char s[80];
}HEADER1;

/*         */
/* HEADER2 */
/*         */
typedef struct sHEADER2{
	char sas1[8];
	char sas2[8];
	char lib[8];
	char ver[8];
	char os[8];
	char pd[24];
	char cd[16];
}HEADER2;

/*         */
/* HEADER3 */
/*         */
typedef struct sHEADER3{
	char md[16];
	char pd[64];
}HEADER3;

/*               */
/* MEMBERHEADER1 */
/*               */
typedef struct sMEMBERHEADER1{
	char pd[74]; /* fill/padding                           */
	char sz[4];  /* size of the NAMESTR struct: 136 or 140 */
	char pd2[2]; /* fill/padding                           */
}MEMBERHEADER1;

/*               */
/* MEMBERHEADER2 */
/*               */
typedef struct sMEMBERHEADER2{
	char pd[80];    
}MEMBERHEADER2;

/*             */
/* REALHEADER1 */
/*             */
typedef struct sREALHEADER1{
	char sas[8];
	char dat[8];
	char sasdata[8];
	char ver[8];
	char os[8];
	char pd[24];
	char cd[16];

}REALHEADER1;

/*             */
/* REALHEADER2 */
/*             */
typedef struct sREALHEADER2{
	char md[16];
	char pd[64];
}REALHEADER2;

/*             */
/* REALHEADER3 */
/*             */
typedef struct sREALHEADER3{
	char pd[54];
	char vc[5];   /* variable count is in first four bytes: fifth is for null term. */
	char pd2[21];
}REALHEADER3;

/*                                                                           */
/* This structure is called NAMESTR by SAS, SASXPORTFIELD here:              */
/*                                                                           */
/* NAMESTR: same role as field subheaders in .DBF formatted files            */
/*                                                                           */
/* This is the IBM PC-type NAMESTR                                           */
/* having 140 bytes (on VAX/VMS systems, just read 136 instead of 140 bytes) */
/*                                                                           */
typedef struct sSASXPORTFIELD{
	UINT16 ty;          /* variable type: 1=numeric, 2=character      */
	UINT16 hs;          /* hash of nname --always zero                */
	UINT16 ln;          /* length of variable in observation (?)      */
	UINT16 i;           /* var num (index?)                           */
	char   nm[8];       /* name of variable                           */
	char   lb[40];      /* label for variable                         */
	char   fmt[8];      /* name of format                             */
	UINT16 fmtl;        /* format length                              */
	UINT16 fmtp;        /* format precision -no. of decimal places    */
	UINT16 fmtj;        /* format justification                       */
	char   pd[2];       /* fill/padding                               */
	char   ifmt[8];     /* name of input format                       */
	UINT16 ifmtl;       /* informat length                            */
	UINT16 ifmtp;       /* informat precision -no. of decimal places  */
	UINT32 of;          /* position (offset?) of value in observation */
	char   pd2[52];     /* padding/fill                               */
} SASXPORTFIELD,*PSASXPORTFIELD;


/*                                                                          */
/* OBSERVATIONHEADER: this comes AFTER the NAMESTR/SASXPORTFIELD subheaders */
/* and can, and should, be discarded after integrity checking:              */
/*                                                                          */
typedef struct sOBSERVATIONHEADER{
	char s[80];
}OBSERVATIONHEADER;


/*                      */
/* SASXPORTCOREHEADER:  */
/*                      */
typedef struct sSASXPORTCOREHEADER{

	HEADER1       h1;
	HEADER2       h2;
	HEADER3       h3;
	MEMBERHEADER1 mh1;
	MEMBERHEADER2 mh2;
	REALHEADER1   rh1;
	REALHEADER2   rh2;
	REALHEADER3   rh3;

}SASXPORTCOREHEADER,*PSASXPORTCOREHEADER;


/*
   UNIONs for the different database types
   xxxxxCOREHEADER and xxxxFIELD structures:
*/

typedef union uHEADER{
	XBASECOREHEADER x;
	MBASECOREHEADER m;
	SASXPORTCOREHEADER s;
}HEADER,*PHEADER;

/*                                 */
/* Notice that FIELD is really     */
/* a union of POINTERS to structs, */
/* and NOT a union of structs      */
/*                                 */
typedef union uFIELD{

	PXBASEFIELD    x;
	PMBASEFIELD    m;
	PSASXPORTFIELD s;       

}FIELD,*PFIELD;

/*                                                              */
/* FIELDINDEX structure provides means to reorder field output: */
/*                                                              */
typedef struct sFIELDINDEX{

	int *c;   /* pointer to core field order vector                     */
	int *p;   /* pointer to phenotype field order vector                */
	int *g;   /* pointer to genotype field order vector                 */
	int *i;   /* cells of this vector show the field index for ith rank */
	int *r;   /* cells of this vector show rank ordering for field i    */
}FIELDINDEX,*PFIELDINDEX;



/*                    */
/*      DBF structure */
/*                    */

typedef struct sDBF{
	UFLAG8 ty;         /* type of database: XBASE,SAS,MBASE,MFLAT                             */
	UFLAG8 subtype;    /* subtype of file: VFPTYPE for Visual Foxpro, VMSTYPE for SAS VMS/VAX */
	char   nm[LNPATH]; /* file name                                                           */
	FILE  *fp;         /* FILE pointer for primary database file                              */
	FILE  *fp2;        /* FILE pointer for an associated file                                 */
	UFLAG8 swap;       /* set to 1 if byte swapping is required                               */
	UFLAG8 op;         /* flag for fopen(): READONLY vs. READWRITE                            */
	HEADER hd;         /* union of core headers                                               */

	UINT32 rc;         /* record count                                                        */
	UINT16 rl;         /* length of one fixed-length record                                   */
	UINT32 of;         /* offset to the very first byte of first data record                  */
	UINT32 cr;         /* current record index                                                */
	UINT16 fc;         /* field count for XBASE,SAS, & MFLAT/MBASE                            */
	FIELD  fd;         /* union containing pointers to XBASE,SAS, and MBASE field sub-headers */

	unsigned char *ff; /* pointer to field flags                                                     */
	PFIELDINDEX fi;    /* field index pointer                                                        */
	PPNODE      ftree; /* pointer to BST field head nodes (vector of head nodes):                    */
	int *levels;       /* vector of the number of levels (unique values) in a field: used with ftree */
	int *missingcount; /* vector to keep count of the number of missing values per field             */
	PPASSOCARRAY fllaa; /* vector of pointers to _f_ield _l_evels _l_abels _a_ssociative _a_rrays    */
	PPASSOCARRAY flcaa; /* vector of pointers to _f_ield _l_evels _c_olors _a_ssociative _a_rrays    */
	PPASSOCARRAY flgaa; /* vector of pointers to _f_ield _l_evels _g_ray   _a_ssociative _a_rrays    */
	FIELDCOUNTER n;    /* field flag counters                                                        */
	
	PMARKER m;         /* pointer to vector of markers: allocate only if "G" fields present */
	int    mc;         /* number of markers in database                                     */
	
	BUFFER b;          /* record buffer structure for XBASE, SAS, MBASE/MFLAT */
	
	INT16     nfi;     /* _NullFlags field index: missing if not present (hence INT16, not UINT16) */
	PNULLINFO ni;      /* vector of NULLINFO structures just for Visual FoxPro type                */
	/*                                                                                             */
	/* Function pointers that are mapped to the                                                    */
	/* correct functions based on database type:                                                   */
	/*                                                                                             */
	/* --> Use the NewDatabase() function to initialize a DBF structure                            */
	/*     with the correct type and function pointers:                                            */
	/*                                                                                             */
	
	struct sDBF *(*OpenDatabase )(struct sDBF * db);
	INT32  (*ReadInt32     )(struct sDBF * db,UINT16 fn,UINT32 rn);
	double (*ReadDouble    )(struct sDBF * db,UINT16 fn,UINT32 rn);
	char * (*ReadString    )(struct sDBF * db,char *hext,UINT16 fn,UINT32 rn);
	long   (*ReadDate      )(struct sDBF * db,UINT16 fn,UINT32 rn);
	int    (*FieldNumber   )(struct sDBF * db,const char *s);
	char * (*FieldName     )(struct sDBF * db,UINT16 fn);
	char   (*FieldType     )(struct sDBF * db,UINT16 fn);
	UINT16 (*FieldLength   )(struct sDBF * db,UINT16 fn);
	UINT8  (*FieldPrecision)(struct sDBF * db,UINT16 fn);
	
}DBF,*PDBF,**PPDBF;

#define DBF_INCLUDED


