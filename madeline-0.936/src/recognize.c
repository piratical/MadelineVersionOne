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
/* "recognize.c" is part of the Madeline software distribution.                      */
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

/*                       */
/* recognize.c           */
/*                       */
#include "recognize.h"
#include "haldanekosambi.h"
#include "date.h"

/*                                                                */
/* RectifyFile: removes embedded tabs and makes all data lines of */
/* equal length.                                                  */
/*                                                                */
/* fin : input file name                                          */
/* fout: output file name                                         */
/*                                                                */
/* returns: count of number of lines written                      */
/*                                                                */
int RectifyFile(const char *fin,const char *fout){

	PLINEINFO p;               /* pointer to lines    */
	int lines;                 /* no. of lines        */
	int BlockCount;            /* Number of blocks    */    
	BLOCKINDICES blockIndices; /* Block indices       */
	char *b;                   /* file buffer         */
	int MaximumExpandedLength; /* length of data line after tab expansion */
	int TabExpansion;          /* tab interval        */
	/*                           */
	/* Open and buffer the file: */
	/*                           */
	b=OpenAndBufferFile(fin);
	/*                      */
	/* Count lines in file: */
	/*                      */
	lines=GetLineCount(b);
	/*                                               */
	/* Assign pointers to the start of each line and */
	/* also figure line length, marking empty lines: */
	/*                                               */
	p=GetLineInfo(b,lines);
	/*                                                  */
	/* Determine Header, Data, and non-data line types: */
	/*                                                  */
	BlockCount=DetermineNumberOfBlocks(p,lines);
	/*                               */
	/* Assign HEADER and DATA blocks */
	/*                               */
	AssignRecordTypes(BlockCount,p,lines,&blockIndices);
	
	/*                                 */
	/* Tidy up the header section:     */
	/* This is not critical, but nice: */
	/*                                 */
	TrimRightEdges(p,blockIndices.headerStart,blockIndices.headerEnd);
	/*                                                                        */
	/* Check for, and deal with, files containing tab characters or           */
	/* uneven data line lengths (hopefully due only to untrimmed-right edges: */
	/*                                                                        */
	if(DataBlockContainsTabs(p,&blockIndices)){
		
		TrimRightEdges(p,blockIndices.dataStart,blockIndices.dataEnd);
		TabExpansion=DetermineTabExpansion(p,&blockIndices,&MaximumExpandedLength);
		if(TabExpansion){
			
			WriteTabExpandedFile(p,&blockIndices,TabExpansion,MaximumExpandedLength,fout);
			
		}
		
	}else if(DataBlockLinesUneven(p,&blockIndices)){
		
		TrimRightEdges(p,blockIndices.dataStart,blockIndices.dataEnd);
		WritePaddedLineFile(p,&blockIndices,fout);
		
	}else{
		
		lines=0;
		
	}
	/*                                         */
	/* free memory resources before returning: */
	/*                                         */
	free(p);
	free(b);
	return lines;
}


/*                                                                              */
/* RecognizeFile(): Recognize an ASCII or UTF-8 rectangular data array          */
/* as a database.                                                               */
/* fin: name of input file                                                      */
/* lang: language conventions for writing dates: this is written into the batch */
/* file used to open the flat file database                                     */
/* CreateRunFile: 1 to create the run file, 0 to not do so.                     */
/* Returns zero if aborted or unsuccessful.                                     */
/*                                                                              */
int RecognizeFile(const char *fin,int lang,PPVARIABLE ivar,int CreateRunFile){
	
	char *b;            /* file buffer                  */
	int lines;          /* number of lines	            */
	PLINEINFO p;        /* vector of line information   */
	PPARSEDFIELDINFO f; /* vector of field information: */
	PDBF db;            /* MBASE database               */
	FILE *fp;           /* file pointer                 */
	
	int i;
	int BlockCount;
	BLOCKINDICES BlockIndices;
	
	int TotalHeaderLines;
	int HeaderDataLines;
	int DataLines;
	int RecordLength;
	int FieldCount;
	int GenderFieldIndex;
	
	int IndividualFieldIndex=MISSING;
	int FatherFieldIndex=MISSING;
	int MotherFieldIndex=MISSING;
	
	char HeaderFileName[LNPATH];
	char RunFileName[LNPATH];
	
	char *FemaleAndMaleValues=NULL;
	
	enum TableType TypeOfTable;
	
	/*                       */
	/* Open and buffer file: */
	/*                       */
	b=OpenAndBufferFile(fin);
	/*                      */
	/* Count lines in file: */
	/*                      */
	lines=GetLineCount(b);
	/*                                               */
	/* Assign pointers to the start of each line and */
	/* also figure line length, marking empty lines: */
	/*                                               */
	p=GetLineInfo(b,lines);
	
	/*                                 */
	/* Determine the number of blocks: */
	/*                                 */
	BlockCount=DetermineNumberOfBlocks(p,lines);
	/*                                   */
	/* Branch based on number of blocks: */
	/*                                   */
	if(!BlockCount){
		
		eprintf(WARNINGFLAG,"Recognize(): The file appears to be empty.\n");
		
		free(b);
		free(p);
		return 0;
		
	}
	if(BlockCount==1){
		
		/*                                                       */
		/* We allow the program to continue here so the user can */
		/* decipher an unlabeled data table with Madeline's help */
		/*                                                       */
		
		lprintf("It appears that this file contains only one block.\n");
		lprintf("Normally there should be two blocks: one HEADER block\n");
		lprintf("followed by one DATA block.\n\n");
		eprintf(WARNINGFLAG,"Recognize(): The file contains only one block.\n");
		
	}
	if(BlockCount>2){
		
		lprintf("It appears that this file contains more than two blocks.\n");
		lprintf("Only two blocks are allowed: one HEADER block followed by\n");
		lprintf("one DATA block.\n\n");
		eprintf(WARNINGFLAG,"Recognize(): The file contains too many blocks.\n");
		
		free(b);
		free(p);
		return 0;
		
	}
	/*                                          */
	/* Get here if there are one or two blocks: */
	/*                                          */
	
	/*                                                  */
	/* Determine Header, Data, and non-data line types: */
	/* and store block start and end indices            */
	AssignRecordTypes(BlockCount,p,lines,&BlockIndices);
	
	/*                                               */
	/* Check for files containing tab characters or  */
	/* uneven data line lengths                      */
	/*                                               */
	
	if(DataBlockContainsTabs(p,&BlockIndices)){
		
		/*                           */
		/* Tell user to run RECTIFY: */
		/*                           */
		lprintf("It appears that the DATA block contains TAB characters.\n");
		lprintf("Please examine the file carefully to verify if this is\n");
		lprintf("the true problem.  If this is the problem, try running\n");
		lprintf("the RECTIFY command which will expand tabs to the\n");
		lprintf("appropriate number of spaces in most cases. Then rerun\n");
		lprintf("RECOGNIZE after correcting all problems.\n\n");
		eprintf(WARNINGFLAG,"Recognize(): The file is not in the required format.\n");
		/*                                                                   */
		/* return 0 as an indicator that recognize command did not complete: */
		/*                                                                   */
		free(b);
		free(p);
		return 0;
		
	}else if(DataBlockLinesUneven(p,&BlockIndices)){
	
		/*                                                                    */
		/* Since there are no tabs, lines require padding: should run SQUARE: */
		/*                                                                    */
		lprintf("It appears that the DATA block contains data rows of varying lengths\n");
		lprintf("which results in a non-rectangular data block. Please examine\n");
		lprintf("the file carefully to verify if this is the true problem.  If it\n");
		lprintf("is the problem, try running the RECTIFY command which will pad data\n");
		lprintf("lines out to a consistent length.  Then rerun RECOGNIZE after\n");
		lprintf("correcting all problems.\n\n");
		eprintf(WARNINGFLAG,"Recognize(): The file is not in the required format.\n");
		/*                                                                   */
		/* return 0 as an indicator that recognize command did not complete: */
		/*                                                                   */
		free(b);
		free(p);
		return 0;
		
	}
	
	/*                                    */
	/* Get Header and Record Information: */
	/*                                    */
	if(GetHeaderAndRecordInfo(p,lines,&TotalHeaderLines,&HeaderDataLines,&DataLines,&RecordLength)==0){
		
		lprintf("It appears that this file is not in the correct format and\n");
		lprintf("Madeline cannot figure out how to parse it.  Please examine\n");
		lprintf("the file carefully and review the documentation regarding the\n");
		lprintf("flat file format.\n\n");
		
		eprintf(WARNINGFLAG,"Recognize(): The file is not in the required format.\n");
		/* free dynamically-allocated resources */
		free(b); 
		free(p);
		return 0;
		
	};
	
	lprintf("Skipping a total of %i line%s at top.\nThere %s %i non-empty header line%s and %i data line%s.\nData records are %i byte%s long.\n\n",
		TotalHeaderLines,
		TotalHeaderLines==1?"":"s",
		HeaderDataLines==1?"is":"are",
		HeaderDataLines,
		HeaderDataLines==1?"":"s",
		DataLines,
		DataLines==1?"":"s",
		RecordLength,
		RecordLength==1?"":"s"
	);	
	/*                                                                            */
	/* Parse the flat file database, obtaining field information and field count: */
	/*                                                                            */
	f=ParseDatabase(p,TotalHeaderLines,DataLines,RecordLength,&FieldCount);
	/*                                            */
	/* Decipher the flat file header, if present: */
	/*                                            */
	if(HeaderDataLines){
		RecognizeFlatFileHeader(p[0].b,p[TotalHeaderLines].b-1,f,FieldCount);
	}
	
	/*                                                                    */
	/* If allele fields are present, convert to genotype field structure: */
	/*                                                                    */
	FieldCount=ConvertAlleleFieldsToGenotypeFields(f,FieldCount);
	
	AssignDefaultFieldNames(f,FieldCount);
	
	/* TABLE TYPE                                  */
	/* TABLE TYPE                                  */
	/* TABLE TYPE                                  */
	/* Try to figure out what type of table it is: */
	/*                                             */
	TypeOfTable=GuessTableType(f,FieldCount);
	
	/*                                                    */
	/* Get the gender field, if it can be determined.     */
	/* But only do that if it might be a pedigree table:  */
	/*                                                    */
	if(TypeOfTable==UNKNOWN_TABLE || TypeOfTable==PEDIGREE_TABLE){
		GenderFieldIndex=GetGenderFieldIndex(f,FieldCount);
	}else{
		GenderFieldIndex=MISSING;
	}
	/*                                                                                */
	/* Try to determine IndividualIDField, FatherIDField, and MotherIDField:          */
	/* this requires the GenderField to be known:                                     */
	/*                                                                                */
	/* Note how we add TotalHeaderLines to p in order to arrive at the correct offset */
	/* to the first actual data record by skipping the header:                        */
	/*                                                                                */
	if(GenderFieldIndex!=MISSING){
		/*            */
		/* Tell user: */
		/*            */
		lprintf("The gender field has been identified.\n");
		
		/*                                                                                   */
		/* Get the gender values: GetFemaleAndMaleValues() returns a pointer to a            */
		/* static memory string containing first the label for female, a space, and          */
		/* then the label for males.  This string is used later for creating the batch file: */
		/*                                                                                   */
		FemaleAndMaleValues=GetFemaleAndMaleValues(GenderFieldIndex,f,p+TotalHeaderLines,DataLines);

		/*                                         */
		/* Determine the core relationship fields: */
		/*                                         */
		DetermineCoreRelationFields(
			f,
			FieldCount,
			p+TotalHeaderLines,
			DataLines,
			GenderFieldIndex,
			&IndividualFieldIndex,
			&FatherFieldIndex,
			&MotherFieldIndex
		);
		/* */
		/* */
		/* */
		if(IndividualFieldIndex!=MISSING){
			AssignDefaultCoreFieldNames(f,IndividualFieldIndex,FatherFieldIndex,MotherFieldIndex);
			lprintf("The individual, father, and mother ID fields have been identified.\n");
			/*                                                           */
			/*lprintf("Individual Field = %i\n",IndividualFieldIndex+1); */
			/*lprintf("Father ID  Field = %i\n",FatherFieldIndex+1);     */
			/*lprintf("Mother ID  Field = %i\n",MotherFieldIndex+1);     */
			/*                                                           */
		
		}

	}


	lprintf(" # . Field Name  Start End   Length Prec. Space Type \n");
	lprintf("---- ----------- ----- ----- ------ ----- ----- -----\n");
	for(i=0;i<FieldCount;i++){

		lprintf("%3i. %-11.11s %5i %5i %5i %5i %5i %c\n",
			i+1,
			f[i].nm,
	 		f[i].stt+1,
			f[i].end,
			f[i].len,
			f[i].prc,
			f[i].spc,
			f[i].typ
		);		
	}

	/*/////////////////////////////////// */
	/*                                    */
	/* END STAGE: Fill in database header */
	/*                                    */
	/*/////////////////////////////////// */

	/*                                                                           */
	/* Create name of of flat file header                                        */
	/* and run file name:                                                        */
	/*                                                                           */
	strcpy(HeaderFileName,ModifyFileName(fin,HEADER_FILE_EXTENSION)); /* Madeline header        */
	// OBSOLETE--REMOVE: strcpy(RunFileName   ,ModifyFileName(fin,RUN_FILE_EXTENSION));    /* batch command template */

	/*                                  */
	/* Construct MBASE database header: */
	/*                                  */
	db=NewDatabase(HeaderFileName,READONLY,MBASE);
	/*                                                                                                        */
	/* Fill in MBASECOREHEADER information:                                                                   */
	/*                                                                                                        */
	strcpy(db->hd.m.mtype,MFLATSTR); /* type of Madeline database table                                       */
	strcpy(db->hd.m.ffn,fin);                  /* original flat file name                                     */
	strcpy(db->hd.m.crd,GenericDateString(JulianToday())); /* creation date                                   */
	strcpy(db->hd.m.lmd,db->hd.m.crd);         /* last modified date                                          */
	strcpy(db->hd.m.endian,(Endian()==M_BIG_ENDIAN)?BIGENDIANSTR:LILENDIANSTR); /* endianess                  */
	db->fc=db->hd.m.fc=FieldCount;             /* the field count                                             */
	db->rl=db->hd.m.ln=RecordLength;           /* the length of one data record                               */
	db->hd.m.nmv[0]='\0';                      /* numeric missing value indicator -- not used: see run file   */
	db->hd.m.cmv[0]='\0';                      /* character missing value indicator -- not used: see run file */
	db->hd.m.sk=TotalHeaderLines;              /* number of lines to skip at top of file                      */
	db->rc=db->hd.m.rc=DataLines;              /* number of data records                                      */
	/*                                                                                                        */
	/* The offset in the flat file to the first record is the difference                                      */
	/* between the first byte in the first line of the file and the                                           */
	/* first byte of the first data line.  This works because the string                                      */
	/* pointers, p[i].b, are really pointing to the beginning of lines in                                     */
	/* a single large contiguous buffer, so subtracting the pointer                                           */
	/* addresses is meaningful:                                                                               */
	/*                                                                                                        */
	db->hd.m.of=p[TotalHeaderLines].b-p[0].b; /* offset in bytes to first record                              */
	/*                                                                                                        */
	/* allocate MBASE field subheader vector:                                                                 */
	/*                                                                                                        */
	db->fd.m=(PMBASEFIELD)malloc(db->fc*sizeof(MBASEFIELD));
	if(!db->fd.m){
		/* free dynamically-allocated resources */
		free(b); free(p); free(f);
		MadelineErrorReport("ParseDatabase(): No memory available for MBASEFIELD vector.");
		
	}
	/*//////////////////////////////////////// */
	/*                                         */
	/* Loop to construct field specifications: */
	/*                                         */
	/*//////////////////////////////////////// */
	for(i=0;i<db->fc;i++){
		/*             */
		/* field name: */
		/*             */
		strcpy(db->fd.m[i].nm,f[i].nm);
		/*                                                 */
		/* field extended type (et) and generic type (ty): */
		/*                                                 */
		db->fd.m[i].et=f[i].typ;
		switch(f[i].typ){
		case 'D':
			db->fd.m[i].ty=MDATE;
			break;
		case 'G':
		case 'C':
		case 'X':
		case '*':
			/*                                                    */
			/* All these extended types map to a character/string */
			/* generic type:                                      */
			/*                                                    */
			db->fd.m[i].ty=MSTRING;
			break;
		case 'N':
			if(f[i].prc>0) db->fd.m[i].ty=MFLATDBL;
			else           db->fd.m[i].ty=MFLATINT;
			break;
		}
		/*                                                    */
		/* field offset, length, terminal spacing, precision: */
		/*                                                    */
		db->fd.m[i].of=f[i].stt;
		db->fd.m[i].ln=f[i].len;
		db->fd.m[i].sp=f[i].spc;
		db->fd.m[i].pr=f[i].prc;
	}
	
	/*                                    */
	/* Open and write completed MFH file: */
	/*                                    */
	fp=fopen(HeaderFileName,"wb");
	if(!fp){
		/*                           */
		/* free allocated resources: */
		/*                           */
		free(b);
		free(p);
		free(f);
		FreeDatabase(db);
		/*               */
		/* Report error: */
		/*               */
		MadelineErrorReport("RecognizeFile(): Unable to open %s for writing",HeaderFileName);
	}
	/*                        */
	/* write database header: */
	/*                        */
	fwrite(&db->hd.m,sizeof(MBASECOREHEADER),1L,fp);
	/*                                                           */
	/* write field sub-header vector, then close database header */
	/*                                                           */
	fwrite(db->fd.m,sizeof(MBASEFIELD),db->fc,fp);
	fclose(fp);
	/*            */
	/* Tell user: */
	/*            */
	lprintf("Binary recognition header file (\"%s\") written.\n",HeaderFileName);
	/*                                          */
	/* Report the kind of table we think it is: */
	/*                                          */
	ReportTableType(TypeOfTable,HeaderFileName);
	
	/*                                                           */
	/* Make a ".run" file template -- but only if we are sure it */
	/* is a pedigree table                                       */
	/*                                                           */
	// OBSOLETE--REMOVE:
	//if(CreateRunFile && TypeOfTable==PEDIGREE_TABLE){
	//	
	//	/*                 */
	//	/* write run file: */
	//	/*                 */
	//	CreateOpenCommandFile(
	//		RunFileName,
	//		HeaderFileName,
	//		f,
	//		FieldCount,
	//		IndividualFieldIndex,
	//		FatherFieldIndex,
	//		MotherFieldIndex,
	//		GenderFieldIndex,
	//		FemaleAndMaleValues,
	//		lang,
	//		ivar
	//	);
	//	
	//}
	
	/*                           */
	/* free allocated resources: */
	/*                           */
	free(b);
	free(p);
	free(f);
	FreeDatabase(db);

	/*                              */
	/* return number of data lines: */
	/*                              */
	return lines;
}


/*               */
/* GetLineInfo() */
/*               */
PLINEINFO GetLineInfo(char *b,int linecount){

	PLINEINFO p;
	int i;
	int j;
	char *stt;

	p=NewLineInfo(linecount);
	/*                                     */
	/* i indexes the line,                 */
	/* j is a running line length counter: */
	/*                                     */
	for(i=j=0,stt=b;*b;){

		/*                                        */
		/* Mark (actually count) if line contains */
		/* tab characters, which are              */
		/* forbidden:                             */
		/*                                        */
		if(*b=='\t') p[i].tb++;
		
		/*                                          */
		/* On DOS PCs, '\r' precedes '\n' normally: */
		/* On UNIX, there is only '\n':             */
		/*                                          */
		/*                                          */
		if(*b=='\r' || *b=='\n'){
		
			p[i].ln=j;
			p[i].b =stt;
			/*                                            */
			/* terminate each line with a NULL character: */
			/*                                            */
			if(*b=='\r'){
				/*                         */
				/* Assume normal DOS file: */
				/*                         */
				*b++='\0';
				if(*b=='\n') *b++='\0';
			}else{
				/*                          */
				/* Assume Normal UNIX file: */
				/*                          */
				*b++='\0';
			}
			/*                                                */
			/* mark if line, now null-terminated,             */
			/* is empty (contains only white space, no data): */
			/*                                                */
			p[i].em=IsEmpty(p[i].b);
			/*                                          */
			/* set up for next round:                   */
			/*                                          */
			stt=b; /* start of next line                */
			i++;   /* increment to next line in p       */
			j=0;   /* reset j for counting next section */
			
		}else{
			b++;
			j++;
		}
	}
	return p;
}



/*                                                          */
/* NewLineInfo():  Allocate a vector of LINEINFO structures */
/*                                                          */
PLINEINFO NewLineInfo(int linecount){

	PLINEINFO p;
	int i;
	
	p=(PLINEINFO)malloc((unsigned)linecount*sizeof(LINEINFO));
	if(!p) ToolsErrorReport("NewLineInfo(): Unable to allocate LINEINFO vector of size n=%i\n",linecount);

	for(i=0;i<linecount;i++){
		p[i].ln=0;    /* default line length                */
		p[i].ty='\0'; /* default line type	                 */
		p[i].em=0;    /* default to not empty               */
		p[i].tb=0;    /* default to no tabs present in line */
		p[i].b=NULL;
	}
	
	return p;
}


/*              */
/* GetLineCount */
/*              */
int GetLineCount(char *b){

	int i;
	
	/*                     */
	/* count line returns: */
	/*                     */
	for(i=0;*b;b++){
		if(*b=='\n') i++;
	}
	/*                            */
	/* The very last line may not */
	/* terminate with a return:   */
	/* See if this occurs:        */
	/*                            */
	b--;
	if(*b!='\n') i++;
	
	return i;
}


/*                                          */
/* AssignRecordTypes()                      */
/*                                          */
/* If there are two blocks, marks lines in  */
/* the FIRST block as HEADER (H), and in    */
/* the SECOND block as DATA (D).            */
/*                                          */
/* If there is only one block, the block is */
/* marked as the DATA block.                */
/*                                          */
/*                                          */
void AssignRecordTypes(int blockCount,PLINEINFO p,int lineCount,PBLOCKINDICES pBlockIndices){
	
	int i;
	
	i=0;
	if(blockCount==2){
		/*                                            */
		/* Skip empty lines before first block:       */
		/*                                            */
		for(;p[i].em;i++) p[i].ty='\0';
		pBlockIndices->headerStart=i;
		/*                                            */
		/* Then of course mark the non-empty lines    */
		/* as the HEADER block:                       */
		for(;!p[i].em;i++) p[i].ty='H';
	}else{
		pBlockIndices->headerStart=i-1;
	}
	pBlockIndices->headerEnd=i-1;
	/*                                             */
	/* Mark blank lines before the data block:     */
	/*                                             */
	for(;p[i].em && i<lineCount;i++) p[i].ty='\0';
	pBlockIndices->dataStart=i;
	/*                                             */
	/* Mark the non-empty lines as the DATA block. */
	/* While doing this, also record the longest   */
	/* data line length and maximum tab count      */
	/*                                             */
	pBlockIndices->maxDataLineLength=0;
	pBlockIndices->maxDataLineTabCount=0;
	for(;!p[i].em && i<lineCount;i++){
		
		p[i].ty='D';
		if(p[i].ln>pBlockIndices->maxDataLineLength)   pBlockIndices->maxDataLineLength  =p[i].ln;
		if(p[i].tb>pBlockIndices->maxDataLineTabCount) pBlockIndices->maxDataLineTabCount=p[i].tb;
	}
	pBlockIndices->dataEnd=i-1;
	/*                                             */
	/* Mark any trailing empty lines:              */
	/*                                             */
	for(;p[i].em && i<lineCount;i++) p[i].ty='\0';
	
	/*             */
	/* DEBUG INFO: */
	/*             */
	if(pBlockIndices->headerStart>=0){
		lprintf("HEADER block spans lines %i to %i.\n",pBlockIndices->headerStart+1,pBlockIndices->headerEnd+1);
	}else{
		lprintf("HEADER block is absent.\n");
	}
	lprintf("DATA block spans lines %i to %i.\n",pBlockIndices->dataStart+1,pBlockIndices->dataEnd+1);
	
}


int foo(PLINEINFO p,int linecount){
	int a,b,DataLineLength;
	int HeaderLinesTooLong=0;
	int HeaderLineLengthSameAsDataLine=0;
	/*                                                               */
	/* The top of the file may or may not have 'H' header lines.     */
	/* The bottom of the file is supposed to contain                 */
	/* 'D' data records, excluding any blank lines.                  */
	/* a indexes from the top, while b indexes from the bottom:      */
	/*                                                               */
	/* This routine is very simple.  It assumes that the data lines  */
	/* starting at the bottom are of constant length greater than    */
	/* zero and that the data lines are of greater length than any   */
	/* header lines at the top.  So, any short lines at the top      */
	/* get designated as header lines, otherwise they are data lines */
	/*                                                               */
	/*                                                               */
	
	/*                                                            */
	/* skip any empty lines at the top:  Empty lines at the top   */
	/* are considered to be part of the header, so they're marked */
	/* as type 'H':                                               */
	/*                                                            */
	for(a=0;p[a].ln==0;a++) p[a].ty='H';
	
	/*                                                               */
	/* skip any empty lines at the bottom: Empty lines at the bottom */
	/* are NOT data lines: they remain defaulted to type '\0':       */
	/*                                                               */
	for(b=linecount-1;p[b].ln==0;b--);
	
	/* 2001.11.08.ET CHANGE:   New algorithm for sorting out the type of the lines:        */
	/*                         Start at the bottom, and move toward the top, marking       */
	/*                         all lines as 'D' data lines as long as they remain the same */
	/*                         length.  Once a line becomes shorter or empty, mark all     */
	/*                         the remaining non-empty lines as header lines until we      */
	/*                         reach the top again.                                        */
	/*                                                                                     */

	/* Mark the very last non-empty line as data and use it's length as the standard: */
	p[b].ty='D';
	DataLineLength=p[b].ln;
	for(;b>=a && p[b].ln==DataLineLength;b--) p[b].ty='D';
	/*                                                               */
	/* Mark the rest as header lines:                                */
	/*                                                               */
	/* Emit warnings if header lines have line lengths greater than  */
	/* or equal to the "standard" data line length:                  */
	/*                                                               */
	for(;b>=a;b--){
		p[b].ty='H';
		if(p[b].ln==DataLineLength) HeaderLineLengthSameAsDataLine++;
		if(p[b].ln>DataLineLength ) HeaderLinesTooLong++;
	}
	
	/* EMIT WARNINGS FOR PROBABLE NON-RECTANGULARITY: */
	if(HeaderLineLengthSameAsDataLine){
		if(HeaderLineLengthSameAsDataLine==1){
			lprintf("Madeline classified one row as a HEADER row even though this row\n");
			lprintf("is equal in length to the DATA row length of %i bytes!\n",DataLineLength);
		}else{
			lprintf("Madeline classified %i rows as HEADER rows even though these rows\n",HeaderLineLengthSameAsDataLine);
			lprintf("are equal in length to the DATA row length of %i bytes!\n",DataLineLength);
		}
		lprintf("This very likely means that the DATA grid is NOT RECTANGULAR!\n");
		lprintf("Please examine the file carefully to verify if this is\n");
		lprintf("the true problem.  If this is the problem, try running\n");
		lprintf("the SQUARE command to make the data block rectangular.\n\n");
		
		eprintf(WARNINGFLAG,"AssignRecordTypes(): The file is not in the required format.\n");
		/*                                                 */
		/* Return zero to indicate that we have a problem: */
		/*                                                 */
		return 0;                                 
	}
	if(HeaderLinesTooLong){
		if(HeaderLinesTooLong==1){
			lprintf("Madeline classified one row as a HEADER row even though this row\n");
			lprintf("is LONGER in length than the DATA row length of %i bytes!\n",DataLineLength);
		}else{
			lprintf("Madeline classified %i rows as HEADER rows even though these rows\n",HeaderLinesTooLong);
			lprintf("are LONGER in length than the DATA row length of %i bytes!\n",DataLineLength);
		}
		lprintf("This very likely means that the DATA grid is NOT RECTANGULAR!\n");
		lprintf("Please examine the file carefully to verify if this is\n");
		lprintf("the true problem.  If this is the problem, try running\n");
		lprintf("the SQUARE command to make the data block rectangular.\n\n");
		
		eprintf(WARNINGFLAG,"AssignRecordTypes(): The file is not in the required format.\n");
		/*                                                 */
		/* Return zero to indicate that we have a problem: */
		/*                                                 */
		return 0;
	}
	
	/* If OK, just return the LineCount: */
	return linecount;
	
}

/*                                                                                                */
/* GetHeaderAndRecordInfo                                                                         */
/*                                                                                                */
/* PLINEINFO p: pointer to line information                                                       */
/* linecount  : count of lines in file                                                            */
/* SkipLines  : pointer which will be filled with the number of lines to skip                     */
/* HeaderLines: pointer which will be filled with the number of data-containing header lines      */
/* DataLines  : pointer which will be filled with the number of data lines (i.e., no. of records) */
/* RecordLength: pointer which will be filled with the data record length                         */
/*                                                                                                */
/* return value: 1 (boolean TRUE) if the file is a good rectangular data file                     */
/*               from Madeline's perspective, 0 (boolean FALSE) if it is not.                     */

/* */
int GetHeaderAndRecordInfo(PLINEINFO p,int linecount,int *TotalHeaderLines,int *HeaderDataLines,int *DataLines,int *RecordLength){

	register int a; /* indexes lines from the top, counts total no. of header lines */
	register int b; /* indexes lines from the bottom:                               */
	register int h; /* counts number of header lines containing data                */
	register int d; /* counts number of data lines                                  */
		
	
	/*                                                             */
	/* Traverse all header lines, counting how many there are, and */
	/* of these, how many contain data:                            */
	/*                                                             */
	
	/*                                                    */
	/* 2004.04.20.ET CHANGE: Count initial '\0','H', and  */
	/* any additional '\0' lines toward the HEADER total: */
	/* stop when we get to 'D':                           */
	/*                                                    */
	for(h=a=0;a<linecount && p[a].ty!='D';a++){
		
		if(p[a].ty=='H') h++;
		
	}
	
	/*                                                       */
	/* store number of TotalHeaderLines and HeaderDataLines: */
	/*                                                       */
	*TotalHeaderLines=a;
	*HeaderDataLines=h;
	
	/*                                      */
	/* count how many data lines there are: */
	/*                                      */
	for(d=0;a<linecount && p[a].ty=='D';a++,d++);
	
	/*                             */
	/* store number of data lines: */
	/*                             */
	*DataLines=d;
	
	/*                                                           */
	/* Check that data lines are all the same length:            */
	/* If not, it's not a rectangular data array -- this usually */
	/* indicates the file is in some other format, such as comma */
	/* or tab delimited:                                         */
	/*                                                           */

	/*                                     */
	/* skip any empty lines at the bottom: */
	/*                                     */
	for(b=linecount-1;p[b].ty=='\0';b--);
	/*                                      */
	/* should now be on the last data line: */
	/* use it's length as the standard:     */
	/*                                      */
	*RecordLength=p[b--].ln;
	/*                                            */
	/* loop up from bottom, checking line length: */
	/*                                            */
	for(;b>=0 && p[b].ty=='D';b--){
	
		if(p[b].ln!=*RecordLength) return 0;	
	}
	/*                                              */
	/* b should now be equal to TotalHeaderLines-1: */
	/*                                              */
	if(b!=*TotalHeaderLines-1) return 0;
	
	/*                                    */
	/* get here if everything checks out: */
	/*                                    */
	
	return 1;
	
}



/*                                               */
/* void ShowLineInfo(): used in test/debug cycle */
/*                                               */
void ShowLineInfo(PLINEINFO p,int linecount){

	int i;
	
	for(i=0;i<linecount;i++){

		lprintf("%4i. %c:%i %4i: %-40.40s\n",
			i+1,
			p[i].ty,
			p[i].em,
			p[i].ln,
			p[i].b
		);
	}
}



/*                            */
/* RecognizeFlatFileHeader(): */
/*                            */
void RecognizeFlatFileHeader(char *HeaderStart,char *HeaderEnd,PPARSEDFIELDINFO f,int FieldCount){

	int i;
	
	char *b;
	char t='\0';
	/*                                                        */
	/* read header only up to number of fields in FieldCount: */
	/*                                                        */
	for(b=HeaderStart,i=0;b<HeaderEnd && i<FieldCount;i++){
		/*                  */
		/* re-initialize t: */
		/*                  */
		t='\0';
		/*                  */
		/* read field name: */
		/*                  */
		b=ReadFieldName(b,HeaderEnd,f[i].nm,32);
		
		/*                               */
		/* also possibly the field type: */
		/*                               */
		b=ReadFieldType(b,HeaderEnd,&t);
		if(t){
			/*                                                      */
			/* A legal type letter was found                        */
			/* since f[i].typ already registers a type,             */
			/* we can compare the type found to the                 */
			/* registered type.  The only allowed conversions       */
			/* are for numeric 'N' and date 'D' which               */
			/* can be alternatively treated as character 'C':       */
			/*                                                      */
			/* --> f[i].typ=='*' can be converted to any type.      */
			/*                                                      */
			/* --> f[i].typ=='C' or 'G' cannot be converted,        */
			/*     except that a field detected as 'G' can be       */
			/*     listed in the flat file header as being of       */
			/*     the more generic type 'C' (19991102 ET Addendum) */
			/*                                                      */
			/* 19990611 ET Addendum:                                */
			/*                                                      */
			/* --> f[i].typ=='X' can be listed by the user          */
			/*     as either 'X' or 'C'                             */
			/*                                                      */
			/* 2000 05 09 ET ADDENDUM: Allow 'N' to be alternatively*/
			/* treated as 'D' in the *assumed* "YYYYMMDD" format    */
			/*                                                      */
			if(f[i].typ=='*'){
				f[i].typ=t;
			}else if(t=='C' && (f[i].typ=='N' || f[i].typ=='D')){
				/* Numeric or date fields can be treated as character by user directive: */
				f[i].typ=t;
				lprintf("Note: Treating field %i as being of type \"%c\"...\n",i+1,f[i].typ);
			}else if(t=='D' && f[i].typ=='N'){
				/* A numeric column in YYYYMMDD format can be treated as date by user directive: */
				f[i].typ=t;
				lprintf("Note: Treating field %i as being of type \"%c\"...\n",i+1,f[i].typ);
				lprintf("For this to work, the dates in this column must be in YYYYMMDD format ...\n"); 
			}else if(t=='A' && f[i].typ=='N'){
				/* Contiguous pairs of numeric columns can be treated as allele columns: */
				f[i].typ=t;
			}else if(f[i].typ=='X' && t!='X'){
				/* X is the reserved type for the gender field: */
				lprintf("Converting field type of field %i from \"%c\" to \"%c\"...\n",i+1,t,f[i].typ);
			}else if(f[i].typ=='G' && t=='C'){
				/* Madeline knows which "character" fields are really genotype fields: */
				lprintf("Converting field type of field %i from \"%c\" to \"%c\"...\n",i+1,t,f[i].typ);
			}else if(t!=f[i].typ){
				/*                         */
				/* Put error code in here: */
				/*                         */
				MadelineErrorReport("RecognizeFlatFileHeader(): Fields of type \"%c\" cannot be alternatively treated as type \"%c\"\n",
					f[i].typ,
					t
				);
			}
		}
	}
}


/*                */
/* ReadFieldName: */
/*                */
char *ReadFieldName(char *Beginning,char *HeaderEnd,char *FieldName,int MaxFieldNameLength){

	char *b;
	char *hh;
	int i;
	
	/*                   */
	/* skip white space: */
	/*                   */
	for(b=Beginning;b<HeaderEnd && (*b==' ' || *b=='\0' || *b=='\n' || *b=='\r' || *b=='\t');b++);
	/*                  */
	/* Copy field name: */
	/*                  */
	for(i=0,hh=FieldName;i<MaxFieldNameLength && b<HeaderEnd && !(*b==' ' || *b=='\0' || *b=='\n' || *b=='\r' || *b=='\t');*hh++=*b++);
	*hh='\0';
	/*                                                            */
	/* skip remaining portion of name if too long for field name: */
	/*                                                            */
	for(;b<HeaderEnd && !(*b==' ' || *b=='\0' || *b=='\n' || *b=='\r' || *b=='\t');b++);
	return b;
}


/*                                                           */
/* ReadFieldType()                                           */
/*                                                           */
/* Allowed field types are: C -- character                   */
/*                          N -- numeric                     */
/*                          D -- date                        */
/*                          G -- genotype                    */
/*                          A -- allele                      */
/*                          X -- character-type gender field */
/*                                                           */
char *ReadFieldType(char *Beginning,char *HeaderEnd,char *type){

	register char a;
	register char c;
	register char *b;

	/*                   */
	/* skip white space: */
	/*                   */
	for(b=Beginning;b<HeaderEnd && (*b==' ' || *b=='\0' || *b=='\n' || *b=='\r');b++);
	/*                                                   */
	/* accept only a single character surrounded on both */
	/* sides by white space for field type:              */
	/*                                                   */
	a=*(b-1);
	c=*(b+1);
	if(   *b 
	   && (*b=='C' || *b=='N' || *b=='G' || *b=='D' || *b=='A' || *b=='X')
	   && (a==' ' || a=='\0' || a=='\n' || a=='\r' || a=='\t')
	   && (c==' ' || c=='\0' || c=='\n' || c=='\r' || c=='\t')
	){
	
		*type=*b;
		return ++b;	/* return after the type */
	}else{	
		*type='\0'; /* indicates field type not found */
		return --b; /* return before the type         */
	}	
}



/*                                             */
/* NewIntVector():  Allocate a vector of ints: */
/*                                             */
int *NewIntVector(int n){

	int *p;
	int i;
	
	p=(int *)malloc((unsigned)n*sizeof(int));
	if(!p) ToolsErrorReport("NewIntVector(): Unable to allocate int vector of size n=%i\n",n);

	for(i=0;i<n;i++){
		p[i]=0;
	}
	
	return (int *) p;
}


/*                 */
/* ParseDatabase() */
/*                 */
PPARSEDFIELDINFO ParseDatabase(PLINEINFO p,int TotalHeaderLines,int DataLines,int RecordLength,int *FieldCount){


	int *filled;    /* fill rates vector                      */
	int *dots;      /* used for determining numeric precision */
	int i;          /* indexes a line   in PLINEINFO   p      */
	int j;          /* indexes a column in PCOLUMNINFO q      */
	char *b;        /* pointer to the string in the line      */

	/* int offset; */   /* offset to field                        */
	/* int stt;    */   /* start position of field                */
	/* int end;    */   /* end position of data in field          */
	PPARSEDFIELDINFO f; /* vector of parsed field information */
	
	/*                                   */
	/* Allocate column fill rate vector: */
	/*                                   */
	filled=NewIntVector(RecordLength);
	dots  =NewIntVector(RecordLength);
	
	/*                                                                 */
	/* advance p to the first data record, which begins just after the */
	/* last header line:                                               */
	/*                                                                 */
	p+=TotalHeaderLines;
	
	/*                                                    */
	/* ***** PHASE I *****                                */
	/*                                                    */
	/* Scan the data, accumulating column fill rates:     */
	/* in order to determine field boundaries.            */
	/*                                                    */
	/* We also store dot frequencies so that we can later */
	/* determine where the decimal place falls in numeric */
	/* fields:                                            */
	/*                                                    */
	/*                                                    */
	for(i=0;i<DataLines;i++){
		/*                                           */
		/* b is used to make the code more readable: */
		/*                                           */
		b=p[i].b;
		
		/*                                            */
		/* scan the line, counting non-empty columns: */
		/*                                            */
		for(j=0;j<RecordLength;j++){
			/*                                     */
			/* Column fill rates:                  */
			/*                                     */
			if(b[j]!=' ') filled[j]++;
			/*                                     */
			/* 2003.02.26.ET ADDENDUM:             */
			/*                                     */
			/* The OLD CODE had:                   */
			/* if(b[j]=='.') dots[j]++;            */
			/* ... which is insufficient           */
			/*                                     */
			/* The dots array must only count      */
			/* dots that FALL BETWEEN TWO DIGITS   */
			/* dots that are sitting by themselves */
			/* are MISSING VALUE indicators,       */
			/* REGARDLESS OF WHERE THEY OCCUR in a */
			/* field.                              */
			/*                                     */
			/* To clarify, we are NOT going to     */
			/* count ".123" as a number, because   */
			/* Madeline's parser requires "0.123". */
			/*                                     */
			/* Likewise, we NEED NOT count "123."  */
			/* as having a dot because the decimal */
			/* precision ends up being ZERO        */
			/* anyway.                             */
			/*                                     */
			/* So, the rule to only count dots     */
			/* between two digits should work.     */
			/* However, we must emit a WARNING     */
			/* we find "blank-dot-digit" which     */
			/* are treated as non-numbers:         */
			/*                                     */
			if(b[j]=='.' && j>0 && j<RecordLength-1){
				if(isdigit(b[j-1]) && isdigit(b[j+1])) dots[j]++;
				if(b[j-1]==' ' && isdigit(b[j+1])){
					/* case of "blank-dot-digit" */
					eprintf(SEVEREWARNINGFLAG,"Entries like \" .%c\" in line %i, column %i of the data grid are NOT treated as numbers in Madeline!\nThis is considered a CHARACTER entry.  To treat as a number, write \"0.%c\" instead.\n",b[j+1],i+1,j+1,b[j+1]);
				}
			}
		}

	}  /* end of for loop down lines */
	
	/*                      */
	/* ***** PHASE II ***** */
	/*                      */
	/* Get the field count: */
	/*                      */
	
	/*                                                     */
	/* Examine filled[]                                    */
	/* to see how many columns there are                   */
	/* and if the data are in fact space-delimited:        */
	/*                                                     */
	/*                                                     */
	/* Start by skipping initial white space on left edge: */
	/*                                                     */
	for(j=0;j<RecordLength && !filled[j];j++);
	/*                   */
	/* Now count fields: */
	/*                   */
	for(i=0;j<RecordLength;){
		/*                                    */
		/* skip the filled part of the field: */
		/*                                    */
		for(;j<RecordLength && filled[j];j++);
		/*                          */
		/* increment field counter: */
		/*                          */
		i++;
		/*                                   */
		/* skip the empty part of the field: */
		/*                                   */
		for(;j<RecordLength && !filled[j];j++);
	}
	/*                       */
	/* i is the field count: */
	/*                       */
	*FieldCount=i;
	/*                                              */
	/* Allocate vector of parsed field information: */
	/*                                              */
	f=NewParsedFieldInfo(*FieldCount);
	
	
	DetermineFieldBoundaries(filled,f,RecordLength);
	DetermineFieldContents(p,DataLines,f,*FieldCount);
	DetermineNumericPrecision(dots,f,*FieldCount);
	/*                              */
	/* release allocated resources: */
	/*                              */
	free(filled);
	free(dots);
	/*                                     */
	/* return vector of field information: */
	/*                                     */
	return (PPARSEDFIELDINFO) f;
}


/*                                        */
/* PPARSEDFIELDINFO NewParsedFieldInfo(): */
/*                                        */
PPARSEDFIELDINFO NewParsedFieldInfo(int n){

	PPARSEDFIELDINFO p;
	int i;
	
	p=(PPARSEDFIELDINFO)malloc((unsigned)n*sizeof(PARSEDFIELDINFO));
	if(!p) ToolsErrorReport("NewParsedFieldInfo(): Insufficient resources to allocate PARSEDFIELDINFO vector");
	
	for(i=0;i<n;i++){

		p[i].nm[0]='\0';	
		p[i].stt=0;
		p[i].end=0;
		p[i].len=0;
		p[i].spc=0;
		p[i].prc=0;
		p[i].typ='\0';
		p[i].character=0;
		p[i].numeric  =0;
		p[i].genotype =0;
		p[i].date     =0;
		p[i].missing  =0;
		p[i].gender   =0;
	
	}
	return p;
}


/*                          */
/* DetermineFieldBoundaries */
/*                          */
void DetermineFieldBoundaries(int *filled,PPARSEDFIELDINFO f,int RecordLength){

	int i; /* indexes the field                   */
	int j; /* indexes the column in the flat file */
	int offset;
	/*int stt; */
	/*int end; */
	
	/*                                                 */
	/* skip any white space preceding the first field: */
	/*                                                 */
	for(j=0;j<RecordLength && !filled[j];j++);
	offset=0;
	/*                                                   */
	/* i indexes fields, j indexes columns in flat file: */
	/*                                                   */
	for(i=0;j<RecordLength;){
		/*                                   */
		/* set true start of filled portion: */
		/*	                                  */
		
		/* stt=j;		 */
		
		/*                                                        */
		/* set FIELD OFFSET (0 for first field, thereafter just j */
		/* at this point):                                        */
		/* offset is the start of the field:                      */
		/*                                                        */
		f[i].stt=offset;						
		/*	                                            */
		/* skip the filled part of the current field:  */
		/* end is actually the next byte after the end */
		/* of the filled portion:                      */
		/*                                             */
		for(;j<RecordLength && filled[j];j++);
		
		/*f[i].end=end=j; */
		
		f[i].end=j;
		
		/*                                  */
		/* this is FIELD LENGTH: end-offset */
		/*                                  */
		f[i].len=f[i].end-f[i].stt;
		/*                                                       */
		/* skip the empty part of the field on the right margin: */
		/*                                                       */
		for(;j<RecordLength && !filled[j];j++);
		/*                                         */
		/* set the field's terminal spacing: now j */
		/* is at the beginning of the next field:  */
		/*                                         */
		f[i].spc=j-f[i].end;

		/*                            */
		/* set offset for next field: */
		/*                            */
		offset=j;

		/* increment the field counter: */
		i++;
	}
}


/*                        */
/* DetermineFieldContents */
/*                        */
void DetermineFieldContents(PLINEINFO p, int DataLines, PPARSEDFIELDINFO f, int FieldCount){

	int i;  /* indexes data lines */
	int j;  /* indexes field      */
	
	/*                   */
	/* Get running sums: */
	/*                   */
	for(i=0;i<DataLines;i++){
		for(j=0;j<FieldCount;j++){
			/* */
			/* */
			/* */
			if(IsEmptyOrDot(p[i].b,f[j].stt,f[j].end)){
				f[j].missing++;
			}else if(IsADate(p[i].b,f[j].stt,f[j].end)){
				f[j].date++;
			}else if(IsAGenotype(p[i].b,f[j].stt,f[j].end)){
				f[j].genotype++;
			}else if(IsNumeric(p[i].b,f[j].stt,f[j].end)){
				f[j].numeric++;
			}else if(IsGenderCode(p[i].b,f[j].stt,f[j].end)){
				f[j].gender++;
			}else{
				f[j].character++;
			}
		} /* end of loop over FieldCount */
	} /* end of loop over data lines  */

	/*                                                         */
	/* Now specify field type: current code does not check for */
	/* multiple categories.                                    */
	/*                                                         */
	/*                                                         */
	/*                                                         */
	/*                                                         */
	for(j=0;j<FieldCount;j++){
		if     (f[j].date     ) f[j].typ='D';
		else if(f[j].genotype ) f[j].typ='G';
		else if(f[j].character) f[j].typ='C';
		else if(f[j].numeric  ) f[j].typ='N';
		else if(f[j].gender   ) f[j].typ='X';			 
		else                    f[j].typ='*';
	}
}


/*                                                        */
/* IsNumeric(): returns 1 if a field                      */
/*              contains an integer, 2 if floating point, */
/*              0 if not numeric                          */
/*                                                        */
int IsNumeric(char *p,int stt,int end){

	char *sstt;
	char *send;
	char *s;
	char *t;
	int  dc; /* counts decimal points */
	
	sstt=p+stt;
	send=p+end;

	
	/*                                */
	/* skip white space on left edge: */
	/*                                */
	for(s=sstt;s<send && *s==' ';s++);
	if(s==send) return 0;
	/*                                 */
	/* skip white space on right edge: */
	/*                                 */
	for(t=send-1;t>=sstt && *t==' ';t--);
		
	/*                                  */
	/* Number may start with '+' or '-' */
	/* sign:                            */
	/*                                  */
	if(*s=='+' || *s=='-') s++;
	if(!isdigit(*s)) return 0;
	/*                        */
	/* skip numbers and dots: */
	/*                        */
	for(dc=0;s<=t && (isdigit(*s) || *s=='.');s++) if(*s=='.') dc++;
	/*                       */
	/* should now be past t: */
	/*                       */
	if(s>t){
		return (dc<=1); /* its a number if zero or just one decimal place */
	}else{
		return 0;
	}
}


/*                                                                  */
/* IsEmptyOrDot(): returns 1 if field is missing (blank or contains */
/*              a lone dot missing value indicator, " . ".          */
/*                                                                  */
int IsEmptyOrDot(char *p,int stt,int end){

	char *sstt;
	char *send;
	char *s;
	char *t;
	
	sstt=p+stt;
	send=p+end;

	/*                   */
	/* skip white space: */
	/*                   */
	for(s=sstt;s<send && *s==' ';s++);
	/*                        */
	/* completely blank case: */
	/*                        */
	if(s==send) return 1;
	/*                          */
	/* if not completely blank: */
	/*                          */
	for(t=send-1;t>=s && *t==' ';t--);
	/*                                     */
	/* if t==s and *s=='.', then the field */
	/* contains nothing but a dot which is */
	/* a standard missing value indicator: */
	/*                                     */
	return (t==s && *s=='.');
}

/*                                                              */
/* IsAGenotype() is similar to ContainsAGenotype (in parser.c): */
/*                                                              */
int IsAGenotype(char *p,int stt,int end){

	char *s;
	int sc;
	
	char *sstt;
	char *send;
	
	sstt=p+stt;
	send=p+end;
	
	/*                          */
	/* skip initial whitespace: */
	/*                          */
	for(s=sstt;s<send && *s==' ';s++);
	if(!*s) return 0;
	/*                       */
	/* traverse first label: */
	/*                       */
	for(;s<send && isdigit(*s);s++);
	/*                                                                 */
	/* only one numeric label, no slash, or else some other disallowed */
	/* character:                                                      */
	/*                                                                 */
	if(!*s) return 0;

	/*                                                                   */
	/* skip middle section of white space, counting slashes, if present: */
	/*                                                                   */
	for(sc=0;s<send && (*s==' ' || *s=='/');s++) if(*s=='/') sc++;
	/*                         */
	/* only one slash allowed: */
	/*                         */
	if(sc!=1)            return 0;
	if(!*s)              return 0;
	/*                                       */
	/* second numeric label is now expected: */
	/*                                       */
	for(;s<send && isdigit(*s);s++);

	/*                            */
	/* skip terminal white space: */
	/*                            */
	for(;s<send && *s==' ';s++);
	/*                                    */
	/* there should not be anything else: */
	/*                                    */
	if(s<send) return 0;
	/*                     */
	/* otherwise, it's OK: */
	/*                     */
	return 1;
}


/*                                                                          */
/* IsADate(): A date must contain two delimiters.                           */
/*            The delimiters may be ".",",", "/", or " ".                   */
/*            Space is allowed for consistency with the way Madeline        */
/*            treats dates elsewhere, even though in reality the way        */
/*            that a flat file is parsed may exclude having spaces internal */
/*            to dates in practice.                                         */
/*                                                                          */
/*            The two delimiters delimit three pieces of information.       */
/*            Two or three of those pieces of information must be numeric.  */
/*            If the third is character, it is assumed to be a month name   */
/*            either abbreviated or spelled out.  Since Madeline can        */
/*            process dates in different languages, the characters aren't   */
/*            actually checked to see if they represent specific months or  */
/*            not.                                                          */
/*                                                                          */
int IsADate(char *p,int stt,int end){

	char *s;
	char *fd; /* first delimiter  */
	char *sd; /* second delimiter */
	
	char *sstt;
	char *send;

	int cc; /* count characters         */
	int cn; /* count digits             */
	int nfc=0; /* numeric field count   */
	int cfc=0; /* character field count */
	
	sstt=p+stt;
	send=p+end;
	
	
	/*                                                    */
	/* start at left margin and look for first delimiter: */
	/*                                                    */
	
	/*                              */
	/* first, skip any white space: */
	/*                              */
	for(fd=sstt;fd<send && *fd==' ';fd++);
	if(fd==send) return 0;
	
	for(;fd<send && !(*fd=='.' || *fd=='/' || *fd==',' || *fd==' ');fd++);
	if(fd==send) return 0;

	/*                                                     */
	/* got first delimiter, so now start from right margin */
	/* and look back for second delimiter:                 */
	/*                                                     */

	/*                             */
	/* first, skip any white space */
	/*                             */
	for(sd=send-1;sd>=sstt && *sd==' ';sd--);
	if(sd==sstt) return 0;
	
	for(;sd>=sstt && !(*sd=='.' || *sd=='/' || *sd==',' || *sd==' ');sd--);
	if(sd==sstt) return 0;
	/*                                                     */
	/* if first and second delimiter are in the same spot, */
	/* forget it: it's not a date:                         */
	/*                                                     */
	if(sd==fd) return 0;
	/*                                                       */
	/* starting from fd, go back toward left margin counting */
	/* characters and numbers to determine content:          */
	/*                                                       */
	for(cc=cn=0,s=fd-1;s>=sstt;s--){
		if(isalpha(*s)) cc++;
		else if(isdigit(*s)) cn++;	
	}
	/*                                                         */
	/* field can be either character or numeric, but not both: */
	/*                                                         */
	if(cc && cn) return 0;
	/*                                      */
	/* determine field type of first field: */
	/*                                      */
	if(cc) cfc++;
	if(cn) nfc++;
	 
	/*                                                     */
	/* second field is between first and second delimiter: */
	/*                                                     */
	for(cc=cn=0,s=fd+1;s<sd;s++){
		if(isalpha(*s)) cc++;
		else if(isdigit(*s)) cn++;
	}
	/*                                    */
	/* do same thing as with first field: */
	/*                                    */
	if(cc && cn) return 0;
	if(cc) cfc++;
	if(cn) nfc++;
	
	/*                     */
	/* now do third field: */
	/*                     */
	for(cc=cn=0,s=sd+1;s<send;s++){
		if(isalpha(*s)) cc++;
		else if(isdigit(*s)) cn++;
	}
	if(cc && cn) return 0;
	if(cc) cfc++;
	if(cn) nfc++;
	/*                                                          */
	/* Now look at cfc and nfc: there must be a total of three  */
	/* filled fields, and at least two of them must be numeric: */
	/*                                                          */
	if(cfc+nfc==3 && nfc>=2) return 1;
	else return 0;
	
}



/*                                                       */
/* IsGenderCode(): returns 1 if field                    */
/*              is a character field containing          */
/*              "M","F","m","f","ma","fe","MA", or "FE", */
/*                                                       */
/*              0 if containing something else           */
/*                                                       */
int IsGenderCode(char *p,int stt,int end){

	char *sstt;
	char *send;
	char *s;
	char *t;
	
	sstt=p+stt;
	send=p+end;

	
	/*                                */
	/* skip white space on left edge: */
	/*                                */
	for(s=sstt;s<send && *s==' ';s++);
	if(s==send) return 0;
	/*                                 */
	/* skip white space on right edge: */
	/*                                 */
	for(t=send-1;t>=sstt && *t==' ';t--);
	
	/*                                                     */
	/* Gender Code may start with "M" or "F" or "m" or "f" */
	/*                                                     */
	if(*s=='M' || *s=='F' || *s=='m' || *s=='f'){
		if(s==t) return 1; /* single letter codes                   */
		/*                                                          */
		/* get here if multiple letter codes: just check 2nd letter */
		/* but no farther than that:                                */
		/*                                                          */
		if(s[1]=='A' && s[0]=='M') return 1;
		if(s[1]=='a' && (s[0]=='M' || s[0]=='m')) return 1;	
		if(s[1]=='E' && s[0]=='F') return 1;
		if(s[1]=='e' && (s[0]=='F' || s[0]=='f')) return 1;
	}
	/*                                         */
	/* get here if something else is going on: */
	/*                                         */
	return 0;
}

/*                                                             */
/* ReadCharacterData                                           */
/*                                                             */
/* This doesn't check boundaries, so make sure d is big enough */
/* to hold everything                                          */
/*                                                             */
void ReadCharacterData(char *dest,char *p,int stt,int end){

	char *s;
	char *d;
	
	for(d=dest,s=p+stt;s<p+end;*d++=*s++);
	/*                            */
	/* be sure to null-terminate: */
	/*                            */
	*d='\0';
	return;
}


/*                                                                 */
/* ReadGenderCode(): returns 0 if a male gender code,              */
/*                       1 if a female gender code,                */
/*                       MISSING if a determination cannot be made */
/*                                                                 */
int ReadGenderCode(char *p,int stt,int end){

	char *sstt;
	char *send;
	char *s;
	char *t;
	
	sstt=p+stt;
	send=p+end;

	
	/*                                */
	/* skip white space on left edge: */
	/*                                */
	for(s=sstt;s<send && *s==' ';s++);
	if(s==send) return 0;
	/*                                 */
	/* skip white space on right edge: */
	/*                                 */
	for(t=send-1;t>=sstt && *t==' ';t--);
	
	/*                                                     */
	/* Gender Code may start with "M" or "F" or "m" or "f" */
	/*                                                     */
	if(*s=='M' || *s=='F' || *s=='m' || *s=='f'){
		if(s==t){
			/*                      */
			/* single letter codes: */
			/*                      */
			if(*s=='M' || *s=='m') return 0;
			if(*s=='F' || *s=='f') return 1;
			return MISSING;
		}
		/*                                                          */
		/* get here if multiple letter codes: just check 2nd letter */
		/* but no farther than that:                                */
		/*                                                          */
		if(s[1]=='A' && s[0]=='M') return 0;
		if(s[1]=='a' && (s[0]=='M' || s[0]=='m')) return 0;	
		if(s[1]=='E' && s[0]=='F') return 1;
		if(s[1]=='e' && (s[0]=='F' || s[0]=='f')) return 1;
	}
	/*                                         */
	/* get here if something else is going on: */
	/*                                         */
	return MISSING;
}


/*                              */
/* DetermineNumericPrecision(): */
/*                              */
void DetermineNumericPrecision(int *dots,PPARSEDFIELDINFO f,int FieldCount){

	int i;
	int j;
	int maxdots;
	int dotidx;
	
	for(i=0;i<FieldCount;i++){
		if(f[i].typ=='N'){
			/*                                           */
			/* scan field to find maximum dot frequency: */
			/*                                           */
			for(maxdots=0,j=f[i].stt;j<f[i].end;j++){
				if(dots[j]>maxdots){
					maxdots=dots[j];
					dotidx =j;
				}
			}
			/*                                                 */
			/* if maxdots>0, then numbers have decimal places, */
			/* so store decimal precision:                     */
			/*                                                 */
			if(maxdots)	f[i].prc=f[i].end-1-dotidx;
		}
	}
}


/*                            */
/* AssignDefaultFieldNames(): */
/*                            */
void AssignDefaultFieldNames(PPARSEDFIELDINFO f,int FieldCount){

	int i;
	
	int cfc; /* character field count */
	int nfc; /* numeric field count   */
	int gfc; /* genotype field count  */
	int dfc; /* date field count      */
	int xfc; /* gender field count    */
	int lfc; /* null field count      */
	
	for(lfc=xfc=cfc=nfc=gfc=dfc=i=0;i<FieldCount;i++){
		/*                              */
		/* Process unnamed fields only: */
		/*                              */
		if(!f[i].nm[0]){
			switch(f[i].typ){
			case 'C':
				++cfc;
				sprintf(f[i].nm,"CHAR_%03i",cfc);
				break;
			case 'N':
				++nfc;
				sprintf(f[i].nm,"NUME_%03i",nfc);
				break;
			case 'D':
				++dfc;
				sprintf(f[i].nm,"DATE_%03i",dfc);
				break;
			case 'G':
				++gfc;
				sprintf(f[i].nm,"GENO_%03i",gfc);
				break;
			case 'X':
				++xfc;
				if(xfc==1) sprintf(f[i].nm,"GENDER");
				else       sprintf(f[i].nm,"GEND_%03i",xfc);
				break;
			case '*':
				++lfc;
				sprintf(f[i].nm,"NULL_%03i",lfc);
				break;
			}
		}
	}
}


/*                                                                 */
/* AssignDefaultCoreFieldNames(): Assigns default core field names */
/*                                                                 */
void AssignDefaultCoreFieldNames(PPARSEDFIELDINFO f,int iidx,int fidx,int midx){


	/*                                            */
	/* Only change name of the IndividualIDField  */
	/* if the program assigned a default name:    */
	/*                                            */
	if(f[iidx].nm[0]=='C' && 
	   f[iidx].nm[1]=='H' && 
	   f[iidx].nm[2]=='A' && 
	   f[iidx].nm[3]=='R' &&
	   f[iidx].nm[4]=='_'
	){
		sprintf(f[iidx].nm,"INDIVIDUAL");
	}
	
	/*                                         */
	/* Only change name of the FatherIDField   */
	/* if the program assigned a default name: */
	/*                                         */
	if(f[fidx].nm[0]=='C' && 
	   f[fidx].nm[1]=='H' && 
	   f[fidx].nm[2]=='A' && 
	   f[fidx].nm[3]=='R' &&
	   f[fidx].nm[4]=='_'
	){
		sprintf(f[fidx].nm,"FATHER");
	}

	/*                                         */
	/* Only change name of the MotherIDField   */
	/* if the program assigned a default name: */
	/*                                         */
	if(f[midx].nm[0]=='C' && 
	   f[midx].nm[1]=='H' && 
	   f[midx].nm[2]=='A' && 
	   f[midx].nm[3]=='R' &&
	   f[midx].nm[4]=='_'
	){
		sprintf(f[midx].nm,"MOTHER");
	}
}		

/*                                                        */
/* CreateOpenCommandFile()                                */
/*                                                        */
/* bfn: batch file name to be written                     */
/*                                                        */
/* hfn: header file name to be opened by the batch file   */
/* iid: Individual ID field index                         */
/* fid: father ID field index                             */
/* mid: mother ID field index                             */
/* gid: gender field index                                */
/* lang: language convention used for writing dates, etc. */
/*                                                        */
void CreateOpenCommandFile(char *bfn,char *hfn,PPARSEDFIELDINFO f,int FieldCount,int iid, int fid, int mid, int gid,char *FemaleAndMaleValues,int lang,PPVARIABLE ivar){

	FILE *fp;
	int i;

	int AffectionStatusIndex=MISSING;
	int DateOfBirthIndex=MISSING;
	int DateOfDeathIndex=MISSING;
	int DeathStatusIndex=MISSING;
	int FamilyIDIndex=MISSING;
	int IndexCaseIndex=MISSING;	
	int MZTwinIndex=MISSING;
	int DZTwinIndex=MISSING;
	
	int ChromosomeIndex=MISSING;
	int LiabilityClassIndex=MISSING;
	int MarkerIndex=MISSING;
	int OrdinalIndex=MISSING;
	int PositionIndex=MISSING;
	int Allele1Index=MISSING;
	int Allele2Index=MISSING;
	int keymatch=0;

	char *female;
	char *male;
	
	/*                                                                      */
	/* While the core gender, individual, father, and mother fields         */
	/* may have been determined algorithmically, it is also possible that   */
	/* the detection algorithm did not succeed -- normally, this is due to  */
	/* the gender field being a numeric field.  In these cases, try to      */
	/* determine these core fields non-algorithmically by guessing based on */
	/* names.                                                               */
	/*                                                                      */
	/*                                                                      */
	/* It is also worthwhile to attempt to                                  */
	/* find other core fields based on field names as this results in a     */
	/* more complete batch command file that may require less editing by    */
	/* the user.  This loop very simplistically looks for the occurrence of */
	/* most likely English-language substrings in the field names:          */
	/*                                                                      */
	for(i=0;i<FieldCount;i++){
	
		/*                                             */
		/* Gender, Individual, Father, Mother, Family: */
		/*                                             */
		if(gid==MISSING && (Find(ivar[GENDERFIELD_IVI]->e.s,f[i].nm) || Find("GENDER",f[i].nm) || Find("SEX",f[i].nm) || Find("MALE",f[i].nm))){
			gid=i;
			keymatch++;
		}
		if(iid==MISSING && (Find(ivar[INDIVIDUALIDFIELD_IVI]->e.s,f[i].nm) || Find("STUDYID",f[i].nm) || Find("INDIV",f[i].nm) || Find("INDV",f[i].nm) || Find("EGO",f[i].nm))) iid=i;
		if(fid==MISSING && (Find(ivar[FATHERIDFIELD_IVI]->e.s,f[i].nm) || Find("FATHER",f[i].nm) || Find("DAD",f[i].nm))) fid=i;
		if(mid==MISSING && (Find(ivar[MOTHERIDFIELD_IVI]->e.s,f[i].nm) || Find("MOTHER",f[i].nm) || Find("MOM",f[i].nm))) mid=i;
		if(FamilyIDIndex==MISSING && (Find(ivar[FAMILYIDFIELD_IVI]->e.s,f[i].nm) || Find("FAMID",f[i].nm) || Find("FAMILY",f[i].nm))) FamilyIDIndex=i;
		/*                   */
		/* Affection Status: */
		/*                   */
		if(AffectionStatusIndex==MISSING && (Find(ivar[AFFECTIONSTATUSFIELD_IVI]->e.s,f[i].nm) || Find("AFFECT",f[i].nm))) AffectionStatusIndex=i;
		/*                    */
		/* MZTwin and DZTwin: */
		/*                    */
		if(MZTwinIndex==MISSING && (Find(ivar[MZTWINFIELD_IVI]->e.s,f[i].nm) || (Find("TWIN",f[i].nm)  && !Find("DZ",f[i].nm)))) MZTwinIndex=i;
		if(DZTwinIndex==MISSING && (Find(ivar[DZTWINFIELD_IVI]->e.s,f[i].nm) || (Find("TWIN",f[i].nm)  && Find("DZ",f[i].nm))))  DZTwinIndex=i;
		/*                          */
		/* Date of Birth and Death: */
		/*		                        */
		if(DateOfBirthIndex==MISSING && f[i].typ=='D' && (Find(ivar[DATEOFBIRTHFIELD_IVI]->e.s,f[i].nm) || Find("BIRTH",f[i].nm) || Find("DOB",f[i].nm))) DateOfBirthIndex=i;
		if(DateOfDeathIndex==MISSING && f[i].typ=='D' && (Find(ivar[DATEOFDEATHFIELD_IVI]->e.s,f[i].nm) || Find("DEATH",f[i].nm) || Find("DEAD",f[i].nm))) DateOfDeathIndex=i;
		/*               */
		/* Death Status: */
		/*               */
		if(DeathStatusIndex==MISSING && f[i].typ!='D' && (Find(ivar[DEATHSTATUSFIELD_IVI]->e.s,f[i].nm) || Find("DEATH",f[i].nm) || Find("DEAD",f[i].nm))) DeathStatusIndex=i;
		/*                     */
		/* Index/proband case: */
		/*                     */
		if(IndexCaseIndex==MISSING && (Find(ivar[INDEXCASEFIELD_IVI]->e.s,f[i].nm) || Find("PROBAND",f[i].nm) || Find("INDEX",f[i].nm))) IndexCaseIndex=i;

		/*                                                                                     */
		/* Chromosome, LiabilityClass, Marker, Ordinal, Position, Allele1, and Allele2 fields: */
		/*		                                                                                   */
		if(ChromosomeIndex    ==MISSING && Find(ivar[CHROMOSOMEFIELD_IVI    ]->e.s,f[i].nm)) ChromosomeIndex    =i;
		if(LiabilityClassIndex==MISSING && Find(ivar[LIABILITYCLASSFIELD_IVI]->e.s,f[i].nm)) LiabilityClassIndex=i;
		if(MarkerIndex        ==MISSING && Find(ivar[MARKERFIELD_IVI        ]->e.s,f[i].nm)) MarkerIndex        =i;		
		if(OrdinalIndex       ==MISSING && Find(ivar[ORDINALFIELD_IVI       ]->e.s,f[i].nm)) OrdinalIndex       =i;
		if(PositionIndex      ==MISSING && Find(ivar[POSITIONFIELD_IVI      ]->e.s,f[i].nm)) PositionIndex      =i;
		if(Allele1Index       ==MISSING && Find(ivar[ALLELE1FIELD_IVI       ]->e.s,f[i].nm)) Allele1Index       =i;
		if(Allele2Index       ==MISSING && Find(ivar[ALLELE2FIELD_IVI       ]->e.s,f[i].nm)) Allele2Index       =i;
		
	}

	
	fp=fopen(bfn,"wt");
	if(!fp) ToolsErrorReport("CreateOpenCommandFile(): Unable to open file %s for writing",bfn);

	fprintf(fp,"//\n");
	fprintf(fp,"// %s\n",bfn);
	fprintf(fp,"//\n");
	fprintf(fp,"// BATCH COMMAND FILE CREATED\n// %s\n// BY MADELINE'S \"RECOGNIZE\" COMMAND:\n",DelimitedGenericDateString(JulianToday(),'.'));
	fprintf(fp,"//\n");
	fprintf(fp,"// (This file generally requires editing before use)\n");
	fprintf(fp,"//\n");
	
	/*                                     */
	/* List the field names for reference: */
	/*                                     */
	fprintf(fp,"// Table field names:\n");
	for(i=0;i<FieldCount;i++){
		if(i%4==0) fprintf(fp,"\n// ");
		fprintf(fp,"%3i. %-11.11s ",i+1,f[i].nm);
	}
	fprintf(fp,"\n");


	fprintf(fp,"//\n");
	fprintf(fp,"// GENERAL SETUP COMMANDS:\n");
	fprintf(fp,"//\n");
	fprintf(fp,"quiet\n");
	fprintf(fp,"\n");

	fprintf(fp,"//\n");
	fprintf(fp,"// MADELINE CANNOT AUTOMATICALLY DETERMINE\n");
	fprintf(fp,"// THE SET OF MISSING VALUE INDICATORS WHICH\n");
	fprintf(fp,"// MAY BE USED IN A FLAT FILE DATABASE.  EDIT\n");
	fprintf(fp,"// OR ERASE HERE AS APPROPRIATE:\n");
	fprintf(fp,"//\n");
	fprintf(fp,"//CharacterMissingValue[???]=\"???\"\n");
	fprintf(fp,"//CharacterMissingValue[???]=\"???\"\n");
	fprintf(fp,"//CharacterMissingValue[???]=\"???\"\n");
	fprintf(fp,"\n");
	fprintf(fp,"//NumericMissingValue[???]=???\n");
	fprintf(fp,"//NumericMissingValue[???]=???\n");
	fprintf(fp,"//NumericMissingValue[???]=???\n");



	fprintf(fp,"//\n");
	fprintf(fp,"// CORE FIELDS REQUIRED IN A PEDIGREE TABLE:\n");
	fprintf(fp,"//\n");

	if(gid!=MISSING){
		fprintf(fp,"GenderField=\"%s\"\n",f[gid].nm);

		if(keymatch){
			/*                                                           */
			/* Gender field was identified within this function, which   */
			/* means that it's not known which value represents male and */
			/* which female, so just print question marks:               */
			/*                                                           */
			if(f[gid].typ=='N'){
				fprintf(fp,"// NumericSexValue[_female]=???\n");
				fprintf(fp,"// NumericSexValue[_Male  ]=???\n");
			}else{
				fprintf(fp,"// CharacterSexValue[_female]=\"???\"\n");
				fprintf(fp,"// CharacterSexValue[_Male  ]=\"???\"\n");
			}
		}else{
			/*                                                                                        */
			/* Gender field was identified algorithmically before this function                       */
			/* was called.  So, gender field must be a character field, and we                        */
			/* know that the FemaleAndMaleValues points to a valid string:                            */
			/*                                                                                        */
			/* FemaleAndMaleValues contains a string like "female male"                               */
			/* or "FEMALE MALE" or "f m", etc., depending on the database:                            */
			/*                                                                                        */
			female=male=FemaleAndMaleValues; /* female is the first value in the string               */
			while(*male!=' ') male++;        /* find where the value for male starts                  */
			*male++='\0';                    /* null terminate the value for female, and advance male */
			fprintf(fp,"CharacterSexValue[_female]=\"%s\"\n",female);
			fprintf(fp,"CharacterSexValue[_Male  ]=\"%s\"\n",male);
		}
	}else{
		fprintf(fp,"//GenderField=\"???\"\n");
		fprintf(fp,"// IF GenderField IS PRESENT, FILL IN EITHER HERE:\n");
		fprintf(fp,"// CharacterSexValue[_female]=\"???\"\n");
		fprintf(fp,"// CharacterSexValue[_Male  ]=\"???\"\n");
		fprintf(fp,"// OR HERE:\n");
		fprintf(fp,"// NumericSexValue[_female]=???\n");
		fprintf(fp,"// NumericSexValue[_Male  ]=???\n");
	}
	
	/*                                                     */
	/* check if individual, father, and mother fields were */
	/* identified or not:                                  */
	/*                                                     */
	if(iid!=MISSING){
		fprintf(fp,"IndividualIDField=\"%s\"\n",f[iid].nm);
	}else{
		fprintf(fp,"//IndividualIDField=\"???\"\n");
	}
	if(fid!=MISSING){
		fprintf(fp,"FatherIDField=\"%s\"\n",f[fid].nm);
	}else{
		fprintf(fp,"//FatherIDField=\"???\"\n");
	}
	if(mid!=MISSING){
		fprintf(fp,"MotherIDField=\"%s\"\n",f[mid].nm);
	}else{
		fprintf(fp,"//MotherIDField=\"???\"\n");
	}
	/*                  */
	/* Family ID Field: */
	/*                  */
	if(FamilyIDIndex!=MISSING){
		fprintf(fp,"FamilyIDField=\"%s\"\n",f[FamilyIDIndex].nm);	
	}else{
		fprintf(fp,"//FamilyIDField=\"???\"\n");	
	}
	
	fprintf(fp,"//\n");
	fprintf(fp,"// CORE FIELDS OPTIONAL IN A PEDIGREE TABLE:\n");
	fprintf(fp,"//\n");
	fprintf(fp,"// (remove any entries that are not needed):\n");
	fprintf(fp,"//\n");
	/*                         */
	/* Affection Status Field: */
	/*                         */
	if(AffectionStatusIndex!=MISSING){
		fprintf(fp,"AffectionStatusField=\"%s\"\n",f[AffectionStatusIndex].nm);
		if(f[AffectionStatusIndex].typ=='C'){
			fprintf(fp,"// CharacterAffectionStatus[_affected  ]=\"???\"\n");
			fprintf(fp,"// CharacterAffectionStatus[_unaffected]=\"???\"\n");
		}else{
			fprintf(fp,"// NumericAffectionStatus[_affected  ]=???\n");
			fprintf(fp,"// NumericAffectionStatus[_unaffected]=???\n");		
		}
	}else{
		fprintf(fp,"//AffectionStatusField=\"???\"\n");
		fprintf(fp,"// IF AffectionStatusField IS PRESENT, FILL IN EITHER HERE:\n");
		fprintf(fp,"//  CharacterAffectionStatus[_affected  ]=\"???\"\n");
		fprintf(fp,"//  CharacterAffectionStatus[_unaffected]=\"???\"\n");
		fprintf(fp,"// OR HERE:\n");
		fprintf(fp,"//  NumericAffectionStatus[_affected  ]=???\n");
		fprintf(fp,"//  NumericAffectionStatus[_unaffected]=???\n");		
	}
	
	/*                */
	/* MZ Twin Field: */
	/*                */
	if(MZTwinIndex!=MISSING){
		fprintf(fp,"MZTwinField=\"%s\"\n",f[MZTwinIndex].nm);	
	}else{
		fprintf(fp,"//MZTwinField=\"???\"\n");		
	}	
	/*                */
	/* DZ Twin Field: */
	/*                */
	if(DZTwinIndex!=MISSING){
		fprintf(fp,"DZTwinField=\"%s\"\n",f[DZTwinIndex].nm);	
	}else{
		fprintf(fp,"//DZTwinField=\"???\"\n");		
	}	
	/*                      */
	/* Date of Birth Field: */
	/*                      */
	if(DateOfBirthIndex!=MISSING){
		fprintf(fp,"DateOfBirthField=\"%s\"\n",f[DateOfBirthIndex].nm);	
	}else{
		fprintf(fp,"//DateOfBirthField=\"???\"\n");		
	}
	/*                      */
	/* Date of Death Field: */
	/*                      */
	if(DateOfDeathIndex!=MISSING){
		fprintf(fp,"DateOfDeathField=\"%s\"\n",f[DateOfDeathIndex].nm);	
	}else{
		fprintf(fp,"//DateOfDeathField=\"???\"\n");		
	}	
	/*                     */
	/* Death Status Field: */
	/*                     */
	if(DeathStatusIndex!=MISSING){
		fprintf(fp,"DeathStatusField=\"%s\"\n",f[DeathStatusIndex].nm);	
		if(f[DeathStatusIndex].typ=='C'){
			/*                  */
			/* Character field: */
			/*                  */
			fprintf(fp,"// CharacterDeathStatus[_dead ]=\"???\"\n");
			fprintf(fp,"// CharacterDeathStatus[_alive]=\"???\"\n");
		}else{
			/*                         */
			/* Assume a numeric field: */
			/*                         */
			fprintf(fp,"// NumericDeathStatus[_dead ]=???\n");
			fprintf(fp,"// NumericDeathStatus[_alive]=???\n");
		}
	}else{
		fprintf(fp,"//DeathStatusField=\"???\"\n");	
			fprintf(fp,"// IF DeathStatusField IS PRESENT, FILL IN EITHER HERE:\n");
			fprintf(fp,"//  CharacterDeathStatus[_dead ]=\"???\"\n");
			fprintf(fp,"//  CharacterDeathStatus[_alive]=\"???\"\n");
			fprintf(fp,"// OR HERE:\n");
			fprintf(fp,"//  NumericDeathStatus[_dead ]=???\n");
			fprintf(fp,"//  NumericDeathStatus[_alive]=???\n");
	}

	fprintf(fp,"//\n");
	fprintf(fp,"// ADDITIONAL CORE FIELDS:\n");
	fprintf(fp,"//\n");
	fprintf(fp,"// (uncomment any entries that are additionally required):\n");
	fprintf(fp,"//\n");

	
	/*                   */
	/* Index Case Field: */
	/*                   */
	if(IndexCaseIndex!=MISSING){
		fprintf(fp,"IndexCaseField=\"%s\"\n",f[IndexCaseIndex].nm);	
	}else{
		fprintf(fp,"// IndexCaseField=\"???\"\n");		
	}	
	/*                      */
	/* LiabilityClassField: */
	/*                      */
	if(LiabilityClassIndex!=MISSING){
		fprintf(fp,"LiabilityClassField=\"%s\"\n",f[LiabilityClassIndex].nm);	
	}else{
		fprintf(fp,"// LiabilityClassField=\"???\"\n");		
	}

	fprintf(fp,"//\n");
	fprintf(fp,"// ADDITIONAL CORE FIELDS REQUIRED IN A MAP OR MARKER TABLE:\n");
	fprintf(fp,"//\n");
	fprintf(fp,"// (uncomment any entries that are additionally required):\n");
	fprintf(fp,"//\n");
		
	if(ChromosomeIndex!=MISSING){
		fprintf(fp,"ChromosomeField=\"%s\"\n",f[ChromosomeIndex].nm);	
	}else{
		fprintf(fp,"// ChromosomeField=\"???\"\n");		
	}	
	if(MarkerIndex!=MISSING){
		fprintf(fp,"MarkerField=\"%s\"\n",f[MarkerIndex].nm);	
	}else{
		fprintf(fp,"// MarkerField=\"???\"\n");		
	}	
	if(OrdinalIndex!=MISSING){
		fprintf(fp,"OrdinalField=\"%s\"\n",f[OrdinalIndex].nm);	
	}else{
		fprintf(fp,"// OrdinalField=\"???\"\n");		
	}	
	if(PositionIndex!=MISSING){
		fprintf(fp,"PositionField=\"%s\"\n",f[PositionIndex].nm);	
	}else{
		fprintf(fp,"// PositionField=\"???\"\n");		
	}	
	if(Allele1Index!=MISSING){
		fprintf(fp,"Allele1Field=\"%s\"\n",f[Allele1Index].nm);	
	}else{
		fprintf(fp,"// Allele1Field=\"???\"\n");		
	}	
	if(Allele2Index!=MISSING){
		fprintf(fp,"Allele2Field=\"%s\"\n",f[Allele2Index].nm);	
	}else{
		fprintf(fp,"// Allele2Field=\"???\"\n");		
	}		
	
	fprintf(fp,"//\n");
	fprintf(fp,"// OPEN THE TABLE:\n");
	fprintf(fp,"//\n");	
	fprintf(fp,"open \"%s\"\n",hfn);

	fprintf(fp,"//\n");
	fprintf(fp,"// *** END OF MADELINE-GENERATED BATCH FILE ***\n");
	fprintf(fp,"//\n");
	/*                   */
	/* close batch file: */
	/*                   */
	fclose(fp);
	/*                         */
	/* Report success to user: */
	/*                         */
	lprintf("\nThe template batch file %s has been created.\n\n",bfn);
	lprintf("NOTE: The \"%s\" file contains commands and parameters to assist\n",RUN_FILE_EXTENSION);
	lprintf("      you in opening a flat file database, but generally requires\n");
	lprintf("      editing before use.\n\n");
}



/*                       */
/* GetGenderFieldIndex() */
/*                       */
int GetGenderFieldIndex(PPARSEDFIELDINFO f,int FieldCount){

	int i;
	
	for(i=0;i<FieldCount;i++)
		/*                                 */
		/* return first field of type "X": */
		/*                                 */
		if(f[i].typ=='X') return i;
	/*                              */
	/* Gender field not identified: */
	/*                              */
	return MISSING;
}


/*                                */
/* DetermineCoreRelationFields(): */
/*                                */
int DetermineCoreRelationFields(PPARSEDFIELDINFO f,int FieldCount,PLINEINFO p,int DataLines,int GenderFieldIndex,int *IndividualFieldIndex,int *FatherFieldIndex,int *MotherFieldIndex){


	int Count[MAXIDWIDTH];
	int NonMissingCount[MAXIDWIDTH];
	int i;

	/*              */
	/* zero counts: */
	/*	             */
	for(i=0;i<MAXIDWIDTH;i++) NonMissingCount[i]=Count[i]=0;
	
	/*                                    */
	/* Find out how many character fields */
	/* are in each field length category: */
	/*                                    */
	for(i=0;i<FieldCount;i++){
		if(f[i].typ=='C' && f[i].len<=MAXIDWIDTH){
			/*                                       */
			/* count of fields in this length class: */
			/*                                       */
			Count[f[i].len-1]++;
			/*                                      */
			/* how many are completely non-missing? */
			/*                                      */
			if(!f[i].missing) NonMissingCount[f[i].len-1]++;
		}
	}
	/*                                                              */
	/* Process each group of three or more character fields having  */
	/* the same length and at least one non-missing field:          */
	/*                                                              */
	/* Remember that the field length group is one less than the    */
	/* field length due to the zero-offset nature of the arrays, so */
	/* we add +1 when passing to FindIndividualIDField:             */
	/*                                                              */
	for(i=0;i<MAXIDWIDTH;i++){
		if(Count[i]>=3 && NonMissingCount[i]<Count[i]){
			/*                                                 */
			/* Check group of character fields of this length: */
			/*                                                 */
			*IndividualFieldIndex=FindIndividualIDField(i+1,
				f,
				FieldCount,
				p,
				DataLines,
				GenderFieldIndex,
				FatherFieldIndex,
				MotherFieldIndex
			);
			
			if(*IndividualFieldIndex!=MISSING) return 1;
		}
	}
	return 0;
}


/*                             */
/* int FindIndividualIDField() */
/*                             */
int FindIndividualIDField(int FieldLengthGroup,PPARSEDFIELDINFO f,int FieldCount,PLINEINFO p,int DataLines,int GenderFieldIndex,int *FatherFieldIndex,int *MotherFieldIndex){

	int i;
	PNODE tree=NULL;
	
	for(i=0;i<FieldCount;i++){
		/*                                                                 */
		/* Potential IndividualIDField must belong to a group of character */
		/* fields having the right field length and must                   */
		/* contain no missing entries:                                     */
		/*                                                                 */
		if(f[i].len==FieldLengthGroup && !f[i].missing){
			/*                                                            */
			/* Read ID and gender data into a BST: BuildIDGenderTree()    */
			/* checks whether the list looks like a list of unique IDs or */
			/* not, and only returns a non-NULL tree if the list contains */
			/* unique, non-repeating keys:                                */
			/*                                                            */
			tree=BuildIDGenderTree(i,f,p,DataLines,GenderFieldIndex);
			if(tree){
				/*                               */
				/* Try to find the father field: */
				/*                               */
				*FatherFieldIndex=SeekParentField(tree,i,FieldLengthGroup,f,FieldCount,p,DataLines,MALESEX);
				if(*FatherFieldIndex!=MISSING){
					/*                               */
					/* Try to find the mother field: */
					/*                               */
					*MotherFieldIndex=SeekParentField(tree,i,FieldLengthGroup,f,FieldCount,p,DataLines,FEMALESEX);
					/*                                                          */
					/* Do we now have both father and mother field indexes?     */
					/* -- if yes, return the index of the IndividualIDField, i: */
					/*                                                          */
					if(*MotherFieldIndex!=MISSING){
						/*                                                      */
						/* Remove tree and return index of successful ID field: */
						/*                                                      */
						FreeTree(tree,FreeIDObject);
						return i;
					}
				}
			}
		}	
	}
	/*                         */
	/* Remove tree and return: */
	/*                         */
	FreeTree(tree,FreeIDObject);
	return MISSING;
}


/*                   */
/* SeekParentField() */
/*                   */
int SeekParentField(PNODE tree,int IDFieldIndex,int FieldLengthGroup,PPARSEDFIELDINFO f,int FieldCount,PLINEINFO p,int DataLines,int female){

	int i;

	/* int j;          */
	/* PNODE ThisNode; */
	
	for(i=0;i<FieldCount;i++){
			
		/*                                                             */
		/* Potential ParentField must belong to the group of character */
		/* fields having the right field length and must not be the    */
		/* IndvIDField itself, and non-missing entries must be of the  */
		/* correct sex:                                                */
		/*                                                             */
		if(i!=IDFieldIndex && f[i].len==FieldLengthGroup){
			/*                                  */
			/* See if there are enough matches: */
			/*                                  */
			if(TestCandidateParentField(tree,i,f,p,DataLines,female)){
				/*                                             */
				/* return index of successful candidate field: */
				/*                                             */
				return i;
			}
		}
	}
	return MISSING;
}



/*                              */
/* TestCandidateParentField():  */
/*                              */
int TestCandidateParentField(PNODE tree,int TestFieldIndex,PPARSEDFIELDINFO f,PLINEINFO p,int DataLines,int female){

	int j;
	PNODE ThisNode;
	IDOBJECT id,*pid=&id; /* to hold ID	 */
	int match;
		
	/*                                  */
	/* examine candidate parental field */
	/*		                                */
	for(match=j=0;j<DataLines;j++){
		/*          */
		/* read ID: */
		/*          */
		ReadCharacterData(pid->id,p[j].b,f[TestFieldIndex].stt,f[TestFieldIndex].end);
		/*             */
		/* Look it up: */
		/*             */
		ThisNode=SeekNode(tree,pid,CompareIDObjects);
		if(ThisNode){
			/*                          */
			/* A matching ID was found: */
			/*                          */
			if( ((PIDOBJECT)ThisNode->o)->female==female){
				/*                 */
				/* Gender matches: */
				/*                 */
				match++;
			
			}else{
				/*                                                 */
				/* gender does not match, so it's not the parental */
				/* field we were hoping for, or else the data are  */
				/* somewhat corrupt:                               */
				/*                                                 */
				return 0;
			}
		}
	}
	/*                                                */
	/* return count where both ID and gender matched: */
	/*                                                */
	return match;
}




/*                   */
/* BuildIDGenderTree */
/*                   */
PNODE BuildIDGenderTree(int IDFieldIndex,PPARSEDFIELDINFO f,PLINEINFO p,int DataLines,int GenderFieldIndex){

	int i;

	PNODE tree=NULL;		
	IDOBJECT id,*pid=&id;

	for(i=0;i<DataLines;i++){
		/*          */
		/* read ID: */
		/*          */
		ReadCharacterData(pid->id,p[i].b,f[IDFieldIndex].stt,f[IDFieldIndex].end);
		/*              */
		/* read gender: */
		/*              */
		pid->female=ReadGenderCode(p[i].b,f[GenderFieldIndex].stt,f[GenderFieldIndex].end);

		if(!tree) tree=HeadNode((void *)pid,CopyIDObject);
		else tree=AddNode(tree,(void *)pid,CompareIDObjects,CopyIDObject,1);
		
	}
	/*                                                                                    */
	/* Look for duplicates in tree - They are not allowed if STUDYIDs are totally unique: */
	/*                                                                                    */
	if(IDTreeHasRepeats(tree)){
		/*                  */
		/* remove the tree: */
		/*                  */
		FreeTree(tree,FreeIDObject);
		/*              */
		/* return NULL: */
		/*              */
		return NULL;	
	}
	
	/*                                   */
	/* This tree is a potential ID list: */
	/*                                   */
	return tree;
}



/*                                     */
/* Warning(): prints a warning message */
/*                                     */
/*void Warning(char *s,...){           */
/*                                     */
/*	va_list argptr;                     */
/*	                                    */
/*	fprintf(stderr,"Warning:\n");       */
/*	va_start(argptr,s);                 */
/*	vfprintf(stderr,s,argptr);          */
/*	va_end(argptr);                     */
/*	printf("\n");                       */
/*};                                   */
/*                                     */


/*                                                                     */
/* DataBlockContainsTabs(): Detects if file contains tab characters in */
/* the DATA block.  We don't care about tabs in the header.            */
/*                                                                     */
int DataBlockContainsTabs(PLINEINFO p,PBLOCKINDICES pBlockIndices){
	
	register int i;
	
	/*                                     */
	/* keep a running sum of the tab count */
	/* -- the tb field has already been    */
	/* filled by GetLineInfo():            */
	/*                                     */
	
	for(i=pBlockIndices->dataStart;i<=pBlockIndices->dataEnd;i++){
		if(p[i].tb) return 1;
	}
	return 0;
	
}


/*                                                                                */
/* DataBlockLinesUneven(): Detects if the DATA block contains uneven line lengths */
/*                                                                                */
int DataBlockLinesUneven(PLINEINFO p,PBLOCKINDICES pBlockIndices){
	
	int i;
	
	for(i=pBlockIndices->dataStart;i<=pBlockIndices->dataEnd;i++){
		if(p[i].ln!=pBlockIndices->maxDataLineLength) return 1;
	}
	return 0;
}



void TrimRightEdges(PLINEINFO p,int startingLine,int EndingLine){
	
	int i;
	char *b;
	char *o;
	
	for(i=startingLine;i<=EndingLine;i++){
		
		o=b=p[i].b+p[i].ln-1;
		/*                                                 */
		/* '\r' and '\n' are not expected to be presently  */
		/* remaining on the right edge of the line, but    */
		/* are nonetheless included here as a prophylactic */
		/* measure:                                        */
		/*                                                 */
		for(;b>=p[i].b && (*b==' ' || *b=='\t' || *b=='\r' || *b=='\n');*b--='\0');
		/*                                   */
		/* Adjust line length, if necessary: */
		/*                                   */
		if(b!=o) p[i].ln=b-p[i].b+(*p[i].b?1:0);
	}
	
}


/*                                                                           */
/* DetermineTabExpansion(): Determine up to how many spaces should be        */
/* used to expand tab characters in the DATA block.  Also stores the maximum */
/* expanded line length in *ExpandedLength.  Even if no tabs occur in        */
/* the data lines of the file, this routine can still be used to find out    */
/* to what length the data lines must be padded if the data lines are        */
/* of uneven lengths.                                                        */
/*                                                                           */
int DetermineTabExpansion(PLINEINFO p,PBLOCKINDICES pBlockIndices,int *ExpandedLength){
	
	int top,bot;
	int i,j,l,t;
	int maxln,maxtb,maxll;
	
	int s[MAXTABEXPANSION+1]; /* vector of column empty counts for each tab size */
	int sidx;
	int maxs;
	int mel[MAXTABEXPANSION+1]; /* maximum expanded line length for each tab size */
	
	char *b;       /* convenient pointer to the line */
	int *fill;     /* vector of column fill counts   */
	int expansion; /* tab expansion amount	          */
	
	/*                                                                   */
	/* zero s and mel vector: actually, s[0] and mel[0] will be ignored: */
	/*                                                                   */
	for(t=0;t<=MAXTABEXPANSION;t++) mel[t]=s[t]=0;
	
	/*                                                                    */
	/* Determine maximum line length and maximum number of tabs per line: */
	/* Also set the top and bot to the start and end of the DATA block:   */
	/* 2004.04.21.ET: revised as below                                    */
	/*                                                                    */
	top  =pBlockIndices->dataStart;
	bot  =pBlockIndices->dataEnd;
	maxln=pBlockIndices->maxDataLineLength;
	maxtb=pBlockIndices->maxDataLineTabCount;
	/*                                                             */
	/* Allocate fill vector based on maximum possible line length  */
	/* under assumption of maximum tab expansion:                  */
	/*                                                             */
	/* The reason for MAXTABEXPANSION-1 is that the TAB character  */
	/* itself counts for one byte which will be replaced by one of */
	/* the replacements spaces in an expansion, so we have to sub- */
	/* tract this off.                                             */
	/*                                                             */
	maxll=maxln+maxtb*(MAXTABEXPANSION-1);
	fill=NewIntVector(maxll);
	
	/*                                               */
	/* Outermost loop through different tab sizes: t */
	/*                                               */
	for(t=MAXTABEXPANSION;t>=1;t--){
		/*                                          */
		/* loop through the DATA block in the file: */
		/*                                          */
		for(l=top;l<=bot;l++){
			/*                           */
			/* assign b for convenience: */
			/*                           */
			b=p[l].b;
			
			/*                       */
			/* loop through columns: */
			/*                       */
			for(j=i=0;i<p[l].ln;i++,j++){
				/*                                      */
				/* Replace each occurrence of           */
				/* a tab by it's the virtual expansion. */
				/*                                      */
				/*                                      */
				/*				                                  */
				if(b[i]=='\t'){
					/*                             */
					/* Subtract one from           */
					/* expansion because           */
					/* j is __always__ incremented */
					/* in the for(;;), so we must  */
					/* avoid over-incrementing:    */
					/*                             */
					expansion=t-(j%t)-1;
					j+=expansion;
				}
				if(!(b[i]==' ' || b[i]=='\t')) fill[j]++;
			}
			/*                                            */
			/* Find maximum expanded line length (mel[]): */
			/*                                            */
			if(j>mel[t]) mel[t]=j;
		}
		
		/*                            */
		/* DEBUGGING CODE:            */
		/*                            */
		/*                            */
		/*printf("t=%i\n",t);         */
		/*for(j=0;j<maxll;j++){       */
		/*	if(fill[j])                */
		/*		printf("%1i",fill[j]/10); */
		/*	else                       */
		/*		printf(" ");              */
		/*}                           */
		/*printf("\n");               */
		/*for(j=0;j<maxll;j++){       */
		/*	if(fill[j])                */
		/*		printf("%1i",fill[j]%10); */
		/*	else                       */
		/*		printf(".");              */
		/*}		                         */
		/*printf(":");                */
		
		/*                                                       */
		/* Calculate number of __empty__ columns (i.e.           */
		/* those that are filled with spaces down the __entire__ */
		/* column.  Then, clear out fill vector for the next     */
		/* round (in the else statement):                        */
		/*                                                       */
		for(j=0;j<maxll;j++){
			if(!fill[j]) s[t]++;
			else         fill[j]=0;	
		}
		
		/*                     */
		/* DEBUGGING CODE:     */
		/*                     */
		/*                     */
		/*                     */
		/*printf("%i\n",s[t]); */
		/*                     */
	}
	/*                                                          */
	/* Determine which expansion is optimal (i.e., has the most */
	/* number of completely empty columns):                     */
	/*                                                          */
	for(maxs=sidx=0,t=1;t<=MAXTABEXPANSION;t++){
		/*                                      */
		/* DEBUG: Show expansion fill histogram */
		/*                                      */
		/* lprintf("s[%i] = %i\n",t,s[t]);      */
		if(s[t]>maxs){
			maxs=s[t];
			sidx=t;
			*ExpandedLength=mel[t];
		}
	}
	/*                                                 */
	/* Look for cases of multiple solutions:           */
	/* Yes, sometimes a tab expansion of either 6 or 8 */
	/* would work just fine.  Or 3 or 7 would be just  */
	/* hunky-dory.  Such is life:                      */
	/*                                                 */
	for(i=0,t=1;t<=MAXTABEXPANSION;t++){
		if(s[t]==s[sidx]) i++;
	}
	
	/*                                                   */
	/* If i>1, then there are multiple valid solutions:  */
	/* In such cases, either expansion should work just  */
	/* as well as any other expansion ...                */
	/*                                                   */
	if(i>1){
		
		lprintf("There are %i equally valid tab expansion solutions:\n\n",i);
		for(t=MAXTABEXPANSION;t>=1;t--){
			if(s[t]==s[sidx]){
				
				lprintf("Tab size = %i\n",t);
				
			}
		}
		if(i==8){
			
			lprintf("\n(Columns in this file are likely completely delimited by tabs\n");
			lprintf("which is why any expansion will work equally well).\n");
		}
		/*                                                   */
		/* ... but of course we use the smallest expansion:  */
		/* (the loop that determines optimal expansion runs  */
		/* from one up to MAXTABEXPANSION, so in the case of */
		/* ties, the smallest one gets selected)             */
		/*                                                   */
		lprintf("\nMadeline will convert the file using a tab setting of %i space%s.\n",sidx,(sidx>1?"s":""));
		
	}else{
		
		lprintf("An optimal tab expansion solution was found using a tab setting of %i spaces.\n",sidx);
		
	}
	
	return sidx;
	
}



/*                                                            */
/* WriteTabExpandedFile(): Replaces a tabbed file with spaces */
/*                                                            */
void WriteTabExpandedFile(PLINEINFO p,PBLOCKINDICES pBlockIndices,int tabsize,int linelength,const char *fn){
	
	int i,j,k,l;
	char *b;
	FILE *fp;
	int expansion;
	
	
	fp=fopen(fn,"wt");
	if(!fp) ToolsErrorReport("WriteTabExpandedFile(): Can't open %s for writing",fn);
	
	// Write out the header lines:
	if(pBlockIndices->headerStart>=0){
		lprintf("Writing header lines to %s...\n",fn);
		for(i=pBlockIndices->headerStart;i<=pBlockIndices->headerEnd;i++){
			fprintf(fp,"%s\n",p[i].b);
		}
	}
	// Write one blank line after the header:
	fprintf(fp,"\n");
	
	// Now write tab-expanded lines of the data grid:
	lprintf("Writing tab-expanded data rows to %s...\n",fn);
	/*                     */
	/* loop through lines: */
	/*                     */
	for(l=pBlockIndices->dataStart;l<=pBlockIndices->dataEnd;l++){
		
		b=p[l].b;
		/*                       */
		/* loop through columns: */
		/*                       */
		for(j=i=0;i<p[l].ln;i++,j++){
			/*                            */
			/* Replace each occurrence of */
			/* a tab by it's expansion.   */
			/*                            */
			if(b[i]=='\t'){
				/*                      */
				/* calculate expansion: */
				/*                      */
				expansion=tabsize-(j%tabsize);
				/*                             */
				/* Avoid over incrementing j   */
				/* which is always incremented */
				/* in the for(;;) loop at top: */
				/*                             */
				j+=(expansion-1);
				/*                             */
				/* write out expansion spaces: */
				/*                             */
				for(k=0;k<expansion;k++) fputc(' ',fp);
				
			}else{
				/*                               */
				/* write out character directly: */
				/*                               */
				fputc(b[i],fp);
			}
		}
		/*                                            */
		/* pad line out to line length, if necessary: */
		/*                                            */
		for(;j<linelength;j++) fputc(' ',fp);
		/*                                          */
		/* add carriage return at end of each line: */
		/*                                          */
		fputc('\n',fp);
	}
	/*             */
	/* close file: */
	/*             */
	fclose(fp);	
}


/*                                                               */
/* WritePaddedLineFile(): For a file that does not contain       */
/* tabs, but that does have uneven line lengths, this function   */
/* writes out a new file with all lines padded out to the length */
/* of the longest trimmed line:                                  */
/*                                                               */
void WritePaddedLineFile(PLINEINFO p,PBLOCKINDICES pBlockIndices,const char *fn){
	
	int i,j;
	int maxln;
	FILE *fp;
	
	/*                                           */
	/* Determine length of longest trimmed line: */
	/*                                           */
	maxln=0;
	for(i=pBlockIndices->dataStart;i<=pBlockIndices->dataEnd;i++){
		if(p[i].ln>maxln) maxln=p[i].ln;
	}
	/*                      */
	/* open and write file: */
	/*                      */
	fp=fopen(fn,"wt");
	if(!fp) ToolsErrorReport("WritePaddedLineFile(): Unable to open %s for writing",fn);
	
	// Write out the header lines:
	if(pBlockIndices->headerStart>=0){
		lprintf("Writing header lines to %s...\n",fn);
		for(i=pBlockIndices->headerStart;i<=pBlockIndices->headerEnd;i++){
			fprintf(fp,"%s\n",p[i].b);
		}
	}
	// Write one blank line after the header:
	fprintf(fp,"\n");
	
	// Now write padded lines of the data grid:
	lprintf("Writing padded data rows to %s...\n",fn);
	/*                     */
	/* loop through lines: */
	/*                     */
	for(i=pBlockIndices->dataStart;i<=pBlockIndices->dataEnd;i++){
		
		/*                 */
		/* print the line: */
		/*                 */
		j=fprintf(fp,"%s",p[i].b);
		/*                                */
		/* pad out the line if necessary: */
		/*                                */
		for(;j<maxln;j++) fputc(' ',fp);
		fputc('\n',fp);
	}
	fclose(fp);
}



/*                                                                             */
/* GetFemaleAndMaleValues(): Get the values used to represent male and female, */
/* for example "f m" or "F M" or "FEMALE MALE" or "female male", etc.          */
/*                                                                             */
char *GetFemaleAndMaleValues(int GenderFieldIndex,PPARSEDFIELDINFO f,PLINEINFO p,int DataLines){

	static char fm[MINIMUMBUFFERSIZE];
	char *stt;	int i;
	
	for(stt=fm,i=0;i<DataLines;i++){
		/*                          */
		/* Get female gender value: */
		/*                          */
		ReadCharacterData(stt,p[i].b,f[GenderFieldIndex].stt,f[GenderFieldIndex].end);
		if(*stt=='f' || *stt=='F'){
			break;		
		}
	}
	/*                                             */
	/* advance stt to the end of the female value: */
	/*                                             */
	while(*stt) stt++;
	/*                                                  */
	/* Change the null terminator to space, advance stt */
	/* to next position to record male value:           */
	/*                                                  */
	*stt++=' ';
	for(i=0;i<DataLines;i++){
		/*                        */
		/* Get male gender value: */
		/*                        */
		ReadCharacterData(stt,p[i].b,f[GenderFieldIndex].stt,f[GenderFieldIndex].end);
		if(*stt=='m' || *stt=='M'){
			break;		
		}
	}
	/*                                         */
	/* return pointer to static string memory: */
	/*                                         */
	return fm;
}


/*                                                                         */
/* If allele fields, marked with f[i].typ=='N', are present, this function */
/* rearranges the database field structure so that two consecutive         */
/* allele fields are treated as a single genotype field.                   */
/*                                                                         */
int ConvertAlleleFieldsToGenotypeFields(PPARSEDFIELDINFO f,int FieldCount){

	int i,j;
	
	/*                                                                    */
	/* j indexes the actual fields while i skips ahead when allele fields */
	/* are present:                                                       */
	/*                                                                    */
	for(j=i=0;i<FieldCount-1;j++,i++){
	
		if(f[i].typ=='A' && f[i+1].typ=='A' && strcmp(f[i].nm,f[i+1].nm)==0){
			/*                                                             */
			/* This field and the next both are coded as "A" allele fields */
			/* and both have the same marker name, so convert to a single  */
			/* genotype field name:                                        */
			/*                                                             */
			
			/*                                                             */
			/* j won't equal i after the first allele field pair has been  */
			/* processed:                                                  */
			/*                                                             */
			if(j!=i) strcpy(f[j].nm,f[i].nm);
			if(j!=i) f[j].stt=f[i].stt;
			f[j].end=f[i+1].end;
			f[j].len=f[j].end-f[j].stt;
			f[j].spc=f[i+1].spc;
			f[j].prc=0;   /* precision must be zero */
			f[j].typ='G'; /* now a genotype field   */
			
			/*                                                  */
			/* advance i by one: the for() loop then advances i */
			/* once again:                                      */
			i++;
		}
	}
	/*                                                                 */
	/* return the modified field count:                                */
	/*                                                                 */
	/* If i==j, then no allele fields were found, so return FieldCount */
	/* unchanged, else return j, the new field count:                  */
	/*                                                                 */
	return i==j?FieldCount:j;
}

/*                                                   */
/* RecognizeCrimapFile():                            */
/*                                                   */
/* -- Recognize the genetic map information          */
/*    contained in a Crimap file or even in an HTML  */
/*    file from an online genetic map server such    */
/*    as Map-O-Mat which uses Crimap on the backend. */
/*    Both Sex-specific and Sex-averaged maps are    */
/*    recognized automatically.                      */
/*                                                   */
int RecognizeCrimapFile(char *fin,char *fmap){

	char *b;            /* file buffer                  */
	int lines;          /* number of lines	             */
	PLINEINFO p;        /* vector of line information   */
	
	char BaseKeyString[]        = "Sex";
	char SexSpecificKeyString[] = "Sex-specific";
	char SexAveragedKeyString[] = "Sex_averaged";
	int  IsSexSpecific=-1;
	int  i,j,MarkerCount;
	
	char delim[] = " ";  /* space delimiter in Crimap files */
	char mvi[]   = ".";  /* missing value indicator         */
	PMAPTOKENS MapTokens;
	char *chr_str;
	char *s;
	int chromosome=0;
	
	FILE *pmap; /* Madeline-formatted map output file */
	
	/*                       */
	/* Open and buffer file: */
	/*                       */
	b=OpenAndBufferFile(fin);
	/*                      */
	/* Count lines in file: */
	/*                      */
	lines=GetLineCount(b);
	/*                                               */
	/* Assign pointers to the start of each line and */
	/* also figure line length, marking empty lines: */
	/*                                               */
	p=GetLineInfo(b,lines);	
	
	/*                                                                */
	/* Search for the line that says "Sex-specific" or "Sex_averaged: */
	/*                                                                */
	for(i=0;i<lines;i++){
		
		/*                                          */
		/* Search for the Chromosome number:        */
		/* This always appears before the map data. */
		if(!chromosome && (chr_str=FindEnd("dat_file = chr",p[i].b))){
			for(s=chr_str;*s!='.';s++); /* walk to full stop ...    */
			*s='\0';                    /* ... and terminate string */
			chromosome=atoi(chr_str);
			lprintf("This is a map of chromosome %i markers ...\n",chromosome);
			
		}
		
		/*                                                                     */
		/* Search for the key string marking beginning of subsequent map data: */
		/*                                                                     */
		if(strncmp(BaseKeyString,p[i].b,strlen(BaseKeyString))==0){
			if(strncmp(SexSpecificKeyString,p[i].b,strlen(SexSpecificKeyString))==0){
				IsSexSpecific=1;
			}else if(strncmp(SexAveragedKeyString,p[i].b,strlen(SexAveragedKeyString))==0){
				IsSexSpecific=0;
			}else{
				continue;
			}
			/* Get here if beginning of line matches key string: */
			break;
		}
	}
	
	/* If we read the entire file without finding the key string we want, it's */
	/* not a Crimap file:                                                      */
	if(IsSexSpecific==-1){
		free(b);
		free(p);
		MadelineErrorReport("RecognizeCrimapFile(): Unable to find either key string,\n\"%s\" or \"%s\".\nPerhaps the file is not a Crimap file or is corrupt.",
			SexSpecificKeyString,
			SexAveragedKeyString
		);
	}else{
		
		lprintf("Converting sex-%s map ...\n",(IsSexSpecific?"specific":"averaged"));
		
	}
	
	/*                                                                  */
	/* The next line after the key string should be blank               */
	/* After that, each map record takes up two lines:                  */
	/* The first line contains the marker name and cumulative distance. */
	/* The second line contains the recombination fraction, theta, and  */
	/* the inter-marker distance.                                       */
	/* The map stops when you reach a blank line.                       */
	/*                                                                  */
	
	/* Here we will allow any number of blank lines before the data begin: */
	/* even though normally there will only be one blank line              */
	for(++i;i<lines;i++){
		if(p[i].b[0]) break;
	}
	
	/* Skip ahead looking for the next blank line after the map data block */
	/* in order to determine the number of markers in the map              */
	for(j=i;j<lines;j++){
		if(!p[j].b[0]) break;
	}
	
	/* Recall that data for each marker spans 2 rows in the Crimap file: */
	MarkerCount=(j+1-i)/2;
	
	/* Allocate the MAPTOKENS array: */
	MapTokens=NewMapTokensArray(MarkerCount);
	
	for(j=0;j<MarkerCount;j++,i++){
		
		/* First grab tokens from FIRST line of data (p[i].b): */
		MapTokens[j].number    =strtok(p[i].b,delim);
		MapTokens[j].markername=strtok(NULL,delim);
		MapTokens[j].position1 =strtok(NULL,delim);
		if(IsSexSpecific) MapTokens[j].position2=strtok(NULL,delim);
		/* Next grab tokens from SECOND line of data (p[i+1].b):   */
		/* BUT there will be no second line of data if this is the */
		/* LAST marker:                                            */
		if(j<MarkerCount-1){
			MapTokens[j].theta1    =strtok(p[++i].b,delim);
			MapTokens[j].distance1 =strtok(NULL,delim);
			if(IsSexSpecific){
				
				MapTokens[j].theta2    =strtok(NULL,delim);
				MapTokens[j].distance2 =strtok(NULL,delim);
				
			}
		}
	}
	
	/* We are now ready to print everything out, */
	/* starting of course with the header:       */
	pmap=fopen(fmap,"w");
	if(!pmap){
		/* Free dynamically-allocated resources */
		free(b);
		free(p);
		free(MapTokens);
		MadelineErrorReport("RecognizeCrimapFile(): Unable to open file \"%s\" for writing!",fmap);
	}
	
	if(IsSexSpecific){
		
		fprintf(pmap,"CHROMOSOME N\nORDINAL    N\nMARKERNAME C\nPOSITION_F N\nTHETA_F    N\nDISTANCE_F N\nPOSITION_M N\nTHETA_M    N\nDISTANCE_M N\n\n");
		
	}else{
		
		fprintf(pmap,"CHROMOSOME N\nORDINAL    N\nMARKERNAME C\nPOSITION   N\nTHETA      N\nDISTANCE   N\n\n");
		
	}
	
	for(j=0;j<MarkerCount;j++){
		
		if(IsSexSpecific){
			
			fprintf(pmap,"%2i %3i %-10.9s   %8.8s %8.8s %8.8s   %8.8s %8.8s %8.8s\n",
				chromosome,
				j+1,
				MapTokens[j].markername,
				MapTokens[j].position1,
				MapTokens[j].theta1 ? MapTokens[j].theta1 : mvi,
				MapTokens[j].distance1 ? MapTokens[j].distance1 : mvi,
				MapTokens[j].position2,
				MapTokens[j].theta2 ? MapTokens[j].theta2 : mvi,
				MapTokens[j].distance2 ? MapTokens[j].distance2 : mvi
			);
			
		}else{
			
			fprintf(pmap,"%2i %3i %-10.9s   %8.8s %8.8s %8.8s\n",
				chromosome,
				j+1,
				MapTokens[j].markername,
				MapTokens[j].position1,
				MapTokens[j].theta1 ? MapTokens[j].theta1 : mvi,
				MapTokens[j].distance1 ? MapTokens[j].distance1 : mvi
			);
			
		}
		
	}
	
	fclose(pmap);
	
	lprintf("%s created ...\n",fmap);
	/* Free dynamically-allocated resources */
	free(b);
	free(p);
	free(MapTokens);
	return 0;
	
}

/*                                                       */
/* NewMapTokensArray(): Allocate an array of MAPTOKENS   */
/* used for parsing a Crimap or similar genetic map file */
/*                                                       */
PMAPTOKENS NewMapTokensArray(int n){
	
	PMAPTOKENS p;
	int i;
	
	p=(PMAPTOKENS) malloc((unsigned)n*sizeof(MAPTOKENS));
	if(!p) MadelineErrorReport("NewMapTokensArray(): no memory to allocate array of MAPTOKENS");
	
	for(i=0;i<n;i++){
		
		p[i].number        =NULL;
		p[i].marshfieldname=NULL;
		p[i].markername    =NULL;
		p[i].position      =NULL;
		p[i].position1     =NULL;
		p[i].position2     =NULL;
		p[i].theta         =NULL;
		p[i].theta1        =NULL;
		p[i].theta2        =NULL;
		p[i].distance      =NULL;
		p[i].distance1     =NULL;
		p[i].distance2     =NULL;
		
	}
	
	return p;
	
}


/*                                                     */
/* RecognizeMarshfieldFile():                          */
/*                                                     */
/* -- This function is very similar to                 */
/*    RecognizeCrimapFile() above, but parses          */
/*    the HTML file from the Marshfield Clinic on-     */
/*    line "Build Your Own Map" server.  A single      */
/*    file contains both sex-specific and sex-averaged */
/*    maps for the markers.  However, recombination    */
/*    fractions are not provided: So Madeline calls    */
/*    KosambiToTheta() to add the recombination frac-  */
/*    tions into the resulting map file.               */
/*                                                     */
int RecognizeMarshfieldFile(char *fin,char *fmap){

	char *b;            /* file buffer                  */
	int lines;          /* number of lines	             */
	PLINEINFO p;        /* vector of line information   */
	
	char Key1[] = "Marker";
	char Key2[] = "D number";
	int  i,j,MarkerCount;
	char delim[] = " ";  /* space delimiter in Marshfield files */
	char mvi[]   = ".";  /* missing value indicator             */
	PMAPTOKENS MapTokens;
	char *chr_str;
	int chromosome=0;
	FILE *pmap; /* Madeline-formatted map output file */
	
	/*                       */
	/* Open and buffer file: */
	/*                       */
	b=OpenAndBufferFile(fin);
	/*                      */
	/* Count lines in file: */
	/*                      */
	lines=GetLineCount(b);
	/*                                               */
	/* Assign pointers to the start of each line and */
	/* also figure line length, marking empty lines: */
	/*                                               */
	p=GetLineInfo(b,lines);	
	
	/*                                                           */
	/* Search for the line that says "Comprehensive genetic map" */
	/* (Also grab the Chromosome number as opportune)            */
	for(i=0;i<lines;i++){
		
		/*                                          */
		/* Search for the Chromosome number:        */
		/* This always appears before the map data. */
		if(!chromosome && (chr_str=FindEnd("Chromosome ",p[i].b))){
			
			chromosome=atoi(chr_str);
			lprintf("This is a map of chromosome %i markers ...\n",chromosome);
			
		}
		
		/*                                                               */
		/* Search for the keys marking beginning of subsequent map data: */
		/*                                                               */
		if(strstr(p[i].b,Key1) && strstr(p[i].b,Key2)){
			break;
		}
	}
	
	/* If we read the entire file without finding the key string we want, it's */
	/* not in the right format:                                                */
	if(i>lines){
		free(b);
		free(p);
		MadelineErrorReport("RecognizeMarshfieldFile(): Unable to find the keys,\n\"%s\" and \"%s\".\nPerhaps the file is not a Marshfield map file or is corrupt.",
			Key1,
			Key2
		);
	}else{
		
		lprintf("Converting Marshfield map ...\n");
		
	}
	
	/*                                                                  */
	/* The next line after the keys line should be blank                */
	/* After that, each map record takes up two lines:                  */
	/*                                                                  */
	/* The first line contains ordinal, Marker, D Number (what we call  */
	/* "MARKERNAME"), sex-averaged cumulative distance, female-specific */
	/* cumulative distance, and male-specific cumulative distance.      */
	/*                                                                  */
	/* The second line contains the inter-marker distances for the sex- */
	/* averaged, female, and male maps respectively.                    */
	/*                                                                  */
	
	/* Here we will allow any number of blank lines before the data begin: */
	/* even though normally there will only be one blank line              */
	for(++i;i<lines;i++){
		if(p[i].b[0]) break;
	}
	
	/* Skip ahead looking for the next blank line after the map data block */
	/* in order to determine the number of markers in the map:             */
	for(j=i;j<lines;j++){
		if(!p[j].b[0]) break;
	}
	
	/* Recall that data for each marker spans 2 rows in the Marshfield file: */
	MarkerCount=(j+1-i)/2;
	
	/* Allocate the MAPTOKENS array: */
	MapTokens=NewMapTokensArray(MarkerCount);
	
	for(j=0;j<MarkerCount;j++,i++){
		
		/* First grab tokens from FIRST line of data (p[i].b): */
		MapTokens[j].number        =strtok(p[i].b,delim);
		MapTokens[j].marshfieldname=strtok(NULL,delim);
		MapTokens[j].markername    =strtok(NULL,delim);
		MapTokens[j].position      =strtok(NULL,delim);
		MapTokens[j].position1     =strtok(NULL,delim);
		MapTokens[j].position2     =strtok(NULL,delim);
		
		/* Next grab tokens from SECOND line of data (p[i+1].b):   */
		/* BUT there will be no second line of data if this is the */
		/* LAST marker:                                            */
		if(j<MarkerCount-1){
			
			MapTokens[j].distance  =strtok(p[++i].b,delim);
			MapTokens[j].distance1 =strtok(NULL,delim);
			MapTokens[j].distance2 =strtok(NULL,delim);
			
		}
	}
	
	/* We are now ready to print everything out, */
	/* starting of course with the header:       */
	pmap=fopen(fmap,"w");
	if(!pmap){
		/* Free dynamically-allocated resources */
		free(b);
		free(p);
		free(MapTokens);
		MadelineErrorReport("RecognizeCrimapFile(): Unable to open file \"%s\" for writing!",fmap);
	}
	
	/* Print the header: */
	fprintf(pmap,"CHROMOSOME N  ORDINAL    N  MARKERNAME C\nPOSITION   N  THETA      N  DISTANCE   N\nPOSITION_F N  THETA_F    N  DISTANCE_F N\nPOSITION_M N  THETA_M    N  DISTANCE_M N\n\n");
	
	/* Print all but the last line in the loop ... */
	for(j=0;j<MarkerCount-1;j++){
		
		fprintf(pmap,"%2i %3i %-10.9s   %8.8s %7.5f %8.8s   %8.8s %7.5f %8.8s   %8.8s %7.5f %8.8s \n",
			chromosome,
			j+1,
			MapTokens[j].markername,
			MapTokens[j].position,
			Kosambi_cM_To_Theta(atof(MapTokens[j].distance)),
			MapTokens[j].distance,
			MapTokens[j].position1,
			Kosambi_cM_To_Theta(atof(MapTokens[j].distance1)),
			MapTokens[j].distance1,
			MapTokens[j].position2,
			Kosambi_cM_To_Theta(atof(MapTokens[j].distance2)),
			MapTokens[j].distance2
		);
		
	}
	/* ... Now print the last line : */
	fprintf(pmap,"%2i %3i %-10.9s   %8.8s %7.7s %8.8s   %8.8s %7.7s %8.8s   %8.8s %7.7s %8.8s \n",
		chromosome,
		j+1,
		MapTokens[j].markername,
		MapTokens[j].position,
		mvi,
		mvi,
		MapTokens[j].position1,
		mvi,
		mvi,
		MapTokens[j].position2,
		mvi,
		mvi
	);
	
	fclose(pmap);
	
	lprintf("%s created ...\n",fmap);
	/* Free dynamically-allocated resources */
	free(b);
	free(p);
	free(MapTokens);
	return 0;
	
}

/*                                                               */
/* This function is, again, quite similar in basic structure     */
/* to the RecognizeCrimapFile() and RecognizeMarshfieldFile()    */
/* functions, but obviously customized to extract the results    */
/* table from Simwalk Parametric and NPL output for graphing in  */
/* Madeline.                                                     */
/*                                                               */
int RecognizeSimwalkResultFile(char *fin,char *fout){
	
	char *b;
	PLINEINFO p;
	char FirstKey[]      = "Marker";         /* in first line  */
	char SecondKey[]     = "Position";       /* in first line  */
	char ThirdKey[]      = "Name";           /* in second line */
	char ParametricKey[] = "Location Score"; /* in first line  */
	char NPLKey[]        = "STAT A";         /* in second line */
	int IsNPL=-1; /* True (=1) if is an NPL result file as opposed to a parametric result file */
	int i,success;
	int lines;
	int chromosome=0;
	char *chr;
	char *s;
	
	/*                       */
	/* Open and buffer file: */
	/*                       */
	b=OpenAndBufferFile(fin);
	/*                      */
	/* Count lines in file: */
	/*                      */
	lines=GetLineCount(b);
	/*                                               */
	/* Assign pointers to the start of each line and */
	/* also figure line length, marking empty lines: */
	/*                                               */
	p=GetLineInfo(b,lines);	
	
	/*                                                           */
	/* Search for the line that says "Comprehensive genetic map" */
	/* (Also grab the Chromosome number as opportune)            */
	for(i=0;i<lines;i++){
		
		/* Look for the word "[c|C]hromosome" and then read the number following therafter: */
		if(!chromosome && (chr=FindEnd("hromosome",p[i].b)) ){
			/* skip any white space: */
			for(;*chr && *chr==' ' || *chr=='\t';chr++);
			/* Now hopefully at beginning of digits: find the end: */
			for(s=chr;isdigit(*s);s++);
			/* terminate the string: */
			*s='\0';
			/* get the chromosome number: */
			chromosome=atoi(chr);
		}
		
		/* Find keys in file: */
		if(strstr(p[i].b,FirstKey) && strstr(p[i].b,SecondKey) && strstr(p[i+1].b,ThirdKey)){
			/* Now we are at the start of the data block */
			/* Is this an NPL or Parametric analysis?    */
			if(strstr(p[i].b,ParametricKey)) IsNPL=0;
			else if(strstr(p[i+1].b,NPLKey)) IsNPL=1;
			else{
				
				free(p);
				free(b);
				MadelineErrorReport("RecognizeSimwalkResultFile(): Unable to determine if these results are \nfrom a Simwalk parametric or non-parametric analysis.");
			}
			break;
		}
	
	}
	
	/* i is now at the start of the data block ...                                         */
	/* Call function specialized for reading the parametric or non-parametric result file: */
	
	if(IsNPL){
		lprintf("This appears to be an NPL analysis of chromosome %i ...\n",chromosome);
		success=RecognizeSimwalkNPLFile(p,i,lines,fout,chromosome);
	}else{
		lprintf("This appears to be a parametric analysis of chromosome %i ...\n",chromosome);
		success=RecognizeSimwalkParametricFile(p,i,lines,fout,chromosome);
	}
	/* Free resources: */
	free(b);
	free(p);
	/* Check for errors: */
	if(!success){
		MadelineErrorReport("RecognizeSimwalkResultFile(): An error occurred while attempting to recognize the file.");
	};
	
	return 1;
	
}


/*                                  */
/* RecognizeSimwalkParametricFile() */
/*                                  */
int RecognizeSimwalkParametricFile(PLINEINFO p,int startline,int lines,char *fout,int chromosome){
	
	int i,j;
	FILE *pout;  /* plot data file                            */
	FILE *pmap;  /* complementary haldane cM genetic map file */
	char delim[] ="\t";
	char *position;
	char *score;
	
	/* Open output file and write Madeline file header: */
	pout=fopen(fout,"w");
	pmap=fopen(ModifyFileName(fout,MAP_FILE_EXTENSION),"w");
	/* Error reporting is handled by the caller: */
	if(!pout) return 0;
	if(!pmap) return 0;
	/* Write header of the graph plot data file: */
	fprintf(pout,"POSITION N\nSCORE    N\n\n");
	/* Write header of the map data file: */
	fprintf(pmap,"CHROMOSOME N\nORDINAL    N\nMARKERNAME C\nPOSITION   N\n\n");
	
	/* The data table starts at startline+3 in a Simwalk 2.83 parametric results file: */
	for(i=startline+3,j=0;i<lines;i++){
		
		/* We've reached the end when we hit a blank line (actually, a single space): */
		if(!p[i].b[0] || p[i].b[0]==' '){
			break;
		}
		
		/* if a line starts with a letter or number instead of a tab,    */
		/* then it is the name of a marker, which we strip out.  We then */
		/* need to skip this line __and__ the next line after it:        */
		if(isalpha(p[i].b[0]) || isdigit(p[i].b[0])){
			/* This line contains only the name of the marker and nothing else:    */
			/* Note that "position" still points to the position from the previous */
			/* line and of course this is the map position of the marker in        */
			/* Haldane centiMorgans:                                               */
			++j;
			fprintf(pmap,"%2i %3i %s %s\n",chromosome,j,p[i].b,position);
			/* Here's where we skip the next line which is a duplicate of the      */
			/* previous line:                                                      */
			++i;
			continue;
		}
		
		/* Read in just position and score: */
		position = strtok(p[i].b,delim);
		score    = strtok(NULL,delim);
		
		/* Write them out: */
		fprintf(pout,"%s  %s\n",position,score);
		
	}
	
	/* Get here if we've read all the data: */
	fclose(pout);
	fclose(pmap);
	lprintf("Graphing files \"%s\" and \"%s%s\" created ...\n",fout,fout,MAP_FILE_EXTENSION);
	
	/* return the current line position within the results file */
	/* so that we can continue reading:                         */
	return i;
	
}

/*                                  */
/* RecognizeSimwalkNPLFile()        */
/*                                  */
int RecognizeSimwalkNPLFile(PLINEINFO p,int startline,int lines,char *fout,int chromosome){
	
	int i,j;
	FILE *pout;  /* plot data file                            */
	FILE *pmap;  /* complementary haldane cM genetic map file */
	char delim[] ="\t";
	char FirstKey[]      = "Name";             /* in second line  */
	char SecondKey[]     = "Haldane cM";       /* in second line  */
	char ThirdKey[]      = "STAT A";           /* in second line  */

	char *markername,*position;
	char *stata,*statb,*statc,*statd,*state;
	
	/* Open output file and write Madeline file header: */
	pout=fopen(fout,"w");
	pmap=fopen(ModifyFileName(fout,MAP_FILE_EXTENSION),"w");
	/* Error reporting is handled by the caller: */
	if(!pout) return 0;
	if(!pmap) return 0;
	/* Write header of the graph plot data file: */
	fprintf(pout,"MARKERNAME C\nPOSITION N\nSTAT_A N\nSTAT_B N\nSTAT_C N\nSTAT_D N\nSTAT_E N\n\n");
	/* Write header of the map data file: */
	fprintf(pmap,"CHROMOSOME N\nORDINAL    N\nMARKERNAME C\nPOSITION   N\n\n");
	
	/*                                                                                  */
	/* For an NPL file, we need to continue looking for the lines that looks like this: */
	/*    Marker 	   Position 	    -LOG10(p-values)                                */
	/*     Name  	(Haldane cM)	 STAT A	 STAT B	 STAT C	 STAT D	 STAT E	            */
	/*                                                                                  */
	for(i=startline;i<lines;i++){
		
		
		/* Find keys in file: */
		if(strstr(p[i].b,FirstKey) && strstr(p[i].b,SecondKey) && strstr(p[i].b,ThirdKey)){
			/* Now we are on the header line of the tab-delimited */
			/* data block provided for graphing. The next line    */
			/* is the start of the data:                          */
			startline=i+1;
			break;
		}
	
	}
	
	for(i=startline,j=1;i<lines;i++,j++){
		
		/* We've reached the end when we hit a blank line (actually, a single space): */
		if(IsEmpty(p[i].b)){
			break;
		}
		
		/* Read in just position and score: */
		markername = strtok(p[i].b,delim);
		position   = strtok(NULL,delim);
		stata      = strtok(NULL,delim);
		statb      = strtok(NULL,delim);
		statc      = strtok(NULL,delim);
		statd      = strtok(NULL,delim);
		state      = strtok(NULL,delim);
		
		/* Write them out: */
		fprintf(pout,"%s  %s  %s  %s  %s  %s  %s\n",markername,position,stata,statb,statc,statd,state);
		fprintf(pmap,"%2i %3i  %s  %s\n",chromosome,j,markername,position);
	}
	
	/* Get here if we've read all the data: */
	fclose(pout);
	fclose(pmap);
	lprintf("Graphing files \"%s\" and \"%s%s\" created ...\n",fout,fout,MAP_FILE_EXTENSION);
	
	/* return the current line position within the results file */
	/* so that we can continue reading:                         */
	return i;
	
}


/*                                                                     */
/* RecognizeWeberLabFile(): Recognizes a Marshfield Clinic Weber Lab   */
/*                          genotype data file format                  */
/*                          and converts it to a Madeline flat table   */
/*                          format (first option) or decomposes it     */
/*                          for importation into a database (2nd opt.) */               
int RecognizeWeberLabFile(const char *fin,const char *fout, int decompose){
	
	extern PCONTROL g;
	
	char *b;            
	int i,j,m;
	int lines,l,maxlength;          
	int chromosome=0; 
	PLINEINFO p;        
	char *s, *chr;
	char **markername;
	char *delim="\t*$&";
	FILE *pout;
	
	int fam,iid,dad,mom,sex,a1,a2,swapa;
	double confidence;
	char genotype[10];
	
	/*                       */
	/* Open and buffer file: */
	/*                       */
	b=OpenAndBufferFile(fin);
	/*                      */
	/* Count lines in file: */
	/*                      */
	lines=GetLineCount(b);
	/*                                               */
	/* Assign pointers to the start of each line and */
	/* also figure line length, marking empty lines: */
	/*                                               */
	p=GetLineInfo(b,lines);	
	
	/*                                                           */
	/* The first line should start with the word "Chromosome"    */
	/*                                                           */
	for(i=0;i<lines;i++){
		/* Look for the word "[c|C]hromosome" and then read the number following therafter: */
		if(!chromosome && (chr=FindEnd("hromosome",p[i].b)) ){
			/* skip any white space: */
			for(;*chr && *chr==' ' || *chr=='\t';chr++);
			/* Now hopefully at beginning of digits: find the end: */
			for(s=chr;isdigit(*s);s++);
			/* terminate the string: */
			*s='\0';
			/* get the chromosome number: */
			chromosome=atoi(chr);
			break;
		}
	}
	/*                                                                                     */
	/* The header line containing the names of the markers should begin at the i+2 line:   */
	/* scan this header line to determine how many marker names (i.e., markers) there are: */
	/*                                                                                     */
	i+=2;
	for(m=j=0;j<p[i].ln;j++){
		/* scan until we find a label: then increment the marker counter */
		/* and skip to the end of that label:                            */
		if(!isspace(p[i].b[j])){
			m++;
			while(j<p[i].ln && !isspace(p[i].b[j])) j++;
		}
	}
	lprintf("This file appears to contain genotype data for %i markers on chromosome %i\n",m,chromosome);
	/*                                                             */
	/* allocate an array of pointers to point to the marker names: */
	/* "m" is the number of markers present:                       */
	/*                                                             */
	markername=(char **)malloc((unsigned)sizeof(char *)*m);
	if(!markername){
		free(b); free(p);
		MadelineErrorReport("RecognizeWeberLabFile(): No memory available to allocate array of pointers to marker names");
		return 0;
	}
	/* Place the markername (pointers) into the markername array: */
	/* and keep a running tab on the longest markername:          */
	s=strtok(p[i].b,delim);
	for(maxlength=j=0;j<m;j++){
		markername[j]=s;
		l=strlen(markername[j]);
		if(l>maxlength) maxlength=l;
		s=strtok(NULL,delim);
	}
	
	if(decompose){
		
		/* ********************************** */
		/* CREATE DECOMPOSED FLAT FILE TABLE: */
		/* ********************************** */
		
		/*                                                      */
		/* Open output file and write out the Madeline header:  */
		/*                                                      */
		/* Recall that this is a DECOMPOSED TABLE:              */
		/*                                                      */
		pout=fopen(fout,"w");
		if(!pout){
			free(b); free(p); free(markername);
			MadelineErrorReport("RecognizeWeberLabFile(): Unable to open \"%s\" for writing",fout);
			return 0;
		}
		fprintf(pout,"FAMID      C\nSTUDYID    C\nMOTHER     C\nFATHER     C\nSEX        X\nMARKERNAME C\nALLELE1    N\nALLELE2    N\n");
		fprintf(pout,"CONFIDENCE N\nINDICATOR  C\n\n");
		
		/*                                         */
		/* iterate through lines in original file: */
		/*                                         */
		for(i+=2;i<lines;i++){
			
			/* grab the core data: */
			fam  = atoi(strtok(p[i].b,delim));
			iid  = atoi(strtok(NULL,delim));
			dad  = atoi(strtok(NULL,delim));
			mom  = atoi(strtok(NULL,delim));
			sex  = atoi(strtok(NULL,delim));
			
			/*                          */
			/* iterate through markers: */
			/*                          */
			for(j=0;j<m;j++){
				/* print it out to the output file: */
				fprintf(pout,"%04i%c%05i%c",fam,g->delimiter,iid,g->delimiter);
				/*                                                     */
				/* print mom first in order to be consistent with both */
				/* Cicada and Madeline standards:                      */
				/*                                                     */
				if(!mom) fprintf(pout,"    .%c",g->delimiter);
				else     fprintf(pout,"%05i%c",mom,g->delimiter);
				if(!dad) fprintf(pout,"    .%c",g->delimiter);
				else     fprintf(pout,"%05i%c",dad,g->delimiter);
				/* print gender using F/M/. Madeline conventions: */
				fprintf(pout,"%c   %c",(sex==2?'F':sex==1?'M':'.'),g->delimiter);
				
				/* print the marker name: */
				fprintf(pout,"%-*.*s  %c",maxlength,maxlength,markername[j],g->delimiter);
				
				/* grab allele1 and allele2: */
				a1         = atoi(strtok(NULL,delim));
				a2         = atoi(strtok(NULL,delim));
				/*                                                   */
				/* Although the confidence column is always present, */
				/* there is no missing value indicator when the      */
				/* indicator column is missing.  To get around this  */
				/* nuisance, we simply include the possible          */
				/* indicator characters, "*,&,$", as part of our     */
				/* delimiter, "delim" string:                        */
				confidence = atof(strtok(NULL,delim));
				
				/* print out the genotype field: */
				/* For some reason, the Weber file format */
				/* seems to print the larger allele first */
				/* followed by the smaller.  Hence, the   */
				/* extra code here to always print the    */
				/* smaller first:                         */
				
				if(a2<a1){ 
					swapa=a1; 
					a1=a2; 
					a2=swapa; 
				}
				fprintf(pout,"%4i%c%4i%c%4.2f\n",a1,g->delimiter,a2,g->delimiter,confidence);
				
			}
		}
		
		
		/* close output file: */
		fclose (pout);

	}else{
		
		/* ******************************************* */
		/* CREATE MADELINE FLAT FILE (COMPOSED) TABLE: */
		/* ******************************************* */
		
		/*                                                      */
		/* Open output file and write out the Madeline header:  */
		/* Note that we are going to write out genotype columns */
		/* instead of the Weber file's allele columns, and we   */
		/* are going to create unique names for the confidence  */
		/* and indicator columns:                               */
		/* (Actually, for now we are going to skip the quality  */
		/* columns)                                             */
		pout=fopen(fout,"w");
		if(!pout){
			free(b); free(p); free(markername);
			MadelineErrorReport("RecognizeWeberLabFile(): Unable to open \"%s\" for writing",fout);
			return 0;
		}
		/* Print core fields (MZ / DZ Twin apparently not present): */
		fprintf(pout,"FAMID   C\nSTUDYID C\nMOTHER  C\nFATHER  C\nSEX     X\n");
		/* Print the genotype field names: */
		for(j=0;j<m;j++) fprintf(pout,"%-*.*s G%c",maxlength,maxlength,markername[j],(j+1)%5==0?'\n':' ');
		/* Print blank lines before starting with the data rectangle: */
		fprintf(pout,"\n\n");
		
		/* Now back to parse out the data:                          */
		/* Skip one line after the line containing the marker names */
		/* to get to the first line of data:                        */
		
		for(i+=2;i<lines;i++){
			
			/* grab the core data: */
			fam  = atoi(strtok(p[i].b,delim));
			iid  = atoi(strtok(NULL,delim));
			dad  = atoi(strtok(NULL,delim));
			mom  = atoi(strtok(NULL,delim));
			sex  = atoi(strtok(NULL,delim));
			
			/* print it out to the output file: */
			fprintf(pout,"%04i %05i ",fam,iid);
			/*                                                     */
			/* print mom first in order to be consistent with both */
			/* Cicada and Madeline standards:                      */
			/*                                                     */
			if(!mom) fprintf(pout,"    . ");
			else     fprintf(pout,"%05i ",mom);
			if(!dad) fprintf(pout,"    . ");
			else     fprintf(pout,"%05i ",dad);
			/* print gender using F/M/. Madeline conventions: */
			fprintf(pout,"%c   ",(sex==2?'F':sex==1?'M':'.'));
			
			/* grab and print the alleles */
			for(j=0;j<m;j++){
				
				/* grab allele1 and allele2: */
				a1         = atoi(strtok(NULL,delim));
				a2         = atoi(strtok(NULL,delim));
				/*                                                   */
				/* Although the confidence column is always present, */
				/* there is no missing value indicator when the      */
				/* indicator column is missing.  To get around this  */
				/* nuisance, we simply include the possible          */
				/* indicator characters, "*,&,$", as part of our     */
				/* delimiter, "delim" string:                        */
				confidence = atof(strtok(NULL,delim));
				
				/* print out the genotype field: */
				/* For some reason, the Weber file format */
				/* seems to print the larger allele first */
				/* followed by the smaller.  Hence, the   */
				/* extra code here to always print the    */
				/* smaller first:                         */
				
				if(a2<a1){ 
					swapa=a1; 
					a1=a2; 
					a2=swapa; 
				}
				sprintf(genotype,"%i/%i",a1,a2);
				fprintf(pout,"%9.9s ",genotype);
				
			}
			
			fprintf(pout,"\n");
		}
		
		/* close output file: */
		fclose (pout);
		
	}
	
	return 1;
	
}

/*                  */
/* SquareDataGrid() */
/*                  */
void SquareDataGrid(char *fin,char *fout){

	PLINEINFO p;               /* pointer to lines */
	BLOCKINDICES BlockIndices; /* header and data block start and end indices */
	int lines;                 /* no. of lines     */
	char *bf;                  /* file buffer      */
	
	int i;
	
	FILE *fp;
	
	/*                           */
	/* Open and buffer the file: */
	/*                           */
	bf=OpenAndBufferFile(fin);
	/*                      */
	/* Count lines in file: */
	/*                      */
	lines=GetLineCount(bf);
	/*                                               */
	/* Assign pointers to the start of each line and */
	/* also figure line length, marking empty lines: */
	/*                                               */
	p=GetLineInfo(bf,lines);
	
	/*                                            */
	/* PHASE 1: Determine HEADER and DATA blocks: */
	/*                                            */
	//DetermineHeaderAndDataBlocks(&LineIndices,p,lines);
	lprintf("This file has %i blocks\n",DetermineNumberOfBlocks(p,lines));
	return;
	
	/*                               */
	/* PHASE 2: Write padded output: */
	/*                               */
	
	/* Open output file for writing: */
	fp=fopen(fout,"w");
	if(!fp) MadelineErrorReport("SquareDataGrid(): Unable to open \"%s\" for writing.",fout);
	/* write HEADER unchanged: */
	for(i=BlockIndices.headerStart;i<=BlockIndices.headerEnd;i++) fprintf(fp,"%s\n",p[i].b);
	/* put a blank line between the header and the data whether it was there before or not: */
	fprintf(fp,"\n");
	/* write padded DATA block: */
	for(i=BlockIndices.dataStart;i<BlockIndices.dataEnd;i++) fprintf(fp,"%-*.*s\n",BlockIndices.maxDataLineLength,BlockIndices.maxDataLineLength,p[i].b);
	/* close file: */
	fclose(fp);
	lprintf("Squared file \"%s\" written.\n",fout);
	
	free(bf);
	free(p);
	
}

/*                         */
/* DetermineNumberOfBlocks */
/*                         */
int DetermineNumberOfBlocks(const PLINEINFO p,int lineCount){ 
	
	int i;
	int blocks=0;
	
	/*                                             */
	/* First count only how many blocks there are: */
	/*                                             */
	
	for(i=0,blocks=0;i<lineCount;){
		
		/* skip empty lines at top:            */
		for(;i<lineCount && p[i].em;i++);
		/* return if no more lines to process: */
		if(i==lineCount) return blocks;
		/* otherwise, increment block counter: */
		blocks++;
		/* now skip non-empty lines representing the block: */
		for(;i<lineCount && !p[i].em;i++);
		
	}
	
	return blocks;
	
}


int HasFieldName(PPARSEDFIELDINFO f,int fc,const char *nm){
	
	int i;
	
	for(i=0;i<fc;i++) if (!strcmp(f[i].nm,nm)) return 1;
	return 0;
	
}

enum TableType GuessTableType(PPARSEDFIELDINFO f,int fc){
	
	extern VARIABLE gfnID;
	extern VARIABLE gfnSex;
	extern VARIABLE gfnMapChromosome;
	extern VARIABLE gfnMapPosition;
	extern VARIABLE gfnAllele1;
	extern VARIABLE gfnAllele2;
	extern VARIABLE gfnAlleleField;
	extern VARIABLE gfnFrequencyField;
	extern VARIABLE gfnGraphPosition;
	extern VARIABLE gfnGraphScore   ;
	
	if( HasFieldName(f,fc,gfnAlleleField.e.s)   && HasFieldName(f,fc,gfnFrequencyField.e.s) ) return ALLELE_FREQUENCY_TABLE;
	if( HasFieldName(f,fc,gfnGraphPosition.e.s) && HasFieldName(f,fc,gfnGraphScore.e.s)     ) return ANALYSIS_RESULTS_TABLE;
	if( HasFieldName(f,fc,gfnMapPosition.e.s)   && HasFieldName(f,fc,gfnMapChromosome.e.s)  ) return MAP_TABLE;
	if( HasFieldName(f,fc,gfnAllele1.e.s)       && HasFieldName(f,fc,gfnAllele2.e.s)        ) return DECOMPOSED_TABLE;
	if( HasFieldName(f,fc,gfnID.e.s)            && HasFieldName(f,fc,gfnSex.e.s)            ) return PEDIGREE_TABLE;
	
	return UNKNOWN_TABLE;
	
}

void ReportTableType(enum TableType TypeOfTable,const char *nm){
	
	switch(TypeOfTable){
	case UNKNOWN_TABLE:
		lprintf("Commands for opening tables include:\n\nPEDIGREE TABLES:\topen \"%1$s\"\nMAP TABLES:\tload \"%1$s\"\nDECOMPOSED TABLES:\tcompose \"%1$s\"\nANALYSIS RESULTS:\tgraph open \"%1$s\"\nALLELE FREQUENCIES:\tread \"%1$s\"\n",nm);
		break;
	case PEDIGREE_TABLE:
		lprintf("This appears to be a PEDIGREE TABLE which can be opened using:\n\n\topen \"%s\"\n\n",nm);
		break;
	case MAP_TABLE:
		lprintf("This appears to be a MAP TABLE which can be opened using:\n\n\tload \"%s\"\n\n",nm);
		break;
	case DECOMPOSED_TABLE:
		lprintf("This appears to be a DECOMPOSED TABLE which can be converted using:\n\n\tcompose \"%s\"\n\n",nm);
		break;
	case ANALYSIS_RESULTS_TABLE:
		lprintf("This appears to be an ANALYSIS RESULTS TABLE which can be opened using:\n\n\tgraph open \"%s\"\n\n",nm);
		break;
	case ALLELE_FREQUENCY_TABLE:
		lprintf("This appears to be an ALLELE FREQUENCY TABLE which can be opened using:\n\tread allele frequencies from \"%s\"\n\n",nm);
		break;
	}
	
}

/*/////////////// */
/*                */
/* end recogniz.c */
/*                */
/*/////////////// */


