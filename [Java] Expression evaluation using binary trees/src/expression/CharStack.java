package expression;

import java.util.ArrayList;

/**
 * Clasa ce desemneaza o stiva de caractere.
 */
public class CharStack
{
    private ArrayList<Character> a;

    /**
     *  Constructor default de 10 elemente.
     */
    public CharStack() {
       a = new ArrayList<Character>(10);       
    }

    /**
     * @param size <p>
     * Constructor cu parametru.
     */
    public CharStack(int size) {
       a = new ArrayList<Character>(size);      
    }
    
    /**
     * @return true daca stiva e goala
     */
    public boolean isEmpty() {
         return a.isEmpty();
    }

    /**
     * @return caracterul scos din stiva <p>
     * Metoda elimina caracterul de asemenea.
     */
    public Character pop() {
        Character last;
        last = a.remove((a.size()- 1));
        return(last);      
    }

    /**
     * @param x <p>
     * Adauga caracterul x in stiva.
     */
    public void push(Character x) {
        a.add(x);
    }

    /**
     * @return caracterul de scos din stiva <p>
     * Spre deosebire de pop, metoda nu il elimina.
     */
    public Character peek() {
        return(a.get(a.size() -1));
    }
    
}