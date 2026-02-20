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
/* "tokentree.cpp" is part of the Madeline software distribution.                    */
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
/* tokentree.cpp         */
/*                       */
/* Replaces tokentree.c  */
/*                       */
/* Written by Ed Trager  */
/* August, 2004          */
/*                       */
/*                       */
/*                       */

#include "token.h"
#include <map>
#include <algorithm>

#include "tokentree.h"

#include "console.h"
#include "logs.h"

using namespace std;

//
// TokenTree Constructor:
//
TokenTree::TokenTree(){
	
	// DEBUG:
	//lprintf("Now in TokenTree() constructor ... \n");
	
}

//
// TokenTree install method: Installs a token into the map.
//
void TokenTree::install(const char *key,const char *pe,unsigned short i,unsigned char t,double v,const void *p){
	
	// DEBUG
	// lprintf("Now in TokenTree::install for key=%s\n",key);
	
	//RECOGNIZEDTOKEN tt(key,pe,i,t,v,p);
	
	//
	// Install the token:
	//
	tokenMap.insert(std::pair<const char *,RECOGNIZEDTOKEN>(key,RECOGNIZEDTOKEN(key,pe,i,t,v,p)));
}

//
// TokenTree remove method: Removes a single node in the token tree:
//
void TokenTree::remove(const char *key){
	
	// Look for entry:
	map <const char *,RECOGNIZEDTOKEN>::iterator found  = tokenMap.find(key);
	//
	// If not found, then just return:
	//
	if(found==tokenMap.end()) return;
	//
	// Else, delete: object first:
	//
	
	tokenMap.erase(found->first);
	
}

//
// TokenTree lookup member function: Looks up a token and places the data into a PTOKENINFO structure.
//
unsigned char TokenTree::lookup(const char *key,PTOKENINFO pti){
	
	// Look for entry:
	map <const char *,RECOGNIZEDTOKEN,stringCompare>::const_iterator found  = tokenMap.find(key);
	
	//
	// If not found, then just return:
	//
	if(found==tokenMap.end()) return (unsigned char) 0;
	
	//
	// Get here if found:
	//
	pti->p  =(const char *)found->second.getProperizedName();
	pti->i  =found->second.getInternalRepresentation();
	pti->t  =found->second.getType();
	pti->v  =found->second.getValue();
	pti->f  =MISSING;
	pti->u.p=found->second.getUnion().p;
	
	/*              */
	/* return type: */
	/*              */
	return pti->t;
	
}


//
// printNode class definition:
//
class printNode{
	
private:
	
	const char *sKey;
	int sLength;
	int sum;
	static const char *TokenTypeDescription[TOKENTYPES];
	
public:
	
	// constructor:
	printNode(const char *seekKey,int seekLength){ sKey=seekKey; sLength=seekLength; sum=0; }
	
	// Overloaded () operator for functor:
	void operator() (std::pair<const char* const, RECOGNIZEDTOKEN>& nodeObject){
		
		if(strncmp(nodeObject.first,sKey,sLength)==0){
			
			// tabulate the number of matching entries:
			lprintf("%3i. ",++sum);
			
			ConsoleBold(stdout);
			lprintf("%s",nodeObject.first);
			ConsoleNormal(stdout);
			// TokenTypeDescription is a static array in tokentypes.h:
			lprintf(" is %s",TokenTypeDescription[nodeObject.second.getType()]);
			
			/*                                           */
			/* Show the value of internal variables:     */
			/* We could show the values of other items   */
			/* too, but showing internal variable values */
			/* is probably the most important & useful:  */
			/*                                           */
			switch(nodeObject.second.getType()){
			case NUMERICVALUE:
				lprintf(". It's value is %g",nodeObject.second.getValue());
				break;
			case INTERNALVARIABLE:
				switch(nodeObject.second.getUnion().v->t){
				case NUMERICVALUE:
					if(nodeObject.second.getUnion().v->e.v==DOUBLEMISSINGVALUE){
						lprintf(". Its current value is #MISSING");
					}else{
						lprintf(". Its current value is %.3f",nodeObject.second.getUnion().v->e.v);
					}
					break;
				case STRINGVALUE:
				case FIELDNAMEVALUE:
					lprintf(". Its current value is \"%s\"",nodeObject.second.getUnion().v->e.s);
					break;
				default:
					lprintf(". Unrecognized value type: %x",nodeObject.second.getUnion().v->t);
				}
				break;
			case INTERNALARRAY:
			case USERARRAY:
				lprintf(". It stores %s values",ValueType(nodeObject.second.getUnion().a->t)); 
				break;
			case ASSOCIATIVEARRAY:
				lprintf(". It accepts %s keys and maps them to %s values",
					ValueType(nodeObject.second.getUnion().aa->kt),
					ValueType(nodeObject.second.getUnion().aa->vt)
				);
				break;
			}
			
			lprintf(".\n");
			
		}
	
	}
	           
};


//
// Static data table used by the printNode class:
//
const char *printNode::TokenTypeDescription[TOKENTYPES]={
	"not a defined token type (0)",
	"a numeric constant",
	"a numeric variable",
	"an operator",
	"a string value",
	"a string variable",
	"an internal variable",
	"a field name value",
	"a command",
	"a function taking and returning a number",
	"a function taking and returning a string",
	"a function taking a string and returning a number",
	"an aggregate function",
	"a function taking two string arguments and returning a boolean result",
	"an internal array",
	"a function taking two string and two numeric arguments and returning a string",
	"a keyword",
	"a numeric property of an individual in a pedigree",
	"an array property of an individual in a pedigree",
	"a string property of an individual in a pedigree",
	"a pointer belonging to an individual that points to another individual",
	"a date value",
	"a date variable",
	"a user variable",
	"a user array",
	"an associative array",
	"a program control operator",
	"a function taking a date and returning a number",
	"a function taking a number and returning a date",
	"a function taking no parameters and returning a date"
};


//
// TokenTree printHelp(): Prints a brief message about a token.
//
void TokenTree::printHelp(const char *seekKey,int seekLength){
	
	for_each(tokenMap.begin(),tokenMap.end(), printNode(seekKey,seekLength) );
	
}


//
// TokenTree printHelp():
//
TokenTree::~TokenTree(){
	
	// DEBUG:
	//lprintf("Releasing token map resources ...\n");
	
}

