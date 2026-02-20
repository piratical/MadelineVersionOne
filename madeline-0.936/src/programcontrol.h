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
/* "control.h" is part of the Madeline software distribution.                        */
/* Written by Edward H. Trager.                                                      */
/*                                                                                   */
/* COPYRIGHT (C) 2003 THE REGENTS OF THE UNIVERSITY OF MICHIGAN,                     */
/* Ann Arbor, Michigan, USA.  ALL RIGHTS RESERVED.                                   */
/*                                                                                   */
/* Copyright (C) 1996-2003 by Edward H. Trager and the FUSION Study Group            */
/* at the University of Michigan School of Public Health in Ann Arbor,               */
/* Michigan, USA.                                                                    */
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
/* Last modified by Ed Trager, Feb.  2003.                                           */
/*                                                                                   */
/* ********************************************************************************* */

/*                                           */
/* programcontrol.h                          */
/*                                           */
#include "stringbuffersizes.h"

#define PROGCTRLSTACKSIZE 200

enum PROGRAMCONTROLTYPE {
	NOT_SET=0,
	FOR_STATEMENT=1,   
	WHILE_STATEMENT,
	IF_STATEMENT,
	ELSE_STATEMENT
};

typedef struct sPROGRAMCONTROL {
	
	PROGRAMCONTROLTYPE type;
	
	/* condition and next iteration are used when executing FOR */
	char initialassignment[MINIMUMBUFFERSIZE];
	char condition[MINIMUMBUFFERSIZE];
	char nextiteration[MINIMUMBUFFERSIZE];
	int level;    /* Nesting level of control block     */
	int stt;      /* block starts at program line "stt" */
	int end;      /* block ends   at program line "end" */
	
} PROGRAMCONTROL, *PPROGRAMCONTROL;

void ExecuteForStatement(void);
void ExecuteEndOfControlBlock(void);

PPROGRAMCONTROL NewProgramControlStack(int size);
void PushProgramControlStack(void);
void PopProgramControlStack(void);

extern PPROGRAMCONTROL PCStack;
extern PPROGRAMCONTROL pControlBlock;

#define PROGRAMCONTROL_INCLUDED
