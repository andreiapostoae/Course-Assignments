package parse_tree;

import java.util.ArrayList;
import java.util.LinkedList;
import java.util.Queue;


/**
 * Clasa corespunzatoare arborelui.
 */
public class Tree {
	Node root;
	
	/**
	 * Constructor parametrizat.
	 */
	public Tree(Node root) {
		this.root = root;
	}
	
	/**
	 * @param root radacina arborelui de evaluat
	 * @param var vectorul de variabile impreuna cu valorile lor
	 * @return rezultatul evaluarii
	 */
	
	public static int evaluation(Node root, Variable[] var) {
		
		if(root.getType() == 'E') {
			int op1 = evaluation(root.getLeftChild(), var);
			int op2 = evaluation(root.getRightChild(), var);
			
			switch (root.getValue()) {
			case "+":
				return op1 + op2;
			case "-":
				return op1 - op2;
			case "*":
				return op1 * op2;
			case ">": {
				if(op1 > op2)
					return Integer.MAX_VALUE;
				return op1;
			}
			case "<": {
				if(op1 < op2)
					return Integer.MAX_VALUE;
				return op1;
			}
			case "?": {
				if(op1 == Integer.MAX_VALUE)
					return op2;
				else
					return Integer.MAX_VALUE;
			}
			case ":": {
				if(op1 != Integer.MAX_VALUE)
					return op1;
				return op2;
			}
			default:
				return 0;
			}
		}
		
		char c = root.getValue().charAt(0);
		if(Character.isDigit(c) || ((c == '-' || c == '+') && root.getValue().length() > 1))
			/* daca intalnim un numar, chiar si cu + sau - in fata, il returnam ca un intreg */
			return Integer.parseInt(root.getValue());

		
		/* daca pana aici nu s-a executat return, inseamna ca suntem in cazul in care am intalnit o variabila
		 * ceea ce solicita cautarea valorii ei efective in vectorul de variabile */
		int i;
		for(i = 0; i < var.length; i++)
			if(var[i].getName().equals(root.getValue()))
				break;
		return var[i].getValue();
	}
	
	
	/**
	 * @param root radacina copacului a carui parcurgere o realizam
	 * @return "matricea" de elemente <p>
	 * Algoritmul folosit pentru parcurgere pe latime a fost cel de la
	 * http://stackoverflow.com/questions/2241513/java-printing-a-binary-tree-using-level-order-in-a-specific-format
	 * adaptat la problema corecta.
	 */
	public static ArrayList<ArrayList<Node>> breadthFirst(Node root) {
	    ArrayList<ArrayList<Node>> result = new ArrayList<ArrayList<Node>>(); 
	    if(root == null) return result;
	    Queue<Node> q1 = new LinkedList<Node>();
	    Queue<Node> q2 = new LinkedList<Node>();

	    ArrayList<Node> list = new ArrayList<Node>();
	    q1.add(root);

	    while(!q1.isEmpty() || !q2.isEmpty()) {
	        while(!q1.isEmpty()){
	            Node temp = (Node)q1.poll();
	            list.add(temp);
	            if(temp.left != null) q2.add(temp.left);
	            if(temp.right != null) q2.add(temp.right);
	        }
	        if(list.size() > 0)result.add(new ArrayList<Node>(list));
	        list.clear();
	        while(!q2.isEmpty()){
	            Node temp = (Node)q2.poll();
	            list.add(temp);
	            if(temp.left != null) q1.add(temp.left);
	            if(temp.right != null) q1.add(temp.right);
	        }
	        if(list.size() > 0)result.add(new ArrayList<Node>(list));
	        list.clear();
	    }
	    return result;
	}
}
