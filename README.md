# Huffman Codification and Decodification

This project implements Huffman encoding and decoding algorithms in C, offering an efficient method for compressing and decompressing text files. The program calculates the frequency of each character in the input file, builds a Huffman tree based on these frequencies, and then uses this tree to encode the text. The encoded file is significantly smaller, demonstrating the efficiency of Huffman coding.

## Features
- Text file compression and decompression using Huffman coding.
- Calculation of Shannon information for input data.
- Generation of Huffman codes for character encoding.
- Command-line interface for easy use.

## Demo usage and output

![Huffman Codification Demo](https://i.giphy.com/gw3IWyGkC0rsazTi.webp)

## Building the Project

To compile the program using the provided Makefile, simply run:

```bash
make build
```

This will generate the executable `huff` based on the `huffCodificareDecodificare.c` source file.

## Using the program

To encode a file, use:
```bash
./huff &ltfilepath&gt
```

The program will create two files: compressedFile.txt for the encoded content and decodedFile.txt for the decoded content, demonstrating the lossless nature of the compression.

## Cleaning Up

To clean up the generated files and any output, use the command:

```bash
make clean
```

## Contributing
Contributions to this project are welcome. Feel free to fork the repository, make your changes, and submit a pull request.

## License
This project is licensed under the MIT License - see the LICENSE file for details.
