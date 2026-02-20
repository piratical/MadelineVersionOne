#include "token.h"

// DEBUG:
//#include "logs.h"

//
// Constructor
//
RECOGNIZEDTOKEN::RECOGNIZEDTOKEN(const char *key=NULL,const char *ppe=NULL,unsigned short ii=0,unsigned char tt=0,double vv=0.0,const void *pp=NULL){
	
	// DEBUG:
	//lprintf("In RECOGNIZEDTOKEN constructor for %s ... \n",key);
	
	if(key) e=strdup(key);  // key (supposed to be capitalized)
	else    e=NULL;
	if(ppe) p=strdup(ppe);   // Properized or explanatory version of the key string
	else    p=NULL;
	i=ii;            // internal representation of the token
	t=tt;            // type of the token
	v=vv;            // value of the token
	u.p=pp;          // pointer to object of type "t" cast to the evil "void *"!
	
}

//
// Copy Constructor:
//
RECOGNIZEDTOKEN::RECOGNIZEDTOKEN( const RECOGNIZEDTOKEN& toBeCopied){
	
	// DEBUG:
	//lprintf("In RECOGNIZEDTOKEN *COPY* constructor for %s ... \n",toBeCopied.e);
	
	if(toBeCopied.e) e=strdup(toBeCopied.e);
	else             e=NULL;
	if(toBeCopied.p) p=strdup(toBeCopied.p);
	else             p=NULL;
	i=toBeCopied.i;
	t=toBeCopied.t;
	v=toBeCopied.v;
	u.p=toBeCopied.u.p;
	
}

// Destructor:
RECOGNIZEDTOKEN::~RECOGNIZEDTOKEN(){

	// DEBUG:
	//lprintf("In RECOGNIZEDTOKEN destructor for %s ... \n",e);
	
	// Free the strings allocated with strdup:
	if(e) free( (void *) e );
	if(p) free( (void *) p );
	
	// Free only the user-defined objects:
	if(t==USERARRAY            && u.a  ) FreeUserArray(u.a);
	if(t==USERVARIABLE         && u.v  ) FreeUserVariable(u.v);
	if(t==USERASSOCIATIVEARRAY && u.aa ) FreeUserAssociativeArray(u.aa);
	
}
