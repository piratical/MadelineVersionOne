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
/* "generic_format.c" is part of the Madeline software distribution.                 */
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
/* generic_format.c                                                         */
/*                                                                          */
/* history: 2003.02.27.ET initiated                                         */
/*                                                                          */
/*                                                                          */

#include "generic_format.h"
#include "logs.h"
#include "dataevaluation.h"

/*                                 */
/* GENERIC LocusHeaderProcessor(): */
/*                                 */
/*                                 */
void GenericLocusHeaderProcessor(PCONTROL c,char *fn){

	extern VARIABLE gOutputFile;
	
	/*                       */
	/* open file for output: */
	/*                       */
	c->streams.fp=fopen(fn,"wt");
	if(!c->streams.fp)
		MadelineErrorReport("GenericLocusHeaderProcessor(): unable to open \"%s\" for locus file output",fn);
	/*                                                   */
	/* store name of output file if opened successfully: */
	/*                                                   */
	StoreString(&gOutputFile,fn);
}

/*                                 */
/* GENERIC LocusFooterProcessor(): */
/*                                 */
/*                                 */
void GenericLocusFooterProcessor(PCONTROL c){

	fclose(c->streams.fp);
	c->streams.fp=NULL;

}

/*                                             */
/*                                             */
/* OutputGenericMarker()                       */
/*                                             */
/* - i is the marker rank, not the true index: */
/*                                             */
void OutputGenericMarker(PCONTROL c,int i){

	int ii;

	/*                      */
	/* Get the field index: */
	/*                      */
	ii=c->db->fi->g[i];
	
	fprintf(c->streams.fp,"\n\n%s has %3i alleles:\n",c->db->FieldName(c->db,ii),c->db->m[ii].n);
	if(c->db->m[ii].nmin){
		fprintf(c->streams.fp,
		        "WARNING: %s has mendelian inheritance inconsistencies in %i nuclear families in %i pedigrees\n",
		        c->db->FieldName(c->db,ii),
		        c->db->m[ii].nmin,
		        c->db->m[ii].nmip
		);
	}
	
	fprintf(c->streams.fp,"\n");
	TraverseAllele2(c->streams.fp,c->db->m[ii].a,c->db->m[ii].s,OutputGenericMarkerAllele);
}

/*                                           */
/*                                           */
/* OutputGenericMarkerAllele: generic format */
/*                                           */
void OutputGenericMarkerAllele(FILE *fp,PALLELE a,int s){

	fprintf(fp,"%3i. %4i %5i/%6i = %6.4f\n",a->i,a->v,a->n,s,(double)a->n/(double)s);

}



/*                                            */
/*                                            */
/* GENERIC data header and footer processors: */
/*                                            */
/*                                            */
void GenericDataHeaderProcessor(PCONTROL c,char *fn){

	extern VARIABLE gOutputFile;
	/*                                      */
	/* open primary output file for output: */
	/*                                      */
	c->streams.fp=fopen(fn,"wt");
	if(!c->streams.fp)
		MadelineErrorReport("GenericDataHeaderProcessor(): unable to open \"%s\" for output",fn);
	/*                                                   */
	/*                                                   */
	/* store name of output file if opened successfully: */
	/*                                                   */
	StoreString(&gOutputFile,fn);

	/*                             */
	/* reset the control counters: */
	/*                             */
	ResetCounters(c);
	
	/*                                */
	/* tell the user what's going on: */
	/*                                */
	lprintf("Writing pedigree data to \"%s\"\n",fn);
}

/*                               */
/* GenericDataFooterProcessor(): */
/*                               */
void GenericDataFooterProcessor(PCONTROL c){

	/*                  */
	/* close data file: */
	/*                  */
	if(c->streams.fp){
		fclose(c->streams.fp);
		c->streams.fp=NULL;
	}

	/*                       */
	/* close parameter file: */
	/*                       */
	if(c->streams.fpar){
		fclose(c->streams.fpar);
		c->streams.fpar=NULL;
	}
	/*                                          */
	/* close alternate parameter file, if open: */
	/*                                          */
	if(c->streams.fpar2){
		fclose(c->streams.fpar2);
		c->streams.fpar2=NULL;
	}
}


