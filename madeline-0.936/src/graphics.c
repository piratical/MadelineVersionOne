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
/* "graphics.c" is part of the Madeline software distribution.                       */
/* Written by Edward H. Trager.                                                      */
/*                                                                                   */
/* COPYRIGHT (C) 2000 THE REGENTS OF THE UNIVERSITY OF MICHIGAN,                     */
/* Ann Arbor, Michigan, USA.  ALL RIGHTS RESERVED.                                   */
/*                                                                                   */
/* Portions of this code were originally part of "gprint.c" copyright                */
/* (c) 1995 by Edward H. Trager                                                      */
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

/*                                                                         */
/* graphics.c   Graphics.h and graphics.c replace gprint.h/.c in           */
/*              new versions of Madeline with PostScript graphics.         */
/*              Unlike gprint.c which originated before Madeline           */
/*              and was designed as a much more generic graphic report     */
/*              library (even though it was limited to PCL and HP/GL),     */
/*              graphics.c has been written for use in Madeline            */
/*              and therefore contains more specialized functionality.     */
/*              Some of the generality built into the structures like CELL */
/*              and CELLARRAY are retained here, even though the full      */
/*              flexibility with which those structures were originally    */
/*              designed may not be employed in Madeline at the current    */
/*              time.                                                      */
/*                                                                         */
/* 19990427 ET initiated.                                                  */
/*                                                                         */
#include "graphics.h"

#include "tools.h"

#include "version.h" /* for version and copyright year */

#include "languageconventions.h"

/*                                                         */
/* NewCell(): Instantiates "n" new CELLs and returns PCELL */
/*                                                         */
/*                                                         */
PCELL NewCell(int n){
	
	PCELL c;
	int i,j;
	
	/*           */
	/* allocate: */
	/*           */
	c=(PCELL)malloc((unsigned) n*sizeof(CELL));
	if(c==NULL) ToolsErrorReport("NewCell(): unable to allocate CELL.");
	
	/*                 */
	/* and initialize: */
	/*                 */
	for(i=0;i<n;i++){
		c[i].u     =CENTIMETERS; /* defaults to centimeters        */
		c[i].x =c[i].y=0.0;      /* zeroed coordinates             */
		c[i].anchor=TOPLEFT;     /* anchor for cell                */
		c[i].xex   =5.0;         /* arbitrary but positive extents */
		c[i].yex   =1.0;         /* used as defaults               */
		c[i].m     =0.1;         /* margins at 1 mm                */
		/*                                                         */
		/* Defaults to no borders:                                 */
		/*                                                         */
		for(j=0;j<MAXBORDERS;j++) c[i].b[j]=NOLINE;
		
		c[i].tc    =TCBLACK;     /* text color                 */
		c[i].bc    =BCWHITE;     /* background color (shading) */
		c[i].j     =CENTER;      /* centered justification     */
		c[i].f     =ARIAL;       /* Arial font default         */
		c[i].t     =NULL;        /* no text labels             */
		c[i].n     =0;           /* no text labels             */
	}
	
	return c;
	
}


/*                                                              */
/* NewPedigreeDrawing(): Instantiates "n" new PEDIGREEDRAWINGs  */
/* and returns PPEDIGREEDRAWING                                 */
/*                                                              */
/*                                                              */
PPEDIGREEDRAWING NewPedigreeDrawing(int n){
	
	PPEDIGREEDRAWING d;
	int i;
	/* int j; */
	
	/*           */
	/* allocate: */
	/*           */
	d=(PPEDIGREEDRAWING)malloc((unsigned) n*sizeof(PEDIGREEDRAWING));
	if(d==NULL) ToolsErrorReport("NewPedigreeDrawing(): unable to allocate PEDIGREEDRAWING.");
	
	/*                 */
	/* and initialize: */
	/*                 */
	for(i=0;i<n;i++){
		d[i].u     =CENTIMETERS; /* defaults to centimeters */
		d[i].x =d[i].y=0.0;      /* zeroed coordinates      */
		d[i].xex   =0.0;         /*                         */
		d[i].yex   =0.0;         /*                         */
		d[i].CellMargin =0.1;    /* margins at 1 mm         */
		/*                                                  */
		/* CELLS all NULL by default:                       */
		/*                                                  */
		d[i].PageHeader=NULL;
		d[i].Title=NULL;
		d[i].Legend=NULL;
		d[i].Individual=NULL;
		d[i].PageFooter=NULL;
		
		d[i].page=0;
	}
	
	return d;
	
}

/*                                                                                   */
/* NewAssignedCell(): Instantiate and assign values to one (1) CELL:                 */
/* pointer to the cell is returned                                                   */
/*                                                                                   */
/* This function is useful for creating fixed cells such as report                   */
/* titles,footers,page headers or page footers                                       */
/*                                                                                   */
/* int n indicates number of text lines, with a variable argument list               */
/* to follow.  The variable argument list must consist of triplets of variables.     */
/* The first argument in a triplet must be a char * to the text label.  The 2nd      */
/* argument must be an int indicating style (BOLD, ITALIC, etc.).  The last argument */
/* is the size of the text in points.                                                */
/*                                                                                   */
PCELL NewAssignedCell(int u,double x,double y,double xex,double yex,double m,
		      int bleft,int btop,int bright,int bbot,
		      int tc,int bc,int j,int f,
		      int n, ... ){

	PCELL c;
	int i;
	va_list ap;
	
	c=(PCELL)malloc((unsigned)sizeof(CELL));
	if(c==NULL) ToolsErrorReport("NewAssignedCell(): no memory for CELL");
	
	c->u  =u;
	c->x  =x;
	c->y  =y;
	c->xex=xex;
	c->yex=yex;
	c->m  =m;
	c->b[LEFTBORDER  ]=bleft;
	c->b[TOPBORDER   ]=btop;
	c->b[RIGHTBORDER ]=bright;
	c->b[BOTTOMBORDER]=bbot;        
	c->tc=tc;
	c->bc=bc;
	c->j=j;
	c->f=f;
	c->n=n;  /* number of text lines */

	if(c->n>0){
		va_start(ap,n);
		c->t=NewTextLabel(c->n);
		for(i=0;i<c->n;i++){
			/* process triplets of arguments: */
			c->t[i].t=va_arg(ap,char *);
			c->t[i].s=va_arg(ap,int);
		    c->t[i].z=va_arg(ap,int);
		}
		va_end(ap);     
	}
	
	return c;
}


/*                                                                   */
/* AssignCellText(): Assign text labels to an extant CELL.           */
/*                                                                   */
/* The second argument, n, indicates number of text labels           */
/* for the cell (which are allocated by this function).              */
/* A variable argument list follows thereafter.                      */
/* The variable argument list consists of triplets of arguments.     */
/* The first argument in a triplet is a char * to the text label.    */
/* This is followed by the style integer (BOLD, ITALIC, etc.).  The  */
/* final argument of each triplet is the size of the text in points. */
/*                                                                   */
PCELL AssignCellText(PCELL c,int n, ... ){

	int i;
	va_list ap;
	
	c->n=n;  /* number of text labels */

	va_start(ap,n);
	c->t=NewTextLabel(c->n);
	for(i=0;i<c->n;i++){
		/* process triplets of arguments: */
		c->t[i].t=va_arg(ap,char *);
		c->t[i].s=va_arg(ap,int);
		c->t[i].z=va_arg(ap,int);
	}
	va_end(ap);
	
	return c;
}


/*                                                              */
/* PrintIconCell(): Prints a CELL using a call to               */
/* Madeline's PostScript IconCell routine                       */
/*                                                              */
/* Note that IconCell uses some fixed formatting conventions    */
/* so some of the formatting data in the CELL will be ignored.  */
/* When CELL was originally designed, a non-Postscript print    */
/* model based on HP GL/2 & PCL was used -- that model made use */
/* of all of the formatting data in CELL.  When I switched to   */
/* Postscript, I had to keep things simple to start with, as I  */
/* was basically learning Postscript by coding it, so I decided */
/* to scrap some of the non-essential formatting niceties in    */
/* favor of some simple stuff that just plain works.            */
/*                                                              */
/* 2002.06.11.ET ADDENDUM: Added "PINDIVIDUAL p" as a parameter */
/*                         so that we can look at the flag      */
/*                         vectors.                             */
/*                                                              */
void PrintIconCell(FILE *fp,PCELL c,PINDIVIDUAL p){

	char   *unit;   /* unit of measure                                          */
	char   *border; /* border style                                             */
	PDBF   db;      /* the database                                             */
	UINT16 fn;      /* field number                                             */
	UINT16 qf;      /* quick formatting data flag                               */
	UINT32 cr;      /* database record number                                   */
	char   hold[MAXSTRINGLENGTH]; /* string buffer                              */
	char   *ss;     /* string pointer                                           */
	char MissingValueString[]="."; /* visible representation for missing values */
	int i;
	INT32 iv;
	double dv;
	int precision;

	extern PCONTROL g;
	extern PLANGUAGEOBJECT Language[LANGUAGE_COUNT];
	
	/*                      */
	/* Set unit of measure: */
	/*                      */
	switch(c->u){
	case CENTIMETERS:
		unit="cm";
		break;
	case MILLIMETERS:	
		unit="mm";
		break;	
	case INCHES:	
		unit="inch";
		break;
	default:
		unit="cm";
		break;
	}
	/*                                       */
	/* Set border type: based on left border */
	/*                                       */
	switch(c->b[LEFTBORDER]){
	case NOLINE:
		border="NOLINE";
		break;
	case THINLINE:
		border="THINLINE";
		break;
	case NORMLINE:
		border="MEDIUMLINE";
		break;
	case THICKLINE:
		border="THICKLINE";
		break;
	default:
		border="NOLINE";
		break;
	}
			
	/*                                                           */
	/* Print x, y, cell width, border type, and array delimiter: */
	/*                                                           */
	fprintf(fp,"%.2f %s %.2f %s %.2f %s %s SHADE [",
		c->x,
		unit,
		c->y,
		unit,
		c->xex,
		unit,
		border
	);
	/*                                                         */
	/* print strings into array by looping through TEXTLABELS: */
	/*                                                         */
	for(i=0;i<c->n;i++){
		/* 
		   if data source not NULL
		*/
		if(c->t[i].d){
			db=c->t[i].d->dbf;
			fn=c->t[i].d->fn;
			qf=c->t[i].d->qfmt;
			cr=(qf&ONEBEFORE)?db->cr-1:db->cr;
			if(qf & CUSTOMFORMAT){
				/*
				   custom format
				   custom format -- use d->fmt as the format specifier
				   custom format
				*/
				/*                                                             */
				/* missing value support not yet implemented for CUSTOMFORMAT: */
				/*                                                             */
				if(qf & TEXTDATA){
					/* read field as a string and format string into hold */
					sprintf(hold,c->t[i].d->fmt,db->ReadString(db,NULL,fn,cr));
				}else if(qf & INTEGERDATA){
					/* read field as integer */					
					sprintf(hold,c->t[i].d->fmt,(int)db->ReadInt32(db,fn,cr));
				}else if(qf & NODATA){
					/*                      */
					/* constant -- no data: */
					/*                      */
					strcpy(hold,c->t[i].d->fmt);
				}else{
					/* read field as double */
					sprintf(hold,c->t[i].d->fmt,db->ReadDouble(db,fn,cr));
				}
				/* point ss to the beginning of hold: // */
				ss=hold;
			}else{
				/* 
				   quick format
				   quick format -- This is almost always used.
				   quick format    Makes life a lot simpler.
				*/
				/*                                      */
				/* Missing value support included here: */
				/*                                      */
				if(qf & GENOTYPEDATA){
					/* read field as a string and format */
					/* string into hold                  */
					db->ReadString(db,hold,fn,cr);
					if(!*hold){
						 ss=MissingValueString;
					}else{
						/* change "/" to "|" if HaplotypeDisplay requested: */
						if(g->Haplotype){
							/* find the "/": */
							for(ss=hold;*ss && *ss!='/';ss++);
							if(*ss=='/') *ss='|';
						}
						/* point ssource to beginning of hold */
						ss=hold;
					}				
				}else if(qf & TEXTDATA){
					/* read field as a string and format string into hold */
					db->ReadString(db,hold,fn,cr);
					if(!*hold){
						 ss=MissingValueString;
					}else{
						/* point ssource to beginning of hold */
						ss=hold;
					}
				}else if(qf & INTEGERDATA){
					/* read field as integer */
					iv=(int)db->ReadInt32(db,fn,cr);
					if(iv==INT32MISSINGVALUE){
						ss=MissingValueString;
					}else{
						sprintf(hold,"%i",iv);
						ss=hold;
					}
				}else if(qf & DATEDATA){
					/*                       */
					/* Read and format date: */
					/*                       */
					sprintf(hold,"%s",Language[g->lang]->PrintBriefDate(db->ReadDate(db,fn,cr)));
					if(!*hold){
						ss=MissingValueString;
					}else{
						/* point ssource to beginning of hold: */
						ss=hold;
					}
				}else{
					/* read field as double, taking precision from least significant 4 bits: */
					dv=db->ReadDouble(db,fn,cr);
					if(dv==DOUBLEMISSINGVALUE){
						ss=MissingValueString;
					}else{
						precision=qf&0x000F;
						sprintf(hold,"%.*f",precision,dv);
						ss=hold;
					}
				}
			}
		}else{
			/*
			   data source NULL
			   data source NULL - fixed format string: so t->t should be non-NULL
			   data source NULL   and use that.
			 */
			 ss=c->t[i].t;
		}
		/*                                  */
		/* print ss as a PostScript string: */
		/*                                  */
		/* 2002.06.11.ET ADDENDUM: Check    */
		/* integrity flag                   */
		if(p && p->integrity && p->integrity[fn]){
			/* Prints the string in BOLD RED */
			/* (On a B/W printer, in BOLD):  */
			fprintf(fp,"(R~%s)",ss);
			
		}else{
			
			fprintf(fp,"(%s)",ss);
			
		}
	}
	/*                                            */
	/* closing array delimiter and call IconCell: */
	/*                                            */
	fprintf(fp,"] IconCell\n");
	/*                        */
	/* That's it -- all done. */
	/*                        */
}


/*                                                                                */
/* AbsoluteLine(): Draw a line from sttx,stty to endx,endy at current line width: */
/*                                                                                */
void AbsoluteLine(FILE *fp,double sttx,double stty,double endx,double endy){

	/*                     */
	/* PostScript command: */
	/*                     */
	fprintf(fp,"newpath %.2f cm %.2f cm moveto %.2f cm %.2f cm lineto stroke\n",
		sttx,
		stty,
		endx,
		endy
	);
};


/*                                                                              */
/* HorizontalLine: draw horizontal line from dx,dy with length dlen at current  */
/* line width:                                                                  */
/*                                                                              */
void HorizontalLine(FILE *fp,double dx,double dy,double dlen){

	/*                     */
	/* PostScript command: */
	/*                     */
	fprintf(fp,"newpath %.2f cm %.2f cm moveto %.2f cm 0 rlineto stroke\n",
		dx,
		dy,
		dlen
	);
};


/*                                                                              */
/* HorizontalLine: draw horizontal line from dx,dy with length dlen at current  */
/* line width:                                                                  */
/*                                                                              */
void DoubleHorizontalLine(FILE *fp,double dx,double dy,double dlen){

	/*                     */
	/* PostScript command: */
	/*                     */
	fprintf(fp,"newpath %.2f cm %.2f cm moveto %.2f cm 0 rlineto DoubleBridge\n",
		dx,
		dy,
		dlen
	);
};


/*                 */
/* VerticalLine(): */
/*                 */
void VerticalLine(FILE *fp,double dx,double dy,double dlen){

	/*                     */
	/* PostScript command: */
	/*                     */
	fprintf(fp,"newpath %.2f cm %.2f cm moveto 0 %.2f cm rlineto stroke\n",
		dx,
		dy,
		dlen
	);
};


/*                  */
/* VerticalDrop1(): */
/*                  */
void VerticalDrop1(FILE *fp,double dx,double dy){

	/*                     */
	/* PostScript command: */
	/*                     */
	fprintf(fp,"newpath %.2f cm %.2f cm moveto 0 VDrop1 rlineto stroke\n",
		dx,
		dy
	);
};


/*                  */
/* VerticalDrop2(): */
/*                  */
void VerticalDrop2(FILE *fp,double dx,double dy){

	/*                     */
	/* PostScript command: */
	/*                     */
	fprintf(fp,"newpath %.2f cm %.2f cm moveto 0 VDrop2 rlineto stroke\n",
		dx,
		dy
	);
};


/*                                 */
/* VerticalDrop2MinusIconRadius(): */
/*                                 */
void VerticalDrop2MinusIconRadius(FILE *fp,double dx,double dy){

	/*                     */
	/* PostScript command: */
	/*                     */
	fprintf(fp,"newpath %.2f cm %.2f cm moveto 0 VDrop2MinusIR rlineto stroke\n",
		dx,
		dy
	);
};


/*                       */
/* HorizontalGrayLine(): */
/*                       */
void HorizontalGrayLine(FILE *fp,double dx,double dy,double dlen,int width, int uom,double density){

	char *LineWidth;
	char *unit;


	/*                */
	/* Set LineWidth: */
	/*                */
	switch(width){
	case NOLINE:
		LineWidth="NOLINE";
		break;
	case THINLINE:
		LineWidth="THINLINE";
		break;
	case NORMLINE:
		LineWidth="MEDIUMLINE";
		break;
	case THICKLINE:
		LineWidth="THICKLINE";
		break;
	default:
		LineWidth="NOLINE";
		break;
	}

	/*                      */
	/* Set unit of measure: */
	/*                      */
	switch(uom){
	case CENTIMETERS:
		unit="cm";
		break;
	case MILLIMETERS:	
		unit="mm";
		break;	
	case INCHES:	
		unit="inch";
		break;
	default:
		unit="cm";
		break;
	}
	/*                     */
	/* PostScript command: */
	/*                     */
	fprintf(fp,"gsave newpath %.2f setgray %s setlinewidth %.2f %s %.2f %s moveto %.2f %s 0 rlineto stroke grestore\n",
		1.0-density,
		LineWidth,
		dx,
		unit,
		dy,
		unit,
		dlen,
		unit
	);
};


/*                                              */
/* NewDataSource(): allocates a new data source */
/*                                              */
PDATASOURCE NewDataSource(PDBF dbf,UINT16 fn,UINT16 qfmt,char *fmt){

	PDATASOURCE d;
	
	d=(PDATASOURCE) malloc((unsigned) sizeof(DATASOURCE));
	if(d==NULL) ToolsErrorReport("Out of memory in NewDataSource()");
	
	d->dbf =dbf;
	d->fn  =fn;
	d->qfmt=qfmt;
	d->fmt =fmt;
	
	return d;

}

/*                                          */
/* NewTextLabel: allocates "n" text labels: */
/*                                          */
PTEXTLABEL NewTextLabel(int n){

	int i;
	PTEXTLABEL t;
	
	t=(PTEXTLABEL)malloc((unsigned) n*sizeof(TEXTLABEL));
	if(t==NULL) ToolsErrorReport("Out of memory in NewTextLabel()");
	
	for(i=0;i<n;i++){
		t[i].t=NULL;
		t[i].s=NORMAL;
		t[i].z=12;
		t[i].d=NULL;
	}

	return t;
}


/*                                                     */
/* SetPostScriptEnvironment(): Wrapper which downloads */
/* Madeline's PostScript routines in postscript.c:     */
/*                                                     */
void SetPostScriptEnvironment(PCONTROL c,int LegendFontSize,int LabelFontSize){

	/*                                                */
	/* This wrapper downloads the Madeline PostScript */
	/* routines to the print device:                  */
	/*                                                */
	DownLoadPostScriptRoutines(c->streams.prn,VERSION,COPYRIGHTYEAR,c,LegendFontSize,LabelFontSize);
}


/*                                 */
/* SetPostScriptPageEnvironment(): */
/*                                 */
void SetPostScriptPageEnvironment(PCONTROL c,char *DrawingTitle,char *PostScriptPage,int PageOrdinal){

	/*              */
	/* Page Prolog: */
	/*              */
	fprintf(c->streams.prn,"%%\n%% %s\n%%%%Page: %s %i\n%%\n",
		DrawingTitle,
		PostScriptPage,
		PageOrdinal
	);

	/*                 */
	/* set line width: */
	/*                 */
	fputs("THINLINE setlinewidth\n",c->streams.prn);	
	/*                           */
	/* Set line caps to project: */
	/*                           */
	fputs("2 setlinecap\n",c->streams.prn);	

}



/*                                                               */
/*                                                               */
/* DrawIndividual(): Draw a square or circle                     */
/* representing an individual, and a CELL containing             */
/* data about that individual                                    */
/*                                                               */
/* FILE *fp      : Print file                                    */
/* PINDIVIDUAL p : INDIVIDUAL to be drawn                        */
/* double px,py  : Center coordinates of the individual's icon   */
/* PCELL cell    : The CELL containing data references to print  */
/* PCONTROL c    : CONTROL structure                             */
/* PPEDIGREE pd  : Pedigree being currently drawn                */
/* int ith       : Ith founding group tree being currently drawn */
/*                                                               */
void DrawIndividual(FILE *fp,PINDIVIDUAL p,double px,double py,PCELL cell,PCONTROL control,PPEDIGREE pd,int ith){
	
	extern PCONTROL g;
	char hold[25];
	int AncestorsPage;
	
	/* Test highlighting: do this on excluded individuals: */
	if(p->excluded) fprintf(fp,"%.2f cm %.2f cm CircleHighlight\n",px,py);
	
	/*                                                         */
	/* Start setting parameters for Madeline PostScript "icon" */
	/* routine:                                                */
	/*                                                         */
	fprintf(fp,"%.2f cm %.2f cm %s %s [",
		px,
		py,
		p->db?"REAL":"VIRTUAL",
		p->female==MISSING?"UNKNOWNGENDER":p->female?"FEMALE":"MALE"
	);
	/*                                                                */
	/* The next set of parameters are PostScript arrays               */
	/* consisting of a string label, level and total number of levels */
	/* for each categorical variable to be represented on the         */
	/* icon symbol.  These are printed by PrintIconSetArray():        */
	/*                                                                */
	PrintIconSetArray(fp,p,g);
	/*                                                                    */
	/* Close array of arrays and call Madeline's PostScript icon routine: */
	/*                                                                    */
	fprintf(fp,"] icon\n");
	
	/*                        */
	/* Draw line if deceased: */
	/*	                       */
	if(p->dead && p->dead!=MISSING){
		fprintf(fp,"%.2f cm %.2f cm Deceased\n",
			px,
			py
		);	
	}
	/*                                */
	/* Draw an arrow for the proband: */
	/*                                */
	if(p->proband && p->proband!=MISSING){
		fprintf(fp,"%.2f cm %.2f cm ProbandArrow\n",
			px,
			py
		);
	}

	/*                                                              */
	/* If the ancestors of this person have already been drawn      */
	/* on a previous page, indicate this on a little icon above the */
	/* drawing:                                                     */
	/*                                                              */
	if(p->drawn && 
	   (
	    (p->f && p->f->drawn && p->f->page!=ith+1) ||
            (p->m && p->m->drawn && p->m->page!=ith+1)
	   )
	){
		VerticalLine(fp,px,py+control->d.IconRadius,0.08);
		sprintf(hold,"see p. %i",p->page);
		QuickLabel(
			fp,
			px,
			py+control->d.IconRadius+0.55,
			control->d.CellWidth,
			NOLINE,
			1,
			hold
		);
	}

	/*                                                             */
	/* Otherwise, if the ancestors of this person are available    */
	/* but have not yet been drawn,                                */
	/* draw a label indicating on which page they WILL be drawn.   */
	/* This is simply the founding group index number. Also        */
	/* store the page number on which this individual has          */
	/* has been drawn, which is the ith page, passed as parameter  */
	/* above:                                                      */
	/*                                                             */
	/* NOTE: 2001.12.15.ET ADDED "!p->x->consanguinity" to prevent */
	/*       inadvertantly drawing a "See p. xx" tag when both     */
	/*       parents are really on the same page but one has not   */
	/*       yet been drawn due to consanguinity forcing us to     */
	/*       wait until that individual is drawn somewhere else    */
	/*       on the page.                                          */
	/*                                                             */
	if( (p->f && !p->f->drawn && !p->f->consanguinity) ||
	    (p->m && !p->m->drawn && !p->m->consanguinity)
	){

		/*                                                               */
		/* Figure out on what page number his ancestors will be drawn:   */
		/*                                                               */
		AncestorsPage=1+DetermineFoundingGroup(p,pd);
		/*                                                               */
		/* draw a small label above the individual                       */
		/* informing the user of the page on which the ancestors of this */
		/* individual will be drawn:                                     */
		/*                                                               */
		VerticalLine(fp,px,py+control->d.IconRadius,0.08);
		sprintf(hold,"see p. %i",AncestorsPage);
		QuickLabel(
			fp,
			px,
			py+control->d.IconRadius+0.55,
			control->d.CellWidth,
			NOLINE,
			1,
			hold
		);

	}

	/*            */
	/* Draw Cell: */
	/*            */
	
	/*                                  */
	/* Set CELL's starting coordinates: */
	/*                                  */
	cell->x=px;
	cell->y=py-control->d.IconDiameter;


	/*                                                    */
	/* -- 980820 & 980821 ET ADDENDA:                     */
	/* -- 19990614 ET ADDENDUM TO FIX POSTSCRIPT LABELING */
	/*                                                    */
	/* NOTE IF INDIVIDUAL WAS MARKED FOR EXCLUSION        */
	/* BY THE USER, IF HAS DATA, OR IF INCLUDED IN        */
	/* OUTPUT                                             */
	/*                                                    */
	/* NOTE IF AN INDIVIDUAL HAS DATA                     */
	/*                                                    */
	if(p->excluded || p->hasdata || p->included){

		hold[0]='\0';
		if(p->excluded && p->included){
			/* marked for exclusion but included for structural maintenance: */
			strcat(hold,"DATA EXCL INDV INCL");
		}else if(p->excluded){
			strcat(hold,"EXCLUDED");
		}else{
			if(p->hasdata) strcat(hold,"HAS DATA");
			if(p->included && p->hasdata) strcat(hold," - ");
			if(p->included) strcat(hold,"INCLUDED");
		}

		QuickLabel(
			fp,
			px,
			py-control->d.IconRadius*0.6,
			control->d.CellWidth,
			NOLINE,
			1,
			hold
		);
	}
		
	/*                 */
	/* Print the cell: */
	/*                 */
	if(p->db){
		/*                            */
		/* person is in the database: */
		/*                            */
				
		/*                  */
		/* print cell data: */
		/*                  */
		GoTo(p->db,p->rci);
		PrintIconCell(fp,cell,p);
	}else{
		/*                                              */
		/* person is inserted, not in the database:     */
		/* -- print only the STUDYID of the person:     */
		/*                                              */
		/*                                              */
		/* However, don't print if LabelCreatedVirtuals */
		/* is turned off                                */
		cell->x=px;
		cell->y=py-control->d.IconDiameter;

		if(p->id[0]!='!' || g->LabelCreatedVirtuals){
			QuickLabel(
				fp,
				px,
				py-control->d.IconDiameter,
				control->d.CellWidth,
				THINLINE,
				2,
				p->id,
				"(not in database)"
			);
		}
	}
	/*                                                  */
	/* record:                                          */
	/* (1) that this individual has been drawn,         */
	/* (2) the (most recent) page on which he was drawn */
	/* (3) his x and y coordinates                      */
	/*                                                  */
	p->drawn++;
	p->page=ith+1;
	p->x=px;
	p->y=py;
	/*                                               */
	/* Keep the minimum and maximum icon coordinates */
	/* so that we can figure out a bounding box for  */
	/* the ith founding group tree:                  */
	/*                                               */
	SaveMinMaxCoordinates(p,pd,ith);
}


/*                         */
/* SaveMinMaxCoordinates() */
/*                         */
void SaveMinMaxCoordinates(PINDIVIDUAL p, PPEDIGREE pd, int i){

	if(p->x<pd->fg[i].lowx ) pd->fg[i].lowx =p->x;
	if(p->x>pd->fg[i].highx) pd->fg[i].highx=p->x;
	if(p->y<pd->fg[i].lowy ) pd->fg[i].lowy =p->y;
	if(p->y>pd->fg[i].highy) pd->fg[i].highy=p->y;	
}



/*            */
/* FreeCell() */
/*            */
void FreeCell(PCELL c){

	int i;

	/*                                                      */
	/* First, free the DATASOURCE data sources, if present: */
	/*                                                      */
	for(i=0;i<c->n;i++)
		if(c->t[i].d) free(c->t[i].d);
	/*                                 */
	/* Then free the TEXTLABEL vector: */
	/*                                 */
	free(c->t);
	/*                                            */
	/* Now, finally, free the CELL object itself: */
	/*                                            */
	free(c);

}


/*                 */
/* FreeCellArray() */
/*                 */
void FreeCellArray(PCELLARRAY ca){

	int i,j;
	
	for(i=0;i<ca->n;i++){
		/*                                          */
		/* First, free the DATASOURCE data sources: */
		/*                                          */
		for(j=0;j<ca->c[i].n;j++)
			if(ca->c[i].t[j].d) free(ca->c[i].t[j].d);
		/*                                  */
		/* Then free the TEXTLABEL vectors: */
		/*                                  */
		free(ca->c[i].t);
	}
	/*                        */
	/* Free the PCELL vector: */
	/*                        */
	free(ca->c);
	/*                                             */
	/* Finally, free the PCELLARRAY object itself: */
	/*                                             */
	free(ca);
}

/*              */
/* FreeReport() */
/*              */
void FreeReport(PREPORT r){

	/*                                                               */
	/* First free allocated sub-objects: page header, title, report  */
	/* header, report line template, report footer, and page footer: */
	/*                                                               */
	if(r->PageHeader) FreeCell(r->PageHeader );
	if(r->Title     ) FreeCell(r->Title      );
	if(r->Header    ) FreeCellArray(r->Header);
	if(r->Line      ) FreeCellArray(r->Line  );
	if(r->Footer    ) FreeCellArray(r->Footer);
	if(r->PageFooter) FreeCell(r->PageFooter );
	/*                        */
	/* Now, free main object: */
	/*                        */
	free(r);
}


/*               */
/* FreeDrawing() */
/*               */
void FreeDrawing(PPEDIGREEDRAWING d){

	/*                                    */
	/* First free allocated sub-objects:  */
	/*                                    */
	if(d->PageHeader) FreeCell(d->PageHeader);
	if(d->Title     ) FreeCell(d->Title     );
	if(d->Legend    ) FreeCell(d->Legend    );
	if(d->Individual) FreeCell(d->Individual);
	if(d->PageFooter) FreeCell(d->PageFooter);
	/*                        */
	/* Now, free main object: */
	/*                        */
	free(d);
}


/*                                                                           */
/* AutoLabel(): Automatically sizes a label based on the text and font sizes */
/*              used.                                                        */
/*                                                                           */
/* x: x center position                                                      */
/* y: y center position                                                      */
/* b: border: 1 for yes, single border, 2 for no, no line border             */
/* u: unit of measure                                                        */
/* j: justification                                                          */
/* font: font (Times Roman, Arial, ...)                                      */
/* n: number of text labels                                                  */
/* ...: "char *text,int style, int fontsize" triplets must follow int n      */
/* to describe the text and font sizes of each                               */
/*                                                                           */

void AutoLabel(FILE *fp,double x, double y,int b,int u,int j,int font,int n,...){

	PCELL c;
	int i;
	/* double deltay,deltax,maxx; */
	va_list ap;
	
	c=(PCELL)malloc((unsigned)sizeof(CELL));
	if(c==NULL) ToolsErrorReport("AutoLabel(): no memory for CELL");
	
	c->u  =u;
	c->x  =x;
	c->y  =y;
	c->m  =0.1;
	if(b){
		c->b[LEFTBORDER  ]=THINLINE;
		c->b[TOPBORDER   ]=THINLINE;
		c->b[RIGHTBORDER ]=THINLINE;
		c->b[BOTTOMBORDER]=THINLINE;
	}else{
		c->b[LEFTBORDER  ]=NOLINE;
		c->b[TOPBORDER   ]=NOLINE;
		c->b[RIGHTBORDER ]=NOLINE;
		c->b[BOTTOMBORDER]=NOLINE;
	}
	c->tc=TCBLACK;
	c->bc=BCWHITE;
	c->j=j;
	c->f=font;
	c->n=n;  /* number of text lines */

	if(c->n>0){
		va_start(ap,n);
		c->t=NewTextLabel(c->n);
		for(i=0;i<c->n;i++){
			/* process triplets of arguments: */
			c->t[i].t=va_arg(ap,char *);
			c->t[i].s=va_arg(ap,int);
		    c->t[i].z=va_arg(ap,int);
		}
		va_end(ap);     
	}
	
	/*                      */
	/* Set CELL's x-extant: */
	/*	                     */
	c->xex=g->d.HorizontalInterval-0.1;
	
	/*                 */
	/* print the cell: */
	/*                 */
	PrintIconCell(fp,c,NULL);
	/*                   */
	/* release the cell: */
	/*                   */
	FreeCell(c);
}



/*                                                           */
/* PrintIconSetArray():  This prints out the arrays for each */
/* ICONSET variable as part of the parameter list that gets  */
/* passed to Madeline's PostScript routine "icon":           */
/*                                                           */
void PrintIconSetArray(FILE *fp,PINDIVIDUAL p,PCONTROL c){

	int i,j;
	
	VARIABLE d,*data=&d;
	PNODE pn;
	int ThisLevel;
	PVARIABLE pLabelVariable=NULL;
	PVARIABLE pColorVariable=NULL;
	PVARIABLE pGrayVariable =NULL;
	/*                                                    */
	/* Case where there are no icon set variables:        */
	/* We still need to pass a postscript array to the    */
	/* icon procedure:                                    */
	/*                                                    */
	if(!c->db->n.ci){
		fprintf(fp,"[() 1.0 1.0 1.0 1.0 ]");
		return;
	}
	/*                                                       */
	/* Get here if there are categorical icon set variables: */
	/*                                                       */
	
	if(!p->db){
		/*                                                           */
		/* Not in database: print MISSING for each ICONSET variable: */
		/*                                                           */
		for(j=0;j<c->db->n.ci;j++){
			
			fprintf(fp,"[(.) 1.0 1.0 1.0 1.0 ]");
		}
	}else{
		/*              */
		/* in database: */
		/*              */
		for(j=i=0;j<p->db->n.ci && i<p->db->fc;i++){
			if(p->db->ff[i]&ICONSET){
				/*                */
				/* ICONSET field: */
				/*                */
				switch(p->db->FieldType(p->db,i)){
				case 'N':
					data->t  =NUMERICVALUE;
					data->i  =MISSING;
					data->e.v=p->db->ReadDouble(p->db,i,p->rci);
					if(data->e.v==DOUBLEMISSINGVALUE){
						ThisLevel=MISSING;
					}else{
						pn=SeekNode(p->db->ftree[i],(void *)data,CompareVariableObjects);
						ThisLevel=pn->rank;
						pLabelVariable = LookupValueFromKey(p->db->fllaa[i],data);
						pColorVariable = LookupValueFromKey(p->db->flcaa[i],data);
						pGrayVariable  = LookupValueFromKey(p->db->flgaa[i],data);
					}
					break;
				case 'D':
					data->t  =DATEVALUE;
					data->i  =MISSING;
					data->e.v=p->db->ReadDate(p->db,i,p->rci);
					if(data->e.v==MISSINGDATE){
						ThisLevel=MISSING;
					}else{
						pn=SeekNode(p->db->ftree[i],(void *)data,CompareVariableObjects);
						ThisLevel=pn->rank;
						pLabelVariable = LookupValueFromKey(p->db->fllaa[i],data);
						pColorVariable = LookupValueFromKey(p->db->flcaa[i],data);
						pGrayVariable  = LookupValueFromKey(p->db->flgaa[i],data);
					}
					break;
				case 'C':
					data->t  =STRINGVALUE;
					data->i  =MISSING;
					data->e.s=p->db->ReadString(p->db,NULL,i,p->rci);
					if(!data->e.s[0]){
						ThisLevel=MISSING;
					}else{
						pn=SeekNode(p->db->ftree[i],(void *)data,CompareVariableObjects);
						ThisLevel=pn->rank;
						pLabelVariable = LookupValueFromKey(p->db->fllaa[i],data);
						pColorVariable = LookupValueFromKey(p->db->flcaa[i],data);
						pGrayVariable  = LookupValueFromKey(p->db->flgaa[i],data);
					}
					break;
				}
				
				if(ThisLevel==MISSING){
					/*                              */
					/* Missing for this individual: */
					/*                              */
					fprintf(fp,"[(.) 1.0 1.0 1.0 1.0 ]");
				}else{
					/*              */
					/* Non-Missing: */
					/*              */
					fprintf(fp,"[(%s) %4.2f %s]",
						pLabelVariable->e.s,
						pGrayVariable->e.v,
						pColorVariable->e.s
					);
				}
			}
		}
	}
}



/*                                                                     */
/* QuickLabel() uses Madeline's Postscript "IconCell" function to      */
/* print labels in a cell with top center coordinates at x,y, width of */
/* w, border type b and n labels which follow after parameter "n":     */
/*                                                                     */
void QuickLabel(FILE *fp,double x,double y,double w,int b,int n,...){

	char *border;
	va_list ap;
	/* char *string; */
	int i;
	
	/*                                       */
	/* Set border type: based on left border */
	/*                                       */
	switch(b){
	case NOLINE:
		border="NOLINE";
		break;
	case THINLINE:
		border="THINLINE";
		break;
	case NORMLINE:
		border="MEDIUMLINE";
		break;
	case THICKLINE:
		border="THICKLINE";
		break;
	default:
		border="NOLINE";
		break;
	}
			
	/*                                                           */
	/* Print x, y, cell width, border type, and array delimiter: */
	/*                                                           */
	fprintf(fp,"%.2f cm %.2f cm %.2f cm %s SHADE [",
		x,
		y,
		w,
		border
	);
	/*                                                         */
	/* print strings into array by looping through TEXTLABELS: */
	/*                                                         */
	if(n>0){
		va_start(ap,n);
		for(i=0;i<n;i++){
			/*                  */
			/* process strings: */
			/*                  */
			fprintf(fp,"(%s)",va_arg(ap,char *));
		}
		va_end(ap);     
	}
	/*                                            */
	/* closing array delimiter and call IconCell: */
	/*                                            */
	fprintf(fp,"] IconCell\n");
	/*                        */
	/* That's it -- all done. */
	/*                        */
}

/*                                                              */
/* PrintPedigreeDrawing() rotates, translates, scales, and even */
/* prints the drawing across multiple pages, as required:       */
/* - Works for DividedPages drawings                            */
#define MAXIMUMREDUCTION 0.68
double PrintPedigreeDrawing(PCONTROL c,PPEDIGREE pd,int i,char *ProcName){

	double scalex=1.0, scaley=1.0,scale=1.0;
	double FrameHeight,FrameWidth;
	double OptimalFrameHeight,OptimalFrameWidth;
	double OptimalScale;
	double WHRatio;
	double ScaledHeight,ScaledWidth;
	double XOffset,YOffset;
	
	int rows,cols;
	double WHRatioDifference;
	/* double ThisWHRatio; */
	double ThisDifference;
	int OptimalRows,OptimalCols;
	int LandscapeOrientation;
	int MultiplePagePrint;
		
	/*                                                            */
	/* Figure actual width and height of drawing:                 */
	/* Note that width is corrected by adding HorizontalInterval, */
	/* and height is corrected by adding VerticalDrop1:           */
	/*                                                            */
	pd->fg[i].width =  pd->fg[i].highx-pd->fg[i].lowx;
	pd->fg[i].width += c->d.HorizontalInterval;
	pd->fg[i].height=  pd->fg[i].highy-pd->fg[i].lowy;
	pd->fg[i].height-= c->d.VerticalDrop1; /* VerticalDrop1 is a negative quantity */
	pd->fg[i].height+= 2.0; /* Distance from top of pedigree title to first icon   */
	/*                                                                             */
	/* Determine and set page orientation and absolute page dimensions,            */
	/* as applicable:                                                              */
	/*                                                                             */
	switch(c->d.Orientation){
	case PORTRAITORIENTATION:
		FrameHeight=c->d.PageHeight-2*c->d.PageMargin;
		FrameWidth =c->d.PageWidth -2*c->d.PageMargin;
		LandscapeOrientation=0;
		break;
	case LANDSCAPEORIENTATION:
		FrameHeight=c->d.PageWidth -2*c->d.PageMargin;
		FrameWidth =c->d.PageHeight-2*c->d.PageMargin;
		LandscapeOrientation=1;
		break;
	case AUTOMATICORIENTATION:
	case MULTIPAGEORIENTATION:
		/*                                  */
		/* Calculate width-to-height ratio: */
		/*                                  */
		WHRatio=pd->fg[i].width/pd->fg[i].height;
		if(WHRatio>=1.0){
			/*            */
			/* Landscape: */
			/*            */
			FrameHeight=c->d.PageWidth -2*c->d.PageMargin;
			FrameWidth =c->d.PageHeight-2*c->d.PageMargin;
			LandscapeOrientation=1;
		}else{
			/*           */
			/* Portrait: */
			/*           */
			FrameHeight=c->d.PageHeight-2*c->d.PageMargin;
			FrameWidth =c->d.PageWidth -2*c->d.PageMargin;
			LandscapeOrientation=0;
		}
		break;
	}

	/*//////////////////////////////// */
	/*                                 */
	/* Determine if scaling is needed: */
	/*                                 */
	/*//////////////////////////////// */
	if(pd->fg[i].width >FrameWidth ) scalex=FrameWidth/pd->fg[i].width; 
	if(pd->fg[i].height>FrameHeight) scaley=FrameHeight/pd->fg[i].height;
		
	if(scalex!=1.0 || scaley!=1.0){
		/*                                                      */
		/* Preserve aspect ratio by picking smallest of the two */
		/* scaling factors:                                     */
		/*                                                      */
		scale=(scalex<scaley)?scalex:scaley;
		/*                                 */
		/* Figure scaled height and width: */
		/*                                 */
		ScaledHeight=scale*pd->fg[i].height;
		ScaledWidth =scale*pd->fg[i].width;
	}else{
		ScaledHeight=pd->fg[i].height;
		ScaledWidth =pd->fg[i].width;
	}

	/*//////////////////////////////////////////////////////// */
	/*                                                         */
	/* In automatic or multipage mode,                         */
	/* If scaling reduces drawing by too much for one page     */
	/* then figure out on how many pages to print the drawing  */
	/* instead:                                                */
	/*                                                         */
	/*//////////////////////////////////////////////////////// */
	if((c->d.Orientation==AUTOMATICORIENTATION || 
	    c->d.Orientation==MULTIPAGEORIENTATION
	   ) && scale<MAXIMUMREDUCTION
	){
		WHRatioDifference=9999.9; /* an arbitrarily huge difference */
		OptimalRows=OptimalCols=1;
		/*                             */
		/* Loop to find optimal match: */
		/*                             */
		for(rows=1;rows<5;rows++){
			for(cols=1;cols<5;cols++){
				/*                                                      */
				/* Compare ratios using the default portrait            */
				/* orientation:                                         */
				/*                                                      */
				/* Note that the printable area printed by              */
				/* the Postscript MultiplePagePrint procedure on        */
				/* each page is only the frame size, not the page size: */
				/*                                                      */
				FrameHeight   = rows*(c->d.PageHeight-2*c->d.PageMargin);
				FrameWidth    = cols*(c->d.PageWidth -2*c->d.PageMargin);
				scalex        = FrameWidth/pd->fg[i].width; 
				scaley        = FrameHeight/pd->fg[i].height;
				scale         = (scalex<scaley)?scalex:scaley;
				/*                                                         */
				/* Minimize the difference in ratio:                       */
				/*                                                         */
				/*ThisDifference=fabs(WHRatio-FrameWidth/FrameHeight);     */
				/*                                                         */
				/* Minimize the difference in ratio (difference from 1.0): */
				/*                                                         */
				ThisDifference=fabs(scale-1.0);
				if(ThisDifference<WHRatioDifference){
					WHRatioDifference=ThisDifference;
					OptimalRows         = rows;
					OptimalCols         = cols;
					OptimalFrameHeight  = FrameHeight;
					OptimalFrameWidth   = FrameWidth;
					OptimalScale        = scale;
					LandscapeOrientation= 0;
				}
				/*                                                     */
				/* Now compare ratios using a landscape orientation:   */
				/*                                                     */
				/*ThisDifference=fabs(WHRatio-FrameHeight/FrameWidth); */
				/*                                                     */
				/* Now compare scale difference using landscape        */
				/*                                                     */
				scalex        = FrameHeight/pd->fg[i].width; 
				scaley        = FrameWidth /pd->fg[i].height;
				scale         = (scalex<scaley)?scalex:scaley;
				ThisDifference=fabs(scale-1.0);
				if(ThisDifference<WHRatioDifference){
					WHRatioDifference=ThisDifference;
					OptimalRows         = rows;
					OptimalCols         = cols;
					OptimalFrameHeight  = FrameHeight;
					OptimalFrameWidth   = FrameWidth;
					OptimalScale        = scale;
					LandscapeOrientation= 1;
				}
			}
		}
		/*                                         */
		/* Figure new scaling for multipage print: */
		/*                                         */
		FrameHeight   = OptimalFrameHeight;
		FrameWidth    = OptimalFrameWidth;
		scale         = OptimalScale;
		/*                                 */
		/* Figure scaled height and width: */
		/*                                 */
		ScaledHeight=scale*pd->fg[i].height;
		ScaledWidth =scale*pd->fg[i].width;
		/*                                                    */
		/* Figure x and y offsets: For a multiple page print, */
		/* DO NOT ADD THE MARGIN OFFSETS:                     */
		/*                                                    */
		if(LandscapeOrientation){
			XOffset=FrameHeight/2.0;
			YOffset=FrameWidth;
		}else{
			XOffset=FrameWidth/2.0;
			YOffset=FrameHeight;
		}		
		MultiplePagePrint=1;
	
	}else{

		/*                                                  */
		/* XOffset and YOffset: For a single-page print, we */
		/* must ADD THE MARGIN OFFSETS:                     */
		/*                                                  */
		XOffset=c->d.PageMargin+FrameWidth/2.0;
		YOffset=c->d.PageMargin+FrameHeight;
		
		MultiplePagePrint=0;
	}

	/*/////////////////////////// */
	/*                            */
	/* Now do the actual drawing: */
	/*                            */
	/*/////////////////////////// */

	/*                                                     */
	/* Create a wrapper procedure that prints the drawing: */
	/*                                                     */
	fprintf(c->streams.prn,"/PrintDrawing {\n");
	/*                         */
	/* Rotate if in landscape: */
	/*                         */
	if(LandscapeOrientation){

		if(MultiplePagePrint){

			/*                    */
			/* MultiplePagePrint: */
			/*                    */
			fprintf(c->streams.prn,
				"0 %.2f cm translate -90 rotate\n",
				FrameHeight
			);
			
		}else{

			/*                     */
			/* Normal single page: */
			/*                     */
			fprintf(c->streams.prn,
				"0 %.2f cm translate -90 rotate\n",
				c->d.PageHeight	
			);

		}
	}	
	/*                         */
	/* X,Y offset translation: */
	/*		                       */
	fprintf(c->streams.prn,"%.2f cm %.2f cm translate\n",
		XOffset,
		YOffset
	);
	/*                   */
	/* scale if not 1.0: */
	/*                   */
	if(scale!=1.0){
		fprintf(c->streams.prn,"%.2f %.2f scale\n",scale,scale);
	}
	/*                                                     */
	/* Call the procedure already set up for this drawing: */
	/*                                                     */
	fprintf(c->streams.prn,"%s\n",ProcName);
	/*                                  */
	/* Terminate the wrapper procedure: */
	/*                                  */
	fprintf(c->streams.prn,"} def\n");
	
	/*                                           */
	/* Call wrapper procedure either directly or */
	/* using MultiplePagePrint:                  */
	/*                                           */
	if(MultiplePagePrint){
		
		fprintf(c->streams.prn,"{PrintDrawing} %i %i MultiplePagePrint\n",
			OptimalCols,
			OptimalRows		
		);
	
	}else{

		fprintf(c->streams.prn,"gsave\n");
		fprintf(c->streams.prn,"PrintDrawing\n");
		fprintf(c->streams.prn,"showpage\n");
		fprintf(c->streams.prn,"grestore\n");
	
	}
	
	/*                                           */
	/* Tell user about multiple page print jobs: */
	/*                                           */
	if(MultiplePagePrint){
		dprintf("Printing virtual %s drawing scaled to %.2lf on \n%i physical page%s wide by %i physical page%s tall.\n",
			LandscapeOrientation?"landscape":"portrait",
			scale,
			OptimalCols,
			OptimalCols==1?"":"s",
			OptimalRows,
			OptimalRows==1?"":"s"
		);
		dprintf("(You may not be able to view entire drawing in Postscript viewing application).\n");
		LogMultiplePagePrintOrder(OptimalRows,OptimalCols);
	}else{
		if(scale!=1.0) dprintf("Printing drawing scaled to %.2lf.\n\n",scale);
	}
	return scale;
}

/*                              */
/* LogMultiplePagePrintOrder(): */
/*                              */
void LogMultiplePagePrintOrder(int MaxRows,int MaxCols){

	int r,c,offset;

	dprintf("Physical page print order index:\n\n");
	for(r=MaxRows;r>=1;r--){
		offset=(r-1)*MaxCols;
		for(c=1;c<=MaxCols;c++){
			dprintf("[%1i]",c+offset);
		}
		dprintf("\n");
	}
	dprintf("\n");
}


/*                                                                 */
/* NewPrintPedigreeDrawing() rotates, translates, scales, and even */
/* prints the drawing across multiple pages, as required:          */
/* - Redesigned to handle "undivided" drawings: Use when           */
/* DividedPages is OFF                                             */
/*                                                                 */
double NewPrintPedigreeDrawing(PCONTROL c,PPEDIGREE pd){

	double scalex=1.0, scaley=1.0,scale=1.0;
	double FrameHeight,FrameWidth;
	double OptimalFrameHeight,OptimalFrameWidth;
	double OptimalScale;
	double WidthSum,HeightMax,WHRatio;
	double ScaledHeight,ScaledWidth;
	double XOffset,YOffset;
	double NewXOffset, NewYOffset;
	int rows,cols,i;
	double WHRatioDifference;
	/* double ThisWHRatio; */
	double ThisDifference;
	int OptimalRows,OptimalCols;
	int LandscapeOrientation;
	int MultiplePagePrint;
		
	/*                                                               */
	/* Figure actual width and height of all so-called "pages"       */
	/* which are V-shaped founding group tree drawings. Originally   */
	/* each tree was printed on separate pages: Now we are going to  */
	/* print them altogether on one "virtual" page (or poster) and   */
	/* use the MultiplePagePrint Postscript procedure to divy up the */
	/* pages sent to the printer.                                    */
	/*                                                               */
	/* Note that width is corrected by adding HorizontalInterval,    */
	/* and height is corrected by adding VerticalDrop1. This is      */
	/* necessary of course because the INDIVIDUAL's p->x and p->y    */
	/* are just the coordinates at the center of the square or cir-  */
	/* cular icons.                                                  */
	/*                                                               */
	WidthSum=0.0;
	HeightMax=DBL_MIN;
	for(i=0;i<pd->nfg;i++){
		pd->fg[i].width =  pd->fg[i].highx-pd->fg[i].lowx;
		pd->fg[i].width += c->d.HorizontalInterval;
		pd->fg[i].height=  pd->fg[i].highy-pd->fg[i].lowy;
		pd->fg[i].height-= c->d.VerticalDrop1;              /* VerticalDrop1 is a negative quantity                */
		pd->fg[i].height+= 2.0;                             /* Distance from top of pedigree title to first icon   */

		/* Store Cumulative width of all founding group "page" drawings: */
		WidthSum+=pd->fg[i].width;
		/* Save height of the tallest founding group "page": */
		if(pd->fg[i].height>HeightMax) HeightMax=pd->fg[i].height;
	}

	/*                                                                             */
	/* Determine and set page orientation and absolute page dimensions,            */
	/* as applicable:                                                              */
	/*                                                                             */
	switch(c->d.Orientation){
	case PORTRAITORIENTATION:
		FrameHeight=c->d.PageHeight-2*c->d.PageMargin;
		FrameWidth =c->d.PageWidth -2*c->d.PageMargin;
		LandscapeOrientation=0;
		break;
	case LANDSCAPEORIENTATION:
		FrameHeight=c->d.PageWidth -2*c->d.PageMargin;
		FrameWidth =c->d.PageHeight-2*c->d.PageMargin;
		LandscapeOrientation=1;
		break;
	case AUTOMATICORIENTATION:
	case MULTIPAGEORIENTATION:
		/*                                  */
		/* Calculate width-to-height ratio: */
		/*                                  */
		WHRatio=WidthSum/HeightMax;
		if(WHRatio>=1.0){
			/*            */
			/* Landscape: */
			/*            */
			FrameHeight=c->d.PageWidth -2*c->d.PageMargin;
			FrameWidth =c->d.PageHeight-2*c->d.PageMargin;
			LandscapeOrientation=1;
		}else{
			/*           */
			/* Portrait: */
			/*           */
			FrameHeight=c->d.PageHeight-2*c->d.PageMargin;
			FrameWidth =c->d.PageWidth -2*c->d.PageMargin;		
			LandscapeOrientation=0;
		}
		break;
	}

	/*//////////////////////////////// */
	/*                                 */
	/* Determine if scaling is needed: */
	/*                                 */
	/*//////////////////////////////// */
	if(WidthSum >FrameWidth ) scalex=FrameWidth/WidthSum; 
	if(HeightMax>FrameHeight) scaley=FrameHeight/HeightMax;
		
	if(scalex!=1.0 || scaley!=1.0){
		/*                                                      */
		/* Preserve aspect ratio by picking smallest of the two */
		/* scaling factors:                                     */
		/*                                                      */
		scale=(scalex<scaley)?scalex:scaley;
		/*                                 */
		/* Figure scaled height and width: */
		/*                                 */
		ScaledHeight=scale*HeightMax;
		ScaledWidth =scale*WidthSum;
	}else{
		ScaledHeight=HeightMax;
		ScaledWidth =WidthSum;
	}

	/*////////////////////////////////////////////////////////  */
	/*                                                          */
	/* In automatic or multipage mode (the only mode that makes */
	/* any kind of real sense for this type of drawing)         */
	/* If scaling reduces drawing by too much for one page      */
	/* then figure out on how many pages to print the drawing   */
	/* instead:                                                 */
	/*                                                          */
	/*////////////////////////////////////////////////////////  */
	if((c->d.Orientation==AUTOMATICORIENTATION || 
	    c->d.Orientation==MULTIPAGEORIENTATION
	   ) && scale<MAXIMUMREDUCTION
	){
		WHRatioDifference=9999.9; /* an arbitrarily huge difference */
		OptimalRows=OptimalCols=1;
		/*                                    */
		/* Loop to find optimal match:        */
		/* Note that we don't allow drawings  */
		/* more than 5 pages high or 5 pages  */
		/* wide.                              */
		/*                                    */
		FrameHeight   = rows*(c->d.PageHeight-2*c->d.PageMargin);
		FrameWidth    = cols*(c->d.PageWidth -2*c->d.PageMargin);
		for(rows=1;rows<5;rows++){
			for(cols=1;cols<5;cols++){
				/*                                                      */
				/* Compare ratios using the default portrait            */
				/* orientation:                                         */
				/*                                                      */
				/* Note that the printable area printed by              */
				/* the Postscript MultiplePagePrint procedure on        */
				/* each page is only the frame size, not the page size: */
				/*                                                      */
				scalex        = FrameWidth/WidthSum; 
				scaley        = FrameHeight/HeightMax;
				scale         = (scalex<scaley)?scalex:scaley;
				/*                                                         */
				/* Minimize the difference in ratio:                       */
				/*                                                         */
				ThisDifference=fabs(scale-1.0);
				if(ThisDifference<WHRatioDifference){
					WHRatioDifference=ThisDifference;
					OptimalRows         = rows;
					OptimalCols         = cols;
					OptimalFrameHeight  = FrameHeight;
					OptimalFrameWidth   = FrameWidth;
					OptimalScale        = scale;
					LandscapeOrientation= 0;
				}
				/*                                                     */
				/* Now compare ratios using a landscape orientation:   */
				/*                                                     */
				/*ThisDifference=fabs(WHRatio-FrameHeight/FrameWidth); */
				/*                                                     */
				/* Now compare scale difference using landscape        */
				/*                                                     */
				scalex        = FrameHeight/WidthSum; 
				scaley        = FrameWidth/HeightMax;
				scale         = (scalex<scaley)?scalex:scaley;
				ThisDifference=fabs(scale-1.0);
				if(ThisDifference<WHRatioDifference){
					WHRatioDifference=ThisDifference;
					OptimalRows         = rows;
					OptimalCols         = cols;
					OptimalFrameHeight  = FrameHeight;
					OptimalFrameWidth   = FrameWidth;
					OptimalScale        = scale;
					LandscapeOrientation= 1;
				}
			}
		}
		/*                                         */
		/* Figure new scaling for multipage print: */
		/*                                         */
		FrameHeight   = OptimalFrameHeight;
		FrameWidth    = OptimalFrameWidth;
		scale         = OptimalScale;
		/*                                 */
		/* Figure scaled height and width: */
		/*                                 */
		ScaledHeight=scale*HeightMax;
		ScaledWidth =scale*WidthSum;
		/*                                                    */
		/* Figure x and y offsets: For a multiple page print, */
		/* DO NOT ADD THE MARGIN OFFSETS:                     */
		/*                                                    */
		if(LandscapeOrientation){
			XOffset=FrameHeight/2.0;
			YOffset=FrameWidth;
		}else{
			XOffset=FrameWidth/2.0;
			YOffset=FrameHeight;
		}		
		MultiplePagePrint=1;
	
	}else{

		/*                                                  */
		/* XOffset and YOffset: For a single-page print, we */
		/* must ADD THE MARGIN OFFSETS:                     */
		/*                                                  */
		XOffset=c->d.PageMargin+FrameWidth/2.0;
		YOffset=c->d.PageMargin+FrameHeight;
		
		MultiplePagePrint=0;
	}

	/*/////////////////////////// */
	/*                            */
	/* Now do the actual drawing: */
	/*                            */
	/*/////////////////////////// */

	/*                                                     */
	/* Create a wrapper procedure that prints the drawing: */
	/*                                                     */
	fprintf(c->streams.prn,"/PrintDrawing {\n");
	/*                         */
	/* Rotate if in landscape: */
	/*                         */
	if(LandscapeOrientation){

		if(MultiplePagePrint){

			/*                    */
			/* MultiplePagePrint: */
			/*                    */
			fprintf(c->streams.prn,
				"0 %.2f cm translate -90 rotate\n",
				FrameHeight
			);
			
		}else{

			/*                     */
			/* Normal single page: */
			/*                     */
			fprintf(c->streams.prn,
				"0 %.2f cm translate -90 rotate\n",
				c->d.PageHeight	
			);

		}
	}



	/*                         */
	/* X,Y offset translation: */
	/*                         */
	NewXOffset=0.0;
	/* XOffset=c->d.PageMargin; */
	fprintf(c->streams.prn,"%.2f cm %.2f cm translate\n",
		NewXOffset,
		YOffset
	);

	/*                   */
	/* Scale if not 1.0: */
	/*                   */
	if(scale!=1.0){
		fprintf(c->streams.prn,"%.2f %.2f scale\n",scale,scale);
	}

	/*                                                            */
	/* Call the procedures which contain the individual drawings, */
	/* incrementing the NewXOffset before printing each one:      */
	NewXOffset=NewYOffset=0.0;
	NewXOffset+=c->d.PageMargin;
	for(i=0;i<pd->nfg;i++){

		/* Move to offset position for the ith "page" drawing: */
		if(i>0) NewXOffset=pd->fg[i-1].width/2.0;
		NewXOffset+=pd->fg[i].width/2.0;
		fprintf(c->streams.prn,"%.2f cm %.2f cm translate\n",
			NewXOffset,
			NewYOffset
		);

		/* Call the previously defined drawing procedure for  */
		/* the ith page:                                      */
		fprintf(c->streams.prn,PS_PEDIGREE_PROC_NAME_FMT,pd->fid,i);
		fprintf(c->streams.prn,"\n");
	}

	/*                                  */
	/* Terminate the wrapper procedure: */
	/*                                  */
	fprintf(c->streams.prn,"} def\n");

	/*                                           */
	/* Call wrapper procedure either directly or */
	/* using MultiplePagePrint:                  */
	/*                                           */
	if(MultiplePagePrint){

		fprintf(c->streams.prn,"{PrintDrawing} %i %i MultiplePagePrint\n",
			OptimalCols,
			OptimalRows
		);

	}else{

		fprintf(c->streams.prn,"gsave\n");
		fprintf(c->streams.prn,"PrintDrawing\n");
		fprintf(c->streams.prn,"showpage\n");
		fprintf(c->streams.prn,"grestore\n");

	}
	
	/*                                           */
	/* Tell user about multiple page print jobs: */
	/*                                           */
	if(MultiplePagePrint){
		dprintf("Printing virtual %s drawing scaled to %.2lf on \n%i physical page%s wide by %i physical page%s tall.\n",
			LandscapeOrientation?"landscape":"portrait",
			scale,
			OptimalCols,
			OptimalCols==1?"":"s",
			OptimalRows,
			OptimalRows==1?"":"s"
		);
		dprintf("(You may not be able to view entire drawing in Postscript viewing application).\n");
		LogMultiplePagePrintOrder(OptimalRows,OptimalCols);
	}else{
		if(scale!=1.0) dprintf("Printing drawing scaled to %.2lf.\n\n",scale);
	}
	return scale;
}

/* end of graphics.c */
