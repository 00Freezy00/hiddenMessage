all:
	gcc hide.c -lm -o hide
	gcc unhide.c -lm -o unhide
clean:
	rm hide
	rm unhide
