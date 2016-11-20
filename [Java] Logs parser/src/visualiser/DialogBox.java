package visualiser;

import java.util.List;

public final class DialogBox extends EditorArea {
	public DialogBox(List<EditorElement> pathInEditor) {
		super(pathInEditor);
	}

	@Override
	public Color getVisualisationColor() {
		return Color.BLUE;
	}
	
}
