package graphics;
import visualiser.Color;

public final class Point {
	private int x = 0;
	private int y = 0;
	private Color color = Color.GREY;
	
	private Point() { }
	
	public static Builder builder() {
		return new Point.Builder();
	}
	
	public Color getColor() {
		return color;
	}

	public void setColor(Color color) {
		this.color = color;
	}

	public double getY() {
		return y;
	}

	public void setY(int y) {
		this.y = y;
	}

	public double getX() {
		return x;
	}

	public void setX(int x) {
		this.x = x;
	}

	public static class Builder {
		private Point point = new Point();
		public Builder() { }
		
		public Builder withX(int x) { 
			point.setX(x);
			return this;
		}
		
		public Builder withY(int y) {
			point.setY(y);
			return this;
		}
		 
		public Builder withColor(Color color) {
			point.setColor(color); 
			return this;
		}
		
		public Point build() {
			return point;
		}
	}

		
	
	
	

}
