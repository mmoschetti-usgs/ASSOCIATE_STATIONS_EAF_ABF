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
void assign_cols_ABF(char **columns, float *stLon, float *stLat, float *vs30, float *amp2s, float *amp3s, float *amp5s, float *amp10s, char *stationName);
char * replace(char const * const original, char const * const pattern, char const * const replacement );

/*--------------------------------------------------------------------------*/
void assign_cols_ABF(char **columns, float *stLon, float *stLat, float *vs30, float *amp2s, float *amp3s, float *amp5s, float *amp10s, char *stationName)
/*--------------------------------------------------------------------------*/
{
//
  *stLon=atof(columns[1]);
  *stLat=atof(columns[2]);
  *vs30=atof(columns[3]);
  *amp2s=atof(columns[9]);
  *amp3s=atof(columns[10]);
  *amp5s=atof(columns[11]);
  *amp10s=atof(columns[12]);
  stationName=strcpy(stationName,columns[13]);
//  fprintf(stderr,"assign_cols_flatfile, evMag/stLat: %f %f\n", *evMag, *stLat);
//  fprintf(stderr,"assign_cols_flatfile, network/stationNm: %s %s\n", network, stationNm);

}


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
  int cols_found;
  float stLon, stLat, vs30, amp2s, amp3s, amp5s, amp10s;
  char fileABF[200], fileEAF[200], fileGMM[200], fileout[200];
  char buff[BUFFLEN];
  char stationName[20];
  char **columns;
  char **columns2;
  char delim[] = ",";


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

// read header lines from CyberShake ABF file
  fgets(buff,BUFFLEN,fpABF);

// loop through all sites in CyberShake ABF file
  while( fgets(buff,BUFFLEN,fpABF) ) {
    if ( strlen(buff) > BUFFLEN ) {
      fprintf(stderr,"Increase BUFFLEN from %d.\n", (int)BUFFLEN);
      exit(1);
    }
    strip(buff);
//    fprintf(fpFlatFileMod,"%s,",buff);
    columns = NULL;
    cols_found = getcols(buff, delim, &columns);
//    assign_cols_ABF(columns,
    assign_cols_ABF(columns, &stLon, &stLat, &vs30, &amp2s, &amp3s, &amp5s, &amp10s, stationName);
    fprintf(stderr,"%f %f %f %f %f %f %f %s\n", stLon, stLat, vs30, amp2s, amp3s, amp5s, amp10s, stationName);
// loop through EAF file and extract 
    free(columns);
  }


// close files
  fclose(fpABF);
  fclose(fpEAF);
  fclose(fpGMM);
  fclose(fpout);

  return 0;
}

