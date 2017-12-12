import java.io.File;
import java.io.IOException;
import java.io.PrintStream;
import ilog.concert.IloException;



public class Main2 {

		// TODO Auto-generated constructor stub
		
		/**
		   * @param args
		   * @throws IOException
		   * @throws IloException
		   */
			
		 
		public static void main(String[] args) throws IOException, IloException {
				 
		   // Resolution de toutes les instances presentes dans le repertoire instance
		   String instanceDirectory = "instances/"; //ALL MY DATA IS HERE
		   //String instanceDirectory = "debug/";
		   File listSol = new File(instanceDirectory);
		   for (String fileName : listSol.list()) { //IN THE DIRECTORY FOR EVERY FILE, YOU SOLVE AND SAVE

		      // Lecture de l'instance
		      instance2 instance = new instance2(instanceDirectory + fileName);//CREATE AN OBJECT
		      System.out.print("Resolution du fichier " + fileName + "...");

		     
		      String fichierLog = "./log/" + fileName.replace(".txt", ".log"); // CREATE A LOGE FILE
		      String fichierSol = "./sol/" + fileName.replace(".txt", ".sol");
		      File fileLog = new File(fichierLog);
		      PrintStream printStreamLog = new PrintStream(fileLog);
		      // Les sorties out et err sont redirigees dans le fichier de log
		      PrintStream original = System.out;
		      System.setOut(printStreamLog);
		      System.setErr(printStreamLog);
		     File fileSol = new File(fichierSol);
		      // Stream pour la solution
		      PrintStream printStreamSol = new PrintStream(fileSol);

		      
		      // Appel de la methode de resolution
		     solution2 sol = solveur2ndproblem.solve(instance, printStreamSol); 

		      if (sol != null){
		    	  sol.print();
		      }
		      else
		      {
		    	  System.out.println("Error on this instance");
		      }
		      
		      System.setOut(original);
		      System.out.println("Instance solved.");
		    
		   }
		   
		   System.out.println("All Instances solved. FINISHED :-)");
		   

		  }	
		
		
	}
