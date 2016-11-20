package visualiser;

public final class UserEvent {
	@Override
	public int hashCode() {
		final int prime = 31;
		int result = 1;
		result = prime * result + ((area == null) ? 0 : area.hashCode());
		result = prime * result + timestamp;
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
		UserEvent other = (UserEvent) obj;
		if (area == null) {
			if (other.area != null)
				return false;
		} else if (!area.equals(other.area))
			return false;
		if (timestamp != other.timestamp)
			return false;
		return true;
	}

	private EditorArea area;
	private int timestamp;
	
	public UserEvent() {
	}
	
	public UserEvent(EditorArea area, int timestamp) {
		this.area = area;
		this.timestamp = timestamp;
	}
	
	public EditorArea getArea() {
		return area;
	}
	
	public int getTimestamp() {
		return timestamp;
	}
	
	public void setPageArea(EditorArea area) {
		this.area = area;
	}
	
	public void setTimestamp(int timestamp) {
		this.timestamp = timestamp;
	}
	
	public String toString() {
		return("["+area.toString()+","+timestamp+"]");
	}
}
