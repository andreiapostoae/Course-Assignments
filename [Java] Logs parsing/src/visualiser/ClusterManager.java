package visualiser;

import java.util.ArrayList;
import java.util.List;

final class ClusterManager {
	private static ClusterManager instance = null;
	private static int window = 10;
	
	protected ClusterManager() {
	}
	
	/* singleton */
	public static ClusterManager getInstance() {
		if(instance == null) {
			instance = new ClusterManager();
	    }
	    return instance;
	}
	
	public ClusterManager(int window) {
		ClusterManager.setWindow(window);
	}
	
	public static List<Cluster> cluster(List<UserEvent> userEvents) {
		/* implementarea se face cu ajutorul unei liste de intregi ce reprezinta indicii ce urmeaza a fi
		 * stersi la fiecare pas, pana lista initiala devine goala; in rest, se respecta conditiile din 
		 * enunt */
		List<UserEvent> auxList = new ArrayList<>(userEvents);
		List<Cluster> clusters = new ArrayList<Cluster>();
		while(!auxList.isEmpty()) {
			List<Integer> toBeDeleted = new ArrayList<Integer>();
			EditorArea area = auxList.get(0).getArea();
			String tipElement = area.getPathInEditor().get(0).getType();
			int tstart = auxList.get(0).getTimestamp(); /* timestamp start */
			int tstop = 0; /* timestamp stop */
			boolean ok = false;
			for(int i = 1; i < auxList.size(); i++) {
				if(auxList.get(i).getArea().getPathInEditor().get(0).getType().compareTo(tipElement) == 0 && 
						auxList.get(i).getTimestamp() <= tstart + getWindow()) {
					tstop = auxList.get(i).getTimestamp();
					ok = true;
					toBeDeleted.add(i);
				}
			}
			
			List<UserEvent> newList = new ArrayList<UserEvent>();
			if(ok == true) 
				newList.add(auxList.get(0));
			for(Integer x : toBeDeleted) {
				newList.add(auxList.get(x));
			}
			
			for(int i = toBeDeleted.size() - 1; i >= 0; i--) {
				int poz = toBeDeleted.get(i);
				auxList.remove(poz);
			}
			auxList.remove(0);
			
			int tfinal = tstop + window - 1; /* timestamp final */
			if(!newList.isEmpty()) {
				Cluster cluster = new Cluster(newList,tstart,tfinal);
				clusters.add(cluster);
			}
		}
		return clusters;
	}
	
	public static int getWindow() {
		return window;
	}

	public static void setWindow(int window) {
		ClusterManager.window = window;
	}

	public static class Cluster {
		@Override
		public int hashCode() {
			final int prime = 31;
			int result = 1;
			result = prime * result + endTimestamp;
			result = prime * result + startTimestamp;
			result = prime * result
					+ ((userEvents == null) ? 0 : userEvents.hashCode());
			return result;
		}

		@Override
		public boolean equals(Object obj) {
			if (this == obj)
				return true;
			if (obj == null)
				return false;
			if (getClass() != obj.getClass())
				return false;
			Cluster other = (Cluster) obj;
			if (endTimestamp != other.endTimestamp)
				return false;
			if (startTimestamp != other.startTimestamp)
				return false;
			if (userEvents == null) {
				if (other.userEvents != null)
					return false;
			} else if (!userEvents.equals(other.userEvents))
				return false;
			return true;
		}

		private List<UserEvent> userEvents;
		private int startTimestamp;
		private int endTimestamp;
		public Cluster(List<UserEvent> userEvents, int start, int end) {
			startTimestamp = start;
			endTimestamp = end;
			this.userEvents = userEvents;
		}
		
		public void addUserEvent(UserEvent e) {
			userEvents.add(e);
		}
		
		public void setStart(int start) {
			startTimestamp = start;
		}
		
		public int getStart() {
			return startTimestamp;
		}
		
		public void setEnd(int end) {
			endTimestamp = end;
		}
		
		public int getEnd() {
			return endTimestamp;
		}
		
		public void setEvents(List<UserEvent> userEvents) {
			this.userEvents = userEvents;
		}
		
		public List<UserEvent> getUserEvents() {
			return userEvents;
		}
		
	}
}
