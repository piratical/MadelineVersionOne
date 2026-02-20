
#include "associativemaps.h"

/*                                  */
/* LookupIntValueFromCharacterKey() */
/*                                  */
/* NOTE BENE: The associative array */
/* *must* be setup to store only    */
/* numeric values.                  */
/*                                  */
inline int LookupIntValueFromCharacterKey(char *s, PASSOCARRAY paa){
	
	VARIABLE key;
	PVARIABLE value;
	
	/* Return MISSING if the empty string was passed */
	if(!s[0]) return MISSING;
	/* Otherwise, lookup the value in the array: */
	key.t=STRINGVALUE;
	key.e.s=s;
	value=LookupValueFromKey(paa,&key);
	if(value){
		if(value->e.v==DOUBLEMISSINGVALUE) return MISSING;
		else return (int) value->e.v;
	}else{
		return MISSING;
	}
}

/*                                  */
/* LookupIntValueFromNumericKey()   */
/*                                  */
/* NOTE BENE: The associative array */
/* *must* be setup to store only    */
/* numeric values.                  */
/*                                  */
inline int LookupIntValueFromNumericKey(double v,PASSOCARRAY paa){
	
	VARIABLE key;
	PVARIABLE value;
	
	/* Return MISSING if a missing value was passed in */
	if(v==DOUBLEMISSINGVALUE) return MISSING;
	/* Otherwise, lookup the value in the array: */
	key.t=NUMERICVALUE;
	key.e.v=v;
	value=LookupValueFromKey(paa,&key);
	if(value){
		if(value->e.v==DOUBLEMISSINGVALUE) return MISSING;
		else return (int) value->e.v;
	}else{
		return MISSING;
	}
	
}


/********************************/
/* ReadCharacter...() FUNCTIONS */
/********************************/

/*                                */
/* ReadCharacterAffectionStatus() */
/*                                */
int ReadCharacterAffectionStatus(char *s){
	
	extern ASSOCARRAY aaas; /* AffectionStatus Associative Array */
	
	return LookupIntValueFromCharacterKey(s,&aaas);
	
}

/*                                */
/* ReadCharacterGenderStatus()    */
/*                                */
int ReadCharacterGenderStatus(char *s){
	
	extern ASSOCARRAY aags; /* GenderStatus Associative Array */
	
	return LookupIntValueFromCharacterKey(s,&aags);
	
}

/*                                */
/* ReadCharacterDeathStatus()     */
/*                                */
int ReadCharacterDeathStatus(char *s){
	
	extern ASSOCARRAY aads; /* DeathStatus Associative Array */
	
	return LookupIntValueFromCharacterKey(s,&aads);
	
}

/*                                */
/* ReadCharacterProbandStatus()   */
/*                                */
int ReadCharacterProbandStatus(char *s){
	
	extern ASSOCARRAY aaps; /* ProbandStatus Associative Array */
	
	return LookupIntValueFromCharacterKey(s,&aaps);
	
}

/*                                       */
/* ReadCharacterLiabilityClassStatus()   */
/*                                       */
int ReadCharacterLiabilityClassStatus(char *s){
	
	extern ASSOCARRAY aalc; /* LiabilityClassStatus Associative Array */
	
	return LookupIntValueFromCharacterKey(s,&aalc);
	
}


/********************************/
/* ReadNumeric...() FUNCTIONS   */
/********************************/

/*                              */
/* ReadNumericAffectionStatus() */
/*                              */
int ReadNumericAffectionStatus(double v){
	
	extern ASSOCARRAY aaas;
	
	return LookupIntValueFromNumericKey(v,&aaas);
	
}

/*                              */
/* ReadNumericGenderStatus() */
/*                              */
int ReadNumericGenderStatus(double v){
	
	extern ASSOCARRAY aags;
	
	return LookupIntValueFromNumericKey(v,&aags);
	
}

/*                              */
/* ReadNumericDeathStatus()     */
/*                              */
int ReadNumericDeathStatus(double v){
	
	extern ASSOCARRAY aads;
	
	return LookupIntValueFromNumericKey(v,&aads);
	
}

/*                              */
/* ReadNumericProbandStatus() */
/*                              */
int ReadNumericProbandStatus(double v){
	
	extern ASSOCARRAY aaps;
	
	return LookupIntValueFromNumericKey(v,&aaps);
	
}

/*                                   */
/* ReadNumericLiabilityClassStatus() */
/*                                   */
int ReadNumericLiabilityClassStatus(double v){
	
	extern ASSOCARRAY aalc;
	
	return LookupIntValueFromNumericKey(v,&aalc);
	
}

/*                                      */
/* InitializeAssociativeArrayMappings() */
/*                                      */
void InitializeAssociativeArrayMappings(void){
	
	extern ASSOCARRAY aaas,aags,aads,aaps;
	
	VARIABLE key,value;
	
	/*                                  */
	/* Default AFFECTIONSTATUS Mappings */
	/*                                  */
	/* AFFECTED */
	key.t=STRINGVALUE;
	key.e.s="A";
	value.t=NUMERICVALUE;
	value.e.v=1.0;
	AssociativeArrayAddKeyValuePair(&aaas,&key,&value);
	/* UNAFFECTED */
	key.e.s="U";
	value.e.v=0.0;
	AssociativeArrayAddKeyValuePair(&aaas,&key,&value);
	/* INDETERMINATE/MISSING */
	key.e.s="I";
	value.e.v=DOUBLEMISSINGVALUE;
	AssociativeArrayAddKeyValuePair(&aaas,&key,&value);
	/* NUMERIC LINKAGE/GENEHUNTER AFFECTED */
	key.t=NUMERICVALUE;
	key.e.v=2.0;
	value.e.v=1.0;
	AssociativeArrayAddKeyValuePair(&aaas,&key,&value);
	/* NUMERIC LINKAGE/GENEHUNTER UNAFFECTED */
	key.e.v=1.0;
	value.e.v=0.0;
	AssociativeArrayAddKeyValuePair(&aaas,&key,&value);
	/* NUMERIC LINKAGE/GENEHUNTER INDETERMINATE/MISSING */
	key.e.v=0.0;
	value.e.v=DOUBLEMISSINGVALUE;
	AssociativeArrayAddKeyValuePair(&aaas,&key,&value);
	
	/*                                  */
	/* Default GENDERSTATUS Mappings    */
	/*                                  */
	/* FEMALE                           */
	key.t=STRINGVALUE;
	key.e.s="F";
	value.t=NUMERICVALUE;
	value.e.v=1.0;
	AssociativeArrayAddKeyValuePair(&aags,&key,&value);
	/* MALE */
	key.e.s="M";
	value.e.v=0.0;
	AssociativeArrayAddKeyValuePair(&aags,&key,&value);
	/* UNICODE FEMALE SIGN FOR ANIMAL STUDIES */
	key.t=STRINGVALUE;
	key.e.s="♀";
	value.t=NUMERICVALUE;
	value.e.v=1.0;
	AssociativeArrayAddKeyValuePair(&aags,&key,&value);
	/* UNICODE MALE SIGN FOR ANIMAL STUDIES */
	key.e.s="♂";
	value.e.v=0.0;
	AssociativeArrayAddKeyValuePair(&aags,&key,&value);

	/* NUMERIC LINKAGE/GENEHUNTER FEMALE */
	key.t=NUMERICVALUE;
	key.e.v=2.0;
	value.e.v=1.0;
	AssociativeArrayAddKeyValuePair(&aags,&key,&value);
	/* NUMERIC LINKAGE/GENEHUNTER FEMALE */
	key.e.v=1.0;
	value.e.v=0.0;
	AssociativeArrayAddKeyValuePair(&aags,&key,&value);
	
	/*                                  */
	/* Default DEATHSTATUS Mappings     */
	/*                                  */
	/* IS DECEASED                      */
	key.t=STRINGVALUE;
	key.e.s="Y";
	value.t=NUMERICVALUE;
	value.e.v=1.0;
	AssociativeArrayAddKeyValuePair(&aads,&key,&value);
	/* ALIVE = NOT DECEASED */
	key.e.s="N";
	value.e.v=0.0;
	AssociativeArrayAddKeyValuePair(&aads,&key,&value);
	/* NUMERIC IS DECEASED */
	key.t=NUMERICVALUE;
	key.e.v=1.0;
	value.e.v=1.0;
	AssociativeArrayAddKeyValuePair(&aads,&key,&value);
	/* NUMERIC ALIVE = NOT DECEASED */
	key.e.v=0.0;
	value.e.v=0.0;
	AssociativeArrayAddKeyValuePair(&aads,&key,&value);
	
	/*                                  */
	/* Default PROBANDSTATUS Mappings   */
	/*                                  */
	/* IS PROBAND                       */
	key.t=STRINGVALUE;
	key.e.s="Y";
	value.t=NUMERICVALUE;
	value.e.v=1.0;
	AssociativeArrayAddKeyValuePair(&aaps,&key,&value);
	/* NOT PROBAND */
	key.e.s="N";
	value.e.v=0.0;
	AssociativeArrayAddKeyValuePair(&aaps,&key,&value);
	/* NUMERIC IS PROBAND */
	key.t=NUMERICVALUE;
	key.e.v=1.0;
	value.e.v=1.0;
	AssociativeArrayAddKeyValuePair(&aaps,&key,&value);
	/* NUMERIC NOT PROBAND */
	key.e.v=0.0;
	value.e.v=0.0;
	AssociativeArrayAddKeyValuePair(&aaps,&key,&value);
	
	
}
