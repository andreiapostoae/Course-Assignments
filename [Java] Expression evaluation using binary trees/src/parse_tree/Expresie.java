package parse_tree;

/**
 * Clasa corespunzatoare expresiilor
 *
 */
public class Expresie extends Node {
	
	/**
	 * @param value valoare efectiva
	 * @param left fiu stanga
	 * @param right fiu dreapta
	 */
	public Expresie(String value, Node left, Node right) {
		super(value,left,right,'E');
	}
	
}
