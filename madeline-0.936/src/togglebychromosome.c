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
/* Last modified by Ed Trager, February 2003.                                        */
/*                                                                                   */
/* ********************************************************************************* */

/*                                                          */
/* togglebychromosome.c                                     */
/*                                                          */
/* init: 2003.02.24 ET                                      */
/*                                                          */
/* -> Functions to support an addition to the               */
/*    TOGGLE command:                                       */
/*                                                          */
/*    TOGGLE ON|OFF OUTPUT FLAGS FOR CHROMOSOME <n> MARKERS */
/*                                                          */

#include "togglebychromosome.h"
#include "control.h"
#include "fieldflags.h"

/*                                    */
/* ToggleMarkersByChromosome()        */
/*                                    */
/* If force_on is true, then          */
/* the output flag is turned on       */
/* for all markers on the designated  */
/* chromosome.  Otherwise, the        */
/* markers are turned off             */
/*                                    */
void ToggleMarkersByChromosome(int force_on,int chromosome){
	
	extern PCONTROL g;
	
	if(force_on) TraverseMarkerMapForChromosome(g->mp,chromosome,ToggleMarkerOn);
	else         TraverseMarkerMapForChromosome(g->mp,chromosome,ToggleMarkerOff);
	
}

void ToggleMendelianInconsistentMarkersByChromosome(int force_on,int chromosome){
	
	extern PCONTROL g;
	
	if(force_on) TraverseMarkerMapForChromosome(g->mp,chromosome,ToggleMendelianInconsistentMarkerOn);
	else         TraverseMarkerMapForChromosome(g->mp,chromosome,ToggleMendelianInconsistentMarkerOff);
	
}

/*                                                     */
/* Toggle inconsistent markers on or off as requested: */
/*                                                     */
void ToggleMendelianInconsistentMarkers(int force_on){
	
	extern PCONTROL g;
	int i,ii;
	
	for(i=0;i<g->db->n.g;i++){
		
		/* get the actual genotype field index: */
		ii=g->db->fi->g[i];
		
		if(g->db->m[ii].nmin){
			if(force_on){
				/* If off, turn on: */
				if(!(g->db->ff[ii]&OUTPUTSET)) g->db->ff[ii]^=OUTPUTSET;
			}else{
				/* If on, turn off: */
				if(g->db->ff[i]&OUTPUTSET) g->db->ff[i]^=OUTPUTSET;
			}
		}
	}
}

/*                 */
/* ToggleMarkerOn  */
/*                 */
void ToggleMarkerOn(PMARKERMAP p){
	
	extern PCONTROL g;
	register int i;
	
	/* Get the field index of this marker: */
	i=g->db->FieldNumber(g->db,p->nm);
	/* If marker is not on, then toggle it on: */
	if(!(g->db->ff[i]&OUTPUTSET)) g->db->ff[i]^=OUTPUTSET;
	
}

/*                 */
/* ToggleMarkerOff */
/*                 */
void ToggleMarkerOff(PMARKERMAP p){
	
	extern PCONTROL g;
	register int i;
	
	/* Get the field index of this marker: */
	i=g->db->FieldNumber(g->db,p->nm);
	/* If marker is on, then toggle it off: */
	if(g->db->ff[i]&OUTPUTSET) g->db->ff[i]^=OUTPUTSET;
	
}

/*                                      */
/* ToggleMendelianInconsistentMarkerOn  */
/*                                      */
void ToggleMendelianInconsistentMarkerOn(PMARKERMAP p){
	
	extern PCONTROL g;
	register int i;
	
	/* Get the field index of this marker: */
	i=g->db->FieldNumber(g->db,p->nm);
	/* If the marker is inconsistent and is not on, */
	/* then toggle it on:                           */
	if(g->db->m[i].nmin && !(g->db->ff[i]&OUTPUTSET)) g->db->ff[i]^=OUTPUTSET;
	
}

/*                                      */
/* ToggleMendelianInconsistentMarkerOff */
/*                                      */
void ToggleMendelianInconsistentMarkerOff(PMARKERMAP p){
	
	extern PCONTROL g;
	register int i;
	
	/* Get the field index of this marker: */
	i=g->db->FieldNumber(g->db,p->nm);
	/* If marker is inconsistent and is on, */
	/*  then toggle it off:                 */
	if(g->db->m[i].nmin && g->db->ff[i]&OUTPUTSET) g->db->ff[i]^=OUTPUTSET;
	
}
