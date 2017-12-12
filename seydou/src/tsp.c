#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <ilcplex/cplex.h>
#include "structure.h"
#include "tsp.h"


double* methodeTsp(double **coordonnees,int n){
int       solstat;
double    objval;
double    *x;
double *retour;
int status;

int colcnt = (n+1) * (n+2);
x          = (double *) malloc (colcnt * sizeof(*x));
retour     = (double *) malloc ((n+2) * sizeof(*retour));
if ( x     == NULL || retour == NULL) {
fprintf (stderr, "Could not allocate colcnt arrays\n");
status = CPXERR_NO_MEMORY;
goto TERMINATE;
}
initialiser_tab_double(x,colcnt,0.0);
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
CPXLPptr lp = CPXcreateprob (env, &status, "TSP");
// traiter l'erreur...
if ( lp == NULL ) {
  fprintf (stderr, "Failed to create LP.\n");
  goto TERMINATE;
}
printf("POPULATE DATA IN CPLEX \n");
// Build the model
status = buildmodelRelax (env, lp, coordonnees,n);
if ( status ) {
  fprintf (stderr, "Failed to build model.\n");
  goto TERMINATE;
}
 // Write a copy of the problem to a file.
//  status = CPXwriteprob (env, lp, "TSP.lp", NULL);
//  if ( status ) {
//    fprintf (stderr, "Failed to write LP to disk.\n");
//    goto TERMINATE;
// }
// Optimize the problem and obtain solution.
status = CPXmipopt (env, lp);
if ( status ) {
  fprintf (stderr, "Failed to optimize MIP.\n");
  goto TERMINATE;
}
status = CPXsolution (env, lp, &solstat, &objval, x, NULL, NULL, NULL);
if ( status ) {
  fprintf (stderr, "Solution failed. Status %d.\n", status);
   goto TERMINATE;
}

printf ("\nSolution status = %d\n", solstat);
printf ("Solution value (min cout) = %f\n\n", objval);

retour[0] = objval;
/*
for (int i = 0; i < colcnt; i++) {
  printf("%lf\n", x[i] );
}
*/

for (int i = 0; i < (n+1); i++) {
  //printf("%lf\n", x[((i+1)*(n+1)+i)] );
  retour[i+1] = x[((i+1)*(n+1)+i)];
}


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
return retour;
}

/*
Build model using indicator constraints and
semi-continuous variables
*/
int buildmodelRelax (CPXENVptr env, CPXLPptr lp, double **coordonnees,int n){

int colcnt = (n+1) * (n+2);
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

rmatind = (int * )   malloc ( ((n+1) * (n+1)) * sizeof(int));
rmatval = (double *) malloc ( ((n+1) * (n+1)) * sizeof(double));
rhs     = (double *) malloc ( (n+1) * sizeof(double));
rmatbeg = (int    *) malloc ((n+1) * sizeof(int));
sense   = (char *) malloc ( (n+1) * sizeof(char));

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
initialiser_tab_double(rmatval,((n+1) * (n+1)),0.0);
initialiser_tab_double(rhs,(n+1),1.0);
initialiser_tab_int(rmatind,((n+1) * (n+1)),0);
initialiser_tab_int(rmatbeg,(n+1),0);
initialiser_tab_char(ctype,colcnt,'B');
initialiser_tab_char(sense,(n+1),'E');
//printf("index\n");
for (int i = 0; i < (n+1); i++) {
  for (int j = 0; j < (n+1); j++) {
    // poids ou distance de liaison entre i et j
    obj[varindex(i,j,n)]    = distance (coordonnees[i][0], coordonnees[i][1], coordonnees[j][0], coordonnees[j][1]);
    lb[varindex (i,j,n)]    = 0.0;
    ub[varindex (i,j,n)]    = 1.0;
    ctype[varindex (i,j,n)] = 'B';
  }
  //indicateur pour les sous tours
  obj[((i+1)*(n+1)+i)]   = 0.0;
  lb[((i+1)*(n+1)+i)]    = 2.0;
  ub[((i+1)*(n+1)+i)]    = n+1;
  ctype[((i+1)*(n+1)+i)] = 'I';
}

lb[((n+1) * (n+2) - 1)]    = 1;

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
//rmatval
for (int i = 0; i < ((n+1) * (n+1)); i++) {
  rmatval[i] = 1.0;
}
//rmatind
int k = 0;
for (int i = 0; i < (n+1); i++) {
  for (int j = 0; j < (n+1); j++) {
    rmatind[k] = varindex (i,j,n);
    k++;
  }
}
//rmatbeg
for (int j = 0; j < (n+1); j++) {
  rmatbeg[j] = j * (n+1);
}
//rhs et sense
//for (int i = 0; i < m; i++) {
//rhs[i]   = 1.0;
//}

status = CPXaddrows (env, lp, 0, (n+1), (n+1) * (n+1), rhs, sense, rmatbeg, rmatind, rmatval, NULL, NULL);

//Deuxieme contrainte

//rmatval
//rmatind
k = 0;
for (int i = 0; i < (n+1); i++) {
  for (int j = 0; j < (n+1); j++) {
    rmatind[k] = varindex (j,i,n);
    k++;
  }
}
//rmatbeg
for (int j = 0; j < (n+1); j++) {
rmatbeg[j] = j * (n+1);
}
status = CPXaddrows (env, lp, 0, (n+1), (n+1) * (n+1), rhs, sense, rmatbeg, rmatind, rmatval, NULL, NULL);

free_and_null ((char **)&rmatind);
free_and_null ((char **)&rmatval);
free_and_null ((char **)&rmatbeg);
free_and_null ((char **)&rhs);
free_and_null ((char **)&sense);



//troisième contrainte      u((n+1)(n+2) - 1) = 1
rmatind = (int * )   malloc (sizeof(int));
rmatval = (double *) malloc (sizeof(double));
rhs     = (double *) malloc (sizeof(double));
rmatbeg = (int    *) malloc (sizeof(int));
sense   = (char *) malloc (sizeof(char));
if ( rmatind == NULL ||
 rmatval == NULL ||
 rmatbeg == NULL ||
 sense   == NULL ||
 rhs     == NULL   ) {
fprintf (stderr, "Could not allocate colcnt arrays\n");
status = CPXERR_NO_MEMORY;
goto TERMINATE;
}

//rmatval
rmatval[0] = 1.0;
//rmatind
rmatind[0] = (n+1)*(n+2)-1;
//rmatbeg
rmatbeg[0] = 0;
//rhs et sense
rhs[0]   = 1.0;
sense[0]   = 'E';
status = CPXaddrows (env, lp, 0, 1, 1, rhs, sense, rmatbeg, rmatind, rmatval, NULL, NULL);



free_and_null ((char **)&rmatind);
free_and_null ((char **)&rmatval);
free_and_null ((char **)&rmatbeg);
free_and_null ((char **)&rhs);
free_and_null ((char **)&sense);


//////////////////////////////////////////////

rmatind = (int * )   malloc (3 * sizeof(int));
rmatval = (double *) malloc (3 * sizeof(double));
rhs     = (double *) malloc (sizeof(double));
rmatbeg = (int    *) malloc (sizeof(int));
sense   = (char *) malloc (sizeof(char));
if ( rmatind == NULL ||
 rmatval == NULL ||
 rmatbeg == NULL ||
 sense   == NULL ||
 rhs     == NULL   ) {
fprintf (stderr, "Could not allocate colcnt arrays\n");
status = CPXERR_NO_MEMORY;
goto TERMINATE;
}
initialiser_tab_double(rmatval,3,0.0);
initialiser_tab_double(rhs,1,1.0);
initialiser_tab_int(rmatind,3,0);
initialiser_tab_int(rmatbeg,1,0);
initialiser_tab_char(sense,1,'E');


for (int i = 0; i < n; i++) {
  for (int j = 0; j < n; j++) {
    if (i != j) {
      //rmatval
      k = 0;
      rmatval[k] = 1.0;
      k++;
      rmatval[k] = -1.0;
      k++;
      rmatval[k] = n;
      k++;
      //rmatind
      k = 0;
      rmatind[k] = ((i+1) * (n+1) + i);
      k++;
      rmatind[k] = ((j+1) * (n+1) + j);
      k++;
      rmatind[k] = varindex (i,j,n);
      k++;
      //rmatbeg
      rmatbeg[0] = 0;
      //rhs et sense
      rhs[0]   = (double)(n-1);
      sense[0] = 'L';

      status = CPXaddrows (env, lp, 0, 1, 3, rhs, sense, rmatbeg, rmatind, rmatval, NULL, NULL);
    }
  }
}

free_and_null ((char **)&rmatind);
free_and_null ((char **)&rmatval);

rmatind = (int * )   malloc (sizeof(int));
rmatval = (double *) malloc (sizeof(double));
if ( rmatind == NULL ||
 rmatval == NULL ) {
fprintf (stderr, "Could not allocate colcnt arrays\n");
status = CPXERR_NO_MEMORY;
goto TERMINATE;
}

//cinquieme contraintes
for (int i = 0; i < (n+1); i++) {
  //rmatval
  rmatval[0] = 1.0;
  //rmatind
  rmatind[0] = varindex (i,i,n);
  //rmatbeg
  rmatbeg[0] = 0;
  //rhs et sense
  rhs[0]   = 0;
  sense[0] = 'E';

  status = CPXaddrows (env, lp, 0, 1, 1, rhs, sense, rmatbeg, rmatind, rmatval, NULL, NULL);
}








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

int varindex (int i, int j, int n){
return n* i+ 2*i + j;
}
