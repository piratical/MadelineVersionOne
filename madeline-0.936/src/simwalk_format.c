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
/* "simwalk_format.c" is part of the Madeline software distribution.                 */
/* Written by Edward H. Trager.                                                      */
/*                                                                                   */
/* COPYRIGHT (C) 2000,2002, 2003 THE REGENTS OF THE UNIVERSITY OF MICHIGAN,          */
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
/* Last modified by Ed Trager, February, 2003.                                       */
/*                                                                                   */
/* ********************************************************************************* */

/*                                                                          */
/*                                                                          */
/* simwalk_format.c                                                         */
/*                                                                          */
/* history: 2003.02.27.ET initiated                                         */
/*                                                                          */
/*                                                                          */

#include "simwalk_format.h"
#include "mendel_format.h"
#include "generic_format.h"
#include "fieldflags.h"
#include "logs.h"
#include "traversal.h"
#include "dataevaluation.h"
#include "config.h"
#include "methods.h"
#include "showstatistics.h"
#include "haldanekosambi.h"
#include "knowntokens.h"

/*////////////////// */
/*                   */
/* SIMWALK:          */
/*                   */
/*                   */
/*                   */
/*////////////////// */
METHODS Simwalk={

	SimwalkLocusHeaderProcessor,
	OutputMendelMarker,
	GenericLocusFooterProcessor,
	
	SimwalkDataHeaderProcessor,
	SimwalkOutput,
	GenericDataFooterProcessor,
	
	ShowPedigreeStatistics
};


/*////////////////// */
/*                   */
/* MENDEL:           */
/*                   */
/*                   */
/*                   */
/*////////////////// */
METHODS Mendel={

	SimwalkLocusHeaderProcessor,
	OutputMendelMarker,
	GenericLocusFooterProcessor,
	
	SimwalkDataHeaderProcessor,
	SimwalkOutput,
	GenericDataFooterProcessor,
	
	ShowPedigreeStatistics
};


void SimwalkLocusHeaderProcessor(PCONTROL c, char *fn){
	
	/* Call Generic function to open file: */
	GenericLocusHeaderProcessor(c,fn);
	
	/* Write out the disease locus information: */
	
	/* The frequencies for U (unaffected) and A (affected) should remain unused    */
	/* for any NPL analyses.  It's just one of those FORTRAN/MENDEL artifacts that */
	/* we need to specify the frequencies here:                                    */
	fprintf(c->streams.fp,"DISEASE AUTOSOME 2 2\nU        0.99900\nA        0.00100\n");
	/* Show genotypes for "U" category: */
	fprintf(c->streams.fp,"U       3\nU/U\nU/A\nA/A\n");
	/* Show genotypes for "A" category: */
	fprintf(c->streams.fp,"A       3\nU/U\nU/A\nA/A\n");
	
}

/*                                                                      */
/* CreateSimwalkBATCH2ControlFile()                                     */
/*                                                                      */
/* fn: name of the "BATCH2.DAT" file for Simwalk (i.e., "BATCH2.DAT" !) */
/* base_fn: base name for the analysis files, to which we will append   */
/* .ped, .map, .loc, etc.                                               */
/*                                                                      */
void CreateSimwalkBATCH2ControlFile(char *fn,char *base_fn,char *pedigree_fn){
	
	FILE *fp;
	
	char SymLinkCommand[MINIMUMBUFFERSIZE];
	
	/* open the file: */
	fp=fopen(fn,"w");
	if(!fp) MadelineErrorReport("CreateSimwalkBATCH2ControlFile(): Unable to open \"%s\" for writing.",fn);
	
	/* BLANK LINE AT BEGINNING: I actually think this is required (Weird, huh?) */
	/* BATCH ITEM 01 TYPE OF ANALYSIS: Madeline assumes NPL to avoid setting penetrances, etc. */
	fprintf(fp,"\n01                       ! batch item number\n");
	fprintf(fp,"3                        ! analysis: 1=Haplotype; 2=LOD; 3=NPL; 4=IBD 5=Mistyping\n");
	/* BATCH ITEM 02 NOT SURE WHAT THIS IS FOR ... */ 
	fprintf(fp,"\n02                       ! batch item number\n");
	fprintf(fp,"33                       ! integer label for this run of the program\n");
	/* BATCH ITEM 03 ANALYSIS TITLE */
	fprintf(fp,"\n03                       ! batch item number\n");
	fprintf(fp,"PUT YOUR ANALYSIS TITLE HERE\n");            
	/* BATCH ITEM 09 MAP FILE   */
	fprintf(fp,"\n09                       ! batch item number\n");
	fprintf(fp,"%s%s              ! name of map file\n",base_fn,MAP_FILE_EXTENSION);
	/* BATCH ITEM 10 LOCUS FILE */
	fprintf(fp,"\n10                       ! batch item number\n");
	fprintf(fp,"%s%s              ! name of locus file\n",base_fn,LOCUS_FILE_EXTENSION);
	/* BATCH ITEM 11 PEDIGREE FILE */
	fprintf(fp,"\n11                       ! batch item number\n");
	fprintf(fp,"%s                 ! name of pedigree file\n",pedigree_fn);
	/* BATCH ITEM 12 SYMBOLS FOR MALE AND FEMALE: USE MADELINE/MENDEL DEFAULTS */
	fprintf(fp,"\n12                       ! batch item number\n");
	fprintf(fp,"F                        ! symbol for female (case insensitive)\n");
	fprintf(fp,"M                        ! symbol for   male (case insensitive)\n");
	/* BATCH ITEM 13 IS TRAIT LISTED IN LOCUS AND PEDIGREE FILES ? YES */
	fprintf(fp,"\n13                       ! batch item number\n");
	fprintf(fp,"Y                        ! is trait listed in locus and pedigree files?\n");
	/* BATCH ITEM 16 LABEL FOR AFFECTED INDIVIDUALS: USE MADELINE CHARACTER STANDARD "A" */
	fprintf(fp,"\n16                       ! label for affected individuals\n");
	fprintf(fp,"A                        ! must match LOCUS and PEDIGREE files\n");
	/* BATCH ITEM 18 NUMBER OF QUANTITATIVE VARIABLES: 0 FOR NOW */
	fprintf(fp,"\n18                       ! batch item number\n");
	fprintf(fp,"0                        ! number of quantitative variables in pedigree file\n");
	/* BATCH ITEM 48 NUMBER OF SIMULATIONS FOR p-VALUES */ 
	fprintf(fp,"\n48                       ! batch item number\n");
	fprintf(fp,"10000                    ! number of unconditional simulations for p-values\n"); 
	
	/* Close file: */
	fclose(fp);
	
	lprintf("NOTE: Simwalk batch file, \"%s\", has been created.\n",fn);
	lprintf("      Edit this file to change the parameters of your analysis.\n\n");
	/* Create symbolic link: */
	snprintf(SymLinkCommand,MINIMUMBUFFERSIZE-1,"ln -s %s BATCH2.DAT",fn);
	system(SymLinkCommand);
	
}

/*                             */
/* SimwalkDataHeaderProcessor: */
/* Almost identical to Mendel  */
/*                             */
void SimwalkDataHeaderProcessor(PCONTROL c, char *fn){
	
	int i,ii;
	int w; /* output field width: adjusted, if necessary */
	
	char BatchFileName[MINIMUMBUFFERSIZE];
	
	char *Path,*Name;
	
	if(c->OutputFormat==SIMWK){
		
		/*                               */
		/* Create the "BATCH2.DAT" file: */
		/*                               */
		SplitPath(fn,&Path,&Name);
		snprintf(BatchFileName,MINIMUMBUFFERSIZE-1,"%sBATCH2.DAT.%s",Path,Name);
		
		CreateSimwalkBATCH2ControlFile(BatchFileName,Name,Name);
		/*                                   */
		/* Create the map (rec. frac.) file: */
		/*                                   */
		snprintf(BatchFileName,MINIMUMBUFFERSIZE-1,"%s%s",fn,MAP_FILE_EXTENSION);
		CreateSimwalkMapFile(c,BatchFileName);
		
	}
	
	/*                                                                    */
	/* call generic to open output stream, store file name, reset control */
	/* counters, and inform user:                                         */
	/*                                                                    */
	GenericDataHeaderProcessor(c,fn);
	
	/* PRINT FORMAT FOR PEDIGREE HEADER RECORD: */
	fprintf(c->streams.fp,"(I3,1X,A8)\n");
	/* PRINT FORMAT FOR STUDYID,FATHER,MOTHER,GENDER,MZTWIN, and AFFECTION STATUS */
	fprintf(c->streams.fp,"(3(A8,1X),2(A1,1X),A1,1X");

	/*                                                                  */
	/* phenotype fields first: these will be output with individualized */
	/* widths                                                           */
	/*                                                                  */
	for(i=0;i<c->db->n.po;i++){
		/*                          */
		/* Get the true field index */
		/*                          */
		ii=c->db->fi->p[i];
		/*                                                     */
		/* Fix width if it exceeds MENDEL's 8-character width: */
		/*                                                     */
		if((w=c->db->FieldLength(c->db,ii))>MAXIMUMMENDELFIELDWIDTH)
			w=MAXIMUMMENDELFIELDWIDTH;
		/*                                     */
		/* print the FORTRAN format specifier: */
		/*                                     */
		fprintf(c->streams.fp,",A%i,1X",w);
	}
	/*                                                     */
	/* Now genotype fields: all fixed width xxx/xxx A7,1X: */
	/*                                                     */
	fprintf(c->streams.fp,",%i(A%i,1X)",c->db->n.go,MENDELGENOTYPEFIELDWIDTH);

	/*                                                        */
	/* terminate the FORTRAN record with closing parentheses  */
	/* and a carriage return:                                 */
	/*                                                        */
	fprintf(c->streams.fp,")\n");
	
}

/*                                                  */
/*                                                  */
/* SimwalkOutput(): outputs data almost identical   */
/* to Generic Mendel                                */
/*                                                  */
void SimwalkOutput(PCONTROL c,PPEDIGREE pd){
	
	int noutput;
	
	/*                                                            */
	/* Check all individuals for data under the current criteria: */
	/*                                                            */
	TraverseIndividualBSTMarkDataFlag(pd->h,MendelIndividualHasData);
	
	/*                                                                 */
	/* Does pedigree have data? If so, MendelPedigreeHasData() returns */
	/* the number of individuals to be included:                       */
	/*	                                                                */
	noutput=MendelPedigreeHasData(c,pd);
	if(noutput){
		fprintf(c->streams.fp,"%3i %-8.8s\n",noutput,pd->fid);	
		TraverseIndividualBST2(c,pd->h,OutputSimwalkIndividualRecord);
	}
	

}


/*                                     */
/*                                     */
/* OutputSimwalkIndividualRecord()     */
/*                                     */
/* Almost identical to Generic MENDEL  */
/*                                     */
void OutputSimwalkIndividualRecord(PCONTROL c,PINDIVIDUAL p){
	
	int i,ii;
	
	
	if(ExcludedForMendel(p)) return;
	
	/*                                            */
	/* Write core fixed fields (COREOUTSET):      */
	/*                                            */
	/* Note that Simwalk only looks for affecteds */
	/* and there is effectively no category for   */
	/* MISSING/UNKNOWN.  It's a dichotomous       */
	/* affair with "A" for affecteds, and "U"     */
	/* for everybody else.                        */
	/*                                            */
	fprintf(c->streams.fp,"%-8.8s %-8.8s %-8.8s %c %c %c ",
		p->id,
		p->fid[0]==CHARMISSINGVALUE?gMendelOutputMissingValue:p->fid,
		p->mid[0]==CHARMISSINGVALUE?gMendelOutputMissingValue:p->mid,
		p->female?'F':'M',
		p->twin && isupper(p->twin)?p->twin:' ',
		p->affected==MISSING?' ':(p->affected?'A':'U')
	);
	
	/*                                     */
	/* now loop and print fields:          */
	/* note that there will be NO data for */
	/* the inserted fathers and mothers,   */
	/* hence need to check db for NULL:    */
	/*                                     */
	
	/*                   */
	/* Phenotype fields: */
	/*                   */
	for(i=0;i<c->db->n.po;i++){
		/*                      */
		/* Get the field index: */
		/*                      */
		ii=c->db->fi->p[i];
		OutputMendelField(c,p,ii,0);
	}
	
	/*                  */
	/* Genotype fields: */
	/*                  */
	for(i=0;i<c->db->n.go;i++){
		/*                      */
		/* Get the field index: */
		/*                      */
		ii=c->db->fi->g[i];
		OutputMendelField(c,p,ii,1);
	}
			
	/*                              */
	/* Terminating carriage return: */
	/*                              */
	fprintf(c->streams.fp,"\n");
	
	/*                            */
	/* Mark individual as output: */
	/*                            */
	p->included++;
	
}

/*                                                  */
/* CreateSimwalkMapFile(): Prints out a file        */
/* containing a list of markers in map order        */
/* with the recombination fractions between each    */
/* pair of entries.                                 */
/*                                                  */
/* WARNING: As currently written, this ASSUMES the  */
/*          map is in Kosambi cM. Normally, that is */
/*          indeed the case ...                     */
/*                                                  */
void CreateSimwalkMapFile(PCONTROL c,const char *fn){
	
	FILE *fp;
	double lpos;
	const char *FieldName;
	int i,ii;
	PMARKERMAP mp;
	
	fp=fopen(fn,"w");
	if(!fp) MadelineErrorReport("CreateSimwalkMapFile(): Unable to open \"%s\" for writing.",fn);
	/*                                                                 */
	/* Print out the "DISEASE" locus and dummy recombination fraction: */
	/*                                                                 */
	fprintf(fp,"DISEASE\n        000.0001\n");
	
	/*                                                 */
	/* Print out inter-marker recombination fractions: */
	/*                                                 */
	for(lpos=0.0,i=0;i<c->db->n.go;i++){
		/*                           */
		/* Get the real field index: */
		/*                           */
		ii=c->db->fi->g[i];
		/*                      */
		/* Get the marker name: */
		/*                      */
		FieldName=c->db->FieldName(c->db,ii);
		/*                          */
		/* Lookup the map position: */
		/*                          */
		mp=LookupMarkerMap(c->mp,FieldName);
		
		/*                     */
		/* check against NULL: */
		/*                     */
		if(!mp){
			fclose(fp);
			MadelineErrorReport("CreateSimwalkMapFile()():\n\tNo map information available for %s",c->db->FieldName(c->db,ii));
		}
		/*                                         */
		/* print out intermarker distances as      */
		/* recombination fractions as soon as      */
		/* the second marker map is seen:          */
		/*                                         */
		if(i){
			
			fprintf(fp,"        %8.4f\n",Kosambi_cM_To_Theta( mp->ps - lpos ));
		}
		
		/*                            */
		/* Print out the marker name: */
		/*                            */
		fprintf(fp,"%s\n",FieldName);
		
		/*                                */
		/* save the most recent position: */
		/*                                */
		lpos=mp->ps;
	}
	
	/*                        */
	/* Close file, tell user: */
	/*                        */
	fclose(fp);
	lprintf("NOTE: Simwalk map file \"%s\" has been created.\n",fn);
	
}

