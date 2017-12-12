#ifndef STRUCTURE_H
#define STRUCTURE_H

void affiche_matrice(double **A,int ligne,int colonne);
void affiche_matrice_int(int **A,int ligne,int colonne);
void affiche_tab_int(int *A, int colonne);
void affiche_tab_double(double *A, int colonne);
void affiche_tab_double_fichier(FILE * output,double *A, int colonne);
void affiche_tab_float(float *A, int colonne);
void initialiser_tab_double(double* a,int nb_var,double valeur);
void initialiser_tab_char(char* a,int nb_var,char valeur);

void initialiser_tab_int(int*a,int nb_var,int valeur);
void initialiser_tab_float(float* a,int nb_var,int valeur);
void initialiser_matrice(double** a,int nb_cte, int nb_var,int valeur);
void affiche_matrice_fichier(FILE* output,double **A,int ligne,int colonne);
void initialiser_matrice_int(int** a,int nb_cte, int nb_var,int valeur);
void free_and_null (char **ptr);
void liberationTabInt (int **ptr,int n);
void liberationTabDouble (double **ptr,int n);

double distance (double ax, double ay, double bx,double by);
double minimum(double totalDemand,double capacite);

#endif
