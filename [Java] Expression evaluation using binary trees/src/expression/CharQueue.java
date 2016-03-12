package expression;

import java.util.ArrayList;

/**
 * Clasa cozii de caractere, folosita in main, in care se introduc operatori.
 */
public class CharQueue
{
    private ArrayList<Character> a;

    /**
     * Constructor default (10 elemente).
     */
    public CharQueue() {
       a = new ArrayList<Character>(10);       
    }

    /**
     * @param size dimensiunea cozii<p>
     * Constructor cu parametru.
     */
    public CharQueue(int size) {
       a = new ArrayList<Character>(size);       
    }
    
    /**
     * @return true daca coada este goala, false altfel
     */
    public boolean isEmpty() {
         return a.isEmpty();
    }

    /**
     * @return caracterul scos din coada <p>
     * Metoda sterge caracterul de asemenea.
     */
    public Character pop() {
        Character c;
        c = a.remove(0);
        return(c);      
    }

    /**
     * @param x <p>
     * Metoda adauga caracterul x in coada.
     */
    public void push(Character x) {
        a.add(x);
    }

    /**
     * @return caracterul de scos din coada. <p>
     * Spre deosebire de pop(), aceasta nu il elimina.
     */
    public Character peek() {
        return(a.get(0));
    }
    
}