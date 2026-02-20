

#include "showstatistics.h"
#include "logs.h"

/*                                                                    */
/* ShowPedigreeStatistics(): show the number of included and excluded */
/* pedigrees and individuals for pedigree-based output and other      */
/* pedigree-block based manipulations:                                */
/*                                                                    */
void ShowPedigreeStatistics(PCONTROL c){

	ShowSummaryStatisticsTable(&c->counters,NULL,GENERALSTATS);	
	/*                                                                */
	/* after each ShowStatistics(), reset c->OutputFormat to generic: */
	/*                                                                */
	c->OutputFormat=0;

}


/*                                                                       */
/* ShowSummaryStatisticsTable():                                         */
/*                                                                       */
/* --> flag can be either OPENSTATS,GENERALSTATS, or NUCLEARSTATS        */
/*                                                                       */
/* --> GENERALSTATS applies to most WRITE formats.                       */
/*                                                                       */
/* --> When OPENSTATS is used, the three sub-categories of "With Data",  */
/*     "Without Data" and "Marked for Exclusion" are NOT shown.  This is */
/*     used when the OPEN command is called.                             */
/*                                                                       */
/* --> When NUCLEARSTATS is selected, the additional information about   */
/*     number of sibpairs is shown.  This applies when a WRITE format    */
/*     like SIBLINK is called.                                           */
/*                                                                       */
void ShowSummaryStatisticsTable(PCORECOUNTERS cc,char *msg,int flag){

	char h1[10];
	char h2[10];
	char h3[10];

	if(msg) lprintf("\n%s\n",msg);
	lprintf("-----------------------------  --------- --------- ---------\n");
	lprintf("Pedigrees and Individuals       Included  Excluded     Total\n");
	lprintf("-----------------------------  --------- --------- ---------\n");
	/*lprintf("\n"); */
	lprintf("Pedigrees ...................  %s %s %s\n",
		Itoa(h1,cc->pedg.in,9),
		Itoa(h2,cc->pedg.ex,9),
		Itoa(h3,cc->pedg.in+cc->pedg.ex,9)
	);
	lprintf("Individuals .................  %s %s %s\n",
		Itoa(h1,cc->din.in+
		        cc->dno.in+
		        cc->dex.in+
		        cc->csp.in+
		        cc->flt.in+
		        cc->vir.in,9),
		Itoa(h2,cc->din.ex+
		        cc->dno.ex+
		        cc->dex.ex+
		        cc->csp.ex+
		        cc->flt.ex+
		        cc->vir.ex,9),
		Itoa(h3,cc->din.in+
		        cc->din.ex+
		        cc->dno.in+
		        cc->dno.ex+
		        cc->dex.in+
		        cc->dex.ex+
		        cc->csp.in+
		        cc->csp.ex+
		        cc->flt.in+
		        cc->flt.ex+
		        cc->vir.in+
		        cc->vir.ex,9)
	);
	lprintf(" + In database ..............  %s %s %s\n",
		Itoa(h1,cc->din.in+
		        cc->dno.in+
		        cc->dex.in+
		        cc->csp.in+
		        cc->flt.in,9),
		Itoa(h2,cc->din.ex+
		        cc->dno.ex+
		        cc->dex.ex+
		        cc->csp.ex+
		        cc->flt.ex,9),
		Itoa(h3,cc->din.in+
		        cc->din.ex+
		        cc->dno.in+
		        cc->dno.ex+
		        cc->dex.in+
		        cc->dex.ex+
		        cc->csp.in+
		        cc->csp.ex+
		        cc->flt.in+
		        cc->flt.ex,9)
	);
	lprintf(" |  + Attached ..............  %s %s %s\n",
		Itoa(h1,cc->din.in+
		        cc->dno.in+
		        cc->dex.in,9),
		Itoa(h2,cc->din.ex+
		        cc->dno.ex+
		        cc->dex.ex,9),
		Itoa(h3,cc->din.in+
		        cc->din.ex+
		        cc->dno.in+
		        cc->dno.ex+
		        cc->dex.in+
		        cc->dex.ex,9)
	);
	/*                                                    */
	/* "Attached" subcategories are printed except when a */
	/* database has just been OPENed -- the categories    */
	/* have no meaning in that context.                   */
	/*                                                    */
	if(flag!=OPENSTATS){
		lprintf(" |  |  + With data ..........  %s %s %s\n",
			Itoa(h1,cc->din.in,9),
			Itoa(h2,cc->din.ex,9),
			Itoa(h3,cc->din.in+
			        cc->din.ex,9)
		);
		lprintf(" |  |  + Without data .......  %s %s %s\n",
			Itoa(h1,cc->dno.in,9),
			Itoa(h2,cc->dno.ex,9),
			Itoa(h3,cc->dno.in+
			        cc->dno.ex,9)
		);
		lprintf(" |  |  + Marked for exclusion  %s %s %s\n",
			Itoa(h1,cc->dex.in,9),
			Itoa(h2,cc->dex.ex,9),
			Itoa(h3,cc->dex.in+
			        cc->dex.ex,9)
		);
	}
	lprintf(" |  + Childless spouses .....  %s %s %s\n",
		Itoa(h1,cc->csp.in,9),
		Itoa(h2,cc->csp.ex,9),
		Itoa(h3,cc->csp.in+
		        cc->csp.ex,9)
	);
	lprintf(" |  + Unattached ............  %s %s %s\n",
		Itoa(h1,cc->flt.in,9),
		Itoa(h2,cc->flt.ex,9),
		Itoa(h3,cc->flt.in+
		        cc->flt.ex,9)
	);
	lprintf(" + Not in database ..........  %s %s %s\n",
		Itoa(h1,cc->vir.in,9),
		Itoa(h2,cc->vir.ex,9),
		Itoa(h3,cc->vir.in+
		        cc->vir.ex,9)
	);
	/*                                                         */
	/* Show sib pairs if it's a nuclear-pedigree-based method: */
	/*                                                         */
	if(flag==NUCLEARSTATS){
		lprintf("-----------------------------  --------- --------- ---------\n");
		lprintf("Number of Sibpairs ..........  %s %s %s\n",
			Itoa(h1,cc->sibp.in,9),
			Itoa(h2,cc->sibp.ex,9),
			Itoa(h3,cc->sibp.in+
			        cc->sibp.ex,9)
		);
		lprintf("-----------------------------  --------- --------- ---------\n");
		
	}

}



/*                                                                    */
/* ShowNuclearStatistics(): Statistics appropriate for nuclear-family */
/* based output formats:                                              */
/*                                                                    */
void ShowNuclearStatistics(PCONTROL c){

	ShowSummaryStatisticsTable(&c->counters,"ACTUAL:",GENERALSTATS);
	ShowSummaryStatisticsTable(&c->nuclear,"NUCLEAR FAMILY-BASED:",NUCLEARSTATS);
	/*                                                                */
	/* after each ShowStatistics(), reset c->OutputFormat to generic: */
	/*                                                                */
	c->OutputFormat=0;
}


/*                                                                                */
/* ShowDatabaseStatistics(): show the number of included and excluded             */
/* pedigrees and individuals when a database is first opened.                     */
/* This is very similar to ShowPedigreeStatistics(), except that when a           */
/* database is first opened, the "Has Data","No Data", and "Marked for exclusion" */
/* categories have no meaning and need not be shown:                              */
/*                                                                                */
void ShowDatabaseStatistics(PCONTROL c){


	ShowSummaryStatisticsTable(&c->counters,NULL,OPENSTATS);
	/*                                                                */
	/* after each ShowStatistics(), reset c->OutputFormat to generic: */
	/*                                                                */
	c->OutputFormat=0;

}
