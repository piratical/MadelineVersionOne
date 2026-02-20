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
/* "mendel_format.c" is part of the Madeline software distribution.                  */
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
/* mendel_format.c                                                          */
/*                                                                          */
/* history: 2003.02.27.ET initiated                                         */
/*                                                                          */
/*                                                                          */

#include "mendel_format.h"
#include "generic_format.h"
#include "stringbuffersizes.h"
#include "dataevaluation.h"
#include "traversal.h"

/*                                               */
/* Mendel output missing value is blank, so here */
/* we've got one that's                          */
/* MAXIMUMMENDELFIELDWIDTH in width:             */
/*                                               */
char gMendelOutputMissingValue[]="        ";

/*                                                      */
/*                                                      */
/* OutputMendelMarker()                                 */
/*                                                      */
/* i is now the rank index, not the actual field index: */
/*                                                      */
void OutputMendelMarker(PCONTROL c,int i){

	int ii;
	
	/*                             */
	/* get the actual field index: */
	/*                             */
	ii=c->db->fi->g[i];

	fprintf(c->streams.fp,"%-8.8sAUTOSOME%2i 0\n",c->db->FieldName(c->db,ii),c->db->m[ii].n);
	
	TraverseAllele2(c->streams.fp,c->db->m[ii].a,c->db->m[ii].s,OutputMendelMarkerAllele);

}

/*                                          */
/*                                          */
/* OutputMendelMarkerAllele: generic format */
/*                                          */
void OutputMendelMarkerAllele(FILE *fp,PALLELE a,int s){

	fprintf(fp,"%3i     %6.4f\n",a->v,(double)a->n/(double)s);

}


/*                                                      */
/*                                                      */
/* MendelDataHeaderProcessor(): Used for Generic Mendel */
/*                                                      */
/*                                                      */
void MendelDataHeaderProcessor(PCONTROL c, char *fn){
	
	int i,ii;
	int w; /* output field width: adjusted, if necessary */


	/*                                                                    */
	/* call generic to open output stream, store file name, reset control */
	/* counters, and inform user:                                         */
	/*                                                                    */
	GenericDataHeaderProcessor(c,fn);
	
	/* PRINT FORMAT FOR PEDIGREE HEADER RECORD: */
	fprintf(c->streams.fp,"(I3,1X,A8)\n");
	/* PRINT FORMAT FOR STUDYID,FATHER,MOTHER,GENDER,MZTWIN: */
	fprintf(c->streams.fp,"(3(A8,1X),2(A1,1X)");

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
/* MendelOutput(): outputs data for Generic Mendel: */
/*                                                  */
/* Used for: Generic Mendel and Relpair outputs     */
/*                                                  */
void MendelOutput(PCONTROL c,PPEDIGREE pd){

	int noutput;	


	/*                          */
	/* Generic MENDEL, RELPAIR: */
	/*                          */

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
		TraverseIndividualBST2(c,pd->h,OutputMendelIndividualRecord);
	}
	

}


/*                                     */
/*                                     */
/* OutputMendelIndividualRecord()      */
/*                                     */
/* -> Can be used for Mendel & Fisher: */
/*                                     */
void OutputMendelIndividualRecord(PCONTROL c,PINDIVIDUAL p){

	int i,ii;
		
	
	if(ExcludedForMendel(p)) return;

		
	/*                                       */
	/* Write core fixed fields (COREOUTSET): */
	/*                                       */
	fprintf(c->streams.fp,"%-8.8s %-8.8s %-8.8s %c %c ",
		p->id,
		p->fid[0]==CHARMISSINGVALUE?gMendelOutputMissingValue:p->fid,
		p->mid[0]==CHARMISSINGVALUE?gMendelOutputMissingValue:p->mid,
		p->female?'F':'M',
		p->twin && isupper(p->twin)?p->twin:' '
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


/*                                                      */
/* OutputMendelField():                                 */
/*                                                      */
/* c: pointer to CONTROL structure                      */
/* p: pointer to INDIVIDUAL                             */
/* i: field index in database                           */
/* j: flag, 1 for genotype field, 0 for phenotype field */
/*                                                      */
void OutputMendelField(PCONTROL c,PINDIVIDUAL p,int i,int j){

	int wide,prec;
	char ty;
	char hold[MINIMUMBUFFERSIZE];
	char *o;
	double v;

	wide=j?MENDELGENOTYPEFIELDWIDTH:c->db->FieldLength(c->db,i);
	if(wide>MAXIMUMMENDELFIELDWIDTH) wide=MAXIMUMMENDELFIELDWIDTH;
	
	if(p->db && !p->excluded){
		/*                     */
		/* person in database: */
		/* read data:          */
		/*                     */
		if((ty=p->db->FieldType(p->db,i))=='C'){
			/*                             */
			/* read character string data: */
			/*                             */
			o=p->db->ReadString(p->db,hold,i,p->rci);
			/*                                                               */
			/* check for missing values:                                     */
			/*                                                               */
			/* if(IsCMV(o)) o=gMendelOutputMissingValue;                     */
			/*                                                               */
			/* if(!strcmp(o,CHARMISSINGSTRING)) o=gMendelOutputMissingValue; */
			/*                                                               */
			if(!o[0]) o=gMendelOutputMissingValue;
			
			/*                       */
			/* write to output file: */
			/*                       */
			fprintf(c->streams.fp,"%-*.*s ",wide,wide,o);
				
		}else if(ty=='N'){
			/*                    */
			/* read numeric data: */
			/*                    */
			v=p->db->ReadDouble(p->db,i,p->rci);
			/*                           */
			/* check for missing values: */
			/*                           */
			/* if(IsNMV(v)){             */
			/*                           */
			if(v==DOUBLEMISSINGVALUE){
				fprintf(c->streams.fp,"%-*.*s ",wide,wide,gMendelOutputMissingValue);
			}else{
				/*                                       */
				/* check precision (adjust if too wide): */
				/*                                       */
				prec=p->db->FieldPrecision(p->db,i);
				if(wide==MAXIMUMMENDELFIELDWIDTH && prec>HALFMAXMENDELFIELDWIDTH)
					prec=HALFMAXMENDELFIELDWIDTH;
					
				fprintf(c->streams.fp,"%s ",FormatNumber(v,NULL,wide,prec));
			}
		}

	}else{
		/*                                                                           */
		/* person is dummied-in or else they are non-terminal excluded:              */
		/*                                                                           */
		/* Some formats, like basic MENDL, can just terminate here with a CR.        */
		/* However, others, like UM13, require an additional column after everything */
		/* else, so we have to write out blanks for each field:                      */
		/*                                                                           */
		fprintf(c->streams.fp,"%-*.*s ",wide,wide,gMendelOutputMissingValue);
	}
}



