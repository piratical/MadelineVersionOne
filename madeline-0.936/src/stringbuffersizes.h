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
/* "stringbuffersizes.h" is part of the Madeline software distribution.              */
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

/*                                                       */
/*                                                       */
/* stringbuffersizes.h                                   */
/*                                                       */
/* history:                                              */
/*                                                       */
/* 2002.04.18.ET: Segregated out of the old "portable.h" */

/*                                                                      */
/* defines for string buffer sizes that                                 */
/* can be used in all modules:                                          */
/*                                                                      */

#define MAXIDWIDTH              32 /* Maximum ID width                  */
#define MAXSHORTSTRINGCODEWIDTH 33 /* Maximum width for short string codes */
#define MAXMARKERNAMEWIDTH      33 /* Maximum Marker name width         */
#define MAXIMUMFIELDNAMELENGTH  33 /* This INCLUDES the null terminator */
#define MAXFIELDNAMEWIDTH  MAXIMUMFIELDNAMELENGTH                                                               
#define DBASEFIELDNAMELENGTH    11 /* This INCLUDES the null terminator */
#define FIELDNAMEWIDTH          11 /* Left over from the old DBASE days <-- THIS NEEDS TO BE FIXED! */
#define SMALLBUFFERSIZE        512
#define MINIMUMBUFFERSIZE      100
#define MAXIMUMGENOTYPESTRING   10

#define MAXIMUMMENDELFIELDWIDTH  8
#define HALFMAXMENDELFIELDWIDTH  4
#define MENDELGENOTYPEFIELDWIDTH 7 /* allows for "nnn/nnn"   */
#define SIBPALGENOTYPEFIELDWIDTH 9 /* allows for "nnnn/nnnn" <- Why is this larger than for other formats? */
#define SIBLINKALLELEFIELDWIDTH  4 /* allows for "nnnn" */
#define CRIMAPALLELEFIELDWIDTH   3 /* allows for "nnn" */

/*                    */
/* screen line width: */
/*                    */
#define LINEWIDTH 79
#define COMMANDLINEWIDTH 1000 /* Increased on 2003.02.14 to accomodate script needs */

#define STRINGBUFFERSIZES_INCLUDED


