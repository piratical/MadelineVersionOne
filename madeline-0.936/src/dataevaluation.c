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
/* "dataevaluation.c" is part of the Madeline software distribution.                 */
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


/*                                                            */
/* START OF REVISED BOOKKEEPING AND DATA EVALUATION FUNCTIONS */
/*                                                            */

#include "dataevaluation.h"

#include "knowntokens.h"
#include "logs.h"
#include "traversal.h"

/*                                                                               */
/* GeneralCaseIndividualHasData() returns 1 if the individual:                   */
/*                                                                               */
/*      (1) Is in the database                                                   */
/*  AND (2) Is not marked for exclusion                                          */
/*  AND (3) Is not a childless spouse or unconnected who will be excluded anyway */
/*  AND (5) Has fields marked for output that are not core fields                */
/*  AND (6) If he has genotype fields, then at least one is non-missing          */
/*                                                                               */
/* 2004.07.09.ET ADDENDUM NOTE BENE: This function previously also required that */
/* if an individual had phenotype fields, then the phenotype fields were all non-*/
/* missing.  I don't know what the rationale for that was, but it made little    */
/* sense.  Changed now so that phenotype fields can all be missing               */
/*                                                                               */
int GeneralCaseIndividualHasData(PINDIVIDUAL p){



	extern PCONTROL g;
	
	/*                                                */
	/* Person "has data" (in a logical sense only) if */
	/* "AutoExclude" is off:                          */
	/*                                                */
	if(!g->AutoExclude) return 1;

	/*                                                             */
	/* Person definitely doesn't have data if not in the database: */
	/*                                                             */
	if(!p->db) return 0;
	
	/*                                                                 */
	/* Person doesn't "have data" if they are supposed to be excluded: */
	/*                                                                 */
	if(p->excluded) return 0;

	/*                                                             */
	/* Person doesn't "have data" if they're a childless spouse or */
	/* otherwise unnattached who has to be excluded anyway:        */
	/*                                                             */
	if(p->csp || p->unconnected) return 0;

	/*                                                            */
	/* No data if there are no output fields above and beyond the */
	/* core output field set:                                     */
	/*                                                            */
	if(p->db->n.o==p->db->n.co) return 0;           

	/*                                                      */
	/* Check the selected set of phenotype fields, if any:  */
	/*                                                      */
	/* The call to HasPhenotypeData() is still here, but it */
	/* Now always returns TRUE:                             */
	
	//if(p->db->n.po && !HasPhenotypeData(p)){
	//	dprintf("Individual %s in pedigree %s has a missing value for one or more phenotype variables\n",p->id,p->pd->fid);
	//	return 0;
	//}
	
	/*                                                    */
	/* Check the selected set of genotype fields, if any: */
	/*                                                    */
	if(p->db->n.go && !HasGenotypeData(p)){
		dprintf("Individual %s in pedigree %s has missing values for all genotype variables\n",p->id,p->pd->fid);
		return 0;
	}
	/*                                    */
	/* get here if everything appears OK: */
	/*                                    */
	return 1;
}



/*                                                                        */
/* MendelIndividualHasData() returns 1 if the individual:                 */
/*                                                                        */
/*      (1) Is in the database                                            */
/*  AND (2) Is not marked for exclusion                                   */
/*  AND (3) Has fields marked for output that are not core fields         */
/*  AND (4) If he has phenotype fields, then they are all non-missing     */
/*  AND (5) If he has genotype fields, then at least one is non-missing   */
/*                                                                        */
/*  AND (6) He or she is NOT MISSING GENDER because Mendel-based programs */
/*          can't deal with missing gender.                               */
/*                                                                        */
int MendelIndividualHasData(PINDIVIDUAL p){


	/* extern PCONTROL g; */
	
	/*                                                                  */
	/* For Mendel-based formats, can't use individuals without          */
	/* gender (such individuals have to be sampled terminal individuals */
	/* without offspring):                                              */
	/*                                                                  */
	if(p->female==MISSING)
		return 0;
	else
		return GeneralCaseIndividualHasData(p);
}


/*                                                                    */
/* FisherNonProbandHasData() returns 1 if the individual:             */
/*                                                                    */
/*     ...is NOT the proband                                          */
/*  AND...is in the database                                          */
/*  AND...Has fields marked for output that are not core fields       */
/*  AND...If he has phenotype fields, then they are all non-missing   */
/*  AND...If he has genotype fields, then at least one is non-missing */
/*  AND... individual has gender marked                               */
/*                                                                    */
/*                                                                    */
int FisherNonProbandHasData(PINDIVIDUAL p){


	/*                                                    */
	/* if he's the proband, we don't count him            */
	/* because we're looking for a non-proband with data: */
	/*                                                    */
	if(p->proband)
		return 0;
	else
		return MendelIndividualHasData(p);
}

/*                                                          */
/* SiblinkIndividualHasData():                              */
/*                                                          */
/* -> For Siblink, an individual "has data" if s/he's:      */
/*    (1) affected   for SiblinkAffectedPairs format        */
/* or (1) unaffected for SiblinkUnaffectedPairs             */
/* or (1) affected or unaffected for SiblinkAllPairs        */
/* or (1) affected or unaffected for SiblinkDiscordantPairs */
/*and (2) qualifies under MendelIndividualHasData()         */
/*                                                          */
int SiblinkIndividualHasData(PINDIVIDUAL p){
	
	extern PCONTROL g;
	
	switch(g->OutputFormat){
	case SIBLK:
		return p->affected==1 && MendelIndividualHasData(p);
	case SIBLKU:
		return p->affected==0 && MendelIndividualHasData(p);
	case SIBLKA:
	case SIBLKD:
		return p->affected!=MISSING && MendelIndividualHasData(p);
	}
	return 0;
}


/*                                                                      */
/* UserM13IndividualHasData()                                           */
/*                                                                      */
/* -> Differs from the GeneralCaseIndividualHasData() in that childless */
/*    spouses and other unconnected individuals are permitted in        */
/*    output:                                                           */
/*                                                                      */
int UserM13IndividualHasData(PINDIVIDUAL p){


	extern PCONTROL g;
	
	/*                                                */
	/* Person "has data" (in a logical sense only) if */
	/* "AutoExclude" is off:                          */
	/*                                                */
	if(!g->AutoExclude) return 1;

	/*                                                             */
	/* Person definitely doesn't have data if not in the database: */
	/*                                                             */
	if(!p->db) return 0;
	
	/*                                                                 */
	/* Person doesn't "have data" if they are supposed to be excluded: */
	/*                                                                 */
	if(p->excluded) return 0;

	/*                                                            */
	/* No data if there are no output fields above and beyond the */
	/* core output field set:                                     */
	/*                                                            */
	if(p->db->n.o==p->db->n.co) return 0;           

	/*                                                     */
	/* Check the selected set of phenotype fields, if any: */
	/*                                                     */
	
	//if(p->db->n.po && !HasPhenotypeData(p)){
	//	dprintf("Individual %s in pedigree %s has a missing value for one or more phenotype variables\n",p->id,p->pd->fid);
	//	return 0;
	//}

	/*                                                    */
	/* Check the selected set of genotype fields, if any: */
	/*                                                    */
	if(p->db->n.go && !HasGenotypeData(p)){
		dprintf("Individual %s in pedigree %s has missing values for all genotype variables\n",p->id,p->pd->fid);
		return 0;
	}
	/*                                    */
	/* get here if everything appears OK: */
	/*                                    */
	return 1;
}


/*                                                         */
/* HasPhenotypeData(): returns 1 if individual             */
/* has data in ALL of the phenotype fields that are marked */
/* for output.  Thus an AND relation is used for the       */
/* selected phenotype fields:                              */
/*                                                         */
int HasPhenotypeData(PINDIVIDUAL p){

	int i,ii;
	int n;
	char hold[MINIMUMBUFFERSIZE];
	char *o;

	/*                      */
	/* prophylactic checks: */
	/*                      */
	if(!p->db || !p->db->n.po) return 0;
	/*                  */
	/* check po fields: */
	/*                  */
	for(n=i=0;i<p->db->n.po;i++){
		/*                  */
		/* get field index: */
		/*                  */
		ii=p->db->fi->p[i];
		/*                      */
		/* check fields by type */
		/*                      */
		switch(p->db->FieldType(p->db,ii)){
		case 'C':

			o=p->db->ReadString(p->db,hold,ii,p->rci);			
			if(o[0]) n++;			
			break;
			
		case 'N':

			if(p->db->ReadDouble(p->db,ii,p->rci)!=DOUBLEMISSINGVALUE) n++;
			break;
			
		case 'D':

			if(p->db->ReadDate(p->db,ii,p->rci)!=MISSINGDATE) n++;
			break;
			
		}
	}
	/*                                                         */
	/* number of non-missing fields must equal total number of */
	/* phenotype fields:                                       */
	/*                                                         */
	return (n==p->db->n.po);
}


/*                                                                */
/* HasGenotypeData(): returns 1 if individual has                 */
/* data in AT LEAST ONE of the genotype fields marked for output: */
/* Note that a genotype field MUST BE A CHARACTER FIELD:          */
/*                                                                */
int HasGenotypeData(PINDIVIDUAL p){

	int i,ii;
	char hold[MINIMUMBUFFERSIZE];
	char *o;

	/*                      */
	/* prophylactic checks: */
	/*                      */
	if(!p->db || !p->db->n.go) return 0;
	/*                        */
	/* check selected fields: */
	/*                        */
	for(i=0;i<p->db->n.go;i++){
		/*                  */
		/* get field index: */
		/*                  */
		ii=p->db->fi->g[i];
		o=p->db->ReadString(p->db,hold,ii,p->rci);		
		if(o[0]) return 1;
	}
	/*                                                      */
	/* get here if all the genotype output fields evaluated */
	/* to missing:                                          */
	/*                                                      */
	return 0;
}



/*/////////////////////////// */
/*/////////////////////////// */
/*                            */
/* General Case               */
/*                            */
/*/////////////////////////// */
/*/////////////////////////// */

/*                                                               */
/* GeneralCaseCountIncludedPedigreeIndividual()                  */
/*                                                               */
/*                                                               */
/* --> This function returns 1 for each __included__ individual, */
/*     zero for each __excluded__ individual.  Note that book-   */
/*     keeping on individuals is broken down by category in the  */
/*     CONTROL structure.                                        */
/*                                                               */
/* --> This function now assumes that p->hasdata has been set    */
/*                                                               */
/* --> 2000 02 28 ET ADDENDUM: Modified to correctly recognize   */
/*     when c->AutoExclude==0                                    */
/*                                                               */
int GeneralCaseCountIncludedPedigreeIndividual(PCONTROL c,PINDIVIDUAL p){

	if(p->db){
		/*              */
		/* in database: */
		/*              */
		if(p->unconnected){
			/*                                                         */
			/* In the general case, unconnected individuals are        */
			/* not included in output, regardless of the status of the */
			/* p->excluded flag:                                       */
			/*                                                         */
			if(c->AutoExclude){
				/* AutoExclude ON */
				c->counters.flt.ex++;
				return 0;
			}else{
				/* AutoExclude OFF */
				c->counters.flt.in++;
				return 1;
			}
		}else if(p->csp){
			/*                                                         */
			/* In the general case, childless spouses are              */
			/* not included in output, regardless of the status of the */
			/* p->excluded flag:                                       */
			/*                                                         */
			if(c->AutoExclude){
				/* AutoExclude ON */
				c->counters.csp.ex++;
				return 0;
			}else{
				/* AutoExclude OFF */
				c->counters.csp.in++;
				return 1;
			}
		}else{
			/*                                                */
			/* normally connected individual in the database: */
			/*                                                */
			if(p->excluded){
				/*                                    */
				/* An excluded person in the database */
				/*                                    */
				if(p->nk){
					/*                                                */
					/* Since this person has                          */
					/* __presumably__ __included__ children,          */
					/* he or she is required in output for structural */
					/* maintenance, but his/her data is to be         */
					/* excluded:                                      */
					/*                                                */
					c->counters.dex.in++;
					return 1;			
				}else{
					/*                                                */
					/* No record is required in output for            */
					/* a terminal person without offspring marked for */
					/* exclusion:                                     */
					/*                                                */
					if(c->AutoExclude){
						/* AutoExclude ON */
						c->counters.dex.ex++;
						return 0;
					}else{
						/* AutoExclude OFF */
						c->counters.dex.in++;
						return 1;
					}
				}
			}else{
				/*                                            */
				/* not an excluded individual: a regular,     */
				/* individual: classify based on whether they */
				/* have data or not:                          */
				/*                                            */
				if(p->hasdata){
					c->counters.din.in++;
					return 1;
				}else{
					if(p->nk){
						/*                                           */
						/* has __presumably__ __included__ children, */
						/* so required for structural maintenance:   */
						/*                                           */
						c->counters.dno.in++;
						return 1;
					}else{
						/*                                          */
						/* terminal without data, so don't include: */
						/*                                          */
						if(c->AutoExclude){
							/* AutoExclude ON */
							c->counters.dno.ex++;
							return 0;
						}else{
							/* AutoExclude OFF */
							c->counters.dno.in++;
							return 1;
						}
					}				
				}
			}
		}
	}else{
		/*                                                                     */
		/* virtual individual, not in database.  These individuals             */
		/* are inserted by Madeline in order to obtain proper pedigree         */
		/* structure.  They are the unsampled parents of a sibship.  Because   */
		/* they have __presumably__ __included__ __offspring__, they           */
		/* need to be included even when marked for exclusion by the user      */
		/*                                                                     */
		c->counters.vir.in++;
		return 1;
	}
}


/*                                                                     */
/* This is used to count excluded pedigree individuals:                */
/* Since everybody in an excluded pedigree is going to be              */
/* excluded, we don't need a return value -- we can just look at pd->n */
/* to find out the total number of people.                             */
/*                                                                     */
/* However, we still need this function in order to categorize people  */
/* properly for bookkeeping in the CONTROL structure                   */
/*                                                                     */
/* If AutoExclude is OFF, then this function should not be called      */
/*                                                                     */
void GeneralCaseCountExcludedPedigreeIndividual(PCONTROL c,PINDIVIDUAL p){

	if(p->db){
		/*              */
		/* in database: */
		/*              */
		if(p->unconnected){
			/*                                                         */
			/* In the general case, unconnected individuals are        */
			/* not included in output, regardless of the status of the */
			/* p->excluded flag:                                       */
			/*                                                         */
			c->counters.flt.ex++;
			return;
		}else if(p->csp){
			/*                                                         */
			/* In the general case, childless spouses are              */
			/* not included in output, regardless of the status of the */
			/* p->excluded flag:                                       */
			/*                                                         */
			c->counters.csp.ex++;
			return;
		}else{
			/*                                                */
			/* normally connected individual in the database: */
			/*                                                */
			if(p->excluded){
				/*                                                        */
				/* An excluded person in the database:                    */
				/* (Since we are excluding everyone, there's              */
				/* no such thing as a "data-excluded but person-included" */
				/* individual)                                            */
				/*                                                        */
				c->counters.dex.ex++;
				return;			
			}else{
				/*                                        */
				/* not an excluded individual: a regular, */
				/* individual:                            */
				/*                                        */
				if(p->hasdata){
					c->counters.din.ex++;
					return;
				}else{
					c->counters.dno.ex++;
					return;
				}
			}
		}
	}else{
		/*                                                                     */
		/* virtual individual, not in database.  These individuals             */
		/* are inserted by Madeline in order to obtain proper pedigree         */
		/* structure, and hence are __usually__ included in output even though */
		/* they don't have data.  In this case, the whole pedigree is          */
		/* excluded, so too them:                                              */
		/*                                                                     */
		c->counters.vir.ex++;
		return;
	}
}


/*                                                                                     */
/*                                                                                     */
/* GeneralCasePedigreeHasData(): check if pedigree has data, as                        */
/* defined below:                                                                      */
/*                                                                                     */
/* At least one individual in the pedigree must "have" data.                           */
/*                                                                                     */
/* An INDIVIDUAL has data if he...                                                     */
/*                                                                                     */
/*    ...is in the database                                                            */
/* AND...has one or more non-core output fields                                        */
/* AND...if he has phenotype fields marked for output, then they are all non-missing   */
/* AND...if he has genotype fields marked for output, then at least one is non-missing */
/*                                                                                     */
/* This function returns zero if a pedigree does not have data.  If a pedigree does    */
/* have data, this function returns the number of people in the pedigree who will      */
/* be output.                                                                          */
/*                                                                                     */
int GeneralCasePedigreeHasData(PCONTROL c,PPEDIGREE pd){

	/*                                                                        */
	/* If AutoExclude is turned off, then we are:                             */
	/* in practice saying that the pedigree has data, even though it may      */
	/* not have any in reality.  For this reason, AutoExclude should normally */
	/* be turned on:                                                          */
	/*                                                                        */
	if(!c->AutoExclude){
		/*                             */
		/* increment include counters: */
		/*		                           */
		c->counters.pedg.in++;
		return TraverseIndividualBST6(c,pd->h,GeneralCaseCountIncludedPedigreeIndividual);
	}
	/*                                                                        */
	/* General case:                                                          */
	/*                                                                        */
	/*if(TraverseIndividualBSTUntilTrue(pd->h,GeneralCaseIndividualHasData)){ */
	/*                                                                        */
	if(TraverseIndividualBSTUntilDataFound(pd->h)){
		/*                             */
		/* increment include counters: */
		/*                             */
		c->counters.pedg.in++;
		return TraverseIndividualBST6(c,pd->h,GeneralCaseCountIncludedPedigreeIndividual);

	}else{	
		/*                                                         */
		/* no data: return zero to indicate exclusion of pedigree: */
		/*                                                         */
		dprintf("GeneralCasePedigreeHasData(): excluding pedigree %s: lacks individual with data.\n",pd->fid);
		c->counters.pedg.ex++;
		TraverseIndividualBST2(c,pd->h,GeneralCaseCountExcludedPedigreeIndividual);
		return 0;
	}
}


/*                                                               */
/* MENDEL ROUTINES: Differ from GeneralCaseXXX() routines by not */
/* permitting output of individuals lacking gender:              */
/*                                                               */
/*///////////////////////////                                    */

/*/////////////////////////// */
/*                            */
/* MENDEL Case:               */
/*                            */
/*/////////////////////////// */
/*/////////////////////////// */

/*                                                               */
/* MendelCountIncludedPedigreeIndividual()                       */
/*                                                               */
/*                                                               */
/* --> This function returns 1 for each __included__ individual, */
/*     zero for each __excluded__ individual.  Note that book-   */
/*     keeping on individuals is broken down by category in the  */
/*     CONTROL structure.                                        */
/*                                                               */
int MendelCountIncludedPedigreeIndividual(PCONTROL c,PINDIVIDUAL p){

	if(p->db){
		/*              */
		/* in database: */
		/*              */
		if(p->unconnected){
			/*                                                         */
			/* In the general case, unconnected individuals are        */
			/* not included in output, regardless of the status of the */
			/* p->excluded flag:                                       */
			/*                                                         */
			if(c->AutoExclude){
				/* AutoExclude is ON */
				c->counters.flt.ex++;
				return 0;
			}else{
				/* AutoExclude is OFF */
				c->counters.flt.in++;
				return 1;
			}
		}else if(p->csp){
			/*                                                         */
			/* In the general case, childless spouses are              */
			/* not included in output, regardless of the status of the */
			/* p->excluded flag:                                       */
			/*                                                         */
			if(c->AutoExclude){
				/* AutoExclude is ON */
				c->counters.csp.ex++;
				return 0;
			}else{
				/* AutoExclude is OFF */
				c->counters.csp.in++;
				return 1;
			}
		}else{
			/*                                                */
			/* normally connected individual in the database: */
			/*                                                */
			if(p->excluded){
				/*                                    */
				/* An excluded person in the database */
				/*                                    */
				if(p->nk){
					/*                                                */
					/* Since this person has                          */
					/* __presumably__ __included__ children,          */
					/* he or she is required in output for structural */
					/* maintenance, but his/her data is to be         */
					/* excluded:                                      */
					/*                                                */
					c->counters.dex.in++;
					return 1;			
				}else{
					/*                                                */
					/* No record is required in output for            */
					/* a terminal person without offspring marked for */
					/* exclusion:                                     */
					/*                                                */
					if(c->AutoExclude){
						/* AutoExclude is ON */
						c->counters.dex.ex++;
						return 0;
					}else{
						/* AutoExclude is OFF */
						c->counters.dex.in++;
						return 1;
					}
				}
			}else{
				/*                                                    */
				/* not an excluded individual: a regular,             */
				/* individual:                                        */
				/*                                                    */
				/* this is the only case remaining where we could     */
				/* get a person without gender where it would matter  */
				/* (the other cases would be excluded already anyway, */
				/*  so gender would not make a difference):           */
				/*                                                    */
				if(p->hasdata){
					if(p->female==MISSING){
						/*                                                 */
						/* terminal individual without gender, so exclude: */
						/*                                                 */
						if(c->AutoExclude){
							/* AutoExclude is ON */
							c->counters.din.ex++;
							return 0;
						}else{
							/* AutoExclude is OFF */
							c->counters.din.in++;
							return 1;
						}
					}else{
						c->counters.din.in++;
						return 1;
					}
				}else{
					/*               */
					/* no data case: */
					/*               */
					if(p->nk){
						/*                                              */
						/* has __presumably included__ offspring, so    */
						/* include for pedigree structure preservation: */
						/* sex of this individual must be known:        */
						/*                                              */
						c->counters.dno.in++;
						return 1;
					}else{
						/*                                                        */
						/* terminal indiv. w/o data -- here it doesn't matter     */
						/* whether they have gender listed or not, we can exclude */
						/* them:                                                  */
						/*                                                        */
						if(c->AutoExclude){
							/* AutoExclude ON */
							c->counters.dno.ex++;
							return 0;
						}else{
							/* AutoExclude OFF */
							c->counters.dno.in++;
							return 1;
						}
					}
				}
			}
		}
	}else{

		/*                                                                     */
		/* virtual individual, not in database.  These individuals             */
		/* are inserted by Madeline in order to obtain proper pedigree         */
		/* structure.  They are the unsampled parents of a sibship.  Because   */
		/* they have __presumably__ __included__ __offspring__, they           */
		/* need to be included even when marked for exclusion by the user      */
		/*                                                                     */
		c->counters.vir.in++;
		return 1;
	}
}


/*                                                                            */
/*                                                                            */
/* MendelPedigreeHasData():                                                   */
/*                                                                            */
/* -> Differs from GeneralCasePedigreeHasData() only in not permitting        */
/*    individuals without gender in the return count by calling               */
/*    MendelIndividualHasData() instead of GeneralCaseIndividualHasData() and */
/*    MendelCountIncludedPedigreeIndividual() instead of                      */
/*    GeneralCaseCountIncludedPedigreeIndividual()                            */
/*                                                                            */
int MendelPedigreeHasData(PCONTROL c,PPEDIGREE pd){

	/*                                                                        */
	/* If AutoExclude is turned off, then we are:                             */
	/* in practice saying that the pedigree has data, even though it may      */
	/* not have any in reality.  For this reason, AutoExclude should normally */
	/* be turned on:                                                          */
	/*                                                                        */
	if(!c->AutoExclude){
		/*                             */
		/* increment include counters: */
		/*		                           */
		c->counters.pedg.in++;
		return TraverseIndividualBST6(c,pd->h,MendelCountIncludedPedigreeIndividual);
	}
	/*                                                                    */
	/* Mendel case:                                                       */
	/*                                                                    */
	/* if(TraverseIndividualBSTUntilTrue(pd->h,MendelIndividualHasData)){ */
	/*                                                                    */
	if(TraverseIndividualBSTUntilDataFound(pd->h)){
		/*                             */
		/* increment include counters: */
		/*                             */
		c->counters.pedg.in++;
		return TraverseIndividualBST6(c,pd->h,MendelCountIncludedPedigreeIndividual);

	}else{	
		/*                                                                  */
		/* no data: return zero to indicate exclusion of pedigree:          */
		/*                                                                  */
		/* -> GeneralCaseCountExcludedPedigreeIndividual() works fine here: */
		/*                                                                  */

		dprintf("MendelPedigreeHasData(): excluding pedigree %s: lacks individual with data.\n",pd->fid);

		c->counters.pedg.ex++;
		TraverseIndividualBST2(c,pd->h,GeneralCaseCountExcludedPedigreeIndividual);
		return 0;
	}
}

/*                                                                       */
/*                                                                       */
/* FisherPedigreeHasData():                                              */
/*                                                                       */
/* -> Much like MendelPedigreeHasData().  The only difference is that    */
/*    if FISHER with ascertainment correction is invoked (FISHER1), then */
/*    we need to find a NON-PROBAND with data.  Other than this, the     */
/*    bookkeeping and who's included is the same as with Mendel:         */
/*                                                                       */
int FisherPedigreeHasData(PCONTROL c,PPEDIGREE pd){

	/*                                                                        */
	/* If AutoExclude is turned off, then we are:                             */
	/* in practice saying that the pedigree has data, even though it may      */
	/* not have any in reality.  For this reason, AutoExclude should normally */
	/* be turned on:                                                          */
	/*                                                                        */
	if(!c->AutoExclude){
		/*                             */
		/* increment include counters: */
		/*		                           */
		c->counters.pedg.in++;
		return TraverseIndividualBST6(c,pd->h,MendelCountIncludedPedigreeIndividual);
	}
	/*                                                                                                                   */
	/* Mendel case, with modification for FISHER1:                                                                       */
	/*                                                                                                                   */
	/* if(TraverseIndividualBSTUntilTrue(pd->h,c->OutputFormat==FISH1?FisherNonProbandHasData:MendelIndividualHasData)){ */
	/*                                                                                                                   */
	if(TraverseIndividualBSTUntilDataFound(pd->h)){
	
		/*                             */
		/* increment include counters: */
		/*                             */
		c->counters.pedg.in++;
		return TraverseIndividualBST6(c,pd->h,MendelCountIncludedPedigreeIndividual);

	}else{	
		/*                                                                  */
		/* no data: return zero to indicate exclusion of pedigree:          */
		/*                                                                  */
		/* -> GeneralCaseCountExcludedPedigreeIndividual() works fine here: */
		/*                                                                  */
		dprintf("FisherPedigreeHasData(): excluding pedigree %s: lacks individual with data.\n",pd->fid);
		
		c->counters.pedg.ex++;
		TraverseIndividualBST2(c,pd->h,GeneralCaseCountExcludedPedigreeIndividual);
		return 0;
	}
}


/*                                                               */
/* UserM13 Case:  UserM13 can use everybody with data, including */
/* the childless spouses and unattached individuals:             */
/*                                                               */
/*///////////////////////////                                    */
/*                                                               */
/* USERM13 Case:                                                 */
/*                                                               */
/*///////////////////////////                                    */
/*///////////////////////////                                    */

/*                                                                      */
/* UserM13CountIncludedPedigreeIndividual()                             */
/*                                                                      */
/*                                                                      */
/* --> This function returns 1 for each __included__ individual,        */
/*     zero for each __excluded__ individual.  Note that book-          */
/*     keeping on individuals is broken down by category in the         */
/*     CONTROL structure.                                               */
/*                                                                      */
/* --> Like MendelCountIncludedPedigreeIndividual, but bookkeeping      */
/*     is different because UserM13 can use anyone with data, including */
/*     unattached individuals and childless spouses:                    */
/*                                                                      */
int UserM13CountIncludedPedigreeIndividual(PCONTROL c,PINDIVIDUAL p){

	if(p->db){
		/*              */
		/* in database: */
		/*              */
		if(p->unconnected){
			/*                                             */
			/* For USERM13, unconnected individuals CAN BE */
			/* INCLUDED in output:                         */
			/*                                             */
			if(p->excluded){
				if(c->AutoExclude){
					/* AutoExclude ON */
					c->counters.flt.ex++;
					return 0;
				}else{
					/* AutoExclude OFF */
					c->counters.flt.in++;
					return 1;
				}
			}else{
				if(p->hasdata){
					c->counters.flt.in++;
					return 1;
				}else{
					if(c->AutoExclude){
						/* AutoExclude ON */
						c->counters.flt.ex++;
						return 0;
					}else{
						/* AutoExclude OFF */
						c->counters.flt.in++;
						return 1;
					}
				}
			}

		}else if(p->csp){
			/*                                       */
			/* For USERM13, childless spouses CAN BE */
			/* INCLUDED in output                    */
			/*                                       */
			if(p->excluded){
				if(c->AutoExclude){
					/* AutoExclude ON */
					c->counters.csp.ex++;
					return 0;
				}else{
					/* AutoExclude OFF */
					c->counters.csp.in++;
					return 1;
				}
			}else{
				if(p->hasdata){
					c->counters.csp.in++;
					return 1;
				}else{
					if(c->AutoExclude){
						/* AutoExclude ON */
						c->counters.csp.ex++;
						return 0;
					}else{
						/* AutoExclude OFF */
						c->counters.csp.in++;
						return 1;
					}
				}
			}

		}else{
			/*                                                */
			/* normally connected individual in the database: */
			/*                                                */
			if(p->excluded){
				/*                                    */
				/* An excluded person in the database */
				/*                                    */
				if(p->nk){
					/*                                                */
					/* Since this person has                          */
					/* __presumably__ __included__ children,          */
					/* he or she is required in output for structural */
					/* maintenance, but his/her data is to be         */
					/* excluded:                                      */
					/*                                                */
					c->counters.dex.in++;
					return 1;			
				}else{
					/*                                                */
					/* No record is required in output for            */
					/* a terminal person without offspring marked for */
					/* exclusion:                                     */
					/*                                                */
					if(c->AutoExclude){
						/* AutoExclude is ON */
						c->counters.dex.ex++;
						return 0;
					}else{
						/* AutoExclude is OFF */
						c->counters.dex.in++;
						return 1;
					}
				}
			}else{
				/*                                                    */
				/* not an excluded individual: a regular,             */
				/* individual:                                        */
				/*                                                    */
				/* this is the only case remaining where we could     */
				/* get a person without gender where it would matter  */
				/* (the other cases would be excluded already anyway, */
				/*  so gender would not make a difference):           */
				/*                                                    */
				if(p->hasdata){
					if(p->female==MISSING){
						/*                                                 */
						/* terminal individual without gender, so exclude: */
						/*                                                 */
						if(c->AutoExclude){
							/* AutoExclude is ON */
							c->counters.din.ex++;
							return 0;
						}else{
							/* AutoExclude is OFF */
							c->counters.din.in++;
							return 1;
						}
					}else{
						c->counters.din.in++;
						return 1;
					}
				}else{
					/*               */
					/* no data case: */
					/*               */
					if(p->nk){
						/*                                              */
						/* has __presumably included__ offspring, so    */
						/* include for pedigree structure preservation: */
						/* sex of this individual must be known:        */
						/*                                              */
						c->counters.dno.in++;
						return 1;
					}else{
						/*                                                        */
						/* terminal indiv. w/o data -- here it doesn't matter     */
						/* whether they have gender listed or not, we can exclude */
						/* them:                                                  */
						/*                                                        */
						if(c->AutoExclude){
							/* AutoExclude ON */
							c->counters.dno.ex++;
							return 0;
						}else{
							/* AutoExclude ON */
							c->counters.dno.in++;
							return 1;
						}
					}
				}
			}
		}
	}else{
		/*                                                                     */
		/* virtual individual, not in database.  These individuals             */
		/* are inserted by Madeline in order to obtain proper pedigree         */
		/* structure, and they have __presumably__ __included__ __offspring__  */
		/*                                                                     */
		c->counters.vir.in++;
		return 1;
	}
}


/*                                                                             */
/*                                                                             */
/* UserM13PedigreeHasData():                                                   */
/*                                                                             */
/* -> Differs from GeneralCasePedigreeHasData() only in not permitting         */
/*    individuals without gender in the return count by calling                */
/*    UserM13IndividualHasData() instead of GeneralCaseIndividualHasData() and */
/*    UserM13CountIncludedPedigreeIndividual() instead of                      */
/*    GeneralCaseCountIncludedPedigreeIndividual()                             */
/*                                                                             */
int UserM13PedigreeHasData(PCONTROL c,PPEDIGREE pd){

	/*                                                                        */
	/* If AutoExclude is turned off, then we are:                             */
	/* in practice saying that the pedigree has data, even though it may      */
	/* not have any in reality.  For this reason, AutoExclude should normally */
	/* be turned on:                                                          */
	/*                                                                        */
	if(!c->AutoExclude){
		/*                             */
		/* increment include counters: */
		/*		                           */
		c->counters.pedg.in++;
		return TraverseIndividualBST6(c,pd->h,UserM13CountIncludedPedigreeIndividual);
	}
	/*                                                                     */
	/* UserM13 case:                                                       */
	/*                                                                     */
	/* if(TraverseIndividualBSTUntilTrue(pd->h,UserM13IndividualHasData)){ */
	/*                                                                     */
	if(TraverseIndividualBSTUntilDataFound(pd->h)){
		
		/*                             */
		/* increment include counters: */
		/*                             */
		c->counters.pedg.in++;
		return TraverseIndividualBST6(c,pd->h,UserM13CountIncludedPedigreeIndividual);

	}else{	
		/*                                                                  */
		/* no data: return zero to indicate exclusion of pedigree:          */
		/*                                                                  */
		/* -> GeneralCaseCountExcludedPedigreeIndividual() works fine here: */
		/*                                                                  */

		dprintf("UserM13PedigreeHasData(): excluding pedigree %s: lacks individual with data.\n",pd->fid);

		c->counters.pedg.ex++;
		TraverseIndividualBST2(c,pd->h,GeneralCaseCountExcludedPedigreeIndividual);
		return 0;
	}
}


/*                              */
/* int ExcludedForGeneralCase() */
/*                              */
int ExcludedForGeneralCase(PINDIVIDUAL p){
	
	extern PCONTROL g;
	
	/* If AutoExclude is OFF, include everyone: */
	if(!g->AutoExclude) return 0;

	/*                                              */
	/* Excluded if childless spouse or unconnected: */
	/*                                              */
	if(p->csp || p->unconnected) return 1;
	/*                                                         */
	/* Excluded if a terminal individual marked for exclusion: */
	/*                                                         */
	if(p->excluded && !p->nk)     return 1;
	/*                                                */
	/* Exclude if a terminal individual without data: */
	/*                                                */
	if(!p->hasdata && !p->nk)    return 1;
		
	/*               */
	/* Not excluded: */
	/*               */
	return 0;
}


/*                         */
/* int ExcludedForMendel() */
/*                         */
int ExcludedForMendel(PINDIVIDUAL p){
	
	extern PCONTROL g;
	
	/* If AutoExclude is OFF, include everyone: */
	if(!g->AutoExclude) return 0;

	if(p->female==MISSING) return 1;
	return ExcludedForGeneralCase(p);
}



/*                          */
/* int ExcludedForUserM13() */
/*                          */
int ExcludedForUserM13(PINDIVIDUAL p){
	
	extern PCONTROL g;
	
	/* If AutoExclude is OFF, include everyone: */
	if(!g->AutoExclude) return 0;

	if(p->female==MISSING) return 1;
	/*                                                         */
	/* Excluded if a terminal individual marked for exclusion: */
	/*                                                         */
	if(p->excluded && !p->nk)     return 1;
	/*                                                */
	/* Exclude if a terminal individual without data: */
	/*                                                */
	if(!p->hasdata && !p->nk)    return 1;
	/*               */
	/* Not excluded: */
	/*               */
	return 0;
}



/*                   */
/* PedigreeTally():  */
/*                   */
void PedigreeTally(PCONTROL c,PPEDIGREE pd){

	c->counters.pedg.in++; /* pedigree tally   */
	/*                                         */
	/* attached, data included individuals:    */
	/* (there are no data excluded individuals */
	/*  when first opening a database)         */
	/*                                         */
	c->counters.din.in+=(pd->n - pd->nvi - pd->nun - pd->ncsp);
	c->counters.csp.in+=pd->ncsp; /* childless spouse tally        */
	c->counters.flt.in+=pd->nun;  /* unconnecteds (floaters) tally */
	c->counters.vir.in+=pd->nvi;  /* virtual (dummied-in) tally    */

}


/*                                                         */
/* ResetTallyCounters(): resets the general tally counters */
/*                       used by VIEW, EXCLUDE, UNEXCLUDE: */
/*                                                         */
void ResetTallyCounters(PCONTROL c){

	/*                                                                                      */
	/* reset tallies to zero, and lastpd to NULL:                                           */
	/*                                                                                      */
	c->tally.pedg.in  =0; /* pedigrees included                                             */
	c->tally.din.in=0; /* data-included indiv. who are included in output                   */
	c->tally.dno.in=0; /* indv. who have no data in database who are included in output     */
	c->tally.dex.in=0; /* data-excluded indiv. who are included in output                   */
	c->tally.csp.in=0; /* childless spouses who are included in output                      */
	c->tally.flt.in=0; /* unattached floaters who are included in output                    */
	c->tally.vir.in=0; /* virtual dummied-ins who are included in output                    */

	/*c->tally.pedg.ex=0; // pedigrees excluded                                             */
	/*c->tally.din.ex=0; // data-included indiv. who are excluded from output               */
	/*c->tally.dno.ex=0; // indv. who have no data in database who are excluded from output */
	/*c->tally.dex.ex=0; // data-excluded indiv. who are excluded from output               */
	/*c->tally.csp.ex=0; // childless spouses who are excluded from output                  */
	/*c->tally.flt.ex=0; // unattached floaters who are excluded from output                */
	/*c->tally.vir.ex=0; // virtual dummied-ins who are excluded from output                */

	/*                                                                                      */
	/* lastpd stores the pointer to the last pedigree:                                      */
	/*                                                                                      */
	c->lastpd=NULL;
	
	
}



/*                                                                          */
/*                                                                          */
/* GenehunterPedigreeHasData():                                             */
/*                                                                          */
/* -> Similar to GeneralCasePedigreeHasData(), except that                  */
/*    the following pedigrees are also excluded:                            */
/*                                                                          */
/*    -> trios of 2 parents and a single offspring are excluded for both    */
/*       parametric (GENEHUNTER keyword) and non-parametric (GENEHUNTERNPL  */
/*       keyword) and QTL (GENEHUNTERQTL keyword).                          */
/*                                                                          */
/*    -> If only a single affected in entire pedigree, exclude for          */
/*       non-parametric (GENEHUNTERNPL)                                     */
/*                                                                          */
/*    -> If in a pedigree there are only 2 affected and the affected        */
/*       form a parent-offspring relation, exclude for non-parametric       */
/*       (GENEHUNTERNPL keyword)                                            */
/*                                                                          */
/*    -> If a total of only three affected individuals who form a           */
/*       parent-parent-single offspring trio, then exclude for non-         */
/*       parametric analysis (GENEHUNTERNPL)                                */
/*                                                                          */
int GenehunterPedigreeHasData(PCONTROL c,PPEDIGREE pd){

	int naffected;
	
	/*                                                                        */
	/* If AutoExclude is turned off, then we are:                             */
	/* in practice saying that the pedigree has data, even though it may      */
	/* not have any in reality.  For this reason, AutoExclude should normally */
	/* be turned on:                                                          */
	/*                                                                        */
	if(!c->AutoExclude){
		/*                             */
		/* increment include counters: */
		/*		                           */
		c->counters.pedg.in++;
		return TraverseIndividualBST6(c,pd->h,GeneralCaseCountIncludedPedigreeIndividual);
	}
	/*                                                                        */
	/* General case:                                                          */
	/*                                                                        */
	/*if(TraverseIndividualBSTUntilTrue(pd->h,GeneralCaseIndividualHasData)){ */
	/*                                                                        */
	if(TraverseIndividualBSTUntilDataFound(pd->h)){
		/*                                                                  */
		/* Exclude if entire pedigree is just a trio (or less than a trio): */
		/*                                                                  */
		/* -> "just a trio" means there's only one founding mother,         */
		/*    and she has just one kid who hasn't any offspring:            */
		/*                                                                  */
		if(!pd->nfm ||
		   (pd->nfm==1 && !pd->fm[0]->nk) ||
		   (pd->nfm==1 &&  pd->fm[0]->nk==1 && pd->fm[0]->k[0]->nk==0)
		){
			/*                    */
			/* unusable pedigree: */
			/*                    */
			dprintf("GenehunterPedigreeHasData(): excluding pedigree %s: consists of a single trio or less.\n",pd->fid);

			c->counters.pedg.ex++;
			TraverseIndividualBST2(c,pd->h,GeneralCaseCountExcludedPedigreeIndividual);
			return 0;
		}		
		/*                                                 */
		/* Additional criteria for GHNPK (non-parametric): */
		/*                                                 */
		if(c->OutputFormat==GHNPK){
			/*                                     */
			/* For NPL exclusions, we need to      */
			/* get a count of number of affecteds: */
			/*                                     */
			naffected=TraverseIndividualBST7(pd->h,IsAffected);
			
			switch(naffected){
			case 1:
				/*                                              */
				/* Single affected is never sufficient for NPL: */
				/*                                              */
				dprintf("GenehunterPedigreeHasData(): excluding pedigree %s: contains only a single affected individual.\n",pd->fid);

				c->counters.pedg.ex++;
				TraverseIndividualBST2(c,pd->h,GeneralCaseCountExcludedPedigreeIndividual);
				return 0;
								
			case 2:
				/*                                                  */
				/* Two affecteds can't be parent <-> offspring pair */
				/*                                                  */
				if(TraverseIndividualBSTUntilTrue(pd->h,OffspringAndOneParentAffected)){

					dprintf("GenehunterPedigreeHasData(): excluding pedigree %s: contains only a parent-offspring affected pair.\n",pd->fid);

					c->counters.pedg.ex++;
					TraverseIndividualBST2(c,pd->h,GeneralCaseCountExcludedPedigreeIndividual);
					return 0;
				}
				break;
			case 3:
				/*                                                    */
				/* Three affecteds can't be parent <-> offspring trio */
				/*                                                    */
				if(TraverseIndividualBSTUntilTrue(pd->h,OffspringAndBothParentsAffected)){

					dprintf("GenehunterPedigreeHasData(): excluding pedigree %s: contains only parent-parent-offspring affected trio.\n",pd->fid);

					c->counters.pedg.ex++;
					TraverseIndividualBST2(c,pd->h,GeneralCaseCountExcludedPedigreeIndividual);
					return 0;
				}
				break;
			}
		}
		
		/*                                           */
		/* Get here if pedigree is OK for inclusion: */
		/*                                           */
		
		/*                             */
		/* increment include counters: */
		/*                             */
		c->counters.pedg.in++;
		return TraverseIndividualBST6(c,pd->h,GeneralCaseCountIncludedPedigreeIndividual);

	}else{	
		/*                                                         */
		/* no data: return zero to indicate exclusion of pedigree: */
		/*                                                         */
		dprintf("GenehunterPedigreeHasData(): excluding pedigree %s: lacks an individual with data.\n",pd->fid);

		c->counters.pedg.ex++;
		TraverseIndividualBST2(c,pd->h,GeneralCaseCountExcludedPedigreeIndividual);
		return 0;
	}
}

/*                                                                */
/* IsAffected(): returns 1 if individual has data and is affected */
/* and is connected:                                              */
/*                                                                */
int IsAffected(PINDIVIDUAL p){

	return p->affected==1 && p->hasdata && !p->csp && !p->unconnected;

}

/*                                                                                          */
/* OffspringAndOneParentAffected(): returns true if its an offspring->parent affected pair: */
/*                                                                                          */
int OffspringAndOneParentAffected(PINDIVIDUAL p){

	return p->affected==1 && ( p->f && p->f->affected==1 || p->m && p->m->affected==1 );

}


/*                                                                                    */
/* OffspringAndBothParentsAffected(): returns true if its an offspring->parent/parent */
/* affected trio;                                                                     */
/*                                                                                    */
int OffspringAndBothParentsAffected(PINDIVIDUAL p){

	return p->affected==1 && p->f && p->f->affected==1 && p->m && p->m->affected==1;

}


/*                       */
/* ResetErrorCounters(): */
/*                       */
void ResetErrorCounters(PCONTROL c){

	if(c->HasSyntaxErrors || c->HasErrors || c->HasWarnings || c->HasSevereWarnings || c->HasInheritanceErrors){
		/*                                                */
		/* Summarize total number of errors in error log: */
		/*                                                */
		if(c->streams.err){
			fprintf(c->streams.err,
				"\n*** %i syntax error%s, %i warning%s, %i severe warning%s, %i inheritance inconsistenc%s, and %i fatal error%s counted ***\n",
				c->HasSyntaxErrors,
				c->HasSyntaxErrors==1?"":"s",
				c->HasWarnings,
				c->HasWarnings==1?"":"s",
				c->HasSevereWarnings,
				c->HasSevereWarnings==1?"":"s",
				c->HasInheritanceErrors,
				c->HasInheritanceErrors==1?"y":"ies",
				c->HasErrors,
				c->HasErrors==1?"":"s"
			);
		}
		/*        */
		/* reset: */
		/*        */
		c->HasSyntaxErrors=c->HasWarnings=0;
		c->HasSevereWarnings=c->HasErrors=0;
		c->HasInheritanceErrors=0;
		c->ResetPrompt=0;
	}
}




/*                              */
/* int CrimapPedigreeHasData(): */
/*                              */
int CrimapPedigreeHasData(PCONTROL c,PPEDIGREE pd){


	int onlyone;   /* TRUE if only one of the two founders has data */
	int noone;     /* TRUE if neither founder has data              */
	int f1typed;
	/* int noutput,nexcluded; */
	int i;
	PINDIVIDUAL p;


	/*                                                                        */
	/* If AutoExclude is turned off, then we are:                             */
	/* in practice saying that the pedigree has data, even though it may      */
	/* not have any in reality.  For this reason, AutoExclude should normally */
	/* be turned on:                                                          */
	/*                                                                        */
	if(!c->AutoExclude){
		/*                             */
		/* increment include counters: */
		/*		                           */
		c->counters.pedg.in++;
		return TraverseIndividualBST6(c,pd->h,CrimapCountIncludedPedigreeIndividual);
	}
	/*              */
	/* Crimap case: */
	/*              */
	if(TraverseIndividualBSTUntilDataFound(pd->h)){
		/*                                   */
		/* Mark if neither founder has data: */
		/*                                   */
		noone=!(pd->ff[0]->hasdata || pd->fm[0]->hasdata);
		/*                                           */
		/* Mark if only one founding parent has data */
		/* using bitwise exclusive OR operator:      */
		/*                                           */
		onlyone= pd->ff[0]->hasdata ^ pd->fm[0]->hasdata;
		
		/*                                                                  */
		/* Count how many f1 offspring are typed:                           */
		/*                                                                  */
		/*  in the case of "onlyone" founding parent, we need instead       */
		/*  to know how many typed f1 offspring who themselves have kids    */
		/*                                                                  */
		/*  in the case of "noone" founding parent, we need to know instead */
		/*  how many typed f1 offspring who have kids _AND_ a typed spouse: */
		/*                                                                  */
		/*                                                                  */
		if(onlyone){
			for(f1typed=i=0;i<pd->fm[0]->nk;i++){
				p=pd->fm[0]->k[i];
				if((p->hasdata || SpouseHasData(p)) && OffspringHasData(p)){
					f1typed++;
				}
			}
		}else if(noone){
			for(f1typed=i=0;i<pd->fm[0]->nk;i++){
				p=pd->fm[0]->k[i];
				if(p->hasdata && SpouseHasData(p) && OffspringHasData(p)){
					f1typed++;
				}
			}
		}else{
			/*                            */
			/* case of two typed parents: */
			/*                            */
			for(f1typed=i=0;i<pd->fm[0]->nk;i++){
				p=pd->fm[0]->k[i];
				if(p->hasdata){
					f1typed++;
				}
			}
		}
		/*                                       */
		/* if no f1 offspring typed, then leave: */
		/*                                       */
		if(!f1typed){
			if(noone)
				dprintf("CrimapPedigreeHasData(): Excluding pedigree %s: Founding parental pair is not typed and there are no typed sibs with typed offspring\n",pd->fid);
			else if(onlyone)
				dprintf("CrimapOutput(): Excluding pedigree %s: One typed founding parent, but no typed sibs with typed offspring\n",pd->fid);
			else
				dprintf("CrimapOutput(): Excluding pedigree %s: Founding parental pair is typed but there are no typed sibs\n",pd->fid);                  

			/*                          */
			/* Count excluded pedigree: */
			/*                          */
			c->counters.pedg.ex++;
			TraverseIndividualBST2(c,pd->h,GeneralCaseCountExcludedPedigreeIndividual);
			return 0;
		}
	
		/*                                                */
		/* get here (finally!) if it is a good family:    */
		/* keep track of how many families are going out: */
		/*                                                */
		/*                                                */
		/* increment include counters:                    */
		/*                                                */
		c->counters.pedg.in++;
		return TraverseIndividualBST6(c,pd->h,CrimapCountIncludedPedigreeIndividual);

	}else{	
		/*                                                                  */
		/* no data: return zero to indicate exclusion of pedigree:          */
		/*                                                                  */
		/* -> GeneralCaseCountExcludedPedigreeIndividual() works fine here: */
		/*                                                                  */

		dprintf("CrimapPedigreeHasData(): excluding pedigree %s: lacks individual with data.\n",pd->fid);

		c->counters.pedg.ex++;
		TraverseIndividualBST2(c,pd->h,GeneralCaseCountExcludedPedigreeIndividual);
		return 0;
	}
}


/* */
/* */
/* */
int CrimapCountIncludedPedigreeIndividual(PCONTROL c,PINDIVIDUAL p){

	/* */
	/* */
	/* */
	if(p->db){
		/*              */
		/* in database: */
		/*              */
		if(p->unconnected){
			/*                                                                */
			/* In Crimap and in the general case, unconnected individuals are */
			/* not included in output, regardless of the status of the        */
			/* p->excluded flag:                                              */
			/*                                                                */
			if(c->AutoExclude){
				/* AutoExclude ON */
				c->counters.flt.ex++;
				return 0;
			}else{
				/* AutoExclude OFF */
				c->counters.flt.in++;
				return 1;
			}
		}else if(p->csp){
			/*                                                           */
			/* In Crimap and in the general case, childless spouses are  */
			/* not included in output, regardless of the status of the   */
			/* p->excluded flag:                                         */
			/*                                                           */
			if(c->AutoExclude){
				/* AutoExclude ON */
				c->counters.csp.ex++;
				return 0;
			}else{
				/* AutoExclude OFF */
				c->counters.csp.in++;
				return 1;
			}
		}else{
			/*                                                */
			/* normally connected individual in the database: */
			/*                                                */
			if(p->excluded){
				/*                                    */
				/* An excluded person in the database */
				/*                                    */
				if(p->nk){
					/*                                                */
					/* Since this person has                          */
					/* __presumably__ __included__ children,          */
					/* he or she is required in output for structural */
					/* maintenance, but his/her data is to be         */
					/* excluded:                                      */
					/*                                                */
					c->counters.dex.in++;
					return 1;			
				}else{
					/*                                                */
					/* No record is required in output for            */
					/* a terminal person without offspring marked for */
					/* exclusion:                                     */
					/*                                                */
					if(c->AutoExclude){
						/* AutoExclude ON */
						c->counters.dex.ex++;
						return 0;
					}else{
						/* AutoExclude OFF */
						c->counters.dex.in++;
						return 1;
					}
				}
			}else{
				/*                                            */
				/* not an excluded individual: a regular,     */
				/* individual: classify based on whether they */
				/* have data or not:                          */
				/*                                            */
				if(p->hasdata){
					/*                                                    */
					/* For Crimap, we are additionally going to           */
					/* exclude fruitless offspring of a founding pair     */
					/* if either only one or neither of the founding pair */
					/* has data:                                          */
					/*                                                    */
					if(!(p->nk || p->f->f || p->f->m || p->m->f || p->m->m)){
						/*                                               */
						/* -> This individual is an offspring of         */
						/*    a founding pair who has no children:       */
						/*    If both of his parents are typed, then     */
						/*    keep him.  If one (or none) of his parents */
						/*    are typed, then toss him (or her):         */
						/*                                               */
						if(!(p->f->hasdata && p->m->hasdata)){
							/*                     */
							/* exclude individual: */
							/*                     */
							if(c->AutoExclude){
								/* AutoExclude ON */
								c->counters.din.ex++;
								return 0;
							}else{
								/* AutoExclude OFF */
								c->counters.din.in++;
								return 1;
							}
						}else{
							/*                     */
							/* include individual: */
							/*                     */
							c->counters.din.in++;
							return 1;
						}					
					}else{
						/*                     */
						/* include individual: */
						/*                     */
						c->counters.din.in++;
						return 1;
					}
				}else{
					if(p->nk){
						/*                                           */
						/* has __presumably__ __included__ children, */
						/* so required for structural maintenance:   */
						/*                                           */
						c->counters.dno.in++;
						return 1;
					}else{
						/*                                          */
						/* terminal without data, so don't include: */
						/*                                          */
						if(c->AutoExclude){
							/* AutoExclude ON */
							c->counters.dno.ex++;
							return 0;
						}else{
							/* AutoExclude OFF */
							c->counters.dno.in++;
							return 1;
						}
					}				
				}
			}
		}
	}else{
		/*                                                             */
		/* virtual individual, not in database.  These individuals     */
		/* are inserted by Madeline in order to obtain proper pedigree */
		/* structure, and hence are included in output even though     */
		/* they don't have data.                                       */
		/*                                                             */
		c->counters.vir.in++;
		return 1;
	}
}


/*                         */
/* int ExcludedForCrimap() */
/*                         */
int ExcludedForCrimap(PINDIVIDUAL p){
	/*                                              */
	/* Excluded if childless spouse or unconnected: */
	/*                                              */
	if(p->csp || p->unconnected) return 1;
	/*                                                         */
	/* Excluded if a terminal individual marked for exclusion: */
	/*                                                         */
	if(p->excluded && !p->nk)     return 1;
	/*                                                */
	/* Exclude if a terminal individual without data: */
	/*                                                */
	if(!p->hasdata && !p->nk)    return 1;
	/*                                                        */
	/* Exclude if a fruitless offspring of a founding pair    */
	/* where either one or both of the founders is not typed: */
	/*                                                        */
	if(  p->hasdata &&
	   !(p->nk || p->f->f || p->f->m || p->m->f || p->m->m)
	){
		/*                                               */
		/* -> This individual is an offspring of         */
		/*    a founding pair and he has no children:    */
		/*    If both of his parents are typed, then     */
		/*    keep him.  If one (or none) of his parents */
		/*    are typed, then toss him (or her):         */
		/*                                               */
		if(!(p->f->hasdata && p->m->hasdata)){
			/*                      */
			/* excluded individual: */
			/*                      */
			return 1;
		}else{
			/*                      */
			/* included individual: */
			/* not excluded:        */
			/*                      */
			return 0;
		}
	}
	/*               */
	/* Not excluded: */
	/*               */
	return 0;
}

/*                                              */
/* NumberOfPairs(): Returns the number of pairs */
/*                  that can be formed from n   */
/*                  individuals                 */
/*                                              */
int NumberOfPairs(int n){

	int i,s;

	if(n<2) return 0;
	for(s=0,i=n-1;i;i--) s+=i;
	return s;

}



/*                                                              */
/* UserFQTLNuclearFamilyHasData()                               */
/*                                                              */
/* -> s1 and s2 represent the founders of the nuclear pedigree: */
/*                                                              */
int UserFQTLNuclearFamilyHasData(PCONTROL c,PPEDIGREE pd,PINDIVIDUAL s1,PINDIVIDUAL s2){

	PINDIVIDUAL mom;
	PINDIVIDUAL dad;
	
	/* int i; */
	int nio; /* number of offspring who will be included: */

	if(s1->female){
		mom=s1;
		dad=s2;
	}else{
		mom=s2;
		dad=s1;
	}
	
	/*                                                             */
	/* If UserFQTLOffspring selected and this is not the offspring */
	/* generation, don't count it:                                 */
	/*                                                             */
	if(c->OutputFormat==UFNOK &&
	   !(mom->m==pd->fm[0] || dad->m==pd->fm[0])
	){
		nio=CountExcludedNuclearPedigree(c,mom,dad);
		dprintf("UserFQTLNuclearFamilyHasData():\nExcluding %i individuals in pedigree %s in nuclear pedigree formed by %s and %s:\n\tparents are not offspring of original founders\n",
			nio,
			pd->fid,
			mom->id,
			dad->id
		);
		return 0;
	}
	
	/*                                                                */
	/* if UserFQTLFounders: exclude any generation other than founder */
	/*                                                                */
	if(c->OutputFormat==UFNFK && mom!=pd->fm[0]){
		nio=CountExcludedNuclearPedigree(c,mom,dad);
		dprintf("UserFQTLNuclearFamilyHasData():\nExcluding %i individuals in pedigree %s in nuclear pedigree formed by %s and %s:\n\tparents are not original founders\n",
			nio,
			pd->fid,
			mom->id,
			dad->id
		);
		return 0;
	}
		
	
	/* In general for UserFQTL,                                  */
	/* we just need one person with complete                     */
	/* phenotype data -- can be either parent, or one of the     */
	/* sibs, treated separately below:                           */
	/*                                                           */
	/* NOTE: use of "haspdata" to check only for PHENOTYPE data: */
	/*                                                           */
	if(mom->haspdata || 
	   dad->haspdata ||
	   OffspringOfPairHasPhenotypeData(mom,dad)
	){
		return CountIncludedNuclearPedigree(c,mom,dad);
						
	}else{
			
		nio=CountExcludedNuclearPedigree(c,mom,dad);
		dprintf("UserFQTLNuclearFamilyHasData():\n\tExcluding %i individuals in pedigree %s in nuclear pedigree formed by %s and %s:\n\tno individual has phenotype data\n",
			nio,
			pd->fid,
			mom->id,
			dad->id
		);
		return 0;

	}
}


/*                                 */
/* CountIncludedNuclearPedigree(): */
/*                                 */
int CountIncludedNuclearPedigree(PCONTROL c,PINDIVIDUAL mom,PINDIVIDUAL dad){

	int i;    /* generic loop counter              */
	int no;   /* number of offspring of the pair   */
	int nio;  /* number of included offspring      */
	int nsp;  /* total number of sibpairs possible */
	int nisp; /* number of included sibpairs       */
	
	/*                                                            */
	/* Nuclear family has data: increment counters for inclusion: */
	/*                                                            */
	c->nuclear.pedg.in++;
	/*                    */
	/* Tally for parents: */
	/*                    */
	CountIncludedNuclearPedigreeParent(c,mom);
	CountIncludedNuclearPedigreeParent(c,dad);
	/*                          */
	/* Tally for the offspring: */
	/*                          */
	for(no=nio=i=0;i<mom->nk;i++){
		/*                                          */
		/* Just look at the offspring of this pair: */
		/*                                          */
		if(mom->k[i]->f==dad){
			no++;
			nio+=CountIncludedNuclearPedigreeOffspring(c,mom->k[i]);
		}
	}
	/*                              */
	/* Number of sibpairs possible: */
	/*                              */
	nsp=NumberOfPairs(no);
	/*                                           */
	/* Number of sibpairs that will be included: */
	/*                                           */
	nisp=NumberOfPairs(nio);
	/*                                                 */
	/* Tally number of sibpairs that will be included: */
	/*                                                 */
	c->nuclear.sibp.in+=nisp;
	/*                                                      */
	/* Tally number of sibpairs that __won't__ be included: */
	/*                                                      */
	c->nuclear.sibp.ex+=(nsp-nisp);
	
	/*                                                                */
	/* Return count of two parents plus number of included offspring: */
	/*                                                                */
	return 2+nio;
}


/*                                 */
/* CountExcludedNuclearPedigree(): */
/*                                 */
int CountExcludedNuclearPedigree(PCONTROL c,PINDIVIDUAL mom,PINDIVIDUAL dad){

	int i;
	int no;  /* number of offspring */
	int nsp; /* number of sibpairs  */
	
	/*                                                          */
	/* Nuclear family doesn't have data: increment counters for */
	/* exclusion:                                               */
	/*                                                          */
	c->nuclear.pedg.ex++;
	/*                    */
	/* Tally for parents: */
	/*                    */
	CountExcludedNuclearPedigreeParent(c,mom);
	CountExcludedNuclearPedigreeParent(c,dad);
	/*                          */
	/* Tally for the offspring: */
	/*                          */
	for(no=i=0;i<mom->nk;i++){
		/*                                          */
		/* Just look at the offspring of this pair: */
		/*                                          */
		if(mom->k[i]->f==dad){
			no++;
			CountExcludedNuclearPedigreeOffspring(c,mom->k[i]);
		}
	}
	
	/*                              */
	/* Number of sibpairs possible: */
	/*                              */
	nsp=NumberOfPairs(no);
	/*                                                      */
	/* Tally number of sibpairs that __won't__ be included: */
	/*                                                      */
	c->nuclear.sibp.ex+=nsp;
	
	return 2+no;
}


/*                                                                    */
/* CountIncludedNuclearPedigreeParent(): return type is               */
/* void because we know without counting that both parents need to be */
/* counted                                                            */
/*                                                                    */
void CountIncludedNuclearPedigreeParent(PCONTROL c,PINDIVIDUAL p){


	if(p->db){
		/*                                                */
		/* normally connected individual in the database: */
		/*                                                */
		if(p->excluded){
			/*                                                              */
			/* An excluded person in the database, known to have kids,      */
			/* so they must be included in output because they're a parent: */
			/*                                                              */

			if(!p->counted) c->counters.dex.in++;
			c->nuclear.dex.in++;
			
		}else{
			/*                                            */
			/* not an excluded individual: a regular,     */
			/* individual: classify based on whether they */
			/* have data or not:                          */
			/*                                            */
			if(p->hasdata){
				if(!p->counted) c->counters.din.in++;
				c->nuclear.din.in++;

			}else{
				if(!p->counted) c->counters.dno.in++;
				c->nuclear.dno.in++;				
			}
		}
	}else{
		/*                                                             */
		/* virtual individual, not in database.  These individuals     */
		/* are inserted by Madeline in order to obtain proper pedigree */
		/* structure -- inclusion of individual is mandatory           */
		/*                                                             */
		if(!p->counted) c->counters.vir.in++;
		c->nuclear.vir.in++;
	}
	/*                             */
	/* flag individual as counted: */
	/*                             */
	p->counted++;
	
	return;
}



/*                                       */
/* CountIncludedNuclearPedigreeOffspring */
/*                                       */
int CountIncludedNuclearPedigreeOffspring(PCONTROL c,PINDIVIDUAL p){

	/*                             */
	/* flag individual as counted: */
	/*                             */
	p->counted++;
	

	if(p->db){
		/*                                                */
		/* normally connected individual in the database: */
		/*                                                */
		if(p->excluded){
			/*                                                               */
			/* An excluded person in the database.  Since they are terminal, */
			/* they are to be excluded from output:                          */
			/*                                                               */
			c->counters.dex.ex++;
			c->nuclear.dex.ex++;
			
			return 0;			

		}else{
			/*                                            */
			/* not an excluded individual: a regular,     */
			/* individual: classify based on whether they */
			/* have data or not:                          */
			/*                                            */
			if(p->hasdata){
				c->counters.din.in++;
				c->nuclear.din.in++;

				return 1;

			}else{
				/*                                                  */
				/* since they are terminal, they are to be excluded */
				/* from output:                                     */
				/*                                                  */
				c->counters.dno.ex++; /* don't double count         */
				c->nuclear.dno.ex++;
				
				return 0;
			}
		}
	}else{
		/*                                                                 */
		/* virtual individual, not in database.  These individuals         */
		/* are inserted by Madeline in order to obtain proper pedigree     */
		/* structure.  When appearing as an offspring in a nuclear family, */
		/* they don't have data and should be excluded:                    */
		/*                                                                 */
		c->counters.vir.ex++;
		c->nuclear.vir.ex++;

		return 0;
	}
}


/*                                                            */
/* CountExcludedNuclearPedigreeParent():                      */
/*                                                            */
/* "included" means they were included in the previous level: */
/*                                                            */
void CountExcludedNuclearPedigreeParent(PCONTROL c,PINDIVIDUAL p){

	if(p->db){
		/*                                                */
		/* normally connected individual in the database: */
		/*                                                */
		if(p->excluded){
			/*                                                    */
			/* An excluded person in the database, here a parent: */
			/* so they must be included in output:                */
			/*                                                    */
			if(!p->counted) c->counters.dex.ex++;
			c->nuclear.dex.ex++;
			
		}else{
			/*                                            */
			/* not an excluded individual: a regular,     */
			/* individual: classify based on whether they */
			/* have data or not:                          */
			/*                                            */
			if(p->hasdata){
				if(!p->counted) c->counters.din.ex++;
				c->nuclear.din.ex++;

			}else{
				if(!p->counted) c->counters.dno.ex++;
				c->nuclear.dno.ex++;
				
			}
		}
	}else{
		/*                                                             */
		/* virtual individual, not in database.  These individuals     */
		/* are inserted by Madeline in order to obtain proper pedigree */
		/* structure -- inclusion of individual is mandatory           */
		/*                                                             */
		if(!p->counted) c->counters.vir.ex++;
		c->nuclear.vir.ex++;
	}
	/*                             */
	/* flag individual as counted: */
	/*                             */
	p->counted++;
}


/*                                       */
/* CountExcludedNuclearPedigreeOffspring */
/*                                       */
int CountExcludedNuclearPedigreeOffspring(PCONTROL c,PINDIVIDUAL p){

	/*                             */
	/* flag individual as counted: */
	/*                             */
	p->counted++;

	if(p->db){
		/*                                                */
		/* normally connected individual in the database: */
		/*                                                */
		if(p->excluded){
			/*                                                               */
			/* An excluded person in the database.  Since they are terminal, */
			/* they are to be excluded from output:                          */
			/*                                                               */
			c->counters.dex.ex++;
			c->nuclear.dex.ex++;
			
			return 0;			

		}else{
			/*                                            */
			/* not an excluded individual: a regular,     */
			/* individual: classify based on whether they */
			/* have data or not:                          */
			/*                                            */
			if(p->hasdata){
				c->counters.din.ex++;
				c->nuclear.din.ex++;

				return 1;

			}else{
				/*                                                  */
				/* since they are terminal, they are to be excluded */
				/* from output:                                     */
				/*                                                  */
				c->counters.dno.ex++; /* don't double count         */
				c->nuclear.dno.ex++;
				
				return 0;
			}
		}
	}else{
		/*                                                                 */
		/* virtual individual, not in database.  These individuals         */
		/* are inserted by Madeline in order to obtain proper pedigree     */
		/* structure.  When appearing as an offspring in a nuclear family, */
		/* they don't have data and should be excluded:                    */
		/*                                                                 */
		c->counters.vir.ex++;
		c->nuclear.vir.ex++;

		return 1;
	}
}

/*                                      */
/* CountExcludedUnattachedIndividual(): */
/*                                      */
void CountExcludedUnattachedIndividual(PCONTROL c,PINDIVIDUAL p){

	if(p->unconnected){
		c->counters.flt.ex++;
		c->nuclear.flt.ex++;
	}else if(p->csp){
		c->counters.csp.ex++;
		c->nuclear.csp.ex++;
	}
}


/*                                                            */
/* OffspringHasData: this routine assumes that the p->hasdata */
/* flags have already been set:                               */
/*                                                            */
/* Returns 1 if AT LEAST ONE offspring is typed.              */
/*                                                            */
int OffspringHasData(PINDIVIDUAL p){

	int i,j;
	PINDIVIDUAL sp;
	
	if(p->female){

		for(i=0;i<p->nk;i++)
			if(p->k[i]->hasdata) return 1;

	}else{
	
		for(j=0;j<p->na;j++){
			sp=p->a[j];
			for(i=0;i<sp->nk;i++)
				if(sp->k[i]->hasdata) return 1;
		}       
	}
	return 0;
}

/*                                                   */
/* OffspringOfPairHasData(): if one of the offspring */
/* of the s1-s2 mating has data, then returns true:  */
/*                                                   */
/*                                                   */
int OffspringOfPairHasData(PINDIVIDUAL s1,PINDIVIDUAL s2){

	PINDIVIDUAL mom; /* pointer to the female */
	PINDIVIDUAL dad;   /* pointer to the male */
	int i;
	
	if(s1->female){
	
		mom=s1;
		dad=s2;
	}else{
		mom=s2;
		dad=s1;
	}
		
	for(i=0;i<mom->nk;i++){
		/*                                    */
		/* restrict to shared offspring only: */
		/*                                    */
		if(mom->k[i]->f==dad && mom->k[i]->hasdata) return 1;
	}
	/*                         */
	/* no offspring have data: */
	/*                         */
	return 0;
}



/*                                                            */
/* OffspringOfPairHasPhenotypeData(): if one of the offspring */
/* of the s1-s2 mating has data, then returns true:           */
/*                                                            */
/*                                                            */
int OffspringOfPairHasPhenotypeData(PINDIVIDUAL s1,PINDIVIDUAL s2){

	PINDIVIDUAL mom; /* pointer to the female */
	PINDIVIDUAL dad;   /* pointer to the male */
	int i;
	
	if(s1->female){
	
		mom=s1;
		dad=s2;
	}else{
		mom=s2;
		dad=s1;
	}
		
	for(i=0;i<mom->nk;i++){
		/*                                    */
		/* restrict to shared offspring only: */
		/*                                    */
		if(mom->k[i]->f==dad && mom->k[i]->haspdata) return 1;
	}
	/*                         */
	/* no offspring have data: */
	/*                         */
	return 0;
}



/*                                                      */
/* SpouseHasData(): returns 1 if one of an individual's */
/* spouse's has data.  This routine assumes that        */
/* p->hasdata flags have already been set               */
/*                                                      */
int SpouseHasData(PINDIVIDUAL p){

	int i;
	
	for(i=0;i<p->na;i++)
		if(p->a[i]->hasdata) return 1;
	return 0;
}


/*                           */
/* void ResetCoreCounters(): */
/*                           */
void ResetCoreCounters(PCORECOUNTERS cc){

	cc->pedg.in  =0; /* pedigrees included */
	cc->pedg.ex  =0; /* pedigrees excluded */
	
	cc->sibp.in  =0; /* sib pairs included */
	cc->sibp.ex  =0; /* sib pairs excluded */

	cc->din.in=0; /* data-included indiv. who are included in output                 */
	cc->din.ex=0; /* data-included indiv. who are excluded from output               */
	cc->dno.in=0; /* indv. who have no data in database who are included in output   */
	cc->dno.ex=0; /* indv. who have no data in database who are excluded from output */
	cc->dex.in=0; /* data-excluded indiv. who are included in output                 */
	cc->dex.ex=0; /* data-excluded indiv. who are excluded from output               */
	cc->csp.in=0; /* childless spouses who are included in output                    */
	cc->csp.ex=0; /* childless spouses who are excluded from output                  */
	cc->flt.in=0; /* unattached floaters who are included in output                  */
	cc->flt.ex=0; /* unattached floaters who are excluded from output                */
	cc->vir.in=0; /* virtual dummied-ins who are included in output                  */
	cc->vir.ex=0; /* virtual dummied-ins who are excluded from output                */

}

/*                                                  */
/* ResetCounters(): Resets the bookkeeping counters */
/*                                                  */
void ResetCounters(PCONTROL c){

	ResetCoreCounters(&c->counters);
	ResetCoreCounters(&c->nuclear);
	
	/*                                                           */
	/* sequential IDs now set elsewhere:                         */
	/*                                                           */
	/*c->psid=1;         // pedigree   sequential id start value */
	/*c->isid=1;         // individual sequential id start value */
	
	/*                                                                    */
	/* reset the general tally counters used by VIEW, EXCLUDE, UNEXCLUDE: */
	/*                                                                    */
	ResetTallyCounters(c);
	
}


