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
  FILT *fp;
  char fileout[200];

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

// write ABF values to file--b-values with respect to GMPE (adjusted to Vs30=760 m/s)
  sprintf(fileout,"AMP_FILES/amp_CS_%.3f_%.3f_%s.txt",*stLon,*stLat,stationName);
  fprintf(stderr,"%s\n", fileout);
  fp=fopen(fileout, "w");
  fprintf(fp,"2.0 %.3f\n3.0 %.3f\n5.0 %.3f\n10.0 %.3f\n", *amp2s, *amp3s, *amp5s, *amp10s);
  fclose(fp);
  
}

/*--------------------------------------------------------------------------*/
void assign_cols_EAF(char **columns, float *stLon, float *stLat)
/*--------------------------------------------------------------------------*/
{
  *stLon=atof(columns[1]);
  *stLat=atof(columns[2]);
//  sprintf(fileout,"AMP_FILES/amp_CS_%.3f_%.3f_%s.txt",*stLon,*stLat,stationName);
//  fprintf(stderr,"%s\n", fileout);
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
  float lon, lat, dist, az, baz;
  char fileABF[200], fileEAF[200], fileGMM[200], fileout[200];
  char buff[BUFFLEN];
  char stationName[20];
  char **columns, **columns_header;
  char **columns2;
  char delim[] = ",";


/* CHECK INPUT ARGUMENTS */
  if ( argc != 4 ) {
    fprintf(stderr,"USAGE: %s [b-values, reference adjusted to Vs30=760 m/s] [EAF values] [GMPE values]\n", argv[0]);
    fprintf(stderr,"Relocated catalog file, e.g., emm_c2_OK_KS_201702_add_all2.csv\n");
    exit(1);
  }
  sscanf(argv[1],"%s", fileABF);
  sscanf(argv[2],"%s", fileEAF);
  sscanf(argv[3],"%s", fileGMM);
//  sscanf(argv[4],"%s", fileout);

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
//    fileout=sprintf("AMP_FILES/amp_CS_%.3f_%.3f_%s.txt",stLon,stLat,stationNameMod);
    assign_cols_ABF(columns, &stLon, &stLat, &vs30, &amp2s, &amp3s, &amp5s, &amp10s, stationName);
    fprintf(stderr,"%f %f %f %f %f %f %f %s\n", stLon, stLat, vs30, amp2s, amp3s, amp5s, amp10s, stationName);
    free(columns);
    
//  loop through EAF file, find matching location
//  header information
    fgets(buff,BUFFLEN,fpEAF);
    strip(buff);
    columns_header = NULL;
    cols_found = getcols(buff, delim, &columns_header);
    while( fgets(buff,BUFFLEN,fpEAF) ) {
      strip(buff);
      columns = NULL;
      cols_found = getcols(buff, delim, &columns);
      assign_cols_EAF(columns, &lon, &lat);
      delaz_(&stLat,&stLon,&lat,&lon,&dist,&az,&baz);
      if ( fabs(stLat-lat)<0.001 && fabs(stLon-lon)<0.001 && dist<0.05 ) {
        fprintf(stderr,"MATCH: %f %f %f %f dist: %f\n", stLon, lon, stLat, lat, dist);
        write_values_EAF(columns, &lon, &lat);
        break;
      }
      free(columns);
    }
    rewind(fpEAF);
    free(columns_header);

// loop through EAF file and extract 
  }


// close files
  fclose(fpABF);
  fclose(fpEAF);
  fclose(fpGMM);
  fclose(fpout);

  return 0;
}

