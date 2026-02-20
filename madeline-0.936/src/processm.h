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
/* "processm.h" is part of the Madeline software distribution.                       */
/* Written by Edward H. Trager.                                                      */
/*                                                                                   */
/* COPYRIGHT (C) 2000 THE REGENTS OF THE UNIVERSITY OF MICHIGAN,                     */
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
/* Last modified by Ed Trager, March 2000.                                           */
/*                                                                                   */
/* ********************************************************************************* */

/*            */
/* processm.h */
/*            */

/* 19991029 initiated by ET                             */
/* processm provides child process spawning in Madeline */

#include <stdio.h>
#include <stdlib.h>

/* __MACH__ works for Mach-based kernels like OS X: */
#if defined(__unix__) || defined(unix) || defined(__MACH__)

/* Unix INCLUDES: */
#include <sys/types.h>
/* #include <sys/proc.h> */
#include <unistd.h>


/* Madeline DEFINES: */
#define DIRECTORYSEPARATOR "/"

#else
#if defined(_WIN32) || defined(__WIN32__)

/* Windows INCLUDES: */
#include <process.h>

/* Madeline DEFINES: */
#define DIRECTORYSEPARATOR "\\"

#else
#error Unknown operating system: processm.h only distinguishes between generic Unix and Windows variants
#endif
#endif


/* FUNCTION PROTOTYPES */

int CreateChildProcess(char *path,char **argv);

#define PROCESSM_INCLUDED

