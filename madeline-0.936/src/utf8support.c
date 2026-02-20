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
/* "utf8support.c" is part of the Madeline software distribution.                    */
/* Written by Edward H. Trager.                                                      */
/*                                                                                   */
/* COPYRIGHT (C) 2003 THE REGENTS OF THE UNIVERSITY OF MICHIGAN,                     */
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
/* Suite 330, Boston, MA 02111-130DAY_SLOTS, USA.                                    */
/*                                                                                   */
/* Last modified by Ed Trager, December 2003.                                        */
/*                                                                                   */
/* ********************************************************************************* */

#include "utf8support.h"

#ifdef MADELINE_UNICODE

#include "utf8routines.h"

#else

#include <stdlib.h>
#include <wchar.h>
#include <string.h>

#endif


//
// ColumnOccupancy(): Returns the number of columns
// that a UTF-8 string will occupy, in theory at least,
// when printed on a Unicode-capable terminal like
// xterm or mlterm.  Note that, at least at the time of
// this writing, the multibyte string
// width functions are not always correct for certain
// scripts, in particular Arabic.  Probably there are
// problems with some other scripts too.
// 
int ColumnOccupancy(const char *UTF8String){
	
#ifdef MADELINE_UNICODE

	return (int) UTF8StringConsoleDisplayWidth((const UTF8*)UTF8String);

#else
	
	wchar_t UCS[MAX_WCS_LEN];
	int columnOccupancy;
	
	/*                                                           */
	/* Convert to UTF-8 byte string to 32-bit wide wchar_t form: */
	/*                                                           */
	mbstowcs((wchar_t *)UCS,UTF8String,MAX_WCS_LEN);
	/*                                 */
	/* Calculate the column occupancy: */
	/*                                 */
	columnOccupancy=wcswidth(UCS,MAX_WCS_LEN);
	/*                                                */
	/* Manual fix-ups, if any, due to known bugs in   */
	/* the wcswidth() implementations, would go here: */
	/*                                                */
	
	/* Return: */
	return columnOccupancy;
	
#endif

}
