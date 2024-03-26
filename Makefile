build:
	gcc -o huff huffCodificareDecodificare.c -lm

clean:
	-rm huff 2> /dev/null
	-rm compressedFile.txt 2> /dev/null
	-rm decodedFile.txt 2> /dev/null
