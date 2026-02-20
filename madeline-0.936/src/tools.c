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
/* "tools.c" is part of the Madeline software distribution.                          */
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
/* Last modified by Ed Trager, August 2002.                                          */
/*                                                                                   */
/* ********************************************************************************* */

/*
   tools.c
   
   set of rather general functions that are useful in many different
   programs.  This version has been adapted for specific use in 
   Madeline.
   
   history
   -------
   
   970219: initiated

*/

#include "tools.h"

/* for Madeline error reporting and CONTROL structure: */
#include "logs.h"

/* for tokentypes */
#ifndef TOKENTYPES_INCLUDED
#include "tokentypes.h"
#endif

/* for the modf() function used by Round() */
#include <math.h>

/* for POSIX file access functionality: */
#include <unistd.h>

/*
  FileCopy:
*/
void FileCopy(FILE *in,FILE *out){
	int c;

	while((c=getc(in))!=EOF) putc(c,out);
}


/*
 
  IsEmpty() returns 1 if a string is all white space characters or NULL string

*/
int IsEmpty(char *s){

	if(s==NULL) return 1;
	for(;*s && (*s==' ' || *s=='\t' || *s=='\n' || *s=='\r');s++);

	return !(*s);
}

/*
   Find(): searches for t in s and returns a pointer to the first occurrence of t in s, or
   NULL if not found.  Note that the returned pointer points to the BEGINNING of t in s, not
   to the end.
*/
char *Find(char *t,char *s){

	char *tt;
	char *ss;
	char *sss;

	for(ss=s;*ss;ss++){
		if(*t==*ss){
			/*                                                   */
			/* first letter matches: see if rest matches or not: */
			/*                                                   */
			for(tt=t,sss=ss;*tt && *tt==*sss;tt++,sss++);
			/*                        */
			/* match if tt exhausted: */
			/*                        */
			if(!*tt) return ss;
		}
	}
	return NULL;
}


/*
   FindEnd(): searches for t in s and returns a pointer to the first occurrence of t in s, or
   NULL if not found.  Note that the returned pointer points to ONE CHARACTER PAST THE END of t
   in s, not to the beginning, and thus differs from the Find() function in this respect.
*/
char *FindEnd(char *t,char *s){
	
	char *tt;
	char *ss;
	char *sss;
  
	for(ss=s;*ss;ss++){
		if(*t==*ss){
			/*                                                   */
			/* first letter matches: see if rest matches or not: */
			/*                                                   */
			for(tt=t,sss=ss;*tt && *tt==*sss;tt++,sss++);
			/*                        */
			/* match if tt exhausted: */
			/*                        */
			if(!*tt) return sss;
		}
	}
	return NULL;
}  


/*

  OpenAndBufferFile(): opens a file and reads the file
  in its entirety into newly allocated memory
  and returns a pointer to the memory buffer
  
*/

char *OpenAndBufferFile(const char *filename){

	FILE *fp;
	char *buf;
	size_t size;

	/* open file */ 
	fp=fopen(filename,"rb");
	if(fp==NULL)
		MadelineErrorReport("OpenAndBufferFile(): Unable to open file %s",filename);
	
	/* get buffer space */
	size=FileSize(fp);
	buf=(char *)malloc((unsigned)size+1);
	if(buf==NULL)
		MadelineErrorReport("OpenAndBufferFile(): Unable to allocate buffer");
	
	/* if size>0, read & close */
	if(size>0) fread(buf,size,1,fp);
	buf[size]='\0';
	fclose(fp);
	
	/* return pointer */
	return buf;
}


/*
   FileSize(): returns size of an open file in bytes
*/
long FileSize(FILE *fp){
	long curpos,length;
	
	/* retain current cursor position         */
	/* (which need not be the file beginning) */
	curpos=ftell(fp);
	/* seek to end of file */
	fseek(fp,0L,SEEK_END);
	/* get the offset at the end */
	length=ftell(fp);
	/* restore saved cursor position */
	fseek(fp,curpos,SEEK_SET);
	if(length==-1L) MadelineErrorReport("FileSize(): failed to determine size of file");
	return length;
}


/*                                            */
/* ToolsErrorReport: error reporting function */
/*                                            */
void ToolsErrorReport(char *s,...){

	va_list argptr;
	
	fprintf(stderr,"Tools error report:\n");
	va_start(argptr,s);
	vfprintf(stderr,s,argptr);
	va_end(argptr);
	fprintf(stderr,"\n");
	exit(1);

};


/*                                                  */
/* File(): returns 1 if file can be opened, else 0: */
/*                                                  */
int File(const char *fn){
	
	return (access(fn,F_OK)==0);
	
}


/*                   */
/* UnderscoredNumber */
/*                   */
char *UnderscoredNumber(int v,int w){

	char *bb;
	static char b[25];

        if(!v){
                /*                            */
                /* show only spaces for zero: */
                /*                            */
                bb=b+w;
                *bb--='\0';
                for(;bb>b;*bb--=' ');
                *bb=' ';
        }else{
                /*                                 */
                /* format number with underscores: */
                /*                                 */
                sprintf(b,"%*i",w,v); 
                for(bb=b;*bb && *bb==' ';*bb++='_'); 
        }
        return b;
}
	

/*                                                      */
/* ModifyFileName()                                     */
/*                                                      */
/* original: pointer to original file name              */
/* extension : extension to add to the end of the file  */
/* name.                                                */
/*                                                      */
/* NOTE BENE: The original version of this function     */
/* took a DOS-based approach: If the file had a three-  */
/* letter DOS-style extension, that was replaced by the */
/* new three-letter extension.  This is no longer the   */
/* case.  The program now doesn't care what the file    */
/* name is nor what extension it already has or doesn't */
/* have.                                                */
/*                                                      */
/* This function just adds the extension to the end of  */
/* the original file name                               */
/*                                                      */
char *ModifyFileName(const char *original,const char *extension){

	static char b[MINIMUMBUFFERSIZE];
	int ExtensionLimit;
	
	strncpy(b,original,MINIMUMBUFFERSIZE-1);
	if(b[MINIMUMBUFFERSIZE-1]) b[MINIMUMBUFFERSIZE-1]='\0';
	ExtensionLimit=MINIMUMBUFFERSIZE-1-strlen(b);
	strncat(b,extension,ExtensionLimit);
	if(b[MINIMUMBUFFERSIZE-1]) b[MINIMUMBUFFERSIZE-1]='\0';
	/*                        */
	/* return static pointer: */
	/*                        */
	return b;
}


/*                       */
/* CopyString() function */
/*                       */
char *CopyString(const char *s){
	
	char *t,*tt;
	int len;
	
	len=strlen(s)+1;
	t=(char *)malloc((unsigned)len*sizeof(char));
	if(!t) ToolsErrorReport("CopyString(): unable to allocate memory for string");
	/*            */
	/* copy:      */
	/*            */
	for(tt=t;*s;*tt++=*s++);
	*tt='\0';
	
	return (char *) t;
}


/*            */
/* ToUpper(): */
/*            */
char *ToUpper(char *s){

	for(;*s;s++)
		if(isalpha(*s)) *s=toupper(*s);
	
	return s;
}


/*                                                   */
/* GenerateRandomID():                               */
/*                                                   */
/* -> s[] must be able to hold eight letters plus    */
/*    terminating '\0':                              */
/*                                                   */
/* -> Randomly generated values are in the ASCII     */
/*    character set decimal range of 48-90 ("0"-"Z") */
/*    excluding the transition range of 7 values     */
/*    spanning decimal 58-64 (":"-"@")               */
/*                                                   */
/* -> In order to make them look really different    */
/*    from regular IDs, randomly generated IDs start */
/*    and end with an exclamation point, "!"         */
/*                                                   */
char *GenerateRandomID(char s[]){

	int i;
	
	s[0]='!';
	for(i=1;i<7;i++)
		/* skip ASCII 58-40 inclusive: */
		while((s[i]='0'+rand()%43)>=':' && s[i]<='@');
	/* terminate string: */
	s[7]='!';
	s[8]='\0';

	return s;

}


/*                                              */
/* IsFusionID(): Very crude check whether an ID */
/*               looks like a FUSION ID         */
/*                                              */
int IsFusionID(char *s){

	int v;
	
	extern PCONTROL g;
	
	/*                                                            */
	/* Automatically return FALSE if FusionSupport is turned OFF: */
	/*                                                            */
	if(!g->FusionSupport) return 0;
	
	/*                                                                    */
	/* 19991015 ET Addendum:                                              */
	/*                                                                    */
	/* s[8]='\0' means that the ID must be exactly 8 characters long,     */
	/* so IDs that are longer or shorter are immediately known to not     */
	/* be FUSION IDs                                                      */
	/*                                                                    */
	/* Note: a '9' in the first position of a FUSION STUDYID represents   */
	/* an analytically constructed ID used when part of a family has been */
	/* split off from another family.  Otherwise, 0, 1, C, and T are the  */
	/* only valid first digits.                                           */
	/*                                                                    */
	/* A-Z in the fifth position is allowed for RESAMPLED IDs: Otherwise, */
	/* the fifth character must be "-" or "+"                             */
	/*                                                                    */
	/* Digits 0-9 are allowed in the 6th position.  No other checks are   */
	/* performed.                                                         */
	/*                                                                    */
	return  s[8]=='\0' &&
                (s[0]=='9' || s[0]=='0' || s[0]=='1' || s[0]=='C' || s[0]=='T')
		&& ( (s[4]=='-' || s[4]=='+') || (s[4]>='A' && s[4]<='Z') )
		&& (v=s[5]-'0')>=1 
		&& v<=9;

}

char * ValueType(unsigned short type){

	static char n[]="numeric";
	static char d[]="date";
	static char s[]="character string";
	static char u[]="unknown";
	static char a[]="any";
	
	switch(type){
	case NUMERICVALUE:
		return n;
	case DATEVALUE:
		return d;
	case STRINGVALUE:
		return s;
	case 0:
		return a;
	default:
		return u;
	}
}


/*                                                                      */
/*  Itoa()                                                              */
/*  Converts an integer into an alphanumeric string stored in b         */
/*  of total width w. The converted numeric string has comma separators */
/*  where they should be.                                               */
/*                                                                      */
char *Itoa(char *b,int v,int w){

	int i;
	char t[25];
	char *tt,*bb;

	/*                                                        */
	/*use sprintf for conversion - no separators in string t: */
	/*                                                        */
	sprintf(t,"%*i",w,v);

	/*                                                                         */
	/* copy from right, inserting comma after every third digit, as necessary: */
	/*                                                                         */
	*(bb=b+w)='\0';
	for(tt=t+w-1,bb--,i=0;bb>=b && tt>=t && !(*tt==' ' || *tt=='-');i++){
		if(i && i%3==0 && !(*tt==' ' || *tt=='-')) *bb--=',';
		*bb--=*tt--;
	}
	if(*tt=='-') *bb--=*tt--;
	/*                                               */
	/* pad remaining left side, if any, with spaces: */
	/*                                               */
	if(bb>b){
		for(;bb>b;*bb--=' ');
		*bb=' ';
	}
	return b;        
}

/*               */
/* Store String: */
/*               */
void StoreString(PVARIABLE v,const char *s){

	unsigned len;
	
	/*free current, if non-null: */
	if(v->e.s)
		free(v->e.s);
	/* string length is limited if FIELDNAMEVALUE */
	len=(unsigned)(v->t==FIELDNAMEVALUE?LNFIELDNAME-1:strlen(s));           
	/* malloc: */
	v->e.s=(char *)malloc((unsigned)len+1);
	strncpy(v->e.s,s,len);
	/* make sure it is null-terminated correctly: */
	v->e.s[len]='\0';
}

/*                                              */
/* FormatNumber(): format a number for printing */
/*                                              */
char *FormatNumber(double v,char *hext,int wide,int prec){

	static char hint[26];
	char *b;
	int result;
	int diff;
	
	/*                                                  */
	/* Use external buffer if passed, else use internal */
	/* buffer:                                          */
	/*                                                  */
	b=hext?hext:(char *)&hint;
	
	result=sprintf(b,"%*.*f",wide,prec,v);
	if(result>wide){
		diff=result-wide;
		//if(diff>prec)
		//	MadelineErrorReport("FormatNumber(): %lf can't be printed with a width of only %i",v,wide);
		/*                                                                   */
		/* Get here if we can squish the remaining number into format space: */
		/*                                                                   */
		eprintf(WARNINGFLAG,"FormatNumber(): WARNING: loss of %i digit%s of requested (%i.%i) precision printing %lf\n",diff,diff>1?"s":"",wide,prec,v);
		/*                                               */
		/* reformat with loss of precision and rounding: */
		/*                                               */
		sprintf(b,"%*.*f",wide,prec-diff,v);
	}
	return b;
}

/*                                    */
/* SplitPath: Splits file name into   */
/* Path and Name components (on UNIX) */
/*                                    */
void SplitPath(const char *FileName,char **PointerToPath,char **PointerToName){
	
	const char *Name;
	int PathLength;
	
	/* find the last slash: */
	for(Name=FileName+strlen(FileName)-1;Name>=FileName && *Name!='/';Name--);
	/* Increment Name so that it points to the beginning of the file name: */
	Name++;
	*PointerToName=strdup(Name);
	if(!*PointerToName) ToolsErrorReport("SplitPath(): Unable to allocate memory for Name.");
	
	PathLength=Name-FileName;
	*PointerToPath=(char *)malloc((unsigned)(PathLength+1)*sizeof(char));
	if(!*PointerToPath) ToolsErrorReport("SplitPath(): Unable to allocate memory for Path.");
	/* copy:      */
	strncpy(*PointerToPath,FileName,PathLength);
	*(*PointerToPath+PathLength)='\0';
	
}

char *Properize(char *s){
	
	char *ss;
	int toggle=1;
	
	for(ss=s;*ss;ss++){
		/* If the toggle is on, capitalize the letter: */
		if(toggle){
			if(islower(*ss)) *ss=toupper(*ss);
			--toggle;
		}else{
			if(isupper(*ss)) *ss=tolower(*ss);
		}
		/* Turn on the toggle for capitalization on the next iteration: */
		/* Capitalize letters immediately following */
		/* a space, dash, or underscore:            */
		if(isspace(*ss) || *ss=='-' || *ss=='_') ++toggle;
	}
	
	return s;
	
}

/*                                   */
/* ShowBits() is used for debugging: */
/*                                   */
/* -> trans. from madeline.c         */
/*                                   */
void ShowBits(unsigned int u){
	unsigned int m=0;
	m=~(~m>>1);
	for(;m;m>>=1) printf("%c",u&m?'1':'0');
	printf("\n");
}

/*                                               */
/* PairCount(): Returns the number of *pairings* */
/* that can be constructed from a set of         */
/* "IndividualCount" individuals.                */
/*                                               */
int PairCount(int IndividualCount){
	
	int i,s;
	
	if(IndividualCount<2) return 0;
	
	for(s=0,i=IndividualCount-1;i;i--) s+=i;
	return s;
	
}

/*                                                   */
/* ReassignFile() copies the contents of fo, the old */
/* file, into a new file, closes the old file,       */
/* and returns the FILE * of the new file:           */
/*                                                   */
FILE *ReassignFile(FILE *oldfp,char *old,char *newfile){

	FILE *newfp;
	char *b;
	size_t len;
	
	/*                                                */
	/* Make sure you can open the new file first      */
	/* and leave if you can't.  The copying from old  */
	/* to new will be done in binary mode, then we'll */
	/* reopen it in text mode for continued writing:  */
	/*                                                */
	newfp=fopen(newfile,"wb");
	if(!newfp){
		lprintf("Can't open \"%s\" for writing.\nFile assignment remains unchanged.",newfile);
		return (FILE *)oldfp;
	}
	/*                                              */
	/* close oldfp and then use OpenAndBufferFile() */
	/* to place contents into a buffer, b:          */
	/*                                              */
	fclose(oldfp);
	b=OpenAndBufferFile(old);
	len=(size_t)strlen(b);
	if(fwrite(b,1L,len,newfp)!=len){
		eprintf(FATALFLAG,"ReassignFile(): error copying file.\n");
	}
	fclose(newfp);
	/*                                               */
	/* now reopen for appending at end in text mode: */
	/*                                               */
	newfp=fopen(newfile,"a+");
	if(!newfp){
		eprintf(FATALFLAG,"ReassignFile(): error opening new file for appending.\n");
	}
	/*                 */
	/* release buffer: */
	/*                 */
	free(b);
	/*         */
	/* return: */
	/*         */
	return (FILE *)newfp;
}

/*                                                          */
/* Concatenate() returns a newly allocated string           */
/* representing the concatenation of the argument list      */
/* with an optional delimiter (NULL if you don't            */
/* wan't a delimiter).  The last argument MUST BE A         */
/* NULL STRING, "", so that the function                    */
/* knows when to stop.  Of course this function isn't       */
/* as elegant as using a C++ String class, but it is        */
/* good enough to get the job done here, that's all...      */
/*                                                          */
/* A typical call looks like this:                          */
/*                                                          */
/* .                                                        */
/* .                                                        */
/* .                                                        */
/* commandString=Concatenate(" ",commandName,arg1,arg2,""); */
/* system(commandString);                                   */
/* .                                                        */
/* .                                                        */
/* .                                                        */
/*                                                          */
/* Remember that the user is responsible for freeing the    */
/* string after he/she no longer needs it.                  */
/*                                                          */
/*                                         -- 2004.04.15.ET */
/*                                                          */
char *Concatenate(const char *delimiter,...){               
	
	int i,imax,len,dlen;
	char *dest,*s;
	va_list argptr;
	
	//
	// Determine length of the delimiter, d,
	// which could be a null string indicating
	// no delimiter:
	//
	dlen=strlen(delimiter);
	 
	va_start(argptr,delimiter);
	// 
	// Iterate through all the string arguments,
	// determining their lengths.  The last
	// argument is supposed to be a null string, "":
	// so we know when to stop:
	// 
	for(i=0,len=0;(s=va_arg(argptr,char *)) && *s;i++){
		len+=strlen(s);
		len+=dlen;
	}
	va_end(argptr);
	
	// Store the total number of string arguments:
	imax=i;
	// We don't need a terminating delimiter:
	len-=dlen;
	// Add one for the final null character:
	len++;
	// Allocate (user is responsible for freeing)
	dest=(char *)malloc((unsigned)len*sizeof(char));
	if(!dest) ToolsErrorReport("Concatenate(): unable to allocate memory for string");
	//
	// Now we run through the list again.  Not
	// efficient, but where this function is used
	// it is not important.  We could make it more
	// efficient by not using strcat and just 
	// keeping a pointer as we move along in dest...
	// 
	va_start(argptr,delimiter);
	for(i=0,dest[0]=0;i<imax;i++){
		s=va_arg(argptr,char *);
		strcat(dest,s);
		if(i<imax-1) strcat(dest,delimiter);
	}
	va_end(argptr);
	
	return dest;
	
}

/*                                                                 */
/* irand(): pseudorandom integer                                   */
/* in range from low to high.  This is NOT meant as a high-quality */
/* random number generator:                                        */
/*                                                                 */
int irand(int low,int high){
	return low + (int)(high*rand()/RAND_MAX+1.0);
}

/*                                              */
/* RandomDeviate() between 0.0 and 1.0          */
/*                                              */
/* This is the "minimal" random number          */
/* generator of Park and Miller presented       */
/* on page 271 of the revised edition of        */
/* Numerical Recipes in C by Press, Flannery,   */
/* Teukolsky and Vetterling (Cambridge          */
/* University Press, 1992).                     */
/*                                              */
/* This routine is better than irand() but      */
/* still has linear correlations that would     */
/* make it unsuitable for more demanding        */
/* applications.  Where it is used in Madeline, */
/* it is perfectly adequate.                    */
/*                                              */
#define IA 16807
#define IM 2147483647
#define AM (1.0/IM)
#define IQ 127773
#define IR 2836
#define MASK 123459876
/* global seed for RandomDeviate to use: */
long globalSeed=RANDOMDEVIATESEED,*pGlobalSeed=&globalSeed;

float RandomDeviate(long *idum){
	
	long k;
	float answer;
	
	*idum ^= MASK;
	k=(*idum)/IQ;
	*idum=IA*(*idum-k*IQ)-IR*k;
	if(*idum<0) *idum+=IM;
	answer=AM*(*idum);
	*idum ^= MASK;
	return answer;
	
}

/*                                */
/* Round(): Rounds away from zero */
/*                                */
double Round( double v ){
	
	double integer;
	return modf(v,&integer) >= 0.5 ? ++integer : integer ;
	
}
/*                                  */
/* END of tools.c, Madeline version */
/*                                  */

