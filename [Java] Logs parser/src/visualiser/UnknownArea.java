package visualiser;

import java.util.List;

final class UnknownArea extends EditorArea {
	public UnknownArea(List<EditorElement> pathInEditor) {
		super(pathInEditor);
	}

	@Override
	public Color getVisualisationColor() {
		return Color.GREY;
	}
	
}
