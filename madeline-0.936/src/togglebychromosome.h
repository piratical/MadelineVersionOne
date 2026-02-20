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
/* "togglebychromosome.h" is part of the Madeline software distribution.             */
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
/* Library General Public License for more details.                                  */
/*                                                                                   */
/* You should have received a copy of the GNU Library General Public                 */
/* License along with this software source code distribution; if not,                */
/* write to the Free Software Foundation, Inc., 59 Temple Place -                    */
/* Suite 330, Boston, MA 02111-1307, USA.                                            */
/*                                                                                   */
/* Last modified by Ed Trager, February, 2003.                                       */
/*                                                                                   */
/* ********************************************************************************* */

/*                                                          */
/* togglebychromosome.h                                     */
/*                                                          */
/* init: 2003.02.24 ET                                      */
/*                                                          */
/* -> Functions to support an addition to the               */
/*    TOGGLE command:                                       */
/*                                                          */
/*    TOGGLE ON|OFF OUTPUT FLAGS FOR CHROMOSOME <n> MARKERS */
/*                                                          */

#ifndef MAP_INCLUDED
#include "map.h"
#endif

void ToggleMarkersByChromosome(int force_on,int chromosome);
void ToggleMendelianInconsistentMarkers(int force_on);
void ToggleMendelianInconsistentMarkersByChromosome(int force_on,int chromosome);
void ToggleMarkerOn(PMARKERMAP p);
void ToggleMarkerOff(PMARKERMAP p);
void ToggleMendelianInconsistentMarkerOn(PMARKERMAP p);
void ToggleMendelianInconsistentMarkerOff(PMARKERMAP p);

#define TOGGLEBYCHR_INCLUDED
