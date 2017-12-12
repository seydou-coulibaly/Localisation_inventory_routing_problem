#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <ilcplex/cplex.h>
#include "structure.h"
#include "routes.h"
#include "model.h"
#include "tsp.h"


/*------------------------------------------------------------
#  Prototype
--------------------------------------------------------------
*/
// void liberation (int **X,int n);

/*------------------------------------------------------------
#  Main
--------------------------------------------------------------
*/

int main(int argc, char const *argv[]) {
  clock_t tic = clock();
  int n=0,m=0,p=0,v_max=0,nbre_sites=0,nbre_routes=0;
  double capacite_vehicule,cout_vehicule,cout_km;
  FILE* fichier = NULL;
  if (argc < 3) {
    printf("ARGUMENT\n");
    exit(1);
  }

  if (strcmp(argv[2], "random") != 0 && strcmp(argv[2], "short") != 0 &&  strcmp(argv[2], "long") != 0 &&  strcmp(argv[2], "paquet") != 0 ) {
    printf("CHOIX DE GENERATION DE ROUTE INVALIDE\n");
    exit(1);
  }

  //sinon aucun problème avec les paramètres

  int CMAX ;
  int RMAX ;
  double DMAX ;
  double LMAX ;

  double *open;
  double *order;
  double *inventory;
  double *inventory_capacity;
  double **demande;
  double **holding;
  double *localisation;
  double ** coordonnees;

  int ** tableRoutes;
  int ** distribution;
  int ** routes = NULL;
  int ** paquet = NULL;
  int ** representations;
  double *CR = NULL;

  // Lmax = longueur maximale de la route (= somme des distances entre clients de la route)
  // Cmax = nombre maximal de clients sur la route
  // Dmax = distance maximale entre le dépot et les clients de la route
  // Rmax = Nombre de routes à créer
  // Ouverture du fichier d'entree/sortie

  //La taille du chemin vaut au plus 250 caractères
  char chemin[250] = "../instances/";
  strcat(chemin, argv[1]);
  strcat(chemin, ".dat");
  fichier   = fopen(chemin,"r");
  if(fichier == NULL){
    printf("Impossible d'ouvrir le fichier d'instances");
    exit(1);
  }

  // Ecriture dans un fichier
  FILE* output = NULL;
  output = fopen("./generer/instances", "w");
  if (output == NULL){
    printf("Impossible d'ouvrir le fichier");
    exit(EXIT_FAILURE);
  }


  fprintf(output,"\n =========================== INSTANCES  %s =========================== \n",chemin);
  // lecture nombre de clients
  fscanf(fichier, "%d", &n);
  // lecture nombre de sites ou de depôts
  fscanf(fichier, "%d", &m);
  // lecture nombre de periodes
  fscanf(fichier, "%d", &p);
  // lecture nombre de vehicules dispo
  fscanf(fichier, "%d", &v_max);
  // lecture capacite de vehicules
  fscanf(fichier, "%lf", &capacite_vehicule);
  // lecture cout vehicule
  fscanf(fichier, "%lf", &cout_vehicule);
  // lecture cout par km
  fscanf(fichier, "%lf", &cout_km);
  // nombre de sites
  nbre_sites = n + m ;
  /**
  PARAMETRE SUR LA ROUTE
  */
  fscanf(fichier, "%d", &CMAX);
  fscanf(fichier, "%d", &RMAX);
  fscanf(fichier, "%lf", &DMAX);
  fscanf(fichier, "%lf", &LMAX);

  //allocations de memoires pour nos tableaux
  open = malloc(m * sizeof(*open));
  order = malloc(m * sizeof(*order));
  inventory = malloc(nbre_sites * sizeof(*inventory));
  inventory_capacity = malloc(nbre_sites * sizeof(*inventory_capacity));
  localisation = malloc(m * sizeof(*localisation));
  demande = malloc(n * sizeof(*demande));
  holding = malloc(nbre_sites * sizeof(*holding));
  coordonnees = malloc((nbre_sites + 1) * sizeof(*coordonnees));

  if(open==NULL || order==NULL || inventory==NULL || inventory_capacity==NULL || demande==NULL || holding==NULL || localisation==NULL || coordonnees==NULL){
    printf("ERREUR ALLOCATION\n");
    exit(1);
  }
  //demande
  for(int i=0 ; i < n; i++){
    demande[i] = malloc(p * sizeof(**demande) );
    if(demande[i] == NULL){
      printf("ERREUR ALLOCATION\n");
      exit(1);
    }
  }
  //holding
  for(int i=0 ; i < nbre_sites; i++){
    holding[i] = malloc(p * sizeof(**holding) );
    if(holding[i] == NULL){
      printf("ERREUR ALLOCATION\n");
      exit(1);
    }
  }
  //coordonnees
  for(int i=0 ; i < (nbre_sites+1); i++){
    coordonnees[i] = malloc(2 * sizeof(**coordonnees) );
    if(coordonnees[i] == NULL){
      printf("ERREUR ALLOCATION\n");
      exit(1);
    }
  }
  /**
  * Initialisation des tableaux
  */

  initialiser_tab_double(open,m,0);
  initialiser_tab_double(order,m,0);
  initialiser_tab_double(inventory,nbre_sites,0);
  initialiser_tab_double(inventory_capacity,nbre_sites,0);
  initialiser_tab_double(localisation,m,0);
  initialiser_matrice(demande,n,p,0);
  initialiser_matrice(holding,nbre_sites,p,0);
  initialiser_matrice(coordonnees,(nbre_sites+1),2,0);
  /**
  * remplissage des tableaux
  */

  /* lecture vecteur couts d'ouverture de depots */
  for (int k=0; k< m; k++){
    fscanf(fichier,"%lf",&open[k]);
  }
  /* lecture vecteur couts de commande de depots */
  for (int k=0; k< m; k++){
    fscanf(fichier,"%lf",&order[k]);
  }
  /* lecture vecteur inventory des sites */
  for (int i=0; i< nbre_sites; i++){
    fscanf(fichier,"%lf",&inventory[i]);
  }
  /* lecture vecteur inventory_capacity des sites */
  for (int i=0; i< nbre_sites; i++){
    fscanf(fichier,"%lf",&inventory_capacity[i]);
  }
  /* lecture matrice demande d'ouverture de sites */
  for (int i=0; i< n; i++){
    for (int j=0; j< p; j++){
      fscanf(fichier,"%lf",&demande[i][j]);
    }
  }
  /* lecture matrice holding cost */
  for (int i=0; i< nbre_sites; i++){
    for (int j=0; j< p; j++){
      fscanf(fichier,"%lf",&holding[i][j]);
    }
  }
  /* lecture matrice coordonnees */
  for (int i=0; i< (nbre_sites + 1); i++){
    for (int j=0; j< 2; j++){
      fscanf(fichier,"%lf",&coordonnees[i][j]);
    }
  }
  //La table localisation
  for (int i = 0; i < m; i++) {
    localisation[i] = distance (coordonnees[(n + i)][0], coordonnees[(n + i)][1], coordonnees[nbre_sites][0], coordonnees[nbre_sites][1]);
  }

  /**
  Generation des routes
  1) Random
  2) routes plus courtes
  3) routes plus longues
  */
  //calcule du nombre de routes possibles
  // for (int i = 1; i <= n; i++) {
  //   nbre_routes = nbre_routes + (int) combinaison(n,i);
  // }
  // //on multiplie par le nombre de sites possibles
  // nbre_routes = m * nbre_routes;
  //La table distribution
  int dernierPaquet = 0 ;
  int nbPaquet = 0 ;

  distribution = malloc( m * sizeof(*distribution));
  if(distribution==NULL){
    printf("ERREUR ALLOCATION\n");
    exit(1);
  }
  for(int i=0 ; i < m; i++){
    distribution[i] = malloc((n+1) * sizeof(**distribution) );
    if(distribution[i] == NULL){
      printf("ERREUR ALLOCATION\n");
      exit(1);
    }
  }
  initialiser_matrice_int(distribution,m,(n+1),0);
  //remplissage de la table distribution
  for (int j = 0; j < m; j++) {
    int nbre = 0 ;
    for (int i = 0; i < n; i++) {
      if (distance (coordonnees[j][0], coordonnees[j][1], coordonnees[i][0],coordonnees[i][1]) >= DMAX) {
        distribution[j][i] = 0 ;
      }else{
        distribution[j][i] = 1 ;
        nbre ++ ;
      }
    }
    if (nbre > CMAX) {
      nbre = CMAX ;
    }
    distribution[j][n] = nbre ;
  }

  // printf("distribution\n");
  // affiche_matrice_int(distribution,m,(n+1));

  //creation d'une table englobant toutes les routes
  int somme = 0 ;
  for (int j = 0; j < m; j++) {
    int test = 0 ;
    for (int i = 1; i <= distribution[j][n]; i++) {
      test = test + (int) combinaison(n,i);
    }
    somme = somme + test ;
  }
  nbre_routes = somme ;
  // printf("TAILLE = %d\n",nbre_routes);
  tableRoutes  = malloc( nbre_routes * sizeof(*tableRoutes));
  if(tableRoutes==NULL){
    printf("ERREUR ALLOCATION\n");
    exit(1);
  }
  //tableRoutes
  for(int i=0 ; i < somme; i++){
    tableRoutes[i] = malloc((nbre_sites+1) * sizeof(**tableRoutes) );
    if(tableRoutes[i] == NULL){
      printf("ERREUR ALLOCATION\n");
      exit(1);
    }
  }
  initialiser_matrice_int(tableRoutes,somme,(nbre_sites+1),0);
  nbre_routes = routesFunction(distribution,tableRoutes,n,m);
  // printf("TAILLE = %d\n",nbre_routes);
  // printf("TAILLE2 = %d\n",somme);
  // printf("Routes Globales\n");
  // affiche_matrice_int(tableRoutes,nbre_routes,nbre_sites+1);

  if ( RMAX >= nbre_routes) {
    //garder toutes les routes
    routes          = malloc( nbre_routes * sizeof(*routes));
    representations = malloc( nbre_routes * sizeof(*representations));
    CR              = malloc( nbre_routes * sizeof(*CR));
    if(CR==NULL || representations==NULL || routes==NULL){
      printf("ERREUR ALLOCATION\n");
      exit(1);
    }
    //routes et representations
    for(int i=0 ; i < nbre_routes; i++){
      routes[i] = malloc(nbre_sites * sizeof(**routes) );
      representations[i] = malloc(nbre_sites * sizeof(**representations) );
      if(routes[i] == NULL || representations[i] == NULL){
        printf("ERREUR ALLOCATION\n");
        exit(1);
      }
    }
    // initialiser_matrice_int(routes,nbre_routes,nbre_sites,0);
    for (int r = 0; r < nbre_routes; r++) {
      for (int i = 0; i < nbre_sites; i++) {
        routes[r][i] = tableRoutes[r][i];
      }
    }
    initialiser_matrice_int(representations,nbre_routes,nbre_sites,0);
    initialiser_tab_double(CR,nbre_routes,0);

    // Calcule su cout optimale pour chaque route
    cout(routes,coordonnees,representations,CR,nbre_routes,n,m,cout_km);

  }else{
    srand(time(NULL));

    //garder RMAX routes

    /*
    ROUTE par paquet
    */

    if (strcmp(argv[2], "paquet") == 0) {
      int reste = nbre_routes % RMAX ;
      if (reste == 0) {
        nbPaquet = nbre_routes/RMAX ;
      }else{
        nbPaquet = (int) (nbre_routes/RMAX) + 1 ;
      }
      // printf("Le nombre de paquets = %d\n",nbPaquet);
      paquet       = malloc( nbPaquet*sizeof(*routes));
      if(paquet == NULL){
        printf("ERREUR ALLOCATION\n");
        exit(1);
      }
      for(int i=0 ; i < (nbPaquet-1); i++){
        paquet[i] = malloc(RMAX * sizeof(**routes) );
        if(paquet[i] == NULL){
          printf("ERREUR ALLOCATION\n");
          exit(1);
        }
      }
      //le dernier paquet
      dernierPaquet = nbre_routes - RMAX*(nbPaquet-1) ;
      paquet[nbPaquet-1] = malloc(dernierPaquet * sizeof(**routes) );
      // printf("Dernier paquet = %d\n",dernierPaquet);

      //initialisation
      initialiser_matrice_int(paquet,(nbPaquet-1),RMAX,0);
      for (int i = 0; i < dernierPaquet; i++) {
        paquet[nbPaquet-1][i] = 0 ;
      }

      //Remplissage
      int nb = 0 ;
      for (int p = 0; p < (nbPaquet-1); p++) {
        nb = 0 ;
        while (nb < RMAX) {
          // printf("\np = %d et nb = %d \t",p,nb);
          int generer = rand() % nbre_routes ;
          // printf("\n%d\n",generer);
          if (tableRoutes[generer][nbre_sites] == 0) {
            //sauvegarder son indice
            paquet[p][nb] = generer ;
            // printf("generer = %d",generer);
            nb++;
            tableRoutes[generer][nbre_sites] = 1 ;
          }
        }
      }

      //le dernier paquet se lance dans le remplissage de taille dernierPaquet
      nb = 0 ;
      int rcompteur = 0 ;
      // printf("\n nb = %d et dernierPaquet = %d\n",nb,dernierPaquet);
      while (nb != dernierPaquet) {
        // printf("compteur = %d\n",rcompteur);
        if (tableRoutes[rcompteur][nbre_sites] == 0) {
          paquet[nbPaquet-1][nb] = rcompteur ;
          nb++ ;
        }
        rcompteur++ ;
      }

      //affichage des paquets
      // printf("MATRICE PAQUETS\n");
      // affiche_matrice_int(paquet,(nbPaquet-1),RMAX);
      // printf("\n");
      // printf("Le dernier paquet de taille %d \n", dernierPaquet);
      // for (int i = 0; i < dernierPaquet; i++) {
      //   printf("%d\t",paquet[nbPaquet-1][i]);
      // }

      representations = malloc( nbre_routes * sizeof(*representations));
      CR              = malloc( nbre_routes * sizeof(*CR));
      if(CR==NULL || representations==NULL){
        printf("ERREUR ALLOCATION\n");
        exit(1);
      }
      //CR et representations
      for(int i=0 ; i < nbre_routes; i++){
        representations[i] = malloc(nbre_sites * sizeof(**representations) );
        if(representations[i] == NULL){
          printf("ERREUR ALLOCATION\n");
          exit(1);
        }
      }
      initialiser_matrice_int(representations,nbre_routes,nbre_sites,0);
      initialiser_tab_double(CR,nbre_routes,0);

      //calcul des couts des paquets
      for (int p = 0; p < (nbPaquet-1); p++) {
        coutPaquets(paquet,p,tableRoutes,coordonnees,representations,CR,RMAX,n,m,cout_km);
      }

      //Cout du dernier paquet
      coutPaquets(paquet,nbPaquet-1,tableRoutes,coordonnees,representations,CR,dernierPaquet,n,m,cout_km);

      //eliminer les routes dont le cout est > à LMAX

      for (int p = 0; p < (nbPaquet-1); p++) {
        for (int r = 0; r < RMAX; r++) {
          int road = paquet[p][r] ;
          if(CR[road] > LMAX){
            for (int i = 0; i < nbre_sites; i++) {
              tableRoutes[road][i] = 0 ;
              CR[road] = 0 ;
            }
          }
        }
      }
      //dernier paquet
      for (int r = 0; r < dernierPaquet; r++) {
        int road = paquet[nbPaquet-1][r] ;
        if(CR[road] > LMAX){
          for (int i = 0; i < nbre_sites; i++) {
            tableRoutes[road][i] = 0 ;
            CR[road] = 0 ;
          }
        }
      }


    }else{

      int tg  = nbre_routes ;
      nbre_routes = RMAX ;
      routes          = malloc( RMAX * sizeof(*routes));
      representations = malloc( RMAX * sizeof(*representations));
      CR              = malloc( RMAX * sizeof(*CR));
      if(CR==NULL || representations==NULL || routes==NULL){
        printf("ERREUR ALLOCATION\n");
        exit(1);
      }
      //routes et representations
      for(int i=0 ; i < RMAX; i++){
        routes[i] = malloc(nbre_sites * sizeof(**routes) );
        representations[i] = malloc(nbre_sites * sizeof(**representations) );
        if(routes[i] == NULL || representations[i] == NULL){
          printf("ERREUR ALLOCATION\n");
          exit(1);
        }
      }
      initialiser_matrice_int(routes,RMAX,nbre_sites,0);
      initialiser_matrice_int(representations,RMAX,nbre_sites,0);
      initialiser_tab_double(CR,RMAX,0);

      /*
      ROUTE RANDOM
      */

      if (strcmp(argv[2], "random") == 0){
        int generer;
        int nb=0;
        // printf("Generation RMAX = %d \n",tg);
        do {
          generer = rand() % tg ;
          // printf("route %d\n",generer);
          if (tableRoutes[generer][nbre_sites] == 0) {
            for (int i = 0; i < nbre_sites; i++) {
              routes[nb][i] = tableRoutes[generer][i] ;
            }
            nb++;
            tableRoutes[generer][nbre_sites] = 1 ;
          }
        } while(nb < RMAX);

        // Calcule su cout optimale pour chaque route
        cout(routes,coordonnees,representations,CR,nbre_routes,n,m,cout_km);

        for (int r = 0; r < nbre_routes; r++) {
          if(CR[r] > LMAX){
            for (int i = 0; i < nbre_sites; i++) {
              routes[r][i] = 0 ;
              CR[r] = 0 ;
            }
          }
        }

      }

      /*
      ROUTE COURTE
      */

      if (strcmp(argv[2], "short") == 0) {

        //Remplissage de la dernière table de tableRoutes
        for (int r = 0; r < tg; r++) {
          int nb = 0 ;
          for (int i = 0; i < n; i++) {
            nb = nb + tableRoutes[r][i] ;
          }
          tableRoutes[r][nbre_sites] = nb ;
        }

        // printf("Routes Globales\n");
        // affiche_matrice_int(tableRoutes,nbre_routes,nbre_sites+1);
        // printf("\n");

        // trie de la tableRoutes
        for (int r1 = 0; r1 < tg-1; r1++) {
          for (int r2 = r1; r2 < tg; r2++) {
            if (tableRoutes[r1][nbre_sites] > tableRoutes[r2][nbre_sites]) {
              // printf("echange %d et %d  \n",r1,r2);
              int tmp = 0 ;
              for (int i = 0; i < (nbre_sites+1); i++) {
                // printf("r1 = %d et r2 = %d\t",tableRoutes[r1][i],tableRoutes[r2][i]);
                tmp = tableRoutes[r2][i] ;
                tableRoutes[r2][i] = tableRoutes[r1][i] ;
                tableRoutes[r1][i] = tmp ;
                // printf("final r1 = %d et r2 = %d\n",tableRoutes[r1][i],tableRoutes[r2][i]);
              }
            }
          }
        }

        // printf("Routes Globales\n");
        // affiche_matrice_int(tableRoutes,nbre_routes,nbre_sites+1);
        // printf("\n");


        for (int r = 0; r < nbre_routes; r++) {
          for (int i = 0; i < nbre_sites; i++) {
            routes[r][i] = tableRoutes[r][i] ;
          }
        }

        // Calcule su cout optimale pour chaque route
        cout(routes,coordonnees,representations,CR,nbre_routes,n,m,cout_km);

        for (int r = 0; r < nbre_routes; r++) {
          if(CR[r] > LMAX){
            for (int i = 0; i < nbre_sites; i++) {
              routes[r][i] = 0 ;
              CR[r] = 0 ;
            }
          }
        }

      }

      /*
      ROUTE longue
      */

      if (strcmp(argv[2], "long") == 0) {

        //Remplissage de la dernière table de tableRoutes
        for (int r = 0; r < tg; r++) {
          int nb = 0 ;
          for (int i = 0; i < n; i++) {
            nb = nb + tableRoutes[r][i] ;
          }
          tableRoutes[r][nbre_sites] = nb ;
        }

        // printf("Routes Globales\n");
        // affiche_matrice_int(tableRoutes,nbre_routes,nbre_sites+1);
        // printf("\n");

        // trie de la tableRoutes
        for (int r1 = 0; r1 < tg-1; r1++) {
          for (int r2 = r1; r2 < tg; r2++) {
            if (tableRoutes[r1][nbre_sites] < tableRoutes[r2][nbre_sites]) {
              // printf("echange %d et %d  \n",r1,r2);
              int tmp = 0 ;
              for (int i = 0; i < (nbre_sites+1); i++) {
                // printf("r1 = %d et r2 = %d\t",tableRoutes[r1][i],tableRoutes[r2][i]);
                tmp = tableRoutes[r2][i] ;
                tableRoutes[r2][i] = tableRoutes[r1][i] ;
                tableRoutes[r1][i] = tmp ;
                // printf("final r1 = %d et r2 = %d\n",tableRoutes[r1][i],tableRoutes[r2][i]);
              }
            }
          }
        }

        // printf("Routes Globales\n");
        // affiche_matrice_int(tableRoutes,nbre_routes,nbre_sites+1);
        // printf("\n");


        for (int r = 0; r < nbre_routes; r++) {
          for (int i = 0; i < nbre_sites; i++) {
            routes[r][i] = tableRoutes[r][i] ;
          }
        }

        // Calcule su cout optimale pour chaque route
        cout(routes,coordonnees,representations,CR,nbre_routes,n,m,cout_km);

        for (int r = 0; r < nbre_routes; r++) {
          if(CR[r] > LMAX){
            for (int i = 0; i < nbre_sites; i++) {
              routes[r][i] = 0 ;
              CR[r] = 0 ;
            }
          }
        }

      }

    }

  }





  fprintf(output,"\n");
  fprintf(output,"\nOpen\n");
  affiche_tab_double_fichier(output,open,m);
  fprintf(output,"\nOrder\n");
  affiche_tab_double_fichier(output,order,m);
  fprintf(output,"\nInventory\n");
  affiche_tab_double_fichier(output,inventory,nbre_sites);
  fprintf(output,"\nInventory capacity\n");
  affiche_tab_double_fichier(output,inventory_capacity,nbre_sites);
  fprintf(output,"\nHolding\n");
  affiche_matrice_fichier(output,holding,nbre_sites,p);
  fprintf(output,"Demande\n");
  affiche_matrice_fichier(output,demande,n,p);
  fprintf(output,"\n");
  fprintf(output,"Coordonnees\n");
  affiche_matrice_fichier(output,coordonnees,(nbre_sites+1),2);

  // fprintf(output,"Localisation\n");
  // affiche_tab_double_fichier(output,localisation,m);

  // printf("Routes\n");
  // affiche_matrice_int(routes,nbre_routes,nbre_sites);
  // printf("Cout Routes\n");
  // affiche_tab_double(CR,nbre_routes);
  // printf("representations\n");
  // affiche_matrice_int(representations,nbre_routes,nbre_sites);
  if (strcmp(argv[2], "paquet") != 0) {
    affichage_routes(routes,CR,representations,nbre_routes,n,m);
  }else{
      //affichage paquets
      affichage_paquet(paquet,tableRoutes,CR,representations,RMAX,n,m,nbPaquet,dernierPaquet);
  }




  fclose(fichier);
  //         =============================  DEBUT EXECUTION PROGRAMME =====================================

  fprintf(output,"\nle nbre de client %d\n",n);
  fprintf(output,"le nbre de depots %d\n",m);
  fprintf(output,"le nbre de periodes %d\n",p);
  fprintf(output,"le nbre de v_max %d\n",v_max);
  fprintf(output,"la capacite du vehicule %.2f\n",capacite_vehicule);
  fprintf(output,"le cout du vehicules %.2f\n",cout_vehicule);
  fprintf(output,"le cout par km %.2f\n",cout_km);
  fprintf(output,"le nbre de sites %d\n",nbre_sites);
  fprintf(output,"le nbre de routes %d\n\n",nbre_routes);

  fprintf(output,"CMAX %d\n\n",CMAX);
  fprintf(output,"RMAX %d\n\n",RMAX);
  fprintf(output,"DMAX %lf\n\n",DMAX);
  fprintf(output,"LMAX %lf\n\n",LMAX);



  //printf("Nbroutes = %d et TEST VAUT : %d avec %d max \n",nbre_routes,somme, CMAX);
  if(strcmp(argv[2], "paquet") != 0){
      model(open,order,inventory,inventory_capacity,holding,demande,routes,CR,n,m,p,nbre_routes,capacite_vehicule,v_max);
  }else{
    //lancer le model sur chaque paquet
  }

  clock_t tac = clock();
  double time;
  time = (double)(tac - tic) / CLOCKS_PER_SEC;
  printf("\nTime en s = %lf\n",time);
  fprintf(output,"\nTime en seconde pour l'execution = %lf\n",time);


  /**
  * lIBERATION DES ALLOCS
  */
  // free_and_null ((char **)&holding,p);
  liberationTabDouble (holding,nbre_sites);
  liberationTabDouble (demande,n);
  liberationTabDouble (coordonnees,(nbre_sites+1));

  liberationTabInt(tableRoutes,somme) ;
  liberationTabInt(distribution,m) ;

  free(open);
  free(order);
  free(inventory);
  free(inventory_capacity);
  free(localisation);
  if (strcmp(argv[2], "paquet") != 0) {
        liberationTabInt(routes,nbre_routes) ;
  }else{
      liberationTabInt(paquet,nbPaquet) ;
  }
  liberationTabInt(representations,nbre_routes) ;
  free(CR);
  fclose(output);
  return 0;
}
