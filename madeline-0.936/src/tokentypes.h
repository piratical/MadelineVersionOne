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
/* "tokentypes.h" is part of the Madeline software distribution.                     */
/* Written by Edward H. Trager.                                                      */
/*                                                                                   */
/* COPYRIGHT (C) 2002 THE REGENTS OF THE UNIVERSITY OF MICHIGAN,                     */
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
/* Last modified by Ed Trager, April, 2002.                                          */
/*                                                                                   */
/* ********************************************************************************* */

/*                                                      */
/*                                                      */
/* tokentypes.h                                         */
/*                                                      */
/* history:                                             */
/*                                                      */
/* 2002.04.18.ET: Segregated out of the old "shared.h"  */

/*                                                                                                    */
/* Token types (pT->t) (also used for VARIABLE and ARRAY types):                                      */
/*                                                                                                    */
#define NUMERICVALUE             1 /* a numeric value                                                 */
#define NUMERICVARIABLE          2 /* a numeric variable (field) in a dbf                             */
#define OPERATOR                 3 /* an operator                                                     */
#define STRINGVALUE              4 /* a string literal                                                */
#define STRINGVARIABLE           5 /* a character variable (field) in a dbf                           */
#define INTERNALVARIABLE         6 /* <<<=== internal variable of type VARIABLE                       */
#define FIELDNAMEVALUE           7 /* same as STRINGVALUE, but limited to LNFIELDNAME in length       */
#define COMMAND                  8 /* a command                                                       */
#define NFUNCTION                9 /* function taking & returning double: double foo(double)          */
#define SFUNCTION               10 /* function taking & returning char *: char *foo(char *)           */
#define CFUNCTION               11 /* function taking char * returning int: int foo(char *)           */
#define AFUNCTION               12 /* aggregate function                                              */
#define ICCFUNCTION             13 /* function of type "int foo(char *,char *)                        */
#define INTERNALARRAY           14 /* <<<=== internal ARRAY type                                      */
#define XFUNCTION               15 /* function of type "void foo(char *,char *,int,int)               */
#define KEYWORD                 16 /* a keyword that is used with a command in the interpreter        */
#define INDIVIDUALVARIABLE      17 /* a NUMERIC member variable of an INDIVIDUAL structure            */
#define INDIVIDUALARRAY         18 /* a member array of an INDIVIDUAL structure                       */
#define INDIVIDUALSTRING        19 /* a STRING member variable of an INDIVIDUAL structure             */
#define INDIVIDUALPOINTER       20 /* a pointer to an INDIVIDUAL (i.e., one's father or mother)       */
#define DATEVALUE               21 /* a Julian day value                                              */
#define DATEVARIABLE            22 /* a variable containing values interpreted as Julian days         */
/*                                                                                                    */
/* USERVARIABLE and USERARRAY are very much like INTERNALVARIABLE and INTERNALARRAY                   */
/* except that they are allocated (and hence require deallocation) from memory:                       */
/*                                                                                                    */
#define USERVARIABLE            23 /* <<<=== a variable of type VARIABLE created as a result of user actions */
#define USERARRAY               24 /* <<<=== an array of type ARRAY created as a result of user actions      */
#define ASSOCIATIVEARRAY        25 /* <<<=== an associative array: BST of key-value VARIABLE pairs           */
#define USERASSOCIATIVEARRAY    26 /* <<<=== an associative array created as a result of user actions        */
#define PROGCTRLOPERATOR        27 /* Program control: FOR, IF, WHILE, }                              */
#define TDRNFUNCTION            28 /* function taking a date & returning double                       */
#define TNRDFUNCTION            29 /* function taking a double and returning a date                   */
#define TVRDFUNCTION            30 /* function taking void and returning a date                       */
#define TOKENTYPES              31 /* Currently not used */


#define TOKENTYPES_INCLUDED

