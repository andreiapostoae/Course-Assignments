package parse_tree;

/**
 * Clasa variabilelor.
 *
 */
public class Variable {
	String nume;
	int valoare;
	
	/**
	 * @param nume numele variabilei
	 * @param valoare valoarea efectiva (int) a variabilei <p>
	 * Constructor parametrizat.
	 */
	public Variable(String nume, int valoare) {
		this.nume = nume;
		this.valoare = valoare;
	}

	/**
	 * @return valoarea efectiva a variabilei
	 */
	public int getValue() {
		return valoare;
	}
	
	/**
	 * @return numele variabilei
	 */
	public String getName() {
		return nume;
	}
	
}
