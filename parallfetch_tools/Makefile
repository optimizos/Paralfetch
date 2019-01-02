all: pfsetmode pfviewer

pfsetmode: pfsetmode.c flashfetch_fetch.h
	gcc pfsetmode.c -o $@ -D_FILE_OFFSET_BITS=64

pfviewer: pfviewer.c flashfetch_fetch.h
	gcc pfviewer.c -o $@ -D_FILE_OFFSET_BITS=64

clean:
	rm -f pfsetmode pfviewer
