#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include "params.h"


/* Calculate distance and azimuth/back-azimuth between two (lon,lat) coordinates 
 * using delaz.f subroutine
 */

// function declaration
void delaz_(float *lat1, float *lon1, float *lat2, float *lon2, float *dist, float *az, float *baz);
int getcols( const char * const line, const char * const delim, char ***out_storage);
void strip(char *s);
void assign_cols_flatfile(char **columns, float *stLat, float *stLon, float *evMag, float *evLon, float *evLat, float *evDep, int *evYear, int *evMon, int *evDay, int *evHour, int *evMin, float *evSec, char *network, char * stationNm);
char * replace(char const * const original, char const * const pattern, char const * const replacement );

/*--------------------------------------------------------------------------*/
void strip(char *s)
/*--------------------------------------------------------------------------*/
{
    char *p2 = s;
    while(*s != '\0') {
        if(*s != '\t' && *s != '\n') {
            *p2++ = *s++;
        } else {
            ++s;
        }
    }
    *p2 = '\0';
}

/*--------------------------------------------------------------------------*/
int main (int argc, char *argv[])
/*--------------------------------------------------------------------------*/
{
  FILE *fpABF, *fpEAF, *fpGMM, *fpout;
  char fileABF[200], fileEAF[200], fileGMM[200], fileout[200];

/* CHECK INPUT ARGUMENTS */
  if ( argc != 5 ) {
    fprintf(stderr,"USAGE: %s [b-values, reference adjusted to Vs30=760 m/s] [EAF values] [GMPE values] [output file name]\n", argv[0]);
    fprintf(stderr,"Relocated catalog file, e.g., emm_c2_OK_KS_201702_add_all2.csv\n");
    exit(1);
  }
  sscanf(argv[1],"%s", fileABF);
  sscanf(argv[2],"%s", fileEAF);
  sscanf(argv[3],"%s", fileGMM);
  sscanf(argv[4],"%s", fileout);

// Open flatfiles
//ABF
  fprintf(stderr,"\nINPUT/OUTPUT FILES:\n");
  if ((fpABF = fopen(fileABF, "r")) == NULL) {
    fprintf(stderr,"Could not open file, %s\n", fileABF);
    exit(0);
  }
  else {
    fprintf(stderr,"Opened fileABF, %s\n", fileABF);
  }
//EAF
  if ((fpEAF = fopen(fileEAF, "r")) == NULL) {
    fprintf(stderr,"Could not open file, %s\n", fileEAF);
    exit(0);
  }
  else {
    fprintf(stderr,"Opened file, %s\n", fileEAF);
  }
//GMM
  if ((fpGMM = fopen(fileGMM, "r")) == NULL) {
    fprintf(stderr,"Could not open file, %s\n", fileGMM);
    exit(0);
  }
  else {
    fprintf(stderr,"Opened file, %s\n", fileGMM);
  }
//  output file
  fpout = fopen(fileout,"w");
  fprintf(stderr,"Writing to file, %s\n\n", fileout);


// close files
  fclose(fpABF);
  fclose(fpEAF);
  fclose(fpGMM);
  fclose(fpout);

  return 0;
}

