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
/* "tokentree.c" is part of the Madeline software distribution.                      */
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
/* Last modified by Ed Trager, August 2004.                                          */
/*                                                                                   */
/* ********************************************************************************* */

/*                       */
/* tokentree.h           */
/* for tokentree.cpp     */
/*                       */
/* Replaces tokentree.c  */
/*                       */
/* Written by Ed Trager  */
/* August, 2004          */
/*                       */

/*                */
/* ANSI INCLUDES: */
/*                */
#include <string.h>
#include <map>

#ifndef TOKEN_INCLUDED
#include "token.h"
#endif

#ifndef TOKENTYPES_INCLUDED
#include "tokentypes.h"
#endif

//
// string comparison functor for the map:
//
struct stringCompare
{
  bool operator()(const char* s1, const char* s2) const
  {
    return strcmp(s1, s2) < 0;
  }
};

//
// TokenTree class declaration:
//
class TokenTree {
	
private:
	
	std::map <const char *,RECOGNIZEDTOKEN,stringCompare> tokenMap;
	                                                          
public:
	
	TokenTree();
	void install(const char *key,const char *pe,unsigned short i,unsigned char t,double v,const void *p);
	void remove(const char *key);
	unsigned char lookup(const char *key,PTOKENINFO pti);
	void printHelp(const char *seekKey,int seekLength);
	~TokenTree();
	
};

#define TOKNTREE_INCLUDED


