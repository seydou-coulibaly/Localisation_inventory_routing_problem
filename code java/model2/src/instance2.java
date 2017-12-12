import java.io.File;
import java.io.IOException;
import java.util.Scanner;


import java.io.File;
import java.io.IOException;
import java.util.Scanner;
import java.math.BigDecimal;

public class instance2 {

	
		// TODO Auto-generated constructor stub
		
		private int nbDepots;
		private int nbPeriods;
		private int nbClients; 
		private int nbSites; 
		private int HoldingCostClient;
		
		private int capacityDepot;
		private int capacityClient;
		 
		private double[][]Demand; //demand for each customer in each period 
		private int[]FixedCost; //cost of opening for candidate depot NAME
		private double[][]DeliveryCost; //cost of delivering from depot j to client i 
		private int K;
		private int VehicleCapacity; //capacity of each vehicle 
		public int []InventoryInitialDepot ; // initial inventory at depot j 
		public int []InventoryInitialClient ; // initial inventory at client i 
		private int []HoldingCost; //holding cost for depot i in period t   it should be in double
		private int[][]coord;
		
		
		
		//private double speed;  
		

		private int nr; // nombre de routes créées
		
		
		public static double epsilon = 0.0001;
		public int RMAX = 100;  // nombre max de routes dans le modèle
		public int DMAX = 4;     // nombre max de clients sur une route
		public int LMAX = 15;   // CHECK 
		
		
		/*****************************************
		 *********** ACCESSEURS *****************
		 *****************************************/
		
		public int getNbDepots() { //is giving the number of depots in other classes 
			return nbDepots;
		}
		
		public int getNbClients() { //is giving the number of clients in other classes 
			return nbClients;
		}
		
		public int getNbPeriods() {
			return nbPeriods;
		}
		
		public int getNbSites() {
			
			return nbSites;
		}
		
		public int getCapacityVehicle() {
			return VehicleCapacity;
		}
		
		public void setNbDepots(int nbDepots) {     
		}
		

		public void setNbClients(int nbClients) {     
			this.nbClients = nbClients;
		}
		
		public void setNbPeriods (int nbPeriods) {
			this.nbPeriods = nbPeriods;
		}
		
		
		public int getCapacityDepot() {
			return capacityDepot;
		}
		
		public int getCapacityClient() {
			return capacityClient;
		}
		

		public double getDemand (int i, int t) {
			return Demand[i][t];
		}
		
		public double getFixedCost (int j) { //I ADDED THIS FOR THE OBJ FUNCTION (with j or nbDepots)
			return FixedCost[j];
		}
		
		public double getDeliveryCost (int j, int i) { //I ADDED THIS FOR THE OBJ FUNCTION (with j or nbDepots)
			return DeliveryCost[j][i];
		}
		
		public double getHoldingCost (int i) { //I ADDED THIS FOR THE OBJ FUNCTION (with i--- nbClients)
			return HoldingCost[i];
		}
		
		public double getInventoryInitialDepot (int j) { //I ADDED THIS FOR THE OBJ FUNCTION (with i--- nbClients)
			return InventoryInitialDepot[j];
		}
		
		public double getInventoryInitialClient (int i) { //I ADDED THIS FOR THE OBJ FUNCTION (with i--- nbClients)
			return InventoryInitialClient[i];
		}
		
		public int getDMAX() { //max number of clients 
			return DMAX;
		}
		
		public int getNbRoutes() { //nr of routes
			return nr;
		}
		
		public int getNbVehicles() { //max number of vehicles
			return K;
		}
		
		public int getHoldingCostClient() {
		 return HoldingCostClient;	
		}
		

		/*****************************************
		 *********** CONSTRUCTEUR ****************
		 *****************************************/

		/**
		 * Constructeur d'une instance à partir d'un fichier de données
		 * @param nomFichier
		 * @throws IOException
		 */
		public instance2 (String nomFichier) throws IOException {

			File mfile = new File(nomFichier); //create a new file with the name 
			if (!mfile.exists()) {
				throw new IOException("Le fichier saisi : " + nomFichier + ", n'existe pas."); //translate the file 
			}
			Scanner sc = new Scanner(mfile); //reads the data file line by line ; sc name of the scanner

			nbDepots = sc.nextInt();	sc.nextLine();   //nb sites , next integer value it scans ! storing the values from the file
			nbClients = sc.nextInt(); sc.nextLine(); 
			nbPeriods = sc.nextInt();	sc.nextLine();
			K = sc.nextInt();	sc.nextLine();
			nbSites = nbClients + nbDepots; 
			
			VehicleCapacity = sc.nextInt();	sc.nextLine(); //vehicle capacity
			
			
			capacityDepot= sc.nextInt();	sc.nextLine(); 
			capacityClient= sc.nextInt();	sc.nextLine();  
			
			
			// fixed cost of opening each depots
			FixedCost = new int[nbDepots];
			for (int i=0;i<nbDepots;i++){
				FixedCost[i]= sc.nextInt();
				}
			sc.nextLine();
			
		
			
			InventoryInitialDepot = new int[nbDepots];  //initial inventory at each depot j  
			for (int i=0;i<nbDepots;i++){
					InventoryInitialDepot[i]= sc.nextInt();
			}
			sc.nextLine();
			
			InventoryInitialClient = new int[nbClients];  //initial inventory at each depot j  
			for (int i=0;i<nbClients;i++){
					InventoryInitialClient[i]= sc.nextInt();
			}
			sc.nextLine();
				
			 
			HoldingCost = new int[nbDepots];  //holding cost   convert to double
			for (int j=0;j<nbDepots;j++){
				HoldingCost[j]= sc.nextInt();  
				sc.nextLine();
			}
		
			HoldingCostClient = sc.nextInt(); sc.nextLine(); //holding cost   convert to double
			
			
			Demand = new double[nbClients][nbPeriods];  //for demand
			for (int i=0;i<nbClients;i++){
				for (int j=0;j<nbPeriods;j++){
					Demand[i][j]= sc.nextInt();
				}
				sc.nextLine();
			}

			// to take the coordinates [i][0], [i][1]] 
			coord = new int[nbClients + nbDepots+ 1][2];
			for (int i=0;i<nbClients + nbDepots+1;i++){
				for (int j=0;j<2;j++){
					coord[i][j]= sc.nextInt();
				}
				sc.nextLine();
			}
			
			int code = sc.nextInt(); 
		
			
			if (code==9999){
				System.out.println("Lecture données OK\t");	 //to see if we read the file 
			} 
			
				sc.close();
		}

		
		
		double[][] calculateDeliveryCost(){
			
	
			DeliveryCost = new double[nbDepots][nbClients];
			for (int j=nbClients;j<nbSites;j++){
				for (int i=0;i<nbClients;i++){
				DeliveryCost[j-nbClients][i]= getDistance(j,i);
				}
			}
		
			return DeliveryCost;	
			
		}
		
		
		
		//-----------------------------------------------------------------------------------------------
		// Génération de toutes les routes réalisables 
		
		double[][] calculateRoutesModel2(){  //returns a table for all the routes
			int DMAX=4;
				double[][] listeRoutes = new double[RMAX][DMAX+1];  //  sides  , max number of customers 
				for( int r= 0 ; r<RMAX ; r++ ){
					for (int c=0;c<DMAX+1;c++){
						listeRoutes[r][c] = -1;  // initialisation
					}
				}
						
				double cout = 0; //temporrary variable for cout
				nr = 0;  // number of routes
						
				// Ensemble de routes avec 1 depot; distance between i and j 
				int i= nbSites; //depot
					for(int j= nbClients; j< nbSites; j++){ //depot
					cout = getDistance(i,j) * 2 ;
						if (cout <= LMAX) {
						// On crée une nouvelle route r, de cout "cout", et avec l'élément i
						if (nr<RMAX) {
							listeRoutes[nr][0] =  Math.round(cout* 100) /100 ;
							listeRoutes[nr][1] = j; 
							
						//	System.out.println("depot is " + i + " customer is " + j + " cost is " + listeRoutes[nr][0] );
							
							nr = nr+1;
						}
					}
				}
				

			
					// Ensemble de routes avec 2 clients i - j - k
					if (DMAX>1){
							
				    {
					for (int j=nbClients;j<nbSites;j++)	{
						for (int k=j+1;k<nbSites;k++)	{
							double cout1 = getDistance(i,j)+getDistance(j,k)+getDistance(k,i);
							double cout2 = getDistance(i,k)+getDistance(k,j)+getDistance(j,i);
							cout = mindouble (cout1,cout2);
							if (cout <= 9999) {
								// On crée une nouvelle route r, de cout "cout", et avec les sites i et j
								if (nr<RMAX) {
									listeRoutes[nr][0] = Math.round(cout* 100) /100 ;
									listeRoutes[nr][1] = j;
									listeRoutes[nr][2] = k;
									// la route r est ajoutée à la liste de routes
									nr = nr+1;
								}
							}
						}
					}
					}
						}
					
					
					// Distance between 3 depots i- j - k - l 
					if (DMAX>2){
						{
								for (int j=nbClients;j<nbSites;j++)	{
									for (int k=j+1;k<nbSites;k++)	{
										for (int l=k+1;l<nbSites;l++)	{
								
								double cout1 = getDistance(i,j)+getDistance(j,k)+getDistance(k,l)+getDistance(l,i);
								double cout2 = getDistance(i,k)+getDistance(k,j)+getDistance(j,l)+getDistance(l,i);
								double cout3 = getDistance(i,l)+getDistance(l,j)+getDistance(j,k)+getDistance(k,i);
								double cout4 = getDistance(i,j)+getDistance(j,l)+getDistance(l,k)+getDistance(k,i);
								double cout5 = getDistance(i,k)+getDistance(k,l)+getDistance(l,j)+getDistance(j,i);
								double cout6 = getDistance(i,l)+getDistance(l,k)+getDistance(k,j)+getDistance(j,i);							
								cout = mindouble(cout1,cout2,cout3,cout4,cout5,cout6); 	
								if (cout <= LMAX) {
									// On crée une nouvelle route r, de cout "cout", et avec les sites i et j
									if (nr<RMAX) {
										listeRoutes[nr][0] = Math.round(cout* 100) /100 ; //cost of the route is the length 
										listeRoutes[nr][1] = j;
										listeRoutes[nr][2] = k;
										listeRoutes[nr][3] = l;
										nr = nr+1;
									}
								}
							}
						}
					}
					}
					}
					
					// Ensemble de routes avec 4 depots 
					if (DMAX>3){
						{
						for (int j=nbClients;j<nbSites;j++)	{
							for (int k=j+1;k<nbSites;k++)	{
								for (int l=k+1;l<nbSites;l++)	{
									for (int m=l+1;m<nbSites;m++)	{
									
									double cout1 = getDistance(i,j)+getDistance(j,k)+getDistance(k,l)+getDistance(l,m)+ getDistance(m,i);
									double cout2 = getDistance(i,j)+getDistance(j,k)+getDistance(k,m)+getDistance(m,l)+ getDistance(l,i);
									double cout3 = getDistance(i,j)+getDistance(j,m)+getDistance(m,k)+getDistance(k,l)+ getDistance(l,i);
									double cout4 = getDistance(i,j)+getDistance(j,m)+getDistance(m,l)+getDistance(l,k)+ getDistance(k,i);
									double cout5 = getDistance(i,j)+getDistance(j,l)+getDistance(l,m)+getDistance(m,k)+ getDistance(k,i);
									double cout6 = getDistance(i,j)+getDistance(j,l)+getDistance(l,k)+getDistance(k,m)+ getDistance(m,i);
										//
									double cout7 = getDistance(i,k)+getDistance(k,l)+getDistance(l,m)+getDistance(m,j)+ getDistance(j,i);
									double cout8 = getDistance(i,k)+getDistance(k,m)+getDistance(m,l)+getDistance(l,j)+ getDistance(j,i);
									double cout9 = getDistance(i,k)+getDistance(k,j)+getDistance(j,m)+getDistance(m,l)+ getDistance(l,i);
									double cout10 = getDistance(i,k)+getDistance(k,j)+getDistance(j,l)+getDistance(l,m)+ getDistance(m,i);
									double cout11 = getDistance(i,k)+getDistance(k,m)+getDistance(m,j)+getDistance(j,l)+ getDistance(l,i);
									double cout12 = getDistance(i,k)+getDistance(k,l)+getDistance(l,j)+getDistance(j,m)+ getDistance(m,i);
										//
							
									double cout13 = getDistance(i,l)+getDistance(l,m)+getDistance(m,j)+getDistance(j,k)+ getDistance(k,i);
									double cout14 = getDistance(i,l)+getDistance(l,m)+getDistance(m,k)+getDistance(k,j)+ getDistance(j,i);
									double cout15 = getDistance(i,l)+getDistance(l,j)+getDistance(j,m)+getDistance(m,k)+ getDistance(k,i);
									double cout16 = getDistance(i,l)+getDistance(l,j)+getDistance(j,k)+getDistance(k,m)+ getDistance(m,i);
									double cout17 = getDistance(i,l)+getDistance(l,k)+getDistance(k,m)+getDistance(m,j)+ getDistance(j,i);
									double cout18 = getDistance(i,l)+getDistance(l,k)+getDistance(k,j)+getDistance(j,m)+ getDistance(m,i);
									//
									double cout19 = getDistance(i,m)+getDistance(m,j)+getDistance(j,k)+getDistance(k,l)+ getDistance(l,i);
									double cout20 = getDistance(i,m)+getDistance(m,j)+getDistance(j,l)+getDistance(l,k)+ getDistance(k,i);
									double cout21 = getDistance(i,m)+getDistance(m,k)+getDistance(k,j)+getDistance(j,l)+ getDistance(l,i);
									double cout22 = getDistance(i,m)+getDistance(m,k)+getDistance(k,l)+getDistance(l,j)+ getDistance(j,i);
									double cout23 = getDistance(i,m)+getDistance(m,l)+getDistance(l,k)+getDistance(k,j)+ getDistance(j,i);
									double cout24 = getDistance(i,m)+getDistance(m,l)+getDistance(l,j)+getDistance(j,k)+ getDistance(k,i);
						
									
									
									
									cout1 =  mindouble(cout1,cout2,cout3,cout4,cout5,cout6,cout7,cout8,cout9,cout10,cout11,cout12) ;
									cout2 =  mindouble(cout13,cout14,cout15,cout16,cout17,cout18,cout19,cout20,cout21,cout22,cout23,cout24) ;
									cout = mindouble(cout1,cout2);
									if (cout <= LMAX) {
										// On crée une nouvelle route r, de cout "cout", et avec les sites i et j
										if (nr<RMAX) {
											listeRoutes[nr][0] = Math.round(cout* 100) /100 ;
											listeRoutes[nr][1] = j;
											listeRoutes[nr][2] = k;
											listeRoutes[nr][3] = l;
											listeRoutes[nr][4] = m;
											nr = nr+1;
										}
									}
								}
							}
						}
					}
					}
					}
					return listeRoutes;
					
			
						}
		

		//------------------------------------------------------------------------------------------
// Calcule la distance euclienne entre deux sites
double getDistance(int i, int j){
	double d = (coord[i][0]-coord[j][0])*(coord[i][0]-coord[j][0])+ (coord[i][1]-coord[j][1])*(coord[i][1]-coord[j][1]);
	d = Math.sqrt(d);
	return d;
}




//-----------------------------------------------------------------
// mindouble returns the minimum value among several (double) values
public static double mindouble(double ... numbers) {
    double min = numbers[0];
    for (int i=1 ; i<numbers.length ; i++) {
        min = (min <= numbers[i]) ? min : numbers[i];
    }
    return min;
}
		
					
		
		// make the routes
		}

