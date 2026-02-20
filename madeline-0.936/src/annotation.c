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
/* "annotation.c" is part of the Madeline software distribution.                     */
/* Written by Edward H. Trager.                                                      */
/*                                                                                   */
/* COPYRIGHT (C) 2000,2002 THE REGENTS OF THE UNIVERSITY OF MICHIGAN,                */
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
/* Last modified by Ed Trager, August, 2002.                                         */
/*                                                                                   */
/* ********************************************************************************* */

/*                                                */
/* HISTORY:                                       */
/*                                                */
/* Last modified: 2002.08.06 by ET                */
/*                                                */

#ifndef MADELINE_INCLUDED
#include "madeline.h"
#endif

#include "annotation.h"

/*                                                                            */
/* GraphAnnotation(): Assign to an associative array                          */
/* using the "GRAPH ADD [ANNOTATION | BAR | ARROW | HORIZONTAL LINE" syntax   */
/*                                                                            */
void GraphAnnotation(void){
	
	VARIABLE key;   
	VARIABLE value;
	PVARIABLE color;
	double hpos,vpos,hend,angle,sign;
	int hunits,vunits,above=1;
	char buffer[SMALLBUFFERSIZE];
	
	TOKENINFO op;
	PTOKENINFO pLabelToken=NULL;
	
	extern PTOKENINFO pT;
	extern ASSOCARRAY aaga;
	extern PVARIABLE  gColorGray;
	
	/*                                                     */
	/* First token needs to refer to an associative array: */
	/*                                                     */
	GetToken();
	switch(pT->i){
		/*                                        */
		/* These options all take an "AT" clause: */
		/*                                        */
		case ARRWK:
		case LABLK:
		case HORZK:
		case VERTK:
			
			op.i=pT->i;
			/*                         */
			/* If HORZK or VERTK, then */
			/* Get the keyword "LINE": */
			/*                         */
			if(pT->i==HORZK || pT->i==VERTK){
				GetToken();
				if(pT->i!=LINEK)
					MadelineErrorReport("GraphAnnotation(): Keyword \"LINE\" expected:\nGRAPH ADD [ HORIZONTAL | VERTICAL ] LINE AT ...");
			}
			/*                                                  */
			/* Get the label, unless it's a horizontal or       */
			/* vertical line, which have no user-defined label: */
			/*                                                  */
			if(!(op.i==HORZK || op.i==VERTK)){
				GetToken();
				if(pT->t!=STRINGVALUE)
					MadelineErrorReport("GraphAnnotation(): Literal string value expected:\nGRAPH ADD [ ARROW | LABEL ] <STRING_LABEL> AT ...");
				pLabelToken=pT; /* Save pointer to the label token */
			}
			/* Get the AT keyword: */
			GetToken();
			if(pT->i!=AT_KY)
				MadelineErrorReport("GraphAnnotation(): Keyword \"AT\" expected :\nGRAPH ADD [ARROW | LABEL | HORIZONTAL LINE | VERTICAL LINE ] <STRING_LABEL> AT ...");
			
			/* Don't get a horizontal position for a horizontal line: */
			if(op.i!=HORZK){
				/*                              */
				/* Get the Horizontal position: */
				/*                              */
				sign=1.0;
				GetToken();
				if(pT->i=='-'){
					sign=-1.0;
					GetToken();
				}
				if(pT->t!=NUMERICVALUE) MadelineErrorReport("GraphAnnotation(): Numeric value expected for the horizontal coordinate:\nGRAPH ADD [ARROW | LABEL | VERTICAL LINE ] <STRING_LABEL> AT <HPOS> [ CENTIMORGANS | CENTIMETERS ] ...");
				hpos=sign*pT->v;
				GetToken();
				/* Horizontal units must be in centimeters or centiMorgans: */
				if(!(pT->i==CM__K || pT->i==CMG_K))
					MadelineErrorReport("GraphAnnotation(): Horizontal coordinate must be measured in \ncentimeters (CM) or centiMorgans (CENTIMORGANS)");
				hunits=pT->i;
				
			}
			
			/* Don't get a vertical coordinate for a vertical line: */
			if(op.i!=VERTK){
				/*                              */
				/* Get the Vertical position:   */
				/*                              */
				sign=1.0;
				GetToken();
				/*                                                                       */
				/* Read over a comma separating the horizontal and vertical coordinates: */
				/*                                                                       */
				if(pT->i==',') GetToken();
				if(pT->i=='-'){
					sign=-1.0;
					GetToken();
				}
				if(pT->t!=NUMERICVALUE) MadelineErrorReport("GraphAnnotation(): Numeric value expected for the vertical coordinate:\nGRAPH ADD [ARROW | LABEL | HORIZONTAL LINE ] <STRING_LABEL> AT [<HPOS> [ CENTIMORGANS | CENTIMETERS ]] , <VPOS> [ LODUNITS | CENTIMETERS ] ...");
				vpos=sign*pT->v;
				GetToken();
				/* Vertical units must be in centimeters or vertical (LOD) units: */
				if(!(pT->i==CM__K || pT->i==VU__K))
					MadelineErrorReport("GraphAnnotation(): Vertical coordinate must be measured in \ncentimeters (CM) or vertical units (VU or LODUNITS)");
				vunits=pT->i;
				
			}
			if(op.i==ARRWK){
				sign=1.0;
				GetToken();
				/*                                                                        */
				/* Read over a comma separating vertical coordinate from the angle value: */
				/*                                                                        */
				if(pT->i==',') GetToken();
				if(pT->i=='-'){
					sign=-1.0;
					GetToken();
				}
				/*                */
				/* Get the Angle: */
				/*                */
				if(pT->i==END){
					
					angle=45.0;
					
				}else{
					
					if(pT->t!=NUMERICVALUE)
						MadelineErrorReport("GraphAnnotation(): Numeric value expected for the angle:\nGRAPH ADD ARROW <STRING_LABEL> AT <H_POS>, <V_POS>, <ANGLE>");
					angle=sign*pT->v;
					GetToken();
					/* Angle must be in degrees */
					if(pT->i!=DEGRK)
						MadelineErrorReport("GraphAnnotation(): Keyword \"DEGREES\" expected after the angle:\nGRAPH ADD ARROW <STRING_LABEL> AT <H_POS>, <V_POS>, <ANGLE> DEGREES");
					
				}
			}
			/*                                         */
			/* Get here if everything is OK to store   */
			/*                                         */
			/* The string label is used as the key.    */
			/* The label type and positioning data     */
			/* are naturally the value (its all stored */
			/* as a string).  Lines require generation */
			/* of a key.                               */
			/*                                         */
			/* Store the KEY:                          */
			/*                                         */
			key.t=STRINGVALUE;
			if(op.i==HORZK){
				++g->psid;
				sprintf(buffer,"HORIZONTAL_LINE_%02i",g->psid);
				key.e.s=CopyString(buffer);
			}else if(op.i==VERTK){
				++g->isid;
				sprintf(buffer,"VERTICAL_LINE_%02i",g->isid);
				key.e.s=CopyString(buffer);
			}else{
				key.e.s=CopyString(pLabelToken->e);
			}
			/*                                           */
			/* Store the VALUE data strings:             */
			/*                                           */
			/* Note that the VALUE strings are exactly   */
			/* the Madeline PostScript commands required */
			/* to create the given annotation.  In this  */
			/* way, the program doesn't need to parse    */
			/* the value string again.  It just prints   */
			/* it out ...                                */
			/*                                           */
			value.t=STRINGVALUE;
			value.i=0;
			switch(op.i){
			case HORZK:
				
				sprintf(buffer,"%.2f %s HorizontalLine",vpos,(vunits==VU__K?"vu":"cm"));
				break;
				
			case VERTK:
				
				sprintf(buffer,"%.2f %s VerticalLine",hpos,(hunits==CMG_K?"cM":"cm"));
				break;
				
			case LABLK:
				
				sprintf(buffer,"%.2f %s %.2f %s (%s) Annotation",
					hpos,
					(hunits==CMG_K?"cM":"cm"),
					vpos,
					(vunits==VU__K?"vu":"cm"),
					key.e.s
				);
				break;
				
			case ARRWK:
				
				sprintf(buffer,"%.3f %s %.3f %s %.2f (%s) ArrowAnnotation",
					hpos,
					(hunits==CMG_K?"cM":"cm"),
					vpos,
					(vunits==VU__K?"vu":"cm"),
					angle,
					key.e.s
				);
				break;
				
			}
			value.e.s=CopyString(buffer);
			/*                              */
			/* Assign to associative array: */
			/*                              */
			AssignToAssociativeArray('=',&aaga,&key,&value);
			
			break;
			
		case BLACK:
		case BLUE :
		case BROWN:
		case CYAN :
		case DKGRY:
		case FRGRN:
		case GRAY :
		case GREEN:
		case LTGRY:
		case MAGEN:
		case ORANG:
		case PEACH:
		case PURPL:
		case RED  :
		case WHITE:
		case YELLW:
		case BAR_K:
			
			/*                                                         */
			/* If a color was specified, then get the \"BAR\" keyword. */
			/* Otherwise, a default gray color is used:                */
			/*                                                         */
			if(pT->i==BAR_K){
				
				color=(PVARIABLE) &gColorGray;
				
			}else{
				
				color=(PVARIABLE) pT->u.v;
				
				/*                        */
				/* Get the "BAR" Keyword: */
				/*                        */
				GetToken();
				if(pT->i!=BAR_K)
					MadelineErrorReport("GraphAnnotation(): Keyword \"BAR\" expected: GRAPH ADD [<COLOR>] BAR ...");
				
			}
			
			/*                       */
			/* Get the string label: */
			/*                       */
			GetToken();
			if(pT->t!=STRINGVALUE)
				MadelineErrorReport("GraphAnnotation(): Literal string value expected:\nGRAPH ADD [<COLOR>] BAR <STRING_LABEL> [ABOVE|BELOW] FROM ...");
			pLabelToken=pT; /* Save pointer to the label token */
			
			/*                                                  */
			/* Get the "ABOVE", "BELOW", AND/OR "FROM" keyword: */
			/*                                                  */
			GetToken();
			/*                               */
			/* "above" defaults to 1 (true): */
			/*                               */
			if(pT->i==ABOVK || pT->i==BELOK){
				
				if(pT->i==BELOK) above=0;
				
				GetToken();
				
			}
			
			if(pT->i!=FROMK)
				MadelineErrorReport("GraphAnnotation(): Keyword \"FROM\" expected:\nGRAPH ADD [<COLOR>] BAR <STRING_LABEL> [ABOVE|BELOW] FROM ...");

			/*                                       */
			/* Get the Horizontal STARTING position: */
			/*                                       */
			sign=1.0;
			GetToken();
			if(pT->i=='-'){
				sign=-1.0;
				GetToken();
			}
			if(pT->t!=NUMERICVALUE) MadelineErrorReport("GraphAnnotation(): Numeric value expected for the horizontal starting coordinate:\nGRAPH ADD [<COLOR>] BAR <STRING_LABEL> FROM <HSTT> TO <HEND> [ CENTIMORGANS | CENTIMETERS ] ...");
			hpos=sign*pT->v;
			
			/*                       */
			/* Get the "TO" keyword: */
			/*                       */
			GetToken();
			if(pT->i!=TOC)
				MadelineErrorReport("GraphAnnotation(): Keyword \"TO\" expected:\nGRAPH ADD [<COLOR>] BAR <STRING_LABEL> FROM <HSTT> TO <HEND> [ CENTIMORGANS | CENTIMETERS ] ...");
				
			/*                                     */
			/* Get the Horizontal ENDING position: */
			/*                                     */
			sign=1.0;
			GetToken();
			if(pT->i=='-'){
				sign=-1.0;
				GetToken();
			}
			if(pT->t!=NUMERICVALUE) MadelineErrorReport("GraphAnnotation(): Numeric value expected for the horizontal ending coordinate:\nGRAPH ADD [<COLOR>] BAR <STRING_LABEL> FROM <HSTT> TO <HEND> [ CENTIMORGANS | CENTIMETERS ] ...");
			hend=sign*pT->v;
			
			
			GetToken();
			/* Horizontal units must be in centimeters or centiMorgans: */
			if(!(pT->i==CM__K || pT->i==CMG_K))
				MadelineErrorReport("GraphAnnotation(): Horizontal coordinate must be measured in \ncentimeters (CM) or centiMorgans (CENTIMORGANS)");
			hunits=pT->i;
			
			
			/*                              */
			/* Get the Vertical position:   */
			/*                              */
			sign=1.0;
			GetToken();
			/*                                                                       */
			/* Read over a comma separating the horizontal and vertical coordinates: */
			/*                                                                       */
			if(pT->i==',') GetToken();
			if(pT->i=='-'){
				sign=-1.0;
				GetToken();
			}
			if(pT->t!=NUMERICVALUE) MadelineErrorReport("GraphAnnotation(): Numeric value expected for the vertical coordinate:\nGRAPH ADD [<COLOR>] BAR <STRING_LABEL> FROM <HSTT> TO <HEND> [ CENTIMORGANS | CENTIMETERS] , <VPOS> [ LODUNITS | CENTIMETERS ] ...");
			vpos=sign*pT->v;
			GetToken();
			/* Vertical units must be in centimeters or vertical (LOD) units: */
			if(!(pT->i==CM__K || pT->i==VU__K))
				MadelineErrorReport("GraphAnnotation(): Vertical coordinate must be measured in \ncentimeters (CM) or vertical units (VU or LODUNITS)");
			vunits=pT->i;
			
			/*                  */
			/* Store the KEY:   */
			/*                  */
			key.t=value.t=STRINGVALUE;
			key.e.s=CopyString(pLabelToken->e);
			/*                  */
			/* Store the VALUE: */
			/*                  */
			/* Example:                                          */
			/* 30 cM 50 cM 6.3 vu 0.5 cm ABOVE (PPMD) LabeledBar */
			/*                                                   */
			sprintf(buffer,"%s setrgbcolor %.3f %s %.3f %s %.3f %s 0.5 cm %s (%s) LabeledBar",
				color->e.s,
				hpos,
				(hunits==CMG_K?"cM":"cm"),
				hend,
				(hunits==CMG_K?"cM":"cm"),
				vpos,
				(vunits==VU__K?"vu":"cm"),
				(above?"ABOVE":"BELOW"),
				key.e.s
			);
			
			value.e.s=CopyString(buffer);
			/*                              */
			/* Assign to associative array: */
			/*                              */
			AssignToAssociativeArray('=',&aaga,&key,&value);
			
			break;
			
		default:
			
			MadelineErrorReport("Graph() ADD directive: unrecognized option.");
			break;	
	}
	
}

