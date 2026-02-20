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
/* This file written by Ed Trager, July 2004.                                        */
/*                                                                                   */
/* REASON FOR THIS FILE:                                                             */
/*                                                                                   */
/* Systems like Linux use GNU glibc which provides a wcswidth() function.  To use    */
/* wcswidth() and have it work properly, you first must convert your UTF-8 strings   */
/* to a "wchar_t" UTF-32 string.  Then you can pass the UTF-32 (or UCS) string to    */
/* wcswidth().  However, you will only get the right answer *if* your locale is also */
/* set to a UTF-8 locale.  If not, wcswidth() will give you a garbage answer.        */
/*                                                                                   */
/* Madeline, however, is designed to run on lots of different systems.  Some, like   */
/* OpenBSD, do not have the wide-character and multi-byte string machinery at all.   */
/* On other systems, the user often is not be the system administrator, and hence    */
/* may not have opportunity to install a UTF-8 locale even if the OS supports such   */
/* things.                                                                           */
/*                                                                                   */
/* If you are using a Unicode-capable terminal like mlterm, then you can set things  */
/* up to have proper display of UTF-8 strings even on systems like OpenBSD().  And   */
/* this file provides wcswidth()-style functionality that operates directly from the */
/* UTF-8 string (no need to set aside memory for a temporary UTF-32 string) which    */
/* returns the right answer regardless of the locale, or lack of locale settings,    */
/* in which the terminal program was started.                                        */
/*                                                                                   */
                         
#include "utf8routines.h"
using namespace std;

/*
 * Index into the table below with the first byte of a UTF-8 sequence to
 * get the number of trailing bytes that are supposed to follow it.
 */
static const char trailingBytesForUTF8[256] = {
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
	1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
	2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 3,3,3,3,3,3,3,3,4,4,4,4,5,5,5,5
};

/*
 * Magic values subtracted from a buffer value during UTF8 conversion.
 * This table contains as many values as there might be trailing bytes
 * in a UTF-8 sequence.
 */
static const UTF32 offsetsFromUTF8[6] = { 0x00000000UL, 0x00003080UL, 0x000E2080UL, 
					 0x03C82080UL, 0xFA082080UL, 0x82082080UL };

/*
 * Once the bits are split out into bytes of UTF-8, this is a mask OR-ed
 * into the first byte, depending on how many bytes follow.  There are
 * as many entries in this table as there are UTF-8 sequence types.
 * (I.e., one byte sequence, two byte... six byte sequence.)
 */
static const UTF8 firstByteMark[7] = { 0x00, 0x00, 0xC0, 0xE0, 0xF0, 0xF8, 0xFC };


/*
 * NumberOfUnicodeValuesInUTF8String():
 * 
 * This function tells you the "length" of
 * a UTF8 string in terms of the number of 
 * Unicode code points (values) encoded in the
 * UTF-8 string.  Since all UTF-8 continuation
 * bytes have the pattern "10xxxxxx"
 * (hence the mask 0x00C0), counting
 * only those bytes that are not continuation
 * bytes (hence "!=0x0080) directly provides 
 * the number of encoded code points:
 * 
 */
unsigned int NumberOfUnicodeValuesInUTF8String(const UTF8* string){
	
	const UTF8* s;
	unsigned int i;
	
	for(i=0,s=string;*s;s++) if( (*s & 0x00C0) != 0x0080) i++;
	
	return i;
	
}

/*
 * NumberOfConsoleCellsRequired():
 * 
 * Returns the number of console cells required
 * to display the UTF32 UnicodeValue:
 * 
 *   0 -- Zero for combining characters that sit
 *        above or below a previous glyph.
 *   1 -- For standard character glyphs for most
 *        scripts.
 *   2 -- For double-wide Chinese and Japanese
 *        characters, plus those double-wide
 *        Latin and numeric characters that
 *        Chinese and Japanese people use.
 *        
 */
unsigned int NumberOfConsoleCellsRequired(UTF32 unicodeValue){
	
	/*                                                                  */
	/* On the (*nix) console, we expect to see ASCII text more often    */
	/* than text in most other scripts.  With this in mind, we attempt  */
	/* to test for what we expect to see more often first, so that the  */
	/* routine returns quickly for the common cases and more slowly for */
	/* the less common cases.                                           */
	
	/*                                                                  */
	/* ASCII, LATIN1, LATIN EXTENSIONS, ETC, ALL THE WAY UP TO (BUT OF  */
	/* COURSE NOT INCLUDING) THE COMBINING DIACRITICALS BLOCK:          */
	/*                                                                  */
	if(unicodeValue>=0x0020 && unicodeValue<=0x02FF) return 1;
	/* Horizontal Tab is a special case: */
	if(unicodeValue==0x0009) return 8;
	/* Other control characters at the beginning of ASCII range:        */
	if(unicodeValue>=0x0000 && unicodeValue<=0x001F) return 0;
	/*                                                            */
	/* COMBINING DIACRITICAL MARKS                                */
	/* u0300-u306F Combining Diacritical Marks have zero width:   */
	/*                                                            */
	if(unicodeValue>=0x0300 && unicodeValue<=0x036F) return 0;
	
	/* CJK                                                        */
	/* BMP CJK Blocks (including Yi, Hangul, BoPoMoFo, etc):      */
	/*                                                            */
	/* NB: I am assuming that the YI script would be double-width */
	/*     just like Hanzi. I have yet to see a console font that */
	/*     has YI, so at this writing (2004.07.02), it is just my */
	/*     assumption.                                            */
	/*                                                            */
	/* NB: u4DC0-u4DFF Yijing hexagrams: review someday.  Why are */
	/*     these even in Unicode?                                 */
	/*                                                            */
	/* NB: The PUA spans E000 - F8FF.  The code below ignores     */
	/*     the PUA.  Presumably most PUA chars would require only */
	/*     a single screen cell, but who knows?  We'll stick with */
	/*     the code below for now in order to cover all CJK stuff */
	/*     with as few tests as possible:                         */
	/*                                                            */
	if(unicodeValue>=0x2E80 && unicodeValue<0xFAFF){
		
		switch(unicodeValue){
		case 0x302A:
		case 0x302B:
		case 0x302C:
		case 0x302D:
		case 0x302E:
		case 0x302F:
		case 0x3099:
		case 0x309A:
			return 0;
		default:
			return 2;
		}
	}
	//
	// FULL-WIDTH LATIN, NUMERAL AND RELATED FORMS:
	// (NOTE: FFE0 -> FFE6 Fullwidth forms are in widthMap)
	//
	if(unicodeValue>=0xFF01 && unicodeValue<=0xFF60) return 2;
	
	
	/* PLANE 1 CJK EXTENSION B and CJK SUPPLEMENT -- UGH!         */
	if(unicodeValue>=0x20000 && unicodeValue<=0x2A6DF) return 2;
	if(unicodeValue>=0x2F800 && unicodeValue<=0x2FA1F) return 2;
	/*                                                            */
	/* END OF CJK RANGES                                          */
	/*                                                            */
	
	/*                                                           */
	/* After the large ranges covered above, it is more          */
	/* efficient to have fast table lookups for the              */
	/* generally scattered and sundry non-spacing and unusually- */
	/* spaced vowels, tone marks, accent marks, and other signs. */
	/*                                                           */
	/* For this, we need a sparse array.  Since ordering is not  */
	/* required, a hash map would be ideal.  At the time of this */
	/* writing (2004.07.01), the STL hash_map was not yet part   */
	/* of the C++ standard.  Our tests however indicate  that    */
	/* using STL "hash_map" instead of the STL "map" leads to    */
	/* only a miniscule performance increase.  Since "map" is    */
	/* already part of the C++ STL standard, we use that instead.*/
	/*                                                           */
	/* Note that we only build the map if we get to this point.  */
	/* So if the text being processed contains only ASCII, Latin */   
	/* or CJK, we never use CPU time or memory building the      */
	/* map.                                                      */
	/*                                                           */
	static map <UTF32,CONSOLECELL> widthMap;
	
	/*                                   */
	/* If the map is empty, populate it: */
	/*                                   */
	if(widthMap.empty()) buildWidthMap(widthMap);
	
	/* Look for entry:                   */
	std::map<UTF32,CONSOLECELL>::const_iterator found  = widthMap.find(unicodeValue);
	/*                                                             */
	/* If it isn't in the map, then it is a regular character with */
	/* a width of one cell:                                        */
	/*                                                             */
	return found==widthMap.end()?1:found->second;
	
}

/*                                                                      */
/* UTF8ToUTF32()                                                        */
/*                                                                      */
/* Converts a NULL-terminated UTF8 string ("source") to a               */
/* 0x0000-terminated UTF32 vector of unicode values.  If "target" is    */
/* not NULL, then "targetEnd" needs to be at one past the end of the    */
/* target.  If "target" is NULL, then the function allocates memory for */
/* the target: the allocated target will be big enough to hold the      */
/* converted result.                                                    */
/*                                                                      */
UTF32 *UTF8ToUTF32(const UTF8* source,UTF32 *target,UTF32 *targetEnd){
	
	unsigned int targetLength;
	UTF32 ch;
	unsigned short extraBytesToRead;
	const UTF8* s;
	const UTF8* sourceEnd;
	UTF32* t;
	
	/*                                             */
	/* If target is NULL, then we allocate our own */
	/* target to hold the conversion result:       */
	if(!target){
		targetLength=NumberOfUnicodeValuesInUTF8String(source);
		/* Add one to the target length so we can "null terminate" */
		/* the UTF32 vector:                                       */
		targetLength++;
		target=(UTF32*)malloc((unsigned)targetLength*sizeof(UTF32));
		if(!target){
			/* Returning a null pointer indicates the error: */
			return (UTF32*) NULL;
		}
		targetEnd=target+targetLength;
	}
	
	/* In case of target and targetEnd passed as arguments: */
	if(targetEnd<=target) return (UTF32*) NULL;
	
	/* determine end of source: */
	sourceEnd=source+strlen((const char *)source);
	
	for(s=source,t=target;*s && t<targetEnd;t++) {
		
		extraBytesToRead = trailingBytesForUTF8[*s];
		if (s + extraBytesToRead >= sourceEnd) {
			
			*t=0x0000;
			return target;
			
		}
		
		
		/*                             */
		/* The cases all fall through: */
		/*                             */
		ch=0;
		switch (extraBytesToRead) {
			case 3:	ch += *s++; ch <<= 6;
			case 2:	ch += *s++; ch <<= 6;
			case 1:	ch += *s++; ch <<= 6;
			case 0:	ch += *s++;
		}
		ch -= offsetsFromUTF8[extraBytesToRead];
		
		*t = (ch<=UNI_MAX_UTF32) ? ch:UNI_REPLACEMENT_CHAR;
		
	}
	/* "Null terminate" the UTF32 vector: */
	*t=0x0000;
	return target;
}


/*                                                                      */
/* UTF8StringConsoleDisplayWidth()                                      */
/*                                                                      */
/* Returns the number of console cells required to display the UTF-8    */
/* string.                                                              */
/*                                                                      */
unsigned int UTF8StringConsoleDisplayWidth(const UTF8* source){
	
	UTF32 ch,previous_ch;
	unsigned short extraBytesToRead;
	const UTF8* s;
	const UTF8* sourceEnd;
	
	unsigned int sum;
	
	/* determine end of source: */
	sourceEnd=source+strlen((const char *)source);
	
	for(previous_ch='\0',sum=0,s=source;*s;) {
		
		extraBytesToRead = trailingBytesForUTF8[*s];
		if (s + extraBytesToRead >= sourceEnd) {
			
			/* The sequence is illegal, probably      */
			/* corrupted at the end. We simply return */
			/* the width of the uncorrupted portion:  */
			return sum;
			
		}
		
		/*                             */
		/* The cases all fall through: */
		/*                             */
		ch=0;
		switch (extraBytesToRead) {
			case 3:	ch += *s++; ch <<= 6;
			case 2:	ch += *s++; ch <<= 6;
			case 1:	ch += *s++; ch <<= 6;
			case 0:	ch += *s++;
		}
		
		// Here is our UTF32 UnicodeValue:
		ch -= offsetsFromUTF8[extraBytesToRead];
		
		// Pass it to NumberOfConsoleCellsRequired:
		sum+=NumberOfConsoleCellsRequired(ch);
		
		/* First pass at code to           */
		/* handle mandatory ligatures:     */
		/*                                 */
		
		// ARABIC
		// LAM-ALEF is a mandatory ligature:
		if(previous_ch==0x0644 && (
		   ch==0x0627 || 
		   ch==0x0622 ||
		   ch==0x0623 ||
		   ch==0x0625)) --sum;
		//
		
		// Save previous character for next
		// round:
		previous_ch=ch;
		
	}
	
	return sum;
	
}

//
//
//
void buildWidthMap( map<UTF32,CONSOLECELL> &widthMap ){
	
	// ADDITIONAL FULL-WIDTH CJK FORMS:
	//    FFE0 -> FFE6 Fullwidth forms for cent, pound, etc:
	insertRangeOfValuesIntoWidthMap(0xFFE0,0xFFE6,2,widthMap);
	
	// ZERO-WIDTH JOINERS, DIRECTIONAL FORMATTING CODES, BOM, ETC:
	//    ZWSP, ZWJ, ZWNJ, LRM, RLM:
	insertRangeOfValuesIntoWidthMap(0x200B,0x200F,0,widthMap);
	//    Word joiner:
	widthMap[0x2060]=0;
	//    BOM:
	widthMap[0xFEFF]=0;
	widthMap[0xFFFE]=0;
	
	// CYRILLIC: (CHECKED)
	//    Historical combining miscellaneous symbols:
	insertRangeOfValuesIntoWidthMap(0x0483,0x0486,0,widthMap);
	widthMap[0x0488]=0;
	widthMap[0x0489]=0;
	
	// HEBREW: (CHECKED)
	//    Cantillation marks:
	insertRangeOfValuesIntoWidthMap(0x0591,0x05B9,0,widthMap);
	insertRangeOfValuesIntoWidthMap(0x05BB,0x05BD,0,widthMap);
	widthMap[0x05BF]=0;
	widthMap[0x05C1]=0;
	widthMap[0x05C2]=0;
	widthMap[0x05C4]=0;
	
	// ARABIC: 
	// Honorifics:
	insertRangeOfValuesIntoWidthMap(0x0610,0x0615,0,widthMap);
	//    Points from ISO 8859-6, Combining Maddah & Hamza
	insertRangeOfValuesIntoWidthMap(0x064B,0x0655,0,widthMap);
	//    Superscript ALEF:
	widthMap[0x0670]=0;
	//    Koranic annotation signs:
	insertRangeOfValuesIntoWidthMap(0x06D6,0x06DC,0,widthMap);
	insertRangeOfValuesIntoWidthMap(0x06DF,0x06E4,0,widthMap);
	widthMap[0x06E7]=0;
	widthMap[0x06E8]=0;
	insertRangeOfValuesIntoWidthMap(0x06EA,0x06ED,0,widthMap);
	
	// SYRIAC: (CHECKED)
	//    Points, Vowels, & Marks:
	insertRangeOfValuesIntoWidthMap(0x0730,0x074A,0,widthMap);
	
	// THAANA: (CHECKED)
	// Thaana vowels:
	insertRangeOfValuesIntoWidthMap(0x07A6,0x07B0,0,widthMap);
	
	// DEVANAGARI:
	//    Candrabindu and Anusvara:
	widthMap[0x0901]=0;
	widthMap[0x0902]=0;
	//    Nukta:
	widthMap[0x09]=0;
	//    Over and under dependent vowels:   
	insertRangeOfValuesIntoWidthMap(0x0941,0x0948,0,widthMap);
	//    Virama:
	widthMap[0x094D]=0;
	//    Stress and accent marks:
	insertRangeOfValuesIntoWidthMap(0x0951,0x0954,0,widthMap);
	//    Vocalic "L" and "LL":
	widthMap[0x0962]=0;
	widthMap[0x0963]=0;
	
	// BENGALI:
	//    
	//    Candrabindu:
	widthMap[0x0981]=0;
	//    Bengali Nukta:
	widthMap[0x09BC]=0;
	//    Over and under dependent vowels:
	insertRangeOfValuesIntoWidthMap(0x09C1,0x09C4,0,widthMap);
	//    Vowel signs O and AO appear on both sides of a letter
	//    using either the single unicode value below, or two 
	//    separate unicode values:
	widthMap[0x09CB]=2;
	widthMap[0x09CC]=2;
	//    Virama:
	widthMap[0x09CD]=0;
	//    Stress and accent marks:
	insertRangeOfValuesIntoWidthMap(0x0951,0x0954,0,widthMap);
	//    Vocalic "L" and "LL":
	widthMap[0x09E2]=0;
	widthMap[0x09E3]=0;
	
	// GURMUKHI:
	//    Bindi:
	widthMap[0x0A02]=0;
	//    Nukta:
	widthMap[0x0A3C]=0;
	//    Vowels U and UU:
	widthMap[0x0A41]=0;
	widthMap[0x0A42]=0;
	//    EE and AI:
	widthMap[0x0A47]=0;
	widthMap[0x0A48]=0;
	//    OO, AU, and Virama:
	widthMap[0x0A4B]=0;
	widthMap[0x0A4C]=0;
	widthMap[0x0A4D]=0;
	//    TIPPI and ADDAK:
	widthMap[0x0A70]=0;
	widthMap[0x0A71]=0;
	
	// GUJARATI:
	//    
	//    Candrabindu and Anusvara:
	widthMap[0x0A81]=0;
	widthMap[0x0A82]=0;
	//    Nukta:
	widthMap[0x0ABC]=0;
	//    Over and under dependent vowels:
	insertRangeOfValuesIntoWidthMap(0x0AC1,0x0AC5,0,widthMap);
	widthMap[0x0AC7]=0;
	widthMap[0x0AC8]=0;
	//    Virama:
	widthMap[0x0ACD]=0;
	//    Vocalic "L" and "LL":
	widthMap[0x0AE2]=0;
	widthMap[0x0AE3]=0;
	
	// ORIYA:
	//    
	//    Candrabindu and Anusvara:
	widthMap[0x0B01]=0;
	widthMap[0x0B02]=0;
	//    Nukta:
	widthMap[0x0B3C]=0;
	//    Dependent vowels:
	widthMap[0x0B3F]=0;
	widthMap[0x0B41]=0;
	widthMap[0x0B42]=0;
	widthMap[0x0B43]=0;
	//    Vowel signs O and AU appear on both sides of a letter
	//    using either the single unicode value below, or two 
	//    separate unicode values:
	widthMap[0x0B4B]=2;
	widthMap[0x0B4C]=2;
	//    Virama:
	widthMap[0x0B4D]=0;
	//    Oriya AI length mark:
	widthMap[0x0B56]=0;
	//    Oriya AU length mark presumably takes 1 cell:
	//    widthMap[0x0B56]=1;
	//
	
	// TAMIL:
	//    Anusvara:
	widthMap[0x0B82]=0;
	//    
	//    Over and under dependent vowels:
	insertRangeOfValuesIntoWidthMap(0x0BC0,0x0BC2,0,widthMap);
	//    Dependent vowels apper on both sides of a letter
	//    take up two spaces:
	insertRangeOfValuesIntoWidthMap(0x0BCA,0x0BCC,2,widthMap);
	//    Virama:
	widthMap[0x0BCD]=0;
	
	// TELUGU:
	//
	//    -- NOT SURE: CHECK:
	//    widthMap[0x0C3E]=1;
	//    Dependent vowels I and II: 
	widthMap[0x0C3F]=0;
	widthMap[0x0C40]=0;
	//    Dependent vowels E, EE, AI:
	insertRangeOfValuesIntoWidthMap(0x0C46,0x0C48,0,widthMap);
	//    Dependent vowels O, OO, AU plus Virama (halant):
	insertRangeOfValuesIntoWidthMap(0x0C4A,0x0C4D,0,widthMap);
	//    Length marks:
	widthMap[0x0C55]=0;
	widthMap[0x0C56]=0;
	
	// KANNADA:
	//
	//    Nukta:
	widthMap[0x0CBC]=0;
	//    Vowel sign I:
	widthMap[0x0CBF]=0;
	//    Vowel sign E:
	widthMap[0x0CC6]=0;
	//    Vowel sign AU:
	widthMap[0x0CCC]=0;
	//    Halant (Virama):
	widthMap[0x0CCD]=0;
	
	// MALAYALAM:
	//    Dependent vowels apper on both sides of a letter
	//    take up two spaces:
	insertRangeOfValuesIntoWidthMap(0x0D4A,0x0D4C,2,widthMap);
	
	// SINHALA:
	//    Vowel signs:
	insertRangeOfValuesIntoWidthMap(0x0DD2,0x0DD4,0,widthMap);
	widthMap[0x0DD6]=0;
	//    Two-peice vowel signs:
	widthMap[0x0DDA]=2;
	insertRangeOfValuesIntoWidthMap(0x0DDC,0x0DDE,2,widthMap);
	
	// THAI: (CHECKED)
	//    "Half over" MAI HAN-AKAT is forced to be directly
	//    over in console fonts:
	widthMap[0x0E31]=0;
	//    Directly over/under vowels:
	insertRangeOfValuesIntoWidthMap(0x0E34,0x0E3A,0,widthMap);
	//    Tone marks, Karan, etc:
	insertRangeOfValuesIntoWidthMap(0x0E47,0x0E4E,0,widthMap);
	
	// LAO: (CHECKED)
	//    In Lao, unlike Thai, MAI KAN is directly over:
	widthMap[0x0EB1]=0;
	//    Directly over/under vowels:
	insertRangeOfValuesIntoWidthMap(0x0EB4,0x0EBC,0,widthMap);
	//    Tone marks, etc:
	insertRangeOfValuesIntoWidthMap(0x0EC8,0x0ECD,0,widthMap);
	
	// TIBETAN: (CHECKED)
	//    Astrological digit-combining marks:
	widthMap[0x0F18]=0;
	widthMap[0x0F19]=0;
	widthMap[0x0F35]=0;
	widthMap[0x0F37]=0;
	widthMap[0x0F39]=0;
	//    Dependent vowels and vocalic modifier:
	insertRangeOfValuesIntoWidthMap(0x0F71,0x0F7E,0,widthMap);
	//    Dependent vowels and signs (virama, etc):
	insertRangeOfValuesIntoWidthMap(0x0F80,0x0F84,0,widthMap);
	widthMap[0x0F86]=0;
	widthMap[0x0F87]=0;
	//    Subjoined consonants:
	insertRangeOfValuesIntoWidthMap(0x0F90,0x0FBC,0,widthMap);
	widthMap[0x0FC6]=0;
	
	// MYANMAR: (CHECKED)
	insertRangeOfValuesIntoWidthMap(0x102D,0x1030,0,widthMap);
	widthMap[0x1032]=0;
	widthMap[0x1036]=0;
	widthMap[0x1037]=0;
	widthMap[0x1039]=0;
	widthMap[0x1058]=0;
	widthMap[0x1059]=0;
	
	// TAGALOG: (CHECKED)
	insertRangeOfValuesIntoWidthMap(0x1712,0x1714,0,widthMap);
	
	// HANUNOO: (CHECKED: NOT SURE ABOUT 0x1734)
	insertRangeOfValuesIntoWidthMap(0x1732,0x1734,0,widthMap);
	
	// BUHID:   (CHECKED)
	widthMap[0x1752]=0;
	widthMap[0x1753]=0;
	
	// TAGBANWA: (CHECKED)
	widthMap[0x1772]=0;
	widthMap[0x1773]=0;
	
	// KHMER: (CHECKED)
	//    Inherint vowels used in transcription:
	widthMap[0x17B4]=0;
	widthMap[0x17B5]=0;
	//    Dependent vowels:
	insertRangeOfValuesIntoWidthMap(0x17B7,0x17BD,0,widthMap);
	//    Two-part dependent vowels with pieces on both sides:
	widthMap[0x17BF]=2;
	widthMap[0x17C0]=2;
	widthMap[0x17C4]=2;
	widthMap[0x17C5]=2;
	//    NIKAHIT:
	widthMap[0x17C6]=0;
	//    Other signs:
	insertRangeOfValuesIntoWidthMap(0x17C9,0x17D3,0,widthMap);
	//    Atthacan:
	widthMap[0x17DD]=0;
	
	// LIMBU:
	insertRangeOfValuesIntoWidthMap(0x1920,0x1922,0,widthMap);
	widthMap[0x1927]=0;
	widthMap[0x1928]=0;
	// widthMap[0x192A]=0;  NOT SURE -- CHECK
	widthMap[0x1932]=0;
	// widthMap[0x1933]=0;  NOT SURE -- CHECK
	// widthMap[0x1937]=0;  NOT SURE -- CHECK
	// widthMap[0x1939]=0;  NOT SURE -- CHECK
	widthMap[0x193A]=0;
	widthMap[0x193B]=0;
	
	/* COMBINING MARKS FOR SYMBOLS (EXCEEDINGLY RARE, NO DOUBT:   */
	insertRangeOfValuesIntoWidthMap(0x20D0,0x20FF,0,widthMap);
	/* COMBINING HALF MARKS        (EXCEEDINGLY RARE, NO DOUBT:   */
	insertRangeOfValuesIntoWidthMap(0xFE20,0xFE23,0,widthMap);
	
}

void insertRangeOfValuesIntoWidthMap(UTF32 start,UTF32 end,CONSOLECELL width,map<UTF32,CONSOLECELL> &widthMap){
	
	for(UTF32 i=start;i<=end;i++) widthMap[i]=width;
	
}

Boolean isLegalUTF8String (const UTF8* source){
	
	int length;
	const UTF8*sourceEnd;
	unsigned short extraBytesToRead;
	
	length=strlen((const char *)source);
	sourceEnd=source+length;
	
	while (source < sourceEnd) {
		extraBytesToRead = trailingBytesForUTF8[*source];
		if (source + extraBytesToRead >= sourceEnd) {
			return false;
		}
		if (! isLegalUTF8(source, extraBytesToRead+1)) {
			return false;
		}
		source+=extraBytesToRead;
		source++;
	}
	// Get here if OK:
	return true;
	
}

/*
 * Utility routine to tell whether a sequence of bytes is legal UTF-8.
 * This must be called with the length pre-determined by the first byte.
 * If not calling this from ConvertUTF8to*, then the length can be set by:
 *	length = trailingBytesForUTF8[*source]+1;
 * and the sequence is illegal right away if there aren't that many bytes
 * available.
 * If presented with a length > 4, this returns false.  The Unicode
 * definition of UTF-8 goes up to 4-byte sequences.
 */
Boolean isLegalUTF8(const UTF8 *source, int length) {
	UTF8 a;
	const UTF8 *srcptr = source+length;
	switch (length) {
	default: return false;
		/* Everything else falls through when "true"... */
	case 4: if ((a = (*--srcptr)) < 0x80 || a > 0xBF) return false;
	case 3: if ((a = (*--srcptr)) < 0x80 || a > 0xBF) return false;
	case 2: if ((a = (*--srcptr)) > 0xBF) return false;
		switch (*source) {
			/* no fall-through in this inner switch */
			case 0xE0: if (a < 0xA0) return false; break;
			case 0xF0: if (a < 0x90) return false; break;
			case 0xF4: if (a > 0x8F) return false; break;
			default:  if (a < 0x80) return false;
		}
	case 1: if (*source >= 0x80 && *source < 0xC2) return false;
		if (*source > 0xF4) return false;
	}
	
	return true;
	
}

