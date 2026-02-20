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
/* "fieldflags.h" is part of the Madeline software distribution.                     */
/* Written by Edward H. Trager.                                                      */
/*                                                                                   */
/* COPYRIGHT (C) 2002 THE REGENTS OF THE UNIVERSITY OF MICHIGAN,                     */
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
/* Last modified by Ed Trager, April, 2002.                                          */
/*                                                                                   */
/* ********************************************************************************* */

/*                                                      */
/*                                                      */
/* fieldflags.h                                         */
/*                                                      */
/* history:                                             */
/*                                                      */
/* 2002.04.18.ET: Segregated out of the old "shared.h"  */

/*///////////////////// */
/*                      */
/* OR-able field flags: */
/*                      */
/*///////////////////// */
#define NOFLAGS            0x00
#define KNOWNSET           0x01
#define ICONSET            0x02  /* variables displayed graphically in partioned icon */
#define COREOUTSET         0x04  /* COREOUTSET is totally obsolete                    */
#define COVARIATESET       0x08
#define GENOTYPESET        0x10
#define PHENOTYPESET       0x20

#define OUTPUTSET          0x40
#define COVARIATEOUTPUTSET 0x48
#define GENOTYPEOUTPUTSET  0x50
#define PHENOTYPEOUTPUTSET 0x60
#define NULLCOLUMNSET      0x80 /* completely empty set */

#define FIELDFLAGS_INCLUDED
