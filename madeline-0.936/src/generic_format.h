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
/* "generic_format.h" is part of the Madeline software distribution.                 */
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
/* generic_format.h                                                         */
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

/*                                 */
/* GENERIC LocusHeaderProcessor(): */
/*                                 */
/*                                 */
void GenericLocusHeaderProcessor(PCONTROL c,char *fn);

/*                                 */
/* GENERIC LocusFooterProcessor(): */
/*                                 */
/*                                 */
void GenericLocusFooterProcessor(PCONTROL c);

/*                                             */
/*                                             */
/* OutputGenericMarker()                       */
/*                                             */
/* - i is the marker rank, not the true index: */
/*                                             */
void OutputGenericMarker(PCONTROL c,int i);

/*                                           */
/*                                           */
/* OutputGenericMarkerAllele: generic format */
/*                                           */
void OutputGenericMarkerAllele(FILE *fp,PALLELE a,int s);

/*                                            */
/*                                            */
/* GENERIC data header and footer processors: */
/*                                            */
/*                                            */
void GenericDataHeaderProcessor(PCONTROL c,char *fn);

/*                               */
/* GenericDataFooterProcessor(): */
/*                               */
void GenericDataFooterProcessor(PCONTROL c);

#define GENERIC_FORMAT_INCLUDED
