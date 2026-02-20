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
/* "allelefrequencies.h" is part of the Madeline software distribution.              */
/* Written by Edward H. Trager.                                                      */
/*                                                                                   */
/* COPYRIGHT (C) 2003 THE REGENTS OF THE UNIVERSITY OF MICHIGAN,                     */
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
/* Last modified by Ed Trager, May 2003.                                             */
/*                                                                                   */
/* ********************************************************************************* */
/* 2003.05.15.ET */

#ifndef CONTROL_INCLUDED
#include "control.h"
#endif

#ifndef NDBF_INCLUDED
#include "ndbf.h"
#endif

/*                               */
/* CalculateAlleleFrequencies(): */
/*                               */
/* Uses simple gene counting:    */
void CalculateAlleleFrequencies(PDBF db);

/*                        */
/* ReuseSavedMarkerVector */
/*                        */
int ReuseSavedMarkerVector(PCONTROL c);

/*                                                              */
/*                                                              */
/* GetMarkerInformation() processes the ith field of a database */
/* ,if that field is a marker field, to obtain allele           */
/* frequency distribution.                                      */
/*                                                              */
/*                                                              */
/*                                                              */
void GetMarkerInformation(PDBF db,int idx);

/*                                                               */
/* FreeSavedMarkerVector(db): Frees the c->m vector, if present: */
/*                                                               */
void FreeSavedMarkerVector(PCONTROL c);

/*                                                */
/*                                                */
/* Read(): read allele frequencies from a table.  */
/*                                                */
/*                                                */
void Read(void);

/*                                                */
/*                                                */
/* Save(): save allele frequencies to a table.    */
/*                                                */
/*                                                */
void Save(void);

/*                                  */
/* OutputMarkerAlleleFrequencyTable */
/*                                  */
void OutputMarkerAlleleFrequencyTable(PCONTROL c,int i,FILE *fout);

/*                                          */
/*                                          */
/* OutputMarkerAlleleFrequencyRow           */
/*                                          */
void OutputMarkerAlleleFrequencyRow(FILE *fout,PMARKER m,PALLELE a,int s);

/*                         */
/* ShowAlleleFrequencies() */
/*                         */
void ListAlleleFrequency(PALLELE a,int s);
void ShowAlleleFrequencies(PCONTROL c);


#define ALLELE_FREQS_INCLUDED
