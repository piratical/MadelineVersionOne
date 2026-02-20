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
/* "directorylisting.h" is part of the Madeline software distribution.               */
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
/* Library General Public License for more details.                                  */
/*                                                                                   */
/* You should have received a copy of the GNU Library General Public                 */
/* License along with this software source code distribution; if not,                */
/* write to the Free Software Foundation, Inc., 59 Temple Place -                    */
/* Suite 330, Boston, MA 02111-1307, USA.                                            */
/*                                                                                   */
/* Last modified by Ed Trager, October 2004.                                         */
/*                                                                                   */
/* ********************************************************************************* */

#include <dirent.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <set>

#include "directorylisting.h"
#include "logs.h"

struct compareFileNames
{
  bool operator()(const char* s1, const char* s2) const
  {
    return strcmp(s1, s2) < 0;
  }
};

void ListCurrentDirectory( void ){
	
	/* pointer to structure containing each entry: */
	struct dirent* entry;
	
	/* structure passed to the stat() system call, to get its results. */
	struct stat fileStatus;
	
	/* Use an STL set for sorting the file names: */
	std::set <const char *,compareFileNames> directoryList;
	
	
	/* OPEN the current directory for reading:    */
	DIR* dir = opendir(".");
	if (!dir) MadelineErrorReport("ListCurrentDirectory(): Unable to open the current directory for reading.");
	
	/* ADD file names to the STL set:             */
	while((entry=readdir(dir)) != NULL) directoryList.insert(entry->d_name);
	
	/* CLOSE the current directory:               */
	if (closedir(dir) == -1) MadelineErrorReport("ListCurrentDirectory(): Unable to close the current directory.");
	
	/* PRINT header:                              */
	lprintf("\n-----  -------------  -------------------------\n");
	lprintf("Perms  Size in Bytes  File Name                \n");
	lprintf("-----  -------------  -------------------------\n");
	
	/* LOOP through the set, printing information about the files: */
	for(std::set <const char *>::const_iterator fileName = directoryList.begin();fileName!=directoryList.end();++fileName){
		
		/* Check the status information of the file:        */
		/* (Note that we intentionally ignore corrupt files */
		/* which may be present but which we can't stat)    */
		if (stat(*fileName, &fileStatus) == 0){
			/* Processes files that are not directories: */
			if (!S_ISDIR(fileStatus.st_mode)){
				
				lprintf(" %c%c%c   %'13i  %s\n",
					(fileStatus.st_mode & S_IRUSR)?'r':'-',
					(fileStatus.st_mode & S_IWUSR)?'w':'-',
					(fileStatus.st_mode & S_IXUSR)?'x':'-',
					(int) fileStatus.st_size,
					*fileName
				);
				
			}
		}
	}
	lprintf("\n");
	
}

