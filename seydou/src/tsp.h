#ifndef DUALE_H
#define DUALE_H
#include <ilcplex/cplex.h>

double* methodeTsp(double **coordonnees,int n);
int buildmodelRelax (CPXENVptr env, CPXLPptr lp, double **coordonnees, int n);
int varindex (int i, int j, int n);
#endif
