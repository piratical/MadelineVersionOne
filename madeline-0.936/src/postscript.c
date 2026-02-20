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
/* "postscript.c" is part of the Madeline software distribution.                     */
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

/*                       */
/* postscript.c          */
/*                       */
/* 19990428 ET initiated */
/*                       */
#include "postscript.h"

/*                               */
/* DownLoadPostScriptRoutines(): */
/*                               */
void DownLoadPostScriptRoutines(FILE *fp,double Version,int CopyrightYear,PCONTROL c,int LegendFontSize,int CellFontSize){

	DownLoadProlog(fp,Version,CopyrightYear,c,LegendFontSize,CellFontSize);
	
	PrintStartEndHeader(fp,"PROCEDURES",START);
	Box(fp);
	HaploHighlight(fp);
	TrianglePoint(fp);
	PointTag(fp);
	MissingSymbol(fp);
	/* AdoptedIn(fp); */
	/* AdoptedOut(fp); */
	Deceased(fp);
	/* NonTermDeceased(fp); */
	Bridge(fp);
	DoubleBridge(fp);
	ClipToPrintablePage(fp);
	MultiplePagePrint(fp);
	Arrow(fp);
	ProbandArrow(fp);
	IconCell(fp);
	FillRegion(fp);
	ShowRegionText(fp);
	DrawPieSlice(fp);
	IncrementAngle(fp);
	Icon(fp);
	DisplayLegendVariable(fp);
	LegendLineItem(fp);
	CircleHighlight(fp);
	PrintStartEndHeader(fp,"PROCEDURES",END);
	
}


/*                     */
/* PrintStartEndHeader */
/*                     */
void PrintStartEndHeader(FILE *fp,char *title,enum STARTEND sttend){
	
	fputs("\n",fp);
	fputs("\n",fp);
	fputs("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n",fp);
	fputs("%\n",fp);
	fprintf(fp,"%% *** %s OF %s ***\n",sttend==START?"START":"END",title);
	fputs("%\n",fp);
	fputs("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n",fp);
	fputs("\n",fp);
	fputs("\n",fp);
	
}


/*                */
/* DownloadProlog */
/*                */
void DownLoadProlog(FILE *fp,double Version,int CopyrightYear,PCONTROL c,int LegendFontSize,int CellFontSize){
	fputs("%!PS-Adobe-3.0\n",fp);
	fputs("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n",fp);
	fputs("%\n",fp);
	/*          */
	/* Version: */
	/*          */
	fprintf(fp,"%% Madeline v. %6.3f Postscript Procedures\n",Version);
	fputs("%\n",fp);
	/*                */
	/* CopyrightYear: */
	/*                */
	fprintf(fp,"%% COPYRIGHT (C) %4i BY THE REGENTS OF THE UNIVERSITY OF MICHIGAN,\n",CopyrightYear);
	fputs("% Ann Arbor, Michigan, USA.  ALL RIGHTS RESERVED.\n",fp);
	fputs("% Distributed under the GNU Public License.\n",fp); 
	fputs("%\n",fp);
	fputs("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n",fp);
	fputs("%\n",fp);
	fputs("%%Creator: Edward H. Trager\n",fp);
	fputs("%%Title: Madeline Postscript Pedigree Procedures\n",fp);
	fputs("%%Version: 1.1\n",fp);
	/*fputs("%%Pages: (atend)\n",fp); */
	fputs("%%EndComments\n",fp);
	fputs("%\n",fp);
	fputs("\n",fp);
	/*               */
	/* Start prolog: */
	/*               */
	PrintStartEndHeader(fp,"PROLOG",START);
	
	fputs("%\n",fp);
	fputs("%%%%%%%%%%%%%%%\n",fp);
	fputs("%\n",fp);
	fputs("% Unit metrics:\n",fp);
	fputs("%\n",fp);
	fputs("%%%%%%%%%%%%%%%\n",fp);
	fputs("/inch {72 mul}      bind def\n",fp);
	fputs("/cm {inch 2.54 div} bind def\n",fp);
	fputs("/mm {cm 10 div}     bind def\n",fp);
	fputs("\n",fp);
	fputs("\n",fp);
	fputs("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n",fp);
	fputs("%\n",fp);
	fputs("% Standardized icon size definitions:\n",fp);
	fputs("%\n",fp);
	fputs("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n",fp);
	/*                            */
	/* Global drawing parameters: */
	/*                            */
	
	/*             */
	/* IconRadius: */
	/*             */
	fprintf(fp,"/IconRadius %.2f cm def\n",c->d.IconRadius);
	fputs("/IconDiameter IconRadius 2 mul      def\n",fp);
	fputs("/IconDiagonal IconRadius 2 sqrt mul def\n",fp);
	fputs("/CircumscribingRadius   IconRadius 1.3 mul def\n",fp);
	fputs("/CircumscribingDiameter CircumscribingRadius 2 mul def\n",fp);
	/*             */
	/* TickLength: */
	/*             */
	fprintf(fp,"/TickLength %.2f cm def\n",c->d.TickLength);
	/*                                                   */
	/* Vertical drop from parent line to offspring line: */
	/*                                                   */
	fprintf(fp,"/VDrop1 %.2f cm def\n",c->d.VerticalDrop1);	
	/*                                    */
	/* Vertical drop from offspring line: */
	/*                                    */
	fprintf(fp,"/VDrop2 %.2f cm def\n",c->d.VerticalDrop2);
	/*                                                     */
	/* Vertical drop from offspring line minus IconRadius: */
	/*                                                     */
	fprintf(fp,"/VDrop2MinusIR VDrop2 IconRadius add def\n");
	/*                                          */
	/* Horizontal interval between individuals: */
	/*                                          */
	fprintf(fp,"/XUnit %.2f cm def\n",c->d.HorizontalInterval);
	/*                                  */
	/* Calculate cell width from XUnit: */
	/*                                  */
	fputs("/CellWidth XUnit 0.1 sub def\n",fp);
	fputs("\n",fp);
	fputs("\n",fp);
	fputs("%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n",fp);
	fputs("%\n",fp);
	fputs("% Useful symbolic constants:\n",fp);
	fputs("%\n",fp);
	fputs("%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n",fp);
	fputs("/FEMALE         1      def\n",fp);
	fputs("/MALE           0      def\n",fp);
	fputs("/MISSING       -1      def\n",fp);
	fputs("/UNKNOWNGENDER MISSING def\n",fp);
	fputs("/VIRTUAL        0      def\n",fp);
	fputs("/REAL           1      def\n",fp);
	fputs("/PI      3.1415927     def\n",fp);
	fputs("\n",fp);
	fputs("\n",fp);
	fputs("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n",fp);
	fputs("%\n",fp);
	fputs("% Symbolic constants for pregnancies\n",fp);
	fputs("% not carried to term:\n",fp);
	fputs("%\n",fp);
	fputs("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n",fp);
	fputs("/NONTERM        8        def\n",fp);
	fputs("/NONTERMMALE    8        def\n",fp);
	fputs("/NONTERMFEMALE  9        def\n",fp);
	fputs("/NONTERMUNKNOWNGENDER 10 def\n",fp);
	fputs("\n",fp);
	fputs("\n",fp);
	fputs("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n",fp);
	fputs("%\n",fp);
	fputs("% Border line styles/weights:\n",fp);
	fputs("%\n",fp);
	fputs("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n",fp);
	fputs("/NOLINE      0      def\n",fp);
	fputs("/THINLINE    1      def\n",fp);
	fputs("/MEDIUMLINE  0.5 mm def\n",fp);
	fputs("/THICKLINE   1.0 mm def\n",fp);
	fputs("\n",fp);
	fputs("\n",fp);
	fputs("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n",fp);
	fputs("%\n",fp);
	fputs("% Shading Flags:\n",fp);
	fputs("%\n",fp);
	fputs("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n",fp);
	fputs("/SHADE         true   def\n",fp);
	fputs("/DONTSHADE     false  def\n",fp);
	fputs("\n",fp);
	fputs("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n",fp);
	fputs("%\n",fp);
	fputs("% Text colors:\n",fp);
	fputs("%\n",fp);
	fputs("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n",fp);
	fputs("/LIGHT 0.5 def\n",fp);
	fputs("/VERYLIGHT 0.8 def\n",fp);
	fputs("/Red { 1.0 0.0 0.0 setrgbcolor}  bind def\n",fp);
	fputs("/LightRed {1.0 LIGHT LIGHT setrgbcolor} bind def\n",fp);
	fputs("/VeryLightRed {1.0 VERYLIGHT VERYLIGHT setrgbcolor} bind def\n",fp);
	fputs("/Green {0.0 1.0 0.0 setrgbcolor} bind def\n",fp);
	fputs("/LightGreen {LIGHT 1.0 LIGHT setrgbcolor} bind def\n",fp);
	fputs("/VeryLightGreen {VERYLIGHT 1.0 VERYLIGHT setrgbcolor} bind def\n",fp);
	fputs("/Blue { 0.0 0.0 1.0 setrgbcolor} bind def\n",fp);
	fputs("/LightBlue { VERYLIGHT VERYLIGHT 1.0 setrgbcolor} bind def\n",fp);
	fputs("/VeryLightBlue { 0.93 0.93 1.0 setrgbcolor} bind def\n",fp);
	fputs("/DarkRed { 0.7 0.0 0.0 setrgbcolor} bind def\n",fp);
	fputs("/DarkestRed { 0.5 0.0 0.0 setrgbcolor} bind def\n",fp);
	fputs("/Maroon { 0.8 0.0 0.4 setrgbcolor} bind def\n",fp);
	fputs("/Orange { 1.0 0.6 0.2 setrgbcolor} bind def\n",fp);
	fputs("/LightOrange { 1.0 0.8 0.6 setrgbcolor} bind def\n",fp);
	fputs("/VeryLightOrange { 1.0 0.9 0.8 setrgbcolor} bind def\n",fp);
	fputs("/Yellow { 0.85 0.8 0.0 setrgbcolor} bind def\n",fp);
	fputs("/Gray1 { 0.95 setgray } bind def\n",fp);
	fputs("/Gray2 { 0.90 setgray } bind def\n",fp);
	fputs("/Gray3 { 0.85 setgray } bind def\n",fp);
	fputs("/Gray4 { 0.80 setgray } bind def\n",fp);
	fputs("/Gray5 { 0.75 setgray } bind def\n",fp);
	fputs("/Gray6 { 0.70 setgray } bind def\n",fp);
	fputs("\n",fp);
	fputs("/RED            (7) 0 get def\n",fp);
	fputs("/GREEN          (8) 0 get def\n",fp);
	fputs("/BLUE           (9) 0 get def\n",fp);
	fputs("/ORANGE         (0) 0 get def\n",fp);
	fputs("\n",fp);
	fputs("/LIGHTRED        (r) 0 get def\n",fp);
	fputs("/VERYLIGHTRED    (R) 0 get def\n",fp);
	fputs("/LIGHTGREEN      (g) 0 get def\n",fp);
	fputs("/VERYLIGHTGREEN  (G) 0 get def\n",fp);
	fputs("/LIGHTBLUE       (b) 0 get def\n",fp);
	fputs("/VERYLIGHTBLUE   (B) 0 get def\n",fp);
	fputs("/LIGHTORANGE     (o) 0 get def\n",fp);
	fputs("/VERYLIGHTORANGE (O) 0 get def\n",fp);
	fputs("\n",fp);
	fputs("\n",fp);
	fputs("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n",fp);
	fputs("%\n",fp);
	fputs("% Define dash lengths for wrapping icons:\n",fp);
	fputs("%\n",fp);
	fputs("% female icon: wrap with exactly 12 dashes,\n",fp);
	fputs("% male icon  : wrap with exactly 16 dashes (4 per side):\n",fp);
	fputs("%\n",fp);
	fputs("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n",fp);
	fputs("/FemaleDashLength IconRadius 2 mul PI mul 24 div def\n",fp);
	fputs("/MaleDashLength   IconDiameter 8 div def \n",fp);
	fputs("\n",fp);
	fputs("\n",fp);
	fputs("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n",fp);
	fputs("%\n",fp);
	fputs("% Boolean toggle for color shading/printing:\n",fp);
	fputs("%\n",fp);
	fputs("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n",fp);
	/*          */
	/* InColor: */
	/*          */
	fprintf(fp,"/INCOLOR %s def\n",c->d.InColor?"true":"false");
	/*                 */
	/* ReverseShading: */
	/*                 */
	fprintf(fp,"/REVERSE_SHADING %s def\n",c->d.ReverseShading?"true":"false");
	/*                          */
	/* Crossover Tags, Tag Size */
	/*                          */
	fputs("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n",fp);
	fputs("% \n",fp);
	fputs("% Boolean toggle for showing crossover tags:\n",fp);
	fputs("% \n",fp);
	fputs("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n",fp);
	fprintf(fp,"/CROSSOVERTAGS %s def\n",c->d.CrossOverTags?"true":"false");
	fputs("/TAGSIZE 2.0 mm def\n",fp);
	fputs("\n",fp);
	fputs("\n",fp);
	fputs("%%%%%%%%%%%%%%%%%\n",fp);
	fputs("%\n",fp);
	fputs("% Font Variables:\n",fp);
	fputs("%\n",fp);
	fputs("%%%%%%%%%%%%%%%%%\n",fp);
	fputs("/IconFontSize 7 def\n",fp);
	/*                             */
	/* Cell and Legend font sizes: */
	/*                             */
	fprintf(fp,"/CellFontSize  %i def\n",CellFontSize);
	fprintf(fp,"/LegendFontSize %i def\n",LegendFontSize);
	fputs("/CellMargin 1.0 mm def\n",fp);
	fputs("/IconFont {/Helvetica-Bold findfont} def\n",fp);
	fputs("/CellFont {/Helvetica findfont} def\n",fp);
	fputs("/BoldFont {/Helvetica-Bold findfont} def\n",fp);
	fputs("\n",fp);
	fputs("\n",fp);
	fputs("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n",fp);
	fputs("%\n",fp);
	fputs("% Page margins and widths: used by MultiplePagePrint:\n",fp);
	fputs("%\n",fp);
	fputs("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n",fp);
	/*                                                                       */
	/* Page width, height, and margin definitions used by MultiplePagePrint: */
	/*                                                                       */
	fprintf(fp,"/leftmargin   %.2f cm def\n",c->d.PageMargin);
	fprintf(fp,"/bottommargin %.2f cm def\n",c->d.PageMargin);
	/*                                                                               */
	/* Note that pagewidth and pageheight are really frame sizes, not page sizes     */
	/* here (i.e., we have to subtract off the margins which leaves the frame size): */
	/*                                                                               */
	fprintf(fp,"/pagewidth    %.2f cm def\n",c->d.PageWidth -2*c->d.PageMargin);
	fprintf(fp,"/pageheight   %.2f cm def\n",c->d.PageHeight-2*c->d.PageMargin);
	/*               */
	/* END   prolog: */
	/*               */
	PrintStartEndHeader(fp,"PROLOG",END);
	
}

/*       */
/* Box() */
/*       */
void Box(FILE *fp){
	
	fputs(
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"%\n"
	"% Box: Prints a rectangular region in color\n"
	"% \n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"/Box {\n"
	" /ColorCode exch def\n"
	" /dy exch def\n"
	" /dx exch def\n"
	" /LeftTopY exch def\n"
	" /LeftTopX exch def\n"
	" gsave\n"
	" newpath\n"
	" LeftTopX LeftTopY moveto\n"
	" dx 0 rlineto\n"
	" 0 dy neg rlineto\n"
	" dx neg 0 rlineto\n"
	" closepath\n"
	" INCOLOR\n"
	" {\n"
	"  ColorCode RED             eq {Red            }if\n"
	"  ColorCode LIGHTRED        eq {LightRed       }if\n"
	"  ColorCode VERYLIGHTRED    eq {VeryLightRed   }if\n"
	"  ColorCode GREEN           eq {Green          }if\n"
	"  ColorCode LIGHTGREEN      eq {LightGreen     }if\n"
	"  ColorCode VERYLIGHTGREEN  eq {VeryLightGreen }if\n"
	"  ColorCode BLUE            eq {Blue           }if\n"
	"  ColorCode LIGHTBLUE       eq {LightBlue      }if\n"
	"  ColorCode VERYLIGHTBLUE   eq {VeryLightBlue  }if\n"
	"  ColorCode ORANGE          eq {Orange         }if\n"
	"  ColorCode LIGHTORANGE     eq {LightOrange    }if\n"
	"  ColorCode VERYLIGHTORANGE eq {VeryLightOrange}if\n"
	"  \n"
	" }\n"
	" {\n"
	"  ColorCode RED             eq {Gray3}if\n"
	"  ColorCode LIGHTRED        eq {Gray2}if\n"
	"  ColorCode VERYLIGHTRED    eq {Gray1}if\n"
	"  ColorCode GREEN           eq {Gray6}if\n"
	"  ColorCode LIGHTGREEN      eq {Gray5}if\n"
	"  ColorCode VERYLIGHTGREEN  eq {Gray4}if\n"
	"  ColorCode BLUE            eq {Gray3}if\n"
	"  ColorCode LIGHTBLUE       eq {Gray2}if\n"
	"  ColorCode VERYLIGHTBLUE   eq {Gray1}if\n"
	"  ColorCode ORANGE          eq {Gray6}if\n"
	"  ColorCode LIGHTORANGE     eq {Gray5}if\n"
	"  ColorCode VERYLIGHTORANGE eq {Gray4}if\n"
	" }ifelse\n"
	" fill\n"
	" grestore\n"
	"} bind def\n"
	,fp);
	
}

/*                  */
/* HaploHighlight() */
/*                  */
void HaploHighlight(FILE *fp){
	
	fputs(
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"%\n"
	"% HaploHighlight: Processes color background highlighting\n"
	"% for haplotype displays\n"
	"%\n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"/HaploHighlight\n"
	"{\n"
	" /Color2 exch def\n"
	" /Color1 exch def\n"
	" /LineHeight exch def\n"
	" /CellWidth exch def\n"
	" /y exch def\n"
	" /x exch def\n"
	" /HalfWidth CellWidth 2 div def\n"
	" x y HalfWidth LineHeight Color1 Box\n"
	" x HalfWidth add y HalfWidth LineHeight Color2 Box\n"
	"} bind def\n"
	,fp);
	
}


/*                  */
/* TrianglePoint()  */
/*                  */
void TrianglePoint(FILE *fp){
	
	fputs(
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"%\n"
	"% TrianglePoint: Prints a little triangular point\n"
	"%\n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"/TrianglePoint{\n"
	" /rotation exch def\n"
	" /size exch def\n"
	" /y exch def\n"
	" /x exch def\n"
	" gsave\n"
	" /HalfSize size 2 div def\n"
	" /ThreeFourthsSize size 3 mul 4 div def\n"
	" x y translate\n"
	" rotation rotate\n"
	" newpath\n"
	" 0 HalfSize neg moveto\n"
	" ThreeFourthsSize HalfSize rlineto\n"
	" ThreeFourthsSize neg HalfSize rlineto\n"
	" closepath\n"
	" 0 setgray fill\n"
	" grestore\n"
	"} bind def\n"
	,fp);
	
}

/*            */
/* PointTag() */
/*            */
void PointTag(FILE *fp){
	
	fputs(
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"%\n"
	"% PointTag: Prints little numerical/letter tags\n"
	"%\n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"/PointTag{\n"
	" /label exch def\n"
	" /rotation exch def\n"
	" /size exch def\n"
	" /y exch def\n"
	" /x exch def\n"
	" gsave\n"
	" /HalfSize size 2 div def\n"
	" /OneFourthSize HalfSize 2 div def\n"
	" /ThreeEighthsSize size 3 mul 8 div def\n"
	" /ThreeFourthsSize OneFourthSize 3 mul def\n"
	" /FontSize size 0.9 mul def\n"
	" x y translate\n"
	" rotation rotate\n"
	" 0 HalfSize sub 0 HalfSize add moveto\n"
	" size 0 rlineto\n"
	" 0 ThreeFourthsSize neg rlineto\n"
	" HalfSize neg ThreeEighthsSize neg rlineto\n"
	" HalfSize neg ThreeEighthsSize rlineto\n"
	" closepath\n"
	" clip\n"
	" gsave\n"
	" 1 setgray fill\n"
	" grestore\n"
	" size 0.06 mul setlinewidth\n"
	" stroke\n"
	" rotation neg rotate\n"
	" CellFont [FontSize 0 0 FontSize 0 0] makefont setfont\n"
	" label stringwidth pop 2 div neg FontSize 3 div neg moveto\n"
	" label show\n"
	" grestore\n"
	"}bind def\n"
	,fp);
	
}

/*                 */
/* MissingSymbol() */
/*                 */
void MissingSymbol(FILE *fp){
	
	fputs(
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"%\n"
	"% MissingSymbol: Draws the missing symbol indicator.\n"
	"% This is currently a small dot\n"
	"%\n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"/MissingSymbol\n"
	"{/yms exch def\n"
	" /xms exch def\n"
	"  gsave\n"
	"  newpath\n"
	"  0 setgray\n"
	"  xms yms 0.25 mm 0 360 arc fill\n"
	"  grestore\n"
	"}bind def\n"
	,fp);
	
}

/*             */
/* AdoptedIn() */
/*             */
void AdoptedIn(FILE *fp){
	
	fputs(
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"%\n"
	"% AdoptedIn: Draws \"adopted in\" symbol around an icon\n"
	"%            xorg and yorg are the coordinates of\n"
	"%            the center of the icon\n"
	"%\n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"/AdoptedIn\n"
	"{/yorg exch def\n"
	" /xorg exch def\n"
	" gsave\n"
	" THINLINE setlinewidth\n"
	" newpath\n"
	" xorg CircumscribingRadius sub TickLength add yorg CircumscribingRadius add moveto\n"
	" TickLength neg 0 rlineto\n"
	" 0 CircumscribingDiameter neg rlineto\n"
	" TickLength 0 rlineto\n"
	" stroke\n"
	" newpath\n"
	" xorg CircumscribingRadius add TickLength sub yorg CircumscribingRadius add moveto\n"
	" TickLength 0 rlineto\n"
	" 0 CircumscribingDiameter neg rlineto\n"
	" TickLength neg 0 rlineto\n"
	" stroke\n"
	" grestore\n"
	"}bind def\n"
	,fp);
	
}

/*              */
/* AdoptedOut() */
/*              */
void AdoptedOut(FILE *fp){
	
	fputs(
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"%\n"
	"% AdoptedOut: Draws \"adopted out\" symbol around an icon\n"
	"%             xorg and yorg are the coordinates of\n"
	"%            the center of the icon\n"
	"%\n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"/AdoptedOut\n"
	"{/yorg exch def\n"
	" /xorg exch def\n"
	" gsave\n"
	" THINLINE setlinewidth\n"
	" newpath\n"
	" xorg CircumscribingRadius sub TickLength sub yorg CircumscribingRadius add moveto\n"
	" TickLength 0 rlineto\n"
	" 0 CircumscribingDiameter neg rlineto\n"
	" TickLength neg 0 rlineto\n"
	" stroke\n"
	" newpath\n"
	" xorg CircumscribingRadius add TickLength add yorg CircumscribingRadius add moveto\n"
	" TickLength neg 0 rlineto\n"
	" 0 CircumscribingDiameter neg rlineto\n"
	" TickLength 0 rlineto\n"
	" stroke\n"
	" grestore\n"
	"}bind def\n"
	,fp);
	
}

/*            */
/* Deceased() */
/*            */
void Deceased(FILE *fp){
	
	fputs(
	"%%%%%%%%%%%\n"
	"%\n"
	"% Deceased:\n"
	"%\n"
	"%%%%%%%%%%%\n"
	"/Deceased\n"
	"{/yorg exch def\n"
	" /xorg exch def\n"
	" gsave\n"
	" newpath\n"
	" THINLINE setlinewidth\n"
	" xorg CircumscribingRadius add yorg CircumscribingRadius add moveto\n"
	" CircumscribingDiameter neg dup rlineto\n"
	" stroke\n"
	" grestore\n"
	"}bind def\n"
	,fp);
	
}


/*                   */
/* NonTermDeceased() */
/*                   */
void NonTermDeceased(FILE *fp){
	
	fputs(
	"%%%%%%%%%%%%%%%%%\n"
	"%\n"
	"% NonTermDeceased\n"
	"%\n"
	"%%%%%%%%%%%%%%%%%\n"
	"/NonTermDeceased\n"
	"{/yorg exch def\n"
	" /xorg exch def\n"
	" gsave\n"
	" newpath\n"
	" THINLINE setlinewidth\n"
	" /xshift IconRadius 2 div def\n"
	" /yshift IconRadius 4 div def\n"
	" xorg xshift sub yorg yshift sub moveto\n"
	" IconRadius dup yshift add rlineto\n"
	" stroke\n"
	" grestore\n"
	"}bind def\n"
	,fp);
	
}


/*          */
/* Bridge() */
/*          */
void Bridge(FILE *fp){
	
	fputs(
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"%\n"
	"% Bridge: Stroke the current path\n"
	"% with a line that appears to bridge\n"
	"% over other lines by virtue of having\n"
	"% a white border:\n"
	"%\n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"/Bridge\n"
	"{\n"
	" gsave\n"
	" gsave\n"
	" /originalwidth currentlinewidth def\n"
	" originalwidth 3 mul setlinewidth\n"
	" 1 setgray\n"
	" stroke\n"
	" grestore\n"
	" originalwidth setlinewidth\n"
	" 0 setgray\n"
	" stroke\n"
	" grestore\n"
	"}bind def\n"
	,fp);
	
}


/*                */
/* DoubleBridge() */
/*                */
void DoubleBridge(FILE *fp){
	
	fputs(
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"%\n"
	"% DoubleBridge: Stroke the current path\n"
	"% with a double line that appears to bridge\n"
	"% over other lines by virtue of being edged\n"
	"% in white:\n"
	"%\n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"/DoubleBridge\n"
	"{\n"
	" gsave\n"
	" gsave\n"
	" gsave\n"
	" /originalwidth currentlinewidth def\n"
	" originalwidth 5 mul setlinewidth\n"
	" 1 setgray\n"
	" stroke\n"
	" grestore\n"
	" originalwidth 3 mul setlinewidth\n"
	" 0 setgray\n"
	" stroke\n"
	" grestore\n"
	" originalwidth setlinewidth\n"
	" 1 setgray\n"
	" stroke\n"
	" grestore\n"
	"}bind def\n"
	,fp);
	
}


/*                       */
/* ClipToPrintablePage() */
/*                       */
void ClipToPrintablePage(FILE *fp){
	
	fputs(
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"%\n"
	"% ClipToPrintablePage: used by MultiplePagePrint\n"
	"% for clipping to the printable portion of a page\n"
	"%\n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"/ClipToPrintablePage\n"
	"{\n"
	" newpath\n"
	" leftmargin bottommargin moveto\n"
	" 0 pageheight rlineto\n"
	" pagewidth 0 rlineto\n"
	" 0 pageheight neg rlineto\n"
	" closepath\n"
	" clip\n"
	"}bind def\n"
	,fp);
	
}


/*                     */
/* MultiplePagePrint() */
/*                     */
void MultiplePagePrint(FILE *fp){
	
	fputs(
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"%\n"
	"% MultiplePagePrint:\n"
	"% Prints a large image defined by BigPictureProc on\n"
	"% multiple pages arranged as n row-pages by m column-pages\n"
	"% Note that BigPictureProc should start with gsave and end\n"
	"% with a grestore in order for MultiplePagePrint to work\n"
	"% properly:\n"
	"%\n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"/MultiplePagePrint\n"
	"{/rows                exch def\n"
	" /columns             exch def\n"
	" /BigPictureProcedure exch def\n"
	" gsave\n"
	"\n"
	" leftmargin bottommargin translate\n"
	"\n"
	" %\n"
	" % Loop through page rows and columns:\n"
	" %\n"
	" 0 1 rows 1 sub\n"
	" {/rowcount exch def\n"
	"  0 1 columns 1 sub\n"
	"  {/columncount exch def\n"
	"\n"
	"   gsave\n"
	"\n"
	"   %\n"
	"   % translate to window the printable region of this page:\n"
	"   %\n"
	"   pagewidth columncount mul neg\n"
	"   pageheight rowcount mul neg\n"
	"   translate\n"
	"\n"
	"   %\n"
	"   % do the big picture procedure:\n"
	"   %\n"
	"\n"
	"   %%page rows columns\n"
	"   BigPictureProcedure\n"
	"\n"
	"   %\n"
	"   % gsave-grestore bracket showpage in order to preserve\n"
	"   % the clipping region:\n"
	"   %\n"
	"   gsave showpage grestore\n"
	"\n"
	"   grestore\n"
	"\n"
	"  }for\n"
	" }for\n"
	" grestore\n"
	"}bind def\n"
	,fp);
	
}


/*         */
/* Arrow() */
/*         */
void Arrow(FILE *fp){
	
	fputs(
	"%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"%\n"
	"% generic arrow procedure:\n"
	"%\n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"/arrow\n"
	"{\n"
	" /TailWidth  exch def\n"
	" /TailLength exch def\n"
	" /HeadWidth  exch def\n"
	" /HeadLength exch def\n"
	" /ArrowAngle exch def\n"
	" /ytip       exch def\n"
	" /xtip       exch def\n"
	"\n"
	" /HalfHeadWidth HeadWidth 2 div def\n"
	" /HalfTailWidth TailWidth 2 div def\n"
	" /TailIndent    HalfTailWidth HeadLength mul HalfHeadWidth div def\n"
	"\n"
	" /mtrx matrix def\n"
	" /SavedMatrix mtrx currentmatrix def\n"
	" xtip ytip translate\n"
	" ArrowAngle rotate\n"
	" 0 0 moveto\n"
	" %\n"
	" % top half outline of arrow from tip to tail indent:\n"
	" %\n"
	" HeadLength HalfHeadWidth rlineto\n"
	" 0 HalfHeadWidth HalfTailWidth sub neg rlineto\n"
	" TailLength 0 rlineto\n"
	" TailIndent neg HalfTailWidth neg rlineto\n"
	" %\n"
	" % bottom half outline from tail indent back to tip:\n"
	" %\n"
	" TailIndent HalfTailWidth neg rlineto\n"
	" TailLength neg 0 rlineto\n"
	" 0 HalfHeadWidth HalfTailWidth sub neg rlineto\n"
	" closepath\n"
	" SavedMatrix setmatrix\n"
	"}bind def\n"
	,fp);
	
}

/*                */
/* ProbandArrow() */
/*                */
void ProbandArrow(FILE *fp){
	
	fputs(
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"%\n"
	"% Proband Arrow: draws an arrow pointing to the proband\n"
	"% pass the __icon's__ x and y center coordinates (this\n"
	"% makes it really easy):\n"
	"%\n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"/ProbandArrow\n"
	"{\n"
	" /yiconorg exch def\n"
	" /xiconorg exch def\n"
	" /OffsetDistance IconRadius 1.15 mul def\n"
	" gsave\n"
	" newpath\n"
	" xiconorg OffsetDistance sub yiconorg OffsetDistance sub 225 3.5 mm 4 mm 3.5 mm 1.3 mm arrow\n"
	" fill\n"
	" grestore\n"
	"}bind def\n"
	,fp);
	
}

/*            */
/* IconCell() */
/*            */
void IconCell(FILE *fp){
	
	fputs(
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"%\n"
	"% IconCell: Prints an icon cell.  A \"cell\" is a rectangular\n"
	"% region with one or more lines of text labels on it.  An\n"
	"% icon cell uses a fixed set of very specific and limited\n"
	"% formatting conventions:\n"
	"%\n"
	"% Special flags for printing of genotypes and haplotypes:\n"
	"% -------------------------------------------------------\n"
	"%\n"
	"% - All flags are embedded in the strings that are passed\n"
	"%   to the routine.\n"
	"% - Flags precede the printable portion of the strings and\n"
	"%   are demarcated by special characters:\n"
	"%\n"
	"% (141/142)    : Genotype without special printing specs.\n"
	"% (23`141/142) : Background colors 2 and 3 specified. Color\n"
	"%                \"2\" prints behind \"141\" and color \"3\"\n"
	"%                prints behind \"142\".\n"
	"% (20^141/142) : A point tag prints with the label \"2\"\n"
	"%                for the maternal crossover.\n"
	"% (01^23`9\\\\10): A point tag prints with the label \"1\"\n"
	"%                for the paternal crossover.  Background\n"
	"%                colors are also specified.\n"
	"% (R~121/124)  : The genotype prints in RED to indicate\n"
	"%                a Mendelian inheritance error.\n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"/IconCell\n"
	"{\n"
	" /ArrayOfStrings   exch def\n"
	" /ShadeFlag        exch def\n"
	" /BorderType       exch def\n"
	" /CellWidth        exch def\n"
	" /Celly            exch def\n"
	" /Cellx            exch def\n"
	"\n"
	" %\n"
	" % get number of lines in cell:\n"
	" %\n"
	" /CellLines ArrayOfStrings length def\n"
	" /LineHeight CellFontSize 1.1 mul def\n"
	" /LineAdjust LineHeight 5 div def\n"
	" /CellHeight CellLines LineHeight mul CellMargin 2 mul add CellFontSize 2 div add def\n"
	" /HalfCellWidth CellWidth 2 div def\n"
	"\n"
	" %\n"
	" % Cellx and Celly specify the center point\n"
	" % of the top horizontal boundary of the cell box.\n"
	" % Define cell bounding box, clip to this:\n"
	" %\n"
	" gsave\n"
	" Cellx HalfCellWidth sub Celly moveto\n"
	" CellWidth 0 rlineto\n"
	" 0 CellHeight neg rlineto\n"
	" CellWidth neg 0 rlineto\n"
	" closepath\n"
	" clip\n"
	"\n"
	" %\n"
	" % shade if more than five lines of text, but only if flag set:\n"
	" %\n"
	" ShadeFlag\n"
	" {\n"
	"  CellLines 5 gt\n"
	"  {\n"
	"   gsave\n"
	"   /currx Cellx HalfCellWidth sub def\n"
	"   /curry Celly CellMargin sub LineAdjust sub def\n"
	"   /dy    LineHeight 5 mul def\n"
	"   INCOLOR\n"
	"   {\n"
	"    VeryLightBlue\n"
	"   }\n"
	"   {\n"
	"    0.90 setgray\n"
	"   }ifelse\n"
	"   5 10 CellLines\n"
	"   {\n"
	"    /yadvance exch def\n"
	"    newpath\n"
	"    currx curry yadvance LineHeight mul neg add moveto\n"
	"    CellWidth 0 rlineto\n"
	"    /remaining CellLines yadvance sub def\n"
	"    remaining 5 lt\n"
	"    {\n"
	"     /dy LineHeight remaining mul def\n"
	"    }if\n"
	"    0 dy neg rlineto\n"
	"    CellWidth neg 0 rlineto\n"
	"    closepath\n"
	"    fill\n"
	"   }for\n"
	"   grestore\n"
	"  }if\n"
	" }if\n"
	"\n"
	" %\n"
	" % move down CellFontSize plus CellMargin to start:\n"
	" %\n"
	" /Celly Celly CellFontSize CellMargin add sub def\n"
	" Cellx Celly moveto\n"
	"\n"
	" %\n"
	" % set font:\n"
	" %\n"
	" CellFont CellFontSize scalefont setfont\n"
	"\n"
	" %\n"
	" % Print text lines from array:\n"
	" %\n"
	" ArrayOfStrings\n"
	" {\n"
	"  /ThisText exch def\n"
	"\n"
	"  gsave\n"
	"  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"  % Check for Labeled PointTags for haplotype displays:\n"
	"  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"  ThisText (^) search\n"
	"  {\n"
	"   /Pre exch def\n"
	"   /Match exch def\n"
	"   /ThisText exch def\n"
	"   % Get the two label codes (one or the other could be zero):\n"
	"   /PointTagLabel1 Pre 0 1 getinterval def\n"
	"   /PointTagLabel2 Pre 1 1 getinterval def\n"
	"  }\n"
	"  {\n"
	"   /PointTagLabel1 (0) def\n"
	"   /PointTagLabel2 (0) def\n"
	"  }ifelse\n"
	"  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"  % Check for background color tags for haplotype displays:\n"
	"  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"  ThisText (`) search\n"
	"  {\n"
	"   % Define Has (Crossover) Tags:\n"
	"   /Pre exch def\n"
	"   /Match exch def\n"
	"   /ThisText exch def\n"
	"   % Get the two highlight color codes:\n"
	"   /Color1 Pre 0 get def\n"
	"   /Color2 Pre 1 get def\n"
	"    %INCOLOR\n"
	"    true\n"
	"    {\n"
	"     % Print the background colors:\n"
	"     Cellx HalfCellWidth sub Celly LineHeight add LineAdjust sub CellWidth LineHeight Color1 Color2 HaploHighlight\n"
	"    }if\n"
	"  }if\n"
	"  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"  % Check whether the string is flagged for printing in\n"
	"  % a different color for Mendelian error notification:\n"
	"  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"  ThisText (~) search\n"
	"  {\n"
	"    %\n"
	"    % The Pre string contains the directives, if any, for\n"
	"    % coloring the text:\n"
	"    %\n"
	"    /Pre exch def\n"
	"    /Match exch def\n"
	"    /ThisText exch def\n"
	"    % Make font bold:\n"
	"    BoldFont CellFontSize scalefont setfont\n"
	"    % Search for directives in the Pre element:\n"
	"    INCOLOR\n"
	"    {\n"
	"     Pre (R) search {Red       }if\n"
	"     Pre (4) search {Red       }if\n"
	"     Pre (3) search {Maroon    }if\n"
	"     Pre (2) search {Orange    }if\n"
	"     Pre (O) search {Orange    }if\n"
	"     Pre (Y) search {Yellow    }if\n"
	"     Pre (B) search {Blue      }if\n"
	"     Pre (G) search {Green     }if\n"
	"    }if\n"
	"  }if\n"
	"  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"  % Print crossover lines for haplotype display ?\n"
	"  %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"  CROSSOVERTAGS\n"
	"  {\n"
	"   gsave\n"
	"   newpath\n"
	"   0.15 mm setlinewidth\n"
	"   % Maternal Crossover: just be sure it's not a date\n"
	"   ThisText (/) search\n"
	"   {\n"
	"    pop search not\n"
	"    {\n"
	"     Cellx HalfCellWidth sub Celly LineHeight add LineAdjust sub moveto\n"
	"     HalfCellWidth 0.5 mm sub 0 rlineto\n"
	"     stroke\n"
	"     % Print the PointTag Label only if its not zero:\n"
	"     PointTagLabel1 (0) ne\n"
	"     {\n"
	"      Cellx HalfCellWidth sub TAGSIZE 2 div add Celly LineHeight add LineAdjust sub TAGSIZE 0 PointTagLabel1 PointTag\n"
	"     }\n"
	"     {\n"
	"      Cellx HalfCellWidth sub Celly LineHeight add LineAdjust sub 1 mm 0 TrianglePoint\n"
	"     }ifelse\n"
	"    }if\n"
	"   }if\n"
	"   % Paternal Crossover\n"
	"   ThisText (\\\\) search\n"
	"   {\n"
	"    Cellx 0.5 mm add Celly LineHeight add LineAdjust sub moveto\n"
	"    HalfCellWidth 0.5 mm sub 0 rlineto\n"
	"    stroke\n"
	"    PointTagLabel2 (0) ne\n"
	"    {\n"
	"     Cellx HalfCellWidth add TAGSIZE 2 div sub Celly LineHeight add LineAdjust sub TAGSIZE 0 PointTagLabel2 PointTag\n"
	"    }\n"
	"    {\n"
	"     Cellx HalfCellWidth add Celly LineHeight add LineAdjust sub 1 mm 180 TrianglePoint\n"
	"    }ifelse\n"
	"   }if\n"
	"   % Both Maternal and Paternal Crossovers\n"
	"   ThisText (+) search\n"
	"   {\n"
	"    Cellx HalfCellWidth sub Celly LineHeight add LineAdjust sub moveto\n"
	"    HalfCellWidth 0.5 mm sub 0 rlineto\n"
	"    Cellx 0.5 mm add Celly LineHeight add LineAdjust sub moveto\n"
	"    HalfCellWidth 0.5 mm sub 0 rlineto\n"
	"    stroke\n"
	"    PointTagLabel1 (0) ne\n"
	"    {\n"
	"     Cellx HalfCellWidth sub TAGSIZE 2 div add Celly LineHeight add LineAdjust sub TAGSIZE 0 PointTagLabel1 PointTag\n"
	"    }\n"
	"    {\n"
	"     Cellx HalfCellWidth sub Celly LineHeight add LineAdjust sub 1 mm 0 TrianglePoint\n"
	"    }ifelse\n"
	"    PointTagLabel2 (0) ne\n"
	"    {\n"
	"     Cellx HalfCellWidth add TAGSIZE 2 div sub Celly LineHeight add LineAdjust sub TAGSIZE 0 PointTagLabel2 PointTag\n"
	"    }\n"
	"    {\n"
	"     Cellx HalfCellWidth add Celly LineHeight add LineAdjust sub 1 mm 180 TrianglePoint\n"
	"    }ifelse\n"
	"   }if\n"
	"   grestore\n"
	"  }if\n"
	"\n"
	"\n"
	"  %\n"
	"  % calculate where to print string centered horizontally:\n"
	"  %\n"
	"  /StringWidth ThisText stringwidth pop def\n"
	"  %\n"
	"  % Squish the fontsize down if string is too wide to print:\n"
	"  %\n"
	"  /WidthRatio CellWidth CellMargin 2 mul sub StringWidth div def\n"
	"  WidthRatio 1.0 lt\n"
	"  {\n"
	"   %\n"
	"   % case where string does not fit:\n"
	"   % scale the width down to fit while maintaining the original height\n"
	"   %\n"
	"   CellFont [CellFontSize WidthRatio mul 0 0 CellFontSize 0 0] makefont setfont\n"
	"   Cellx ThisText stringwidth pop 2 div sub Celly moveto\n"
	"   ThisText show\n"
	"  }\n"
	"  {\n"
	"   % case where string fits:\n"
	"   Cellx StringWidth 2 div sub Celly moveto\n"
	"   ThisText show\n"
	"  }ifelse\n"
	"  grestore\n"
	"  %\n"
	"  % increment y for next round:\n"
	"  %\n"
	"  /Celly Celly LineHeight sub def\n"
	" }forall\n"
	"\n"
	" %\n"
	" % Draw bounding border if not NOLINE:\n"
	" %\n"
	" BorderType NOLINE ne\n"
	" {\n"
	"  gsave\n"
	"  BorderType setlinewidth\n"
	"  stroke\n"
	"  grestore\n"
	" }if\n"
	" grestore\n"
	"}bind def\n"
	,fp);
	
}


/*              */
/* FillRegion() */
/*              */
void FillRegion(FILE *fp){
	
	fputs(
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"%\n"
	"% FillRegion: Fills region with color (if INCOLOR)\n"
	"% or with a shade of gray (if not INCOLOR).  Pushes GrayLevel\n"
	"% onto the stack at the end of the procedure\n"
	"% \n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"/FillRegion\n"
	"{\n"
	" /BluC exch def\n"
	" /GrnC exch def\n"
	" /RedC exch def\n"
	" /GrayLevel exch def\n"
	" \n"
	" gsave\n"
	" INCOLOR\n"
	" {\n"
	"  %\n"
	"  % In color case:\n"
	"  %\n"
	"  RedC GrnC BluC setrgbcolor\n"
	" }\n"
	" {\n"
	"  REVERSE_SHADING\n"
	"  {\n"
	"   1.0 GrayLevel sub setgray\n"
	"  }\n"
	"  {\n"
	"   GrayLevel setgray\n"
	"  }ifelse % if REVERSE_SHADING\n"
	" }ifelse % if INCOLOR\n"
	" fill\n"
	" grestore\n"
	"}bind def\n"
	,fp);
	
}

 
/*                  */
/* ShowRegionText() */
/*                  */
void ShowRegionText(FILE *fp){
	
	fputs(
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"%\n"
	"% ShowRegionText: changes ink color in black-and-white mode\n"
	"% before showing the text based on background GrayLevel.  In\n"
	"% color mode, text is just shown in black.\n"
	"%\n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"/ShowRegionText\n"
	"{\n"
	" /ThisText  exch def\n"
	" /GrayLevel exch def\n"
	"\n"
	" %\n"
	" % Text label is always printed in black ink if INCOLOR. However,\n"
	" % for black-and-white, it depends on how dark the gray fill is:\n"
	" % for dark fills, print text in white:\n"
	" %\n"
	" INCOLOR\n"
	" {\n"
	"  ThisText show\n"
	" }\n"
	" {\n"
	"  gsave                            % gsave because of possible ink color change\n"
	"  REVERSE_SHADING { /GrayLevel 1.0 GrayLevel sub def } if % reverse\n"
	"  GrayLevel 0.5 lt {1 setgray} if  % set letter color to white if on dark ground\n"
	"  ThisText show\n"
	"  grestore                         % restored\n"
	" }ifelse\n"
	"}bind def\n"
	,fp);
	
}


/*                */
/* DrawPieSlice() */
/*                */
void DrawPieSlice(FILE *fp){
	
	fputs(
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"%\n"
	"% DrawPieSlice: Draws one slice of a \"pie\":\n"
	"%\n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"/DrawPieSlice\n"
	"{\n"
	" /DataArray exch def\n"
	" /endangle exch def\n"
	" /sttangle exch def\n"
	" /radius exch def\n"
	" /yorg exch def\n"
	" /xorg exch def\n"
	"\n"
	" gsave\n"
	" newpath\n"
	" % set to fairly thin lines:\n"
	" 1 setlinewidth\n"
	"\n"
	" % draw arc and clip to it:\n"
	" xorg yorg moveto\n"
	" xorg yorg radius sttangle endangle arc\n"
	" closepath\n"
	" clip\n"
	"\n"
	" % set miter limit for acute angle line joins:\n"
	" 1.415 setmiterlimit\n"
	"\n"
	" % Pull data elements from array:\n"
	" DataArray aload pop\n"
	" /BluC exch def\n"
	" /GrnC exch def\n"
	" /RedC exch def\n"
	" /GryC exch def\n"
	" /ThisText    exch def\n"
	"\n"
	" GryC RedC GrnC BluC FillRegion\n"
	"\n"
	" % print text in middle of pie slice:\n"
	" /HalfRadius IconRadius 2 div def\n"
	" /MiddleAngle endangle sttangle add 2 div def\n"
	" gsave                       % save unrotated coordinates and graphics\n"
	" xorg yorg translate         % translate to pie center\n"
	" MiddleAngle rotate          % rotate to middle of slice\n"
	" HalfRadius 1.1 mul 0 translate  % translate along x-axis half the radius (plus a little more)\n"
	" 0 0 transform               % push coord. of origin in device space onto stack\n"
	" grestore                    % restore unrotated coord. system\n"
	" itransform                  % inverse transform the two coord. on the stack\n"
	" /tcy exch def /tcx exch def % transformed coordinates defined\n"
	" tcx tcy moveto\n"
	"\n"
	" %\n"
	" % Special handling to indicate MISSING:\n"
	" %\n"
	" ThisText (.) eq ThisText () eq or\n"
	" {%\n"
	"  % A centered small dot represents MISSING:\n"
	"  %\n"
	"  TotalLevels MISSING ne { tcx tcy MissingSymbol } if\n"
	" }\n"
	" {%\n"
	"  % center the text on the point: The positioning is more or less\n"
	"  % optimized for placing capital letters:\n"
	"  %\n"
	"  ThisText stringwidth pop 2 div neg IconFontSize 3 div neg rmoveto\n"
	"  %\n"
	"  % ShowRegionText adjusts ink color based on GrayLevel when in\n"
	"  % black-and-white mode:\n"
	"  %\n"
	"  GrayLevel ThisText ShowRegionText\n"
	" }ifelse % end if ThisText (.) eq\n"
	"\n"
	" 0.5 setlinewidth                 % use a very thin line\n"
	" stroke                           % outline the closed arc\n"
	"\n"
	" grestore\n"
	"}bind def\n"
	,fp);
	
}


/*                  */
/* IncrementAngle() */
/*                  */
void IncrementAngle(FILE *fp){
	
	fputs(
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"%\n"
	"% IncrementAngle procedure: used by /icon:\n"
	"%\n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"/IncrementAngle\n"
	"{/sttangle endangle def\n"
	" /endangle sttangle incrangle add def\n"
	"}bind def\n"
	,fp);
	
}


/*        */
/* Icon() */
/*        */
void Icon(FILE *fp){
	
	fputs(
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"%\n"
	"% icon procedure.  This procedure produces the icon as either\n"
	"% a square for males, a circle for females, or rectangle for unknown\n"
	"% gender.  The icon may be divided into one or more \"pie slices\", with\n"
	"% each slice representing a categorical variable.  Each slice may be\n"
	"% colored (or shaded gray) to represent the level of the categorical\n"
	"% variable, and a short text label optionally printed in the middle\n"
	"% of the slice:\n"
	"%\n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"/icon\n"
	"{/ArrayOfArrays exch def\n"
	" /gender        exch def\n"
	" /icontype      exch def\n"
	" /yorigin       exch def\n"
	" /xorigin       exch def\n"
	"\n"
	" % Into how many pie slices is the icon divided?\n"
	" /slices ArrayOfArrays length def\n"
	"\n"
	" % Define angles and increments:\n"
	" /incrangle 360 slices div def\n"
	" /sttangle 90 def\n"
	" /endangle sttangle incrangle add def\n"
	"\n"
	" %save graphic state:\n"
	" gsave\n"
	"\n"
	" %\n"
	" % Set font for icon:\n"
	" %\n"
	" IconFont IconFontSize scalefont setfont\n"
	"\n"
	" newpath\n"
	"\n"
	" % If male, set radius to IconDiagonal, and clip to a square\n"
	" % else if female, set radius to IconRadius (plus a little extra), clip to circle\n"
	" % else if unknown, set radius to IconRadius, clip to a diamond\n"
	"\n"
	" gender NONTERM ge\n"
	" {%\n"
	"  % Pregnancy not carried to term cases:\n"
	"  %\n"
	"\n"
	"  %\n"
	"  % print non-term gender in small letters\n"
	"  % below symbol:\n"
	"  %\n"
	"  IconFont IconFontSize scalefont setfont\n"
	"  xorigin yorigin IconRadius sub moveto\n"
	"  gender NONTERMMALE eq\n"
	"  {% non-term male:\n"
	"   (male)\n"
	"  }\n"
	"  {\n"
	"   gender NONTERMFEMALE eq\n"
	"   {% non-term female:\n"
	"    (female)\n"
	"   }\n"
	"   {% non-term gender unknown:\n"
	"    (unknown)\n"
	"   }ifelse  % end if non-term female\n"
	"  }ifelse   % end if non-term male\n"
	"  dup stringwidth pop 2 div neg 0 rmoveto\n"
	"  show\n"
	"\n"
	"  /CircleRadius IconRadius def\n"
	"  % clip to small triangle:\n"
	"  xorigin IconRadius sub yorigin moveto\n"
	"  IconRadius dup rlineto\n"
	"  IconRadius dup neg rlineto\n"
	"  closepath\n"
	"  clip\n"
	" }\n"
	" {%\n"
	"  % Normal gender cases:\n"
	"  %\n"
	"  gender UNKNOWNGENDER eq\n"
	"  {\n"
	"   % unknown gender:\n"
	"   /CircleRadius IconRadius def\n"
	"   % clip to diamond:\n"
	"   xorigin IconRadius sub yorigin moveto\n"
	"   IconRadius dup rlineto\n"
	"   IconRadius dup neg rlineto\n"
	"   IconRadius neg dup rlineto\n"
	"   closepath\n"
	"   clip\n"
	"  }\n"
	"  {\n"
	"\n"
	"   % male or female:\n"
	"   gender MALE eq\n"
	"   {% male gender\n"
	"    /CircleRadius IconDiagonal def\n"
	"    % clip to a square:\n"
	"    xorigin IconRadius sub yorigin IconRadius sub moveto\n"
	"    0 IconDiameter rlineto\n"
	"    IconDiameter 0 rlineto\n"
	"    0 IconDiameter neg rlineto\n"
	"    closepath\n"
	"    clip\n"
	"   }\n"
	"   {% female gender\n"
	"    /CircleRadius IconRadius 0.5 mm add def\n"
	"    xorigin yorigin IconRadius 0 360 arc\n"
	"    %\n"
	"    % clipping is needed in case where only one slice\n"
	"    % where DrawPieSlice is not called:\n"
	"    %\n"
	"    clip\n"
	"   }ifelse              % end if gender male\n"
	"  }ifelse               % end if gender unknown\n"
	" }ifelse                % end if non-term\n"
	" %\n"
	" % Process data in array of arrays - but only if\n"
	" % array length is greater than 1.  If array length is 1,\n"
	" % then there is only one categorical variable, so don't call\n"
	" % /DrawPieSlice at all in that case:\n"
	" %\n"
	" slices 1 eq\n"
	" {\n"
	"  %\n"
	"  % process case where there is only one slice:\n"
	"  %\n"
	"  ArrayOfArrays 0 get   % get the single array nested in ArrayOfArrays\n"
	"  aload pop             % put the elements of that array on the stack\n"
	"  /BluC exch def\n"
	"  /GrnC exch def\n"
	"  /RedC exch def\n"
	"  /GrayLevel exch def\n"
	"  /ThisText    exch def % text label for this level (can be null string)\n"
	"\n"
	"  GrayLevel RedC GrnC BluC FillRegion\n"
	"\n"
	"  %\n"
	"  % Special handling to indicate MISSING:\n"
	"  %\n"
	"  ThisText (.) eq ThisText () eq or\n"
	"  {%\n"
	"   % A centered small dot represents MISSING:\n"
	"   %\n"
	"   TotalLevels MISSING ne { xorigin yorigin MissingSymbol } if\n"
	"  }\n"
	"  {\n"
	"   % print the text label:\n"
	"   xorigin yorigin moveto\n"
	"   ThisText stringwidth pop 2 div neg IconFontSize 3 div neg rmoveto\n"
	"   %\n"
	"   % ShowRegionText adjusts ink color based on GrayLevel when in\n"
	"   % black-and-white mode:\n"
	"   %\n"
	"   GrayLevel ThisText ShowRegionText\n"
	"  }ifelse  % end if ThisText (.) eq\n"
	" }\n"
	" {\n"
	"  %\n"
	"  % process case where there are multiple slices:\n"
	"  %\n"
	"  ArrayOfArrays\n"
	"  {\n"
	"   /SliceArray exch def\n"
	"   xorigin yorigin CircleRadius sttangle endangle SliceArray DrawPieSlice\n"
	"   IncrementAngle\n"
	"  }forall\n"
	" }ifelse\n"
	" %\n"
	" % if VIRTUAL, stroke clip path with a dashed line instead of a black line:\n"
	" %\n"
	" icontype VIRTUAL eq\n"
	" {\n"
	"  gender FEMALE eq\n"
	"  {%\n"
	"   % case of FEMALE: wrap circle with exactly 12 dashes:\n"
	"   %\n"
	"   /DashLength FemaleDashLength def\n"
	"  }\n"
	"  {%\n"
	"   % case of MALE: 4 black dashes per side = 16 total\n"
	"   %\n"
	"   /DashLength MaleDashLength def\n"
	"  }ifelse\n"
	"  %\n"
	"  % The offset of a half DashLength makes the dashes\n"
	"  % wrap the corners of the squares and centers the\n"
	"  % the dashes over the cardinal directions on the circles:\n"
	"  %\n"
	"  [DashLength DashLength] DashLength 2 div setdash\n"
	" } if\n"
	" stroke\n"
	"\n"
	" grestore\n"
	"}bind def\n"
	,fp);
	
}


/*                         */
/* DisplayLegendVariable() */
/*                         */
void DisplayLegendVariable(FILE *fp){
	
	fputs(
	"%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"%\n"
	"% DisplayLegendVariable\n"
	"%\n"
	"%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"/DisplayLegendVariable{\n"
	" /ArrayOfArrays exch def\n"
	" /TotalLevels exch def\n"
	" /TextHeading exch def\n"
	" /cy exch def\n"
	" /cx exch def\n"
	"\n"
	" gsave\n"
	" IconFont IconFontSize scalefont setfont\n"
	" /curry cy 2 mm sub def\n"
	" cx TextHeading stringwidth pop 2 div sub curry moveto\n"
	" TextHeading show\n"
	" /curry curry 0.7 mm sub def\n"
	" /currx cx 1.5 cm sub def\n"
	" newpath\n"
	" currx curry moveto\n"
	" 3.0 cm 0 rlineto\n"
	" 0.5 setlinewidth\n"
	" stroke\n"
	" /curry curry 2.3 mm sub def\n"
	" /currx currx 3 mm add def\n"
	" ArrayOfArrays\n"
	" {\n"
	"  /ThisArray exch def\n"
	"  currx curry TotalLevels ThisArray LegendLineItem\n"
	"  /curry curry 4 mm sub def\n"
	" }forall\n"
	" %\n"
	" grestore\n"
	"}bind def\n"
	,fp);
	
}

/*                  */
/* LegendLineItem() */
/*                  */
void LegendLineItem(FILE *fp){
	
	fputs(
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"%\n"
	"% LegendLineItem: Prints the keys to each categorical level of an icon variable\n"
	"% for the drawing's icon display legend\n"
	"%\n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"/LegendLineItem {\n"
	" /ThisArray exch def\n"
	" /TotalLevels exch def\n"
	" /cy exch def\n"
	" /cx exch def\n"
	"\n"
	" ThisArray aload pop\n"
	" /BluC exch def\n"
	" /GrnC exch def\n"
	" /RedC exch def\n"
	" /GrayLevel exch def\n"
	" /ValueLabel exch def\n"
	" /IconLabel exch def\n"
	" /CurrentLevel exch def\n"
	" gsave\n"
	" IconFont IconFontSize scalefont setfont\n"
	" newpath\n"
	" cx 3 mm sub cy 1.5 mm add moveto\n"
	" 0 -3 mm rlineto\n"
	" 6 mm 0 rlineto\n"
	" 0 3 mm rlineto\n"
	" closepath\n"
	" GrayLevel RedC GrnC BluC FillRegion\n"
	" 0.5 setlinewidth\n"
	" stroke\n"
	" cx IconLabel stringwidth pop 2 div sub cy IconFontSize 3 div sub moveto\n"
	" GrayLevel IconLabel ShowRegionText\n"
	" cx ValueLabel stringwidth pop 2 div sub 1.5 cm add cy IconFontSize 3 div sub moveto\n"
	" ValueLabel show\n"
	" grestore\n"
	"}bind def\n"
	,fp);
	
}

/*                 */
/* CircleHighlight */
/*                 */
void CircleHighlight(FILE *fp){
	
	fputs(
	"%\n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"%\n"
	"% CircleHighlight: Prints a circular region in color\n"
	"%\n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"/CircleHighlight {\n"
	" /Ycenter exch def\n"
	" /Xcenter exch def\n"
	" gsave\n"
	"  newpath\n"
	"  Xcenter Ycenter IconRadius 2 mul 0 360 arc\n"
	"  gsave\n"
	"   LightBlue\n"
	"   fill\n"
	"  grestore\n"
	"  Blue\n"
	"  stroke\n"
	" grestore\n"
	"} bind def\n"
	,fp);
	
}

