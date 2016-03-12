package testare;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;
import java.io.PrintWriter;
import java.util.ArrayList;
import java.util.Arrays;

import parse_tree.Node;
import parse_tree.PostfixToTree;
import parse_tree.Tree;
import parse_tree.Variable;
import expression.CharCheck;
import expression.CharQueue;
import expression.InfixToPostfix;

/**
 * @author apo
 *
 */
public class Test {
	/**
	 * numarul maxim de variabile valide ce poate aparea in input
	 */
	public static final int NR_MAX_VAR = 50; 
	/**
	 * numarul maxim de caractere ce il poate avea o linie din input
	 */
	public static final int NR_MAX_LINE_SIZE = 50; 
	/**
	 * numarul maxim de operatori ce poate aparea intr-o linie din input
	 */
	public static final int NR_MAX_LINE_OPERATORS = 20;
	
	/**
	 * @param args argumente introduse in linia de comanda
	 */
	public static void main(String[] args) {
		BufferedReader in = null;
		PrintWriter out1 = null;
		PrintWriter out2 = null;
		PrintWriter out3 = null;
		
		int linenr = 0; /* numar linie curenta */
		int variableCursor = 0;
		String expression; /* linie citita din fisierul input */
		Variable[] variables = new Variable[NR_MAX_VAR];
		for(int i = 0; i < NR_MAX_VAR; i++)
			variables[i] = new Variable("0", 0);

		try {
			in = new BufferedReader(new FileReader(args[0]));
			out1 = new PrintWriter(args[0] + "_pt");
			out2 = new PrintWriter(args[0] + "_sa");
			out3 = new PrintWriter(args[0] + "_ee");
			
			while((expression = in.readLine()) != null) {
				linenr++; 
				InfixToPostfix infixString = new InfixToPostfix(expression);
				String[] postfixNodes = infixString.converse();
				Node root = PostfixToTree.postfixToTree(postfixNodes);
				String levelExpression = new String();
				CharQueue queue = new CharQueue(NR_MAX_LINE_OPERATORS);
				ArrayList<ArrayList<Node>> lista = Tree.breadthFirst(root);
				int newCursor; /* arata pozitia din noul vector de caractere */
				for(int i = 0; i < lista.size(); i++) {
						if(i == 0) {
							levelExpression += 'E';
							queue.push('=');
							out1.println(levelExpression);
						}
						else {
							for(int j = 0; j < lista.get(i).size(); j++) {
								if(lista.get(i).get(j).getType() == 'E')
									queue.push(lista.get(i).get(j).getValue().charAt(0));
							}
							/* va memora expresia generalizata de pe fiecare linie */
							char[] expressionLine = new char[50]; 
							newCursor = 0;
							int k = 0; /* iterator */
							for(int j = 0; j < levelExpression.length(); j++) 
								if(levelExpression.charAt(j) != 'E') {
									expressionLine[newCursor] = levelExpression.charAt(j);
									newCursor++;
								} 
								else {
									String toAdd;
									if(i!=1 && i!=2 && CharCheck.isOperator(queue.peek())){
										/* se scriu tipurile in loc de valorile efective, adaugandu-se 
										   paranteze la exterior si operatorul intre ele  */
										toAdd = "(";
										toAdd += Character.toString(lista.get(i).get(k).getType());
										toAdd += queue.pop();
										toAdd += Character.toString(lista.get(i).get(k+1).getType());
										toAdd += ')';
									} 
									else { 
										/* altfel se procedeaza analogi, dar fara paranteze */
										toAdd = Character.toString(lista.get(i).get(k).getType());
										toAdd += queue.pop();
										toAdd += Character.toString(lista.get(i).get(k+1).getType());
									}
									
									k = k+2; /* deoarece am citit doua elemente, iteratorul creste cu 2 */
									char[] toAddArray = toAdd.toCharArray();
									
									for(int q = 0; q < toAddArray.length; q++) {
										/* se adauga elementele din toAdd in expressionLine */
										expressionLine[newCursor] = toAddArray[q];
										newCursor++;
									}
									
								}
							
							/* se pastreaza doar elementele completate ale vectorului */
							expressionLine = Arrays.copyOf(expressionLine, newCursor); 
							levelExpression = String.valueOf(expressionLine);
							out1.println(levelExpression);
							
						}	
						
				} 
				
				out1.println(); /* spatiu intre expresii */
				
				/* ---------------- aici se incheie prima cerinta ---------------- */

				int k = 1; /* cursor pentru vectorul de caractere al expresiei */
				char[] lineVariablesArray = expression.toCharArray();
				lineVariablesArray[0] = expression.charAt(0);
				for(int i = 1; i < expression.length(); i++) {
					/* transforma fiecare caracter care nu face parte din numele unei variabile in spatiu */
					char c = expression.charAt(i);
					char b = expression.charAt(i-1);
					if((Character.isDigit(c) && Character.isLetter(b)) || Character.isLetter(c))
						/* numele variabilei poate fi alcatuit din litere si eventual cifre la sfarsit */
						lineVariablesArray[k++] = expression.charAt(i);
					else
						lineVariablesArray[k++] = ' ';
				} 
				String[] lineVariables = String.valueOf(lineVariablesArray).replaceAll(" +", " ").split(" ");
				
				/* memoreaza coloanele la care incep numele variabilelor */
				int[] column = new int[NR_MAX_LINE_SIZE]; 
				int n = 1;
				column[0] = 1;
				newCursor = 0;
				for(int j = 1; j < expression.length(); j++) {
					char c = expression.charAt(j);
					char b = expression.charAt(j-1);
					if(Character.isLetter(c) && !Character.isLetter(b))
						column[n++] = j+1;
				}
				
				boolean variableFound = true;
				boolean validLine = false;
				for(int j = 1; j < lineVariables.length; j++) {
					variableFound = false;
					for(k = 0; k < variables.length; k++) {
						/* cauta variabila in vectorul de variabile folosind comparatorul de stringuri */
						if(variables[k].getName().equals(lineVariables[j])) {
							variableFound = true;
						}
					}
					
					if(variableFound == false){
						if(!Character.isLetter(lineVariables[0].charAt(0))){
							/* daca variabila nu a fost gasita, dar stringul nu este valid, se afiseaza eroare */
							out2.println("membrul stang nu este o variabila la linia " + linenr + " coloana 1");
							break;
						}
						else {
							/* daca variabila nu a fost gasita si stringul este valid, se afiseaza alta eroare */
							out2.println(lineVariables[j] + " nedeclarata la linia " + linenr + " coloana " + column[j]);
							break;
						}
					}
						
				}
				
				if(variableFound == true) 
					/*  daca nu a fost gasita nicio variabila invalida in afara de prima, se testeaza prima */
					if(!Character.isLetter(lineVariables[0].charAt(0)))
						out2.println("membrul stang nu este o variabila la linia "+ linenr+ " coloana 1");
					else {
						variables[variableCursor] = new Variable(lineVariables[0],0);
						out2.println("Ok!");
						validLine = true; /* expresia este valida */
					}
				
				/* ---------------- aici se incheie a doua cerinta ---------------- */
				
				if(validLine == true) {
					/* intr-un final, daca expresia e valida, atunci se poate face evaluarea */
					int r = Tree.evaluation(root.getRightChild(),variables); 
					/* evaluarea se executa pentru arborele drept fata de '=' */
					String temp = variables[variableCursor].getName();
					variables[variableCursor] = new Variable(temp,r); /* stocare variabila in vector */
					out3.println(variables[variableCursor].getName() + '=' + variables[variableCursor].getValue());
					variableCursor++;
				}
				else 
					out3.println("Error!");
				
				/* ---------------- aici se incheie a treia cerinta ---------------- */
			}
			
		} catch (IOException e) {
			e.printStackTrace();
		} finally {
			if (in != null) {
				try {
					in.close();
					out1.close();
					out2.close();
					out3.close();
				} catch (IOException e) {
					// error closing file: oh well...
				}
			}
		}

	
	}
	
}

