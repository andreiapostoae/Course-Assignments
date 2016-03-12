package visualiser;

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;
import java.util.Scanner;

public final class Task {
	private final List<UserEvent> userEvents;
	String log;
	
	public Task() {
		userEvents = new ArrayList<UserEvent>();
	}
	
	public Task(String s) {
		userEvents = parseLogs(s);
		log = s;
	}
	
	public String getLog() {
		return log;
	}
	
	public List<UserEvent> getUserEvents() {
		return userEvents;
	}
	
	public Double meanFrequencyPerTenSeconds() {
		/* in loc sa fac media frecventelor, am calculat numarul total de timestampuri
		 * si le-am impartit la numarul de intervale de cate 10 secunde */
		List<UserEvent> lista = parseLogs(log);
		int lastTimestamp = lista.get(lista.size() - 1).getTimestamp();
		return (double) ((lista.size() / 10.0) / (lastTimestamp / 10 + 1));
	}
	
	public Map<String, Double> computeClicksPerArea() {
		List<UserEvent> lista = parseLogs(log);
		Map<String, Double> map = new HashMap<String, Double>();
		double canvas = 0;
		double dialog = 0;
		double menu = 0;
		double unknownArea = 0;
		for(UserEvent event : lista) {
			EditorArea area = event.getArea();
			List<EditorElement> elemente = area.getPathInEditor();
			int size = elemente.size();
			switch(area.getVisualisationColor()) {
			case RED:
				canvas += size;
				break;
			case BLUE:
				dialog += size;
				break;
			case GREEN:
				menu += size;
				break;
			case GREY:
				unknownArea += size;
				break;
			default:
				break;
			}
		}
		if(canvas != 0)
			map.put("visualiser.Canvas", canvas);
		if(dialog != 0)
			map.put("visualiser.DialogBox", dialog);
		if(menu != 0)
			map.put("visualiser.Menu", menu);
		if(unknownArea != 0)
			map.put("visualiser.UnknownArea", unknownArea);
		return map;
	}

	static List<UserEvent> parseLogs(String logs) {
		/* se citesc liniile si in functie de structura lor, se stabileste daca sunt
		 * elemente ale unui user event sau nu; daca sunt elemente, se pastreaza 
		 * numele lor cu tot cu ghilimele */
		Scanner scanner = new Scanner(logs);
		List<UserEvent> lista = new ArrayList<UserEvent>();
		while (scanner.hasNextLine()) {
			String line = scanner.nextLine();
			if(line.charAt(0) == 'u') {
				int ts = 0; //timestamp
				List<EditorElement> elemente = new ArrayList<EditorElement>();
				
				do {
					line = scanner.nextLine();
					if(line.charAt(0) != '}' && line.charAt(4) == 'e') {
						String aux = line.substring(14, line.length());
						EditorElement element = new EditorElement(aux);
						elemente.add(element);
					}
					if(line.charAt(0) != '}' && line.charAt(4) == 't') {
						ts = Integer.parseInt(line.substring(16, line.length()));
					}
				} while (line.charAt(0) != '}');
				EditorArea area = determineAreaForElements(elemente);
				UserEvent event = new UserEvent(area,ts);
				lista.add(event);
			}
		}
		scanner.close();
		return lista;
	}

	static EditorArea determineAreaForElements(List<EditorElement> elements) {
		Color color = Color.GREY;
		for(EditorElement element : elements) {
			String aux = element.getType();
			if(aux.compareTo("\"card\"") == 0 ||
					aux.compareTo("\"input\"") == 0 || 
						aux.compareTo("\"page\"") == 0)
				color = Color.RED; //CANVAS
			else
				if(aux.compareTo("\"menu\"") == 0 || 
						aux.compareTo("\"menu-button\"") == 0 || 
							aux.compareTo("\"icon\"") == 0)
					color = Color.GREEN; //MENU
				else
					if(aux.compareTo("\"dialog\"") == 0)
						color = Color.BLUE; //DIALOG
		}
		EditorArea area = AreaFactory.createArea(color,elements);
		return area;
	}
}
