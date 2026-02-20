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
/* "knowntokens.h" is part of the Madeline software distribution.                    */
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
/* Last modified by Ed Trager, August, 2002.                                         */
/*                                                                                   */
/* ********************************************************************************* */

/*                                                                       */
/*                                                                       */
/* knowntokens.h                                                         */
/*                                                                       */
/* history:                                                              */
/* -- Pulled out of madeline.h on 2002.08.22 in order to make the code   */
/*    more manageable. -- ET                                             */
/*                                                                       */

/*                                                                       */
/* internal representations (pT->i) for tokens that are longer than      */
/* one character in external representation:                             */
/*                                                                       */

#define NOINTERNALTOKEN   0 /* used when there is no internal token      */
#define END             ';' /* end of expression                         */
#define LE                1 /* less than or equals:    <=                */
#define GE                2 /* greater than or equals: >=                */
#define AND               3 /* AND                                       */
#define OR                4 /* OR                                        */
#define NE                5 /* not equals:             !=                */
#define MI                6 /* MISSING                                   */
#define PIV               7 /* value of PI 3.1415926                     */
#define EBASE             8 /* base e of natural logarithms              */
#define CMV               9 /* character missing value internal token    */
#define NMV              10 /* numeric missing value internal token      */
#define CSV              12 /* character sex value array internal token  */
#define NSV              13 /* numeric sex value array internal token    */
#define FEM              14 /* female internal variable token            */
#define MAL              15 /* male internal variable token              */
#define IIDFD            16 /* individual id field name                  */
#define FIDFD            17 /* father id field name                      */
#define MIDFD            18 /* mother id field name                      */
#define SEXFD            19 /* sex field name                            */
#define MZTFD            20 /* MZ twin field name                        */
#define FAMFD            21 /* Family ID field name                      */
#define PBDFD            22 /* Index case (proband) indicator field name */
#define AFFFD            23 /* Affected indicator field name             */
#define CLSFD            24 /* FUSION study relationship class field     */
#define LCLFD            25 /* Liability class field                     */
#define PLUS_ASSIGN      26 /* += assignment                             */
#define MINUS_ASSIGN     27 /* -= assignment                             */
#define INCREMENT_ASSIGN 28 /* ++ increment operator                     */
#define DECREMENT_ASSIGN 29 /* -- decrement operator                     */
#define MULTIPLY_ASSIGN  30 /* *= assignment operator                    */
#define DIVIDE_ASSIGN    31 /* /= assignment operator                    */
/*                                                         */
/* NOTE: Because the printable ASCII characters occupy     */
/*       the decimal numeric range from 32-127, Madeline   */
/*       assigns their numeric value directly as the       */
/*       token's internal value as needed.  128 and 129    */
/*       are skipped for no particular known reason, other */
/*       than that it aesthetically more pleasing to       */
/*       start again at 130.  Some values are skipped due  */
/*       to the removal of obsolete commands or keywords   */
/*                                                         */
/*       The following #defines were added incre-          */
/*       mentally as the program was developed over time,  */
/*       so they are not arranged alphabetically or in     */
/*       any other logical order.  New #defines should     */
/*       be simply added to the very end of the list.      */
/*                                                         */
#define LOG   130 /* natural LOG function                                              */
#define INV   131 /* inverse: 1/x function                                             */
#define SQRT  132 /* square root                                                       */
#define LOG10 133 /* log base 10                                                       */
#define EXP   134 /* e raised to a power                                               */
#define SIN   135 /* SIN function                                                      */
#define COS   136 /* COS function                                                      */
#define TAN   137 /* TAN function                                                      */
#define ASIN  138 /* arc sine                                                          */
#define ACOS  139 /* arc cosine                                                        */
#define ATAN  140 /* arc tangent                                                       */
#define FLOOR 141 /* floor                                                             */
#define CEIL  142 /* ceiling                                                           */
#define SINH  143 /* hyperbolic sin                                                    */
#define COSH  144 /* hyperbolic cosine                                                 */
#define TANH  145 /* hyperbolic tangent                                                */
#define SBS   146 /* SubString function defined in parser.c                            */
#define SHOWC 147 /* Show()       command function                                     */
#define ASSNC 148 /* Assignment() command function                                     */
#define QUITC 149 /* Quit()       command function                                     */
#define HELOC 150 /* Hello()      command function                                     */
#define WHATC 151 /* WhatIs()     command function: alternate to the "SHOW " command   */
#define ISC   152 /* required word after "WHAT" to complete the "WHAT IS" command      */
#define BANC  153 /* Banner()     command function                                     */
#define OPENC 154 /* Open()       command function to open a database                  */
#define GOC   155 /* Go()         command function to move to a record in the database */
#define LISTC 156 /* List()       command function                                     */
#define FLDK  157 /* "FIELDS" or "FIELD" keyword used with the List() command          */
#define WRITC 158 /* Write()      command function                                     */
#define INC   159 /* "IN"         keyword (Should be "INK", but this is a legacy issue)*/
#define TOC   160 /* "TO"         keyword (Should be "TOK", but this is a legacy issue)*/
#define FORMC 161 /* "FORMAT"     keyword (Should be "FORMK", but this is a legacy ...)*/
#define MENDL 162 /* "MENDEL"     format keyword                                       */
#define UM13  163 /* "USERM13"    format keyword                                       */
#define FISH0 164 /* "FISHER0"    format keyword                                       */
#define FISH1 165 /* "FISHER1"    format keyword                                       */
#define SIBLK 166 /* "SIBLINK"    format keyword                                       */
#define COVAK 167 /* "COVARIATE"  field flag keyword                                   */
#define GENEK 168 /* "GENOTYPE"   field flag keyword                                   */
#define OUTPK 169 /* "OUTPUT"     field flag keyword                                   */
#define FLAGK 170 /* "FLAG"       keyword in Toggle command                            */
#define TOGLC 171 /* "TOGGLE"     command                                              */
#define FORK  172 /* "FOR"        keyword                                              */
#define HELPC 173 /* "HELP"       command                                              */
#define SYSC  174 /* "SYSTEM"     command                                              */
#define DRAWC 175 /* "DRAW"       command                                              */
#define PEDIK 176 /* "PEDIGREE"   keyword used in Draw() command                       */
#define CRIMK 177 /* "CRIMAP"     keyword                                              */
#define FSPK  178 /* "FSP"        keyword                                              */
#define PHENK 179 /* "PHENOTYPE"  keyword                                              */
#define SIBP1 180 /* "SIBPAL1"    keyword                                              */
#define SIBP2 181 /* "SIBPAL2"    keyword                                              */
#define SIBP3 182 /* "SIBPAL3"    keyword                                              */
#define SIBP4 183 /* "SIBPAL4"    keyword                                              */
#define TRAIK 184 /* "TRAIT"      keyword                                              */
#define WITHK 185 /* "WITH"       keyword                                              */
#define DBFFV 186 /* "DatabaseFile"        internal variable                           */
#define DETFV 187 /* "DetailFile"          internal variable                           */
#define LOGFV 188 /* "LogFile"             internal variable                           */
#define OUTFV 189 /* "OutputFile"          internal variable                           */
#define PARFV 190 /* "ParameterOutputFile" internal variable                           */
#define QUIEC 191 /* QUIET command                                                     */
#define SILEC 192 /* SILENCE or SILENT command                                         */
#define VERBC 193 /* VERBOSE command                                                   */
#define GHK   194 /* GENEHUNTER keyword                                                */
#define RELAK 195 /* RELATE     keyword                                                */
#define FLOAK 198 /* FLOATERS   keyword                                                */
#define TURNC 199 /* TURN       command                                                */
#define ONK   200 /* ON         keyword                                                */
#define OFFK  201 /* OFF        keyword                                                */
#define AUTOK 202 /* AUTODETECT keyword                                                */
#define SFIS0 204 /* SOLARFISHER0 file format keyword                                  */
#define SFIS1 205 /* SOLARFISHER1 file format keyword                                  */
#define AXCLK 206 /* AUTOEXCLUDE keyword                                               */
#define GENER 207 /* GENERIC     keyword                                               */
#define FILEK 208 /* FILE or FILES keyword                                             */
#define LOCUS 209 /* LOCUS       keyword                                               */
#define SAGEK 210 /* SAGE        keyword                                               */
#define CAS   211 /* CharacterAffectionStatus[] internal array                         */
#define NAS   212 /* NumericAffectionStatus[]   internal array                         */
#define MAPDB 213 /*                                                                   */
#define MFCHR 214 /* map database ChromosomeField internal variable                    */
#define MFPOS 215 /* map database PositionField   internal variable                    */
#define MFORD 216 /* map database OrdinalField    internal variable                    */
#define MFMRK 217 /* map database MarkerField     internal variable                    */
#define LOADC 218 /* LOAD command for loading a map                                    */
#define MAPK  219 /* MAP keyword                                                       */
#define CHRMK 220 /* CHROMOSOME keyword                                                */
#define CDS   221 /* CharacterDeathStatus[] internal array                             */
#define NDS   222 /* NumericDeathStatus[] internal array                               */
#define DSFD  223 /* DeathStatusField internal variable                                */
#define CDLMK 224 /* Generic comma-delimited output: CommaDelimited                    */
#define SDLMK 225 /* Generic space-delimited output: SpaceDelimited                    */
#define EXCLC 226 /* Exclude command                                                   */
#define UNEXC 227 /* Unexclude command                                                 */
#define RECGC 228 /* Recognize command                                                 */
#define UFQ0K 229 /* UserFQTL keyword (for no ascertainment)                           */
#define UFQ1K 230 /* UserFQTL keyword (for ascertainment)                              */
#define UFQTK 231 /* UserFQTL keyword (for locus files)                                */
#define UFNAK 232 /* UserFQTL ALL NUCLEAR blocks keyword    -- UserFQTLAll             */
#define UFNOK 233 /* UserFQTL OFFSPRING NUCLEAR blocks only -- UserFQTLOffspring       */
#define UFNFK 234 /* UserFQTLFounders keyword                                          */
#define SVAFK 235 /* SaveAlleleFrequencies keyword                                     */
#define NV_AL 236 /* "ALIVE"                                                           */
#define NV_DE 237 /* "DEAD"                                                            */
#define NV_AF 238 /* "AFFECTED"                                                        */
#define NV_UN 239 /* "UNAFFECTED"                                                      */
#define NV_TR 240 /* "TRUE"                                                            */
#define NV_FA 241 /* "FALSE"                                                           */
#define RUNC  242 /* RUN command                                                       */
#define VIEWC 243 /* VIEW command                                                      */
#define CLRC  244 /* CLEAR command                                                     */
#define EXCLK 245 /* EXCLUSIONS keyword                                                */
#define FAMK  246 /* FAMILY or FAMILIES keyword                                        */
#define PRNCV 247 /* PrintCommand internal variable                                    */
#define AFSYK 248 /* Affection status symbol array                                     */
#define AFSZK 249 /* affection status symbol size (in pts) variable                    */
#define CHEKC 250 /* CHECK command                                                     */
#define INHRK 251 /* INHERITANCE keyword                                               */
#define SIBIBD 252 /* ASPEX sib_ibd   program                                          */
#define SIBPHS 253 /* ASPEX sib_phase program                                          */
#define SIBMAP 254 /* ASPEX sib_map   program                                          */
#define SIBTDT 255 /* ASPEX sib_tdt   program                                          */
#define KINSHP 256 /* ASPEX kinship   program                                          */
#define LINKAG 257 /* LINKAGE  program format -- can be used for Aspex too.            */
#define USINGK 258 /* USING keyword                                                    */
#define SKIPK  259 /* SKIPPING keyword                                                 */
#define LINEK  260 /* LINE and LINES keyword                                           */
#define RELPRK 261 /* RELPAIR keyword                                                  */
#define AUTOMK 262 /* AUTOMATICALLY keyword                                            */
#define ORDERK 263 /* ORDER keyword                                                    */
#define RECORK 264 /* RECORD keyword                                                   */
#define IK     265 /* offspring k[] vector of an INDIVIDUAL                            */
#define IA     266 /* mate a[] vector of an INDIVIDUAL                                 */
#define INK    267 /* number of offspring of an INDIVIDUAL                             */
#define INA    268 /* number of mates of an INDIVIDUAL                                 */
#define IID    269 /* individual id                                                    */
#define IFID   270 /* family id                                                        */
#define IDAD   271 /* reference to individual's father                                 */
#define IMOM   272 /* reference to individual's mother                                 */
#define AMEAN  273 /* aggregate _oMean() function                                      */
#define ASUM   274 /* aggregate _oSum() function                                       */
#define ACOUNT 275 /* aggregate _oCount() function                                     */
#define AVAR   276 /* aggregate _oVariance() function                                  */
#define ASTDV  277 /* aggregate _oStdDev() function                                    */
#define ACTRUE 278 /* aggregate _oCountTrue() function                                 */
#define ACFALS 279 /* aggregate _oCountFalse() function                                */
#define ACMISS 280 /* aggregate _oCountMissing() function                              */
#define FABS   281 /* absolute value, fabs() function                                  */
#define INFF   282 /* number of founding fathers                                       */
#define INFM   283 /* number of founding mothers                                       */
#define INIP   284 /* total number in pedigree                                         */
#define LBSZ   285 /* label font size                                                  */
#define LGSZ   286 /* legend font size                                                 */
#define LAMER  288 /* American_English language                                        */
#define LSUOM  289 /* Finnish                                                          */
#define LFREN  290 /* French                                                           */
#define LANGK  291 /* "LANGUAGE" keyword                                               */
#define ROUND  292 /* Round() round up/down to next whole number function              */
#define DOBFV  293 /* DateOfBirthField name                                            */
#define DODFV  294 /* DateOfDeathField name                                            */
#define SORTC  295 /* Sort() command                                                   */
#define SASCK  296 /* ASCENDING  keyword                                               */
#define SDESK  297 /* DESCENDING keyword                                               */
#define PRNDV  298 /* PrintDevice internal variable reference                          */
#define DZTFD  299 /* Dizygotic twin indicator field, gfnDZTwin                        */
#define FSP0K  300 /* FSP format when only core fields are required.                   */
#define STATK  301 /* STATS or STATISTICS keyword                                      */
#define GHNPK  302 /* GENEHUNTERNPL (Genehunter non-parametric keyword)                */
#define ERRFV  303 /* ErrorFile internal variable                                      */
#define INCSP  304 /* reference individual's childless spouse state variable           */
#define INEXC  305 /* reference individual's excluded state variable                   */
#define INHDT  306 /* reference individual's has data state variable                   */
#define INFLT  307 /* reference individual's unattached state variable                 */
#define INDBF  308 /* reference individual's p->db cast to a 0-1 state variable        */
#define NV_EI  309 /* EvaluationInterval variable                                      */
#define NV_OE  310 /* OffEndDistance     variable                                      */
#define INISP  311 /* reference individual's p->proband state variable                 */
#define CSIBL  312 /* current Siblink format                                           */
#define SIBLKU 313 /* Siblink unaffected pairs                                         */
#define SIBLKA 314 /* Siblink all pairs                                                */
#define SIBLKD 315 /* Siblink discordant pairs                                         */
#define GHQTL  316 /* Genehunter Quantative Traits Linkage Analysis format keyword     */
#define TDLMK  317 /* Generic tab-delimited output: TabDelimited                       */
#define CONVC  318 /* CONVERT   command                                                */
#define RECTC  319 /* RECTIFY   command                                                */
#define CMMAK  320 /* COMMA     keyword                                                */
#define TABAK  321 /* TAB       keyword                                                */
#define DLMAK  322 /* DELIMITED keyword                                                */
#define ICONK  323 /* ICON keyword used with TOGGLE command                            */
#define UNIQK  324 /* UNIQUE and DISTINCT keywords (equivalent)                        */
#define VALUK  325 /* VALUES keyword                                                   */
#define OF__K  326 /* OF keyword                                                       */
#define EDITC  327 /* edit command                                                     */
#define EDITV  328 /* FileEditor internal variable                                     */
#define COMPC  329 /* Compose command  (formerly Transpose() )                         */
#define MERGC  330 /* merge command                                                    */
#define AL1FD  331 /* Allele1 field internal variable                                  */
#define AL2FD  332 /* Allele2 field internal variable                                  */
#define DBUGC  333 /* DEBUG command -interactive wrapper for a debug routine           */
#define ORIEK  334 /* ORIENTATION keyword                                              */
#define LANDK  335 /* LANDSCAPE   keyword                                              */
#define PORTK  336 /* PORTRAIT    keyword                                              */
#define AUTPK  337 /* AUTOMATIC   keyword                                              */
#define MLTPK  338 /* MULTIPAGE   keyword                                              */
#define COLRK  339 /* COLOR       keyword                                              */
#define PSVNM  340 /* PostScriptViewer internal variable                               */
#define PEDCK  341 /* PEDCHECK format keyword                                          */
#define LDEQK  342 /* LDEQH    format keyword                                          */
#define LDE2K  343 /* LDEQ2    format keyword                                          */
#define LDE3K  344 /* LDEQ3    format keyword                                          */
#define DIVDK  345 /* DividedDrawings keyword                                          */
#define SZLTK  346 /* US LETTER paper size keyword                                     */
#define SZLGK  347 /* US LEGAL paper size keyword                                      */
#define SZA4K  348 /* A4 paper size keyword                                            */
#define SZALK  349 /* A4 long paper size keyword                                       */
#define SZASK  350 /* A4 Super paper size keyword                                      */
#define SZPAK  351 /* PaperSize keyword                                                */
#define PMRGK  352 /* PaperMargin keyword                                              */
#define IPSPS  353 /* pointer to an INDIVIDUAL's first spouse                          */
#define IPK01  354 /* pointer to an INDIVIDUAL's  1st child                            */
#define IPK02  355 /* pointer to an INDIVIDUAL's  2nd child                            */
#define IPK03  356 /* pointer to an INDIVIDUAL's  3rd child                            */
#define IPK04  357 /* pointer to an INDIVIDUAL's  4th child                            */
#define IPK05  358 /* pointer to an INDIVIDUAL's  5th child                            */
#define IPK06  359 /* pointer to an INDIVIDUAL's  6th child                            */
#define IPK07  360 /* pointer to an INDIVIDUAL's  7th child                            */
#define IPK08  361 /* pointer to an INDIVIDUAL's  8th child                            */
#define IPK09  362 /* pointer to an INDIVIDUAL's  9th child                            */
#define IPK10  363 /* pointer to an INDIVIDUAL's 10th child                            */
#define NV_UU  364 /* UnstudiedUnaffected array indexing constant                      */
#define NV_UA  365 /* UnstudiedAffected array indexing constant                        */
#define NV_UC  366 /* UnstudiedConflicting array indexing constant                     */
#define TEXTK  367 /* TEXT keyword                                                     */
#define HTMLK  368 /* HTML keyword                                                     */
#define WEBVK  369 /* WebViewerName                                                    */
#define WEBAK  370 /* WebAddress                                                       */
#define ININC  371 /* _included individual variable reference                          */
#define HAPLK  371 /* Haplotype keyword                                                */
#define ALPHAK 372 /* ALPHA order                                                      */
#define PHYSIK 373 /* PHYSICAL order                                                   */
#define FUSNK 374 /* FUSIONSUPPORT keyword                                             */
#define LISCC 375 /* License() command to print Madeline license                       */
#define LBRIT 376 /* BRITISH_ENGLISH language conventions                              */
#define LOOKC 377 /* Lookup Command to replace HELP (TEXT VERSION)                     */
/*                                                                                     */
/* 20000923 ET ADDENDA: A-Z Scratch variables (E is somewhere else now that the base   */
/* of the natural logarithms is "#e" instead of "e")                                   */
/*                                                                                     */
#define SVR_A 378
#define SVR_B 379
#define SVR_C 380
#define SVR_D 381
#define SVR_F 382
#define SVR_G 383
#define SVR_H 384
#define SVR_I 385
#define SVR_J 386
#define SVR_K 387
#define SVR_L 388
#define SVR_M 389
#define SVR_N 390
#define SVR_O 391
#define SVR_P 392
#define SVR_Q 393
#define SVR_R 394
#define SVR_S 395
#define SVR_T 396
#define SVR_U 397
#define SVR_V 398
#define SVR_W 399
#define SVR_X 400
#define SVR_Y 401
#define SVR_Z 402
/* 2001.10.30 ET ADDENDUM                                                              */
#define PROMP 403 /* PROMPT (?)                                                        */
#define LBLVI 404 /* 2001.12.20.ET ADDENDUM: LabelCreatedVirtualIndividuals            */
#define HIGHL 405 /* 2001.12.20.ET ADDENDUM: Highlight alternate groups of             */
                  /*                         five rows of data on drawings.            */
#define INGTC 406 /* GenotypeCount reference                                           */
#define INGTP 407 /* PercentGenotyped reference                                        */
#define AMIN  408 /* aggregate _OffspringMinimum() function                            */
#define AMAX  409 /* aggregate _OffspringMaximum() function                            */
/* 2002.03.26                                                                          */
#define IPMGM 410 /* Pointer to maternal grandmother                                   */
#define IPMGF 411 /* Pointer to maternal grandfather                                   */
#define IPPGM 412 /* Pointer to paternal grandmother                                   */
#define IPPGF 413 /* Pointer to paternal grandfather                                   */
#define IRCI  414 /* Individual's database record no.                                  */
/* 2002.04.22  ADDENDA:                                                                */
#define AS_KY 415 /* Keyword "AS"                                                      */
#define AAAS  416 /* AffectionStatus Associative Array                                 */
#define INFEM 417 /* _IsFemale                                                         */
#define INAFF 418 /* _IsAffected                                                       */
#define INDED 419 /* _IsDeceased                                                       */
#define INCSG 420 /* _IsConsanguinous                                                  */
#define INMZT 421 /* _IsMonozygoticTwin                                                */
#define INDZT 422 /* _IsDizygoticTwin                                                  */
#define ISELF 423 /* _Self pointer to the individual himself                           */
#define REM   424 /* RegExpMatch() function                                            */
#define DATAK 425 /* DATA keyword                                                      */
#define DLMBK 426 /* DELIMITER keyword                                                 */
#define SPCAK 427 /* SPACE keyword                                                     */
#define XOR   428 /* XOR keyword                                                       */
#define AADS  429 /* DeathStatus Associative Array                                     */
#define AAGS  430 /* GenderStatus Associative Array                                    */
#define AAPS  431 /* ProbandStatus Associative Array                                   */
#define AALC  432 /* LiabilityClass Associative Array                                  */
#define AINHK 433 /* AutoCheckInheritance Keyword                                      */
#define INMDL 434 /* _IsMendelianInconsistent Keyword                                  */
#define GRAPH 435 /* Graph command category                                            */
#define PLOTK 436 /* Graph plot command                                                */
#define GFPOS 437 /* Graph database GraphPositionField   internal variable             */
#define GFSCR 438 /* Graph database LOD GraphScoreField  internal variable             */
#define MFPBP 439 /* map database PositionBPField internal variable                    */
#define ADDK  440 /* ADD keyword                                                       */
#define ARRWK 441 /* ARROW keyword                                                     */
#define BAR_K 442 /* BAR keyword                                                       */
#define LABLK 443 /* LABEL keyword                                                     */
#define GFDRW 444 /* GRAPHDRAWINGFILE                                                  */
#define GTITL 445 /* GRAPHTITLE                                                        */
#define GXAXL 446 /* GRAPHXAXISLABEL                                                   */
#define GYAXL 447 /* GRAPHYAXISLABEL                                                   */
#define GXAMN 448 /* GRAPHXAXISMINIMUM                                                 */
#define GXAMX 449 /* GRAPHXAXISMAXIMUM                                                 */
#define GYAMN 450 /* GRAPHYAXISMINIMUM                                                 */
#define GYAMX 451 /* GRAPHYAXISMAXIMUM                                                 */
#define GXMAJ 452 /* GRAPHXAXISMAJORTICK                                               */
#define GXMIN 453 /* GRAPHXAXISMINORTICK                                               */
#define GYMAJ 454 /* GRAPHYAXISMAJORTICK                                               */
#define GYMIN 455 /* GRAPHYAXISMINORTICK                                               */
#define HORZK 456 /* HORIZONTAL Keyword                                                */
#define VERTK 457 /* VERTICAL   Keyword                                                */
#define AT_KY 458 /* AT         Keyword                                                */
#define CM__K 459 /* CM & CENTIMETER & CENTIMETERS Keywords                            */
#define CMG_K 460 /* CENTIMORGAN Keyword                                               */
#define VU__K 461 /* VU          Keyword                                               */
#define AAGA  462 /* GraphAnnotation[] Associative Array                               */
#define DEGRK 463 /* DEGREES     Keyword                                               */
#define INMDC 464 /* _MendelianInconsistencyCount  Keyword                             */
#define INMDP 465 /* _PercentMendelianInconsistent Keyword                             */
/*                                                                                     */
/* MADELINE COLORS                                                                     */
/*                                                                                     */
#define COLOR_STT 464  /* FIRST COLOR INDEX                                            */
#define BLACK     464
#define BLUE      465
#define BROWN     466
#define CYAN      467
#define DKGRY     468
#define FRGRN     469
#define GRAY      470
#define GREEN     471
#define LTGRY     472
#define MAGEN     473
#define ORANG     474
#define PEACH     475
#define PURPL     476
#define RED       477
#define WHITE     478
#define YELLW     479
#define COLOR_END 479 /* LAST COLOR INDEX                                              */
#define FROMK     480 /* FROM Keyword                                                  */
#define ABOVK     481 /* ABOVE Keyword                                                 */
#define BELOK     482 /* BELOW Keyword                                                 */
#define CNSLK     483 /* ConsoleHighlights keyword                                     */

/*                                         */
/* KEYWORDS USED FOR MAP FILE CONVERSIONS: */
/*                                         */
#define MARSH     484 /* Marshfield file (or similar HTML file)                           */
#define KS2TH     485 /* KosambiToTheta() function                                        */
#define TH2KS     486 /* ThetaToKosambi() function                                        */
#define LODK      487 /* LOD keyword: lod() function                                      */
#define MFFEM     488 /* MapFemalePositionField                                           */
#define MFMAL     489 /* MapMalePositionField                                             */
#define MAPDT     490 /* MapDetails setting to show details of genetic map                */
#define SIMWK     491 /* Simwalk keyword                                                  */
#define HL2TH     492 /* HaldaneToTheta() conversion function                             */
#define TH2HL     493 /* ThetaToHaldane() conversion function                             */
#define WEBRK     494 /* Weber keyword (for Marshfield Weber lab format genotype files    */
#define DECOC     495 /* New DECOMPOSE command (opposite of COMPOSE, formerly TRANSPOSE)  */
#define MADLN     496 /* MADELINE keyword.  Can you believe it took until 2003.02.13 for  */
                      /* the program to get a keyword of her own name? Added as a keyword */
                      /* for the DECOMPOSE command.                                       */
                      /*                                                                  */
#define MARKK     497 /* MARKER and MARKERS keyword                                       */
#define MERLK     498 /* MERLIN keyword                                                   */
#define SQUAR     499 /* SQUARE command                                                   */

#define LARAB     500 /* ARABIC     LANGUAGE keyword                                      */
#define LCHIN     501 /* CHINESE    LANGUAGE keyword                                      */
#define LGERM     502 /* GERMAN     LANGUAGE keyword                                      */
#define LGREK     503 /* GREEK      LANGUAGE keyword                                      */
#define LITAL     504 /* ITALIAN    LANGUAGE keyword                                      */
#define LJAPN     505 /* JAPANESE   LANGUAGE keyword                                      */
#define LPORT     506 /* PORTUGESE  LANGUAGE keyword                                      */
#define LRUSS     507 /* RUSSIAN    LANGUAGE keyword                                      */
#define LSPAN     508 /* SPANISH    LANGUAGE keyword                                      */
#define LTHAI     509 /* THAI       LANGUAGE keyword                                      */

#define NDLHY     510 /* NeedleInHaystack (i.e., "strstr()") function                     */
#define CTRLC     511 /* EndOfControlBlock                                                */
#define RSHDK     512 /* ReverseShading                                                   */
#define INFND     513 /* _IsFounder                                                       */
#define INPFD     514 /* _IsPrimaryFounder                                                */
#define INCPX     515 /* _Complexity = Pedigree complexity (2N-F)                         */
#define INNON     516 /* _NumberOfNonFounders                                             */
#define INNOF     517 /* _NumberOfFounders                                                */
#define ALLEK     518 /* "allele" keyword                                                 */
#define FREQK     519 /* "frequencies" keyword                                            */
#define READK     520 /* "read" (allele frequencies) command                              */
#define SAVEK     521 /* "save" (allele frequencies) command                              */
#define ALEFD     522 /* "AlleleField" internal variable                                  */
#define FRQFD     523 /* "FrequencyField" internal variable                               */
#define ACPAIR    524 /* _OffspringCountPairsTrue()                                       */
#define PCTRUE    525 /* _PedigreeCountTrue() function identifier                         */
#define INNOA     526 /* _NumberAffected attribute  == _NumberOfAffected                  */
#define INNOAP    527 /* _NumberOfAffectedPairs attribute                                 */
#define JULDAY    528 /* DateToJulian() function                                          */
#define JULDAT    529 /* JulianToDate() function                                          */
#define TODAY     530 /* Today() function                                                 */
#define LOGFA     531 /* AllLogFiles internal variable for setting log file names en masse*/
#define CMDFV     532 /* CommandFile log                                                  */
#define KNOWNTOKENS_INCLUDED
