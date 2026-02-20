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
/* "control.h" is part of the Madeline software distribution.                        */
/* Written by Edward H. Trager.                                                      */
/*                                                                                   */
/* COPYRIGHT (C) 2003 THE REGENTS OF THE UNIVERSITY OF MICHIGAN,                     */
/* Ann Arbor, Michigan, USA.  ALL RIGHTS RESERVED.                                   */
/*                                                                                   */
/* Copyright (C) 1996-2003 by Edward H. Trager and the FUSION Study Group            */
/* at the University of Michigan School of Public Health in Ann Arbor,               */
/* Michigan, USA.                                                                    */
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
/* Last modified by Ed Trager, Feb.  2003.                                           */
/*                                                                                   */
/* ********************************************************************************* */

/*                                           */
/* programcontrol.c                          */
/*                                           */

#include "programcontrol.h"
#include "madeline.h"
#include <readline/history.h>

PPROGRAMCONTROL PCStack=NULL;
PPROGRAMCONTROL pControlBlock=NULL;
int gScanning=0;

void ExecuteForStatement(void){
	
	extern PTOKENINFO pT; /* global pointer to the last token on the token stack */
	extern char *e;       /* global expression buffer                            */
	char *s;
	//RESULT result;
	
	extern PPROGRAMCONTROL PCStack;
	extern PPROGRAMCONTROL pControlBlock;
	
	/* Set up the program control stack if it doesn't exist */
	if(!PCStack){ 
		PCStack=NewProgramControlStack(PROGCTRLSTACKSIZE);
		/* One before start of object */
		pControlBlock=PCStack-1;
	}
	
	/* PUSH The stack: */
	PushProgramControlStack();
	
	/* Turn on the scanning level flag if the program */
	/* lines have not yet been scanned                */
	if(!pControlBlock->end) gScanning++;
	
	
	if(gScanning){
		
		/* Next token after FOR should be an opening parentheses: */
		GetToken();
		if(pT->i!='(') MadelineErrorReport("ExecuteForStatement(): Opening parentheses \"(\" expected.");
		
		/* Save the initial assignment: */
		for(s=pControlBlock->initialassignment;*e && *e!=';';*s++=*e++);
		/* terminate the string: */
		*s='\0';
		
		/* Go past the first ";" : we don't use GetToken() here because */
		/* GetToken() doesn't increment "e" when at the "END" (";")     */
		if(*e++!=';') MadelineErrorReport("ExecuteForStatement(): Semicolon \";\" expected to terminate the INITIAL ASSIGNMENT in the FOR statement");
		
		/* Save the condition expression string: */
		for(s=pControlBlock->condition;*e && *e!=';';*s++=*e++);
		/* terminate the string: */
		*s='\0';
		
		/* Check for semicolon, and skip past it: */
		if(*e++!=';') MadelineErrorReport("ExecuteForStatement(): Semicolon \";\" expected to terminate the CONDITIONAL expression of the FOR statement");
		
		/* Save the iterator expression string: */
		for(s=pControlBlock->nextiteration;*e && *e!=')';*s++=*e++);
		/* terminate the string: */
		*s='\0';
		
		/* Get the closing parentheses: */
		GetToken();
		if(pT->i!=')') MadelineErrorReport("ExecuteForStatement(): Closing parentheses \")\" expected to terminate the NEXT ITERATION expression of the FOR statement");
		/* Next token should be "{": */
		GetToken();
		if(pT->i!='{') MadelineErrorReport("ExecuteForStatement(): Opening curly brace \"{\" expected to mark START OF PROGRAM CONTROL BLOCK in the FOR statement on this line");
		
		/* Set the stt of the block (statements for     */
		/* this block actually start on the NEXT line): */
		pControlBlock->stt=where_history();
		
		/* For now, just print the conditions: */
		lprintf("INITIAL  : %s\n",pControlBlock->initialassignment);
		lprintf("CONDITION: %s\n",pControlBlock->condition);
		lprintf("NEXT     : %s\n",pControlBlock->nextiteration);
		
	}else{
		
		/* If not scanning, execute the loop: */
		
		ExecuteEndOfControlBlock();
		
	}
	
}


void ExecuteEndOfControlBlock(void){
	
	int i;
	extern PPROGRAMCONTROL pControlBlock;
	RESULT result;
	
	HIST_ENTRY **ProgramListing;
	
	if(gScanning){
		
		pControlBlock->end=where_history();
		
		/* DEBUG: Print the control loop: */
		//lprintf("--- END OF SCANNING BLOCK LEVEL %i FROM LINE STT=%i TO END=%i ---\n",
		//	pControlBlock->level,
		//	pControlBlock->stt,
		//	pControlBlock->end
		//);
		
		/* Decrement gScanning: */
		gScanning--;
		
	}
	
	if(!gScanning){
		
		/* EXECUTE THE CONTROL BLOCK: */
		
		ProgramListing=history_list();
		
		/* Interpret initial assignment: */
		Interpret(pControlBlock->initialassignment);
		
		/* Now loop: */
		for(;;){
			e=pControlBlock->condition;
			EvaluateExpression(&result);
			if(result.t!=NUMERICVALUE) MadelineErrorReport("ExecuteEndOfControlBlock(): Condition must evaluate to a numeric expression");
			if(result.v){
				/* execute block: */
				for(i=pControlBlock->stt+1;i<pControlBlock->end;i++) Interpret(ProgramListing[i]->line);
				/* Next iteration: */
				Interpret(pControlBlock->nextiteration);
			}else{
				break;
			}
		}
	}
	
	/* Pop the control stack: */
	PopProgramControlStack();
	
}

/*                          */
/* NewProgramControlStack() */
/*                          */
PPROGRAMCONTROL NewProgramControlStack(int size){
	
	PPROGRAMCONTROL p;
	int i;
	
	p=(PPROGRAMCONTROL) malloc((unsigned)size*sizeof(PROGRAMCONTROL));
	if(!p) MadelineErrorReport("NewProgramControlStack(): Unable to allocate program control stack of size %i!",size);
	
	for(i=0;i<size;i++){
		
		p[i].type=NOT_SET;
		p[i].initialassignment[0]='\0';
		p[i].condition[0]='\0';
		p[i].nextiteration[0]='\0';
		p[i].level=i+1;
		p[i].stt=0;
		p[i].end=0;
		
	}
	
	return p;
	
}

void PushProgramControlStack(void){
	
	extern PPROGRAMCONTROL pControlBlock;
	
	if(pControlBlock<PCStack+PROGCTRLSTACKSIZE) pControlBlock++;
	else MadelineErrorReport("PushProgramControlStack(): Out of stack space!");
	
}

void PopProgramControlStack(void){
	
	extern PPROGRAMCONTROL pControlBlock;
	
	if(pControlBlock>=PCStack) pControlBlock--;
	else MadelineErrorReport("PopProgramControlStack(): Already at beginning of stack!");
	
}
