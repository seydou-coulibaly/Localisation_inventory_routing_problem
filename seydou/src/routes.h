#ifndef ROUTES_H
#define ROUTES_H

// void allroutes(int **routes,int **representations,double *CR,double **coordonnees, double cout_km,int nbre_routes,int n,int m);
double combinaison(int n, int p);
int zero_un(int cp,int *indice,int valeur);
void cout(int ** routes, double ** coordonnees, int ** representations, double * CR, int RMAX, int n,int m,double cout_km);
void coutPaquets(int **paquet,int p,int ** routes, double ** coordonnees, int ** representations, double * CR, int RMAX, int n,int m, double cout_km);
int routesFunction(int **distribution,int **tableAllroutes,int n,int m);

void affichage_routes(int **routes,double *CR,int **representations,int nbre_routes,int n,int m);
void affichage_paquet(int **paquet,int **routes,double *CR,int **representations,int RMAX,int n,int m,int nbPaquet,int dernierPaquet);

int solution(int *tab,int n,int **distribution,int j,int l);
int incrementer(int *tab,int **tableAllroutes,int **distribution,int n,int j,int i,int k,int cprtab);
int suivant(int *tab,int **tableAllroutes,int **distribution,int n,int j,int i,int k,int cprtab,int l);
int inserer(int *tab,int **tableAllroutes,int n,int j,int cprtab);
void init(int *tab,int **distribution,int j,int i,int k,int n);

#endif
