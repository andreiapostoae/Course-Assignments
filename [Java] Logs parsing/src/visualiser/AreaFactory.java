package visualiser;

import java.util.List;

public class AreaFactory {
	
	public static EditorArea createArea(Color color, List<EditorElement> elements) {
		switch(color) {
		case RED:
			return new Canvas(elements);
		case BLUE:
			return new DialogBox(elements);
		case GREEN:
			return new Menu(elements);
		case GREY:
			return new UnknownArea(elements);
		default:
			break;
		}
		throw new IllegalArgumentException("Culoarea nu este recunoscuta");
	}
}
