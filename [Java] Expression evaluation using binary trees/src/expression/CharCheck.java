package expression;

/**
 * Clasa se ocupa cu prelucrarea caracterelor. <p>
 * Metodele create sunt statice, cu scopul de a fi folosite direct asupra caracterelor.
 */
public class CharCheck {
	/**
	 * @param c operator al carui prioritate trebuie obtinuta
	 * @return numarul intreg corespunzator prioritatii <p>
	 * Se respecta ordinea relativa din enunt, la care se mai adauga parantezele.
	 */
	public static int priority(char c) {
		if(c == '=')
			return 1;
		if(c == ':')
			return 3;
		if(c == '?')
			return 4;
		if(c == '>')
			return 5;
		if(c == '+' || c == '-')
			return 6;
		if(c == '(' || c == ')')
			return 2;
		return 7;
	}
	

	/**
	 * @param c
	 * @return true daca parametrul este un operator <p>
	 * Metoda este folosita la prima parte a problemei, cu scopul stabilirii operatiilor unde sunt puse parantezele.
	 */
	public static boolean isOperator(char c) {
		if(!Character.isLetter(c) && !Character.isDigit(c) && c != '(' && c != ')' && c!= '?')
			return true;
		return false;
	}
	
	/**
	 * @param c
	 * @return true daca parametrul este alfanumeric <p>
	 * Metoda este folosita in transformarea din infix in postfix.
	 */
	public static boolean isAlphanumerical(char c) {
		if(Character.isLetter(c) || Character.isDigit(c))
			return true;
		return false;
		
	}
	
}
