#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "structure.h"

void initialiser_tab_int(int* a,int nb_var,int valeur){
    for (int i=0; i< nb_var; i++){
      a[i] = valeur;
    }
  }
void initialiser_tab_double(double* a,int nb_var,double valeur){
  for (int i=0; i< nb_var; i++){
    a[i] = valeur;
  }
}
void initialiser_tab_char(char* a,int nb_var,char valeur){
  for (int i=0; i<nb_var; i++){
    a[i] = valeur;
  }
}
void initialiser_matrice(double** a,int nb_cte, int nb_var,int valeur){
    for (int i=0; i<nb_cte; i++){
      for (int j=0; j<nb_var; j++){
        a[i][j] = valeur;
    }
  }
}
void initialiser_matrice_int(int** a,int nb_cte, int nb_var,int valeur){
    for (int i=0; i<nb_cte; i++){
      for (int j=0; j<nb_var; j++){
        a[i][j] = valeur;
    }
  }
}
void initialiser_tab_float(float* a,int nb_var,int valeur){
    for (int i=0; i<nb_var; i++){
      a[i] = valeur;
    }
  }
void affiche_matrice(double **A,int ligne,int colonne){
  for(int j=0;j<ligne;j++){
    printf("    ");
    for(int i=0;i<colonne;i++){
      printf("%.2f\t",A[j][i] );
    }
  	printf("\n");
  }
  printf("\n\n");
}
void affiche_matrice_fichier(FILE* output,double **A,int ligne,int colonne){
  for(int j=0;j<ligne;j++){
    fprintf(output,"    ");
    for(int i=0;i<colonne;i++){
      fprintf(output,"%.2f\t",A[j][i] );
    }
  	fprintf(output,"\n");
  }
  fprintf(output,"\n\n");
}
void affiche_matrice_int(int **A,int ligne,int colonne){
  for(int j=0;j<ligne;j++){
    printf("    ");
    for(int i=0;i<colonne;i++){
      printf("%d\t",A[j][i] );
    }
  	printf("\n");
  }
  printf("\n\n");
}
void affiche_tab_int(int *A, int colonne){
  for(int i=0;i<colonne;i++){
    printf("%d\t", A[i]);
  }
  printf("\t\t");
}
void affiche_tab_double(double *A, int colonne){
  for(int i=0;i<colonne;i++){
    printf("%.2f\t", A[i]);
  }
  printf("\n\n");
}
void affiche_tab_double_fichier(FILE * output,double *A, int colonne){
  for(int i=0;i<colonne;i++){
    fprintf(output,"%.2f\t", A[i]);
  }
  fprintf(output,"\n\n");
}
void affiche_tab_float(float *A, int colonne){
  for(int i=0;i<colonne;i++){
    printf("%6f\t", A[i]);
  }
  printf("\n\n");
}
//liberation
void free_and_null (char **ptr){
if ( *ptr != NULL ) {
free (*ptr);
*ptr = NULL;
}
}
// Fonction liberation
void liberationTabDouble (double **ptr,int n){
  if (ptr != NULL) {
    for (int  i = 0; i < n; i++) {
          free(ptr[i]);
    }
    free (ptr);
    ptr = NULL;
  }
}
void liberationTabInt (int **ptr,int n){
  if (ptr != NULL) {
    for (int  i = 0; i < n; i++) {
          free(ptr[i]);
    }
    free (ptr);
    ptr = NULL;
  }
}

//calcule de la distance
double distance (double ax, double ay, double bx, double by){
  return sqrt(((bx-ax) * (bx-ax)) + ((by-ay) * (by-ay)));
}

double minimum(double totalDemand,double capacite){
  double retour = 0 ;
  if(totalDemand <= capacite){
    retour = totalDemand ;
  }else{
    retour = capacite ;
  }
  return retour ;
}
