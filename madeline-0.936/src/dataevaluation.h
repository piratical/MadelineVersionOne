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
/* "dataevaluation.h" is part of the Madeline software distribution.                 */
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

#ifndef PEDIGREE_INCLUDED
#include "pedigree.h"
#endif

#ifndef CONTROL_INCLUDED
#include "control.h"
#endif

/*                                                                               */
/* GeneralCaseIndividualHasData() returns 1 if the individual:                   */
/*                                                                               */
/*      (1) Is in the database                                                   */
/*  AND (2) Is not marked for exclusion                                          */
/*  AND (3) Is not a childless spouse or unconnected who will be excluded anyway */
/*  AND (5) Has fields marked for output that are not core fields                */
/*  AND (6) If he has phenotype fields, then they are all non-missing            */
/*  AND (7) If he has genotype fields, then at least one is non-missing          */
/*                                                                               */
/*                                                                               */
int GeneralCaseIndividualHasData(PINDIVIDUAL p);

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
int MendelIndividualHasData(PINDIVIDUAL p);

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
int FisherNonProbandHasData(PINDIVIDUAL p);

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
int SiblinkIndividualHasData(PINDIVIDUAL p);

/*                                                                      */
/* UserM13IndividualHasData()                                           */
/*                                                                      */
/* -> Differs from the GeneralCaseIndividualHasData() in that childless */
/*    spouses and other unconnected individuals are permitted in        */
/*    output:                                                           */
/*                                                                      */
int UserM13IndividualHasData(PINDIVIDUAL p);

/*                                                         */
/* HasPhenotypeData(): returns 1 if individual             */
/* has data in ALL of the phenotype fields that are marked */
/* for output.  Thus an AND relation is used for the       */
/* selected phenotype fields:                              */
/*                                                         */
int HasPhenotypeData(PINDIVIDUAL p);

/*                                                                */
/* HasGenotypeData(): returns 1 if individual has                 */
/* data in AT LEAST ONE of the genotype fields marked for output: */
/* Note that a genotype field MUST BE A CHARACTER FIELD:          */
/*                                                                */
int HasGenotypeData(PINDIVIDUAL p);

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
int GeneralCaseCountIncludedPedigreeIndividual(PCONTROL c,PINDIVIDUAL p);

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
void GeneralCaseCountExcludedPedigreeIndividual(PCONTROL c,PINDIVIDUAL p);

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
int GeneralCasePedigreeHasData(PCONTROL c,PPEDIGREE pd);

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
int MendelCountIncludedPedigreeIndividual(PCONTROL c,PINDIVIDUAL p);

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
int MendelPedigreeHasData(PCONTROL c,PPEDIGREE pd);

/*                                                                       */
/*                                                                       */
/* FisherPedigreeHasData():                                              */
/*                                                                       */
/* -> Much like MendelPedigreeHasData().  The only difference is that    */
/*    if FISHER with ascertainment correction is invoked (FISHER1), then */
/*    we need to find a NON-PROBAND with data.  Other than this, the     */
/*    bookkeeping and who's included is the same as with Mendel:         */
/*                                                                       */
int FisherPedigreeHasData(PCONTROL c,PPEDIGREE pd);

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
int UserM13CountIncludedPedigreeIndividual(PCONTROL c,PINDIVIDUAL p);

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
int UserM13PedigreeHasData(PCONTROL c,PPEDIGREE pd);

/*                              */
/* int ExcludedForGeneralCase() */
/*                              */
int ExcludedForGeneralCase(PINDIVIDUAL p);

/*                         */
/* int ExcludedForMendel() */
/*                         */
int ExcludedForMendel(PINDIVIDUAL p);

/*                          */
/* int ExcludedForUserM13() */
/*                          */
int ExcludedForUserM13(PINDIVIDUAL p);

/*                   */
/* PedigreeTally():  */
/*                   */
void PedigreeTally(PCONTROL c,PPEDIGREE pd);

/*                                                         */
/* ResetTallyCounters(): resets the general tally counters */
/*                       used by VIEW, EXCLUDE, UNEXCLUDE: */
/*                                                         */
void ResetTallyCounters(PCONTROL c);

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
int GenehunterPedigreeHasData(PCONTROL c,PPEDIGREE pd);

/*                                                                */
/* IsAffected(): returns 1 if individual has data and is affected */
/* and is connected:                                              */
/*                                                                */
int IsAffected(PINDIVIDUAL p);

/*                                                                                          */
/* OffspringAndOneParentAffected(): returns true if its an offspring->parent affected pair: */
/*                                                                                          */
int OffspringAndOneParentAffected(PINDIVIDUAL p);

/*                                                                                    */
/* OffspringAndBothParentsAffected(): returns true if its an offspring->parent/parent */
/* affected trio;                                                                     */
/*                                                                                    */
int OffspringAndBothParentsAffected(PINDIVIDUAL p);

/*                       */
/* ResetErrorCounters(): */
/*                       */
void ResetErrorCounters(PCONTROL c);

/*                              */
/* int CrimapPedigreeHasData(): */
/*                              */
int CrimapPedigreeHasData(PCONTROL c,PPEDIGREE pd);

/* */
/* */
/* */
int CrimapCountIncludedPedigreeIndividual(PCONTROL c,PINDIVIDUAL p);

/*                         */
/* int ExcludedForCrimap() */
/*                         */
int ExcludedForCrimap(PINDIVIDUAL p);

/*                                              */
/* NumberOfPairs(): Returns the number of pairs */
/*                  that can be formed from n   */
/*                  individuals                 */
/*                                              */
int NumberOfPairs(int n);

/*                                                              */
/* UserFQTLNuclearFamilyHasData()                               */
/*                                                              */
/* -> s1 and s2 represent the founders of the nuclear pedigree: */
/*                                                              */
int UserFQTLNuclearFamilyHasData(PCONTROL c,PPEDIGREE pd,PINDIVIDUAL s1,PINDIVIDUAL s2);

/*                                 */
/* CountIncludedNuclearPedigree(): */
/*                                 */
int CountIncludedNuclearPedigree(PCONTROL c,PINDIVIDUAL mom,PINDIVIDUAL dad);

/*                                 */
/* CountExcludedNuclearPedigree(): */
/*                                 */
int CountExcludedNuclearPedigree(PCONTROL c,PINDIVIDUAL mom,PINDIVIDUAL dad);

/*                                                                    */
/* CountIncludedNuclearPedigreeParent(): return type is               */
/* void because we know without counting that both parents need to be */
/* counted                                                            */
/*                                                                    */
void CountIncludedNuclearPedigreeParent(PCONTROL c,PINDIVIDUAL p);

/*                                       */
/* CountIncludedNuclearPedigreeOffspring */
/*                                       */
int CountIncludedNuclearPedigreeOffspring(PCONTROL c,PINDIVIDUAL p);

/*                                                            */
/* CountExcludedNuclearPedigreeParent():                      */
/*                                                            */
/* "included" means they were included in the previous level: */
/*                                                            */
void CountExcludedNuclearPedigreeParent(PCONTROL c,PINDIVIDUAL p);

/*                                       */
/* CountExcludedNuclearPedigreeOffspring */
/*                                       */
int CountExcludedNuclearPedigreeOffspring(PCONTROL c,PINDIVIDUAL p);

/*                                      */
/* CountExcludedUnattachedIndividual(): */
/*                                      */
void CountExcludedUnattachedIndividual(PCONTROL c,PINDIVIDUAL p);

/*                                                            */
/* OffspringHasData: this routine assumes that the p->hasdata */
/* flags have already been set:                               */
/*                                                            */
/* Returns 1 if AT LEAST ONE offspring is typed.              */
/*                                                            */
int OffspringHasData(PINDIVIDUAL p);

/*                                                   */
/* OffspringOfPairHasData(): if one of the offspring */
/* of the s1-s2 mating has data, then returns true:  */
/*                                                   */
/*                                                   */
int OffspringOfPairHasData(PINDIVIDUAL s1,PINDIVIDUAL s2);

/*                                                            */
/* OffspringOfPairHasPhenotypeData(): if one of the offspring */
/* of the s1-s2 mating has data, then returns true:           */
/*                                                            */
/*                                                            */
int OffspringOfPairHasPhenotypeData(PINDIVIDUAL s1,PINDIVIDUAL s2);

/*                                                      */
/* SpouseHasData(): returns 1 if one of an individual's */
/* spouse's has data.  This routine assumes that        */
/* p->hasdata flags have already been set               */
/*                                                      */
int SpouseHasData(PINDIVIDUAL p);

/*                      */
/* Reset core counters: */
/*                      */
void ResetCoreCounters(PCORECOUNTERS cc);

/*                                       */
/* Reset all counters (wrapper function) */
/*                                       */
void ResetCounters(PCONTROL c);

#define DATA_EVAL_INCLUDED
