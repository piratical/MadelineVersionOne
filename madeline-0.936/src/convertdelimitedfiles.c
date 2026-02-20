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
/* "convertdelimitedfiles.c" is part of the Madeline software distribution.          */
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

/*                                                                     */
/* convertdelimitedfiles.c -- originally called "cnvtdlm.c"            */
/*                                                                     */
/*                                                                     */
/* 19990226 init.                                                      */
/*                                                                     */
/* cnvtdlm.c provides functionality for converting tab, comma          */
/* or other delimited ASCII files to a space-delimited, column-aligned */
/* format that Madeline can recognize directly:                        */
/*                                                                     */
/*                                                                     */

#include "convertdelimitedfiles.h"

/*                                                                       */
/* ConvertFile(): Converts a comma-, tab-, or other-delimited file       */
/*                to the rectangular ASCII format that Madeline can use. */
/*                                                                       */
int ConvertFile(char *fin,char *fout,char dlm){

	char *b;

	int FieldCount;  /* count of fields                             */
	int *FieldWidth; /* vector to track maximum width of each field */
	int n;           /* number of records written                   */

        int i,j;
	
	FILE *fcout;

	/*                                  */
	/* Open and buffer input data file: */
	/*	                                 */
	b=OpenAndBufferFile(fin);


	/*                                                            */
	/* Determine how many fields are present in data file:        */
	/* The whole file is scanned in case there are irregularities */
	/* in some lines:                                             */
	/*	                                                           */
	FieldCount=GetFieldCount(b,dlm);
	
	if(!FieldCount){
		free(b);
		MadelineErrorReport("ConvertFile(): File is not delimited using \"%s\"",dlm);
	}

	/*                                                  */
	/* allocate an int vector to store field widths in: */
	/*                                                  */
	FieldWidth=NewFieldWidthVector(FieldCount);
	/*                                              */
	/* Rescan the file, now recording field widths: */
	/*	                                             */
	DetermineFieldWidths(b,FieldWidth,FieldCount,dlm);

	/*                                                */
	/* Let the user know what Madeline has found out: */
	/*                                                */
        lprintf("Data table has %i fields with the following widths:\n\n",FieldCount);
        lprintf("FLD|  1   2   3   4   5   6   7   8   9  10 \n");
        lprintf("---+--------------------------------------- \n");
        for(i=0,j=0;i+j<FieldCount;j+=10){
		lprintf("%3i|",j);
		for(i=0;i<10 && i+j<FieldCount;i++) lprintf("%3i ",FieldWidth[i+j]);
		lprintf("\n");
		i=0;
	}
	lprintf("\n");
        
	/*                                                       */
	/* Convert file to space-padded, space delimited format: */
	/*                                                       */
	fcout=fopen(fout,"wb");
	if(!fcout){
		free(b);
		free(FieldWidth);
		MadelineErrorReport("ConvertFile(): Unable to open \"%s\" for writing",fout);
	}
			
	n=WriteAlignedColumnFile(b,FieldWidth,FieldCount,dlm,fcout);
	
	/*                 */
	/* release memory: */
	/*                 */
	free(b);
	free(FieldWidth);
	
	return n;

}



/*                                                    */
/* GetFieldCount(): Determine number of fields by     */
/* counting how many times the delimiter appears in a */
/* CR/LF-delimited row (on Unix, there is no LF):     */
/*                                                    */
int GetFieldCount(char *buffer,char dlm){

	char *b;
	int i;
	int maxi;
	
	for(i=maxi=0,b=buffer;*b;b++){
		
		/*                           */
		/* count occurrences of dlm: */
		/*                           */
		if(*b==dlm) i++;
		/*                     */
		/* check for newlines: */
		/*                     */
		if(*b=='\r' || *b=='\n'){
			/*                                     */
			/* on DOS, skip past '\r' to the '\n': */
			/*                                     */
			if(*b=='\r') b++;
			
			/*                  */
			/* store only maxi: */
			/*                  */
			if(i>maxi) maxi=i;
			/*          */
			/* reset i: */
			/*          */
			i=0;
		}
	}
	/*                                                 */
	/* assuming no comma at end of the last field,     */
	/* then we need to add one to get the field count: */
	/*                                                 */
	return maxi+1;
}


/*                                                  */
/* NewFieldWidthVector(): allocates an int * vector */
/* for storing field widths:                        */
/*                                                  */
int *NewFieldWidthVector(int FieldCount){

	int *v;
	int i;
	
	v=(int *)malloc((unsigned)FieldCount*sizeof(int));
	if(!v){
		eprintf(FATALFLAG,"NewFieldWidthVector(): No memory for int vector");
	}
		
	for(i=0;i<FieldCount;i++) v[i]=0;
	
	return v;
}




/*                         */
/* Determine Field Widths: */
/*                         */
void DetermineFieldWidths(char *buffer,int *FieldWidth,int FieldCount,char dlm){

	char *b;
	
	int i;
	/* int w; */
	
	for(i=0,b=buffer;*b;){
		/*                                             */
		/* this function advances b a field at a time: */
		/*                                             */
		b=DetermineIndividualFieldWidth(b,(int *)(FieldWidth+i),dlm);
		/*                   */
		/* go to next field: */
		/*                   */
		i++;
		/*                                      */
		/* reset i before scanning next record: */
		/*		                                    */
		if(i==FieldCount) i=0;
	}		
}


/*                               */
/* DetermineIndividualFieldWidth */
/*                               */
char *DetermineIndividualFieldWidth(char *field,int *FieldWidth,char dlm){

	char *b;
	int   i;
	char qt;
	
	b=field;
	/*                                                          */
	/* Possibility of a text field surrounded by double quotes: */
	/*                                                          */
	if(*b==DOUBLE_QUOTE || *b==SINGLE_QUOTE){
		/*                       */
		/* Assume quoted string: */
		/*                       */
		qt=*b++;
		for(i=0;*b && *b!=qt && !(*b=='\r' || *b=='\n');b++,i++);
	
		if(*b!=qt){
			eprintf(FATALFLAG,"DetermineIndividualFieldWidth(): Can't find matching quote - corrupt file format");
		}
		
		/*                            */
		/* skip past the final quote: */
		/*                            */
		b++;
		
	}else{
		/*                                                             */
		/* not a quoted string, so just look for delimiter or newline: */
		/*                                                             */
		for(i=0;*b && *b!=dlm && !(*b=='\r' || *b=='\n');b++,i++);
		
	}
	/*                                                                         */
	/* should now be on delimiter, or if this is the last field on the line, a */
	/* '\r' or a '\n' newline:                                                 */
	/*                                                                         */
	if(!(*b==dlm || *b=='\n' || *b=='\r')){
		eprintf(FATALFLAG,"DetermineIndividualFieldWidth(): Delimiter or end of line expected - corrupt file format");
	}
	/*                                                                      */
	/* On a file from a DOS or Win95/98/NT system,                          */
	/* the EOL is always supposed to be a "\r\n" pair, so skip past the \r: */
	/*                                                                      */
	if(*b=='\r') b++;

	/*                                                        */
	/* skip past delimiter or newline to start of next field: */
	/*                                                        */
	b++;
	
	/*                                                               */
	/* store i if it is greater than the width value currently saved */
	/* for this field:                                               */
	/*                                                               */
	if(i>*FieldWidth) *FieldWidth=i;
			
	/*                                            */
	/* return pointer to beginning of next field: */
	/*                                            */
	return b;
}



/*                                                              */
/* WriteAlignedColumnFile(): returns number of records written: */
/*                                                              */
int WriteAlignedColumnFile(char *buffer,int *FieldWidth,int FieldCount,char dlm,FILE *fcout){

	char *b;
	int i;
	int j;

	for(j=i=0,b=buffer;*b;){
	
		/*                                                        */
		/* Write field of data: This function returns a pointer		 */
		/* to the start of the next field:                        */
		/*                                                        */
		b=WriteIndividualField(b,FieldWidth+i,dlm,fcout);
		
		i++;
		if(i>=FieldCount){
			/*          */
			/* reset i: */
			/*          */
			i=0;
			j++;
			/*                           */
			/* write end of record mark: */
			/*                           */
			fputc('\n',fcout);
		}
	}
	fclose(fcout);
	return j;
}


/*                      */
/* WriteIndividualField */
/*                      */
char *WriteIndividualField(char *field,int *FieldWidth,char dlm,FILE *fout){

	char *b;
	int i;
	char qt;
	
	b=field;
	/*                                                          */
	/* Possibility of a text field surrounded by double quotes: */
	/*                                                          */
	if(*b==DOUBLE_QUOTE || *b==SINGLE_QUOTE){
		/*                       */
		/* Assume quoted string: */
		/*                       */
		qt=*b++;
		for(i=0;*b && *b!=qt && !(*b=='\r' || *b=='\n');b++,i++) fputc(*b,fout);
			
		/*                              */
		/* skip past the closing quote: */
		/*                              */
		b++;
		
	}else{
		/*                                                             */
		/* not a quoted string, so just look for delimiter or newline: */
		/*                                                             */
		for(i=0;*b && *b!=dlm && !(*b=='\r' || *b=='\n');b++,i++) fputc(*b,fout);
		
	}
	/*                                                                            */
	/* should now be on delimiter, or if this is the last field on the line, on a */
	/* '\r' or '\n' newline:                                                      */
	/*                                                                            */

	/*                                                                      */
	/* On a file from a DOS or Win95/98/NT system,                          */
	/* the EOL is always supposed to be a "\r\n" pair, so skip past the \r: */
	/*                                                                      */
	if(*b=='\r') b++;

	/*                                                        */
	/* skip past delimiter or newline to start of next field: */
	/*                                                        */
	b++;

	/*                                                            */
	/* pad field with spaces out to width specified in FieldWidth */
	/*                                                            */
	for(;i<*FieldWidth;i++) fputc(' ',fout);
	/*                                                        */
	/* add one space to represent the inter-column delimiter: */
	/*                                                        */
	fputc(' ',fout);

			
	/*                                            */
	/* return pointer to beginning of next field: */
	/*	                                           */
	return b;
}

/* end of cnvtdlm.c */


