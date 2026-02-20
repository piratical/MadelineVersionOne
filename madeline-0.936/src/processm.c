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
/* "processm.c" is part of the Madeline software distribution.                       */
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
/* processm.c */
/*            */

/* 19991029 initiated by ET                             */
/* processm provides child process spawning in Madeline */


#include "processm.h"
#include "logs.h"

#include <curses.h>

/*                    */
/* CreateChildProcess */
/*                    */
int CreateChildProcess(char *path,char **argv){

/* __MACH__ works for Mach-based kernels like OS X: */
#if defined(__unix__) || defined(unix) || defined(__MACH__)

	/*                         */
	/* Code for a unix system: */
	/*                         */
	
	pid_t pid;
	
	pid=fork();
	if(pid==0){
		
		/*                   */
		/* In child process: */
		/*                   */
		
		if(execvp(path,argv)==-1){
			
			MadelineErrorReport("In child: couldn't execute %s: execvp() failed in CreateChildProcess()",path);
			
		}
		fprintf(stdout,"...Returned.\n");
	}else if(pid>0){
		/*                    */
		/* In parent process: */
		/*                    */
		return 1;
		
	}else{
		/* */
		/* */
		/* */
		MadelineErrorReport("Couldn't execute %s: fork() failed in CreateChildProcess()",path);
		return 0;
	}
	return 1;
	

#else
#if defined(_WIN32) || defined(__WIN32__)

	/*                                                                             */
	/* Code for a windows system (only Unix and Windows are currently recognized): */
	/* This covers Microsoft Windows 95, 98, NT, and presumably 2K too:            */
	/*                                                                             */

	if(spawnvp(P_NOWAIT,path,argv)==-1){
		/*         */
		/* failed: */
		/*         */
		MadelineErrorReport("Couldn't execute %s: spawnvp() failed in CreateChildProcess()",path);
		return 0;

	}else{
		/*            */
		/* succeeded: */
		/*            */
		return 1;	
	
	}
	
#else
#error Unknown operating system: processm.h only distinguishes between generic Unix and Windows variants
#endif
#endif

}

/* end of processm.c */
