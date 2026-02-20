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
/* "logs.c" is part of the Madeline software distribution.                           */
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

/*                    */
/* logs.c             */
/*                    */
/* init: 19990316 ET  */
/*                    */
#ifndef LOGS_INCLUDED
#include "logs.h"
#endif

#ifndef DATE_INCLUDED
#include "date.h"
#endif

#include "version.h"  /* for printing version information */

#ifndef CONSOLE_INCLUDED
#include "console.h"
#endif

/*                                */
/*                                */
/* MadelineErrorReport():         */
/*                                */
/* - Prints syntax error messages */
/*   on stderr, also into the log */
/*   files:                       */
/*                                */
void MadelineErrorReport(const char *s,...){
	
	va_list argptr;
	char *stt;
	char *end;
	
	extern PCONTROL g;
	
	/*                                                           */
	/* Print header in error log if this is the very first error */
	/* encountered processing this database:                     */
	/*                                                           */
	PrintErrorLogHeader(g);

	/*                                               */
	/* record syntax errors in syntax error counter: */
	/*                                               */
	g->HasSyntaxErrors++;
	
	/* Reset the prompt: */
	g->ResetPrompt=1;

	va_start(argptr,s);
	vfprintf(stderr ,s,argptr);
	/*                                                 */
	/* send output to log, error and detail files too: */
	/*                                                 */
	if(g->streams.log)    vfprintf(g->streams.log   ,s,argptr);
	if(g->streams.detail) vfprintf(g->streams.detail,s,argptr);
	if(g->streams.err){
		fprintf(g->streams.err,"SYNTAX #%04i: ",g->HasSyntaxErrors);
		vfprintf(g->streams.err   ,s,argptr);
	}

	va_end(argptr);

	/*                                          */
	/* send output to log and detail files too: */
	/*                                          */
	fprintf(stderr,"\n");
	if(g->streams.log)    fprintf(g->streams.log   ,"\n");
	if(g->streams.detail) fprintf(g->streams.detail,"\n");
	if(g->streams.err)    fprintf(g->streams.err   ,"\n");
	
	/*                                                */
	/* show the expression and where it was in error: */
	/*                                                */
		
	/* if at the true end, back up: */
	if(e>estt && !*e) e--;
	/* if directly on a final '\n', change to terminating '\0': */
	if(*e=='\n') *e--='\0';
	
	/* back "e" up past all terminating white space: */
	while(e>estt && (*e==' ' || *e=='\t')) e--;
	/*                                             */
	/* locate the beginning of the current line of */
	/* the possibly multi-line expression:         */
	/*                                             */
	for(stt=e;stt>estt && *stt!='\n';stt--);
	if(*stt=='\n') stt++;
	/*                                       */
	/* locate the end of the current line of */
	/* the possibly multi-line expression:   */
	/*                                       */
	for(end=e;*end && *end!='\n';end++);
	if(*end=='\n') *end='\0'; /* terminate it at the new line */
	
	fprintf(stderr,"%s\n",stt);
	for(end=stt;end<e-1;end++) fputc(' ',stderr);
	fprintf(stderr,"^^^\n");
	
	/*                                                             */
	/* repeat the same thing for the log, error, and detail files: */
	/*                                                             */
	if(g->streams.log){
		fprintf(g->streams.log,"%s\n",stt);
		for(end=stt;end<e-1;end++) fputc(' ',g->streams.log);
		fprintf(g->streams.log,"^^^\n");
		fflush(g->streams.log);
	}
	if(g->streams.detail){
		fprintf(g->streams.detail,"%s\n",stt);
		for(end=stt;end<e-1;end++) fputc(' ',g->streams.detail);
		fprintf(g->streams.detail,"^^^\n");
		fflush(g->streams.detail);
	}
	if(g->streams.err){
		fprintf(g->streams.err,"%s\n",stt);
		for(end=stt;end<e-1;end++) fputc(' ',g->streams.err);
		fprintf(g->streams.err,"^^^\n");
		fflush(g->streams.err);
	}
		
	/*                                */
	/* Jump back to the command line: */
	/*                                */
	longjmp(BunnyHop,1);

}

/*                                                                   */
/*                                                                   */
/* dprintf(): prints to detail log and, if gVerbose, also to stdout: */
/*                                                                   */
void dprintf(const char *s,...){
	
	va_list argptr;
	
	extern PCONTROL g;

	va_start(argptr,s);

	if(g->Verbose){
	        vfprintf(stdout           ,s,argptr);
	}
	if(g->streams.detail){
		vfprintf(g->streams.detail,s,argptr);
		fflush(g->streams.detail);
	}

	va_end(argptr);
}

/*                                                          */
/*                                                          */
/* lprintf(): prints to summary log file, detail log file,  */
/* and also to stdout (unless gSilent):                     */
/*                                                          */
void lprintf(const char *s,...){
	
	va_list argptr;
	
	extern PCONTROL g;

	va_start(argptr,s);
	
	if(!g->Silent){
	        vfprintf(stdout           ,s,argptr);
	}
	if(g->streams.log){
		vfprintf(g->streams.log   ,s,argptr);
		fflush(g->streams.log);
	}
	if(g->streams.detail){
		vfprintf(g->streams.detail,s,argptr);
		fflush(g->streams.detail);
	}

	va_end(argptr);

}


/*                                                      */
/*                                                      */
/* eprintf(): prints to error log and also to stderr    */
/* of setting of verbose:                               */
/*                                                      */
/* flag: WARNINGFLAG or SEVEREWARNINGFLAG or ERRORFLAG  */
/* or FATALFLAG (which terminates application)          */
/*                                                      */
/* s   : format string                                  */
/* ... : parameters for format                          */
/*                                                      */
void eprintf(int flag,const char *s,...){
	
	va_list argptr;
	char DyingWords[]="\nMadeline cannot recover from the last error.  Goodbye!\n";

	extern PCONTROL g;

	/*                                                               */
	/* Print to error log.  If there have not been previous errors   */
	/* on this database, first print some informational stuff, i.e., */
	/* date, time, database name, flag status...                     */
	/* ...then print the error message:                              */
	/*                                                               */
	PrintErrorLogHeader(g);
	
	/* Print warning and error headers in red on stderr: */
	ConsoleRed(stderr);
	/*                                                   */
	/* Count number of errors or warnings, respectively: */
	/*                                                   */
	switch(flag){
	case WARNINGFLAG:
		g->HasWarnings++;
		if(g->Verbose)        fprintf(stderr           ,"WARNING #%04i: ",g->HasWarnings);
		if(g->streams.log)    fprintf(g->streams.log   ,"WARNING #%04i: ",g->HasWarnings);
		if(g->streams.detail) fprintf(g->streams.detail,"WARNING #%04i: ",g->HasWarnings);
		if(g->streams.err)    fprintf(g->streams.err   ,"WARNING #%04i: ",g->HasWarnings);
		break;
	case SEVEREWARNINGFLAG:
		g->HasSevereWarnings++;
		if(g->Verbose)        fprintf(stderr           ,"SEVERE #%04i: ",g->HasSevereWarnings);
		if(g->streams.log)    fprintf(g->streams.log   ,"SEVERE #%04i: ",g->HasSevereWarnings);
		if(g->streams.detail) fprintf(g->streams.detail,"SEVERE #%04i: ",g->HasSevereWarnings);
		if(g->streams.err)    fprintf(g->streams.err   ,"SEVERE #%04i: ",g->HasSevereWarnings);
		break;
	case INHERITANCEFLAG:
		g->HasInheritanceErrors++;
		if(g->Verbose)        fprintf(stderr           ,"INHERITANCE #%04i: ",g->HasInheritanceErrors);
		if(g->streams.log)    fprintf(g->streams.log   ,"INHERITANCE #%04i: ",g->HasInheritanceErrors);
		if(g->streams.detail) fprintf(g->streams.detail,"INHERITANCE #%04i: ",g->HasInheritanceErrors);
		if(g->streams.err)    fprintf(g->streams.err   ,"INHERITANCE #%04i: ",g->HasInheritanceErrors);
		break;
	case ERRORFLAG:
	case FATALFLAG:
		g->HasErrors++;
		if(g->Verbose)        fprintf(stderr           ,"ERROR #%04i: ",g->HasErrors);
		if(g->streams.log)    fprintf(g->streams.log   ,"ERROR #%04i: ",g->HasErrors);
		if(g->streams.detail) fprintf(g->streams.detail,"ERROR #%04i: ",g->HasErrors);
		if(g->streams.err)    fprintf(g->streams.err   ,"ERROR #%04i: ",g->HasErrors);
		break;	
	}
	ConsoleNormal(stderr);
	
	/*                                    */
	/* print error message in error file: */
	/*                                    */
	va_start(argptr,s);
	if(g->Verbose)        vfprintf(stderr           ,s,argptr);
	if(g->streams.log)    vfprintf(g->streams.log   ,s,argptr);
	if(g->streams.detail) vfprintf(g->streams.detail,s,argptr);
	if(g->streams.err)    vfprintf(g->streams.err   ,s,argptr);
	va_end(argptr);
	
	/*              */
	/* flush files: */
	/*              */
	if(g->streams.log)    fflush(g->streams.log);
	if(g->streams.detail) fflush(g->streams.detail);
	if(g->streams.err)    fflush(g->streams.err);
	
	/* Reset the prompt: */
	g->ResetPrompt=1;
	
	/*                  */
	/* FATAL FLAG CASE: */
	/*                  */
	if(flag==FATALFLAG){
		/*                 */
		/* say last words: */
		/*                 */
		ConsoleRed(stderr);
		if(g->Verbose)        fprintf(stderr           ,DyingWords);
		ConsoleNormal(stderr);
		if(g->streams.log)    fprintf(g->streams.log   ,DyingWords);
		if(g->streams.detail) fprintf(g->streams.detail,DyingWords);
		if(g->streams.err)    fprintf(g->streams.err   ,DyingWords);
		/*       */
		/* exit: */
		/*       */
		exit(1);
	}
	
}


/*                        */
/* PrintErrorLogHeader(): */
/*                        */
void PrintErrorLogHeader(PCONTROL c){
	/*                                */
	/* Only print if not yet printed: */
	/*                                */
	if(!(c->HasSyntaxErrors      ||
	     c->HasWarnings          ||
	     c->HasSevereWarnings    ||
	     c->HasInheritanceErrors || 
	     c->HasErrors)
	){
		if(c->streams.err){
			fprintf(c->streams.err,"\nMadeline v.%6.3f, %s\n",
				VERSION,
				DelimitedGenericDateString(JulianToday(),'.')
			);
			fprintf(c->streams.err,"Warnings or errors detected");
			if(c->db){
				/*                                              */
				/* Errors detected while processing a database: */
				/*                                              */
				fprintf(c->streams.err," while processing \"%s\":\n\n",
				gDatabaseFile.e.s);
			}else{
				/*                                              */
				/* Errors detected prior to opening a database: */
				/*                                              */
				fprintf(c->streams.err," prior to opening a pedigree table:\n\n");
			}
			fflush(c->streams.err);
		}
	}
}



/*                                                             */
/* LinePrint(): Divides a long string into lines:              */
/* -Choose lprintf,dprintf, or the like as the print function: */
/*                                                             */
#define LINEPRINTLENGTH 79  /*                                 */
void LinePrint(const char *s,void PrintFunction(const char *s,...)){

	const char *ss;
	char *hh;
	char hold[LINEPRINTLENGTH+3]; /* add three to give room for '\r' and '\n', plus null terminator */
	int i;
	int HasSpaces;

	/*                          */
	/* set ss to start of text: */
	/*                          */
	for(ss=s;*ss;){
		/*                                      */
		/* Search for '\n' before line length   */
		/* is exhausted: copy string into hold: */
		/*                                      */
		for(hh=hold,HasSpaces=i=0;*ss && *ss!='\n' && i<LINEPRINTLENGTH;*hh++=*ss++){
			/*                                  */
			/* flag tells whether spaces occur: */
			/*                                  */
			if(*ss==' ' || *ss=='\t') HasSpaces++;
			/*                                                        */
			/* when present, don't count '\r' as part of line length: */
			/*                                                        */
			if(*ss!='\r') i++;
		};
		/*                                                     */
		/* if already at line length, then '\n' did not occur: */
		/*                                                     */
		if(i==LINEPRINTLENGTH){
			/*                                   */
			/* back up to space if one occurred: */
			/*                                   */
			if(HasSpaces){
				for(;!(*ss==' ' || *ss=='\t');ss--,hh--);
				/*                                                  */
				/* Null terminate at hh: advance ss for next round: */
				/*                                                  */
				*hh='\0';
				ss++;
			}else{
				/*                                            */
				/* must cut line because there are no breaks: */
				/* Don't advance ss:                          */
				/*                                            */
				*hh='\0';			
			}		
		}else{
			/*                                                   */
			/* line break with '\n': need to also check for '\r' */
			/*                                                   */
			*hh--='\0';
			if(*hh=='\r') *hh='\0';
			ss++;
		}
		/*             */
		/* print hold: */
		/*             */
		PrintFunction("%s\n",hold);
	}
}

/* end of logs.c */
