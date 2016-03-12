package parse_tree;

/**
 * Clasa parinte a tuturor elementelor: expresii, factori, termeni, ternari
 *
 */
public class Node {
	Node right = null;
	Node left = null;
	String value;
	char type;
	
	/**
	 * @param value valoare efectiva <p>
	 * Constructor primitiv.
	 */
	public Node(String value) {
		this.value = value;
		this.right = null;
		this.left = null;
	}
	
	/**
	 * @param value valoare efectiva
	 * @param left fiu stanga
	 * @param right fiu dreapta
	 * @param type tipul nodului <p>
	 * Constructor complet.
	 */
	public Node(String value, Node left, Node right, char type) {
		this.value = value;
		this.right = right;
		this.left = left;
		this.type = type;
	}
	
	/**
	 * @return valoarea efectiva
	 */
	public String getValue() {
		return value;
	}
	
	/**
	 * @return true daca nodul este frunza
	 */
	public boolean isLeaf() {
		if(right == null && left == null)
			return true;
		return false;
	}
	
	/**
	 * @return fiul din stanga
	 */
	public Node getLeftChild() {
        return left;
    }
	
	/**
	 * @return fiul din dreapta
	 */
	public Node getRightChild() {
        return right;
    }
	
	/**
	 * @return tipul nodului
	 */
	public char getType() {
		return type;
	}

}
