package visualiser;

import java.util.List;

public final class Canvas extends EditorArea {

	public Canvas(List<EditorElement> pathInEditor) {
		super(pathInEditor);
	}

	@Override
	public Color getVisualisationColor() {
		return Color.RED;
	}
	
}
