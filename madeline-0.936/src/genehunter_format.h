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
/* "genehunter_format.h" is part of the Madeline software distribution.              */
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
/* genehunter_format.h                                                      */
/*                                                                          */
/* history:  These routines were originally stuffed into madeline.c.        */
/*           They have now been segregated out in an effort to streamline   */
/*           code maintenance, management, and intelligibility.             */
/*           -2003.02.28.ET                                                 */
/*                                                                          */

#ifndef CONTROL_INCLUDED
#include "control.h"
#endif

#ifndef MARKER_INCLUDED
#include "marker.h"
#endif

#ifndef PEDIGREE_INCLUDED
#include "pedigree.h"
#endif

/*                                   */
/* OutputGenehunterLocusFileHeader() */
/*                                   */
void OutputGenehunterLocusFileHeader(PCONTROL c,char *fn);

/*                                    */
/* OutputGenehunterLocusFileFooter(): */
/*                                    */
void OutputGenehunterLocusFileFooter(PCONTROL c);

/*                          */
/*                          */
/* OutputGenehunterMarker() */
/*                          */
/*                          */
void OutputGenehunterMarker(PCONTROL c,int i);

/*                                              */
/*                                              */
/* OutputGenehunterMarkerAllele: generic format */
/*                                              */
void OutputGenehunterMarkerAllele(FILE *fp,PMARKER m,PALLELE a,int s);

/*                                   */
/* GENEHUNTER data header processor: */
/*                                   */
/*                                   */
void GenehunterDataHeaderProcessor(PCONTROL c,char *fn);

/*                   */
/* GenehunterOutput: */
/*                   */
void GenehunterOutput(PCONTROL c,PPEDIGREE pd);

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
void OutputGenehunterIndividualRecord(PCONTROL c,PPEDIGREE pd,PINDIVIDUAL p);

