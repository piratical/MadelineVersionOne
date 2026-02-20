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
/* "integrity.c" is part of the Madeline software distribution.                      */
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
/* General Public License for more details.                                          */
/*                                                                                   */
/* You should have received a copy of the GNU General Public                         */
/* License along with this software source code distribution; if not,                */
/* write to the Free Software Foundation, Inc., 59 Temple Place -                    */
/* Suite 330, Boston, MA 02111-1307, USA.                                            */
/*                                                                                   */
/* Last modified by Ed Trager, May, 2002.                                            */
/*                                                                                   */
/* ********************************************************************************* */

/*                                                                                    */
/*                                                                                    */
/* integrity.c                                                                        */
/*                                                                                    */
/* history:                                                                           */
/*                                                                                    */
/* An initial attempt at having Madeline check Mendelian inheritance inconsistencies  */
/* was implemented in the program by the end of August, 1998 while I was              */
/* working for the FUSION Project under Dr. Mike Boehnke at the School of             */
/* Public Health (UM).  However, because programs such as Jeff O'Connell's            */
/* PEDCHECK and MENDEL-based programs were available to do the same thing,            */
/* and because there were other pressing development priorities for Madeline,         */
/* the partially-completed implementation to check for inheritance inconsistencies    */
/* was abandoned.  The CHECK command was never removed from the code, but             */
/* documentation about it was excised.                                                */
/*                                                                                    */
/* Now (May 2002) that development of the program has been renewed, a new emphasis    */
/* has developed to make Madeline more convenient to use.  While working closely      */
/* with colleagues in Dr. Julia Richard's lab at the Kellogg Eye Center (UM) in 2002, */
/* it became apparent to me that a lot of time was being consumed pouring over        */
/* PEDCHECK reports trying to match the relevant output with the original traces and  */
/* allele calling.                                                                    */
/*                                                                                    */
/* I thought it would be more convenient if Madeline could simply highlight           */
/* inconsistent markers directly on pedigree drawings by displaying the original      */
/* allele calls in a highlighted color.  Thus, life was breathed back into the        */
/* effort and integrity.c was born in May, 2002.                                      */
/*                                                                                    */

#include "integrity.h"
#include "logs.h"

#ifndef MADELINE_INCLUDED
#include "madeline.h"
#endif

/*                */
/* ReadGenotype() */
/*                */
BOOLEAN ReadGenotype(PINDIVIDUAL p,PALLELELABEL a,int GenotypeFieldIndex){

	char buffer[14];
	char *genotype,*allele1,*allele2;
	
	/* Return if person is not in database: */
	if(!p->db){
		a[0]=a[1]=0;
		return (BOOLEAN) 0;
	}
	/*               */
	/* Get genotype: */
	/*               */
	genotype=p->db->ReadString(p->db,buffer,GenotypeFieldIndex,p->rci);
	
	if(genotype[0]){
		/*                                     */
		/* get allele1 and allele2 substrings: */
		/*                                     */
		for(allele1=allele2=genotype;*allele2 && *allele2!='/';allele2++);
		*allele2++='\0';
		/*                                       */
		/* store values into ALLELELABEL vector: */
		/*                                       */
		/*                                       */
		a[0]=atoi(allele1); /* first  allele     */
		a[1]=atoi(allele2); /* second allele     */
	}else{
		/*                     */
		/* Store null alleles: */
		/*                     */
		a[0]=a[1]=0;
	}
	/* Return a boolean status flag indicating whether a genotype is present: */
	return (BOOLEAN) a[0] || a[1];
	
}


/*                         */
/* IsMendelianConsistent() */
/*                         */
/* Note how we reference the alternate genotype        */
/* configurations.  Suppose we have offspring with     */
/* the following alleles:                              */ 
/*                                                     */
/*     ( 1 / 3 )    ( 2 / 4 )                          */
/*                                                     */
/* So, parental genotypes are:                         */
/*                                                     */
/* either: ( 1 / 2 ) , ( 3 / 4 )                       */
/* or:     ( 1 / 4 ) , ( 3 / 2 )                       */
/*                                                     */
/* ... and we store and refer to the alleles this way: */
/*                                                     */
/*         Parent 1                  Parent 2          */
/*         +---+---+                 +---+---+         */
/*         |   | 2 |<--m[1]          |   | 4 |<--f[1]  */
/*  m[0]-->| 1 +---+          f[0]-->| 3 +---+         */
/*         |   | 4 |<--f[1]          |   | 2 |<--m[1]  */
/*         +---+---+                 +---+---+         */
/*                                                     */
/* ... where m[] and f[] are containers for the        */
/* alleles of *either* parent and do not specify       */
/* *which* is the mother or the father (despite what   */
/* you think may be implied by the variable names      */
/* "m[]" and "f[]").                                   */
/*                                                     */
/*  The "top" configuration is called "option[0]"      */
/*  The "bottom" configuration is called "option[1]"   */
/*                                                     */
int IsMendelianConsistent(PALLELELABEL m,PALLELELABEL f,PALLELELABEL a){
	
	/* Check whether top or bottom configuration applies:  */
	if( 
	    a[0]==m[0] && a[1]==f[0] ||
	    a[1]==m[0] && a[0]==f[0] ||
	    a[0]==m[1] && a[1]==f[1] ||
	    a[1]==m[1] && a[0]==f[1]
	){
		/* Consistent with both the top and bottom */
		/* reconstruction:                         */
		return CONSISTENT_WITH_BOTH_OPTIONS;
	}else if( 
	    (a[0]==m[0] || a[0]==m[1]) &&
	    (a[1]==f[0] || a[1]==f[1])
	    ||
	    (a[1]==m[0] || a[1]==m[1]) &&
	    (a[0]==f[0] || a[0]==f[1])
	){
		/* Consistent with reconstruction 1: */
		return 1;
	}else if(
	    (a[0]==m[0] || a[0]==f[1]) &&
	    (a[1]==f[0] || a[1]==m[1])
	    ||
	    (a[1]==m[0] || a[1]==f[1]) &&
	    (a[0]==f[0] || a[0]==m[1])
	){
		/* Consistent with reconstruction 2: */
		return 2;
	}else{
		/* Not consistent with either reconstruction */
		return 0;
	}
}

/*                                                */
/* IsMendelianConsistentOnPartialReconstruction() */
/*                                                */
int IsMendelianConsistentOnPartialReconstruction(PALLELELABEL m,PALLELELABEL f,PALLELELABEL a,BOOLEAN MotherKnown,BOOLEAN FatherKnown){
	
	/* Check whether top or bottom configuration applies:  */
	if(m[0] && f[0] && !(m[1] || f[1])){
		/*                                               */
		/* ( X | . )   [ X | . ]                         */
		/* FIRST SLOT ON m and f FILLED                  */
		/*                                               */
		return a[0]==m[0] && a[1]==f[0] || a[1]==m[0] && a[0]==m[1];
		
	}else if(m[0] && m[1] && f[0] && !f[1]){
		/*                                               */
		/* ( X | X )   [ X | . ]                         */
		/* GENOTYPE ON m FILLED, FIRST SLOT ON f FILLED  */
		/*                                               */
		if(MotherKnown){
			return a[0]==m[0] && a[1]==f[0] ||
			       a[1]==m[0] && a[0]==f[0] ||
			       a[0]==m[1] && a[1]==f[0] ||
			       a[1]==m[1] && a[0]==f[0];
		}else{
			return a[0]==m[0] && a[1]==f[0] ||
			       a[1]==m[0] && a[0]==m[1] ||
			       a[0]==m[1] && a[1]==f[0] ||
			       a[1]==m[1] && a[0]==f[0] ||
			       a[0]==m[0] && a[1]==m[1] ||
			       a[1]==m[0] && a[0]==m[1];
		}
	}else if(m[0] && !m[1] && f[0] && f[1]){
		/*                                               */
		/* ( X | . )   [ X | X ]                         */
		/* GENOTYPE ON f FILLED, FIRST SLOT ON m FILLED  */
		/*                                               */
		if(FatherKnown){
			return a[0]==f[0] && a[1]==m[0] ||
			       a[1]==f[0] && a[0]==m[0] ||
			       a[0]==f[1] && a[1]==m[0] ||
			       a[1]==f[1] && a[0]==m[0];
		}else{
			return a[0]==f[0] && a[1]==m[0] ||
			       a[1]==f[0] && a[0]==f[1] ||
			       a[0]==f[1] && a[1]==m[0] ||
			       a[1]==f[1] && a[0]==m[0] ||
			       a[0]==f[0] && a[1]==f[1] ||
			       a[1]==f[0] && a[0]==f[1];
		}       
	}else{
		/* not sufficiently filled in, so return 0: */
		return 0;
	}
}



/*                                                                          */
/* SetPossibleParentalGenotypes():  If parent's are not typed, then set the */
/* possible parental genotypes.                                             */
/*                                                                          */
/* We don't know the true phase for the mother and/or father  unless        */
/* MotherKnown or FatherKnown is true.  So, when MotherKnown and            */
/* FatherKnown are false, m[0] and m[1] don't necessarily represent         */
/* the mom, nor f[0] and f[1] necessarily the father.  In this case, they   */
/* are just containers for the  alleles of either parent -- don't let the   */
/* "m" and "f" of the variable names confuse you into thinking that         */
/* Madeline knows something other programs don't!                           */
/*                                                                          */
int SetPossibleParentalGenotypes(PALLELELABEL m,PALLELELABEL f,PALLELELABEL a,BOOLEAN MotherKnown,BOOLEAN FatherKnown){

	ALLELELABEL AlleleToForward;
	
	/* There's a possibility that the genotype of either the mother or father is known */
	/* which allows us to narrow the possible genotypes of the remaining parent.       */
	/* When one parent is known, then there can only be a single configuration for the */
	/* other parent:                                                                   */
	
	if(MotherKnown){
		/* Mother's genotype has been stored: */
		if(IsMendelianConsistentOnPartialReconstruction(m,f,a,MotherKnown,FatherKnown)){
			return ALLELES_SET_OK;
		}else if(a[0]==m[0] || a[0]==m[1]){
			AlleleToForward=a[1];
		}else if(a[1]==m[0] || a[1]==m[1]){
			AlleleToForward=a[0];
		}else{
			/* Neither a[0] nor a[1] matches to mother */
			return ALLELES_INCONSISTENT_WITH_MOTHER;
		}
		/* Get here if one of the two alleles matches to mother */
		/* AlleleToForward either needs to match with father    */
		/* or be stored in an empty slot on the father:         */
		if(!f[0]){
			/* Slot is empty, so store: */
			f[0]=AlleleToForward;
			return ALLELES_SET_OK;
		}else if(AlleleToForward==f[0]){
			return ALLELES_SET_OK;
		}else if(!f[1]){
			/* Slot 2 is empty, so store: */
			f[1]=AlleleToForward;
			return ALLELES_SET_OK;
		}else if(AlleleToForward==f[1]){
			return ALLELES_SET_OK;
		}else{
			/* AlleleToForward doesn't match either f allele: */
			return ALLELES_INCONSISTENT_WITH_FATHER;
		}
	}else if(FatherKnown){
		/* Father's genotype has been stored: */
		if(IsMendelianConsistentOnPartialReconstruction(m,f,a,MotherKnown,FatherKnown)){
			return ALLELES_SET_OK;
		}else if(a[0]==f[0] || a[0]==f[1]){
			AlleleToForward=a[1];
		}else if(a[1]==f[0] || a[1]==f[1]){
			AlleleToForward=a[0];
		}else{
			/* Neither a[0] nor a[1] matches to father */
			return ALLELES_INCONSISTENT_WITH_FATHER;
		}
		/* Get here if one of the two alleles matches to father  */
		/* AlleleToForward either needs to match with mother     */
		/* or be stored in an empty slot on the mother:          */
		if(!m[0]){
			/* Slot is empty, so store: */
			m[0]=AlleleToForward;
			return ALLELES_SET_OK;
		}else if(AlleleToForward==m[0]){
			return ALLELES_SET_OK;
		}else if(!m[1]){
			/* Slot 2 is empty, so store: */
			m[1]=AlleleToForward;
			return ALLELES_SET_OK;
		}else if(AlleleToForward==m[1]){
			return ALLELES_SET_OK;
		}else{
			/* AlleleToForward doesn't match either m allele: */
			return ALLELES_INCONSISTENT_WITH_MOTHER;
		}
	}else{
		/* Get here if both father and mother are unknown:    */
		/*                                                    */
		/* In this case, we don't know phase and m[] and f[]  */
		/* are just containers for reconstructed parental     */
		/* genotypes, but it doesn't mean "m[]" is the mother */
		/* and "f[]" the father.                              */
		/*                                                    */
		/* The m[] and f[] allele slots may be partially      */
		/* filled, but in this case we know that we will      */
		/* fill m[0] first, then f[0], then m[1] and then     */ 
		/* f[1] in that order:                                */
		/*                                                    */
		if(!m[0]){
			/*                                 */
			/* ( . | . )   [ . | . ]           */
			/* NO SLOTS FILLED                 */
			/*                                 */
			/* This is the easy case where     */
			/* NO SLOTS FILLED, so fill them:  */
			m[0]=a[0];
			f[0]=a[1];
			
		}else if(IsMendelianConsistentOnPartialReconstruction(m,f,a,MotherKnown,FatherKnown)){
			
			return ALLELES_SET_OK;
			
		}else if(m[0] && f[0] && !(m[1] || f[1])){
			/*                                               */
			/* ( X | . )   [ X | . ]                         */
			/* FIRST SLOT ON m and f FILLED                  */
			/*                                               */
			/* One of a[0] or a[1] could match to one of     */
			/* m[0] or f[0], so determine if there is a      */
			/* match (If there is a match, then this in fact */
			/* determines the configuration option too ...)  */
			/*                                               */
			if     (a[0]==m[0] || a[0]==f[0]){
				/* a[0] matches: */
				if(a[0]==m[0]){
					/* matches to m[0]:               */
					/* so if a[1] doesn't match f[0], */
					/* store a[1] on f[1]:            */
					if(a[1]!=f[0]) f[1]=a[1];
					return ALLELES_SET_OK;
				}else{
					/* matches to f[0]: */
					/* so if a[1] doesn't match m[0], */
					/* store a[1] on m[1]:            */
					if(a[1]!=m[0]) m[1]=a[1];
					return ALLELES_SET_OK;
				}
			}else if(a[1]==m[0] || a[1]==f[0]){
				/* a[1] matches: */
				if(a[1]==m[0]){
					/* matches to m[0]:               */
					/* so if a[0] doesn't match f[0], */
					/* store a[0] on f[1]:            */
					if(a[0]!=f[0]) f[1]=a[0];
					return ALLELES_SET_OK;
				}else{
					/* matches to f[0]: */
					/* so if a[0] doesn't match m[0], */
					/* store a[0] on m[1]:            */
					if(a[0]!=m[0]) m[1]=a[0];
					return ALLELES_SET_OK;
				}
			}else{
				/* Neither a[0] nor a[1] match, so store */
				/* a[0] and a[1] on the remaining empty  */
				/* slots:                                */
				m[1]=a[0];
				f[1]=a[1];
			}
		}else if(m[0] && m[1] && f[0] && !f[1]){
			/*                                               */
			/* ( X | X )   [ X | . ]                         */
			/* GENOTYPE ON m FILLED, FIRST SLOT ON f FILLED  */
			/*                                               */
			
			/* First see if a[] matches without filling      */
			/* the empty slot:                               */
			if( 
			    (a[0]==m[0] || a[0]==m[1]) &&
			    (a[1]==f[0])
			    ||
			    (a[1]==m[0] || a[1]==m[1]) &&
			    (a[0]==f[0])
			){
				return ALLELES_SET_OK;
			}else{
				/* One or both alleles don't match. */
				/* If both don't match,  that's an  */
				/* error:                           */
				if(
				   !(a[0]==m[0] || a[0]==m[1] || a[0]==f[0]) && 
				   !(a[1]==m[0] || a[1]==m[1] || a[1]==f[0])
				){
					/* If it's a homozygote, we want to report too many alleles: */
					if(a[0]==a[1]) return TOO_MANY_ALLELES;
					else return ALLELES_INCONSISTENT;
				}else{
					/* Determine which allele doesn't match */
					/* and store that on f[1]:              */
					if(!(a[0]==m[0] || a[0]==m[1] || a[0]==f[0])){
						f[1]=a[0];
						return ALLELES_SET_OK;
					}else{
						f[1]=a[1];
						return ALLELES_SET_OK;
					}
				}
			}
			
		}else if(m[0] && !m[1] && f[0] && f[1]){
			/*                                               */
			/* ( X | . )   [ X | X ]                         */
			/* GENOTYPE ON f FILLED, FIRST SLOT ON m FILLED  */
			/*                                               */

			/* First see if a[] matches without filling      */
			/* the empty slot:                               */
			if( 
			    (a[0]==f[0] || a[0]==f[1]) &&
			    (a[1]==m[0])
			    ||
			    (a[1]==f[0] || a[1]==f[1]) &&
			    (a[0]==m[0])
			){
				return ALLELES_SET_OK;
			}else{
				/* One or both alleles don't match. */
				/* If both don't match,  that's an  */
				/* error:                           */
				if(
				   !(a[0]==f[0] || a[0]==f[1] || a[0]==m[0]) && 
				   !(a[1]==f[0] || a[1]==f[1] || a[1]==m[0])
				){
					/* If it's a homozygote, we want to report too many alleles: */
					if(a[0]==a[1]) return TOO_MANY_ALLELES;
					else return ALLELES_INCONSISTENT;
				}else{
					/* Determine which allele doesn't match */
					/* and store that on f[1]:              */
					if(!(a[0]==f[0] || a[0]==f[1] || a[0]==m[0])){
						m[1]=a[0];
						return ALLELES_SET_OK;
					}else{
						m[1]=a[1];
						return ALLELES_SET_OK;
					}
				}
			}
		}
	}
	/* returns here only if all four alleles for the parents */
	/* have been filled in and there are no inconsistencies  */
	return ALLELES_SET_OK;
}

/*                                                                            */
/* CheckNuclearFamilyMarkerInheritance(): Checks Mendelian inheritance of a   */
/* single marker for a nuclear family originating with person p and spouse s. */
/*                                                                            */
void CheckNuclearFamilyMarkerInheritance(PINDIVIDUAL mom,PINDIVIDUAL dad,int GenotypeFieldIndex){

	ALLELELABEL m[2];
	ALLELELABEL f[2];
	ALLELELABEL a[2]; 
	ALLELELABEL hz[2]={0,0}; /* Homozygote allele stack            */
	ALLELELABEL pa[2]={0,0}; /* Parent's bearing same allele stack */
	ALLELELABEL as[4]={0,0,0,0}; /* Allele count stack */
	BOOLEAN MotherKnown;     /* True if mother's genotype is known */
	BOOLEAN FatherKnown;     /* True if father's genotype is known */
	BOOLEAN option[2]={0,0}; /* Which configuration option         */
	
	int ErrorType=0;
	int HomozygoteCount=0;  /* Number of Homozygotes */
	int MatchingCount=0;    /* Matching of alleles among the parents */
	int AlleleCount=0;      /* Number of alleles seen so far */
	int i;
	int consistent;
	int ConfigurationCanBeChecked=0;
	int ParentalAllelesAreFilled=0;
	//int shift;
	int NotYetReported=0;
	
	extern PCONTROL g;
	
	/* If mom has no kids, then clearly there's naught to do: */
	if(!mom->nk) return;
	
	/* Read parental genotypes for this marker: */
	MotherKnown=ReadGenotype(mom,m,GenotypeFieldIndex);
	FatherKnown=ReadGenotype(dad,f,GenotypeFieldIndex);
	
	/*                                                                    */
	/* If neither parent is typed and there are fewer than two offspring, */
	/* then there's nothing to check, so return:                          */
	/*                                                                    */
	if(!(MotherKnown || FatherKnown) && mom->nk<2) return;
	
	/*                                                      */
	/* Put parental alleles, if present, onto allele stack: */
	/*                                                      */
	if(MotherKnown){
		/* Check if homozygote: */
		if(m[0]==m[1]){
			/* Push allele onto homozygote stack: */
			hz[0]=m[0];
			HomozygoteCount++;
			/* Push homozygote allele onto allele stack: */
			PushAllelesOntoStack(m[0],as,&AlleleCount);
		}else{
			/* Push distinct alleles onto allele stack: */
			PushAllelesOntoStack(m[0],as,&AlleleCount);
			PushAllelesOntoStack(m[1],as,&AlleleCount);
		}
	}
	
	if(FatherKnown){
		/* Check if homozygote: */
		if(f[0]==f[1]){
			/* Push allele onto homozygote stack: */
			if(!hz[0]){
				hz[0]=f[0];
				HomozygoteCount++;
			}else if(f[0]!=hz[0]){
				hz[1]=f[0];
				HomozygoteCount++;
			}
			/* Push homozygote allele onto allele stack: */
			PushAllelesOntoStack(f[0],as,&AlleleCount);
		}else{
			/* Push distinct alleles onto allele stack: */
			PushAllelesOntoStack(f[0],as,&AlleleCount);
			PushAllelesOntoStack(f[1],as,&AlleleCount);
		}
	}
	
	/* Note if both parental alleles are already filled in: */
	if(MotherKnown && FatherKnown){ 
		ParentalAllelesAreFilled++;
		ConfigurationCanBeChecked++;
	}
	
	/*                                                */
	/* LOOP THROUGH THE OFFSPRING                     */
	/* Loop through offspring -- and only             */
	/* look at offspring of this mating!              */
	/*                                                */
	/* NOTE BENE: We can't check for consistency      */
	/* until the iteration *after* completely filling */
	/* in the parental allele slots. That is to say,  */
	/* the iteration of the child which completes the */
	/* filling isn't counted because the orientation  */
	/* of the configuration isn't known until the     */
	/* iteration after that.                          */
	/*                                                */
	
	for(i=0;i<mom->nk;i++){
		
		/* Make sure kid's dad is this dad! */
		if(mom->k[i]->f!=dad) continue;
		/* Read offspring's genotype                                 */
		/*                                                           */
		/* NOTE BENE: In a future version of the program, an         */
		/* untyped offspring should be checked to see whether he/she */
		/* has typed children. Then the program could recurse down   */
		/* to the next level to check inheritance there:             */
		/*                                                           */
		if(!ReadGenotype(mom->k[i],a,GenotypeFieldIndex)) continue;
		
		/*                                  */
		/* Get here if offspring was typed: */
		/* Is offspring a homozygote ?      */
		/*                                  */
		if(a[0]==a[1]){
			/*                      */
			/* Case of a homozygote */
			/*                      */
			if(!hz[0]){
				/* No homozygotes yet recorded */
				hz[0]=a[0];
				HomozygoteCount++;
			}else if(!hz[1] && a[0]!=hz[0]){
				/* One homozygote recorded, but different from current allele: */
				hz[1]=a[1];
				HomozygoteCount++;
			}else if(!(a[0]==hz[0] || a[0]==hz[1])){
				/* Three homozygote alleles: */
				HomozygoteCount++;
				ErrorType = TOO_MANY_ALLELES;
				goto HandleError;
			}
			/* Put Homozygote allele onto allele stack, if necessary: */
			if((ErrorType=PushAllelesOntoStack(a[0],as,&AlleleCount))){
				 goto HandleError;
			}
			
		}else{
			/*                        */
			/* Case of a heterozygote */
			/*                        */
			
			/* Put both alleles on stack, as required: */
			if((ErrorType=PushAllelesOntoStack(a[0],as,&AlleleCount))){
				goto HandleError;
			}
			if((ErrorType=PushAllelesOntoStack(a[1],as,&AlleleCount))){
				goto HandleError;
			}
			
		}
		
		/*                                                                             */
		/* If only one parent is typed and a heterozygous offspring's alleles match    */
		/* that parent exactly, then skip the offspring because it doesn't             */
		/* provide any additional information in a 2-allele system.  If the system is  */
		/* really a 3 allele system, but we've only seen 2 of the alleles so far, then */
		/* this offspring certainly doesn't tell us which of the 2 alleles is the      */
		/* allele that matches between the two parents, so we don't yet know which of  */
		/* the two alleles belongs on the other parent.                                */
		/*                                                                             */
		/* if(a[0]!=a[1] && ((m[0] && m[1] && !f[0] && !f[1]) || (!m[0] && !m[1] && f[0] && f[1]))){ */
		if(a[0]!=a[1] && !ParentalAllelesAreFilled && (MotherKnown || FatherKnown)){
			if( MotherKnown && 
			    ((a[0]==m[0] && a[1]==m[1]) ||
			     (a[0]==m[1] && a[1]==m[0]))
			){
				continue; 
			}
			if( FatherKnown && 
			    ((a[0]==f[0] && a[1]==f[1]) ||
			     (a[0]==f[1] && a[1]==f[0]))
			){
				continue; 
			}
		}
		
		if(!ParentalAllelesAreFilled){
			
			/*                                                                         */
			/* If there are now a total of four alleles in the system and three of     */
			/* the four slots are already filled, then the orientation is already      */
			/* fixed and we can set "ConfigurationCanBeChecked" to true.  The code     */
			/* then tells what we already know -- that the first orientation           */
			/* applies.                                                                */
			/*                                                                         */
			/* Howevever, if there are only three alleles in the system, and three     */
			/* of the four slots are filled, then there still remains the possibility  */
			/* of having a homozygote (in which case the parents would have one allele */
			/* matching between them), so the orientation cannot yet be fixed.         */
			/*                                                                         */
			if( AlleleCount==MAX_ALLOWABLE_ALLELES &&
			    m[0] && f[0] && ( (m[1] && !f[1]) || (!m[1] && f[1]))){
				/*                                                                     */
				/* Since we now know that there are four alleles, then the previous    */
				/* iteration which back filled the third allele on the parents must    */
				/* be counted as "fixing" the configuration of the parents.  Moreover, */
				/* The previous iteration is fixed as "option[0]" (We could not "fix"  */
				/* the configuration on the previous iteration because we had no idea  */
				/* whether a fourth allele would be seen).                             */
				/*                                                                     */
				option[0]++;
				ConfigurationCanBeChecked++;
			}
			
			/*                                                      */
			/* Parents are not completely reconstructed, so fill in */
			/* possible parental genotypes:                         */
			/*                                                      */
			if((ErrorType=SetPossibleParentalGenotypes(m,f,a,MotherKnown,FatherKnown))) goto HandleError;
		}
		
		/*                                                              */
		/* Consistency checking begins once ConfigurationCanBeChecked   */
		/* has been set true:                                           */
		/*                                                              */
		if(ConfigurationCanBeChecked){
			/* Check whether top or bottom configuration applies:  */
			/*                                                     */
			/*  The "top" configuration is "option[0]"             */
			/*  The "bottom" configuration is "option[1]"          */
			/*                                                     */
			consistent=IsMendelianConsistent(m,f,a);
			if(consistent){
				if(consistent!=CONSISTENT_WITH_BOTH_OPTIONS){
					/* Flag whether consistent with reconstruction */
					/* 1 or 2:                                     */
					option[--consistent]++;
					/* Error if both option[0] and option[1]       */
					/* because only one configuration can be valid */
					/* for the nuclear family:                     */
					if(option[0] && option[1]){
						ErrorType = ALLELES_INCONSISTENT;
						goto HandleError;
					}
				}
			}else{
				/* Neither configuration works:           */
				/* This is an error if the parental       */
				/* genotypes are filled either because    */
				/* the parents are known or because of    */
				/* back substitution.  Because one parent */
				/* may be known, we can get here even if  */
				/* there are only four alleles, so we     */
				/* need to check whether it's a problem   */
				/* with too many alleles or not:          */
				ErrorType = ALLELES_INCONSISTENT_WITH_PARENT;
				goto HandleError;
			}
		}
		
		/*                                            */
		/* Record if parental alleles are now filled: */
		/* for use in subsequent iteration:           */
		/*                                            */
		if(m[0] && m[1] && f[0] && f[1]){
			ParentalAllelesAreFilled++;
			ConfigurationCanBeChecked++;
		}
		
	}
	
	/* Check for matching parental alleles which have the same effect */
	/* as homozygotes among the sibs:                                 */
	MatchingCount=CheckForMatchingParentalAlleles(m,f,pa);
	
	/*              */
	/* goto target: */
	/*              */
	HandleError:
	
	if(ErrorType){ 
		
		/*                                        */
		/* FILL IN INTEGRITY FLAG VECTORS         */
		/*                                        */
		/*                                        */
		/* ErrorFlags are:                        */
		/*    4 : 4+ alleles, or just plain       */
		/*        inconsistent                    */
		/*    3 : 3+ alleles w/ 1 homozygote      */
		/*    2 : 2+ alleles w/ 2 homozygotes     */
		/*                                        */
		
		/* The reason for using "NotYetReported"     */
		/* is that an individual may already have    */
		/* been visited if the pedigree has multiple */
		/* founding groups.  So, we only want to     */
		/* report an error once                      */
		
		/* First, do MOM: */
		if(mom->db && !mom->integrity) mom->integrity=NewIntegrityVector(mom->db->fc);
		if(mom->integrity && !mom->integrity[GenotypeFieldIndex]){
			mom->integrity[GenotypeFieldIndex]++;
			NotYetReported++;
		}
		/* Now do DAD: */
		if(dad->db && !dad->integrity) dad->integrity=NewIntegrityVector(dad->db->fc);
		if(dad->integrity && !dad->integrity[GenotypeFieldIndex]){
			dad->integrity[GenotypeFieldIndex]++;
			NotYetReported++;
		}
		/* Finally, do KIDS (but only kids of this mating): */
		for(i=0;i<mom->nk;i++){
			/* Make sure kid's dad is this dad! */
			if(mom->k[i]->f!=dad) continue;
			/* get here if kid is offspring of this mating: */
			if(mom->k[i]->db && !mom->k[i]->integrity) mom->k[i]->integrity=NewIntegrityVector(mom->k[i]->db->fc);
			if(mom->k[i]->integrity && !mom->k[i]->integrity[GenotypeFieldIndex]){
				mom->k[i]->integrity[GenotypeFieldIndex]++;
				NotYetReported++;
			}
		}
		
		/* Flag marker: */
		/* Nuclear family count: */
		g->db->m[GenotypeFieldIndex].nmin++;
		
		
		if(NotYetReported) 
			ReportInheritanceInconsistency(ErrorType,mom,dad,GenotypeFieldIndex,AlleleCount,HomozygoteCount,MatchingCount,g);
		
	}    
	                                                              
}

/*                                  */
/* ReportInheritanceInconsistency() */
/*                                  */
void ReportInheritanceInconsistency(int ErrorType,PINDIVIDUAL mom,PINDIVIDUAL dad,int GenotypeFieldIndex,int AlleleCount,int HomozygoteCount,int MatchingCount,PCONTROL g){
	
	/*                                                                  */
	/* NOTE BENE: Currently Madeline simply reports inheritance errors  */
	/*            in a simple tabular format without specifying the     */
	/*            error type.  As a result, some of the parameters      */
	/*            passed into this function are not being used          */
	/*            at the current time.                                  */
	
	IntegrityPrint(g,mom,dad,GenotypeFieldIndex);
	
}

/*                                                      */
/*  CheckForMatchingParentalAlleles()                   */
/*  is used to check when a homozygous offspring        */
/*  has not yet been seen, but backfilling of parental  */
/*  alleles can only work if the parents have a         */
/*  matching allele.                                    */
/*                                                      */
/*  If parents have one matching allele, then of course */
/*  there can only be 3 unique alleles.  If 2 matching, */
/*  then of course only 2 unique alleles in the sibship */
/*                                                      */
int CheckForMatchingParentalAlleles(PALLELELABEL m, PALLELELABEL f,PALLELELABEL pa){
	
	int MatchingCount=0;
	
	if      (m[0] && m[1] && f[0] && !f[1]){
		/*                                               */
		/* ( X | X )   [ X | . ]                         */
		/* GENOTYPE ON m FILLED, FIRST SLOT ON f FILLED  */
		/*                                               */
		if(m[0]==f[0] || m[1]==f[0]){
			/* Matching allele: */
			 pa[0]=f[0];
			return 1;
		}
	}else if(f[0] && f[1] && m[0] && !m[1]){
		/*                                               */
		/* ( X | . )   [ X | X ]                         */
		/* GENOTYPE ON f FILLED, FIRST SLOT ON m FILLED  */
		/*                                               */
		if(f[0]==m[0] || f[1]==m[0]){
			/* Matching allele! */
			pa[0]=m[0];
			return 1;
		}
	}else if(m[0] && m[1] && f[0] && f[1]){
		/*                                              */
		/* ( X | X ), [ X | X ]                         */
		/* BOTH f AND  m GENOTYPES FILLED               */
		if(m[0]==f[0] || m[0]==f[1]){
			pa[0]=m[0];
			MatchingCount++;
		}
		if(m[1]==f[0] || m[1]==f[1]){
			if(pa[0]){
				if(m[1]!=pa[0]) pa[1]=m[1];
			}else{
				pa[0]=m[1];
			}
			MatchingCount++;
		}
		return MatchingCount;
	}
	
	/* This is to please the compiler   */
	/* which doesn't see that we have   */
	/* a return within each if clause:  */
	/* -- just so we don't get pedantic */
	/* warnings:                        */
	
	return MatchingCount;
}

/*           */
/* Check():  */
/*           */
void Check(void){
	
	extern PCONTROL g;
	/*                          */
	/* database must be opened: */
	/*                          */
	if(!g->db)
		MadelineErrorReport("Check(): A pedigree table must be opened to use this command.");
	
	/*                  */
	/* get the keyword: */
	/*                  */
	GetToken();

	/*                             */
	/* switch recognized keywords: */
	/*	                            */
	switch(pT->i){
	case END:
		MadelineErrorReport("Check(): INHERITANCE keyword expected");
		break;
	case INHRK:
		
		/* Clear the PMARKER inconsistency counters: */
		ClearMarkerInconsistencyCounters(g);

		lprintf("Checking simple Mendelian inheritance in nuclear families... :\n");
		lprintf("==============================================================\n");
		lprintf("Inheritance inconsistency:%cPEDIGREE%cMOTHER%cFATHER%cMARKER\n",
			g->delimiter,g->delimiter,g->delimiter,g->delimiter
		);
		lprintf("--------------------------%c--------%c------%c------%c------\n",
			g->delimiter,g->delimiter,g->delimiter,g->delimiter
		);
		TraverseBinaryPedigreeTree2(g,g->pd,CheckPedigreeInheritance);
		lprintf("==============================================================\n");
		
		PrintMarkerInconsistencySummary(g);
		
		break;
	default:
		MadelineErrorReport("Check(): Keyword \"%s\" not recognized for this command.",pT->e);
		break;
	}
}


/*                             */
/* CheckPedigreeInheritance(): */
/*                             */
void CheckPedigreeInheritance(PCONTROL c,PPEDIGREE pd){

	int i;
	
	for(i=0;i<pd->nfm;i++){
		CheckInheritance(c,pd->fm[i]);
	}
}


/*                     */
/* CheckInheritance(): */
/*                     */
void CheckInheritance(PCONTROL c,PINDIVIDUAL p){

	int i;

	if(!p) return;
	/*                               */
	/* return if no spouse and kids: */
	/*                               */
	if(!p->na) return;
	/*                                  */
	/* run through all spouse pairings: */
	/*                                  */
	for(i=0;i<p->na;i++) CheckNuclearInheritance(c,p,p->a[i]);
	
}


/*                                                                         */
/* CheckNuclearInheritance(): Checks inheritance in a                      */
/* nuclear family originating with person p and spouse s.                  */
/*                                                                         */
void CheckNuclearInheritance(PCONTROL c,PINDIVIDUAL p,PINDIVIDUAL s){

	PINDIVIDUAL mom; /* pointer to the female */
	PINDIVIDUAL dad; /* pointer to the male   */
	int i,GenotypeFieldIndex;
	
	/* Determine which is mom and which is dad: */
	if(p->female){
		mom=p;
		dad=s;
	}else{
		mom=s;
		dad=p;
	}
	
	/*                                                  */
	/* Run through the markers for this nuclear family: */
	/*                                                  */
	for(i=0;i<c->db->n.go;i++){
		/*                       */
		/* Get true field index: */
		/*                       */
		GenotypeFieldIndex=c->db->fi->g[i];
		/*                                              */
		/* Check Mendelian inheritance for this marker: */
		/*                                              */ 
		CheckNuclearFamilyMarkerInheritance(mom,dad,GenotypeFieldIndex);
		
	}
	
	/*                        */
	/* Recurse down children: */
	/*                        */
	for(i=0;i<mom->nk;i++) CheckInheritance(c,mom->k[i]);
	
}


/*                  */
/* IntegrityPrint() */
/*                  */
void IntegrityPrint(PCONTROL g,PINDIVIDUAL mom,PINDIVIDUAL dad,int GenotypeFieldIndex){
	
	if(g->AutoCheckInheritance){
		eprintf(INHERITANCEFLAG,"%c%s%c%s%c%s%c%s\n",
			g->delimiter,
			mom->pd->fid,g->delimiter,
			mom->id,g->delimiter,
			dad->id,g->delimiter,
			g->db->FieldName(g->db,GenotypeFieldIndex)
		);
		
	}else{
		
		lprintf("Inheritance inconsistency:%c%s%c%s%c%s%c%s\n",
			g->delimiter,
			mom->pd->fid,g->delimiter,
			mom->id,g->delimiter,
			dad->id,g->delimiter,
			g->db->FieldName(g->db,GenotypeFieldIndex)
		);
		
	}
}

/*                        */
/* PushAllelesOntoStack() */
/*                        */
int PushAllelesOntoStack(ALLELELABEL allele,PALLELELABEL pa,int *AlleleCount){
	
	int i;
	
	/* See if allele matches to an allele that's already there: */
	for(i=0;i<*AlleleCount;i++){
		
		if(pa[i]==allele) return ALLELES_SET_OK;
	}
	 
	/* Get here if allele doesn't match: */
	/* so store it on stack if there is  */
	/* room:                             */
	if(*AlleleCount<MAX_ALLOWABLE_ALLELES){
		
		pa[(*AlleleCount)++]=allele;
		
		return ALLELES_SET_OK;
		
	}else{
		
		return TOO_MANY_ALLELES;
		
	}
}


/*                                   */
/* PrintMarkerInconsistencySummary() */
/*                                   */
void PrintMarkerInconsistencySummary(PCONTROL g){
	
	int i,ii,j;
	PMARKERMAP p;
	char *fn;
	
	lprintf("\n================================================\n");
	lprintf("Summary of Mendelian Inheritance Inconsistencies\n");
	lprintf("                   by Marker\n");
	lprintf("================================================\n");
	if(g->mp){
		lprintf(" #  %1$cCHR.%1$cRANK%1$cMARKERNAME                      %1$cNUCLEAR FAMILIES\n",g->delimiter);
		lprintf("----%1$c----%1$c----%1$c--------------------------------%1$c----------------\n",g->delimiter);
	}else{
		lprintf(" #  %1$cMARKERNAME                      %1$cNUCLEAR FAMILIES\n",g->delimiter);
		lprintf("----%1$c--------------------------------%1$c----------------\n",g->delimiter);
	}
	for(i=j=0;i<g->db->n.g;i++){
		
		/* get the actual genotype field index: */
		ii=g->db->fi->g[i];
		
		if(g->db->m[ii].nmin){
			/* If the marker map is present, also list Chromosome number and rank: */
			if(g->mp){
				/* print table showing chromosome number and rank: */
				p=LookupMarkerMap(g->mp,(fn=g->db->FieldName(g->db,ii)));
				lprintf("%3i.%c%02i%c%3i%c%-32s%c%9i\n",
				        ii+1,
				        g->delimiter,
				        p->ch,
				        g->delimiter,
				        p->i,
				        g->delimiter,
				        fn,
				        g->delimiter,
				        g->db->m[ii].nmin
				);
				
			}else{
				/* print table without showing chromosome number and rank: */
				lprintf("%3i.%c%-15s%c%9i\n",
				        ii+1,
				        g->delimiter,
				        g->db->FieldName(g->db,ii),
				        g->delimiter,
				        g->db->m[ii].nmin
				);
			}
			j++;
		}
	}
	
	lprintf("------------------------------------------------\n");
	lprintf("Inconsistencies present among %i of %i markers.\n",j,g->db->n.g);
	lprintf("================================================\n\n");
}

/*                                  */
/* ClearMarkerInconsistencyCounters */
/*                                  */
void ClearMarkerInconsistencyCounters(PCONTROL g){
	
	int i,ii;
	
	for(i=0;i<g->db->n.go;i++){
		
		/* get the actual genotype field index: */
		ii=g->db->fi->g[i];
		
		/* clear the flags: */
		g->db->m[ii].nmip=g->db->m[ii].nmin=0;
		
	}
	
}


int CountIndividualMarkerInconsistencies(PCONTROL g,PINDIVIDUAL p){
	
	int i,j;
	
	/* Case where individual is not in the database (virtual individual): */
	if(!p->db) return 0;
	/* Case where individual has no inheritance inconsistencies: */
	if(!p->integrity) return 0;
	
	/* Case where inconsistencies are present: */
	for(i=j=0;i<g->db->fc;i++) if(p->integrity[i]) j++;
	
	return j;
	
}

