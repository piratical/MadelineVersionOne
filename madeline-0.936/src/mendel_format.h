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
/* "mendel_format.h" is part of the Madeline software distribution.                  */
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
/* mendel_format.h                                                          */
/*                                                                          */
/* history: 2003.02.27.ET initiated                                         */
/*                                                                          */
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

extern char gMendelOutputMissingValue[];

/*                                                      */
/*                                                      */
/* OutputMendelMarker()                                 */
/*                                                      */
/* i is now the rank index, not the actual field index: */
/*                                                      */
void OutputMendelMarker(PCONTROL c,int i);

/*                                          */
/*                                          */
/* OutputMendelMarkerAllele: generic format */
/*                                          */
void OutputMendelMarkerAllele(FILE *fp,PALLELE a,int s);

/*                                                      */
/*                                                      */
/* MendelDataHeaderProcessor(): Used for Generic Mendel */
/*                                                      */
/*                                                      */
void MendelDataHeaderProcessor(PCONTROL c, char *fn);

/*                                                  */
/*                                                  */
/* MendelOutput(): outputs data for Generic Mendel: */
/*                                                  */
/* Used for: Generic Mendel and Relpair outputs     */
/*                                                  */
void MendelOutput(PCONTROL c,PPEDIGREE pd);

/*                                     */
/*                                     */
/* OutputMendelIndividualRecord()      */
/*                                     */
/* -> Can be used for Mendel & Fisher: */
/*                                     */
void OutputMendelIndividualRecord(PCONTROL c,PINDIVIDUAL p);

/*                                                      */
/* OutputMendelField():                                 */
/*                                                      */
/* c: pointer to CONTROL structure                      */
/* p: pointer to INDIVIDUAL                             */
/* i: field index in database                           */
/* j: flag, 1 for genotype field, 0 for phenotype field */
/*                                                      */
void OutputMendelField(PCONTROL c,PINDIVIDUAL p,int i,int j);
