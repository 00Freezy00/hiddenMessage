all:
	gcc hide.c supportLib.c -lm -o hide
	gcc unhide.c supportLib.c -lm -o unhide
debug:
	gcc -g hide.c supportLib.c -lm -o hide
	gcc -g unhide.c supportLib.c -lm -o unhide
clean:
	rm hide
	rm unhide
	rm kittens-*.ppm
populateFileOne:
	cp sample.ppm kittens-000.ppm
	cp sample.ppm kittens-001.ppm
	cp sample.ppm kittens-002.ppm
	cp sample.ppm kittens-003.ppm
	cp sample.ppm kittens-004.ppm
	cp sample.ppm kittens-005.ppm
