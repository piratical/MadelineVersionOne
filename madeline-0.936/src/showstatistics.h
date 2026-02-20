

#ifndef CONTROL_INCLUDED
#include "control.h"
#endif

/*                                                            */
/* Defines which tailor the summary statistics display table: */
/*                                                            */
#define OPENSTATS    0
#define GENERALSTATS 1
#define NUCLEARSTATS 2

void ShowPedigreeStatistics(PCONTROL c);
void ShowSummaryStatisticsTable(PCORECOUNTERS cc,char *msg,int flag);
void ShowNuclearStatistics(PCONTROL c);
void ShowDatabaseStatistics(PCONTROL c);

#define SHOW_STATS_INCLUDED
