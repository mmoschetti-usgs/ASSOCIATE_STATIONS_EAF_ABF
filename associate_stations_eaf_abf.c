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
void write_values_EAF(char **columns_header, char **columns, float stLon, float stLat, int cols_found);


/*--------------------------------------------------------------------------*/
void remove_all_chars(char* str, char c)
/*--------------------------------------------------------------------------*/
{
    char *pr = str, *pw = str;
    while (*pr) {
        *pw = *pr++;
        pw += (*pw != c);
    }
    *pw = '\0';
}

/*--------------------------------------------------------------------------*/
void assign_cols_ABF(char **columns, float *stLon, float *stLat, float *vs30, float *amp2s, float *amp3s, float *amp5s, float *amp10s, char *stationName)
/*--------------------------------------------------------------------------*/
{
  FILE *fp;
  int cnt;
  char fileout[200], stationNameMod[50];
  char stationName1[200];

//
  *stLon=atof(columns[1]);
  *stLat=atof(columns[2]);
  *vs30=atof(columns[3]);
  *amp2s=atof(columns[9]);
  *amp3s=atof(columns[10]);
  *amp5s=atof(columns[11]);
  *amp10s=atof(columns[12]);
  sprintf(stationName1,"%s\0",columns[13]);
  stationName1[strlen(stationName1)-1]='\0';
  strcpy(stationNameMod,stationName1);
  remove_all_chars(stationNameMod,'"');
  if ( strlen(stationName1) > strlen(stationNameMod) ) {
    fprintf(stderr,"Multiple columns: %d %d\n", strlen(stationName1), strlen(stationName));
fprintf(stderr,"col13- %s END\n",columns[13]);
fprintf(stderr,"col14- %s END\n",columns[14]);
//fprintf(stderr,"col15- %s END\n",columns[15]);
    sprintf(stationNameMod,"%s %s\0",columns[13], columns[14]);
    stationNameMod[strlen(stationNameMod)-1]='\0';
    remove_all_chars(stationName,'"');
  }

// write ABF values to file--b-values with respect to GMPE (adjusted to Vs30=760 m/s)
// modify station names to remove blank space, other characters
//  fprintf(stderr,"Station Name: %s\n", stationName);
  remove_all_chars(stationNameMod,' ');
  remove_all_chars(stationNameMod,'&');
  remove_all_chars(stationNameMod,';');
  remove_all_chars(stationNameMod,'.');
  remove_all_chars(stationNameMod,'-');
  for(cnt=0; cnt<20; cnt++) {
    stationName[cnt]=stationNameMod[cnt];
  }
  fprintf(stderr,"Station Name: %s %s\n", stationName, stationNameMod);
//  fprintf(stderr,"%s_%s_%s\n", columns[13], columns[14], columns[15]);
//h
  sprintf(fileout,"AMP_FILES/amp_CS_%.3f_%.3f_%s.txt",*stLon,*stLat,stationName);
//  fprintf(stderr,"%s\n", fileout);
  system("if [ ! -d AMP_FILES ]; then mkdir AMP_FILES; fi");
//
  fp=fopen(fileout, "w");
  fprintf(fp,"2.0 %.4f\n3.0 %.4f\n5.0 %.4f\n10.0 %.4f\n", *amp2s, *amp3s, *amp5s, *amp10s);
  fclose(fp);
}

/*--------------------------------------------------------------------------*/
void assign_cols_EAF_GMM(char **columns, float *stLon, float *stLat)
/*--------------------------------------------------------------------------*/
{
  *stLon=atof(columns[1]);
  *stLat=atof(columns[2]);
//  sprintf(fileout,"AMP_FILES/amp_CS_%.3f_%.3f_%s.txt",*stLon,*stLat,stationName);
//  fprintf(stderr,"%s\n", fileout);
}


/*--------------------------------------------------------------------------*/
void write_values_GMM(char **columns_header, char **columns, float stLon, float stLat, int cols_found)
/*--------------------------------------------------------------------------*/
{
  FILE *fid;
  int cnt, cnt2, cntPer, cntMean=0, cntStd=0;
//  float meanVal[200], stdVal[200];
  float per_arr[200], meanVal_arr[200];
  char meanHeader[50]="BSSA_Fs_Vs30_best_z1_s4_";
  char colHeader1[50];
  char colHeader[50];
  char fileout[200];
  char per_string[50];

// loop over all column headers
// REMOVE
  for(cnt=0;cnt<cols_found;cnt++) {
//  for(cnt=0;cnt<15;cnt++) {
// remove double quotes
    strcpy(colHeader1,columns_header[cnt]);
    for(cnt2=1;cnt2<strlen(colHeader1)-1;cnt2++) {
      colHeader[cnt2-1]=colHeader1[cnt2];
//      fprintf(stderr,"%c %s\n", colHeader1[cnt2], colHeader);
    }
    colHeader[strlen(colHeader1)-2]='\0';
//    fprintf(stderr,"%s %s %d %s\n", colHeader1, colHeader, strncmp(meanHeader, colHeader,10), columns[cnt] );
    if ( strncmp(meanHeader, colHeader,23)==0 ) {
      cntPer=0;
      for(cnt2=24; cnt2<strlen(colHeader); cnt2++) {
        per_string[cntPer]=colHeader[cnt2];
        cntPer++;
      }
      per_string[cntPer]='\0';
//      fprintf(stderr,"Match-mean: %s %s\n", colHeader, per_string);
      per_arr[cntMean]=atof(per_string);
      meanVal_arr[cntMean]=atof(columns[cnt]);
//      fprintf(stderr,"%f %f\n", per_arr[cntMean], meanVal_arr[cntMean]);
      cntMean++;
    }
  }

// write mean/std values to file
  sprintf(fileout,"AMP_FILES/amp_BSSA_%.3f_%.3f.txt",stLon,stLat);
//  fprintf(stderr,"%s\n", fileout);
  fid=fopen(fileout,"w");
  for(cnt=0; cnt<cntMean; cnt++) {
//    fprintf(stderr,"%f %f %f %f\n", per_arr[cnt], perS_arr[cnt], meanVal_arr[cnt], stdVal_arr[cnt]);
    if ( meanVal_arr[cnt]>0 ) {
      fprintf(fid,"%f %f\n", per_arr[cnt], meanVal_arr[cnt]);
    }
  }
  fclose(fid);

//  *stLon=atof(columns[1]);

}


/*--------------------------------------------------------------------------*/
void write_values_EAF(char **columns_header, char **columns, float stLon, float stLat, int cols_found)
/*--------------------------------------------------------------------------*/
{
  FILE *fid;
  int cnt, cnt2, cntPer, cntMean=0, cntStd=0;
//  float meanVal[200], stdVal[200];
  float per_arr[200], meanVal_arr[200];
  float perS_arr[200], stdVal_arr[200];
  char meanHeader[20]="lnEAF_mean_";
  char stdHeader[20]="lnEAF_sd_";
  char colHeader1[20];
  char colHeader[20];
  char fileout[200];
  char per_string[20];

// loop over all column headers
// REMOVE
  for(cnt=0;cnt<cols_found;cnt++) {
//  for(cnt=0;cnt<15;cnt++) {
// remove double quotes
    strcpy(colHeader1,columns_header[cnt]);
    for(cnt2=1;cnt2<strlen(colHeader1)-1;cnt2++) {
      colHeader[cnt2-1]=colHeader1[cnt2];
//      fprintf(stderr,"%c %s\n", colHeader1[cnt2], colHeader);
    }
    colHeader[strlen(colHeader1)-2]='\0';
//    fprintf(stderr,"%s %s %d %s\n", colHeader1, colHeader, strncmp(meanHeader, colHeader,10), columns[cnt] );
    if ( strncmp(meanHeader, colHeader,10)==0 ) {
      cntPer=0;
      for(cnt2=11; cnt2<strlen(colHeader); cnt2++) {
        per_string[cntPer]=colHeader[cnt2];
        cntPer++;
      }
      per_string[cntPer]='\0';
//      fprintf(stderr,"Match-mean: %s %s\n", colHeader, per_string);
      per_arr[cntMean]=atof(per_string);
      meanVal_arr[cntMean]=atof(columns[cnt]);
//fprintf(stderr,"%f %f\n", per_arr[cntMean], meanVal_arr[cntMean]);
      cntMean++;
    }
    else if ( strncmp(stdHeader, colHeader,9)==0 ) {
      cntPer=0;
      for(cnt2=9; cnt2<strlen(colHeader); cnt2++) {
        per_string[cntPer]=colHeader[cnt2];
        cntPer++;
      }
      per_string[cntPer]='\0';
//      fprintf(stderr,"Match-sd: %s %s\n", colHeader, per_string);
      perS_arr[cntStd]=atof(per_string);
      stdVal_arr[cntStd]=atof(columns[cnt]);
//fprintf(stderr,"%f %f\n", perS_arr[cntStd], stdVal_arr[cntStd]);
      cntStd++;
    }
  }

// write mean/std values to file
  sprintf(fileout,"AMP_FILES/amp_EAF_%.3f_%.3f.txt",stLon,stLat);
//  fprintf(stderr,"%s\n", fileout);
  fid=fopen(fileout,"w");
  for(cnt=0; cnt<cntMean; cnt++) {
//    fprintf(stderr,"%f %f %f %f\n", per_arr[cnt], perS_arr[cnt], meanVal_arr[cnt], stdVal_arr[cnt]);
    if ( stdVal_arr[cnt]>0 ) {
//      fprintf(fid,"%f %f %f %f\n", per_arr[cnt], perS_arr[cnt], meanVal_arr[cnt], stdVal_arr[cnt]);
      fprintf(fid,"%f %f %f\n", per_arr[cnt], meanVal_arr[cnt], stdVal_arr[cnt]);
    }
  }
  fclose(fid);

//  *stLon=atof(columns[1]);

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
  char stationName[50]={0};
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
//  fpout = fopen(fileout,"w");
//  fprintf(stderr,"Writing to file, %s\n\n", fileout);

// read header lines from CyberShake ABF file
  fgets(buff,BUFFLEN,fpABF);

// loop through all sites in CyberShake ABF file
  while( fgets(buff,BUFFLEN,fpABF) ) {
    if ( strlen(buff) > BUFFLEN ) {
      fprintf(stderr,"Increase BUFFLEN from %d.\n", (int)BUFFLEN);
      exit(1);
    }
    buff[strcspn(buff, "\n")] = 0;
    columns = NULL;
    cols_found = getcols(buff, delim, &columns);
    assign_cols_ABF(columns, &stLon, &stLat, &vs30, &amp2s, &amp3s, &amp5s, &amp10s, stationName);
//    fprintf(stderr,"%f %f %f %f %f %f %f %s\n", stLon, stLat, vs30, amp2s, amp3s, amp5s, amp10s, stationName);
    free(columns);
    
//  EAF file, loop to find matching location
//  header information
    fgets(buff,BUFFLEN,fpEAF);
    buff[strcspn(buff, "\n")] = 0;
    columns_header = NULL;
    cols_found = getcols(buff, delim, &columns_header);
    while( fgets(buff,BUFFLEN,fpEAF) ) {
      strip(buff);
      columns = NULL;
      cols_found = getcols(buff, delim, &columns);
      assign_cols_EAF_GMM(columns, &lon, &lat);
      delaz_(&stLat,&stLon,&lat,&lon,&dist,&az,&baz);
      if ( fabs(stLat-lat)<0.001 && fabs(stLon-lon)<0.001 && dist<0.05 ) {
        fprintf(stderr,"Match EAF: %f %f\n", stLon, stLat);
//        fprintf(stderr,"MATCH: %f %f %f %f dist: %f\n", stLon, lon, stLat, lat, dist);
        write_values_EAF(columns_header,columns, lon, lat, cols_found);
        break;
      }
      free(columns);
    }
    rewind(fpEAF);
    free(columns_header);
    free(columns);

// GMPE amp file, find matching location/coordinate
//  header information
    fgets(buff,BUFFLEN,fpGMM);
//fprintf(stderr,"buff %s", buff);
    buff[strcspn(buff, "\n")] = 0;
    columns_header = NULL;
//fprintf(stderr,"HERE 1\n");
    cols_found = getcols(buff, delim, &columns_header);
//fprintf(stderr,"HERE 2\n");
    while( fgets(buff,BUFFLEN,fpGMM) ) {
//fprintf(stderr,"buff %s", buff);
      strip(buff);
      columns = NULL;
      cols_found = getcols(buff, delim, &columns);
//      fprintf(stderr,"Reading columns from BSSA... ");
      assign_cols_EAF_GMM(columns, &lon, &lat);
//      fprintf(stderr,"%s\n", columns); 
//      fprintf(stderr,"... Read columns from BSSA... %f %f\n", lon, lat);
      delaz_(&stLat,&stLon,&lat,&lon,&dist,&az,&baz);
      if ( fabs(stLat-lat)<0.001 && fabs(stLon-lon)<0.001 && dist<0.05 ) {
        fprintf(stderr,"Match GMM: %f %f\n", stLon, stLat);
//        fprintf(stderr,"MATCH: %f %f %f %f dist: %f\n", stLon, lon, stLat, lat, dist);
        write_values_GMM(columns_header,columns, lon, lat, cols_found);
        break;
      }
      free(columns);
    }
    rewind(fpGMM);
    free(columns_header);


// loop through EAF file and extract 
  }


// close files
  fclose(fpABF);
  fclose(fpEAF);
  fclose(fpGMM);
//  fclose(fpout);

  return 0;
}

