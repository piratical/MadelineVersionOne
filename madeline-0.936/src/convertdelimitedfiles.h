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
/* "convertdelimitedfiles.h" is part of the Madeline software distribution.          */
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

/*                                                     */
/* convertdelimitedfiles.h -- originally "cnvtdlm.h"   */
/*                                                     */
/*                                                     */
/* 19990226 init.                                      */
/*                                                     */

/*                */
/* ANSI Includes: */
/*                */
#include <stdio.h>
#include <stdlib.h>

/*                    */
/* Madeline Includes: */
/*                    */
#include "tools.h"
#include "logs.h"


#define CNVTDLM_INCLUDED
#define DOUBLE_QUOTE '\"'
#define SINGLE_QUOTE '\''

/*                       */
/* FUNCTION DECLARATIONS */
/*                       */

/* int main(int argc,char *argv[]); // used in DEBUGGING: now see ConvertFile() */
int ConvertFile(char *fin,char *fout,char dlm);
int *NewFieldWidthVector(int FieldCount);
int GetFieldCount(char *b,char dlm);
void DetermineFieldWidths(char *buffer,int *FieldWidth,int FieldCount,char dlm);
char *DetermineIndividualFieldWidth(char *field,int *FieldWidth,char dlm);
int WriteAlignedColumnFile(char *buffer,int *FieldWidth,int FieldCount,char dlm,FILE *fcout);
char *WriteIndividualField(char *field,int *FieldWidth,char dlm,FILE *fout);


