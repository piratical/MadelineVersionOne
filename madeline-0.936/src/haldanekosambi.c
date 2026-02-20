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
/* "haldanekosambi.c" is part of the Madeline software distribution.                 */
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
/* General Public License for more details.                                          */
/*                                                                                   */
/* You should have received a copy of the GNU General Public                         */
/* License along with this software source code distribution; if not,                */
/* write to the Free Software Foundation, Inc., 59 Temple Place -                    */
/* Suite 330, Boston, MA 02111-1307, USA.                                            */
/*                                                                                   */
/* Last modified by Ed Trager, January, 2003.                                        */
/*                                                                                   */
/* ********************************************************************************* */

/*                                                                                   */
/*                                                                                   */
/* haldanekosambi.c                                                                  */
/*                                                                                   */
/* history:                                                                          */
/*                                                                                   */
/* 2003.01.29 initiated by ET                                                        */
/* 2003.02.03 added haldane conversion functions.                                    */
/*                                                                                   */
/* This file provides functions to convert between Kosambi and Haldane centiMorgans  */
/* and recombination fractions (theta).                                              */
/* A p-value-to LOD score function is also provided.                                 */
/*                                                                                   */

#include "haldanekosambi.h"
#include <math.h>

double Kosambi_cM_To_Theta(double m){
	
	register double four_m;
	register double n;
	
	/*                                                                      */
	/* Remember that m is in centiMorgans, so we need to convert to Morgans */
	/* and then multiply by 4:                                              */
	four_m=0.04*m;
	n=exp(four_m);
	
	return 0.5*(n - 1.0)/(n + 1.0);
	
}

double Theta_To_Kosambi_cM(double r){
	
	register double two_r;
	
	/*                                                                    */
	/* Here again, we want to multiply by 100 to get centiMorgans instead */
	/* of Morgans:                                                        */
	/*                                                                    */
	two_r=2.0*r;
	return 25.0*log((1+two_r)/(1-two_r));
	
}

double lod(double m){
	
	return -log10(m);
	
}

double Haldane_cM_To_Theta(double m){
	
	/*                                                                      */
	/* Remember that m is in centiMorgans, so we need to convert to Morgans */
	/* and then multiply by negative 2:                                     */
	
	return 0.5*(1.0-exp(-0.02*m));
	
}

double Theta_To_Haldane_cM(double r){
	
	/*                                                                    */
	/* Here again, we want to multiply by 100 to get centiMorgans instead */
	/* of Morgans:                                                        */
	/*                                                                    */
	return -50.0*log(1.0-2.0*r);
	
}


