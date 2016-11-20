package graphics;

import java.awt.Color;
import java.awt.Graphics;
import java.awt.Graphics2D;
import java.awt.Shape;
import java.awt.geom.Ellipse2D;
import java.util.ArrayList;
import java.util.List;
import java.util.Map;

import javax.swing.JFrame;

public final class Renderer extends JFrame {
	private static final long serialVersionUID = 1L;
	private List<Point> pointList = new ArrayList<Point>();
	private double circleSize = 0;
	private String title = "undefined";
	private int EXIT_ON_CLOSE = 0;
	private Map<visualiser.Color, Color> colorMap;
	
	/* setteri si getteri */
	public List<Point> getPointList() {
		return pointList;
	}

	public void setPointList(List<Point> pointList) {
		this.pointList = pointList;
	}

	public double getCircleSize() {
		return circleSize;
	}

	public void setCircleSize(double circleSize) {
		this.circleSize = circleSize;
	}

	public String getTitle() {
		return title;
	}

	public void setTitle(String title) {
		this.title = title;
	}

	public int getEXIT_ON_CLOSE() {
		return EXIT_ON_CLOSE;
	}
	

	public void setEXIT_ON_CLOSE(int eXIT_ON_CLOSE) {
		EXIT_ON_CLOSE = eXIT_ON_CLOSE;
	}
	
	public void setMap(Map<visualiser.Color,Color> map) {
		colorMap = map;
	}
	
	/* implementare Fluent Builder Pattern */
	private Renderer() { }
	
	public static Builder builder() {
		return new Renderer.Builder();
	}

	
	public static class Builder {
		private Renderer renderer = new Renderer();
		public Builder() { }
		
		public Builder withDimensions(int x, int y) {
			renderer.setSize(x,y);
			return this;
		}
		public Builder withTitle(String x) { 
			renderer.setTitle(x);
			return this;
		}
		
		public Builder withColorMap(Map<visualiser.Color,Color> map) {
			renderer.setMap(map);
			return this;
		}
		
		public Builder withDefaultCloseOperation(int x) {
			renderer.setEXIT_ON_CLOSE(x);
			return this;
		}
		 
		public Builder withCircleSize(double x) {
			renderer.setCircleSize(x); 
			return this;
		}
		
		public Builder withPointList(List<Point> x) {
			renderer.setPointList(x);
			return this;
		}
		
		public Renderer build() {
			return renderer;
		}
	}
	
	/* functiile propriu-zise desenare */
	public void draw() {
		setVisible(true);
	}
	
	public void addPoint(int x, int y, visualiser.Color color) {
		Point point = new Point.Builder()
			.withColor(color)
			.withX(x)
			.withY(y)
			.build();
		pointList.add(point);
	}
	
	@Override
	public void paint(Graphics g) {
		Shape circle;
		Graphics2D ga = (Graphics2D)g;
		for (Point p : pointList) {
			circle = new Ellipse2D.Double(p.getX(), p.getY(), circleSize, circleSize);
			ga.draw(circle);
		  	ga.setPaint(getColor(p));	
		  	ga.fill(circle);
		}
	}
	
	private Color getColor(Point p) {
		visualiser.Color color = p.getColor();
		return colorMap.get(color);
	}
}
