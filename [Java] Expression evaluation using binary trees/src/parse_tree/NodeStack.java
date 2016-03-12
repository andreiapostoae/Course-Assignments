package parse_tree;

import java.util.ArrayList;

/**
 * Clasa ce desemneaza o stiva de noduri.
 *
 */
public class NodeStack {
    private ArrayList<Node> a;

    /**
     *  Constructor default de 10 elemente.
     */
    public NodeStack() {
       a = new ArrayList<Node>(10);      
    }

    /**
     * @param size <p>
     * Constructor cu parametru.
     */
    public NodeStack(int size) {
       a = new ArrayList<Node>(size);    
    }
    
    /**
     * @return true daca stiva e goala
     */
    public boolean isEmpty() {
         return a.isEmpty();
    }

    /**
     * @return nodul scos din stiva <p>
     * Metoda elimina nodul de asemenea. 
     */
    public Node pop() {
        Node last;
        last = a.remove((a.size()- 1));
        return(last);     
    }
    
    /**
     * @param x <p>
     * Adauga nodul x in stiva.
     */
    public void push(Node x) {
        a.add(x);
    }

    /**
     * @return nodul de scos din stiva <p>
     * Spre deosebire de pop, metoda nu il elimina.
     */
    public Node peek() {
        return(a.get(a.size() -1));
    }
    
}