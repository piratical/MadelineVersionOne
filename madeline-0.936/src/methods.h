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
/* "methods.h" is part of the Madeline software distribution.                        */
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
/* Last modified by Ed Trager, February, 2003.                                       */
/*                                                                                   */
/* ********************************************************************************* */

/* 2003.02.25.ET: Separated out into its own header file */

#ifndef CONTROL_INCLUDED
#include "control.h"
#endif

/*                                                                   */
/* METHODS structure is used to hold the function                    */
/* pointers for writing different locus and                          */
/* pedigree file formats:                                            */
/*                                                                   */
/* Note that some output formats don't have any                      */
/* special header or footer, and so can be NULL.                     */
/*                                                                   */
/* Also, there may be only a few "locus" file                        */
/* formats, but many different data file formats.                    */
/* Most programs use just one locus file format,                     */
/* but may rely on different data file formats for                   */
/* different analyses or modules.                                    */
/*                                                                   */
/* Note that function pointers numbered 1,3,4 and 6                  */
/* point to functions that, because they write the                   */
/* headers and footers, need to be called just once                  */
/* per file write.  There is one subtlety that the                   */
/* reader should be aware of: OutputFooter() might actually          */
/* do any sort of post-processing required, not just                 */
/* writing a footer at the sequential end of a data file.            */
/* In fact, most formats have no need of a file footer               */
/* at all.  But there are formats, like CRIMAP, where                */
/* information obtained in the process of writing pedigree           */
/* data itself needs to be inserted back up in the header            */
/* of the file.  Since I deemed it easiest to do this after          */
/* writing out the pedigree data, the CrimapFooter() function        */
/* actually writes some information for the header, then copies      */
/* in the pedigree data already written to disk.  So, JUST           */
/* REMEMBER that OutputHeader() can perform any kind of              */
/* PRE-processing required prior to actually writing the pedigree    */
/* data, and OutputFooter() can perform any kind of POST-processing  */
/* after writing out pedigree data, as required by the program.      */
/*                                                                   */
/* In contrast, OutputMarker() writes                                */
/* out the allele frequency data for a single marker and             */
/* so a loop is required to call OutputMarker() for                  */
/* each marker field marked as a GENOTYPE OUTPUT field.              */
/*                                                                   */
/* Similarly, OutputPedigree() writes out the data for               */
/* a single pedigree.  Since PEDIGREEs are nodes in a                */
/* binary sorted tree in Madeline, a traversal function that visits  */
/* each node in order is required.                                   */
/*                                                                   */
typedef struct sMETHODS{

	void(*LocusHeaderProcessor)(PCONTROL,char *fn);
	void(*OutputMarker        )(PCONTROL,int);
	void(*LocusFooterProcessor)(PCONTROL);
	
	void(*DataHeaderProcessor)(PCONTROL,char *fn);
	void(*OutputPedigree     )(PCONTROL,PPEDIGREE);
	void(*DataFooterProcessor)(PCONTROL);
	/*                                             */
	/* Method used for showing summary statistics: */
	/*                                             */
	void(*ShowStatistics     )(PCONTROL);
	
}METHODS,*PMETHODS;

#define METHODS_INCLUDED

