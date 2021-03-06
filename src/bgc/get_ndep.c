/* 
 * get_ndep.c
 * retrieve the appropriate nitrogen deposition value for the current simulation year
 * 
 * *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
 * Biome-BGC version 4.2 (final release)
 * See copyright.txt for Copyright information
 * *-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*-*
 */

#include "bgc.h"

double GetNdep (ts_struct ndep_ts, int t)
{
    double          ndep;

    IntrplForcing (ndep_ts, t, 1, &ndep);

    return (ndep);
}
