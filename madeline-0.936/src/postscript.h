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
/* "postscript.h" is part of the Madeline software distribution.                     */
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

/*              */
/* postscript.h */
/*              */
/* 19990428 ET  */
/*              */

/*                */
/* ANSI INCLUDES: */
/*                */
#include <stdio.h>
#include <stdlib.h>

/*                    */
/* MADELINE INCLUDES: */
/*                    */
#ifndef PORTABLE_INCLUDED
#include "portable.h"
#endif

#ifndef CONTROL_INCLUDED
#include "control.h"
#endif

enum STARTEND { START, END };

/*            */
/* FUNCTIONS: */
/*            */
void DownLoadPostScriptRoutines(FILE *fp,double Version,int CopyrightYear,PCONTROL c,int LegendFontSize,int CellFontSize);
void PrintStartEndHeader(FILE *fp,char *title,enum STARTEND sttend);
void DownLoadProlog(FILE *fp,double Version,int CopyrightYear,PCONTROL c,int LegendFontSize,int CellFontSize);
void Box(FILE *fp);
void HaploHighlight(FILE *fp);
void TrianglePoint(FILE *fp);
void PointTag(FILE *fp);
void MissingSymbol(FILE *fp);
void AdoptedIn(FILE *fp);
void AdoptedOut(FILE *fp);
void Deceased(FILE *fp);
void NonTermDeceased(FILE *fp);
void Bridge(FILE *fp);
void DoubleBridge(FILE *fp);
void ClipToPrintablePage(FILE *fp);
void MultiplePagePrint(FILE *fp);
void Arrow(FILE *fp);
void ProbandArrow(FILE *fp);
void IconCell(FILE *fp);
void FillRegion(FILE *fp);
void ShowRegionText(FILE *fp);
void DrawPieSlice(FILE *fp);
void IncrementAngle(FILE *fp);
void Icon(FILE *fp);
void DisplayLegendVariable(FILE *fp);
void LegendLineItem(FILE *fp);
void CircleHighlight(FILE *fp);

#define POSTSCRIPT_INCLUDED
