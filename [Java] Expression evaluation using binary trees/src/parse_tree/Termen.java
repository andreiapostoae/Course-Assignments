package parse_tree;

/**
 * Clasa corespunzatoare termenilor
 *
 */
public class Termen extends Node {

	/**
	 * @param value valoare efectiva
	 * @param left fiu stanga
	 * @param right fiu dreapta
	 */
	public Termen(String value, Node left, Node right) {
		super(value, left, right, 'T');
	}

}
