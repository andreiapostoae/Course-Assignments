package parse_tree;

/**
 * Clasa corespunzatoare factorilor
 *
 */
public class Factor extends Node {

	/**
	 * @param value valoare efectiva
	 * @param left fiu stanga
	 * @param right fiu dreapta
	 */
	public Factor(String value, Node left, Node right) {
		super(value,left,right,'F');
	}
	
}
