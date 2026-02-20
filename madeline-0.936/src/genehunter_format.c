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
/* "genehunter_format.c" is part of the Madeline software distribution.              */
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
/* genehunter_format.c                                                      */
/*                                                                          */
/* history:  These routines were originally stuffed into madeline.c.        */
/*           They have now been segregated out in an effort to streamline   */
/*           code maintenance, management, and intelligibility.             */
/*           -2003.02.28.ET                                                 */
/*                                                                          */

#include "genehunter_format.h"
#include "generic_format.h"
#include "fieldflags.h"
#include "logs.h"
#include "traversal.h"
#include "dataevaluation.h"
//#include "config.h"
#include "methods.h"
#include "showstatistics.h"
#include "knowntokens.h"

/*//////////// */
/*             */
/* GENEHUNTER: */
/*             */
/* -OK         */
/*             */
/*//////////// */
METHODS Genehunter={

	OutputGenehunterLocusFileHeader,
	OutputGenehunterMarker,
	OutputGenehunterLocusFileFooter,
	
	GenehunterDataHeaderProcessor,
	GenehunterOutput,
	GenericDataFooterProcessor,
	
	ShowPedigreeStatistics

};


/*                                   */
/* OutputGenehunterLocusFileHeader() */
/*                                   */
void OutputGenehunterLocusFileHeader(PCONTROL c,char *fn){

	int i;

	/*                                                       */
	/* Genehunter format now requires that map be available: */
	/*                                                       */
	if(!c->mp)
		MadelineErrorReport("OutputGenehunterLocusFileHeader():\n\ta marker map table is required for Genehunter output");

	/*                   */
	/* open file stream: */
	/*                   */
	GenericLocusHeaderProcessor(c,fn);
	/*                                                    */
	/* add one to the number of markers because the idea  */
	/* is that the disease locus is one of the loci along */
	/* with the marker loci and we're trying to place the */
	/* disease locus amongst the marker loci:             */
	/*                                                    */
	
	/*                                                                 */
	/* Now we also include the number of phenotypes (with covariates): */
	/*                                                                 */
	/* -981104 ET                                                      */
	/*                                                                 */
	fprintf(c->streams.fp,"%3i 0 0 5\n",c->db->n.go+1+c->db->n.po);
	fprintf(c->streams.fp,"0 0.0 0.0 0\n");
	/*                                                            */
	/* print out the map order of the markers:                    */
	/*                                                            */
	/* the NEW code correctly accounts for the fact that the user */
	/* MAY NOT BE using all of the markers:                       */
	/*                                                            */
	/* We add two so that the numbering goes from 2 to n+1        */
	/* where n is the number of markers                           */
	/* because the disease locus is considered the first          */
	/* locus in Genehunter                                        */
	/*                                                            */
	for(i=0;i<c->db->n.go;i++){
		fprintf(c->streams.fp,"%3i ",i+2);
	}
	
	
	/*                              */
	/* terminating carriage return: */
	/*                              */
	fprintf(c->streams.fp,"\n");

	/*                                                                */
	/* print out the default normal and disease allele frequencies:   */
	/* this is just made up for the user to edit: "1" is for normals, */
	/* "2" for disease:                                               */
	/*                                                                */
	fprintf(c->streams.fp,"1 2\n0.99 0.01\n");
	/*                                                            */
	/* print out the default liability class /penetrance function */
	/* information: here we just default to one class:            */
	/*                                                            */
	fprintf(c->streams.fp,"1\n0.050 0.950 0.950\n");
	
	/*                                                */
	/* that's it! -- next come the marker information */
	/*                                                */
}

/*                                    */
/* OutputGenehunterLocusFileFooter(): */
/*                                    */
void OutputGenehunterLocusFileFooter(PCONTROL c){

	int i,ii,j;
	PMARKERMAP mp;
	double lpos;    /* inter-marker distances */
	
	int ntr,ncv; /* no. of traits, covariates, respectively */
	
	ncv=c->db->n.vo;
	ntr=c->db->n.po-ncv;
	
	

	/*                                  */
	/* Print out phenotype information: */
	/*                                  */
	for(j=i=0;j<ntr && i<c->db->n.po;i++){
		/*                  */
		/* get field index: */
		/*                  */
		ii=c->db->fi->p[i];
		
		if(  c->db->ff[ii]&PHENOTYPESET  &&
		   !(c->db->ff[ii]&COVARIATESET)
		){
		
			fprintf(c->streams.fp,"0 2 # %s\n",c->db->FieldName(c->db,ii));
			/* to maintain consistency with LINKAGE format, need five */
			/* blank lines after each of these:                       */
			fprintf(c->streams.fp,"\n\n\n\n\n");		
			j++;
		}
	
	}
	
	/*                                  */
	/* Print out covariate information: */
	/*                                  */
	for(j=i=0;j<ncv && i<c->db->n.po;i++){
		/*                  */
		/* get field index: */
		/*                  */
		ii=c->db->fi->p[i];
			
		if(c->db->ff[ii]&OUTPUTSET    &&
		   c->db->ff[ii]&COVARIATESET
		){
		
			fprintf(c->streams.fp,"4 0 # %s\n",c->db->FieldName(c->db,ii));		
			j++;
		}
	
	}

	
	fprintf(c->streams.fp,"0 0\n"); /* don't know what these are for */
	/*                                                               */
	/*                                                               */

	/*                                                   */
	/* Print out inter-marker distances in CentiMorgans: */
	/*                                                   */
	for(lpos=0.0,i=0;i<c->db->n.go;i++){
		/*                      */
		/* Get the field index: */
		/*                      */
		ii=c->db->fi->g[i];
		/*                      */
		/* Lookup map position: */
		/*                      */
		mp=LookupMarkerMap(c->mp,c->db->FieldName(c->db,ii));
		/*                     */
		/* check against NULL: */
		/*                     */
		if(!mp){
			GenericLocusFooterProcessor(c);
			MadelineErrorReport("OutputGenehunterLocusFileHeader():\n\tNo map information available for %s",c->db->FieldName(c->db,ii));
		}
		/*                                         */
		/* print out intermarker distances as soon */
		/* as the second marker map is seen:       */
		/*                                         */
		if(i){
			/*                                                      */
			/* now printing distances with greater precision:       */
			/* This is being done so that markers which map with no */
			/* recombination distance between them can be marked    */
			/* as having a very small distance between them:        */
			/*                                                      */
			/* -- previously had 6.2lf:                             */
			/*                                                      */
			fprintf(c->streams.fp,"%7.3f ",
				mp->ps-lpos);
		}
		/*                                */
		/* save the most recent position: */
		/*                                */
		lpos=mp->ps;
	}

		
	/*                                                                          */
	/* note carriage return, then magic(?) numbers:                             */
	/*                                                                          */
	fprintf(c->streams.fp,"\n1 0.1 0.45\n");         /* dummied who knows what? */

	/*                    */
	/* close file stream: */
	/*                    */
	GenericLocusFooterProcessor(c);
}


/*                          */
/*                          */
/* OutputGenehunterMarker() */
/*                          */
/*                          */
void OutputGenehunterMarker(PCONTROL c,int i){

	int ii;
	/*                                               */
	/* i is now just the rank, not the actual index. */
	/* Get the field index:                          */
	/*                                               */
	ii=c->db->fi->g[i];	

	fprintf(c->streams.fp,"3 %3i # %s\n",c->db->m[ii].n,c->db->FieldName(c->db,ii));
	
	TraverseAllele3(c->streams.fp,c->db->m+ii,c->db->m[ii].a,c->db->m[ii].s,OutputGenehunterMarkerAllele);
}

/*                                              */
/*                                              */
/* OutputGenehunterMarkerAllele: generic format */
/*                                              */
void OutputGenehunterMarkerAllele(FILE *fp,PMARKER m,PALLELE a,int s){

	fprintf(fp,"%6.4f%s",
		(double)a->n/(double)s,
		m->r-1==a->i?"\n":" "
	);
}


/*                                   */
/* GENEHUNTER data header processor: */
/*                                   */
/*                                   */
void GenehunterDataHeaderProcessor(PCONTROL c,char *fn){
	
	extern VARIABLE gfnAffected;
	extern VARIABLE gOutputFile;
	extern VARIABLE gParameterOutputFile;
	extern VARIABLE OffEndDistance;
	extern VARIABLE EvaluationInterval;
	
	char hold[MINIMUMBUFFERSIZE];
	char *s;
	char *h;
	
	/*                                                    */
	/* Affection status field is required for Genehunter: */
	/*                                                    */
	if(gfnAffected.i==MISSING)
		MadelineErrorReport("GenehunterDataHeaderProcessor():\naffection status field is required for the Genehunter format");

	/*                         */
	/* Store output file name: */
	/*                         */
	StoreString(&gOutputFile,fn);
	/*                                       */
	/* we also write to a .ctl control file: */
	/*                                       */
	StoreString(&gParameterOutputFile,fn);

	/*                                                         */
	/* change ending to .ctl by overwriting last 4 characters: */
	/*                                                         */
	strncpy(gParameterOutputFile.e.s+strlen(gParameterOutputFile.e.s)-4,
	        ".ctl",4);

	/*                                         */
	/* Then open up the .ctl file for writing: */
	/*                                         */
	c->streams.fpar=fopen(gParameterOutputFile.e.s,"wt");
	if(!c->streams.fpar)
		MadelineErrorReport("GenehunterDataHeaderProcessor(): unable to open \"%s\" for output",gParameterOutputFile.e.s);

	lprintf("Creating associated Genehunter control file called \"%s\"\n",gParameterOutputFile.e.s);

	/*                                         */
	/* get the core file name minus extension: */
	/*                                         */
	for(h=hold,s=gOutputFile.e.s;*s && *s!='.';*h++=*s++);
	*h++='.';
	*h='\0';

	if(c->OutputFormat==GHQTL){
		/*                                                          */
		/* Write control file defaults for a complete QTL analysis: */
		/*                                                          */
		fprintf(c->streams.fpar,"time\n");		
		fprintf(c->streams.fpar,"haplotype off\n");
		fprintf(c->streams.fpar,"score all\n");
		fprintf(c->streams.fpar,"ps on\n");
		fprintf(c->streams.fpar,"off end %f\n",
			OffEndDistance.e.v
		);
		fprintf(c->streams.fpar,"increment distance %f\n",
			EvaluationInterval.e.v
		);
		/*                                        */
		/* pairs used command and response option */
		/* for all possible indep. pairs:         */
		/*                                        */
		fprintf(c->streams.fpar,"pairs used\n");
		fprintf(c->streams.fpar,"3\n");
		/*                         */
		/* load and scan commands: */
		/*		                       */
		fprintf(c->streams.fpar,"load %sloc\n",hold);
		fprintf(c->streams.fpar,"scan %sped\n",hold);		
		/*                                          */
		/* Means by sex command and response option */
		/* for no gender-specific means:            */
		/*                                          */
		fprintf(c->streams.fpar,"means by sex\n");		
		fprintf(c->streams.fpar,"1\n");
		/*                                               */
		/* haseman elston regression and response option */
		/* for first phenotype variable by default:      */
		/*                                               */
		fprintf(c->streams.fpar,"haseman elston\n");
		fprintf(c->streams.fpar,"1\n");
		fprintf(c->streams.fpar,"%she_trad.out\n",hold);
		fprintf(c->streams.fpar,"%she_em.out\n",hold);
		fprintf(c->streams.fpar,"%she.ps\n",hold);
		/*                                                     */
		/* variance components and response option             */
		/* for analysis of first phenotype by default,         */
		/* no for dominance variance, and no for QTL dominance */
		/* variance, the three file names, then no for         */
		/* choosing a start value:                             */
		/*                                                     */
		fprintf(c->streams.fpar,"variance components\n");
		fprintf(c->streams.fpar,"1\n");
		fprintf(c->streams.fpar,"n\n");
		fprintf(c->streams.fpar,"n\n");
		fprintf(c->streams.fpar,"%svc.out\n",hold);
		fprintf(c->streams.fpar,"%svc_lod.ps\n",hold);
		fprintf(c->streams.fpar,"%svc_var.ps\n",hold);
		fprintf(c->streams.fpar,"n\n");
		/*                  */
		/* quit genehunter: */
		/*                  */
		fprintf(c->streams.fpar,"quit\n");
	
	}else{	
		/*                                          */
		/* Write control file defaults for          */
		/* a parametric or non-parametric analysis: */
		/*                                          */
		
		fprintf(c->streams.fpar,"time\n");		
		fprintf(c->streams.fpar,"haplotype off\n");
		fprintf(c->streams.fpar,"score all\n");
		fprintf(c->streams.fpar,"ps on\n");
		fprintf(c->streams.fpar,"off end %f\n",
			OffEndDistance.e.v
		);
		fprintf(c->streams.fpar,"increment distance %f\n",
			EvaluationInterval.e.v
		);
		/*                         */
		/* load and scan commands: */
		/*		                       */
		fprintf(c->streams.fpar,"load %sloc\n",hold);
		fprintf(c->streams.fpar,"scan %sped\n",hold);
		/*                                                 */
		/* total stat command and responses for postscript */
		/* file output:                                    */
		/*                                                 */
		fprintf(c->streams.fpar,"total stat\n");
		fprintf(c->streams.fpar,"%snpl.ps\n",hold);
		fprintf(c->streams.fpar,"%slod.ps\n",hold);
		fprintf(c->streams.fpar,"%sinf.ps\n",hold);
		/*                  */
		/* quit genehunter: */
		/*                  */
		fprintf(c->streams.fpar,"quit\n");

	}

	/*                                  */
	/* Call the "base class" processor: */
	/*                                  */
	GenericDataHeaderProcessor(c,fn);
}


/*                   */
/* GenehunterOutput: */
/*                   */
void GenehunterOutput(PCONTROL c,PPEDIGREE pd){

	/*                                           */
	/* clear has data flag:                      */
	/*                                           */
	/*TraverseIndividualBSTClearDataFlag(pd->h); */

	/*                                                            */
	/* Check all individuals for data under the current criteria: */
	/*                                                            */
	TraverseIndividualBSTMarkDataFlag(pd->h,GeneralCaseIndividualHasData);

	/*                                                             */
	/* return if sufficient data is not available in the pedigree: */
	/*                                                             */
	/*                                                             */
	if(!GenehunterPedigreeHasData(c,pd)) return;
	
	/*                  */
	/* Output the data: */
	/*                  */
	TraverseIndividualBST4(c,pd,pd->h,OutputGenehunterIndividualRecord);
	
}


/*                                                                             */
/*                                                                             */
/* OutputGenehunterIndividualRecord()                                          */
/*                                                                             */
/* The Genehunter format looks a lot like Crimap format,                       */
/* and so, not surprisingly, this function looks not too unlike                */
/* the OutputCrimapIndividual() function, including using                      */
/* #define CRIMAPALLELEFIELDWIDTH 3 to specify the number of character columns */
/* in which the allele labels are printed:                                     */
/*                                                                             */
void OutputGenehunterIndividualRecord(PCONTROL c,PPEDIGREE pd,PINDIVIDUAL p){
	
	extern VARIABLE gfnID;
	extern VARIABLE gfnLClass;
	
	int i,ii,j;
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
	   	
	/*                                          */
	/* first output id,mother's id,father's id: */
	/*                                          */
	
	/*              */
	/* Pedigree ID: */
	/*              */
	fprintf(c->streams.fp,"%s ",pd->fid);
	
	/*                        */
	/* Find out the ID width: */
	/*                        */
	/*                        */
	w=c->db->FieldLength(c->db,gfnID.i);
		
	/*           */
	/* Study ID: */
	/*           */
	fprintf(c->streams.fp,"%s ",p->id);
	
	/*         */
	/* father: */
	/*         */
	if(!p->fid[0] || p->fid[0]==CHARMISSINGVALUE)
		fprintf(c->streams.fp,"%-*.*s ",w,w,"0");
	else
		fprintf(c->streams.fp,"%s ",p->fid);     /* father */
	/*                                                  */
	/* mother:                                          */
	/*                                                  */
	if(!p->mid[0] || p->mid[0]==CHARMISSINGVALUE)
		fprintf(c->streams.fp,"%-*.*s ",w,w,"0");
	else
		fprintf(c->streams.fp,"%s ",p->mid);     /* mother */
	/*                                                  */
	/* sex:                                             */
	/*                                                  */
	fprintf(c->streams.fp,"%c ",p->female==MISSING?'0':(p->female?'2':'1'));

	/*                   */
	/* affection status: */
	/*                   */
	fprintf(c->streams.fp,"%c ",p->affected==MISSING?'0':(p->affected?'2':'1'));
		
	/*                                                            */
	/* liability class (OPTIONAL) -specified in marker data file: */
	/*                                                            */
	if(gfnLClass.i!=MISSING){
		if(p->db){
			v=p->db->ReadDouble(p->db,gfnLClass.i,p->rci);
			/*                     */
			/* if(IsNMV(v)) v=0.0; */
			/*                     */
			if(v==DOUBLEMISSINGVALUE) v=0.0;
			
		}else{
			v=0.0;
		}
		/*                                */
		/* print out the liability class: */
		/*                                */
		fprintf(c->streams.fp,"%2.0f ",v);
	}

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
				fprintf(c->streams.fp,"%*i %*i  ",
					CRIMAPALLELEFIELDWIDTH,
					0,
					CRIMAPALLELEFIELDWIDTH,
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
				if(nallele1==nallele2){
					nallele2=nallele1=GetAlleleRank(p->db->m+ii,nallele1);
				}else{
					nallele1=GetAlleleRank(p->db->m+ii,nallele1);
					nallele2=GetAlleleRank(p->db->m+ii,nallele2);
				}
				/*            */
				/* and print: */
				/*            */
				fprintf(c->streams.fp,"%*i %*i  ",
					CRIMAPALLELEFIELDWIDTH,
					nallele1,
					CRIMAPALLELEFIELDWIDTH,
					nallele2
				);
			}
		}
		/*                                     */
		/* Now print PHENOTYPE AND COVARIATE   */
		/* FIELDS for new GH 2.0 QTL analysis: */
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
							/* Print a dash to represent a missing value: */
							/*                                            */
							fprintf(c->streams.fp,"%*.*s ",wide,wide,"-");
							
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
							/* Print a dash to represent a missing value: */
							/*                                            */
							fprintf(c->streams.fp,"%*.*s ",wide,wide,"-");
							
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
			fprintf(c->streams.fp,"%*.*s %*.*s  ",
				CRIMAPALLELEFIELDWIDTH,
				CRIMAPALLELEFIELDWIDTH,
				allele1,
				CRIMAPALLELEFIELDWIDTH,
				CRIMAPALLELEFIELDWIDTH,
				allele2
			);
		}
		/*                                                         */
		/* Now print PHENOTYPE FIELDS for new GH 2.0 QTL analysis: */
		/*                                                         */
		
		if(c->db->n.po){
			/*                                                                        */
			/*                                                                        */
			/*                                                                        */
			v=pNmv->e[1].v; /* use value in cell 1 of NMV for missing value indicator */
			/*                                                                        */
			/*                                                                        */
			/* TRAITS:                                                                */
			/*                                                                        */
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
						/* Print a dash to represent a missing value: */
						/*                                            */
						fprintf(c->streams.fp,"%*.*s ",wide,wide,"-");
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
						/* Print a dash to represent a missing value: */
						/*                                            */
						fprintf(c->streams.fp,"%*.*s ",wide,wide,"-");
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



