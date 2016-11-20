package visualiser;

import java.util.List;

public final class Menu extends EditorArea {
	public Menu(List<EditorElement> pathInEditor) {
		super(pathInEditor);
	}

	@Override
	public Color getVisualisationColor() {
		return Color.GREEN;
	}
	
}
