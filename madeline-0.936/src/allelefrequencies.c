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
/* "allelefrequencies.c" is part of the Madeline software distribution.              */
/* Written by Edward H. Trager.                                                      */
/*                                                                                   */
/* COPYRIGHT (C) 2003 THE REGENTS OF THE UNIVERSITY OF MICHIGAN,                     */
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
/* Last modified by Ed Trager, May 2003.                                             */
/*                                                                                   */
/* ********************************************************************************* */
/* 2003.05.15.ET */

#include "allelefrequencies.h"
#include "logs.h"
#include "stringbuffersizes.h"
#include "result.h"
#include "token.h"
#include "knowntokens.h"

//#ifndef RECOGNIZE_INCLUDED
#include "recognize.h"
//#endif

// For SmartTableSelect():
#include "smartselect.h"

/*                               */
/* CalculateAlleleFrequencies(): */
/*                               */
/* Uses simple gene counting:    */
void CalculateAlleleFrequencies(PDBF db);
/*                        */
/* ReuseSavedMarkerVector */
/*                        */
int ReuseSavedMarkerVector(PCONTROL c);
/*                                                              */
/*                                                              */
/* GetMarkerInformation() processes the ith field of a database */
/* ,if that field is a marker field, to obtain allele           */
/* frequency distribution.                                      */
/*                                                              */
/*                                                              */
/*                                                              */
void GetMarkerInformation(PDBF db,int idx);


/*                               */
/* CalculateAlleleFrequencies(): */
/*                               */
/* Uses simple gene counting:    */
void CalculateAlleleFrequencies(PDBF db){

	int i,j;
	
	
	/*                                          */
	/* if there are no genotype fields, return: */
	/*                                          */
	if(!db->n.g) return;
		
	/*                                                        */
	/* Remove previous marker allele frequencies, if present: */
	/*                                                        */
	if(db->m){
		lprintf("Removing old marker allele frequencies...\n");
		FreeMarkerVector(db);
	}
		
	/*                                     */
	/* allocate marker vector in database: */
	/*	                                    */
	db->m=NewMarker(db->fc);
	
	/*                                                    */
	/* Now calculate allele frequencies for each marker:  */
	/* everybody with typed markers for a given allele is */
	/* currently included in the calculation:             */
	/*                                                    */
	for(j=i=0;j<db->n.g && i<db->fc;i++){
		if(db->ff[i]&GENOTYPESET){
			dprintf("Calculating allele frequencies for %3i. %s...\n",i+1,db->FieldName(db,i));
			GetMarkerInformation(db,i);
			j++;
		}
	}	
}


/*                        */
/* ReuseSavedMarkerVector */
/*                        */
int ReuseSavedMarkerVector(PCONTROL c){
	
	int i,j;
	char *fn;
	
	/*                                           */
	/* In the event that there is no             */
	/* saved marker vector present, just return: */
	/*                                           */
	if(!c->m){
		return 0;
	}
	/*                                                       */
	/* In the event that SaveMarkerInfo has been turned off  */
	/* but a marker vector is still lingering about, then we */
	/* need to remove it:                                    */
	/*                                                       */
	if(!c->SaveMarkerInfo && c->m){
		lprintf("Removing previously saved marker allele frequencies...\n");
		FreeSavedMarkerVector(c);
		return 0;
	}
	/*                                                         */
	/* In the event that SaveMarkerInfo has been turned on AND */
	/* there is a marker vector that can be reused:            */
	/*                                                         */
	if(c->SaveMarkerInfo && c->m){
		/*                                            */
		/* check that the marker vector is, at least, */
		/* compatable with the new database:          */
		/*                                            */

		/*if(c->ms!=c->db->fc){                                                                                                                                               */
		/*	lprintf("ReuseSavedMarkerVector(): current and previous table structures do not match exactly:\n\tremoving previously saved marker allele frequencies...\n"); */
		/*	c->SaveMarkerInfo=0;                                                                                                                                          */
		/*	FreeSavedMarkerVector(c);                                                                                                                                     */
		/*	return 0;                                                                                                                                                     */
		/*}                                                                                                                                                                   */
		
		/*                                                              */
		/* Check for match among all currently present genotype fields: */
		/*                                                              */
		for(j=i=0;j<c->db->n.g && i<c->db->fc;i++){
			
			/*if( (c->db->ff[i]&GENOTYPESET && !c->m[i].a) || */
			/*    (!c->db->ff[i]&GENOTYPESET && c->m[i].a)    */
			/*  ){                                            */
			
			if(c->db->ff[i]&GENOTYPESET){
				/*                                       */
				/* Check for mismatch against saved set: */
				/*                                       */
				if(!c->m[i].a ||
				   strcmp(c->m[i].nm,fn=c->db->FieldName(c->db,i))
				){
					if(!c->m[i].a)
						lprintf("ReuseSavedMarkerVector(): field %i was not a\ngenotype field in the previously opened pedigree table\n",i);
					else
						lprintf("ReuseSavedMarkerVector(): field %i represented \"%s\", not \"%s\" in previously opened pedigree table\n",
							i,
							c->m[i].nm,
							fn);
					lprintf("Now removing previously saved allele frequencies...\n");
					c->SaveMarkerInfo=0;
					FreeSavedMarkerVector(c);
					return 0;
				}
				
				j++;
			}
		}
		/*                                 */
		/* Get here if they match exactly: */
		/*                                 */
		c->db->m=c->m;
		lprintf("Reusing previously calculated allele frequencies...\n");
		return 1;
	}
	/*                              */
	/* shouldn't actually get here: */
	/*                              */
	return 0;
}


/*                                                              */
/*                                                              */
/* GetMarkerInformation() processes the ith field of a database */
/* ,if that field is a marker field, to obtain allele           */
/* frequency distribution.                                      */
/*                                                              */
/*                                                              */
/*                                                              */
void GetMarkerInformation(PDBF db,int idx){

	UINT32 i;
	char genotype[14];
	char *o;            /* points to genotype      */
	char *allele1;      /* points to first allele  */
	char *allele2;      /* points to second allele */
	int iallele1,iallele2; /* alleles in integer format */

	/*                               */
	/* Only process genotype fields: */
	/*                               */
	if(!(db->ff[idx]&GENOTYPESET)) return;
	
	/*                                                         */
	/* store the marker name in case of saving allele vectors: */
	/*                                                         */
	strcpy(db->m[idx].nm,db->FieldName(db,idx));
	
	/*                           */
	/* loop through all records: */
	/*                           */
	for(i=0;i<db->rc;i++){

		/*                           */
		/* get individual's genotype */
		/*                           */
		o=db->ReadString(db,genotype,idx,i);
		
		/*                                           */
		/* Skip missing values:                      */
		/*                                           */
		if(!o[0]) continue;
				
		/*                                     */
		/* get allele1 and allele2 substrings: */
		/*                                     */
		for(allele1=allele2=o;*allele2 && *allele2!='/';allele2++);
		*allele2++='\0';

		/*                            */
		/* Insert alleles into tree:  */
		/*                            */
		/* 2002.04.16.ET: But ONLY if */
		/* the allele is non-zero     */
		/*                            */
		iallele1=atoi(allele1);
		iallele2=atoi(allele2);
		if(iallele1) db->m[idx].a=InsertAllele(db->m[idx].a,iallele1);
		if(iallele2) db->m[idx].a=InsertAllele(db->m[idx].a,iallele2);
		
	}
	
	/*                                                 */
	/* Now assign ranks and calculate the overall sum: */
	/*                                                 */
	SetAlleleRanksAndSumCounts(db->m+idx,db->m[idx].a);
}

/*                                                               */
/* FreeSavedMarkerVector(db): Frees the c->m vector, if present: */
/*                                                               */
void FreeSavedMarkerVector(PCONTROL c){

	int i;
	
	if(!c->m) return;
	/*                                             */
	/* free all of the allocated allele BST trees: */
	/*                                             */
	for(i=0;i<c->ms;i++)
		if(c->m[i].a) FreeAllele(c->m[i].a);
	/*                              */
	/* free the c->m vector itself: */
	/*                              */
	free(c->m);
	c->m=NULL;
	c->ms=0;
}


/*                                                              */
/*                                                              */
/* LoadMarkerAlleleFrequencyInformation():                      */
/*                                                              */
/* Loads allele frequency information from a table of marker    */
/* allele frequencies:                                          */
/*                                                              */
/* 2003.05.15.ET ADDENDUM                                       */
/*                                                              */
void LoadMarkerAlleleFrequencyInformation(PDBF PedigreeDB,PDBF AlleleFrequencyDB,UINT32 MarkerNameFieldIndex, UINT32 AlleleFieldIndex, UINT32 FrequencyFieldIndex){
	
	UINT32 i;
	char buffer[MAXMARKERNAMEWIDTH];
	const char *MarkerName;
	int AlleleValue;
	double AlleleFrequency;
	int MarkerFieldIndex;
	
	/*                               */
	/* loop through all the records  */
	/* in the AlleleFrequency table: */
	/*                               */
	for(i=0;i<AlleleFrequencyDB->rc;i++){
		//
		// Get the marker name, allele, and allele frequency:
		//
		MarkerName      = AlleleFrequencyDB->ReadString(AlleleFrequencyDB,buffer,MarkerNameFieldIndex,i);
		AlleleValue     = AlleleFrequencyDB->ReadInt32(AlleleFrequencyDB,AlleleFieldIndex,i);
		AlleleFrequency = AlleleFrequencyDB->ReadDouble(AlleleFrequencyDB,FrequencyFieldIndex,i);
		//
		// Find the matching marker field in the Pedigree table:
		//
		MarkerFieldIndex= PedigreeDB->FieldNumber(PedigreeDB,MarkerName);
		if(MarkerFieldIndex==MISSING){
			
			eprintf(WARNINGFLAG,"Marker \"%s\" was not found in the pedigree table.\n",MarkerName);
			
		}else{
			//
			// Store the marker name, if not yet present:
			//
			if(!PedigreeDB->m[MarkerFieldIndex].nm[0])
				strcpy(PedigreeDB->m[MarkerFieldIndex].nm,MarkerName);
			/*                           */
			/* Insert allele into tree:  */
			/*                           */
			PedigreeDB->m[MarkerFieldIndex].a=InsertAlleleAndFrequency(PedigreeDB->m[MarkerFieldIndex].a,AlleleValue,AlleleFrequency);
			
		}
	}
	
	//
	// Finally, loop through the genotype fields in the pedigree database to recalculate the ranks and sums:
	//
	for(i=0;i<PedigreeDB->fc;i++){
		
		if( !(PedigreeDB->ff[i] & GENOTYPESET)) continue;
		ZeroMarkerRanksAndSumCounts(PedigreeDB->m+i);
		SetAlleleRanksAndSumCounts(PedigreeDB->m+i,PedigreeDB->m[i].a);
		
	}
	
}


/*                                                */
/*                                                */
/* Read(): read allele frequencies from a table.  */
/*                                                */
/*                                                */
void Read(void){

	RESULT result;
	const char *TableName;
	
	extern PCONTROL g;
	extern int GetToken(void);
	extern PTOKENINFO pT;
	extern void EvaluateExpression(PRESULT result);
	
	int TableType;
	extern VARIABLE gfnMapMarker;
	extern VARIABLE gfnAlleleField;
	extern VARIABLE gfnFrequencyField;
	PDBF PTable;
	
	
	/*                                                           */
	/* The next two tokens should be "allele" and "frequencies": */
	/*                                                           */
	GetToken();
	if(pT->i!=ALLEK) MadelineErrorReport("Read(): Keyword ALLELE expected: READ ALLELE FREQUENCIES [ FROM \"TableName\" ]");
	GetToken();
	if(pT->i!=FREQK) MadelineErrorReport("Read(): Keyword FREQUENCIES expected: READ ALLELE FREQUENCIES [ FROM \"TableName\" ]");
	
	/*                            */
	/* a database MUST be opened: */
	/*                            */
	if(!g->db) MadelineErrorReport("Read(): No pedigree table is open in which to write allele frequency information.");
	
	/*                                   */
	/* Look for optional keyword "FROM": */
	/*                                   */
	GetToken();
	if(pT->i==FROMK){
		
		EvaluateExpression(&result);  /* get the filename */
		if(result.t!=STRINGVALUE)
			MadelineErrorReport("Read(): Specify a filename to read from: READ ALLELE FREQUENCIES [ FROM \"TableName\" ]");
		/* Use user-specified name: */
		TableName=result.s;
	}else{
		
		/* Use default name from config.h: */
		TableName=ALLELE_FREQ_FILE_NAME;
		
	}
	
	
	/*                                            */
	/* SmartTableSelect figures out whether the   */
	/* user specified the .data or the .mfh file  */
	/* and checks the usability of the .mfh file. */
	/* If the .mfh does not exist, recognize() is */
	/* called to create it:                       */
	/*                                            */
	TableName=SmartTableSelect(TableName);
	
	/*                                                       */
	/* Make sure that the table is in an identifiable format */
	/*                                                       */
	TableType=CharacterizeDatabase(TableName);
	if(!TableType) MadelineErrorReport("Read(): \"%s\" is not in a recognized table format or is corrupt\n(For flat files, be sure to open the Madeline %s file).",TableName,HEADER_FILE_EXTENSION);
	/*                              */
	/* Set up and open the database */
	/*                              */
	PTable=NewDatabase(TableName,READONLY,TableType);
	PTable->OpenDatabase(PTable);
	GoTo(PTable,0);    /* set pointer to the top */
	/*                                                                      */
	/* Three columns, MapMarkerField, AlleleField, and FrequencyField, are  */
	/* required:                                                            */
	/*                                                                      */
	gfnMapMarker.i      = PTable->FieldNumber(PTable,gfnMapMarker.e.s);
	if(gfnMapMarker.i==MISSING)
		MadelineErrorReport("Read(): Unable to locate the required  MAP MARKER field in this table.");
	gfnAlleleField.i    = PTable->FieldNumber(PTable,gfnAlleleField.e.s);
	if(gfnAlleleField.i==MISSING)
		MadelineErrorReport("Read(): Unable to locate the required ALLELE field in this table.");
	gfnFrequencyField.i = PTable->FieldNumber(PTable,gfnFrequencyField.e.s);
	if(gfnFrequencyField.i==MISSING)
		MadelineErrorReport("Read(): Unable to locate the required FREQUENCY field in this table.");
	
	LoadMarkerAlleleFrequencyInformation(g->db,PTable,gfnMapMarker.i,gfnAlleleField.i,gfnFrequencyField.i);
	CloseDatabase(PTable);
	
	lprintf("Allele frequencies have been read from %s\n",TableName);
	
}


/*                                                */
/*                                                */
/* Save(): save allele frequencies to a table.    */
/*                                                */
/*                                                */
void Save(void){

	RESULT result;
	const char *TableName;
	
	extern PCONTROL g;
	extern int GetToken(void);
	extern PTOKENINFO pT;
	extern void EvaluateExpression(PRESULT result);
	
	extern VARIABLE gfnMapMarker;
	extern VARIABLE gfnAlleleField;
	extern VARIABLE gfnFrequencyField;
	
	int i;
	FILE *fout;
	
	/*                                                           */
	/* The next two tokens should be "allele" and "frequencies": */
	/*                                                           */
	GetToken();
	if(pT->i!=ALLEK) MadelineErrorReport("Save(): Keyword ALLELE expected: SAVE ALLELE FREQUENCIES [ TO \"TableName\" ]");
	GetToken();
	if(pT->i!=FREQK) MadelineErrorReport("Save(): Keyword FREQUENCIES expected: SAVE ALLELE FREQUENCIES [ TO \"TableName\" ]");
	
	/*                            */
	/* a database MUST be opened: */
	/*                            */
	if(!g->db) MadelineErrorReport("Save(): No pedigree table is open from which to obtain allele frequencies.");
	
	/*                                 */
	/* Look for optional keyword "TO": */
	/*                                 */
	GetToken();
	if(pT->i==TOC){
		
		EvaluateExpression(&result);  /* get the filename */
		if(result.t!=STRINGVALUE)
			MadelineErrorReport("Save(): Specify a filename to write to: SAVE ALLELE FREQUENCIES [ TO \"TableName\" ]");
		/* Use user-specified name: */
		TableName=result.s;
	}else{
		
		/* Use default name from config.h: */
		TableName=ALLELE_FREQ_FILE_NAME;
		
	}
	
	/*                       */
	/* Open file for output: */
	/*                       */
	fout=fopen(TableName,"w");
	if(!fout) MadelineErrorReport("Save(): Unable to open \"%s\" for writing.",TableName);
	
	/*                             */
	/* Print out the header lines: */
	/*                             */
	fprintf(fout,"%s C\n%s N\n%s N\n\n",
		gfnMapMarker.e.s,
		gfnAlleleField.e.s,
		gfnFrequencyField.e.s
	);
	
	/*                                           */
	/* Loop through output genotype fields only: */
	/*                                           */
	for(i=0;i<g->db->n.go;i++){
		/*                                                      */
		/* i is now the rank index, not the actual field index: */
		/* all the OutputMarker routines now recognize this to  */
		/* figure out the actual field index:                   */
		/*                                                      */
		OutputMarkerAlleleFrequencyTable(g,i,fout);
	}
	
	/*                    */
	/* Close output file: */
	/*                    */
	fclose(fout);
	lprintf("Table of allele frequencies for %i markers written to \"%s\"\n",
		g->db->n.go,
		TableName
	);
	/*                     */
	/* Recognize the file: */
	/*                     */
	RecognizeFile(TableName,g->lang,g->ivar,0);
	
	
}


/*                                  */
/* OutputMarkerAlleleFrequencyTable */
/*                                  */
void OutputMarkerAlleleFrequencyTable(PCONTROL c,int i,FILE *fout){
	
	int ii;
	
	/*                             */
	/* get the actual field index: */
	/*                             */
	ii=c->db->fi->g[i];
	
	TraverseAllele3(fout,c->db->m+ii,c->db->m[ii].a,c->db->m[ii].s,OutputMarkerAlleleFrequencyRow);
	
}

/*                                          */
/*                                          */
/* OutputMarkerAlleleFrequencyRow           */
/*                                          */
void OutputMarkerAlleleFrequencyRow(FILE *fout,PMARKER m,PALLELE a,int s){
	
	fprintf(fout,"%-32.32s %3i    %6.4f\n",m->nm,a->v,(double)a->n/(double)s);
	
}

/*                     */
/*                     */
/* ListAlleleFrequency */
/*                     */
void ListAlleleFrequency(PALLELE a,int s){
	
	extern PCONTROL g;
	
	lprintf("%3i%c%5i/%5i%c%6.4f\n",
		a->v,
		g->delimiter,
		a->n,
		s,
		g->delimiter,
		(double)a->n/(double)s
	);
	
}


/*                         */
/* ShowAlleleFrequencies() */
/*                         */
void ShowAlleleFrequencies(PCONTROL c){
	
	int i,end,stt;
	PTOKENINFO ptt[4];
	
	extern PTOKENINFO pT;
	extern int GetToken(void);
	
	/*                                     */
	/* The keyword "ALLELE" has been seen: */
	/* Get the next keyword, "FREQUENCIES" */
	/*                                     */
	GetToken();
	if(pT->i!=FREQK) MadelineErrorReport("ShowAlleleFrequencies(): Keyword \"FREQUENCIES\" expected: LIST ALLELE FREQUENCIES FOR <MARKER_1>, ...");
	
	GetToken();
	if(pT->i!=FORK) MadelineErrorReport("ShowAlleleFrequencies(): Keyword \"FOR\" expected: LIST ALLELE FREQUENCIES FOR <MARKER_1>, ...");
	
	do{
		
		end=MISSING;
		stt=MISSING;
		/*                                                             */
		/* Get field index numbers or field names separated by commas  */
		/* or possibly consisting of a series, like 21-25. Get no more */
		/* than three tokens needed for a series                       */
		/*                                                             */
		i=0;
		do{
			
			GetToken();
			ptt[i++]=pT;
			
		}while(i<4 && pT->i!=END && pT->i!=',');
		i--;
		if(i==1){
			
			/*                              */
			/* Single field or index number */
			/* followed by END or a comma:  */
			/*                              */
			if(ptt[0]->t==NUMERICVARIABLE || 
			   ptt[0]->t==STRINGVARIABLE  ||
			   ptt[0]->t==DATEVARIABLE
			){
				/*                                */
				/* Numeric, character, or date    */
				/* field name seen. In this case, */
				/* date and numeric fields will   */
				/* just be ignored:               */
				end=stt=ptt[0]->f;
				
			}else if(ptt[0]->t==NUMERICVALUE){      
				/*                             */
				/* literal integer value seen: */
				/*                             */
				end=stt=(int)ptt[0]->v-1;
			}else
				MadelineErrorReport("ShowAlleleFrequencies(): field name or field index expected");
			
		}else if(i==3 && ptt[1]->i=='-'){
			/*                                       */
			/* series specified with dash separator: */
			/*                                       */
			
			/*                        */
			/* first field in series: */
			/*                        */
			if(ptt[0]->t==NUMERICVARIABLE || ptt[0]->t==STRINGVARIABLE || ptt[0]->t==DATEVARIABLE ){
				/*                  */
				/* field name seen: */
				/*                  */
				stt=ptt[0]->f;
			}else if(ptt[0]->t==NUMERICVALUE){      
				/*                             */
				/* literal integer value seen: */
				/*                             */
				stt=(int)ptt[0]->v-1;
			}else
				MadelineErrorReport("ShowAlleleFrequencies(): field name or field index expected");
			
			/*                       */
			/* last field in series: */
			/*                       */
			if(ptt[2]->t==NUMERICVARIABLE || ptt[2]->t==STRINGVARIABLE || ptt[2]->t==DATEVARIABLE ){
				/*                  */
				/* field name seen: */
				/*                  */
				end=ptt[2]->f;
			}else if(ptt[2]->t==NUMERICVALUE){      
				/*                             */
				/* literal integer value seen: */
				/*                             */
				end=(int)ptt[2]->v-1;
			}else
				MadelineErrorReport("ShowAlleleFrequencies(): field name or field index expected");
			/*                          */
			/* error checking on range: */
			/*                          */
			if(end<stt)
				MadelineErrorReport("ShowAlleleFrequencies(): specify an increasing range like 3-5,7-9, etc.");
		}else
			MadelineErrorReport("ShowAlleleFrequencies(): specify field names or indices separated by commas or dashes");
		
		for(i=stt;i<=end;i++){
			
			if(!(c->db->ff[i]&GENOTYPESET)){
				
				eprintf(WARNINGFLAG,"Ignoring field \"%i. %s\" because it is not a genotype marker field...\n",
					i+1,
					c->db->FieldName(c->db,i)
				);
				continue;
				
			}
			/*                                                             */
			/* get here if it's OK to print the marker allele frequencies: */
			/*                                                             */
			lprintf("%i. %s:\n\n",i+1,c->db->FieldName(c->db,i));
			TraverseAllele4(c->db->m[i].a,c->db->m[i].s,ListAlleleFrequency);
			lprintf("\n");
			
		}
		
	}while(pT->i==',');
	
}

