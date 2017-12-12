#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "structure.h"
#include "tsp.h"
#include "routes.h"

double combinaison(int n, int p){
  double numerateur=1,denominateur=1,second=1;
  double result=0;
  if (n < p) {
    return 0;
  }
  else if(n == p){
    return 1;
  }
  else{
    if (p == 0) {
      return 1;
    }
    else{
      for (int i = 1; i <= n; i++) {
        numerateur = numerateur * i;
        //printf("numerateur = %.3lf\n",numerateur);
      }
      for (int i = 1; i <= p; i++) {
        denominateur = denominateur * i;
        //printf("denominateur = %.3lf\n",denominateur);
      }
      for (int i = 1; i <= (n-p); i++) {
        second = second * i;
        //printf("second = %.3lf\n",second);
      }
      result = numerateur/(denominateur * second);
    }
  }
  return result;
}
int zero_un(int cp,int *indice,int valeur){
  int retour = 0 ;
  if (*indice < cp) {
    retour = valeur;
    *indice = *indice + 1;
  }else{
    retour = (valeur+1)%2;
    *indice = 1 ;
  }
  return retour;
}

void cout(int ** routes, double ** coordonnees, int ** representations, double * CR, int RMAX, int n,int m, double cout_km){
  double ** C = NULL ;
  double *  X = NULL ;
  int nbre_sites = n+m;
  int nbre = 0 ;
  for (int r = 0; r < RMAX; r++) {
    printf("------------------  Routes  %d  -------------------------\n",r);
    nbre = 0 ;
    //determination du nombre de sites sur cette route
    for (int j = 0; j < n; j++) {
      if (routes[r][j] == 1) {
        nbre++;
      }
    }
    //allocation table coordonnees C de taille nbre+1 pour le tsp
    C = malloc((nbre+1) * sizeof(*C));
    X = malloc((nbre+2) * sizeof(*X));
    if(C == NULL || X == NULL){
      printf("ERREUR ALLOCATION\n");
      exit(1);
    }
    for(int i=0 ; i < (nbre+1); i++){
      C[i] = malloc(2 * sizeof(**C) );
      if(C[i] == NULL){
        printf("ERREUR ALLOCATION\n");
        exit(1);
      }
    }
    //remplissage de table coordonnees C pour le tsp
    int k = 0 ;
    for (int j = 0; j < nbre_sites; j++) {
      if (routes[r][j] == 1) {
        C[k][0] = coordonnees[j][0] ;
        C[k][1] = coordonnees[j][1] ;
        k++;
      }
    }
    initialiser_tab_double(X,(nbre+2),0);
    //appeler le tsp pour le resoudre et recuperer la meilleure solution
    //affiche_matrice(C,(nbre+1),2);
    X = methodeTsp(C,nbre);
    //remplissage de cout de la route
    CR[r] = cout_km * X[0];

    //remplissage de table representations de la route
    int id = 1;
    for (int j = 0; j < nbre_sites; j++) {
      if (routes[r][j] == 1) {
        representations[r][j] = (int) X[id] ;
        id++;
      }
    }
  }
  liberationTabDouble (C,nbre+1);
  free(X);
}

int routesFunction(int **distribution,int **tableRoutes,int n,int m){
  int cprtab = 0 ;
  int * tab;
  tab = malloc(n * sizeof(*tab));
  if(tab == NULL){
    printf("ERREUR ALLOCATION\n");
    exit(1);
  }

  for (int j = 0; j < m; j++) {
    // printf("----------------------------------------\n");
    //route passant par depot j
    for (int i = 0; i < n; i++) {
      if (distribution[j][i] == 1) {
        //route passant par site i
        for (int k = 1; k <= distribution[j][n]; k++) {
          //de un client jusqu'a CMAX clients distribuable sur la route
          initialiser_tab_int(tab,n,0);
          // construire le init
          // printf("\nPour j = %d , i = %d et k = %d \n",j,i,k);
          init(tab,distribution,j,i,k,n);
          if (solution(tab,n,distribution,j,k) == 1) {
            cprtab = inserer(tab,tableRoutes,n,j,cprtab);
            cprtab = incrementer(tab,tableRoutes,distribution,n,j,i,k,cprtab);
          }
        }
      }
    }
  }
  free(tab);
  return cprtab ;
}
void affichage_routes(int **routes,double *CR,int **representations,int nbre_routes,int n,int m){
  // Ecriture dans un fichier
  FILE* fichier = NULL;
  fichier = fopen("./generer/routesGenerer", "w");
  if (fichier == NULL){
    printf("Impossible d'ouvrir le fichier");
    exit(EXIT_FAILURE);
  }
 // On peut lire et écrire dans le fichier

  for (int r = 0; r < nbre_routes; r++) {
    int ok = 0 ;
      // printf("ROUTE = %d \t",r);
    for (int j = 0; j < m; j++) {
      if (routes[r][n+j] == 1) {
        ok++ ;
        // printf("\nDepot %d\t\t",j+1);
        fprintf(fichier,"Depot %d\t\t",j+1);
      }
    }
    if (ok == 1) {
      for (int i= 0; i < n; i++) {
        // printf("%d (",routes[r][i]);
        fprintf(fichier,"%d (",routes[r][i]);
        // printf("%d)\t",representations[r][i]);
        fprintf(fichier,"%d)\t",representations[r][i]);
      }
      // printf("%.3lf\n",CR[r]);
      fprintf(fichier,"%.3lf\n",CR[r]);
    }
  }

   fclose(fichier);
}
void affichage_paquet(int **paquet,int **routes,double *CR,int **representations,int RMAX,int n,int m,int nbPaquet,int dernierPaquet){
  // Ecriture dans un fichier
  FILE* fichier = NULL;
  fichier = fopen("./generer/routesGenerer", "w");
  if (fichier == NULL){
    printf("Impossible d'ouvrir le fichier");
    exit(EXIT_FAILURE);
  }
 // On peut lire et écrire dans le fichier
 for (int p = 0; p < (nbPaquet-1); p++) {
    fprintf(fichier,"\n\n  ******************************  Paquet %d  ****************************** \n",p+1);
    // fprintf(fichier,"\nDepot %d\t\t",j+1);

    for (int r = 0; r < RMAX; r++) {
      int road = paquet[p][r] ;
      int ok = 0 ;
        // printf("ROUTE = %d \t",r);
        for (int j = 0; j < m; j++) {
          if (routes[road][n+j] == 1) {
            ok++ ;
            fprintf(fichier,"\nDepot %d\t\t",j+1);
          }
        }
        if (ok == 1) {
          for (int i= 0; i < n; i++) {
            fprintf(fichier,"%d (",routes[road][i]);
            fprintf(fichier,"%d)\t",representations[road][i]);
          }
          fprintf(fichier,"%.3lf\n",CR[road]);
        }

      }
  }

fprintf(fichier,"\n  ******************************  Paquet %d ******************************\n",nbPaquet);

for (int r = 0; r < dernierPaquet; r++) {
  int road = paquet[nbPaquet-1][r] ;
  int ok = 0 ;
    for (int j = 0; j < m; j++) {
      if (routes[road][n+j] == 1) {
        ok++;
        fprintf(fichier,"\nDepot %d\t\t",j+1);
      }
    }
    if (ok == 1) {
      for (int i= 0; i < n; i++) {
        fprintf(fichier,"%d (",routes[road][i]);
        fprintf(fichier,"%d)\t",representations[road][i]);
      }
      fprintf(fichier,"%.3lf\n",CR[road]);
    }
  }
  fclose(fichier);
}
void init(int *tab,int **distribution,int j,int i,int k,int n){
  tab[i] = 1 ;
  int nb = 1 ;
  for (int l = i+1; l < n; l++) {
    if (nb == k) {
      break ;
    }else if (distribution[j][l] == 1 && l != i) {
      tab[l] = 1 ;
      nb++;
    }
  }
}
int inserer(int *tab,int **tableRoutes,int n,int j,int cprtab){
  tableRoutes[cprtab][n+j] = 1 ;
  for (int i = 0; i < n; i++) {
      tableRoutes[cprtab][i] = tab[i] ;
  }
  cprtab = cprtab + 1;
  // affiche_tab_int(tab,n);
  // printf("\t");
  // printf("\tInsertion a la place %d\n", cprtab);
  return cprtab;
}
int suivant(int *tab,int **tableRoutes,int **distribution,int n,int j,int i,int k,int cprtab,int l){
  tab[l]   = 0 ;
  tab[l+1] = 1 ;
  if (solution(tab,n,distribution,j,k) == 1) {
    cprtab = inserer(tab,tableRoutes,n,j,cprtab);
  }
  cprtab = incrementer(tab,tableRoutes,distribution,n,j,i,k,cprtab);
  return cprtab ;
}
int incrementer(int *tab,int **tableRoutes,int **distribution,int n,int j,int i,int k,int cprtab){
  // printf("\n");
  //recuperer l'indice du dernier element à 1
  int l = n-1 ;
  while (l >= 0) {
    if (tab[l] == 1 && l != i) {
      break;
    }
    l-- ;
  }
  if (l < 0) {
    //do nothing
  }
  else if (l < n-1) {
    cprtab = suivant(tab,tableRoutes,distribution,n,j,i,k,cprtab,l);
  }
  else {  //l == (n-1)
    //recuperer le premier 1 non i
    int nb = i+1 ;
    //nb+2 <= l
    while (nb+2 <= l) {
      if (tab[nb] == 1 && nb != i) {
        break;
      }
      nb++ ;
    }
    if (nb+2 <= l) {
      tab[nb]   = 0 ;
      tab[l]    = 0 ;
      tab[nb+1] = 1 ;
      tab[nb+2] = 1 ;
      if (solution(tab,n,distribution,j,k) == 1) {
        cprtab = inserer(tab,tableRoutes,n,j,cprtab);
      }
      cprtab = incrementer(tab,tableRoutes,distribution,n,j,i,k,cprtab);
    }
  }
  return cprtab ;
}
int solution(int *tab,int n,int **distribution,int j,int l){
  int nb = 0 ;
  for (int k = 0; k < n; k++) {
    if (tab[k] == 1 && distribution[j][k] == 1) {
      nb++;
    }
  }
  if (nb == l) {
    return 1 ;
  }else{
    return 0 ;
  }
}
void coutPaquets(int **paquet,int p,int ** routes, double ** coordonnees, int ** representations, double * CR, int RMAX, int n,int m, double cout_km){
  double ** C = NULL ;
  double *  X = NULL ;
  int nbre_sites = n+m;
  int nbre = 0 ;
  for (int r = 0; r < RMAX; r++) {
    int road = paquet[p][r] ;
    printf("------------------  Routes  %d > equivaut %d  -------------------------\n",r,road);
    nbre = 0 ;
    //determination du nombre de sites sur cette route
    for (int j = 0; j < n; j++) {
      if (routes[road][j] == 1) {
        nbre++;
      }
    }
    //allocation table coordonnees C de taille nbre+1 pour le tsp
    C = malloc((nbre+1) * sizeof(*C));
    X = malloc((nbre+2) * sizeof(*X));
    if(C == NULL || X == NULL){
      printf("ERREUR ALLOCATION\n");
      exit(1);
    }
    for(int i=0 ; i < (nbre+1); i++){
      C[i] = malloc(2 * sizeof(**C) );
      if(C[i] == NULL){
        printf("ERREUR ALLOCATION\n");
        exit(1);
      }
    }
    //remplissage de table coordonnees C pour le tsp
    int k = 0 ;
    for (int j = 0; j < nbre_sites; j++) {
      if (routes[road][j] == 1) {
        C[k][0] = coordonnees[j][0] ;
        C[k][1] = coordonnees[j][1] ;
        k++;
      }
    }
    initialiser_tab_double(X,(nbre+2),0);
    //appeler le tsp pour le resoudre et recuperer la meilleure solution
    //affiche_matrice(C,(nbre+1),2);
    X = methodeTsp(C,nbre);
    //remplissage de cout de la route
    CR[road] = cout_km * X[0];

    //remplissage de table representations de la route
    int id = 1;
    for (int j = 0; j < nbre_sites; j++) {
      if (routes[road][j] == 1) {
        representations[road][j] = (int) X[id] ;
        id++;
      }
    }
  }
  liberationTabDouble (C,nbre+1);
  free(X);
}
