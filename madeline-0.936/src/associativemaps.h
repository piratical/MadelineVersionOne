
 
#ifndef ASSOCIATIVEARRAY_INCLUDED
#include "associativearrays.h"
#endif

inline int LookupIntValueFromCharacterKey(char *s, PASSOCARRAY paa);
inline int LookupIntValueFromNumericKey(double v,PASSOCARRAY paa);
int ReadCharacterAffectionStatus(char *s);
int ReadCharacterGenderStatus(char *s);
int ReadCharacterDeathStatus(char *s);
int ReadCharacterProbandStatus(char *s);
int ReadCharacterLiabilityClassStatus(char *s);
int ReadNumericAffectionStatus(double v);
int ReadNumericGenderStatus(double v);
int ReadNumericDeathStatus(double v);
int ReadNumericProbandStatus(double v);
int ReadNumericLiabilityClassStatus(double v);
void InitializeAssociativeArrayMappings(void);

#define ASSOCIATIVEMAPS_INCLUDED
