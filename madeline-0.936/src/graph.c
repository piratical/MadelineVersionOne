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
/* "graph.c" is part of the Madeline software distribution.                          */
/* Written by Edward H. Trager.                                                      */
/*                                                                                   */
/* COPYRIGHT (C) 2000,2002 THE REGENTS OF THE UNIVERSITY OF MICHIGAN,                */
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
/* Last modified by Ed Trager, July 2002.                                            */
/*                                                                                   */
/* ********************************************************************************* */

/*                                                */
/* HISTORY:                                       */
/*                                                */
/* "graph.c" was created on 2002.07.29            */
/* by ET in order to add LOD plotting             */
/* functionality (initially using Simwalk         */
/* results) to Madeline                           */
/*                                                */
/* Last modified: 2002.07.29 by ET                */
/*                                                */
#include "graph.h"

/* for RESULT definition, intra alia: */
#include "madeline.h"

/* for SmartTableSelect() functionality: */
#include "smartselect.h"

#ifndef ANNOTATION_INCLUDED
#include "annotation.h" /* for definition of GraphAnnotation() function */
#endif


/*              */
/* void Graph() */
/*              */
void Graph(void){
	
	RESULT result;
	extern PCONTROL g;
	const char *fileToOpen;
	
	/*                                                */
	/* After the keyword "graph", get the next token: */
	/*                                                */
	
	GetToken();
	
	switch(pT->i){
	case ADDK:
		/*                                 */
		/* ADD an annotation to the graph: */
		/*                                 */
		GraphAnnotation();
		break;
		
	case LOADC:
		/*                                                  */
		/* LOAD a map table: Equivalent to the LOAD command */
		/*                                                  */
		EvaluateExpression(&result);  /* get the filename */
		if(result.t!=STRINGVALUE)
			MadelineErrorReport("Graph() LOAD directive: Specify a genetic map data table to open.");
		/*                                            */
		/* SmartTableSelect figures out whether the   */
		/* user specified the .data or the .mfh file  */
		/* and checks the usability of the .mfh file. */
		/* If the .mfh does not exist, recognize() is */
		/* called to create it:                       */
		/*                                            */
		fileToOpen=SmartTableSelect(result.s);
		GetMapData(g,fileToOpen);
		break;
		
	case OPENC:
		/*                         */
		/* OPEN a graph data table */
		/*                         */
		EvaluateExpression(&result);  /* get the filename */
		if(result.t!=STRINGVALUE)
			MadelineErrorReport("Graph() OPEN directive: Specify a graph data table to open.");
		/*                                            */
		/* SmartTableSelect figures out whether the   */
		/* user specified the .data or the .mfh file  */
		/* and checks the usability of the .mfh file. */
		/* If the .mfh does not exist, recognize() is */
		/* called to create it:                       */
		/*                                            */
		fileToOpen=SmartTableSelect(result.s);
		GraphOpen(fileToOpen);
		break;
		
	case PLOTK:
		/*                                 */
		/* PLOT the data in the data table */
		/*                                 */
		GraphPlot(g);
		break;
		
	default:
		
		MadelineErrorReport("Graph(): Unrecognized GRAPH command.");
		break;
		
	}
}

/*             */
/* GraphOpen() */
/*             */
void GraphOpen(const char *s){
	
	int table_type;
	double cM_min,cM_max,vu_min,vu_max;
	extern PCONTROL g;
	extern VARIABLE gfnGraphPosition;
	extern VARIABLE gfnGraphScore;
	extern VARIABLE gGraphXAxisMin, gGraphXAxisMax, gGraphYAxisMin, gGraphYAxisMax;
	extern VARIABLE gGraphXMajorTick,gGraphXMinorTick,gGraphYMajorTick,gGraphYMinorTick;
	
	/*                                                       */
	/* Make sure that the table is in an identifiable format */
	/*                                                       */
	table_type=CharacterizeDatabase(s);
	if(!table_type) MadelineErrorReport("GraphOpen(): \"%s\" is not in a recognized table format or is corrupt\n(For flat files, be sure to open the Madeline %s file).",s,HEADER_FILE_EXTENSION);
	/*                              */
	/* Set up and open the database */
	/*                              */
	g->graphdb=NewDatabase(s,READONLY,table_type);
	g->graphdb->OpenDatabase(g->graphdb);
	GoTo(g->graphdb,0);    /* set pointer to the top */
	/*                                                                      */
	/* Only two columns, POSITION and SCORE, are required in a graph table: */
	/*                                                                      */
	gfnGraphPosition.i   =g->graphdb->FieldNumber(g->graphdb,gfnGraphPosition.e.s);
	gfnGraphScore.i      =g->graphdb->FieldNumber(g->graphdb,gfnGraphScore.e.s   );
	
	if(gfnGraphPosition.i==MISSING)
		MadelineErrorReport("GraphOpen(): Unable to locate the required POSITION field in this table.");
	if(gfnGraphScore.i==MISSING)
		MadelineErrorReport("GraphOpen(): Unable to locate the required SCORE field in this table. ");
	
	/* Store calculated graph range endpoints:  */
	
	/*                                          */
	/* Calculate the graph X and Y axis ranges: */
	/*                                          */
	GraphCalculateMinimaMaxima(g->graphdb,gfnGraphPosition.i,gfnGraphScore.i,&cM_min,&cM_max,&vu_min,&vu_max);
	/*                                                                   */
	/* Calculate the default "autosized" endpoints for the X and Y axes  */
	/* and the "autosized" tick intervals:                               */
	/*                                                                   */
	GraphCalculateEndpointsAndIntervals(
		cM_min,
		cM_max,
		X_AXIS_MINIMUM_INTERVALS,
		X_AXIS_MAXIMUM_INTERVALS,
		&gGraphXAxisMin.e.v,
		&gGraphXAxisMax.e.v,
		&gGraphXMajorTick.e.v,
		&gGraphXMinorTick.e.v
	);
	GraphCalculateEndpointsAndIntervals(
		vu_min,
		vu_max,
		Y_AXIS_MINIMUM_INTERVALS,
		Y_AXIS_MAXIMUM_INTERVALS,
		&gGraphYAxisMin.e.v,
		&gGraphYAxisMax.e.v,
		&gGraphYMajorTick.e.v,
		&gGraphYMinorTick.e.v
	);
	
	GraphResetSequentialIDs(g);
	
}

/*                           */
/* GraphResetSequentialIDs() */
/*                           */
void GraphResetSequentialIDs(PCONTROL g){
	
	/*                                                                                   */
	/* Reset the horizontal and vertical line sequential ids used for graph annotations: */
	/* Here we just recycle the g->psid and g->isid in the CONTROL structure:            */
	/*                                                                                   */
	g->psid=g->isid=0;
	
}

/*                              */
/* GraphCalculateMinimaMaxima() */
/*                              */
void GraphCalculateMinimaMaxima(PDBF db,UINT16 cM_idx,UINT16 vu_idx,double *cM_min,double *cM_max,double *vu_min,double *vu_max){

	double cM;
	double vu;
	unsigned int i;
	
	*cM_min=*vu_min=DOUBLE_MAXIMUM;
	*cM_max=*vu_max=DOUBLE_MINIMUM;
	
	/*                           */
	/* loop through all records: */
	/*                           */
	for(i=0;i<db->rc;i++){

		/*                           */
		/* get individual's genotype */
		/*                           */
		cM=db->ReadDouble(db,cM_idx,i);
		vu=db->ReadDouble(db,vu_idx,i);
		
		if(cM!=DOUBLEMISSINGVALUE){
			if(cM>*cM_max) *cM_max=cM;
			if(cM<*cM_min) *cM_min=cM;
		}
		if(vu!=DOUBLEMISSINGVALUE){
			if(vu>*vu_max) *vu_max=vu;
			if(vu<*vu_min) *vu_min=vu;
		}
		
	}
	
}

/*             */
/* GraphPlot() */
/*             */
void GraphPlot(PCONTROL g){
	
	char *args[3];
	
	extern VARIABLE gfnGraphPosition,gfnGraphScore;
	extern VARIABLE gGraphDrawing;
	extern VARIABLE gGraphTitle,gGraphXAxisLabel,gGraphYAxisLabel;
	extern VARIABLE gPSViewerName;
	extern VARIABLE gGraphXAxisMin, gGraphXAxisMax, gGraphYAxisMin, gGraphYAxisMax;
	extern VARIABLE gGraphXMajorTick,gGraphXMinorTick,gGraphYMajorTick,gGraphYMinorTick;
	extern ASSOCARRAY aaga;
	
	/*                                 */
	/* Database must have been opened: */
	/*                                 */
	if(!g->graphdb)
		MadelineErrorReport("GraphPlot(): No graph data table is open.");
	
	/*                              */
	/* Open the graph drawing file: */
	/*                              */
	g->streams.graph=fopen(gGraphDrawing.e.s,"wb");
	if(!g->streams.graph) MadelineErrorReport("GraphPlot(): Unable to open %s for drawing.",gGraphDrawing.e.s);
	
	/*                                                             */
	/* Download Madeline's core PostScript graph drawing routines: */
	/*                                                             */
	/* NOTE BENE: The graph axes starting and ending positions are */
	/* calculated when GRAPH OPEN is run.  The results are stored  */
	/* in user-accessible variables, like GraphXAxixMinimum, which */
	/* can be accessed and modified by the user before the GRAPH   */
	/* PLOT command is called here:                                */
	/*                                                             */ 
	GraphPostscriptProlog(g->streams.graph,gGraphXAxisMin.e.v,gGraphXAxisMax.e.v,gGraphYAxisMin.e.v,gGraphYAxisMax.e.v);
	/*                          */
	/* Start drawing the graph: */
	/*                          */
	GraphStart(g->streams.graph);
	
	GraphTickMarks(
		g->streams.graph,
		gGraphXAxisMin.e.v,
		gGraphXAxisMax.e.v,
		gGraphXMinorTick.e.v,
		gGraphXMajorTick.e.v,
		gGraphYAxisMin.e.v,
		gGraphYAxisMax.e.v,
		gGraphYMinorTick.e.v,
		gGraphYMajorTick.e.v
	);
	
	GraphTitles(g->streams.graph,gGraphTitle.e.s,gGraphXAxisLabel.e.s,gGraphYAxisLabel.e.s);
	/*                                                  */
	/* Draw marker labels on graph if a map is present: */
	/*                                                  */
	if(g->mp) GraphMarkerLabels(g->streams.graph,g->mp,1);
	
	
	/*                */
	/* Plot the data: */
	/*                */
	GraphPlotData(g->streams.graph,g->graphdb,gfnGraphPosition.i,gfnGraphScore.i,gGraphXAxisMin.e.v,gGraphYAxisMin.e.v);
	
	/*                    */
	/* Graph Annotations: */
	/*                    */
	if(aaga.n){
		/*                                                 */
		/* Translate to 0,0 in order to print annotations: */
		/*                                                 */
		GraphStartAnnotations(g->streams.graph, gGraphXAxisMin.e.v, gGraphYAxisMin.e.v);
		/*                    */
		/* Traverse the tree: */
		/*                    */
		TraverseTree(aaga.h,GraphPrintAssociativeArrayStringValue);
		GraphEndAnnotations(g->streams.graph);
	}
	/*                   */
	/* Finish the graph: */
	/*                   */
	GraphFinish(g->streams.graph);
	
	/*                   */
	/* Close graph file: */
	/*                   */
	fclose(g->streams.graph);
	lprintf("Graph printed to \"%s\"\n",gGraphDrawing.e.s);
	/*              */
	/* open viewer: */
	/*              */
	if(gPSViewerName.e.s[0]){
		
		lprintf("Calling external viewer using the command \"%s %s\" ...\n",gPSViewerName.e.s,gGraphDrawing.e.s);
		args[0]=gPSViewerName.e.s;
		args[1]=gGraphDrawing.e.s;
		args[2]=NULL;
		CreateChildProcess(gPSViewerName.e.s,args);
		
	}
	
}


/*                           */
/* GraphCalculateEndpoints() */
/* --> Given low and high,   */
/*     return the stt and    */
/*     end points for graph- */
/*     ing.                  */
/*                           */
void GraphCalculateEndpointsAndIntervals(double low,double high,int MinimumIntervals,int MaximumIntervals,double *stt,double *end,double *MajorTick,double *MinorTick){
	
	double range,magnitude,NewRange,TickBasis,IntervalBasis;
	double tolerance=10e-6;
	
	int i,multiplier[MULTIPLIER_COUNT]={10,5,2,1};
	
	range=high-low;
	magnitude=floor(log10(range))-1;
	/* DEBUG */
	lprintf("Low=%.2f High=%.2f Range=%.2f Magnitude=%.0f\n",low,high,range,magnitude);
	
	if(floor(low)==low) *stt=low;
	else *stt=GraphRoundDown(low,magnitude);
	if(ceil(high)==high) *end=high;
	else *end=GraphRoundUp(high,magnitude);
	NewRange=*end-*stt;
	/* DEBUG */
	lprintf("Stt=%.2f End =%.2f NewRange=%.2f\n",*stt,*end,NewRange);
	
	/*                                                                                  */
	/* The tick interval is calculated as a multiple (1,2, or 5) of the base magnitude. */
	/* After the Tick interval has been decided, the stt and end may be readjusted to   */
	/* be evenly divisible by the tick interval:                                        */
	/*                                                                                  */
	TickBasis=pow(10,magnitude);
	/* DEBUG */
	lprintf("TickBasis=%.2f\n",TickBasis);
	for(i=0;i<MULTIPLIER_COUNT;i++){
		*MajorTick=TickBasis*multiplier[i];
		IntervalBasis = NewRange / *MajorTick;
		if(IntervalBasis>=MinimumIntervals && IntervalBasis<=MaximumIntervals){
			break;
		}
	}
	/*                                              */
	/* Minor Tick is just half of Major by default: */
	/*                                              */
	*MinorTick = *MajorTick / 2.0;
	/* DEBUG */
	lprintf("i=%i MajorTick is now %.2f\n",i,*MajorTick);
	/*                                                               */
	/* Adjust stt and end so they are evenly divisible by MajorTick: */
	/*                                                               */
	for(i=9;i && fabs(fmod(*stt,*MajorTick))>tolerance;*stt-=TickBasis,i--) lprintf("i=%i Adj. stt: %.2f rem=%g\n",i,*stt,fmod(*stt,*MajorTick));
	for(i=9;i && fabs(fmod(*end,*MajorTick))>tolerance;*end+=TickBasis,i--) lprintf("i=%i Adj. end: %.2f rem=%g\n",i,*end,fmod(*end,*MajorTick));
	
}

/* GraphRoundDown
   Rounds a real number n
   down to the next lowest real number
   with d decimal places.  Returns
   the rounded number.
*/
double GraphRoundDown(double n,double d){
	
	double r,p,q;
	
	p = pow(10,-d);  /* figure the inverse of the power of 10 maginitude */
	q = pow(10, d);  /* figure the power of 10 magnitude                 */
	r = floor(n*p); /* floor the number multiplied by the inverse magn. */
	return r*= q;   /* back multiply by the power of 10 magnitude       */
	
}

/* GraphRoundUp
   Rounds a real number n
   up to the next highest real number
   with d decimal places.  Returns
   the rounded number.
*/
double GraphRoundUp(double n,double d){
	
	double r,p,q;
	
	p = pow(10,-d);  /* figure the inverse of the power of 10 maginitude */
	q = pow(10, d);  /* figure the power of 10 magnitude                 */
	r = ceil(n*p);  /* ceiling the number multiplied by the inverse mag.*/
	return r*= q;   /* back multiply by the power of 10 magnitude       */
}

/*                         */
/* GraphStartAnnotations() */
/*                         */
void GraphStartAnnotations(FILE *fp,double GraphXMin,double GraphYMin){
	
	fputs( 
	"%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"%\n"
	"% Start Graph Annotations:\n"
	"%\n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	,fp);
	fprintf(fp,"%.2f cM neg %.2f neg vu translate\n",GraphXMin,GraphYMin);
	return;
	
}

/*                                          */
/* GraphPrintAssociativeArrayStringValue(): */
/* Print the value                          */
/*                                          */
void GraphPrintAssociativeArrayStringValue(void *KeyValuePair){
	
	PKEYVALUEPAIR pkvp;
	extern PCONTROL g;
	
	pkvp=(PKEYVALUEPAIR)KeyValuePair;
	
	fprintf(g->streams.graph,"%s\n",pkvp->value.e.s);
	
	return;
	
}

/*                       */
/* GraphEndAnnotations() */
/*                       */
void GraphEndAnnotations(FILE *fp){

	fputs(
	"%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"%\n"
	"% End of Graph Annotations\n"
	"%\n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	,fp);
	
	return;
	
}

/*                         */
/* GraphPostscriptProlog() */
/*                         */
void GraphPostscriptProlog(FILE *fp,double cM_stt,double cM_end,double vu_stt,double vu_end){
	
	fputs(
	"%!PS-Adobe-2.0\n"
	"%%Creator: Madeline-0.935 2002-07-12\n"
	"%%LanguageLevel: 2\n"
	"%%DocumentMedia: plain 612 792 0 () ()\n"
	"%%Pages: 1\n"
	"%%EndComments\n"
	"%%BeginDefaults\n"
	"%%PageMedia: plain\n"
	"%%PageOrientation: Seascape\n"
	"%%EndDefaults\n"
	"\n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"%\n"
	"% *** START OF PROLOG ***\n"
	"%\n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"\n"
	"%%%%%%%%%%%%%%%\n"
	"%\n"
	"% Unit metrics:\n"
	"%\n"
	"%%%%%%%%%%%%%%%\n"
	"/inch {72 mul}      bind def\n"
	"/cm {inch 2.54 div} bind def\n"
	"/mm {cm 10 div}     bind def\n"
	"\n"
	"%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"%\n"
	"% Page Size:\n"
	"%\n"
	"%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"/PageLongAxis  11.0 inch def\n"
	"/PageShortAxis 8.5 inch def\n"
	"\n"
	"/HzAxisLength 22 cm def\n"
	"/VtAxisLength 15 cm def\n"
	"/BoundaryOffset 3 mm def\n"
	"/TickLength 2 mm def\n"
	"/HzCenter HzAxisLength 2 div def\n"
	"/VtCenter VtAxisLength 2 div def\n"
	"\n"
	"\n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"%\n"
	"% Horizontal Axis Units = centiMorgans = cM\n"
	"%\n"
	"% This is based on the range in cM\n"
	"% to be displayed on the graph:\n"
	"%\n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"% Requires the starting and ending\n"
	"% labels from the horizontal axis:\n"
	"%\n"
	,fp);
	
	/*                                   */
	/* Starting and ending centiMorgans: */
	/*                                   */
	fprintf(fp,"/Starting_cM %.2f def\n",cM_stt);
	fprintf(fp,"/Ending_cM %.2f def\n",cM_end);
	
	fputs(
	"/Range_cM Ending_cM Starting_cM sub def\n"
	"/Units_per_cM HzAxisLength Range_cM div def\n"
	"/cM { Units_per_cM mul } bind def\n"
	"\n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"%\n"
	"% Vertical Axis Units = vertical units = vu\n"
	"%\n"
	"% This is based on the range in vertical \n"
	"% units to be displayed on the graph:\n"
	"% \n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"% Requires the starting and ending\n"
	"% labels from the horizontal axis:\n"
	"%\n"
	,fp);
	/*                                     */
	/* Starting and ending vertical units: */
	/*                                     */
	fprintf(fp,"/Starting_vu %.2f def\n",vu_stt);
	fprintf(fp,"/Ending_vu   %.2f def\n",vu_end);
	
	fputs(
	"/Range_vu Ending_vu Starting_vu sub def\n"
	"/Units_per_vu VtAxisLength Range_vu div def\n"
	"/vu { Units_per_vu mul } bind def\n"
	"\n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"% Registration for lower left (0,0) corner of graph:\n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"/HzRegistration 3.0 cm def\n"
	"/VtRegistration 4.0 cm def\n"
	"\n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"%\n"
	"% Global LastPosition and DisplayLevel \n"
	"% (For Marker labeling):\n"
	"%\n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"/LastPosition -1.0 def\n"
	"/DisplayLevel 0 def\n"
	"\n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"% \n"
	"% MarkerLabel-related constants:\n"
	"% \n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"/Y1 -0.30 cm def\n"
	"/Y2 -0.85 cm def\n"
	"/Y3 -0.12 cm def\n"
	"/Y4 -0.20 cm def\n"
	"/ABOVE true def\n"
	"/BELOW false def\n"
	"\n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"%\n"
	"% Font Routines\n"
	"%\n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"/TitleFontSize 32 def\n"
	"/HeaderFontSize 14 def\n"
	"/AxisLabelFontSize 20 def\n"
	"/LabelFontSize 12 def\n"
	"/MarkerFontSize 10 def\n"
	"/HzMinimumGap MarkerFontSize 1.414 mul def\n"
	"/SansSerifFont {/size exch def /Helvetica findfont size scalefont setfont} bind def\n"
	"/SerifFont {/size exch def /Times-Roman findfont size scalefont setfont} bind def\n"
	"/BoldSansSerifFont {/size exch def /Helvetica-Bold findfont size scalefont setfont} bind def\n"
	"/BoldSerifFont {/size exch def /Times-Roman-Bold findfont size scalefont setfont} bind def\n"
	"\n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"%\n"
	"% Landscape:\n"
	"%\n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"/Landscape {\n"
	"  0 PageLongAxis translate\n"
	" -90 rotate\n"
	"} bind def\n"
	"\n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"%\n"
	"% TranslateToZeroPoint:\n"
	"%\n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"/TranslateToZeroPoint {\n"
	" HzRegistration VtRegistration translate\n"
	"} bind def\n"
	"\n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"%\n"
	"% DrawGraphBoundary\n"
	"% \n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"/DrawGraphBoundary {\n"
	" gsave\n"
	" 0 setgray\n"
	" 0.5 mm setlinewidth\n"
	" BoundaryOffset neg BoundaryOffset neg moveto\n"
	" /Vt VtAxisLength BoundaryOffset 2 mul add def\n"
	" /Hz HzAxisLength BoundaryOffset 2 mul add def\n"
	" 0 Vt rlineto\n"
	" Hz 0 rlineto\n"
	" 0 Vt neg rlineto\n"
	" closepath\n"
	" stroke\n"
	" grestore\n"
	"} bind def\n"
	"\n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"%\n"
	"% HorizontalLine: \n"
	"%\n"
	"% This is used for graph annotations,\n"
	"% so the translation point is wherever\n"
	"% 0,0 is on the graph.  This way, the\n"
	"% user can annotate the graph directly\n"
	"% \n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"/HorizontalLine {\n"
	" gsave\n"
	" /Y exch def\n"
	" /Hz HzAxisLength BoundaryOffset 2 mul add def\n"
	" 0 setgray\n"
	" [] 0 setdash\n"
	" 0.5 mm setlinewidth\n"
	" BoundaryOffset neg Starting_cM cM add Y moveto\n"
	" Hz 0 rlineto\n"
	" stroke\n"
	" grestore\n"
	"} bind def\n"
	"\n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"%\n"
	"% VerticalLine\n"
	"% \n"
	"% This is used for graph annotations,\n"
	"% so the translation point is wherever\n"
	"% 0,0 is on the graph.  This way, the\n"
	"% user can annotate the graph directly\n"
	"% \n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"/VerticalLine {\n"
	" gsave\n"
	" /X exch def\n"
	" /Vt VtAxisLength BoundaryOffset 2 mul add def\n"
	" 0 setgray\n"
	" [] 0 setdash\n"
	" 0.5 mm setlinewidth\n"
	" X BoundaryOffset neg Starting_vu vu add moveto\n"
	" 0 Vt rlineto \n"
	" stroke\n"
	" grestore\n"
	"} bind def\n"
	"\n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"% \n"
	"% HzAxisInterTick\n"
	"% \n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"/HzAxisInterTick {\n"
	" /X exch def\n"
	" X BoundaryOffset neg moveto\n"
	" 0 TickLength 2 div neg rlineto stroke\n"
	"} bind def\n"
	"\n"
	"\n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"% \n"
	"% HzAxisTick\n"
	"% \n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"/HzAxisTick {\n"
	" /Label exch def\n"
	" /X exch def\n"
	" X BoundaryOffset neg moveto\n"
	" 0 TickLength neg rlineto stroke\n"
	" X Label stringwidth pop 2 div sub BoundaryOffset neg TickLength sub AxisLabelFontSize sub moveto\n"
	" Label show\n"
	"} bind def\n"
	"\n"
	"%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"% \n"
	"% DrawHzTicks\n"
	"% \n"
	"%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"/DrawHzTicks {\n"
	" /BigInterval exch def\n"
	" /LittleInterval exch def\n"
	" /End exch def\n"
	" /Stt exch def\n"
	"\n"
	" /Range End Stt sub def\n"
	" /hold 20 string def\n"
	" \n"
	" 0 LittleInterval Range\n"
	" {\n"
	"  /CurrentI exch def\n"
	"  %\n"
	"  % ifelse to determine whether to print labeled\n"
	"  % or unlabeled tick mark. We use \"floor\" \n"
	"  % because \"mod\" only works with integers\n"
	"  % in Postscript:\n"
	"  % \n"
	"  /Quotient CurrentI BigInterval div def\n"
	"  Quotient floor Quotient eq\n"
	"  {\n"
	"   %\n"
	"   % Print Labeled tick mark without decimal\n"
	"   % places if the floating point number is\n"
	"   % really just an integer.  Here again, we\n"
	"   % use floor to detect if the number is \n"
	"   % integral:\n"
	"   % \n"
	"   /L CurrentI Stt add def\n"
	"   L floor L eq\n"
	"   {\n"
	"    /L L cvi def \n"
	"   }\n"
	"   if\n"
	"   CurrentI cM L 10 hold cvrs HzAxisTick \n"
	"  }\n"
	"  {\n"
	"   %\n"
	"   % Print unlabeled tick mark:\n"
	"   % \n"
	"    CurrentI cM HzAxisInterTick  \n"
	"  }\n"
	"  ifelse\n"
	" }\n"
	" for\n"
	"} bind def\n"
	"\n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"% \n"
	"% VtAxisInterTick\n"
	"% \n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"/VtAxisInterTick {\n"
	" /Y exch def\n"
	" BoundaryOffset neg Y moveto\n"
	" TickLength 2 div neg 0 rlineto stroke\n"
	"} bind def\n"
	"\n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"% \n"
	"% VtAxisInterTick\n"
	"% \n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"/VtAxisTick {\n"
	" /Label exch def\n"
	" /Y exch def\n"
	" BoundaryOffset neg Y moveto\n"
	" TickLength neg 0 rlineto stroke\n"
	" BoundaryOffset TickLength add neg Y moveto\n"
	" Label stringwidth pop TickLength add neg AxisLabelFontSize 3 div neg rmoveto\n"
	" Label show\n"
	"} bind def\n"
	"\n"
	"%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"% \n"
	"% DrawVtTicks\n"
	"% \n"
	"%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"/DrawVtTicks {\n"
	" /BigInterval exch def\n"
	" /LittleInterval exch def\n"
	" /End exch def\n"
	" /Stt exch def\n"
	"\n"
	" /Range End Stt sub def\n"
	" /hold 20 string def\n"
	" \n"
	" 0 LittleInterval Range\n"
	" {\n"
	"  /CurrentI exch def\n"
	"  %\n"
	"  % ifelse to determine whether to print labeled\n"
	"  % or unlabeled tick mark. We use \"floor\" \n"
	"  % because \"mod\" only works with integers\n"
	"  % in Postscript:\n"
	"  % \n"
	"  /Quotient CurrentI BigInterval div def\n"
	"  Quotient floor Quotient eq\n"
	"  {\n"
	"   %\n"
	"   % Print Labeled tick mark without decimal\n"
	"   % places if the floating point number is\n"
	"   % really just an integer.  Here again, we\n"
	"   % use floor to detect if the number is \n"
	"   % integral:\n"
	"   % \n"
	"   /L CurrentI Stt add def\n"
	"   L floor L eq\n"
	"   {\n"
	"    /L L cvi def \n"
	"   }\n"
	"   if\n"
	"   CurrentI vu L 10 hold cvrs VtAxisTick \n"
	"  }\n"
	"  {\n"
	"   %\n"
	"   % Print unlabeled tick mark:\n"
	"   % \n"
	"    CurrentI vu VtAxisInterTick  \n"
	"  }\n"
	"  ifelse\n"
	" }\n"
	" for\n"
	"} bind def\n"
	"\n"
	"%%%%%%%%%%%%%%%%%%%\n"
	"% \n"
	"% Title\n"
	"% \n"
	"%%%%%%%%%%%%%%%%%%%\n"
	"/Title {\n"
	" /Label exch def\n"
	" TitleFontSize SerifFont \n"
	" HzAxisLength 2 div Label stringwidth pop 2 div sub VtAxisLength 1 cm add moveto\n"
	" Label show\n"
	"} bind def\n"
	"\n"
	"%%%%%%%%%%%%%%%%%%%\n"
	"% \n"
	"% HzAxisLabel\n"
	"% \n"
	"%%%%%%%%%%%%%%%%%%%\n"
	"/HzAxisLabel {\n"
	" /Label exch def\n"
	" AxisLabelFontSize SerifFont \n"
	" HzAxisLength 2 div Label stringwidth pop 2 div sub -2 cm moveto\n"
	" Label show\n"
	"} bind def\n"
	"\n"
	"%%%%%%%%%%%%%%%%%%%\n"
	"% \n"
	"% VtAxisLabel\n"
	"% \n"
	"%%%%%%%%%%%%%%%%%%%\n"
	"/VtAxisLabel {\n"
	" /Label exch def\n"
	" gsave\n"
	" AxisLabelFontSize SerifFont\n"
	" -2.0 cm VtAxisLength 2 div moveto\n"
	" 90 rotate\n"
	" Label stringwidth pop 2 div neg 0 rmoveto\n"
	" Label show\n"
	" grestore\n"
	"} bind def\n"
	"\n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"%\n"
	"% LastPositionReset\n"
	"%\n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"/LastPositionReset {\n"
	" /LastPosition -1.0 def\n"
	"} bind def\n"
	"\n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"%\n"
	"% MarkerLabel\n"
	"% \n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"/MarkerLabel {\n"
	" /XcM exch def\n"
	" /Label exch def\n"
	" gsave\n"
	" MarkerFontSize SansSerifFont\n"
	" 0.03 cm setlinewidth\n"
	" 0.0 setgray\n"
	" newpath\n"
	" XcM BoundaryOffset neg moveto\n"
	" 0 Y1 rlineto\n"
	" LastPosition 0 gt{\n"
	"  XcM LastPosition sub HzMinimumGap lt{\n"
	"   /NewPosition LastPosition HzMinimumGap add def\n"
	"  }\n"
	"  {\n"
	"   /NewPosition XcM def\n"
	"  }\n"
	"  ifelse\n"
	" }\n"
	" {\n"
	"  /NewPosition XcM def \n"
	" }\n"
	" ifelse\n"
	" NewPosition Y1 Y2 add lineto\n"
	" 0 Y3 rlineto\n"
	" Y4 Y4 rlineto\n"
	" stroke\n"
	" %NewPosition 0.2 cm sub -1.2 cm translate 45 rotate\n"
	" NewPosition Y4 add Y1 Y2 add Y3 add Y4 add translate\n"
	" 45 rotate\n"
	" 0 Label stringwidth pop 0.1 cm add sub MarkerFontSize -3 div moveto\n"
	" Label show\n"
	" grestore\n"
	" %\n"
	" % Save the last position:\n"
	" %\n"
	" /LastPosition NewPosition def\n"
	"} bind def\n"
	"\n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"%\n"
	"% VtMarkerLabel: VerticalMarkerLabel\n"
	"% \n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"/VtMarkerLabel {\n"
	" /XcM exch def\n"
	" /Label exch def\n"
	" gsave\n"
	" MarkerFontSize SansSerifFont\n"
	" XcM VtAxisLength translate 90 rotate\n"
	" LastPosition 0 gt{\n"
	"  XcM LastPosition sub MarkerFontSize lt{\n"
	"   /DisplayLevel DisplayLevel 1 add def\n"
	"   DisplayLevel 3 eq {\n"
	"    /DisplayLevel 0 def\n"
	"   }\n"
	"   if\n"
	"  }\n"
	"  {\n"
	"   /DisplayLevel 0 def\n"
	"  }ifelse\n"
	" }\n"
	" if\n"
	" 0 Label stringwidth pop 0.1 cm add 1.6 cm DisplayLevel mul add sub MarkerFontSize -3 div moveto\n"
	" Label show\n"
	" grestore\n"
	" %\n"
	" % Save the last position:\n"
	" %\n"
	" /LastPosition XcM def\n"
	"} bind def\n"
	"\n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"%\n"
	"% Box: Shade and frame a \n"
	"% rectangular region\n"
	"% \n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"/Box {\n"
	" /dy exch def\n"
	" /VtPosition exch def\n"
	" /EndX exch def\n"
	" /StartX exch def\n"
	"\n"
	" /LeftTopY VtPosition dy 2 div add def\n"
	" /dx EndX StartX sub def\n"
	" gsave\n"
	" newpath\n"
	" StartX LeftTopY moveto\n"
	" dx 0 rlineto\n"
	" 0 dy neg rlineto\n"
	" dx neg 0 rlineto\n"
	" closepath\n"
	" %\n"
	" % Fill box using current color\n"
	" % or shade of gray:\n"
	" % \n"
	" gsave\n"
	" fill\n"
	" grestore\n"
	" %\n"
	" % Frame the box:\n"
	" %\n"
	" 0 setgray\n"
	" [] 0 setdash\n"
	" 0.3 mm setlinewidth\n"
	" stroke\n"
	" grestore\n"
	"} bind def\n"
	"\n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"%\n"
	"% Region: Shade a rectangular\n"
	"% region without framing it\n"
	"% \n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"/Region {\n"
	" /EndY exch def\n"
	" /StartY exch def\n"
	" /EndX exch def\n"
	" /StartX exch def\n"
	"\n"
	" /dx EndX StartX sub def\n"
	" /dy EndY StartY sub def\n"
	" newpath\n"
	" StartX StartY moveto\n"
	" dx 0 rlineto\n"
	" 0 dy rlineto\n"
	" dx neg 0 rlineto\n"
	" closepath\n"
	" %\n"
	" % Fill box using current color\n"
	" % or shade of gray:\n"
	" % \n"
	" gsave\n"
	" fill\n"
	" grestore\n"
	"} bind def\n"
	"\n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"%\n"
	"% LabeledBar\n"
	"% \n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"/LabeledBar {\n"
	" /Label exch def\n"
	" /AboveBelow exch def\n"
	" /dy exch def\n"
	" /VtPosition exch def\n"
	" /EndX exch def\n"
	" /StartX exch def\n"
	" %\n"
	" % Draw the bar:\n"
	" %\n"
	" StartX EndX VtPosition dy Box \n"
	" %\n"
	" % Label the bar:\n"
	" %\n"
	" gsave\n"
	" /HzCenter StartX EndX StartX sub 2 div add def\n"
	" 0 setgray\n"
	" LabelFontSize SansSerifFont\n"
	" AboveBelow {\n"
	"  HzCenter Label stringwidth pop 2 div neg add VtPosition dy 2 div add 2 mm add moveto\n"
	" }\n"
	" {\n"
	"  HzCenter Label stringwidth pop 2 div neg add VtPosition dy 2 div sub LabelFontSize sub 2 mm sub moveto \n"
	" }\n"
	" ifelse\n"
	" Label show\n"
	" grestore\n"
	" \n"
	"} bind def\n"
	"\n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"% \n"
	"% Annotation\n"
	"% \n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"/Annotation {\n"
	" /Label exch def\n"
	" /y exch def\n"
	" /x exch def\n"
	" gsave\n"
	" 0 setgray\n"
	" LabelFontSize SansSerifFont\n"
	" x y moveto\n"
	" Label show\n"
	" grestore\n"
	"} bind def\n"
	"\n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"% \n"
	"% ArrowAnnotation\n"
	"% \n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"/ArrowAnnotation {\n"
	" /Label      exch def\n"
	" /Angle      exch def\n"
	" /Y          exch def\n"
	" /X          exch def\n"
	"\n"
	" /HeadWidth  3 mm def\n"
	" /HeadLength 4 mm def \n"
	" /HalfHeadWidth HeadWidth 2 div def  \n"
	" gsave\n"
	" 0 setgray\n"
	" [] 0 setdash\n"
	" LabelFontSize SansSerifFont\n"
	" X Y translate\n"
	" Angle rotate\n"
	" 0 0 moveto\n"
	" HeadLength HalfHeadWidth rlineto\n"
	" 0 HeadWidth neg rlineto\n"
	" closepath\n"
	" fill\n"
	" newpath\n"
	" HeadLength 0 moveto\n"
	"\n"
	" 2 mm 0 rlineto\n"
	" Angle neg rotate\n"
	"\n"
	" Angle abs 90 gt {\n"
	"  5 mm neg 0 rlineto \n" 
	"  currentpoint /Y exch LabelFontSize 3 div sub def /X exch 1 mm sub def\n"
	"  stroke\n"
	"  X Label stringwidth pop sub Y  moveto\n"
	" }\n"
	" {\n"
	"  5 mm 0 rlineto \n"
	"  currentpoint /Y exch LabelFontSize 3 div sub def /X exch 1 mm add def\n"
	"  stroke\n"
	"  X Y  moveto\n"
	" }ifelse\n"
	"  Label show\n"
	" grestore\n"
	" \n"
	"} bind def\n"
	,fp);

}

/*            */
/* GraphStart */
/*            */
void GraphStart(FILE *fp){
	
	fputs(
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"%\n"
	"% START OF GRAPH\n"
	"% \n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"Landscape\n"
	"TranslateToZeroPoint\n"
	"DrawGraphBoundary\n"
	"newpath 0 0.5 mm setlinewidth\n"
	,fp);
	
}

/*                  */
/* GraphTickMarks() */
/*                  */
void GraphTickMarks(FILE *fp,double cM_stt,double cM_end,double cM_smalltick,double cM_bigtick,double vu_stt,double vu_end,double vu_smalltick,double vu_bigtick){
	
	fputs(
	"%%%%%%%%%%%%%%%%%%%%\n"
	"%\n"
	"% Graph Tick marks:\n"
	"% \n"
	"%%%%%%%%%%%%%%%%%%%%\n"
	"AxisLabelFontSize SerifFont\n"
	,fp);
	fprintf(fp,"%.2f %.2f %.2f %.2f DrawHzTicks\n",cM_stt,cM_end,cM_smalltick,cM_bigtick);
	fprintf(fp,"%.2f %.2f %.2f %.2f DrawVtTicks\n",vu_stt,vu_end,vu_smalltick,vu_bigtick);
	
}


/*             */
/* GraphTitles */
/*             */
void GraphTitles(FILE *fp,char *title,char *HzAxis,char *VtAxis){
	
	fputs(
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	"%\n"
	"% Titles and Axis Labeling:\n"
	"%\n"
	"%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n"
	,fp);
	fprintf(fp,"(%s)  Title\n",title);
	fprintf(fp,"(%s) HzAxisLabel\n",HzAxis);
	fprintf(fp,"(%s) VtAxisLabel\n",VtAxis);
	
}

/*                                                              */
/* GraphRainingMarkerLabel(): Print the map info for one marker */
/*                                                              */
void GraphRainingMarkerLabel(FILE *fp,PMARKERMAP p){
	
	extern VARIABLE gGraphXAxisMin;
	
	fprintf(fp,"(%s) %.2f cM VtMarkerLabel\n",p->nm,p->ps-gGraphXAxisMin.e.v);
	
}

/*                                                       */
/* GraphMarkerLabel(): Print the map info for one marker */
/*                                                       */
void GraphMarkerLabel(FILE *fp,PMARKERMAP p){
	
	extern VARIABLE gGraphXAxisMin;
	
	fprintf(fp,"(%s) %.2f cM MarkerLabel\n",p->nm,p->ps-gGraphXAxisMin.e.v);
	
}

/*                                  */
/* GraphTraverseMarkerMapByPosition */
/*                                  */
void GraphTraverseMarkerMapByPosition(FILE *fp,PMARKERMAP p,void (*visit)(FILE *fp,PMARKERMAP p)){

	if(!p) return;
	GraphTraverseMarkerMapByPosition(fp,p->ll,visit);
	visit(fp,p);
	GraphTraverseMarkerMapByPosition(fp,p->rr,visit);
	
}


/*                   */
/* GraphMarkerLabels */
/*                   */
void GraphMarkerLabels(FILE *fp,PMARKERMAP p,int RainingStyle){

	fputs("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n",fp);
	fputs("%\n",fp);
	fprintf(fp,"%% %s Marker Labels:\n",RainingStyle?"Vertical \"Raining\"":"Angled");
	fputs("%\n",fp);
	fputs("%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%\n",fp);
	fprintf(fp,"/%s {\n",RainingStyle?"RainingMarkerLabels":"AngleMarkerLabels");
	/*                                                                */
	/* If angled style, reset the last position bookkeeping variable: */
	/*                                                                */
	if(!RainingStyle)
		fputs("LastPositionReset\n",fp);
	/*                               */
	/* Now loop through the markers: */
	/*                               */
	GraphTraverseMarkerMapByPosition(fp,p,RainingStyle?GraphRainingMarkerLabel:GraphMarkerLabel);
	
	fputs("} bind def\n",fp);
	fprintf(fp,"%s\n",RainingStyle?"RainingMarkerLabels":"AngleMarkerLabels");

}

/*                 */
/* GraphPlotData() */
/*                 */
void GraphPlotData(FILE *fp,PDBF db,int PositionFieldIndex,int ScoreFieldIndex,double cM_min,double vu_min){

	unsigned int i;
	double cM,vu;
	
	fputs(
	"%%%%%%%%%%%%%%%%%%%%\n"
	"%\n"
	"% The plot data:\n"
	"% \n"
	"%%%%%%%%%%%%%%%%%%%%\n"
	"/PlotData_01 {\n"
	"[2] 1 setdash\n"
	"0 0 moveto\n"
	,fp);
	
	/*                           */
	/* loop through all records: */
	/*                           */
	for(i=0;i<db->rc;i++){

		/*                                     */
		/* get centiMorgan position and score: */
		/*                                     */
		cM=db->ReadDouble(db,PositionFieldIndex,i);
		vu=db->ReadDouble(db,ScoreFieldIndex,i);
		
		if(cM==DOUBLEMISSINGVALUE || vu==DOUBLEMISSINGVALUE){
			continue;
		}
		
		fprintf(fp,"%.4f cM %.4f vu %s\n",
			cM-cM_min,
			vu-vu_min,
			i==0?"moveto":"lineto"
		);
		
	}
	
	fputs(
	"stroke\n"
	"} bind def\n"
	"PlotData_01\n"
	,fp);
	
	
}


/*               */
/* GraphFinish() */
/*               */
void GraphFinish(FILE *fp){

	fputs( 
	"%%%%%%%%%%%%%%%%%%%\n"
	"%\n"
	"% Finish the page:\n"
	"%\n"
	"%%%%%%%%%%%%%%%%%%%\n"
	"showpage\n"
	,fp);
	
}

/*               */
/* GraphSetColor */
/*               */
void GraphSetColor(FILE *fp,double red, double green, double blue){
	
	fprintf(fp,"%.2f %.2f %.2f setrgbcolor\n",red,green,blue);
	
}

/*              */
/* GraphSetGray */
/*              */
void GraphSetGray(FILE *fp,double gray){
	
	fprintf(fp,"%.2f setgray\n",gray);
	
}


/*                   */
/* GraphUnlabeledBar */
/*                   */
void GraphUnlabeledBar(FILE *fp,double start,char *start_units,double end,char *end_units,double vposition,char *v_units,double barheight){
	
	fprintf(fp,"%.2f %s %.2f %s %.2f %s %.2f cm Box\n",
		start,
		start_units,
		end,
		end_units,
		vposition,
		v_units,
		barheight
	);
	
}

/*                   */
/* GraphLabeledBar() */
/*                   */
void GraphLabeledBar(FILE *fp,double start,char *start_units,double end,char *end_units,double vposition,char *v_units,double barheight,int AboveBelow,char *bartitle){

	fprintf(fp,"%.2f %s %.2f %s %.2f %s %.2f cm %s (%s) LabeledBar\n",
		start,
		start_units,
		end,
		end_units,
		vposition,
		v_units,
		barheight,
		AboveBelow?"ABOVE":"BELOW",
		bartitle
	);
	
}

/*                      */
/* GraphPlainAnnotation */
/*                      */
void GraphPlainAnnotation(FILE *fp,double hposition,char *h_units,double vposition,char *v_units,char *annotation){
	
	fprintf(fp,"%.2f %s %.2f %s (%s) Annotation\n",
		hposition,
		h_units,
		vposition,
		v_units,
		annotation
	);
	
}

/*             */
/* GraphRegion */
/*             */
void GraphRegion(FILE *fp,double hstart,double hend,char *h_units,double vstart,double vend,char *v_units){
	
	fprintf(fp,"%.2f %s %.2f %s %.2f %s %.2f %s Region\n",
		hstart,
		h_units,
		hend,
		h_units,
		vstart,
		v_units,
		vend,
		v_units
	);
	
}

/*                      */
/* GraphArrowAnnotation */
/*                      */
void GraphArrowAnnotation(FILE *fp,double hposition,char *h_units,double vposition,char *v_units,double arrowangle,char *annotation){
	
	fprintf(fp,"%.2f %s %.2f %s %.2f (%s) ArrowAnnotation\n",
		hposition,
		h_units,
		vposition,
		v_units,
		arrowangle,
		annotation
	);
	
}

