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
/* "integrity.h" is part of the Madeline software distribution.                      */
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
/* integrity.h                                                                        */
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
/* effort and integrity.c was born in May, 2002                                       */
/*                                                                                    */

/*           */
/* INCLUDES: */
/*           */

#ifndef CONTROL_INCLUDED
#include "control.h"
#endif

#ifndef PEDIGREE_INCLUDED
#include "pedigree.h"
#endif

/* MAXKIDS, used by integrity.c, is in config.h: */
#include "config.h"

/*          */
/* DEFINES: */
/*          */
#define ALLELES_SET_OK                    0 /* No error: alleles filled or checked OK */
#define ALLELES_INCONSISTENT              1 /* Case where father and mother untyped   */
#define TOO_MANY_ALLELES                  2 /* Too many alleles w/ or w/o homozygotes */
#define ALLELES_INCONSISTENT_WITH_PARENT  4 /* Inconsistent with a typed parent       */
#define ALLELES_INCONSISTENT_WITH_MOTHER  8
#define ALLELES_INCONSISTENT_WITH_FATHER 16

#define CONSISTENT_WITH_BOTH_OPTIONS      3

#define MAX_ALLOWABLE_ALLELES             4

/*           */
/* TYPEDEFS: */
/*           */
typedef unsigned short ALLELELABEL,*PALLELELABEL,**PPALLELELABEL;
typedef unsigned char BOOLEAN;

/*                                 */
/* Inheritance Checking FUNCTIONS: */
/*                                 */
BOOLEAN ReadGenotype(PINDIVIDUAL p,PALLELELABEL a,int GenotypeFieldIndex);
int IsMendelianConsistent(PALLELELABEL m,PALLELELABEL f,PALLELELABEL a);
int IsMendelianConsistentOnPartialReconstruction(PALLELELABEL m,PALLELELABEL f,PALLELELABEL a,BOOLEAN MotherKnown, BOOLEAN FatherKnown);
int SetPossibleParentalGenotypes(PALLELELABEL m,PALLELELABEL f,PALLELELABEL a,BOOLEAN MotherKnown,BOOLEAN FatherKnown);
void CheckNuclearFamilyMarkerInheritance(PINDIVIDUAL mom,PINDIVIDUAL dad,int GenotypeFieldIndex);
void ReportInheritanceInconsistency(int ErrorType,PINDIVIDUAL mom,PINDIVIDUAL dad,int GenotypeFieldIndex,int AlleleCount,int HomozygoteCount,int MatchingCount,PCONTROL g);
int CheckForMatchingParentalAlleles(PALLELELABEL m, PALLELELABEL f,PALLELELABEL pa);
void Check(void);
void CheckPedigreeInheritance(PCONTROL c,PPEDIGREE pd);
void CheckInheritance(PCONTROL c,PINDIVIDUAL p);
void CheckNuclearInheritance(PCONTROL c,PINDIVIDUAL p,PINDIVIDUAL s);
void IntegrityPrint(PCONTROL g,PINDIVIDUAL mom,PINDIVIDUAL dad,int GenotypeFieldIndex);
int PushAllelesOntoStack(ALLELELABEL allele,PALLELELABEL pa,int *AlleleCount);
void PrintMarkerInconsistencySummary(PCONTROL g);
void ClearMarkerInconsistencyCounters(PCONTROL g);
int CountIndividualMarkerInconsistencies(PCONTROL g,PINDIVIDUAL p);

#define INTEGRITY_INCLUDED


