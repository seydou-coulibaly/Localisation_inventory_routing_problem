
#ifndef MODEL_H
#define MODEL_H
void model(double *open,double *order,double *inventory,double *inventory_capacity,double **holding,double **demande,int **routes,double *CR,int n, int m, int p, int nbre_routes,double capacite_vehicule, double v_max);
int buildmodel (CPXENVptr env, CPXLPptr lp, double *open,double *order,double *inventory,double *inventory_capacity,double **holding,double **demande,int **routes,double *CR,int n, int m, int p, int nbre_routes,double capacite_vehicule, double v_max);
int varind (int i, int j, int m);
int varindex_MT (int i,int j,int m, int p);
int varindex_RT (int i,int j,int m, int p);
int varindex_IT (int i,int j,int m, int p, int nbre_routes);
int varindex_QDT (int i,int j,int m, int p, int nbre_routes, int nbre_sites);
int varindex_QCT (int i,int r,int t,int m, int p, int nbre_routes, int nbre_sites);

#endif
