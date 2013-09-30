all: 
	gcc sushi.c -o sushi -pthread 

clean:
	rm -rf *o sushi