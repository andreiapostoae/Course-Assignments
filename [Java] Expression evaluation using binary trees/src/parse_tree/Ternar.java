package parse_tree;

/**
 * Clasa corespunzatoare elementelor operatiei ternare
 *
 */
public class Ternar extends Node {
	
	/**
	 * @param value valoare efectiva
	 * @param left fiu stanga
	 * @param right fiu dreapta
	 */
	public Ternar(String value, Node left, Node right) {
		super(value, left, right, 'N');
	}
}
