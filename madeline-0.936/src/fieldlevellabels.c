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
/* "fieldlevellabels.c" is part of the Madeline software distribution.               */
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
/* General Public License for more details.                                          */
/*                                                                                   */
/* You should have received a copy of the GNU General Public                         */
/* License along with this software source code distribution; if not,                */
/* write to the Free Software Foundation, Inc., 59 Temple Place -                    */
/* Suite 330, Boston, MA 02111-1307, USA.                                            */
/*                                                                                   */
/* Last modified by Ed Trager, April, 2004.                                          */
/*                                                                                   */
/* ********************************************************************************* */

#include "fieldlevellabels.h"
#include "date.h"

#include "knowntokens.h"
#include "fieldflags.h"
#include "config.h"


/*                                                                           */
/* CreateFieldLevelsLabelsArray():                                           */
/*                                                                           */
/* (1) Creates an array containing label strings for each level in a field   */
/* tree if an array does not already exist in the token tree.  The array's   */
/* external name is simply the field name with "_LABEL" added at the end.    */
/*                                                                           */
/* (2) Creates an array to store color triplets for each level: this array's */
/* external name is the field name with "_COLOR" added at the end.           */
/*                                                                           */
void CreateFieldLevelsLabelsArray(TokenTree& knownTokens,PDBF db,int idx){
	
	char *LabelArrayName, *ProperizedLabelArrayName;
	char *ColorArrayName, *ProperizedColorArrayName;
	char *GrayArrayName , *ProperizedGrayArrayName;
	
	PASSOCARRAY a,c,r;
	int keyType;
	
	extern long globalSeed;
	
	/*                         */
	/* Create the array names: */
	/*                         */
	
	LabelArrayName           = Concatenate("_",db->FieldName(db,idx),"LABEL","");
	ColorArrayName           = Concatenate("_",db->FieldName(db,idx),"COLOR","");
	GrayArrayName            = Concatenate("_",db->FieldName(db,idx),"GRAY" ,"");
	ProperizedLabelArrayName = CopyString(LabelArrayName);
	ProperizedColorArrayName = CopyString(ColorArrayName);
	ProperizedGrayArrayName  = CopyString(GrayArrayName );
	Properize(ProperizedLabelArrayName);
	Properize(ProperizedColorArrayName);
	Properize(ProperizedGrayArrayName );
	
	//
	// TokenTree remove method: Removes a single node in the token tree:
	// If the key is not present, then nothing at all occurs, so it's
	// perfectly safe to call even if the key is not in the map.  These
	// already exist in the map *only* in the case of a previous table
	// already being open:
	// 
	knownTokens.remove(LabelArrayName);
	knownTokens.remove(ColorArrayName);
	knownTokens.remove(GrayArrayName );
	
	/*                                               */
	/* Now we want to create and install the arrays  */
	/* into the token tree:                          */
	/*                                               */
	
	/*                                                               */
	/* All field levels labels arrays will contain string values     */
	/* because they are labels for display on the pedigree drawings. */
	/*                                                               */
	/* The color arrays are also just RGB color triplets in string   */
	/* form.                                                         */
	/*                                                               */
	/* In contrast, gray values are numeric values                   */
	/*                                                               */
	/* The key type however depends on the FieldType from the        */
	/* database:                                                     */
	/*                                                               */
	switch(db->FieldType(db,idx)){
	case 'C':
		keyType=STRINGVALUE;
		break;
	case 'N':
		keyType=NUMERICVALUE;
		break;
	case 'D':
		keyType=DATEVALUE;
		break;
	}
	/*                                                         */
	/* Create new associative array to hold labels and colors: */
	/*                                                         */
	a=NewAssociativeArray(keyType,STRINGVALUE);
	c=NewAssociativeArray(keyType,STRINGVALUE);
	r=NewAssociativeArray(keyType,NUMERICVALUE);
	/*                                                       */
	/* Attach the arrays to the database field levels labels */
	/* and colors vectors:                                   */
	/*                                                       */
	db->fllaa[idx]=a;
	db->flcaa[idx]=c;
	db->flgaa[idx]=r;
	/*                                           */
	/* Set the labels based on the field levels: */
	/*                                           */
	SetLabelsFromFieldTree(a,db->ftree[idx]);
	/*                                                              */
	/* Also set colors and gray values for each level of the field: */
	/*                                                              */
	
	/* Reset the seed so we *ALWAYS* get the same sequence of colors: */
	globalSeed=RANDOMDEVIATESEED;
	SetLevelColorsAndGrayLevels(c,r,db->ftree[idx],db->levels[idx],0);
	
	/*                                      */
	/* Install new Token for the new array: */
	/*                                      */
	knownTokens.install(LabelArrayName,ProperizedLabelArrayName,NOINTERNALTOKEN,ASSOCIATIVEARRAY,0,(void *)a);
	knownTokens.install(ColorArrayName,ProperizedColorArrayName,NOINTERNALTOKEN,ASSOCIATIVEARRAY,0,(void *)c);
	knownTokens.install(GrayArrayName ,ProperizedGrayArrayName ,NOINTERNALTOKEN,ASSOCIATIVEARRAY,0,(void *)r);
	
}


/*                                  */
/* ManageFieldLevelsLabelsArrays(): */
/*                                  */
void ManageFieldLevelsLabelsArrays(TokenTree& knownTokens,PDBF db){

	int i;
	
	for(i=0;i<db->fc;i++){
		if(db->ff[i]&ICONSET){
			CreateFieldLevelsLabelsArray(knownTokens,db,i);
		}
	}
	
}


/*                                                                            */
/* SetLabelsFromFieldTree(): This function sets the labels in the array       */
/* based on the unique values in a field tree.                                */
/*                                                                            */
/*                                                                            */
/*                                                                            */
void SetLabelsFromFieldTree(PASSOCARRAY a,PNODE node){
	
	PVARIABLE key;
	VARIABLE value;
	
	char hold[MINIMUMBUFFERSIZE];  /* place to hold the label */
	
	/*                                               */
	/* Return if at end:                             */
	/*                                               */
	if(!node) return;
	
	/*                    */
	/* Process left node: */
	/*                    */
	if(node->l) SetLabelsFromFieldTree(a,node->l);

	/*////////////////////// */
	/*                       */
	/* Process current node: */
	/*                       */
	/*////////////////////// */
	
	/* Value is always a STRINGVALUE */
	value.t=STRINGVALUE;
	/* value.e.s uses hold here: */
	value.e.s=(char *)hold;
	
	key=(PVARIABLE) node->o;
	switch(key->t){
	case STRINGVALUE:
		
		value.e.s=key->e.s;
		break;
		
	case NUMERICVALUE:
		/*                          */
		/* Set integer labels only: */
		/*                          */
		sprintf(hold,"%i",(int)key->e.v);
		value.e.s=hold;
		break;
	case DATEVALUE:
		/*           */
		/* Set date: */
		/*           */
		value.e.s=DelimitedGenericDateString((long)key->e.v,DEFAULT_ISO_DATE_DELIMITER);
		break;
	}
	/*                                        */
	/* Add the label to the Associative array */
	/*                                        */
	AssociativeArrayAddKeyValuePair(a,key,&value);
	
	/*                     */
	/* Process right node: */
	/*                     */
	if(node->r) SetLabelsFromFieldTree(a,node->r);
	
}

/*                 */
/* SetLevelColors: */
/*                 */
int SetLevelColorsAndGrayLevels(PASSOCARRAY c, PASSOCARRAY r, PNODE node,int totalLevels,int idx){
	
	PVARIABLE key;
	VARIABLE colorValue, grayValue;
	extern long *pGlobalSeed;
	double value,correction;
	//double luminosity;
	
	char hold[MINIMUMBUFFERSIZE];  /* place to hold the label */
	
	COLORTRIPLET randomColor;
	
	/*                             */
	/* Standard fixed color array: */
	/*                             */
	const static COLORTRIPLET fixedColor[NUMBEROFFIXEDCOLORS]={
		{0.7 ,0.7 ,1.0 },
		{1.0 ,0.75,0.3 },
		{1.0 ,1.0 ,1.0 },
		{1.0 ,0.5 ,0.6 },
		{0.63,1.00,0.52},
		{0.91,0.92,0.68},
		{1.0 ,0.62,1.0 },
		{1.0 ,1.0 ,0.0 },
		{1.0 ,0.25,0.25}
	};
	
	/*                                               */
	/* Return if at end:                             */
	/*                                               */
	if(!node) return idx;
	
	/*                    */
	/* Process left node: */
	/*                    */
	if(node->l) idx=SetLevelColorsAndGrayLevels(c,r,node->l,totalLevels,idx);
	
	/*////////////////////// */
	/*                       */
	/* Process current node: */
	/*                       */
	/*////////////////////// */
	
	/*                                                */
	/* VALUE is always a STRINGVALUE                  */
	/* value.e.s uses hold as its string buffer here: */
	/*                                                */
	colorValue.t   = STRINGVALUE;
	colorValue.e.s = (char *)hold;
	
	/*                                */
	/* SET COLOR TRIPLETS:            */
	/* Note that we make sure the     */
	/* colors that are not too dark.  */
	/*                                */
	
	if(idx<NUMBEROFFIXEDCOLORS){
		
		snprintf(colorValue.e.s,MINIMUMBUFFERSIZE-1,"%4.2f %4.2f %4.2f",
			fixedColor[idx].r,
			fixedColor[idx].g,
			fixedColor[idx].b
		);
		
	}else{
		
		//
		// These colors are random, but still adjacent colors
		// are often still too close to one another to look
		// very good.  The *real* problem is that people should
		// not really have categorical variables with more than
		// "NUMBEROFFIXEDCOLORS" colors because after a point 
		// too many levels is just too many.
		// 
		randomColor.r = RandomDeviate(pGlobalSeed);
		randomColor.g = RandomDeviate(pGlobalSeed);
		randomColor.b = RandomDeviate(pGlobalSeed);
		
		/* Since human                    */
		/* color perception is quite non- */
		/* linear, calculating value this */
		/* way from the "color cube" is   */
		/* just an approximation:         */
		
		/*                                  */
		/* This is value in the HSV model:  */
		/* Other measures of brightness/    */
		/* luminescence/intensity do exist. */
		/*                                  */
		value = randomColor.r>randomColor.g?randomColor.r:randomColor.g>randomColor.b?randomColor.g:randomColor.b;
		if(value<MINIMUMHSVCOLORVALUE){
			correction=MINIMUMHSVCOLORVALUE-value;
			
			randomColor.r += correction;
			randomColor.g += correction;
			randomColor.b += correction;
			
		}
		
		snprintf(colorValue.e.s,MINIMUMBUFFERSIZE-1,"%4.2f %4.2f %4.2f",
			randomColor.r,
			randomColor.g,
			randomColor.b
		);
		
	}
	
	/*                                */
	/* ALSO SET THE GRAY VALUE:       */
	/*                                */
	/* Note that this sets things     */
	/* up for the default shading     */
	/* where the first level is black */
	/* and subsequent levels approach */
	/* white.  "ReverseShading" is    */
	/* implemented as a switch in the */
	/* Postscript code, not here:     */
	/*                                */
	grayValue.t  =NUMERICVALUE;
	if(totalLevels>1){
		/*                                */
		/* We round to two decimal places */
		/* to make it look nice:          */
		/*                                */
		grayValue.e.v = Round(100*idx/(totalLevels-1))/100;
	}else{
		/* Naturally we avoid division by zero: */
		grayValue.e.v = 1.0;
	}
	
	/*                                          */
	/* The KEY is directly from the field tree: */
	/*                                          */
	key=(PVARIABLE) node->o;
	
	/*                                                      */
	/* (1) Add the color to the Color associative array     */
	/* (2) Add the gray level to the Gray associative array */
	/*                                                      */
	AssociativeArrayAddKeyValuePair(c,key,&colorValue);
	AssociativeArrayAddKeyValuePair(r,key,&grayValue);
	
	/*                      */
	/* Increment the level: */
	/*                      */
	idx++;
	
	/*                     */
	/* Process right node: */
	/*                     */
	if(node->r) idx=SetLevelColorsAndGrayLevels(c,r,node->r,totalLevels,idx);
	
	return idx;
	
}

