#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <ilcplex/cplex.h>
#include "structure.h"
#include "model.h"


void model(double *open,double *order,double *inventory,double *inventory_capacity,double **holding,double **demande,int **routes,double *CR,int n, int m, int p, int nbre_routes,double capacite_vehicule, double v_max){
  int       solstat;
  double    objval;
  int status;
  int nbre_sites  = n+m ;
  int colcnt      = m + m*p + nbre_routes*p + nbre_sites*p + m*p + p * nbre_routes * n ;
  double    *X;
  double    *retour;
  X          = (double *) malloc (colcnt * sizeof(*X));
  retour     = (double *) malloc ((n+2) * sizeof(*retour));
  if ( X     == NULL || retour == NULL) {
    fprintf (stderr, "Could not allocate colcnt arrays\n");
    status = CPXERR_NO_MEMORY;
    goto TERMINATE;
  }
  initialiser_tab_double(X,colcnt,0.0);
  initialiser_tab_double(retour,(n+1),0.0);

  CPXENVptr env = CPXopenCPLEX (&status); // ouvre un environnement Cplex

  if ( env == NULL ) {
    char  errmsg[CPXMESSAGEBUFSIZE];
    fprintf (stderr, "Could not open CPLEX environment.\n");
    CPXgeterrorstring (env, status, errmsg);
    fprintf (stderr, "%s", errmsg);
    goto TERMINATE;
  }
  // Turn on output to the screen
  status = CPXsetintparam (env, CPXPARAM_ScreenOutput, CPX_ON);
  if ( status ) {
      fprintf (stderr,"Failure to turn on screen indicator, error %d.\n", status);
      goto TERMINATE;
  }
  // Turn on data checking
  status = CPXsetintparam (env, CPXPARAM_Read_DataCheck, CPX_ON);
  if ( status ) {
    fprintf (stderr,"Failure to turn on data checking, error %d.\n", status);
    goto TERMINATE;
  }

  //crée un PL vide
  CPXLPptr lp = CPXcreateprob (env, &status, "LIRP");
  // traiter l'erreur...
  if ( lp == NULL ) {
    fprintf (stderr, "Failed to create LP.\n");
    goto TERMINATE;
  }
  printf("POPULATE DATA IN CPLEX \n");
  // Build the model
  status = buildmodel(env, lp,open,order,inventory,inventory_capacity,holding,demande,routes,CR,n,m,p,nbre_routes,capacite_vehicule,v_max);
  if ( status ) {
    fprintf (stderr, "Failed to build model.\n");
    goto TERMINATE;
  }
   // Write a copy of the problem to a file.
  status = CPXwriteprob (env, lp, "./generer/LIRP.lp", NULL);
  if ( status ) {
    fprintf (stderr, "Failed to write LP to disk.\n");
    goto TERMINATE;
  }
  // Optimize the problem and obtain solution.
  status = CPXmipopt (env, lp);
  if ( status ) {
    fprintf (stderr, "Failed to optimize MIP.\n");
    goto TERMINATE;
  }
  status = CPXsolution (env, lp, &solstat, &objval, X, NULL, NULL, NULL);
  if ( status ) {
    fprintf (stderr, "Solution failed. Status %d.\n", status);
     goto TERMINATE;
  }

  printf ("\nSolution status = %d\n", solstat);
  printf ("Solution value (min cout) = %f\n\n", objval);

  // Ecriture dans un fichier
  FILE* fichier = NULL;
  fichier = fopen("./generer/log", "w");
  if (fichier == NULL){
    printf("Impossible d'ouvrir le fichier");
    exit(EXIT_FAILURE);
  }
 // On peut lire et écrire dans le fichier


  fprintf(fichier,"------------------------  OPEN DEPOTS  ------------------------\n");
  for (int j = 0; j < m; j++) {
    if (X[j] == 1) {
      fprintf(fichier,"\n\nDEPOT %d\n",j);
      fprintf(fichier,"\tINIT TIME \t STOCK %.1lf\n",inventory[n+j]);
      for (int t = 0; t < p; t++) {
        fprintf(fichier,"\n\tPeriode %d\t STOCK %.1lf\t",t,X[varindex_IT(n+j,t,m,p,nbre_routes)]);
        if (X[varindex_MT(j,t,m,p)] == 1) {
          fprintf(fichier,"QD %.1lf",X[varindex_QDT (j,t,m,p,nbre_routes,nbre_sites)]);
       }
       fprintf(fichier,"\n");
      }
     }
  }
 fprintf(fichier,"\n------------------------    CLIENTS  ------------------------\n");
 for (int i = 0; i < n; i++) {
   fprintf(fichier,"\nCLIENT %d",i);
   fprintf(fichier,"\tINIT TIME \t STOCK %.1lf\n",inventory[i]);
   for (int t = 0; t < p; t++) {
     fprintf(fichier,"\n\n\t\tPeriode %d\t STOCK %.1lf",t,X[varindex_IT(i,t,m,p,nbre_routes)]);
       for (int r = 0; r < nbre_routes; r++) {
         if (X[varindex_RT(r,t,m,p)] == 1) {
           if (X[varindex_QCT(i,r,t,m,p,nbre_routes,nbre_sites)] != 0) {
             fprintf(fichier,"\tQD %.1lf BY ROUTE %d",X[varindex_QCT(i,r,t,m,p,nbre_routes,nbre_sites)],r);
             for (int j = 0; j < m; j++) {
               if (routes[r][n+j] == 1 ) {
                 fprintf(fichier,"\tVia DEPOT %d",j);
                 break ;
               }
             }
           }
         }
       }
       fprintf(fichier,"\n");
   }
 }
  fprintf(fichier,"\n------------------------    LIST OF ROUTES  ------------------------\n");
  for (int t = 0; t < p; t++) {
    for (int r = 0; r < nbre_routes; r++) {
      if (X[varindex_RT(r,t,m,p)] == 1) {
         fprintf(fichier,"\tPeriode %d ROUTE %d visiting clients :\t",t,r);
         for (int i = 0; i < n; i++) {
           if (routes[r][i] == 1) {
             fprintf(fichier,"%d\t",i);
           }
         }
         fprintf(fichier,"\t\t\t(cost %.2lf)\n",CR[r]);
       }
    }
  }
  fprintf(fichier,"\n------------------------    OBJECTIVE Function  ------------------------\n");
  fprintf(fichier,"COUT = %.2lf\n",objval);

  fclose(fichier);


  TERMINATE:
    //libration de tous les allocations
    if ( lp != NULL ) {
      status = CPXfreeprob (env, &lp);
      if ( status ) {
        fprintf (stderr, "CPXfreeprob failed, error code %d.\n", status);
      }
    }
    // Free up the CPLEX environment, if necessary
    if ( env != NULL ) {
      status = CPXcloseCPLEX (&env);
      if ( status ){
        char  errmsg[CPXMESSAGEBUFSIZE];
        fprintf (stderr, "Could not close CPLEX environment.\n");
        CPXgeterrorstring (env, status, errmsg);
        fprintf (stderr, "%s", errmsg);
        printf("ERROR CPLEX ENVIRONNEMENT LIBERATION\n");
      }
    }
  //return retour;
  }

  /*
  Build model using indicator constraints and
  semi-continuous variables
  */
int buildmodel (CPXENVptr env, CPXLPptr lp, double *open,double *order,double *inventory,double *inventory_capacity,double **holding,double **demande,int **routes,double *CR,int n, int m, int p, int nbre_routes,double capacite_vehicule,double v_max){
  int nbre_sites  = n+m ;
  int colcnt      = m + m*p + nbre_routes*p + nbre_sites*p + m*p + p * nbre_routes * n ;
  double *obj     = NULL;
  double *lb      = NULL;
  double *ub      = NULL;
  char   *ctype   = NULL;
  int    *rmatind = NULL;
  double *rmatval = NULL;
  int    *rmatbeg = NULL;
  double     *rhs = NULL;
  char     *sense = NULL;
  int    status   = 0;

  /* Allocate colcnt-sized arrays */
  obj     = (double *) malloc (colcnt * sizeof(*obj));
  lb      = (double *) malloc (colcnt * sizeof(*lb));
  ub      = (double *) malloc (colcnt * sizeof(*ub));
  ctype   = (char *)   malloc (colcnt * sizeof(char));

  rmatind = (int * )   malloc ( nbre_routes * sizeof(int));
  rmatval = (double *) malloc ( nbre_routes * sizeof(double));
  rhs     = (double *) malloc ( sizeof(double));
  rmatbeg = (int    *) malloc ( sizeof(int));
  sense   = (char *) malloc (sizeof(char));

  if ( obj     == NULL ||
  lb      == NULL ||
  ub      == NULL ||
  ctype   == NULL ||
  rmatind == NULL ||
  rmatval == NULL ||
  rmatbeg == NULL ||
  sense   == NULL ||
  rhs     == NULL   ) {
  fprintf (stderr, "Could not allocate colcnt arrays\n");
  status = CPXERR_NO_MEMORY;
  goto TERMINATE;
  }
  initialiser_tab_double(obj,colcnt,0.0);
  initialiser_tab_double(lb,colcnt,0.0);
  initialiser_tab_double(ub,colcnt,0.0);
  initialiser_tab_char(ctype,colcnt,'B');

  initialiser_tab_double(rmatval,nbre_routes,0.0);
  initialiser_tab_double(rhs,1,1.0);
  initialiser_tab_int(rmatind,nbre_routes,0);
  initialiser_tab_int(rmatbeg,1,0);
  initialiser_tab_char(sense,1,'E');

  //Fonction objective
  //y(j)
  for (int i = 0; i < m; i++) {
    obj[i]    = open[i];
    lb[i]     = 0.0;
    ub[i]     = 1.0;
    ctype[i]  = 'B';
  }
  //x(it)
  for (int i = 0; i < m; i++) {
    for (int j = 0; j < p; j++) {
      obj[varindex_MT (i,j,m,p)]    = order[i];
      lb[varindex_MT (i,j,m,p)]     = 0.0;
      ub[varindex_MT (i,j,m,p)]     = 1.0;
      ctype[varindex_MT (i,j,m,p)]  = 'B';
    }
  }
  //z(rt)
  for (int i = 0; i < nbre_routes; i++) {
    for (int j = 0; j < p; j++) {
      obj[varindex_RT (i,j,m,p)]    = CR[i];
      lb[varindex_RT (i,j,m,p)]     = 0.0;
      ub[varindex_RT (i,j,m,p)]     = 1.0;
      ctype[varindex_RT (i,j,m,p)]  = 'B';
    }
  }
  //inventory
  for (int i = 0; i < nbre_sites; i++) {
    for (int j = 0; j < p; j++) {
      obj[varindex_IT (i,j,m,p,nbre_routes)]    = holding[i][j];
      lb[varindex_IT (i,j,m,p,nbre_routes)]     = 0.0;
      ub[varindex_IT (i,j,m,p,nbre_routes)]     = CPX_INFBOUND;
      ctype[varindex_IT (i,j,m,p,nbre_routes)]  = 'C';
    }
  }
  //q(jt)
  for (int i = 0; i < m; i++) {
    for (int j = 0; j < p; j++) {
      obj[varindex_QDT (i,j,m,p,nbre_routes,nbre_sites)]    = 0;
      lb[varindex_QDT (i,j,m,p,nbre_routes,nbre_sites)]     = 0.0;
      ub[varindex_QDT (i,j,m,p,nbre_routes,nbre_sites)]     = CPX_INFBOUND;
      ctype[varindex_QDT (i,j,m,p,nbre_routes,nbre_sites)]  = 'C';
    }
  }
  //u(irt)
  for (int i = 0; i < n; i++) {
    for (int r = 0; r < nbre_routes; r++) {
      for (int t = 0; t < p; t++) {
        obj[varindex_QCT (i,r,t,m,p,nbre_routes,nbre_sites)]    = 0;
        lb[varindex_QCT (i,r,t,m,p,nbre_routes,nbre_sites)]     = 0.0;
        ub[varindex_QCT (i,r,t,m,p,nbre_routes,nbre_sites)]     = CPX_INFBOUND;
        ctype[varindex_QCT (i,r,t,m,p,nbre_routes,nbre_sites)]  = 'C';
      }
    }
  }
  status = CPXnewcols (env, lp, colcnt, obj, lb, ub, ctype, NULL);
  if ( status ) {
  fprintf (stderr, "Could not add new columns.\n");
  goto TERMINATE;
  }

  free_and_null ((char **)&obj);
  free_and_null ((char **)&lb);
  free_and_null ((char **)&ub);
  free_and_null ((char **)&ctype);

  // Now add the constraints

  //Première contrainte
  for (int i = 0; i < n; i++) {
    for (int t = 0; t < p; t++) {
      //rmatval
      for (int r = 0; r < nbre_routes; r++) {
        rmatval[r] = routes[r][i];
      }
      //rmatind
      for (int r = 0; r < nbre_routes; r++) {
        rmatind[r] = varindex_RT (r,t,m,p);
      }
      //rmatbeg
      rmatbeg[0] = 0;
      //rhs et sense
      rhs[0]   = 1;
      sense[0] = 'L';

      status = CPXaddrows (env, lp, 0, 1, nbre_routes, rhs, sense, rmatbeg, rmatind, rmatval, NULL, NULL);
    }
  }

  free_and_null ((char **)&rmatind);
  free_and_null ((char **)&rmatval);

  //Deuxieme contrainte

  rmatind = (int * )   malloc ( 2 * sizeof(int));
  rmatval = (double *) malloc ( 2 * sizeof(double));

  if ( rmatind == NULL ||
  rmatval == NULL) {
  fprintf (stderr, "Could not allocate colcnt arrays\n");
  status = CPXERR_NO_MEMORY;
  goto TERMINATE;
  }
  initialiser_tab_double(rmatval,2,0.0);
  initialiser_tab_int(rmatind,2,0);

  for (int i = 0; i < m; i++) {
    for (int t = 0; t < p; t++) {
      //rmatval
        rmatval[0] = 1;
        rmatval[1] = -capacite_vehicule;
      //rmatind
        rmatind[0] = varindex_QDT (i,t,m,p,nbre_routes,nbre_sites);
        rmatind[1] = varindex_MT (i,t,m,p);
      //rmatbeg
      rmatbeg[0] = 0;
      //rhs et sense
      rhs[0]   = 0;
      sense[0] = 'L';

      status = CPXaddrows (env, lp, 0, 1, 2, rhs, sense, rmatbeg, rmatind, rmatval, NULL, NULL);
    }
  }

  //Trosieme contrainte

  for (int i = 0; i < m; i++) {
    for (int t = 0; t < p; t++) {
      //rmatval
        rmatval[0] = 1;
        rmatval[1] = -1;
      //rmatind
        rmatind[0] = varindex_MT (i,t,m,p);;
        rmatind[1] = i;
      //rmatbeg
      rmatbeg[0] = 0;
      //rhs et sense
      rhs[0]   = 0;
      sense[0] = 'L';

      status = CPXaddrows (env, lp, 0, 1, 2, rhs, sense, rmatbeg, rmatind, rmatval, NULL, NULL);
    }
  }

  free_and_null ((char **)&rmatind);
  free_and_null ((char **)&rmatval);

  //Quatrieme contrainte

  rmatind = (int * )   malloc ( (n+1) * sizeof(int));
  rmatval = (double *) malloc ( (n+1) * sizeof(double));

  if ( rmatind == NULL ||
  rmatval == NULL) {
  fprintf (stderr, "Could not allocate colcnt arrays\n");
  status = CPXERR_NO_MEMORY;
  goto TERMINATE;
  }
  initialiser_tab_double(rmatval,(n+1),0.0);
  initialiser_tab_int(rmatind,(n+1),0);

  for (int r = 0; r < nbre_routes; r++) {
    for (int t = 0; t < p; t++) {
      //rmatval
      for (int i = 0; i < n; i++) {
        rmatval[i] = 1.0;
      }
      rmatval[n] = - capacite_vehicule ;

      //rmatind
      for (int i = 0; i < n; i++) {
        rmatind[i] =  varindex_QCT (i,r,t,m,p,nbre_routes,nbre_sites);
      }
      rmatind[n] = varindex_RT (r,t,m,p);

      //rmatbeg
      rmatbeg[0] = 0;
      //rhs et sense
      rhs[0]   = 0;
      sense[0] = 'L';

      status = CPXaddrows (env, lp, 0, 1, (n+1), rhs, sense, rmatbeg, rmatind, rmatval, NULL, NULL);
    }
  }

  free_and_null ((char **)&rmatind);
  free_and_null ((char **)&rmatval);

  //Cinquieme contrainte

  rmatind = (int * )   malloc ( (m+1) * sizeof(int));
  rmatval = (double *) malloc ( (m+1) * sizeof(double));

  if ( rmatind == NULL ||
  rmatval == NULL) {
  fprintf (stderr, "Could not allocate colcnt arrays\n");
  status = CPXERR_NO_MEMORY;
  goto TERMINATE;
  }
  initialiser_tab_double(rmatval,(m+1),0.0);
  initialiser_tab_int(rmatind,(m+1),0);

  for (int r = 0; r < nbre_routes; r++) {
    for (int t = 0; t < p; t++) {
      //rmatval
      rmatval[0] = 1.0;
      for (int i = 0; i < m; i++) {
        rmatval[i+1] = -routes[r][n+i];
      }

      //rmatind
      rmatind[0] = varindex_RT (r,t,m,p);
      for (int i = 0; i < m; i++) {
        rmatind[i+1] = i ;
      }

      //rmatbeg
      rmatbeg[0] = 0;
      //rhs et sense
      rhs[0]   = 0;
      sense[0] = 'L';

      status = CPXaddrows (env, lp, 0, 1, (m+1), rhs, sense, rmatbeg, rmatind, rmatval, NULL, NULL);
    }
  }

  free_and_null ((char **)&rmatind);
  free_and_null ((char **)&rmatval);

  //Sixieme contrainte

  rmatind = (int * )   malloc ( nbre_routes * sizeof(int));
  rmatval = (double *) malloc ( nbre_routes * sizeof(double));

  if ( rmatind == NULL ||
  rmatval == NULL) {
  fprintf (stderr, "Could not allocate colcnt arrays\n");
  status = CPXERR_NO_MEMORY;
  goto TERMINATE;
  }
  initialiser_tab_double(rmatval,nbre_routes,0.0);
  initialiser_tab_int(rmatind,nbre_routes,0);

  for (int t = 0; t < p; t++) {
    //rmatval
    for (int r = 0; r < nbre_routes; r++) {
      rmatval[r] = 1.0 ;
    }
    //rmatind
    for (int r = 0; r < nbre_routes; r++) {
      rmatind[r] = varindex_RT(r,t,m,p); ;
    }
    //rmatbeg
    rmatbeg[0] = 0;
    //rhs et sense
    rhs[0]   = v_max;
    sense[0] = 'L';
    status = CPXaddrows (env, lp, 0, 1, nbre_routes, rhs, sense, rmatbeg, rmatind, rmatval, NULL, NULL);

  }

  free_and_null ((char **)&rmatind);
  free_and_null ((char **)&rmatval);

  // Septième contrainte

  // Quand t != 0 avec la contrainte 7


  rmatind = (int * )   malloc ( (3 + n * nbre_routes) * sizeof(int));
  rmatval = (double *) malloc ( (3 + n * nbre_routes) * sizeof(double));

  if ( rmatind == NULL ||
  rmatval == NULL) {
  fprintf (stderr, "Could not allocate colcnt arrays\n");
  status = CPXERR_NO_MEMORY;
  goto TERMINATE;
  }
  initialiser_tab_double(rmatval,(3 + n * nbre_routes),0.0);
  initialiser_tab_int(rmatind,(3 + n * nbre_routes),0);

  for (int j = 0; j < m; j++) {
    for (int t = 1; t < p; t++) {
      //rmatval
      rmatval[0] =  1 ;
      rmatval[1] = -1 ;
      rmatval[2] = -1 ;
      int k = 3 ;
      for (int r = 0; r < nbre_routes; r++) {
        for (int i = 0; i < n; i++) {
          rmatval[k] = routes[r][n+j] ;
          k++;
        }
      }
      //rmatind
      rmatind[0] = varindex_IT ((n+j),t,m,p,nbre_routes);
      rmatind[1] = varindex_IT ((n+j),(t-1),m,p,nbre_routes);
      rmatind[2] = varindex_QDT (j,t,m,p,nbre_routes,nbre_sites);
      k = 3 ;
      for (int r = 0; r < nbre_routes; r++) {
        for (int i = 0; i < n; i++) {
          rmatind[k] = varindex_QCT (i,r,t,m,p,nbre_routes,nbre_sites);
          k++;
        }
      }
      //rmatbeg
      rmatbeg[0] = 0;
      //rhs et sense
      rhs[0]   = 0;
      sense[0] = 'E';

      status = CPXaddrows (env, lp, 0, 1, (nbre_routes*n+3), rhs, sense, rmatbeg, rmatind, rmatval, NULL, NULL);
    }
  }
  free_and_null ((char **)&rmatind);
  free_and_null ((char **)&rmatval);

  //Quand t = 0 avec la contrainte 7

  rmatind = (int * )   malloc ( (n * nbre_routes + 2) * sizeof(int));
  rmatval = (double *) malloc ( (n * nbre_routes + 2) * sizeof(double));

  if ( rmatind == NULL ||
  rmatval == NULL) {
  fprintf (stderr, "Could not allocate colcnt arrays\n");
  status = CPXERR_NO_MEMORY;
  goto TERMINATE;
  }
  initialiser_tab_double(rmatval,(n * nbre_routes + 2),0.0);
  initialiser_tab_int(rmatind,(n * nbre_routes + 2),0);

  // affiche_tab_double(rmatval,(n * nbre_routes + 2));


  for (int j = 0; j < m; j++) {
    int t = 0 ;
    //rmatval
    rmatval[0] =  1 ;
    rmatval[1] = -1 ;
    int k = 2 ;
    for (int r = 0; r < nbre_routes; r++) {
      for (int i = 0; i < n; i++) {
        rmatval[k] = routes[r][n+j] ;
        k++;
      }
    }
    //rmatind
    rmatind[0] = varindex_IT ((n+j),t,m,p,nbre_routes);
    rmatind[1] = varindex_QDT (j,t,m,p,nbre_routes,nbre_sites);
    k = 2 ;
    for (int r = 0; r < nbre_routes; r++) {
      for (int i = 0; i < n; i++) {
        rmatind[k] = varindex_QCT (i,r,t,m,p,nbre_routes,nbre_sites);
        k++;
      }
    }
    //rmatbeg
    rmatbeg[0] = 0 ;
    //rhs et sense
    rhs[0]   = inventory[n+j] ;
    sense[0] = 'E' ;

    // affiche_tab_int(rmatind,(n * nbre_routes + 2));
    status = CPXaddrows (env, lp, 0, 1, (nbre_routes*n+2), rhs, sense, rmatbeg, rmatind, rmatval, NULL, NULL);
  }

  //Huitième contrainte

  free_and_null ((char **)&rmatind);
  free_and_null ((char **)&rmatval);
  rmatind = (int * )   malloc ( (nbre_routes + 2) * sizeof(int));
  rmatval = (double *) malloc ( (nbre_routes + 2) * sizeof(double));

  if ( rmatind == NULL ||
  rmatval == NULL) {
  fprintf (stderr, "Could not allocate colcnt arrays\n");
  status = CPXERR_NO_MEMORY;
  goto TERMINATE;
  }
  initialiser_tab_double(rmatval,(nbre_routes + 2),0.0);
  initialiser_tab_int(rmatind,(nbre_routes + 2),0);

  //t!= 0

  for (int i = 0; i < n; i++) {
    for (int t = 1; t < p; t++) {
      //rmatval
      rmatval[0] =  1 ;
      rmatval[1] = -1 ;
      for (int r = 0; r < nbre_routes; r++) {
        rmatval[r+2] = -1 ;
      }
      //rmatind
      rmatind[0] = varindex_IT (i,t,m,p,nbre_routes);
      rmatind[1] = varindex_IT (i,(t-1),m,p,nbre_routes);
      for (int r = 0; r < nbre_routes; r++) {
        rmatind[r+2] = varindex_QCT (i,r,t,m,p,nbre_routes,nbre_sites);
      }
      //rmatbeg
      rmatbeg[0] = 0;
      //rhs et sense
      rhs[0]   = -demande[i][t];
      sense[0] = 'E';

      status = CPXaddrows (env, lp, 0, 1, (nbre_routes+2), rhs, sense, rmatbeg, rmatind, rmatval, NULL, NULL);
    }
  }
  free_and_null ((char **)&rmatind);
  free_and_null ((char **)&rmatval);

  // t = 0

  rmatind = (int * )   malloc ( (nbre_routes + 1) * sizeof(int));
  rmatval = (double *) malloc ( (nbre_routes + 1) * sizeof(double));

  if ( rmatind == NULL ||
  rmatval == NULL) {
  fprintf (stderr, "Could not allocate colcnt arrays\n");
  status = CPXERR_NO_MEMORY;
  goto TERMINATE;
  }
  initialiser_tab_double(rmatval,(nbre_routes + 1),0.0);
  initialiser_tab_int(rmatind,(nbre_routes + 1),0);

  for (int i = 0; i < n; i++) {
    int t = 0 ;
    //rmatval
    rmatval[0] =  1 ;
    for (int r = 0; r < nbre_routes; r++) {
      rmatval[r+1] = - 1 ;
    }
    //rmatind
    rmatind[0] = varindex_IT (i,t,m,p,nbre_routes);
    for (int r = 0; r < nbre_routes; r++) {
      rmatind[r+1] = varindex_QCT (i,r,t,m,p,nbre_routes,nbre_sites);
    }
    //rmatbeg
    rmatbeg[0] = 0;
    //rhs et sense
    rhs[0]   = inventory[i] - demande[i][t];
    sense[0] = 'E';

    status = CPXaddrows (env, lp, 0, 1, (nbre_routes+2), rhs, sense, rmatbeg, rmatind, rmatval, NULL, NULL);
  }

  free_and_null ((char **)&rmatind);
  free_and_null ((char **)&rmatval);

  // Neuvième contrainte

  rmatind = (int * )   malloc (sizeof(int));
  rmatval = (double *) malloc (sizeof(double));

  if ( rmatind == NULL ||
  rmatval == NULL) {
  fprintf (stderr, "Could not allocate colcnt arrays\n");
  status = CPXERR_NO_MEMORY;
  goto TERMINATE;
  }
  initialiser_tab_double(rmatval,1,0.0);
  initialiser_tab_int(rmatind,1,0);

  for (int i = 0; i < n; i++) {
    for (int t = 0; t < p; t++) {
      double totalDemand = 0;
      for (int t2 = (t+1); t2 < p; t2++) {
        totalDemand = totalDemand + demande[i][t2] ;
      }
      //rmatval
      rmatval[0] =  1 ;
      //rmatind
      rmatind[0] = varindex_IT (i,t,m,p,nbre_routes);
      //rmatbeg
      rmatbeg[0] = 0;
      //rhs et sense
      rhs[0]   = minimum(totalDemand,inventory_capacity[i]);
      sense[0] = 'L';

      status = CPXaddrows (env, lp, 0, 1, 1, rhs, sense, rmatbeg, rmatind, rmatval, NULL, NULL);
    }
  }

  //Dixième contrainte

  for (int i = 0; i < n; i++) {
    for (int r = 0; r < nbre_routes; r++) {
        for (int t = 0; t < p; t++) {
          //rmatval
          rmatval[0] =  1 ;
          //rmatind
          rmatind[0] = varindex_QCT (i,r,t,m,p,nbre_routes,nbre_sites);
          //rmatbeg
          rmatbeg[0] = 0;
          //rhs et sense
          rhs[0]   = capacite_vehicule * routes[r][i] ;
          sense[0] = 'L';

          status = CPXaddrows (env, lp, 0, 1, 1, rhs, sense, rmatbeg, rmatind, rmatval, NULL, NULL);
        }
    }
  }

  free_and_null ((char **)&rmatind);
  free_and_null ((char **)&rmatval);

  //Treizième contrainte
  rmatind = (int * )   malloc (2 * sizeof(int));
  rmatval = (double *) malloc (2 * sizeof(double));

  if ( rmatind == NULL ||
  rmatval == NULL) {
  fprintf (stderr, "Could not allocate colcnt arrays\n");
  status = CPXERR_NO_MEMORY;
  goto TERMINATE;
  }
  initialiser_tab_double(rmatval,2,0.0);
  initialiser_tab_int(rmatind,2,0);

  for (int i = 0; i < m; i++) {
    for (int t = 0; t < p; t++) {
      //rmatval
      rmatval[0] =  1 ;
      rmatval[1] =  - inventory_capacity[n+i] ;
      //rmatind
      rmatind[0] = varindex_IT ((n+i),t,m,p,nbre_routes);
      rmatind[1] = i ;
      //rmatbeg
      rmatbeg[0] = 0;
      //rhs et sense
      rhs[0]   = 0 ;
      sense[0] = 'L';
      status = CPXaddrows (env, lp, 0, 1, 2, rhs, sense, rmatbeg, rmatind, rmatval, NULL, NULL);
    }
  }


  free_and_null ((char **)&rmatind);
  free_and_null ((char **)&rmatval);
  free_and_null ((char **)&rmatbeg);
  free_and_null ((char **)&rhs);
  free_and_null ((char **)&sense);

  TERMINATE:
  free_and_null ((char **)&obj);
  free_and_null ((char **)&lb);
  free_and_null ((char **)&ub);
  free_and_null ((char **)&ctype);
  free_and_null ((char **)&rmatind);
  free_and_null ((char **)&rmatval);
  free_and_null ((char **)&rmatbeg);
  free_and_null ((char **)&rhs);
  free_and_null ((char **)&sense);

  printf("\n");
  return (status);


}

int varind (int i, int j, int n){
  return n * i+ 2 * i + j;
}
int varindex_MT (int i,int j,int m,int p){
  return i * p + j + m;
}
int varindex_RT (int i,int j,int m, int p){
  return i*p + j + (m + m * p);
}
int varindex_IT (int i,int j,int m, int p, int nbre_routes){
  return i * p + j + (m + m * p + nbre_routes * p);
}
int varindex_QDT (int i,int j,int m, int p, int nbre_routes, int nbre_sites){
  return i * p + j + (m + m * p + nbre_routes * p + nbre_sites * p);
}
int varindex_QCT (int i,int r,int t,int m, int p, int nbre_routes, int nbre_sites){
  return i * nbre_routes * p + r * p + t + (m + m * p + nbre_routes * p + nbre_sites * p + m * p);
}
