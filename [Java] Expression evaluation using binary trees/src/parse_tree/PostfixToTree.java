package parse_tree;

import expression.CharCheck;

/**
 * Clasa contine metoda ce formeaza un tree din vectorul de siruri de caractere corespunzator formei postfix.
 */
public class PostfixToTree {
	/**
	 * @param postfix expresia in postfix, "vectorizata" 
	 * @return radacina tree-ului format
	 */
	public static Node postfixToTree(String[] postfix) {
		
		Node root;
		NodeStack stack = new NodeStack(20); // stiva de noduri
		
		for(int i = 0; i <= postfix.length - 1; i++) {
			
			char c = postfix[i].charAt(0); // se citeste primul caracter
			boolean flag = false;
			
			if(postfix[i].length() >= 2) { /* aflam daca avem de a face cu + sau - unar; daca da => flag = true */
				char d = postfix[i].charAt(1); 
				if((c=='+' || c=='-') && CharCheck.isAlphanumerical(d))
					flag = true;
			}
			
			if(CharCheck.isAlphanumerical(c) || flag == true) {
				/* daca elementul este o variabila sau un numar (eventual cu + sau - unar in fata), il adaugam 
				  in stiva asa cum e, fara a-i declara si tipul (operator, expresie, etc) */
				Node newnode = new Node(postfix[i]);
				stack.push(newnode);
			}
			
			else {
				/* elementul nu este o variabila sau un numar, deci este un operator */

				Node op1 = stack.pop();
				Node exp;
				if( !op1.isLeaf() ){
					/* daca operandul nu este frunza, inseamna ca fii sai formeaza o expresie */
					exp = new Expresie(op1.getValue(), op1.left, op1.right);
				}
				else if(c == '*') {
					/* daca operandul este frunza si parintele sau este *,  avem de a face cu un factor */
					exp = new Factor(op1.getValue(), op1.left, op1.right);
				}
				else if(c == '+' || c == '-' || c == '=') {
					/* daca operandul este frunza si parintele sau este +, - sau =, avem de a face cu un termen */
					exp = new Termen(op1.getValue(), op1.left, op1.right);
				}
				else
					/* daca operandul nu este frunza, nici +, *, - sau =, atunci avem de a face cu o operatie a>b?c:d */
					exp = new Ternar(op1.getValue(), op1.left, op1.right);
				
				/* pentru al doilea operand se procedeaza in mod analog */
				Node exp2;
				Node op2 = stack.pop();
				if( !op2.isLeaf() ){
					exp2 = new Expresie(op2.getValue(), op2.left, op2.right);
				}
				else if(c == '*') {
					exp2 = new Factor(op2.getValue(), op2.left, op2.right);
				}
				else if(c == '+'|| c == '-' || c == '=') {
					exp2 = new Termen(op2.getValue(), op2.left, op2.right);
				}
				else
					exp2 = new Ternar(op2.getValue(), op2.left, op2.right);
				
				/* formez nodul nou cu cu fii de tipul corespunzator si il introduc inapoi in stiva */
				Node newnode = new Expresie(postfix[i], exp2, exp);
				stack.push(newnode);
			}
		}
		
		/* dupa toate operatiile de mai sus, mai ramane sa extragem ultimul element din stiva, adica radacina arborelui */
		root = stack.pop();
		return root;
	}
	
	
}
