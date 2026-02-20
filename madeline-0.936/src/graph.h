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
/* "graph.h" is part of the Madeline software distribution.                          */
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
/* "graph.h" was created on 2002.07.29            */
/* by ET in order to add LOD plotting             */
/* functionality (initially using Simwalk         */
/* results) to Madeline                           */
/*                                                */
/* Last modified: 2002.07.29 by ET                */
/*                                                */


#ifndef CONTROL_INCLUDED
#include "control.h"
#endif

#ifndef TOKENTYPES_INCLUDED
#include "tokentypes.h"
#endif

#ifndef PROCESS_INCLUDED
#include "processm.h"
#endif

#ifndef CONFIG_INCLUDED
#include "config.h"
#endif

/*                      */
/* Marker Label Styles: */
/*                      */
#define RAINING_MARKER_LABELS 1
#define ANGLED_MARKER_LABELS  0

#define BAR_LABEL_ABOVE_STYLE 1
#define BAR_LABEL_BELOW_STYLE 0

/*                                 */
/* Default Graph bar height        */
/*                                 */
#define GRAPH_BAR_HEIGHT 0.5 /* cm */

/*                                  */
/* Units of Measure for annotations */
/*                                  */
#define CENTIMORGAN_UNITS  "cM"
#define CENTIMETER_UNITS   "cm"
#define VERTICAL_UNITS     "vu"

/*                                 */
/* Number of different multipliers */
/* by which the TickBasis may be   */
/* multiplied:                     */
#define MULTIPLIER_COUNT 4

/*                                       */
/* Number of intervals along an axis     */
/* counting between major ticks          */
/* -- used to determine the major        */
/* tick increment.  Because the X axis   */
/* is longer, we can have more intervals */
/* relative to the Y axis:               */
/*                                       */
#define Y_AXIS_MINIMUM_INTERVALS  3
#define Y_AXIS_MAXIMUM_INTERVALS  7
#define X_AXIS_MINIMUM_INTERVALS  5
#define X_AXIS_MAXIMUM_INTERVALS 15


void Graph(void);
void GraphOpen(const char *s);
void GraphPlot(PCONTROL g);
void GraphPostscriptProlog(FILE *fp,double cM_stt,double cM_end,double vu_stt,double vu_end);
void GraphStart(FILE *fp);
void GraphTickMarks(FILE *fp,double cM_stt,double cM_end,double cM_smalltick,double cM_bigtick,double vu_stt,double vu_end,double vu_smalltick,double vu_bigtick);
void GraphTitles(FILE *fp,char *title,char *HzAxis,char *VtAxis);
void GraphCalculateMinimaMaxima(PDBF db,UINT16 cM_idx,UINT16 vu_idx,double *cM_min,double *cM_max,double *vu_min,double *vu_max);
void GraphPlotData(FILE *fp,PDBF db,int PositionFieldIndex,int ScoreFieldIndex,double cM_min,double vu_min);
void GraphFinish(FILE *fp);
void GraphRainingMarkerLabel(FILE *fp,PMARKERMAP p);
void GraphMarkerLabel(FILE *fp,PMARKERMAP p);
void GraphTraverseMarkerMapByPosition(FILE *fp,PMARKERMAP p,void (*visit)(FILE *fp,PMARKERMAP p));
void GraphMarkerLabels(FILE *fp,PMARKERMAP p,int RainingStyle);
void GraphSetColor(FILE *fp,double red, double green, double blue);
void GraphSetGray(FILE *fp,double gray);
void GraphUnlabeledBar(FILE *fp,double start,char *start_units,double end,char *end_units,double vposition,char *v_units,double barheight);
void GraphLabeledBar(FILE *fp,double start,char *start_units,double end,char *end_units,double vposition,char *v_units,double barheight,int AboveBelow,char *bartitle);
void GraphPlainAnnotation(FILE *fp,double hposition,char *h_units,double vposition,char *v_units,char *annotation);
void GraphRegion(FILE *fp,double hstart,double hend,char *h_units,double vstart,double vend,char *v_units);
void GraphArrowAnnotation(FILE *fp,double hposition,char *h_units,double vposition,char *v_units,double arrowangle,char *annotation);
void GraphCalculateEndpointsAndIntervals(double low,double high,int MinimumIntervals,int MaximumIntervals,double *stt,double *end,double *MajorTick,double *MinorTick);
double GraphRoundDown(double n,double d);
double GraphRoundUp(double n,double d);
void GraphResetSequentialIDs(PCONTROL g);
void GraphPrintAssociativeArrayStringValue(void *KeyValuePair);
void GraphStartAnnotations(FILE *fp,double GraphXMin,double GraphYMin);
void GraphEndAnnotations(FILE *fp);

#define GRAPH_INCLUDED
