

public class solution2 {

	private instance2 instance  ;
		// TODO Auto-generated constructor stub
		
		private int [] openDepots;
		private double[][] deliveryDepot;   // quantity delivered au site j le jour t q[j][t]
		private double[][] deliveryClient;   // quantity delivered to client i, from depot j in period t v[i][j][t] 
		private double[] stockInitialDepot;		// initial inventory at depots, InventoryInitialDepot(j)
		private double[] stockInitialClient;		// initial inventory at clients, InventoryInitialClient(j)
		private double[][] stockDepot;		// InvDepot
		private double[][] stockClient;		// InvClients
		private int[][] listOfRoutes;		// listOfRoutes
		//private double [][] DeliveryCost; //DeliveryCost
		private double[] coutRoute;  //    cout de la route r le jour t. Vaut cout[r] si la route est faite, et 0 sinon. 
		private double[][][] quantityDeliveredToClient;  // v[i][j][t] , quantity delivered to client u by route r on period t
		private double[][][] quantityDeliveredToDepot;  //  q[j][r][t]  quantity delivered to client u by route r on period t
		private double[][][] quantityDepotToClient; // quantity delivered from each depot to each client
		
			
		
			/*****************************************
		 *********** ACCESSEURS *****************
		 *****************************************/
		
		

		public double  getDeliveryDepot(int j, int t){
			return deliveryDepot[j][t];
		}
		
		public void setDeliveryDepot(int j, int t, int v){
			deliveryDepot[j][t]=v;
		}
		
		public void setStockDepot(int j, int t, double v){
			stockDepot[j][t]=v;
		}
		
		public void setStockClient(int j, int t, double v){
			stockClient[j][t]=v;
		}
		
		
		public void setCoutRoute(int r, double co){
			coutRoute[r] = co;
		}
		
		
		public int setOpenDepots(int j, int value){
		 return openDepots[j]=value ;	
		}
		
		public void setQuantityDeliveredToClient(int i, int j, int t, double q){
			quantityDeliveredToClient[i][j][t]=q;
		}
		
		
		public void setQuantityDeliveredToDepot(int j, int r, int t, double q){
				quantityDeliveredToDepot[j][r][t]=q;
		}
		
		public void setQuantityDepotToClient(int i, int j, int t, double q){
			quantityDepotToClient[i][j][t]=q;
		}
			
			public void setListOfRoutes(int r, int t, int value){
				listOfRoutes[r][t]=value;	
				}

			
		/*****************************************
		 *********** CONSTRUCTEUR ****************
		 *****************************************/
		public solution2(instance2 instance, int nbRoutes){
			this.instance = instance;
			int J = instance.getNbDepots();
			int I = instance.getNbClients();
			int T = instance.getNbPeriods();
			int R = nbRoutes;
			
			// initialisation of all data
			openDepots = new int[J];
			deliveryDepot = new double[J][T];
			deliveryClient = new double[I][T];
			stockInitialDepot = new double[J];
			stockInitialClient = new double[I];
			stockDepot = new double[J][T];
			stockClient = new double[I][T];
			listOfRoutes = new int[R][T];
			coutRoute = new double[R];
			quantityDeliveredToDepot = new double[J][R][T];
			quantityDepotToClient = new double[I][J][T];
			
			for (int j=0;j<J;j++){
				{
				  	openDepots[j] =-1;
				}
			}

			// initialisation of inventory at depots (-1)
			for (int j=0;j<J; j++){
				stockInitialDepot[j] =instance.getInventoryInitialDepot(j);
			}
			
			
			// intialisation of inventory at clients (-1)
			for (int i=0;i<I; i++){
				stockInitialClient[i] =instance.getInventoryInitialClient(i);
			}
					

			
			// initialisation of inventory at depots (-1)
			for (int i=0;i<I;i++){
			for (int t=0;t<T;t++){
				for (int j=0;j<J; j++){
					deliveryDepot[j][t] =-1;
					stockDepot[j][t] =-1;
				}
			 }
			}
			
			// intialisation of inventory at clients (-1)
					for (int t=0;t<T;t++){
						for (int i=0;i<I; i++){
							deliveryClient[i][t] =-1;
							stockClient[i][t] =-1;
						}
					}
			
			// initialisation  of routes
										
					for (int t=0;t<T;t++){
						for (int r=0;r<R; r++){
							listOfRoutes[r][t] =-1;	
							coutRoute[r] =-1;
					}
					}
					
						
			// initialisation of the quantities delivered to depots
					for (int j=0;j<J;j++){
						for (int r=0;r<R; r++){
							for (int t=0;t<T;t++){
								quantityDeliveredToDepot[j][r][t] =-1;
							}
						}
					}
				
					
			// initialisation of the quantities delivered to clients
		/*	for (int i=0;i<I;i++){
				for (int t=0;t<T;t++){
					for (int r=0;r<R; r++){
						quantityDeliveredToClient[i][r][t] =-1;
					}
				}
			}*/
			
			// initialisation of the quantities delivered from depots to clients
					for (int i=0;i<I;i++){
						for (int j=0;j<J;j++){
							for (int t=0;t<T; t++){
								quantityDepotToClient[i][j][t] =-1;
							}
						}
					}
			
			
		}
		
		/*************************************
		 *********** METHODES ****************
		 *************************************/
		
		// Print the open depots
		public void printOpenDepots(){	
			System.out.println("----DEPOTS ----------------------  ");
			System.out.print("Open depots: \t");
			for (int j = 0; j<instance.getNbDepots(); j++){
				if (openDepots[j]==1) 
				{
					System.out.print(j+" \t  ");
				}
				
			}
			System.out.print("Depots not selected: \t");
			for (int j = 0; j<instance.getNbDepots(); j++){
				if (openDepots[j]==0) 
				{
					System.out.print(j+" \t  ");
				}
				
			}
			System.out.println();
			}
		
		
		// Print the inventory at depot for every period
		public void afficheStockDepot(){
			System.out.println("------ INVENTORY AT DEPOTS ---------");	
			
			System.out.print("Initial inventory :   ");
			for (int j=0;j<instance.getNbDepots(); j++){
						System.out.print(stockInitialDepot[j]+"\t");	
			}
			System.out.println();
		
			for (int t=0;t<instance.getNbPeriods();t++){
				System.out.print("Period "+t+":   ");
				for (int j=0;j<instance.getNbDepots(); j++){
							System.out.print(stockDepot[j][t]+"\t");	
				}
				System.out.println();
			}
		}
		
		
		// Print the quantities delivered at depots at every period
		public void 	printDeliveryDepot(){
			System.out.println("------ QUANTITY DELIVERED TO DEPOTS ---------");	
			for (int t=0;t<instance.getNbPeriods();t++){
				System.out.print("Period "+t+":   ");
				for (int j=0;j<instance.getNbDepots(); j++){
					double somme = 0;
					for (int r=0;r<instance.getNbRoutes(); r++){
							somme = somme + quantityDeliveredToDepot[j][r][t];
					}
					System.out.print(somme+"\t");
					
				}
				System.out.println();
			}
			
			System.out.println("------ QUANTITY DELIVERED FROM DEPOTS to CLIENTS ---------");	
			for (int t=0;t<instance.getNbPeriods();t++){
				System.out.println("Period "+t+":   ");
				for (int j=0;j<instance.getNbDepots(); j++){
					System.out.print("Depot "+j+": \t  ");
					for (int i=0;i<instance.getNbClients(); i++){
							System.out.print(quantityDepotToClient[i][j][t]+"\t");
					}
					System.out.println();
				}
				System.out.println();
			}
			
			
		}
		
			
		// Print the inventory at client for every period
		public void afficheStockClient(){
				System.out.println("------ INVENTORY AT CLIENTS ---------");	
				System.out.print("Initial inventory :");
				for (int i=0;i<instance.getNbClients(); i++){
							System.out.print(stockInitialClient[i]+"\t");	
				}
				System.out.println();
				
				
				for (int t=0;t<instance.getNbPeriods();t++){
					System.out.print("Period "+t+":   ");
					for (int i=0;i<instance.getNbClients(); i++){
								System.out.print(stockClient[i][t]+"\t");	
					}
					System.out.println();
					}
			}
		
		
		// Print routes
			public void printListOfRoutes(){
					System.out.println("------ LIST OF ROUTES ---------");	
					for (int t=0;t<instance.getNbPeriods();t++){
						System.out.print("Period "+t+":   ");
						for (int r=0;r<instance.getNbRoutes(); r++){
							if (listOfRoutes[r][t]==1) 
							{
								System.out.print(r+" \t  ");
							}
						}
						System.out.println();
					}
				}
		
			
			public void printDetailedListOfRoutes(){
				System.out.println("------ DETAILED ROUTES ---------");	
				for (int t=0;t<instance.getNbPeriods();t++){
					System.out.println("Period "+t+":   ");
					for (int r=0;r<instance.getNbRoutes(); r++){
						if (listOfRoutes[r][t]==1) 
						{
							System.out.print(" route " + r + " visiting depots : \t");
							for (int j=0;j<instance.getNbDepots();j++){
								double qu = quantityDeliveredToDepot[j][r][t]; 
								if (qu>0){
								System.out.print(j+" \t ");
								}
							}
							System.out.println();
						}
					}
				}
			}
			
			
				// Print the detail of each route 
			public void printDetailedRoutes(){
				System.out.println("----  DETAILED DELIVERIES AT CLIENTS -------------------------");
				for (int i=0;i<instance.getNbClients();i++){
					System.out.println(" Client " + i + ": \t");
					for (int t=0;t<instance.getNbPeriods();t++){
						System.out.print("Period "+t+": ");
							for (int j=0;j<instance.getNbDepots(); j++){
								double qu = quantityDepotToClient[i][j][t]; 
								//System.out.println(qu);
								if (qu>0){
									System.out.print(" depot " + j + "/ quantity = "+qu+ "\t");
								}
							}
							System.out.println();
					}
					System.out.println();
				}
			}	
			
			
			
			// Methode permettant de recalculer la fonction objectif
			 
			public void evaluate(){
				System.out.println("----------- RECALCULATION OF THE OBJECTIVE FUNCTION  ----- ");
				double objective1 = 0;
				double objective2 = 0;
				double objective3 = 0;
				double objective4 = 0;
				double objective5 = 0;
				
				//------------------------------------------------------------------
				for (int j = 0; j < instance.getNbDepots(); j++) {
					objective1 = objective1 + instance.getFixedCost(j)*openDepots[j]; 
				}
				System.out.println("Fixed cost of opening depots: "+ objective1);
				
				
				//------------------------------------------------------------------
				for (int t = 0; t < instance.getNbPeriods(); t++) {
					for (int r=0;r<instance.getNbRoutes(); r++){
						objective2 = objective2 + coutRoute[r]*listOfRoutes[r][t];   
					}
				}
				System.out.println("Cost of routes: "+ objective2);
				
				//------------------------------------------------------------------
				for (int t = 0; t < instance.getNbPeriods(); t++) {
					for (int j = 0; j <instance.getNbDepots() ; j++) {
						for (int i = 0; i < instance.getNbClients(); i++) {
							if( quantityDepotToClient[i][j][t]>0)
							{
								objective3 = objective3 + instance.getDeliveryCost(j, i);
							}
					}
				}
				}
				System.out.println("Cost of delivering clients: "+ objective3); 
					
				
				//------------------------------------------------------------------
			
				for (int t = 0; t < instance.getNbPeriods(); t++) {
					for (int j = 0; j < instance.getNbDepots(); j++) {
						objective4 = objective4 + instance.getHoldingCost(j)*stockDepot[j][t];   
					}
				}
				System.out.println("Inventory cost at depots: "+ objective4);
	
				//------------------------------------------------------------------
				
				for (int i = 0; i < instance.getNbClients(); i++) {
					for (int t = 0; t < instance.getNbPeriods(); t++) {
						objective5 = objective5 + instance.getHoldingCostClient()*stockClient[i][t];   
					}
				}
				System.out.println("Inventory cost at clients: "+ objective5);
				
				//------------------------------------------------------------------
				double objective = objective1+objective2+objective3+objective4+objective5;
					System.out.println("Objective function recalculated: "+ objective);
			}
			
			
		// PRINT SOLUTION
		public void print() { 
			printOpenDepots();
			printDeliveryDepot();
			afficheStockDepot();
			afficheStockClient();
			printListOfRoutes();
			printDetailedListOfRoutes();
			printDetailedRoutes();
			evaluate();	
		
		
		}
		
		
	}
