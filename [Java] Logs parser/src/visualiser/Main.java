package visualiser;

import graphics.Point;
import graphics.Renderer;
import io.Parser;

import java.awt.Color;
import java.io.File;
import java.io.IOException;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import javax.swing.JFrame;

import visualiser.ClusterManager.Cluster;

final class Main {
	private static Renderer renderer;
	private final static int Xscale = 10;
	private final static int Yscale = 50;
	
	public static void main(String[] args) {
		
		List<Task> tasks = readTasks("logs/alfred");
		
		/* harta culorilor */
		Map<visualiser.Color, Color> colorMap = new HashMap<visualiser.Color, Color>();
		colorMap.put(visualiser.Color.RED, Color.RED);
		colorMap.put(visualiser.Color.GREEN, Color.GREEN);
		colorMap.put(visualiser.Color.BLUE, Color.BLUE);
		colorMap.put(visualiser.Color.GREY, Color.GRAY);
		colorMap.put(visualiser.Color.ORANGE, Color.ORANGE);
		
		/* renderer pentru toate punctele folosind Fluent Builder Pattern */
		List<Point> allPointList = new ArrayList<Point>();
		renderer = new Renderer.Builder()
				.withDefaultCloseOperation(JFrame.EXIT_ON_CLOSE)
				.withTitle("All data")
				.withCircleSize(10)
				.withPointList(allPointList)
				.withDimensions(1200,400)
				.withColorMap(colorMap)
				.build();
		
		drawAll(tasks);
		renderer.draw();
		
		/* renderer pentru clustere folosind Fluent Builder Pattern */
		allPointList = new ArrayList<Point>();
		renderer = new Renderer.Builder()
		.withDefaultCloseOperation(JFrame.EXIT_ON_CLOSE)
		.withTitle("Clusters")
		.withCircleSize(10)
		.withPointList(allPointList)
		.withDimensions(1200,400)
		.withColorMap(colorMap)
		.build();
		
		drawClusters(tasks);
		renderer.draw();
		
	}
	
	private static List<Task> readTasks(String path) {
		File[] filesList = new File(path).listFiles();
		Arrays.sort(filesList); /* a fost necesara sortarea, ceea ce consider putin straniu */
		List<Task> tasks = new ArrayList<>();
		for(int i = 0 ; i < filesList.length; i++) {
			File f = filesList[i];
			try {
				String s = Parser.readFromFile(path + "/" + f.getName());	
				Task task = new Task(s);
				tasks.add(task);
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
		return tasks;
	}
	
	private static void drawAll(List<Task> tasks) {
		/* afiseaza toate punctele cu culorile corespunzatoare */
		for(int i = 0; i < tasks.size(); i++) {
			List<UserEvent> userEventList = tasks.get(i).getUserEvents();
			for(int j = 0; j < userEventList.size(); j++) {
				int x = getXValueFor(userEventList.get(j));
				int y = getYValueFor(userEventList.get(j));
				visualiser.Color color = userEventList.get(j).getArea().getVisualisationColor();
				renderer.addPoint(x*Xscale, y*Yscale, color);
			}
		}
	}
	
	private static void drawClusters(List<Task> tasks) {
		/* afiseaza mai intai toate punctele cu culoarea ORANGE, apoi le acopera pe cele care fac
		 * parte din clustere cu culorile corecte */
		for(int i = 0; i < tasks.size(); i++) {
			List<UserEvent> userEventList = tasks.get(i).getUserEvents();
			for(int j = 0; j < userEventList.size(); j++) {
				int x = getXValueFor(userEventList.get(j));
				int y = getYValueFor(userEventList.get(j));
				renderer.addPoint(x*Xscale, y*Yscale, visualiser.Color.ORANGE);
			}
		}
		
		for(int i = 0; i < tasks.size(); i++) {
			List<UserEvent> userEventList = tasks.get(i).getUserEvents();
			List<Cluster> clusters = ClusterManager.cluster(userEventList);
			for(int j = 0; j < clusters.size(); j++) {
				ClusterManager.Cluster cluster = clusters.get(j);
				List<UserEvent> finalList = cluster.getUserEvents();
				for(int k = 0; k < finalList.size(); k++) {
					int x = getXValueFor(finalList.get(k));
					int y = getYValueFor(finalList.get(k));
					visualiser.Color color = finalList.get(k).getArea().getVisualisationColor();
					renderer.addPoint(x*Xscale, y*Yscale, color);
				}
			}
		}
	}
	
	private static int getXValueFor(UserEvent e) {
		return e.getTimestamp();
	}
	
	private static int getYValueFor(UserEvent e) {
		String s = e.getArea().getPathInEditor().get(0).getType();
		switch(s) {
		case "\"input\"":
			return 1;
		case "\"card\"":
			return 2;
		case "\"page\"":
			return 3;
		case "\"menu\"":
			return 4;
		case "\"menu-button\"":
			return 5;
		case "\"icon\"":
			return 6;
		case "\"dialog\"":
			return 7;
		}
		return 8;
	}	
	
}
