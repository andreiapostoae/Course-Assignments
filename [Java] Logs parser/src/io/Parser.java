package io;

import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;

public final class Parser {
	@SuppressWarnings("resource")
	public static String readFromFile(String path) throws IOException {
		BufferedReader br = new BufferedReader(new FileReader(path));
		String s= "";
		String aux;
		while ((aux = br.readLine()) != null) {
		       s = s + aux + "\n";
		    }
		return s;
	}
}
