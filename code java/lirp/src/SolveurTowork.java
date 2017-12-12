import java.io.PrintStream;
import ilog.concert.IloException;
import ilog.concert.IloIntVar;
import ilog.concert.IloLinearIntExpr;
import ilog.concert.IloLinearNumExpr;
import ilog.concert.IloNumVar;
import ilog.concert.IloObjectiveSense;
import ilog.cplex.IloCplex;
import ilog.cplex.IloCplex.DoubleParam;

public class SolveurTowork {

	private static final double TimeLimit = 10;


	public static Solution solve(Instance instance, PrintStream printStreamSol) throws IloException {

		//int nbSites = instance.getNbSites(); // number of sites
		int nbPeriods = instance.getNbPeriods(); // number of periods
		double[][] listeRoutes = instance.calculateRoutes(); // 	// routes list
		int nbRoutes = instance.getNbRoutes(); // number of routes

		int J = instance.getNbDepots();
		int T = nbPeriods;
		int R = instance.getNbRoutes();
		int I = instance.getNbClients();

		IloCplex solver = new IloCplex();
		solver.setParam(DoubleParam.TiLim, TimeLimit);

		// Boolean VARIABLES
		// cplex.intVar(0, 1, "q0_"+l);

		IloIntVar[] y = solver.boolVarArray(J);

		IloIntVar[][] z = new IloIntVar[R][T];
		for (int r = 0; r < R; r++) {
			for (int t = 0; t < T; t++) {
				z[r][t] = solver.boolVar(); // its not consistent
			}
		}
		IloIntVar[][] x = new IloIntVar[J][T];
		for (int j = 0; j < J; j++) {
			for (int t = 0; t < T; t++) {
				x[j][t] = solver.boolVar(); // int and boolean
			}
		}

		double[] coutRoute = new double[nbRoutes]; // creating cost of routes
		for (int r = 0; r < R; r++) {
			coutRoute[r] = listeRoutes[r][0];
		}


		// Continuous Variables
		// quantity delivered to j in t
		IloNumVar[][] q = new IloNumVar[J][T];
		for (int j = 0; j < J; j++) {
			for (int t = 0; t < T; t++) {
				q[j][t] = solver.numVar(0, Double.MAX_VALUE);
			}
		}


		IloNumVar[][][] u = new IloNumVar[I][R][T]; // quantity delivered route
													// R to client I in time T
		for (int i = 0; i < I; i++) {
			for (int r = 0; r < R; r++) {
				for (int t = 0; t < T; t++) {
					u[i][r][t] = solver.numVar(0, Double.MAX_VALUE);
				}
			}
		}

		IloNumVar[][] InvClients = new IloNumVar[I][T]; // inv at clients
		for (int i = 0; i < I; i++) {
			for (int t = 0; t < T; t++) {
				InvClients[i][t] = solver.numVar(0, Double.MAX_VALUE);
			}
		}

		IloNumVar[][] InvDepots = new IloNumVar[J][T]; // inv at depots
		for (int j = 0; j < J; j++) {
			for (int t = 0; t < T; t++) {
				InvDepots[j][t] = solver.numVar(0, Double.MAX_VALUE);
			}
		}

		IloNumVar obj = solver.numVar(0, Double.MAX_VALUE, "obj"); // objective
																	// function

		// to create the Alpha

		int[][] A = new int[R][I]; // if client i is in route r
		int[][] B = new int[R][J]; // if depot j is in route r

		for (int r = 0; r < nbRoutes; r++) {
			for (int i = 0; i < I; i++)				A[r][i] = 0;
			for (int j = 0; j < J; j++)				B[r][j] = 0;
		}

		for (int r = 0; r < nbRoutes; r++) {
			for (int c = 2; c < instance.CMAX + 2; c++) {
				if (listeRoutes[r][c] == -1) {
					break;
				}
				int customer = (int) listeRoutes[r][c];
				A[r][customer] = 1;
			}
		}

		for (int r = 0; r < nbRoutes; r++) {
			if (listeRoutes[r][1] == -1) {
				break;
			}
			int depotOftheRoute = (int) listeRoutes[r][1] ;
			//System.out.println("depotOftheRoute " +  r  + " is equal to  " + depotOftheRoute);
			B[r][depotOftheRoute-I] = 1;
		}


		// constraint 2
		for (int i = 0; i < I; i++)
			for (int t = 0; t < T; t++) {
				IloLinearNumExpr expr = solver.linearNumExpr();
				for (int r = 0; r < nbRoutes; r++) {
					expr.addTerm(A[r][i], z[r][t]);
				}
				solver.addLe(expr, 1);
			}

		// Constraint 3
		for (int j = 0; j < J; j++) {
			for (int t = 0; t < T; t++) {
				IloLinearNumExpr expr = solver.linearNumExpr();

				expr.addTerm(1, q[j][t]);
				expr.addTerm(-instance.getCapacityVehicle(), x[j][t]);
				solver.addLe(expr, 0);
			}
		}

		// constraint 4
		for (int j = 0; j < J; j++) {
			for (int t = 0; t < T; t++) {
				IloLinearIntExpr expr = solver.linearIntExpr();
				expr.addTerm(1, x[j][t]);
				expr.addTerm(-1, y[j]);
				solver.addLe(expr, 0);
			}
		}

		// constraint 5
				for (int r = 0; r < R; r++) {
					for (int t = 0; t < T; t++) {
						IloLinearNumExpr expr = solver.linearNumExpr();
						for (int i = 0; i < I; i++) {
						expr.addTerm(1, u[i][r][t]);
						}
						expr.addTerm(-instance.getCapacityVehicle(), z[r][t]);
						solver.addLe(expr, 0);
					}
				}



		// constraint 6

		for (int r = 0; r < R; r++) {
			for (int t = 0; t < T; t++) {
				IloLinearNumExpr expr = solver.linearNumExpr();
				for (int j = 0; j < J; j++) { // SUMATORY

					expr.addTerm(B[r][j], y[j]);
				}
				expr.addTerm(-1, z[r][t]);
				solver.addGe(expr, 0);
			}

		}
		//constraint 7 not implemented

		for (int t = 0; t < T; t++) {
			IloLinearNumExpr expr = solver.linearNumExpr();
			for (int r = 0; r < R; r++) {
				expr.addTerm(1, z[r][t]);
			}
			solver.addLe(expr, instance.getNbVehicles());
		}



		// constraint 8 --------> between initial inventory and the first period (t=0)

		for (int j = 0; j < J; j++)// for all j
		{
			IloLinearNumExpr expr = solver.linearNumExpr();
			expr.addTerm(1, InvDepots[j][0]);
			expr.addTerm(-1, q[j][0]);
			for (int r = 0; r < R; r++) {
				for (int i = 0; i < I; i++) // is it good ?
				{
					expr.addTerm(A[r][i] * B[r][j], u[i][r][0]);
				}
			}
			solver.addEq(expr, instance.getInventoryInitialDepot(j));
	}









		// constraint 8 --------> between any period t>=1 and t-1

		for (int t = 1; t < nbPeriods; t++) { 		// for all t
			for (int j = 0; j < J; j++)// for all j
			{
				IloLinearNumExpr expr = solver.linearNumExpr();
				expr.addTerm(1, InvDepots[j][t]);
				expr.addTerm(-1, InvDepots[j][t - 1]);
				expr.addTerm(-1, q[j][t]);

				for (int r = 0; r < R; r++) {
					for (int i = 0; i < I; i++) // is it good ?
					{
						expr.addTerm(A[r][i] * B[r][j], u[i][r][t]);
					}
				}
				solver.addEq(expr, 0);
			}
		}





		// constraint 8bis --------> From any depot, do not send more than the current inventory

		/*		for (int t = 0; t < nbPeriods; t++) { 		// for all t
					for (int j = 0; j < J; j++)// for all j
					{
						IloLinearNumExpr expr = solver.linearNumExpr();
						expr.addTerm(1, InvDepots[j][t]);
						for (int r = 0; r < R; r++) {
							for (int i = 0; i < I; i++) // is it good ?
							{
								expr.addTerm(-A[r][i] * B[r][j], u[i][r][t]);
							}
						}
						solver.addGe(expr, 0);
					}
				}*/




		// constraint 9 inventory at clients, period 0 ---> 1
		for (int i = 0; i < I; i++)
		{
			IloLinearNumExpr expr = solver.linearNumExpr();
			expr.addTerm(1, InvClients[i][0]);
			for (int r = 0; r < R; r++) {
				expr.addTerm(-A[r][i], u[i][r][0]);
			}
			solver.addEq(expr, instance.getInventoryInitialClient(i)-instance.getDemand(i,0));
		}



		// constraint 9 inventory at clients, period t-1 ---> t
		for (int i = 0; i < I; i++)
		{
			for (int t = 1; t < nbPeriods; t++) {

				IloLinearNumExpr expr = solver.linearNumExpr();
				expr.addTerm(1, InvClients[i][t]);
				expr.addTerm(-1, InvClients[i][t - 1]);
				for (int r = 0; r < R; r++) {
					expr.addTerm(-A[r][i], u[i][r][t]);
				}
				double Dem = instance.getDemand(i, t);
				solver.addEq(expr, -Dem);
			}
		}


		// constraint 10

		for (int i = 0; i < I; i++) {
			for (int t = 0; t < nbPeriods; t++) {
				IloLinearNumExpr expr = solver.linearNumExpr();
				expr.addTerm(1, InvClients[i][t]);
				double totalDemand = 0;
				for (int t2 = t; t2 < nbPeriods; t2++) {
					totalDemand = totalDemand + instance.getDemand(i, t2);
					}
				solver.addLe(expr, totalDemand);
			}
		}


		// constraint 11 , if one customer does not belong to a route, then this route does not deliver it

		for (int r = 0; r < R; r++) {
			for (int t = 0; t < T; t++) {
				for (int i = 0; i < I; i++) {
					IloLinearNumExpr expr = solver.linearNumExpr();
					expr.addTerm(1, u[i][r][t]);
					solver.addLe(expr, 1000*A[r][i]);
				}
			}
		}

	//constraint 12, if a route is not performed, then it cannot deliver any customer

		for (int r = 0; r < R; r++) {
			for (int t = 0; t < T; t++) {
				for (int i = 0; i < I; i++) {
					IloLinearNumExpr expr = solver.linearNumExpr();
					expr.addTerm(1, u[i][r][t]);
					expr.addTerm(-1000 , z[r][t]);
					solver.addLe(expr, 0);
				}
			}
		}


		// constraint 13: capacity constraints at depots

		for (int t = 0; t < T; t++) {
			for (int j = 0; j < J; j++) {
				IloLinearNumExpr expr = solver.linearNumExpr();
				expr.addTerm(1, InvDepots[j][t]);
				int capD = instance.getCapacityDepot();
				solver.addLe(expr, capD);
			}
		}


		// constraint 14 : capacity constraints at client
		for (int t = 0; t < T; t++) {
			for (int i = 0; i < I; i++) {
				IloLinearNumExpr expr = solver.linearNumExpr();
				expr.addTerm(1, InvClients[i][t]);
				int capC = instance.getCapacityClient();
				solver.addLe(expr, capC);
				}
		}


		// constraint 15: if a depot is not selected, then its inventory is 0.

		for (int t = 0; t < T; t++) {
			for (int j = 0; j < J; j++) {
				IloLinearNumExpr expr = solver.linearNumExpr();
				expr.addTerm(1, InvDepots[j][t]);
				expr.addTerm(-instance.getCapacityDepot(), y[j]);
				solver.addLe(expr, 0);
			}
		}





		// -------Objective function SCIP-------------

		IloLinearNumExpr expr = solver.linearNumExpr();

		// term 1: fixed location costs
		for (int j = 0; j < J; j++) {
			expr.addTerm(instance.getFixedCost(j), y[j]);
		}

		// 2nd term: cost of delivering depots
		for (int t = 0; t < T; t++) {
			for (int j = 0; j < J; j++) {
				expr.addTerm(instance.getOrderCost(j), x[j][t]);
			}
		}

		// 3rd term : routing costs from depots to clients
		for (int t = 0; t < T; t++) {
			for (int r = 0; r < R; r++) {
				expr.addTerm(coutRoute[r], z[r][t]);
			}
		}

		// 4th term inventory at depots
		for (int t = 0; t < T; t++) {
			for (int j = 0; j < J; j++) {
				expr.addTerm(instance.getHoldingCost(j), InvDepots[j][t]);
			}
		}

		// 5th term inventory at clients
				for (int t = 0; t < T; t++) {
					for (int i = 0; i < I; i++) {
						expr.addTerm(instance.getHoldingCostClient(), InvClients[i][t]);
					}
				}


		//
		solver.addLe(expr, obj);
		solver.addObjective(IloObjectiveSense.Minimize, obj);

		solver.solve();

		System.out.println();
		System.out.println();
		System.out.println("======== RESULTS  =================");
		System.out.println();


		System.out.print("Status of solver :   ");
		System.out.println(solver.getStatus());
		System.out.print("Objective function :   ");
		System.out.println(solver.getObjValue());


	//-----------------------------------------------------
		Solution sol  = new Solution(instance,nbRoutes );

		//save the status of depots (open/closed)
			for (int j = 0; j<J; j++){
				if (solver.getValue(y[j])>0)
				{
					sol.setOpenDepots(j,1);
				}
				else
				{
					sol.setOpenDepots(j,0);
				}
			}

		// save the deliveries to depots (1 if the depot is delivered, 0 otherwise)
		for (int t = 0; t<nbPeriods; t++){
				for (int j = 0; j<J; j++){
				if (solver.getValue(x[j][t]) >0) {
					sol.setDeliveryDepot(j,t,1);
					}
				else {sol.setDeliveryDepot(j,t, 0);	}
			}
		}


		// save quantities delivered to depot
		for (int t = 0; t<nbPeriods; t++){
			for (int j = 0; j<J; j++){
				sol.setQuantityDeliveredToDepot(j,t,(int) solver.getValue(q[j][t]));
			}
		}

		// save the inventory variables
		for (int t = 0; t<nbPeriods; t++){
			for (int j = 0; j<J; j++){
				sol.setStockDepot(j,t, solver.getValue(InvDepots[j][t]));
			}
		}

		// save the quantity delivered to clients
			for (int t=0;t<instance.getNbPeriods();t++){
				for (int i=0;i<instance.getNbClients();i++){
					for (int r=0;r<nbRoutes; r++){
						sol.setQuantityDeliveredToClient(i,r,t,(int) solver.getValue(u[i][r][t]));
				}
			}
		}


		// save the quantity delivered from each depot to each client
		for (int t=0;t<instance.getNbPeriods();t++){
			for (int j=0;j<instance.getNbDepots();j++){
				for (int i=0;i<instance.getNbClients(); i++){
					double flow = 0;
					for (int r=0;r<nbRoutes; r++){
						//System.out.println("Route " +r+ " Brj = "+B[r][j]+" Ari= "+A[r][i]);
						flow = flow + B[r][j]*A[r][i]*solver.getValue(u[i][r][t]); // see the sum in constraints (8)
					}
						sol.setQuantityDepotToClient(i,j,t,(int) flow);
				}
			}
		}



		//-------------------------------------------------
		// save inventory at clients for every period t>=1
		for (int t = 0; t<nbPeriods; t++){
			for (int i = 0; i<I; i++){
				int stcli = (int) solver.getValue(InvClients[i][t]);
				sol.setStockClient(i, t, stcli);
			}
		}

		//save the routes performed in each period
		for (int t = 0; t<nbPeriods; t++){
			for (int r = 0; r<nbRoutes; r++){
				if (solver.getValue(z[r][t]) >0) {
					sol.setListOfRoutes(r, t, 1);
				}
				else sol.setListOfRoutes(r, t, 0);
			}
		}

		// save the route costs
		for (int r = 0; r<nbRoutes; r++){
				sol.setCoutRoute(r, coutRoute[r]);
			}





		printStreamSol.println("--------------------------------------------");
return sol;

}
}

	//------------------------------------------------------
