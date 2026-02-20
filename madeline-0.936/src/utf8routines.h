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
/* "utf8routines.cpp" is part of the Madeline software distribution.                 */
/* Written by Edward H. Trager.                                                      */
/*                                                                                   */
/* COPYRIGHT (C) 2004 THE REGENTS OF THE UNIVERSITY OF MICHIGAN,                     */
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
/* NOTE BENE: The functions in this file that convert from UTF8 to UTF32,            */
/*            namely UTF8ToUTF32() and UTF8StringConsoleDisplayWidth(),              */
/*            are based on the conversion routines written by                        */
/*            Mark E. Davis in 1994 with fixes and revisions by Rick McGowan         */
/*            in 2001 provided by the Unicode Consortium with the following          */
/*            original copyright notice:                                             */
/*                                                                                   */
/*          +----------------------------------------------------------------------+ */
/*          | Copyright 2001 Unicode, Inc.                                         | */
/*          |                                                                      | */
/*          | Disclaimer                                                           | */
/*          |                                                                      | */
/*          | This source code is provided as is by Unicode, Inc. No claims are    | */
/*          | made as to fitness for any particular purpose. No warranties of any  | */
/*          | kind are expressed or implied. The recipient agrees to determine     | */
/*          | applicability of information provided. If this file has been         | */
/*          | purchased on magnetic or optical media from Unicode, Inc., the       | */
/*          | sole remedy for any claim will be exchange of defective media        | */
/*          | within 90 days of receipt.                                           | */
/*          |                                                                      | */
/*          | Limitations on Rights to Redistribute This Code                      | */
/*          |                                                                      | */
/*          | Unicode, Inc. hereby grants the right to freely use the information  | */
/*          | supplied in this file in the creation of products supporting the     | */
/*          | Unicode Standard, and to make copies of this file in any form        | */
/*          | for internal or external distribution as long as this notice         | */
/*          | remains attached.                                                    | */
/*          +----------------------------------------------------------------------+ */
/*                                                                                   */
/*            The extensive modifications, changes, additions, plus all the other    */
/*            new functionality implemented herein are of course copyright by        */
/*            the Regents of the University of Michigan.                             */
/*                                                                                   */

#include <map>

typedef unsigned long	UTF32;	/* at least 32 bits */
typedef unsigned char	UTF8;	/* typically 8 bits */
typedef unsigned char	Boolean;	/* 0 or 1 */
typedef unsigned char	CONSOLECELL;	/* 0, 1, 2 */

#define UNI_REPLACEMENT_CHAR (UTF32)0x0000FFFD
#define UNI_MAX_UTF32 (UTF32)0x7FFFFFFF

#define false 0
#define true  1

unsigned int NumberOfConsoleCellsRequired(UTF32 UnicodeValue);
unsigned int NumberOfUnicodeValuesInUTF8String(const UTF8* string);
unsigned int UTF8StringConsoleDisplayWidth(const UTF8* source);
UTF32 *UTF8ToUTF32(const UTF8* source,UTF32 *target,UTF32 *targetEnd);
void buildWidthMap( std::map<UTF32,CONSOLECELL> &widthMap );
void insertRangeOfValuesIntoWidthMap(UTF32 start,UTF32 end,CONSOLECELL width,std::map<UTF32,CONSOLECELL> &widthMap);

Boolean isLegalUTF8String (const UTF8* source);
Boolean isLegalUTF8(const UTF8 *source, int length);


