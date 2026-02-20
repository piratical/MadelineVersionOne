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
/* "simwalk_format.h" is part of the Madeline software distribution.                 */
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
/* simwalk_format.h                                                         */
/*                                                                          */
/* history: 2003.02.27.ET initiated                                         */
/*                                                                          */
/*                                                                          */
#ifndef CONTROL_INCLUDED
#include "control.h"
#endif

#ifndef PEDIGREE_INCLUDED
#include "pedigree.h"
#endif


/*                                                     */
/* SimwalkLocusHeaderProcessor: very similar to Mendel */
/*                                                     */
void SimwalkLocusHeaderProcessor(PCONTROL c, char *fn);

/*                                                                      */
/* CreateSimwalkBATCH2ControlFile()                                     */
/*                                                                      */
/* fn: name of the "BATCH2.DAT" file for Simwalk (i.e., "BATCH2.DAT" !) */
/* base_fn: base name for the analysis files, to which we will append   */
/* .ped, .map, .loc, etc.                                               */
/*                                                                      */
void CreateSimwalkBATCH2ControlFile(char *fn,char *base_fn,char *pedigree_fn);

/*                                                  */
/* CreateSimwalkMapFile(): Prints out a file        */
/* containing a list of markers in map order        */
/* with the recombination fractions between each    */
/* pair of entries.                                 */
/*                                                  */
/* WARNING: As currently written, this ASSUMES the  */
/*          map is in Kosambi cM. Normally, that is */
/*          indeed the case ...                     */
/*                                                  */
void CreateSimwalkMapFile(PCONTROL c,const char *fn);

/*                             */
/* SimwalkDataHeaderProcessor: */
/* Almost identical to Mendel  */
/*                             */
void SimwalkDataHeaderProcessor(PCONTROL c, char *fn);

/*                                                  */
/*                                                  */
/* SimwalkOutput(): outputs data almost identical   */
/* to Generic Mendel                                */
/*                                                  */
void SimwalkOutput(PCONTROL c,PPEDIGREE pd);

/*                                     */
/*                                     */
/* OutputSimwalkIndividualRecord()     */
/*                                     */
/* Almost identical to Generic MENDEL  */
/*                                     */
void OutputSimwalkIndividualRecord(PCONTROL c,PINDIVIDUAL p);

