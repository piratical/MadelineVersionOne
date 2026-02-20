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
/* "merlin_qtdt_format.h" is part of the Madeline software distribution.             */
/* Written by Edward H. Trager.                                                      */
/*                                                                                   */
/* COPYRIGHT (C) 2000,2002,2003 THE REGENTS OF THE UNIVERSITY OF MICHIGAN,           */
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
/* merlin_qtdt_format.c                                                     */
/*                                                                          */
/* history: 2003.02.25.ET initiated                                         */
/*                                                                          */
/*                                                                          */

#include "merlin_qtdt_format.h"
#include "generic_format.h"
#include "fieldflags.h"
#include "logs.h"
#include "traversal.h"
#include "dataevaluation.h"

/*////////////////// */
/*                   */
/* MERLIN:           */
/*                   */
/*                   */
/*                   */
/*////////////////// */
METHODS MerlinQTDT={

	NULL,
	NULL,
	NULL,
	
	MerlinQTDTDataHeaderProcessor,
	MerlinQTDTOutputPedigree,
	MerlinQTDTDataFooterProcessor,
	
	ShowPedigreeStatistics
};


/*                               */
/* MerlinQTDTDataHeaderProcessor */
/*                               */
void MerlinQTDTDataHeaderProcessor(PCONTROL c,char *fn){
	
	extern VARIABLE gParameterOutputFile;
	
	int i,ii;
	
	
	/*                                             */
	/* For MERLIN / QTDT, we first write a ".data" */
	/* file which is really just a header for the  */
	/* pedigree file.  We use the parameter file   */
	/* for this:                                   */
	StoreString(&gParameterOutputFile,ModifyFileName(fn,DATA_FILE_EXTENSION));
	
	/*                                                  */
	/* Then open up the .data file to write the header: */
	/*                                                  */
	c->streams.fpar=fopen(gParameterOutputFile.e.s,"wt");
	if(!c->streams.fpar){
		MadelineErrorReport("MerlinQTDTDataHeaderProcessor(): unable to open \"%s\" for output",gParameterOutputFile.e.s);
	}

	lprintf("Creating associated header file called \"%s\" ...\n",gParameterOutputFile.e.s);

	
	/* First, always output the ZYGOSITY column: */
	fprintf(c->streams.fpar,"Z ZYGOSITY\n");
	/* Next, always output an AFFECTION status column: */
	fprintf(c->streams.fpar,"A AFFECTION_STATUS\n");
	
	/* First, GENOTYPE columns: */
	for(i=0;i<c->db->n.go;i++){
		/*                          */
		/* Get the true field index */
		/*                          */
		ii=c->db->fi->g[i];
		/* print the column type and name: */
		fprintf(c->streams.fpar,"M %s\n",c->db->FieldName(c->db,ii));
	}
	
	/*                                               */
	/* Next, we output all phenotype (trait) fields, */
	/* followed by all covariate fields:             */
	/*                                               */
	
	/* TRAITS first: */
	for(i=0;i<c->db->n.po;i++){
		/*                          */
		/* Get the true field index */
		/*                          */
		ii=c->db->fi->p[i];
		/*                                                  */
		/* choose only the "PO" phenotype output variables: */
		/*                                                  */
		if(     c->db->ff[ii]&PHENOTYPESET 
		   && !(c->db->ff[ii]&COVARIATESET)
		){
			/* print the column type and name: */
			fprintf(c->streams.fpar,"T %s\n",c->db->FieldName(c->db,ii));
		}
	}
	
	/* COVARIATES next:                                  */
	/*                                                   */
	/* Recall that covariates are treated as a subset of */
	/* the phenotype variables:                          */
	/*                                                   */
	for(i=0;i<c->db->n.po;i++){
		/*                          */
		/* Get the true field index */
		/*                          */
		ii=c->db->fi->p[i];
		/*                                                  */
		/* choose only the "VO" phenotype output variables: */
		/*                                                  */
		if(     c->db->ff[ii]&COVARIATESET 
		   &&   c->db->ff[ii]&OUTPUTSET
		){
			/* print the column type and name: */
			fprintf(c->streams.fpar,"C %s\n",c->db->FieldName(c->db,ii));
		}
	}
	
	/* close the ".data" header file: */
	fclose(c->streams.fpar);
	c->streams.fpar=NULL;
	
	/*                                                                    */
	/* call generic to open output stream, store file name, reset control */
	/* counters, and inform user:                                         */
	/*                                                                    */
	GenericDataHeaderProcessor(c,fn);
	
	
}

/*                          */
/* MerlinQTDTOutputPedigree */
/*                          */
void MerlinQTDTOutputPedigree(PCONTROL c,PPEDIGREE pd){

	/*                                                                  */
	/* Check all individuals for data under the current criteria:       */
	/*                                                                  */
	TraverseIndividualBSTMarkDataFlag(pd->h,GeneralCaseIndividualHasData);

	/*                                                             */
	/* return if sufficient data is not available in the pedigree: */
	/*                                                             */
	/*                                                             */
	if(!GeneralCasePedigreeHasData(c,pd)) return;
	
	/*                  */
	/* Output the data: */
	/*                  */
	TraverseIndividualBST4(c,pd,pd->h,OutputMerlinQTDTIndividualRecord);
	
}


/*                                                                             */
/*                                                                             */
/* OutputMerlinQTDTIndividualRecord()                                          */
/*                                                                             */
/*                                                                             */
void OutputMerlinQTDTIndividualRecord(PCONTROL c,PPEDIGREE pd,PINDIVIDUAL p){

	
	extern VARIABLE gfnID;
	
	int i,ii,j;

	char notwin[]=" 0";
	char mztwin[]="MZ";
	char dztwin[]="DZ";
	char twinbuffer[3]="  ";
	char *twin;
	
	char hold[SMALLBUFFERSIZE];
	int wide,prec;
	
	int ntr,ncv; /* no. of traits, covariates, respectively */
	
	char *o;
	char *allele1;
	char *allele2;
	int nallele1;
	int nallele2;
	double v;
	int w;
	
	if(ExcludedForGeneralCase(p)) return;
	 
	/*                         */
	/* Find out the ID width   */
	/* so we know how wide to  */
	/* print missing values:   */
	w=c->db->FieldLength(c->db,gfnID.i);
	
	/*                                                             */
	/* The following are MERLIN/QTDT default structure fields that */
	/* are not explicitly noted in the ".data" header file:        */
	/*                                                             */
	/* 1. PEDIGREE   ID                                            */
	/* 2. INDIVIDUAL ID                                            */
	/* 3. FATHER                                                   */
	/* 4. MOTHER                                                   */
	/* 5. GENDER                                                   */
	/*                                                             */
	
	/*                   */
	/* 1. PEDIGREE ID:   */
	/*                   */
	fprintf(c->streams.fp,"%s ",pd->fid);
	
	/*                   */
	/* 2. INDIVIDUAL ID: */
	/*                   */
	fprintf(c->streams.fp,"%s ",p->id);
	
	/*                   */
	/* 3. FATHER:        */
	/*                   */
	if(!p->fid[0] || p->fid[0]==CHARMISSINGVALUE) fprintf(c->streams.fp,"%-*.*s ",w,w,"x");
	else                                          fprintf(c->streams.fp,"%s ",p->fid);
	
	/*                   */
	/* 4. MOTHER:        */
	/*                   */
	if(!p->mid[0] || p->mid[0]==CHARMISSINGVALUE) fprintf(c->streams.fp,"%-*.*s ",w,w,"x");
	else                                          fprintf(c->streams.fp,"%s ",p->mid);
	
	/*                   */
	/* 5. GENDER:        */
	/*                   */
	fprintf(c->streams.fp,"%c ",p->female==MISSING?'x':(p->female?'F':'M'));

	/*                                                             */
	/* The next set are not required by default for MERLIN/QTDT,   */
	/* but we are going to have Madeline always output them:       */
	/*                                                             */
	/* 6. Zygosity                                                 */
	/* 7. Affection Status                                         */
	/*                                                             */
	
	/*              */
	/* 6. ZYGOSITY: */
	/*              */
	if(!p->twin) twin=notwin;
	else if(p->twin=='A') twin=mztwin;
	else if(p->twin=='a') twin=dztwin;
	else if(isupper(p->twin)){
		/* This produces odd digits '1','3','5','7','9' */
		twinbuffer[1] = (p->twin - 'A')*2-1 + '0';
		twin=twinbuffer;
	}else if(islower(p->twin)){
		/* This produces even digits '2','4','6','8' */
		twinbuffer[1] = (p->twin - 'a')*2   + '0';
		twin=twinbuffer;
	}else{
		twin=twinbuffer;
	}
	fprintf(c->streams.fp,"%s ",twin);
	
	/*                      */
	/* 7. AFFECTION         */
	/*    STATUS:           */
	/*                      */
	/* Coded as in LINKAGE: */
	/*                      */
	fprintf(c->streams.fp,"%c ",p->affected==MISSING?'0':(p->affected?'2':'1'));
	
	/*                                                        */
	/* Add some extra spaces after the core fields and before */
	/* the genotypes to improve readability                   */
	/*                                                        */
	fprintf(c->streams.fp,"   ");
	
	/*                                            */
	/* note number of trait and covariate fields: */
	/*                                            */
	ncv=c->db->n.vo;
	ntr=c->db->n.po-ncv;
	
	
	/*                  */
	/* GENOTYPE FIELDS: */
	/*                  */
	if(p->db && !p->excluded){
		/*                                              */
		/* print out only genotype fields in map order: */
		/*                                              */
		for(i=0;i<p->db->n.go;i++){
			/*                  */
			/* get field index: */
			/*                  */
			ii=p->db->fi->g[i];
			/*                          */
			/* read the genotype field: */
			/*                          */
			o=p->db->ReadString(p->db,hold,ii,p->rci);
			/*                                   */
			/* check for missing values:         */
			/*                                   */
			/* if(IsCMV(o)){                     */
			/*                                   */
			/* if(!strcmp(o,CHARMISSINGSTRING)){ */
			/*                                   */
			if(!o[0]){
				/*                                 */
				/* Print zeros for missing values: */
				/*                                 */
				/* old code had: o="0/0";          */
				/*                                 */
				fprintf(c->streams.fp,"%*i/%*i  ",
					MERLIN_ALLELE_WIDTH,
					0,
					MERLIN_ALLELE_WIDTH,
					0
				);
			}else{
				/*                                     */
				/* get allele1 and allele2 substrings: */
				/*                                     */
				for(allele1=allele2=o;*allele2 && *allele2!='/';allele2++);
				*allele2++='\0';
				/*                                                  */
				/* Convert the alleles to their corresponding ranks */
				/*                                                  */
				nallele1=atoi(allele1);
				nallele2=atoi(allele2);
				
				/* ORIGINAL CODE FOR PRINTING RANKS: */
				//if(nallele1==nallele2){
				//	nallele2=nallele1=GetAlleleRank(p->db->m+ii,nallele1);
				//}else{
				//	nallele1=GetAlleleRank(p->db->m+ii,nallele1);
				//	nallele2=GetAlleleRank(p->db->m+ii,nallele2);
				//}
				
				/*            */
				/* and print: */
				/*            */
				fprintf(c->streams.fp,"%*i/%*i  ",
					MERLIN_ALLELE_WIDTH,
					nallele1,
					MERLIN_ALLELE_WIDTH,
					nallele2
				);
			}
		}
		/*                                     */
		/* Now print PHENOTYPE AND COVARIATE   */
		/* FIELDS:                             */
		/*                                     */
		if(c->db->n.po){
			/*         */
			/* TRAITS: */
			/*         */
			if(ntr){
				for(j=i=0;j<ntr && i<c->db->n.po;i++){
					/*                  */
					/* get field index: */
					/*                  */
					ii=p->db->fi->p[i];
					
					if(  c->db->ff[ii]&PHENOTYPESET &&
					   !(c->db->ff[ii]&COVARIATESET)
					){
						
						/*              */
						/* Field width: */
						/*              */
						wide=p->db->FieldLength(p->db,ii);
						/* 2003.04.16.ET ADDENDUM FOR TESTING: */
						if(p->db->FieldType(p->db,ii)=='C'){
							o=p->db->ReadString(p->db,hold,ii,p->rci);
							if(!o[0]){
								fprintf(c->streams.fp,"%*.*s ",wide,wide,"x");
							}else{
								fprintf(c->streams.fp,"%*.*s ",wide,wide,o);
							}
						}else{
							/*                */
							/* get precision: */
							/*                */
							prec=p->db->FieldPrecision(p->db,ii);
							/*                    */
							/* read numeric data: */
							/*                    */
							v=p->db->ReadDouble(p->db,ii,p->rci);
							/*                           */
							/* check for missing values: */
							/*                           */
							if(v==DOUBLEMISSINGVALUE){
								/*                                            */
								/* Print an "x" to represent a missing value: */
								/*                                            */
								fprintf(c->streams.fp,"%*.*s ",wide,wide,"x");
								
							}else{
								/*             */
								/* Print data: */
								/*             */
								fprintf(c->streams.fp,"%s ",FormatNumber(v,NULL,wide,prec));
							}
						}
						j++;
						
						
						
					}
				}
			}
			/*             */
			/* COVARIATES: */
			/*             */
			if(ncv){
				for(j=i=0;j<ncv && i<c->db->n.po;i++){
					/*                  */
					/* get field index: */
					/*                  */
					ii=p->db->fi->p[i];
					
					if(c->db->ff[ii]&OUTPUTSET &&
					   c->db->ff[ii]&COVARIATESET
					){
						/*              */
						/* Field width: */
						/*              */
						wide=p->db->FieldLength(p->db,ii);
						/*                */
						/* get precision: */
						/*                */
						prec=p->db->FieldPrecision(p->db,ii);
						/*                    */
						/* read numeric data: */
						/*                    */
						v=p->db->ReadDouble(p->db,ii,p->rci);
						/*                           */
						/* check for missing values: */
						/*                           */
						if(v==DOUBLEMISSINGVALUE){
							/*                                            */
							/* Print an "x" to represent a missing value: */
							/*                                            */
							fprintf(c->streams.fp,"%*.*s ",wide,wide,"x");
							
						}else{
							/*             */
							/* Print data: */
							/*             */
							fprintf(c->streams.fp,"%s ",FormatNumber(v,NULL,wide,prec));
						}
						j++;
					}
				}
			}
		}
	}else{ /* not in database, or marked for exclusion:      */
		/*                                                      */
		/* print out zeros for both alleles of genotype fields: */
		/*                                                      */
		allele1=allele2="0";
		for(i=0;i<c->db->n.go;i++){
			fprintf(c->streams.fp,"%*.*s/%*.*s  ",
				MERLIN_ALLELE_WIDTH,
				MERLIN_ALLELE_WIDTH,
				allele1,
				MERLIN_ALLELE_WIDTH,
				MERLIN_ALLELE_WIDTH,
				allele2
			);
		}
		/*                                                         */
		/* Now print PHENOTYPE FIELDS:                             */
		/*                                                         */
		
		if(c->db->n.po){
			/*           */
			/* TRAITS:   */
			/*           */
			if(ntr){
				for(j=i=0;j<ntr && i<c->db->n.po;i++){
					/*                  */
					/* get field index: */
					/*                  */
					ii=c->db->fi->p[i];
					
					if(  c->db->ff[ii]&PHENOTYPESET &&
					   !(c->db->ff[ii]&COVARIATESET)
					){
						/*              */
						/* Field width: */
						/*              */
						wide=c->db->FieldLength(c->db,ii);
						/*                                            */
						/* Print an "x" to represent a missing value: */
						/*                                            */
						fprintf(c->streams.fp,"%*.*s ",wide,wide,"x");
						j++;
					}
				}
			}
			/*             */
			/* COVARIATES: */
			/*             */
			if(ncv){
				for(j=i=0;j<ncv && i<c->db->n.po;i++){
					/*                  */
					/* get field index: */
					/*                  */
					ii=c->db->fi->p[i];
					
					if(c->db->ff[ii]&OUTPUTSET &&
					   c->db->ff[ii]&COVARIATESET
					){
						/*              */
						/* Field width: */
						/*              */
						wide=c->db->FieldLength(c->db,ii);
						/*                                            */
						/* Print an "x" to represent a missing value: */
						/*                                            */
						fprintf(c->streams.fp,"%*.*s ",wide,wide,"x");
						j++;
					}
				}
			}
		}		
	}
	fprintf(c->streams.fp,"\n");
	/*                            */
	/* Mark individual as output: */
	/*                            */
	p->included++;
}


/*                               */
/* MerlinQTDTDataFooterProcessor */
/*                               */
void MerlinQTDTDataFooterProcessor(PCONTROL c){
	
	/*                      */
	/* close the data file: */
	/*                      */
	if(c->streams.fp){
		fclose(c->streams.fp);
		c->streams.fp=NULL;
	}
	
}

