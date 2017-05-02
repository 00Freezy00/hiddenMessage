all:
	gcc hide.c supportLib.c -lm -o hide
	gcc unhide.c supportLib.c -lm -o unhide
clean:
	rm hide
	rm unhide
