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
/* "smartselect.c" is part of the Madeline software distribution.                    */
/* Written by Edward H. Trager.                                                      */
/*                                                                                   */
/* COPYRIGHT (C) 2004 THE REGENTS OF THE UNIVERSITY OF MICHIGAN,                     */
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
/* Last modified by Ed Trager, October, 2004.                                        */
/*                                                                                   */
/* ********************************************************************************* */

#include "tools.h"
#include "recognize.h"
/* ndbf.h includes dbf.h which is also needed here: */
#include "ndbf.h"
#include "config.h"

/*                  */
/* SmartTableSelect */
/*                  */
const char *SmartTableSelect( const char *fileName){
	
	int tableType;
	const char *fileNameWithExtension;
	
	/* Check if table exists: */
	if(!File(fileName)){
		
		MadelineErrorReport("SmartTableSelect(): Can't open \"%s\" for reading.\nPerhaps the path or file name is incorrect.",fileName);
		
	}
	
	/* Check if the table is a FLATFILE table: */
	tableType=CharacterizeDatabase(fileName);
	/*                                                                                      */
	/* If table_type==FLATFILE, this strongly suggests the user typed the name of the table */
	/* and forgot to type the ".mfh" header extension.  If the header file exists, we will  */
	/* open that for the user as a courtesy:                                                */ 
	/*                                                                                      */
	if(tableType==FLATFILE){
		/* See if file name with extension (usually ".mfh") exists: */
		fileNameWithExtension=ModifyFileName(fileName,HEADER_FILE_EXTENSION);
		/* If file exists, return this file name: */
		if(File(fileNameWithExtension)){
			
			tableType=CharacterizeDatabase(fileNameWithExtension);
			if(!tableType){
				
				MadelineErrorReport("SmartTableSelect(): \"%s\" is not in a recognized table format or is corrupt.",fileNameWithExtension);
				
			}
			
			lprintf("NOTE: Madeline will attempt to use \"%s\" ...\n",fileNameWithExtension);
			/* This is the file to open: */
			return fileNameWithExtension;
		}else{
			/* Try automatically recognizing the file:         */
			/* Since we are not going to create a "run"        */
			/* file, we can get away with lang=0 and ivar=NULL */
			/*                                                 */
			lprintf("NOTE: Since \"%s\" does not exist, Madeline will\nattempt to recognize \"%s\" now ...\n",
				fileNameWithExtension,
				fileName
			);
			if(!RecognizeFile(fileName,0,NULL,0)){
				
				MadelineErrorReport("SmartTableSelection(): Unable to successfully recognize \"%s\".\nYou should run the RECOGNIZE command manually after\nevaluating the problem.",fileName);
				
			}else{
				
				/* This is the file to open: */
				return fileNameWithExtension;
				
			}
		}
	}
	
	if(!tableType){
		
		MadelineErrorReport("SmartTableSelect(): \"%s\" is not in a recognized table format or is corrupt.",fileName);
		
	}
	/* We get here if the file is not a flat file  */
	/* so return that name (which will normally be */
	/* a table that can be opened directly):       */
	return fileName;
	
}


