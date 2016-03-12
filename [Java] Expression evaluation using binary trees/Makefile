.PHONY: build clean run

run: build
	java -Xmx512m testare.Test ${ARGS}
build: 
	javac -d . src/expression/*.java
	javac -d . src/parse_tree/*.java
	javac -d . src/testare/*.java
clean:
	rm -rf parse_tree/*.class
	rm -rf testare/*.class
	rm -rf expression/*.class
