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
/* "token.h" is part of the Madeline software distribution.                          */
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

/*                   */
/* token.h:          */
/*                   */
/* 19990412 ET INIT. */
/*                   */

#ifndef METHODS_INCLUDED
#include "methods.h"
#endif

#ifndef ASSOCIATIVEARRAY_INCLUDED
#include "associativearrays.h"
#endif


/*                                                                                              */
/* Typedefs for function pointer types:                                                         */
/*                                                                                              */
typedef double (*PNF)(double);                /* function taking and returning double           */
typedef char * (*PSF)(char *);                /* function taking and returning char *           */
typedef int    (*PCF)(char *);                /* function taking char * and returning int       */
typedef void   (*PXF)(char *,char *,int,int); /* a string function like "SubString()"           */
typedef void   (*PVF)(void  );                /* function taking and returning no parameters    */
typedef int    (*PIF)(char *,char *);         /* function taking two char * and returning int   */

/*                                                       */
/* UNION structure to store variable pointer information */
/*                                                       */
typedef union sUNION{

	/* the void pointer type required for initialization: */

	const void   *p;              /* used for initialization */
	
	/* the VARIABLE and ARRAY pointer types: */

	VARIABLE *v;                  /* pointer to type VARIABLE  : INTERNALVARIABLE              */
	ARRAY    *a;                  /* pointer to type ARRAY     : INTERNALARRAY                 */
	ASSOCARRAY *aa;               /* pointer to type ASSOCARRAY: ASSOCIATIVEARRAY              */

	METHODS  *m;                  /* pointer to METHODS struct containing function pointers    */
	                              /* function pointer types:                                   */

	PNF     pnf;                  /* pointer to a numeric function return double               */
	PSF     psf;                  /* pointer to a string function returning char *             */
	PCF     pcf;                  /* pointer to a string function returning int                */
	PXF     pxf;                  /* pointer to a string function like SubString()             */
	PVF     pvf;                  /* pointer to a function taking and returning no parameters  */
	PIF     pif;                  /* pointer to a function taking two char * and returning int */
	
}UNION,*PUNION;


/*                                                                     */
/* Each token read by GetToken() is placed into a TOKENINFO structure: */
/*                                                                     */
typedef struct sTOKENINFO{

	char   e[MINIMUMBUFFERSIZE];   /* external token representation */
	const char    *p;              /* "Properized" token representation */
	unsigned short i;              /* internal token representation (was char in older versions) */
	unsigned char  t;              /* type of token                                              */
	int            f;              /* field index of a variable                                  */
	double         v;              /* value of VARIABLE or VALUE token                           */
	UNION          u;              /* union of pointers to arrays and functions                  */

} TOKENINFO,*PTOKENINFO;


//
// 2004.08.04.ET: RECOGNIZEDTOKEN has now been converted from a simple *struct* to a true *class*,
//                complete with *constructors* and *destructor*, but member variables are still
//                *public* for now so that legacy code will still work unchanged in many places
//                in the code base.

/*                                                                    */
/* Each token recognized by the parser as an internal command         */
/* or internal variable or static internal mathematical constant      */
/* is stored in a RECOGNIZEDTOKEN structure.                          */
/*                                                                    */
/* The void pointer vp points to a function if the type is FUNCTION   */
/* and points to a value if the type is a VALUE or STRINGVALUE, etc.  */
/*                                                                    */
class RECOGNIZEDTOKEN{
	
private:
	
	const char    *e;            /* external token representation (in ALL CAPS)    */
	const char    *p;            /* "Proper" Capitalized version of external token */
	unsigned short i;            /* internal token representation                  */
	unsigned char  t;            /* type of token                                  */
	double         v;            /* value of internal constant like PI,E, etc.     */
	UNION          u;            /* union of pointers to arrays and functions      */
	
public:
	
	// Constructors:
	RECOGNIZEDTOKEN(const char *key,const char *ppe,unsigned short ii,unsigned char tt,double vv,const void *pp);
	
	// Copy Constructor:
	RECOGNIZEDTOKEN(const RECOGNIZEDTOKEN& );
	
	// Destructor:
	~RECOGNIZEDTOKEN();
	
	// Get Methods:
	const char *   getProperizedName() const { return p; }
	unsigned short getInternalRepresentation() const { return i; }
	unsigned char  getType() const { return t; }
	double         getValue() const { return v; }
	UNION          getUnion() const { return u; }
	
};

#define TOKEN_INCLUDED


