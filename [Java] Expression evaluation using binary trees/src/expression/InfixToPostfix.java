package expression;

/**
 * @author apo
 *
 */
public class InfixToPostfix {
	String expression;
	
	/**
	 * @param infix sirul de caractere care trebuie convertit
	 */
	public InfixToPostfix(String infix) {
		this.expression = infix;
	}
	
	/**
	 * @return vector de siruri de caractere ce contine elementele expresiei in postfix <p>
	 * Algoritmul folosit a fost acesta: http://www.dreamincode.net/forums/topic/37428-converting-and-evaluating-infix-postfix-and-prefix-expressions-in-c/
	 * gasit in unul dintre linkurile din tema. <p>
	 * Bineinteles, a trebuit sa ii aduc modificari pentru situatia problemei de fata.
	 */
	public String[] converse() {
		// 
		String postfix = "";
		CharStack stack = new CharStack(30);
		int i = 0;
		boolean flag;
		char c, aux;
		while(i < expression.length()) {
			/* alegem fiecare litera din expresia initiala */
			flag = false;
			c = expression.charAt(i);
			if(c == '(') {
				/* parantezele deschise sunt adaugate in stiva */
				stack.push('(');
			} 
			else 
				if (c == ')') {
					/* la intalnirea parantezelor inchise se scot elemente din stiva pana la 
					   intalnirea parantezei inchise si se introduc cu spatii in postfix */
					aux = stack.peek();
					while(!stack.isEmpty() && aux != '(') {
						postfix += aux + " ";
						stack.pop();
						if(!stack.isEmpty()) 
							aux = stack.peek();
					}
					aux = stack.peek();
					if(aux == '(')
						stack.pop();
				}
				else 
					if(CharCheck.isAlphanumerical(c)) {
						/* la intalnirea unui caracter alfanumeric, daca dupa el urmeaza un operator
						 * sau o paranteza, atunci il introducem in postfix impreuna cu un spatiu;
						 * daca dupa el urmeaza alfanumeric, nu mai introducem spatiul */
						char d = ' ';
						if(i < expression.length() - 1 )
							d = expression.charAt(i+1);
						if(i < expression.length() - 1 && !CharCheck.isAlphanumerical(d)) {
							postfix += c + " ";
						}
						else 
							if(i == expression.length() - 1)
								postfix += c + " ";
							else 
								postfix += c;
					} 
					else {
						/* in aceasta situatie, intalnim un operator */
						char b = expression.charAt(i-1);
						if((c=='-' || c=='+') && !CharCheck.isAlphanumerical(b) && b != '(' && b!= ')') {
							/* se citeste caracterul dinainte; daca acesta nu este alfanumeric,
							 * atunci avem de a face cu cazul + si - unar;
							 * tot aici, se vor citi in continuare caractere pana la intalnirea unui operator */
							postfix += c;
							i++;
							while(true) {
								c = expression.charAt(i);
								char d = ' ';
								if(i < expression.length() - 1)
									d = expression.charAt(i+1);
								postfix += c;
								i++;
								if(i == expression.length() || !CharCheck.isAlphanumerical(c) || !Character.isDigit(d))
									break;
							}
							postfix += " ";
							flag = true;
						}
						else
							/* cazul operatorului binar/ternar */
							if(stack.isEmpty())
								stack.push(c);
							else {
								aux = stack.peek();
								while(!stack.isEmpty() && CharCheck.priority(aux) >= CharCheck.priority(c) ) {
									postfix += aux+" ";
									stack.pop();
									if(!stack.isEmpty()) 
										aux = stack.peek();
								}
								stack.push(c);
							}
					}

					if(flag == false) 
						i++;
		}
		
		/* la sfarsit se adauga toate elementele din stiva in postfix */
		while(!stack.isEmpty()) {
			postfix += stack.pop()+" ";
		}
		
		/* parantezele sunt eliminate, spatiile consecutive sunt reduse la unul singur, iar apoi se face
		   split dupa spatiu, elementele nodurilor urmand a fi introduse intr-un vector si returnate  */
		postfix = postfix.replace("(", "");
		postfix = postfix.trim().replaceAll(" +", " ");
		
		String[] result = postfix.split(" ");
		return result;
	}
	
}


