import java.io.File;
import java.io.IOException;
import java.util.Scanner;
import java.math.BigDecimal;


public class Instance {

	private int nbDepots;
	private int nbPeriods;
	private int nbClients; 
	private int nbSites; 
	 
	private double[][]Demand; //demand for each customer in each period 
	private int[]FixedCost; //cost of opening for candidate depot NAME
	private int[]OrderCost; //cost of ordering from plant to depot
	private int K; 			// max number of vehicle (constraint 7
	private int vehicleCapacity; //capacity of each vehicle 
	public int []InventoryInitialDepot ;//initial inventory at depot j
	public int []InventoryInitialClient ;// initial inventory at customer i 
	private double [] holdingCost; //holding cost for depot i in period t   it should be in double
	private int[][]coord;
	private int capacityDepot;
	private int capacityClient;
	private double holdingCostClient; 
	
	
	//private double speed;  
	

	private int nr; // nombre de routes créées
	
	
	public static double epsilon = 0.0001;
	public int RMAX = 100;  // nombre max de routes dans le modèle
	public int CMAX = 4;     // nombre max de clients sur une route
	public int LMAX = 9999;   // CHECK 
	
	
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
		return vehicleCapacity;
	}
	
	public int getCapacityDepot() {
		return capacityDepot;
	}
	
	public int getCapacityClient() {
		return capacityClient;
	}
	
	
	
	public void setNbDepots(int nbDepots) {     
		this.nbDepots = nbDepots;
	}
	

	public void setNbClients(int nbClients) {     
		this.nbClients = nbClients;
	}
	
	public void setNbPeriods (int nbPeriods) {
		this.nbPeriods = nbPeriods;
	}
	

	public double getDemand (int i, int t) {
		return Demand[i][t];
	}
	
	public double getFixedCost (int j) { //I ADDED THIS FOR THE OBJ FUNCTION (with j or nbDepots)
		return FixedCost[j];
	}
	
	public double getOrderCost (int j) { //I ADDED THIS FOR THE OBJ FUNCTION (with j or nbDepots)
		return OrderCost[j];
	}
	
	public double getHoldingCost (int j) { //I ADDED THIS FOR THE OBJ FUNCTION (with i--- nbClients)
		return holdingCost[j];
	}
	
	public double getHoldingCostClient() { // added to the objective function
		return holdingCostClient;
	}
	
	
	public double getInventoryInitialDepot (int j) { //I ADDED THIS FOR THE OBJ FUNCTION (with i--- nbClients)
		return InventoryInitialDepot[j];
	}
	
	public double getInventoryInitialClient (int i) { //I ADDED THIS FOR THE OBJ FUNCTION (with i--- nbClients)
		return InventoryInitialClient[i];
	}
	
	public int getCMAX() { //max number of clients 
		return CMAX;
	}
	
	public int getNbRoutes() { //nr of routes
		return nr;
	}
	
	public int getNbVehicles() { //max number of vehicles
		return K;
	}
	
		

	/*****************************************
	 *********** CONSTRUCTEUR ****************
	 *****************************************/

	/**
	 * Constructeur d'une instance à partir d'un fichier de données
	 * @param nomFichier
	 * @throws IOException
	 */
	public Instance(String nomFichier) throws IOException {

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
				
		vehicleCapacity = sc.nextInt();	sc.nextLine(); //vehicle capacity
		
		capacityDepot= sc.nextInt();	sc.nextLine(); 
		capacityClient= sc.nextInt();	sc.nextLine();  
		
		// fixed cost of opening each depots
		FixedCost = new int[nbDepots];
		for (int i=0;i<nbDepots;i++){
			FixedCost[i]= sc.nextInt();
			}
		sc.nextLine();
		
		// ordering cost for each depots
		OrderCost = new int[nbDepots];
		for (int i=0;i<nbDepots;i++){
			OrderCost[i]= sc.nextInt();
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
		
		 
		holdingCost = new double[nbDepots];  //holding cost   convert to double
		for (int i=0;i<nbDepots;i++){
				holdingCost[i]= sc.nextInt();
				holdingCost[i] = holdingCost[i]/100;
				sc.nextLine();
		}
		
		holdingCostClient =  sc.nextInt();	sc.nextLine(); 
		holdingCostClient = holdingCostClient/100;
	
		
		Demand = new double[nbClients][nbPeriods];  //for demand
		for (int i=0;i<nbClients;i++){
			for (int j=0;j<nbPeriods;j++){
				Demand[i][j]= sc.nextInt();
			}
			sc.nextLine();
		}

		// to take the coordinates [i][0], [i][1]] 
		coord = new int[nbClients + nbDepots+1][2];
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

	
	//-----------------------------------------------------------------------------------------------
	// Génération de toutes les routes réalisables 
	
	double[][] calculateRoutes(){  //returns a table for all the routes
		
		double[][] listeRoutes = new double[RMAX][CMAX+2];  //  sides  , max number of customers 
		for( int r= 0 ; r<RMAX ; r++ ){
			for (int c=0;c<CMAX+2;c++){
				listeRoutes[r][c] = -1;  // initialisation
			}
		}
				
		double cout = 0; //temporary variable for route cost
		nr = 0;  // number of routes
		
		System.out.println();
		System.out.println("-------  ROUTE GENERATION --------------------");
		System.out.println("Generating all routes with at most "+CMAX+" clients, and length not larger than "+LMAX);
		System.out.println();


		// Ensemble de routes avec 1 client; distance between i and j 
		for (int i=nbClients;i<nbSites;i++){ //depot
			for(int j= 0; j< nbClients; j++){ //client
			cout = getDistance(i,j) * 2 ;
				if (cout <= LMAX) {
				// On crée une nouvelle route r, de cout "cout", et avec l'élément i
				if (nr<RMAX) {
					listeRoutes[nr][0] =  round(cout,2);
					listeRoutes[nr][1] = i;
					listeRoutes[nr][2] = j ; 
					System.out.println("Route "+nr+": depot = "+i+" clients = "+j+" \t\t cost = "+round(cout,2));
					nr = nr+1;
				}
			}
		}
		}
		
	
			// Ensemble de routes avec 2 clients  j - k
				if (CMAX>1){
			for (int i=nbClients;i<nbSites;i++){
				for (int j=0;j<nbClients;j++)	{
					for (int k=j+1;k<nbClients;k++)	{
					double cout1 = getDistance(i,j)+getDistance(j,k)+getDistance(k,i);
					double cout2 = getDistance(i,k)+getDistance(k,j)+getDistance(j,i);
					cout = mindouble (cout1,cout2);
					if (cout <= LMAX) {
						// On crée une nouvelle route r, de cout "cout", et avec les sites i et j
						if (nr<RMAX) {
							listeRoutes[nr][0] = round(cout, 2);
							listeRoutes[nr][1] = i;
							listeRoutes[nr][2] = j;
							listeRoutes[nr][3] = k; 
							System.out.println("Route "+nr+": depot = "+i+" clients = "+j+" "+k+" \t cost = "+round(cout,2));
						nr = nr+1;
						}
					}
				}
			}
			}
			}
			
			// Distance between 3 clients i- j - k - l 
			if (CMAX>2){
			for (int i=nbClients;i<nbSites;i++){
				for (int j=0;j<nbClients;j++)	{
					for (int k=j+1;k<nbClients;k++)	{
						 for (int l=k+1;l<nbClients;l++)	{
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
								listeRoutes[nr][0] = round(cout,2); //cost of the route is the length 
								listeRoutes[nr][1] = i;
								listeRoutes[nr][2] = j;
								listeRoutes[nr][3] = k;
								listeRoutes[nr][4] = l; 
								System.out.println("Route "+nr+": depot = "+i+" clients = "+j+" "+k+" "+l+" \t cost = "+round(cout,2));
								nr = nr+1;
							}
						}
					}
				}
			}
			}
			}

			// Ensemble de routes avec 4 clients
			if (CMAX>3){
			for (int i=nbClients;i<nbSites;i++){
				for (int j=0;j<nbClients;j++)	{
					for (int k=j+1;k<nbClients;k++)	{
						for (int l=k+1;l<nbClients;l++)	{
							for (int m=l+1;m<nbClients;m++)	{
							
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
				
							cout =  mindouble(cout1,cout2,cout3,cout4,cout5,cout6,cout7,cout8,cout9,cout10,cout11,cout12,cout13,cout14,cout15,cout16,cout17,cout18,cout19,cout20,cout21,cout22,cout23,cout24) ;
							if (cout <= LMAX) {
								// On crée une nouvelle route r, de cout "cout", et avec les sites i et j
								if (nr<RMAX) {
									listeRoutes[nr][0] = round(cout,2);
									listeRoutes[nr][1] = i;
									listeRoutes[nr][2] = j;
									listeRoutes[nr][3] = k;
									listeRoutes[nr][4] = l;
									listeRoutes[nr][5] = m;
									System.out.println("Route "+nr+": depot = "+i+" clients = "+j+" "+k+" "+l+" "+m+" \t cost = "+round(cout,2));
									nr = nr+1;
								}
							}
						}
					}
				}
			}
			}
			}
			System.out.println("------- END OF ROUTE GENERATION --------------------");
			System.out.println();
				
			return listeRoutes;
			
	
	}
	
	
	
	//------------------------------------------------------------------------------------------
	// Calcule la distance euclienne entre deux sites
	double getDistance(int i, int j){
		//System.out.println("i = "+i+" \t j = "+j);
	//	System.out.println("i = ["+coord[i][0]+","+coord[i][1]+"] \t\t j = ["+coord[j][0]+","+coord[j][1]+"]" );
			
		double d = (coord[i][0]-coord[j][0])*(coord[i][0]-coord[j][0])+ (coord[i][1]-coord[j][1])*(coord[i][1]-coord[j][1]);
		d = Math.sqrt(d);
		//System.out.println("distance "+i+" " +j+ " = " + d);
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
	
	
	// rounding with d decimals
	
	    public static double round(double value, int numberOfDigitsAfterDecimalPoint) {
	        BigDecimal bigDecimal = new BigDecimal(value);
	        bigDecimal = bigDecimal.setScale(numberOfDigitsAfterDecimalPoint,
	                BigDecimal.ROUND_HALF_UP);
	        return bigDecimal.doubleValue();
	    }
	
	
	
}
