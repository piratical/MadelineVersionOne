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
/* "assignment.c" is part of the Madeline software distribution.                     */
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
/* Last modified by Ed Trager, April 2002.                                           */
/*                                                                                   */
/* ********************************************************************************* */

/*                                                */
/* HISTORY:                                       */
/*                                                */
/* "assignment.c" was segregated out              */
/* of "madeline.c" on 2002.04.22 by ET in order   */
/* to make changes to Assignment() and            */
/* improve program maintainability                */
/*                                                */
/* Last modified: 2002.04.22 by ET                */
/*                                                */

#include "assignment.h"
#include "madeline.h"

/*                                                            */
/* Assignment: assign a value to a variable or array element: */
/*                                                            */
void Assignment(void){

	RESULT index;  /* for array index, if array present                  */
	RESULT result; /* result of expression to be assigned                */
	TOKENINFO op;  /* store the internal variable being referenced       */
	int expand=0;  /* equals 1 if need to expand array                   */
	short int AssignmentType;
	
	int noted=0;   /* flag for notifying user if converted to upper case           */
	int trunc=0;   /* flag for notifying user if truncated                         */
	int illegal=0; /* flag for notifying user if core field name has illegal chars */
	int len;       /* length of string to copy                                     */
	char *s;       /* pointer to string                                            */
	
	VARIABLE key;   /* for associative array */
	VARIABLE value; /* for associative array */
	
	extern PCONTROL g;
	
	
	/*                             */
	/* Initialize index and result */
	/*                             */
	result.t=index.t=0;
	result.v=index.v=DOUBLEMISSINGVALUE;
	result.s[0]=index.s[0]='\0';
	
	//
	// First token must be for either an *internal*
	// or *user*-created variable, array, or 
	// associative array:
	// 
	if(
	   pT->t==INTERNALVARIABLE ||
	   pT->t==INTERNALARRAY    ||
	   pT->t==ASSOCIATIVEARRAY ||
	   pT->t==USERVARIABLE     ||
	   pT->t==USERARRAY        ||
	   pT->t==USERASSOCIATIVEARRAY
	){
	
		/*                                                         */
		/* save the information from the global Token pointer, pT: */
		/*                                                         */
		op.i  =pT->i;
		op.t  =pT->t;
		op.u.p=pT->u.p;
	
		/*                                         */
		/* The next token can be '[' for an array: */
		/* otherwise, it should be '='             */
		/* for assignment:                         */
		/*                                         */
		/* --> Now (2000.09.23 ET) can also be     */
		/*     +=, -=, *=, or /= :                 */
		GetToken();
		if(pT->i=='['){
			/*                                                */
			/* Grab the array index or associative array key: */
			/*                                                */
			EvaluateParenthesizedExpression(&index);
			/*                          */
			/* IF it is a regular ARRAY */
			/*                          */
			switch(op.t){
			case INTERNALARRAY:
			case USERARRAY:
				
				if(index.t!=NUMERICVALUE)
					MadelineErrorReport("Assignment(): The array subscript must evaluate to numeric value.");
				if(index.v<1)
					MadelineErrorReport("Assignment(): The array subscript must be 1 or greater.");
				if(index.v>=op.u.a->n+1){
					/* must be equal to "n" to be the next adjacent free cell: */
					if(index.v>op.u.a->n+1)
						MadelineErrorReport("Assignment(): The next free cell in the array is at position %2i",op.u.a->n+1);
					expand++; /* flag for expanding array */
				}
				break;
				
			case ASSOCIATIVEARRAY:
			case USERASSOCIATIVEARRAY:
				
				/*                                                                               */
				/* Note that if the key type, "kt", is NULL, then keys of ANY type are accepted. */
				/* Pretty cool, eh?                                                              */
				/*                                                                               */
				if(op.u.aa->kt && index.t!=op.u.aa->kt){
					MadelineErrorReport("Assignment(): The key you specified is a %s value,\nbut this associative array only accepts %s keys.",
						ValueType(index.t),
						ValueType(op.u.aa->kt)
					);
				}
				break;
				
			default:
				
				MadelineErrorReport("Assignment(): Bracket can only follow an array or an associative array.");
				break;
				
			}
			/*                                             */
			/* the next token after the ']' should already */
			/* have been fetched by the call to            */
			/* EvaluateParenthesizedExpression() and will  */
			/* now hopefully be '=':                       */
			/*                                             */
		}
		/*                                                 */
		/* make sure it really is an assignment statement: */
		/*                                                 */
		AssignmentType=pT->i;
		if(!(   
		        AssignmentType=='=' 
		     || AssignmentType==PLUS_ASSIGN 
		     || AssignmentType==INCREMENT_ASSIGN
		     || AssignmentType==MINUS_ASSIGN
		     || AssignmentType==DECREMENT_ASSIGN     
		     || AssignmentType==MULTIPLY_ASSIGN
		     || AssignmentType==DIVIDE_ASSIGN
		)){
			MadelineErrorReport("Assignment(): Variable or array not followed by an assignment statement.");
		}
		
		/*                                                         */
		/* get value to assign, unless it is ++ or -- :            */
		/* EvaluateExpression() starts off with a GetToken()       */
		/* after clearing the token stack:                         */
		/*                                                         */
		if(!(AssignmentType==INCREMENT_ASSIGN || AssignmentType==DECREMENT_ASSIGN)){
			EvaluateExpression(&result);  /* get the result to assign: */
			
			/*                                                         */
			/* check if the type of the result is OK for the container */
			/* which will hold the result:                             */
			/*                                                         */
			switch(op.t){
			case INTERNALVARIABLE:
			case USERVARIABLE:
				if(op.u.v->t==FIELDNAMEVALUE){
					if(result.t!=STRINGVALUE){
						MadelineErrorReport("Assignment(): Specify a field name in quoted capitals for the assignment.");
					}
				}else{
					if(result.t!=op.u.v->t){
						MadelineErrorReport("Assignment(): The value you specified is a %s value,\nbut this variable only accepts %s values.",
						ValueType(result.t),
						ValueType(op.u.v->t)
						);
					}
				}
				break;
				
			case INTERNALARRAY:
			case USERARRAY:
				if(result.t!=op.u.a->t){
					MadelineErrorReport("Assignment(): The value you specified is a %s value,\nbut this array only accepts %s values.",
					ValueType(result.t),
					ValueType(op.u.a->t)
					);
				}
				break;
				
			case ASSOCIATIVEARRAY:
			case USERASSOCIATIVEARRAY:
				
				
				/*                                                                                   */
				/* Note that if the value type, "vt", is NULL, then values of ANY type are accepted. */
				/* Pretty cool, eh?                                                                  */
				/*                                                                                   */
				if(op.u.aa->vt && result.t!=op.u.aa->vt){
					MadelineErrorReport("Assignment(): The value you specified is a %s value,\nbut this associative array only accepts %s values.",
						ValueType(result.t),
						ValueType(op.u.aa->vt)
					);
				}
				break;
				
			default:
				
				MadelineErrorReport("Assignment(): Unrecognized data container."); 
				break;
				
			}
		}
		
		/*                                 */
		/* get here if it is OK to assign: */
		/*                                 */
		
		switch(op.t){
		case INTERNALVARIABLE:
		case USERVARIABLE:
			
			if(op.u.v->t==NUMERICVALUE){
				
				AssignNumericValue(AssignmentType,&op.u.v->e.v,result.v);
				
				if(gDrawing && (op.u.v==&LabelFontSize || op.u.v==&LegendFontSize) ){
					/*                            */
					/* Free the current pedigree  */
					/* graphics set-up, so that   */
					/* DrawPedigree() will create */
					/* a new set-up employing     */
					/* the modified font sizes:   */
					/*                            */
					FreeDrawing(gDrawing);
					gDrawing=NULL;
				}
				
			}else if(op.u.v->t==STRINGVALUE || op.u.v->t==FIELDNAMEVALUE){
				
				if(AssignmentType!='=')
					MadelineErrorReport("Assignment(): +=, -=, *=, /=, ++ and -- assignment operators \nare not supported for character string values.");
				
				/*                                                                */
				/* if it is a change to the logfile or detail file or error file, */
				/* then change the log file and copy over the old contents:       */
				/*                                                                */
				if(op.u.v==&gAllLogFiles || op.u.v==&gLogFile || op.u.v==&gDetailFile || op.u.v==&gErrorFile || op.u.v==&gCommandFile){
					if(op.u.v==&gAllLogFiles){
						/*                                                 */
						/* Reassign all three log files in one fell swoop: */
						/* Notice that this requires not only that we call */
						/* "ReassignFile()", but that we also change the   */
						/* file name in the internal variables.            */
						
						/* Summary log file: */
						s=ModifyFileName(result.s,".log");
						g->streams.log    = ReassignFile(g->streams.log,gLogFile.e.s,s);
						if(gLogFile.e.s) free(gLogFile.e.s);
						gLogFile.e.s=CopyString(s);
						
						/* Detail log file: */
						s=ModifyFileName(result.s,".dtl");
						g->streams.detail = ReassignFile(g->streams.detail,gDetailFile.e.s,s);
						if(gDetailFile.e.s) free(gDetailFile.e.s);
						gDetailFile.e.s=CopyString(s);
						
						/* Error log file: */
						s=ModifyFileName(result.s,".err");
						g->streams.err    = ReassignFile(g->streams.err,gErrorFile.e.s,s);
						if(gErrorFile.e.s) free(gErrorFile.e.s);
						gErrorFile.e.s=CopyString(s);
						
						/* Command log file: */
						s=ModifyFileName(result.s,".cmd");
						g->streams.command= ReassignFile(g->streams.command,gCommandFile.e.s,s);
						if(gCommandFile.e.s) free(gCommandFile.e.s);
						gCommandFile.e.s=CopyString(s);
						
						lprintf("All log files now have \"%s\" as the base name:\n",result.s);
						lprintf("\tLogFile    = \"%s\"\n",gLogFile.e.s);
						lprintf("\tDetailFile = \"%s\"\n",gDetailFile.e.s);
						lprintf("\tErrorFile  = \"%s\"\n",gErrorFile.e.s);
						lprintf("\tCommandFile= \"%s\"\n",gCommandFile.e.s);
						
					}else if(op.u.v==&gLogFile){
						/*                      */
						/* reassigned log file: */
						/*                      */
						g->streams.log=ReassignFile(g->streams.log,op.u.v->e.s,result.s);
						lprintf("LogFile has been changed from \"%s\" to \"%s\"\n",op.u.v->e.s,result.s);
					}else if(op.u.v==&gDetailFile){
						/*                         */
						/* reassigned detail file: */
						/*                         */
						g->streams.detail=ReassignFile(g->streams.detail,op.u.v->e.s,result.s);
						lprintf("DetailFile has been changed from \"%s\" to \"%s\"\n",op.u.v->e.s,result.s);
					}else if(op.u.v==&gErrorFile){
						/*                        */
						/* reassigned error file: */
						/*                        */
						g->streams.err=ReassignFile(g->streams.err,op.u.v->e.s,result.s);
						lprintf("ErrorFile has been changed from \"%s\" to \"%s\"\n",op.u.v->e.s,result.s);
					}else{
						/*                          */
						/* reassigned command file: */
						/*                          */
						g->streams.command=ReassignFile(g->streams.command,op.u.v->e.s,result.s);
						lprintf("CommandFile has been changed from \"%s\" to \"%s\"\n",op.u.v->e.s,result.s);
					}
				}
				/*                   */
				/* Do the assignment */
				/*                   */
				
				/*free current, if non-null: */
				if(op.u.v->e.s) free(op.u.v->e.s);
				/*                                            */
				/* string length is limited if FIELDNAMEVALUE */
				/*                                            */
				len=strlen(result.s);
				if(op.u.v->t==FIELDNAMEVALUE && len>LNFIELDNAME-1){
					len=LNFIELDNAME-1;
					trunc++;
				}
				/* malloc: */
				op.u.v->e.s=(char *)malloc(len+1);
				strncpy(op.u.v->e.s,result.s,len);
				/* make sure it is null-terminated correctly: */
				op.u.v->e.s[len]='\0';
				/*                                            */
				/* If it is a FIELDNAMEVALUE, make sure it is */
				/* capitalized:                               */
				/*                                            */
				if(op.u.v->t==FIELDNAMEVALUE){
					for(s=op.u.v->e.s;*s;s++){
						if(islower(*s)){
							*s=toupper(*s);
							noted++;
						}
						if(isspace(*s) || iscntrl(*s)){
							illegal++;
						}
					}
				}
				/*                     */
				/* Show flag messages: */
				/*                     */
				if(trunc && noted) eprintf(WARNINGFLAG,"Field name assignment has been truncated and capitalized to \"%s\".\n",op.u.v->e.s);
				else if(noted)     eprintf(WARNINGFLAG,"Field name assignment has been capitalized to \"%s\".\n",op.u.v->e.s);
				else if(trunc)     eprintf(WARNINGFLAG,"Field name assignment has been truncated to \"%s\".\n",op.u.v->e.s);
				if(illegal)        eprintf(SEVEREWARNINGFLAG,"Field name assignment contains %i illegal character%s!\n",illegal,illegal>1?"s":"");

				/*                                                     */
				/* if it is a FIELDNAMEVALUE _AND_ a database is open, */
				/* then store the field index right away:              */
				/*                                                     */
				if(op.u.v->t==FIELDNAMEVALUE && g->db){
					op.u.v->i=g->db->FieldNumber(g->db,op.u.v->e.s);
					/*                                              */
					/* Call SetknownFieldFieldFlags() to update any */
					/* changes to the known fields:                 */
					/*                                              */
					SetKnownFieldFieldFlags(g->db);   
				}
			}
			break;
			
		case INTERNALARRAY:
		case USERARRAY:
			
			if(expand){
				op.u.a->n++;
				if(op.u.a->n>op.u.a->m)
					IncreaseArray(op.u.a);
			}

			if(op.u.a->t==NUMERICVALUE){
				
				AssignNumericValue(AssignmentType,&op.u.a->e[(int)index.v-1].v,result.v);
				
			}else if(op.u.a->t==STRINGVALUE || FIELDNAMEVALUE){
				
				if(AssignmentType!='=')
					MadelineErrorReport("Assignment(): +=, -=, *=, /=, ++ and -- assignment operators \nare not supported for character string values");

				/* remove (free allocation of) current string, if present: */
				if(op.u.a->e[(int)index.v-1].s) free(op.u.a->e[(int)index.v].s);
				/* string length is limited if FIELDNAMEVALUE: */
				len=(op.u.a->t==FIELDNAMEVALUE?LNFIELDNAME-1:strlen(result.s));
				/* allocate space for new string: */
				op.u.a->e[(int)index.v-1].s=(char *)malloc(len+1);
				/* copy in new string: */
				strncpy(op.u.a->e[(int)index.v-1].s,result.s,len);
				/* make sure it is null-terminated correctly: */
				op.u.a->e[(int)index.v-1].s[len]='\0';
			}
			break;
			
		case ASSOCIATIVEARRAY:
		case USERASSOCIATIVEARRAY:
			
			/* Set up the KEY */
			key.t=index.t;
			key.i=0;
			if(key.t==NUMERICVALUE || key.t==DATEVALUE) key.e.v=index.v;
			else                                        key.e.s=CopyString(index.s);
			
			/* Set up the VALUE */
			value.t=result.t;
			value.i=0;
			if(value.t==NUMERICVALUE || value.t==DATEVALUE) value.e.v=result.v;
			else                                            value.e.s=CopyString(result.s);
			
			/* Call the assignment function in associativearrays.c: */
			AssignToAssociativeArray(AssignmentType,op.u.aa,&key,&value);
			
			break;
			
		}
		
	}else if(pT->i!=END){
		
		MadelineErrorReport("Assignment(): Not an assignment statement");
		
	}
}


/*                       */
/* AssignNumericValue(): */
/*                       */
void AssignNumericValue(short int AssignmentType, double *pdestination, double source){

	switch(AssignmentType){
	case '=':
		*pdestination=source;
		break;
	case PLUS_ASSIGN:
		*pdestination += source;
		break;
	case MINUS_ASSIGN:
		*pdestination -= source;
		break;
	case INCREMENT_ASSIGN:
		*pdestination += 1.0;
		break;
	case DECREMENT_ASSIGN:
		*pdestination -= 1.0;
		break;
	case MULTIPLY_ASSIGN:
		*pdestination *= source;
		break;
	case DIVIDE_ASSIGN:
		if(source==0) MadelineErrorReport("AssignNumericValue(): Division by zero is not permitted.");
		*pdestination /= source;
		break;
	}
}


/*                                                        */
/* MapAssignmentCommand(): Assign to an associative array */
/* using the "MAP <ARRAY_NAME> <KEY> AS <VALUE>" syntax   */
/*                                                        */
void MapAssignmentCommand(void){
	
	VARIABLE key;   
	VARIABLE value;
	TOKENINFO op;
	
	extern PTOKENINFO pT;
	
	/*                                                     */
	/* First token needs to refer to an associative array: */
	/*                                                     */
	GetToken();
	if(!(pT->t==ASSOCIATIVEARRAY || pT->t==USERASSOCIATIVEARRAY))
		MadelineErrorReport("MapAssignmentCommand(): Name of an associative array expected:\nMAP <ARRAY_NAME> <ARRAY_KEY> AS|TO <ARRAY_VALUE>.");
	
	/* Save the associative array pointer: */
	op.i  =pT->i;
	op.t  =pT->t;
	op.u.p=pT->u.p;
	
	/*             */
	/* Get the KEY */
	/*             */
	GetToken();
	key.t=pT->t;
	key.i=0;
	if(key.t==NUMERICVALUE || key.t==DATEVALUE) key.e.v=pT->v;
	else                                        key.e.s=CopyString(pT->e);
	/*                                                                               */
	/* CHECK KEY TYPE                                                                */
	/* --------------                                                                */
	/* Note that if the key type, "kt", is NULL, then keys of ANY type are accepted. */
	/* Pretty cool, eh?                                                              */
	/*                                                                               */
	if(op.u.aa->kt && key.t!=op.u.aa->kt){
		MadelineErrorReport("MapAssignmentCommand(): The key you specified is a %s value,\nbut this associative array only accepts %s keys.",
			ValueType(key.t),
			ValueType(op.u.aa->kt)
		);
	}
	
	GetToken();
	/* Should be the word "AS" or the word "TO" */
	if(!(pT->i==AS_KY || pT->i==TOC))
		MadelineErrorReport("MapCommand(): Keyword \"AS\" or \"TO\" expected:\nMAP <ARRAY_NAME> <ARRAY_KEY> AS|TO <ARRAY_VALUE>.");
	

	/*                  */
	/* Set up the VALUE */
	/*                  */
	GetToken();
	value.t=pT->t;
	value.i=0;
	if(value.t==NUMERICVALUE || value.t==DATEVALUE) value.e.v=pT->v;
	else                                            value.e.s=CopyString(pT->e);
	/*                                                                                   */
	/* CHECK VALUE TYPE                                                                  */
	/* ----------------                                                                  */
	/* Note that if the value type, "vt", is NULL, then values of ANY type are accepted. */
	/* Pretty cool, eh?                                                                  */
	/*                                                                                   */
	if(op.u.aa->vt && value.t!=op.u.aa->vt){
		MadelineErrorReport("MapAssignmentCommand(): The value you specified is a %s value,\nbut this associative array only accepts %s values.",
			ValueType(value.t),
			ValueType(op.u.aa->vt)
		);
	}
	
	/*                                 */
	/* get here if it is OK to assign: */
	/*                                 */
	
	/*                                                      */
	/* Call the assignment function in associativearrays.c: */
	/*                                                      */
	AssignToAssociativeArray('=',op.u.aa,&key,&value);
	
}






