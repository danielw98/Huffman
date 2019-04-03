#include <stdio.h> 
#include <math.h>
#include <stdlib.h>

/*
**********************************************************************
****************************** Partea I ******************************
******FUNCTII PENTRU DETERMINAREA INFORMATIEI SHANNON DIN FISIER******
**********************************************************************
*/

//declarate global pentru a nu trata initializarea cu 0.
//nu cunoastem la inceput numarul de caractere distincte din text
int nr_aparitii[255];
double probabilitati[255];


//functie care determina dimensiunea fisierului
int getFileLength(FILE *f)
{

	int length;
	if(f == NULL)
	{	
		printf("Fisierul nu exista!\n");
		return 0;
	}
	fseek(f, 0, SEEK_END);
	length = ftell(f);
	rewind(f);
	return length;
}


//functie care incarca in buffer fisierul
char* loadBuffer(FILE *f, int length)
{

	if (length == 0) 
	{

		printf("Nu aveti continut in fisier!\n");
		return NULL;	
	}

	char *buffer = (char*) malloc (length * sizeof(char));
	fread(buffer, length, 1, f);
	fclose(f);

	return buffer;
}


/*
functie pentru determinarea frecventei aparitei caracterelor
si determinarea probabilitatilor pe baza lungimii textului
*/
void computeProbabilities(char* buffer, int length)
{

	int i;

	for(i = 0; i < length; i++)
	{
	
		nr_aparitii[*(buffer+i)]++;
	}
	
 	for(i = 0; i < 255; i++)
	{
		probabilitati[i] = (float) nr_aparitii[i] / length;
	}

}


//calculeaza informatia Shannon
long double computeShannonInformation()
{

	long double I = 0;
	for (int i = 0; i < 255; i++)
	{	

		if (probabilitati[i] != 0)
		{	

			I = I + probabilitati[i] * (log(probabilitati[i]) / log(2));
		}
	}

	return -I;
}


/*
**********************************************************************
****************************** Partea II *****************************
****FUNCTII PENTRU DETERMINAREA ARBORELUI HUFFMAN PE BAZA TEXTULUI****
**********************************************************************
*/


// structura unui nod din arborele Huffman
typedef struct MinHeapNode
{ 

	// litera
	char data; 

	// frecventa acesteia in text
	int freq; 

	// fiii nodului
	struct MinHeapNode *st, *dr;
}MinHeapNode; 


// un MinHeap - colectie de noduri din arborele Huffman
typedef struct MinHeap
{ 

	// dimensiunea heap-ului la un moment dat
	int size;

	// cate elemente pot sa incapa in heap
	int capacity; 

	// vector de pointeri la noduri 
	MinHeapNode** array; 
}MinHeap; 


// Alocarea unui nou nod (litera, frecventa); 
MinHeapNode* newNode(char data, int freq) 
{ 

	if(freq == 0)
	{

		return NULL;
	}

	MinHeapNode* temp = (MinHeapNode*) malloc (sizeof(MinHeapNode)); 
	temp->st = temp->dr = NULL; 
	temp->data = data; 
	temp->freq = freq; 

	return temp; 
} 


// Crearea unui min heap de o capacitate data
MinHeap* createMinHeap(int capacity) 
{ 

	MinHeap* minHeap = (MinHeap*) malloc (sizeof(MinHeap)); 

	// dimensiunea curenta este 0
	minHeap->size = 0; 
	minHeap->capacity = capacity; 

	// aloc spatiu pentru numarul de noduri dat
	minHeap->array = (MinHeapNode**) malloc(minHeap->capacity * sizeof(MinHeapNode*)); 

	return minHeap; 
} 


// functie pentru inversarea a doua noduri
void swapMinHeapNode(MinHeapNode** a, MinHeapNode** b)
{ 

	MinHeapNode* t = *a; 
	*a = *b; 
	*b = t; 
} 


//Functie pentru asigurarea prorpietatii de MinHeap
void Heapify(MinHeap* minHeap, int i) 
{ 

	int minim = i; 
	int st = 2 * i + 1; 
	int dr = 2 * i + 2; 
	
	if ((st < minHeap -> size) && (minHeap -> array[st] -> freq) < (minHeap -> array[minim] -> freq))
	{ 

		minim = st; 
	}

	if ((dr < minHeap -> size) && (minHeap -> array[dr] -> freq) < (minHeap -> array[minim] -> freq))
	{

		minim = dr; 
	}

	if (minim != i) 
	{ 

		swapMinHeapNode(&minHeap -> array[minim], &minHeap -> array[i]); 
		Heapify(minHeap, minim); 
	} 
} 


// functie care verifica daca dimensiunea Heap-ului este 1
int isSizeOne(MinHeap* minHeap) 
{ 

	return (minHeap -> size == 1); 
} 


// functie care extrage nodul minim din heap
MinHeapNode* extractMin(MinHeap* minHeap) 
{ 

	//minimul este elementul din cap, pastrat in temp
	MinHeapNode* temp = minHeap -> array[0];

	//il inlocuim cu ultimul element
	minHeap -> array[0] = minHeap -> array[minHeap -> size - 1];

	//scadem dimensiunea heap-ului
	--minHeap -> size;

	//refacem proprietatea de heap
	Heapify(minHeap, 0); 

	return temp; 
} 


//functie pentru inserarea unui nou nod in heap
void insertMinHeap(MinHeap* minHeap, MinHeapNode* minHeapNode) 
{ 

	++minHeap -> size; 
	int i = minHeap -> size - 1; 
	
	// caut pozitia nodului in Heap
	while (i && minHeapNode->freq < minHeap->array[(i - 1) / 2]->freq) 
	{ 
		
		minHeap -> array[i] = minHeap -> array[(i - 1) / 2]; 
		i = (i - 1) / 2; 
	} 

	minHeap -> array[i] = minHeapNode; 
} 


// functie pentru construirea minHeap-ului 
void buildHeap(MinHeap* minHeap) 
{ 

	int n = minHeap->size - 1; 
	int i; 

	for (i = (n - 1) / 2; i >= 0; i--) 
	{

		Heapify(minHeap, i);
	}
} 



// functie care verifica daca nodul este frunza 
_Bool isLeaf(MinHeapNode* root) 
{ 

	return !(root->st) && !(root->dr); 
} 




// functie care creeaza si construieste MinHeap-ul pe baza datelor obtinute din fisier
MinHeap* createAndBuildHeap(char data[], int freq[], int size) 
{ 
	
	// creez un MinHeap de dimensiunea dorita
	MinHeap* minHeap = createMinHeap(size); 

	// il initializez cu datele cunoscute - caracterele si frecventa aparitiei acestora
	for (int i = 0; i < size; i++)
	{
	
		minHeap -> array[i] = newNode(data[i], freq[i]); 
	}

	// stabilesc dimensiunea
	minHeap -> size = size;
	
	// construiesc Heap-ul
	buildHeap(minHeap); 

	return minHeap; 
} 


// functie care construieste arborele Huffman
MinHeapNode* buildHuffmanTree(char data[], int freq[], int size) 
{ 
	MinHeapNode *st, *dr, *root; 

	// initial, creez un minHeap cu capacitatea egala cu numarul de noduri 
	MinHeap* minHeap = createAndBuildHeap(data, freq, size); 

	// Iterate while size of heap doesn't become 1 
	while (!isSizeOne(minHeap)) 
	{ 

		// Extrag cele doua elemente cu frecventa minima
		st = extractMin(minHeap); 
		dr = extractMin(minHeap); 

		/* 
		- Creez un nod nou cu frecventa egala cu suma frecventelor
		celor doua  noduri.
		- Nodurile extrase devin fiu stang si fiu drept pentru nodul creat.
		- Adaug nodul creat in minHeap.
		- '$' valoare simbolica folosita pentru nodurile interne
		*/

		root = newNode('$', st -> freq + dr -> freq); 

		root -> st = st; 
		root -> dr = dr; 

		insertMinHeap(minHeap, root); 
	} 

	// Nodul ramas este radacina. Arborele este complet.
	return extractMin(minHeap); 
} 




// afisarea unui vector de dimensiune n
void print(int arr[], int n) 
{ 

	int i; 
	for (i = 0; i < n; i++)
	{

		printf("%d", arr[i]);
	}
	printf("\n");
} 



/* 
********************************************************************************************************
***************** FUNCTII PENTRU CREAREA SI INITIALIZAREA VARIABILELOR SI A VECTORILOR *****************
******************************************************************************************************** 
*/
// numarul de caractere distincte din text
int returnCount()
{

	int i;
	int count = 0;
	// parcurgere pt determinarea nr de caractere nenule
	for (i = 0; i < 255; i++)
	{

		if(nr_aparitii[i] != 0)
			{

				count++;
			}	
	}

	return count;
}


// vectorul de caractere
char* createData(int count)
{

	int i, k = 0;
	char* data = (char*) malloc (count * sizeof(char));
	
	for (i = 0; i < 255; i ++)
	{
	
		if( nr_aparitii[i] != 0)
		{	

			*(data + k) = (char) i;
			k++;
		}
	}

	return data;
}


// frecventa caracterelor
int* createFrecv(int count)
{

	int i, k = 0;
	int* frecv = (int*) malloc (count * sizeof(int));
	for (i = 0; i < 255; i++)
	{

		if ( nr_aparitii[i] != 0)
		{

			*(frecv + k) = nr_aparitii[i];
			k++;
		}
	}

	return frecv;
}




/*
**********************************************************************************
*********************** FUNCTII PENTRU MEMORAREA CODURILOR ***********************
**********************************************************************************
*/


// functie auxiliara - determina pozitia unui caracter in vector
int getPos(MinHeapNode* root, char* data)
{

	int pos = 0;
	// parcurg vectorul
	while(root->data != *(data+pos))
	{

		pos++;
	}

	//pana cand determin pozitia la care se afla elementul
	/*if(root->data == *(data+pos))
	{
		printf("Pozitia este: %d\n", pos);
	}*/

	return pos;
}


// functie auxiliara - creaza un vector de vectori elemente de dimensiune necunoscuta momentan
int** createCodesArray (int count)
{
	
	int** codes = (int**) malloc (count * sizeof(int*));

	return codes;
}


// functie auxiliara - afiseaza continutul buffer-ului
void printBuffer (char* buffer, int length)
{
	
	printf("\nContinutul fisierului initial este:\n\n");
	int i;
	for(i = 0; i < length; i++)
	{
	
		printf("%c", *(buffer+i));
	}
	printf("\n");
}

// functie auxiliara - memoreaza codul unui caracter in codes[pos]
// si lungimea acestuia in bitLength[pos]
void createCodesElement(int pos, int arr[], int top, int** codes, int* bitLength)
{
	
	int i;
	int* codesElement = (int*) malloc (top * sizeof(int));

	// copiez caracterele in codesElement
	for (i = 0; i < top; i ++)
	{

		*(codesElement + i) = arr[i];
	}

	// memorez codul caracterului la pozitia corespunzatoare in vector
	*(codes + pos) = codesElement;
	*(bitLength + pos) = top;
}


// afisez codul binar asociat unui element
void printElement(int *element, int size)
{

	int i;
	for (i = 0; i < size; i ++)
	{

		printf("%d", *(element + i));
	}

	printf("\n\n");
}
// functie care memoreaza codurile asociate fiecarui caracter intr-un vector de string-uri
void storeCodes (MinHeapNode* root, int arr[], int top, int** codes, int* bitLength)
{

	int count = returnCount();
	char* data = createData(count);

	if (root -> st) 
	{ 

		arr[top] = 0; 
		storeCodes(root -> st, arr, top + 1, codes, bitLength); 
	} 

	if (root -> dr) 
	{ 

		arr[top] = 1; 
		storeCodes(root -> dr, arr, top + 1, codes, bitLength); 
	} 

	/*
	- Daca nodul curent este frunza, atunci contine un caracter.
	- Memorez caracterul asociat
	*/
	if (isLeaf(root)) 
	{ 

		// determin pozitia la care se afla caracterul in tablou
		int pos = getPos(root, data);
		
		// memorez bitii in vectorul de vectori de biti la pozitia corespunzatoare
		createCodesElement(pos, arr, top, codes, bitLength);

		//verific daca s-a memorat corect
		/*printf("Caracterul %c: ", root -> data);
		printElement(*(codes + pos), top);*/
	} 
}





//structura care retine datele despre codificare
typedef struct HuffmanMap
{

	char* data;
	int* freq;
	int* bitLength;
	int** codes;
	int count;
} HuffmanMap;


//constructor pentru structura
HuffmanMap* createHuffmanMap(char* data, int* freq, int* bitLength, int** codes, int count)
{

	HuffmanMap* myMap;
	myMap -> data = data;
	myMap -> freq = freq;
	myMap -> bitLength = bitLength;
	myMap -> codes = codes;
	myMap -> count = count;

	return myMap;
}


//functie care returneaza vectorul de coduri
int** getCodes (HuffmanMap* myMap)
{

	return myMap->codes;
}

//functie auxiliara pentru afisarea codului unul caracter
void printCharacterCode(int* character, int length)
{
	
	int i;
	for(i = 0; i < length; i++)
	{

		printf("%d", *(character + i));
	}
	printf("\t");
}


//functie care verifica daca a fost memorat corect tabelul
void printMap(HuffmanMap* myMap)
{
	
	int i;
	
	printf("\n\nAfisarea elementelor din myMap: \n");
	printf("Nr\tCaracterul\tFrecventa\tCodificarea\t#bits\n");
	
	
	for (i = 0; i < myMap->count; i ++)
	{

		printf("%d \t %c \t\t   %d \t\t", i, myMap->data[i], myMap->freq[i]);
		printCharacterCode(myMap->codes[i], myMap->bitLength[i]);
		if(myMap->bitLength[i] > 7)
		{

			printf("%d\n",myMap->bitLength[i]);
		}
		else
		{
	
			printf("\t%d\n",myMap->bitLength[i]);
		}
	}
}



/*
*********************************************************
*********** FUNCTII PENTRU LUCRUL CU FISIERE ************
**************** PARTEA I -  CODIFICARE *****************
*********************************************************
*/



/*
lungimea in biti a fisierului rezultat este bitLength[i] * freq[i],
pentru i de la 0 la count. 
*/
int determineCompressedLength(HuffmanMap* myMap)
{

	int i;
	int length = 0;
	for(i = 0; i <  myMap->count; i++)
	{

		length += myMap->bitLength[i] * myMap->freq[i];
	}

	// returnez lungimea in Bytes
	if (length % 8 == 0)
	{
	
		return length/8;
	}
	else
	{
	
		return length/8 + 1;
	}
}


/*
- vreau sa accesez in O(1)
- toate caracterele au o pozitie unica
- am nevoie sa accesez codul caracterului pe baza caracterului,
- si nu cunosc pozitia caracterului in vector
- asociez fiecarui element o pozitie in hashTable
- pozitia este hashTable(codASCII)
- valoarea este pozitia la care se afla elementul in myMap
- asigur accesare in O(1)
*/
void createHashTable(int *hashTable, HuffmanMap* myMap)
{
	
	int i;
	int pos;
	for(i = 0; i < myMap->count; i++)
	{
		
		pos = (int) myMap -> data[i];
		*(hashTable + pos) = i;
	}

	/*printf("\n\nAfisarea hashTable-ului:\n");
	for(i = 0; i < myMap->count; i++)
	{
		pos = (int) myMap -> data[i];
		printf("caracterul %c: hashTable[%i]\n", myMap->data[*(hashTable + pos)], (int) (*(hashTable + pos)));
	}*/
}


// pozitia in hashTable a caracterului curent este valoarea de la pozitia hashTable[current]
int returnPosition (char current, int* hashTable)
{
	
	return *(hashTable + (int) current);
}


/*
- vectorul temp este un vector a carui indici contin bitii,
de la MSB la LSB ( 0 -> 7)
- lui temp[0], care este MSB ii corespunde 2^7, sau 1<<7,
cu operatii pe biti
....
- temp[6] corespunde 2^1 1<<1
- lui temp[7], care este LSB, ii corespunde 2^0, sau 1<<0
*/
int* updateBuffer(unsigned char* compressedBuffer, int* iterator, unsigned char* temp)
{
	int bit;
	unsigned char toWrite = 0;
	int i = 0;
	//printf("Urmeaza sa scriu:");
	for(i = 0; i < 8; i++)
	{
		bit = *(temp + i);
		//printf("%d", bit);
		if (bit == 1)
		{
			toWrite += bit<<(7-i);
		}
	}
	//printf("\nIn updateBuffer_%d: %d\n", *iterator, (char) toWrite);
	*(compressedBuffer + *iterator) = toWrite;
	(*iterator)++;
	return iterator;
}


// functie auxiliara care afiseaza continutul vectorului tem
void printTempInt (int* temp)
{
	int i;
	for (i = 0; i < 8; i ++)
	{

		printf("%d", *(temp+i));
	}
	printf("\n");
}
void printTemp (unsigned char* temp)
{
	int i;
	for (i = 0; i < 8; i ++)
	{

		printf("%i", *(temp+i));
	}
	printf("\n");
}


// functie care scrie biti in temp pana cand se strang 32
// apeleaza updateBuffer epentru a scrie un int in compressedBuffer
int* writeBits(int bitLength, unsigned char* compressedBuffer, int* codeBits, int* iterator, int* bitFlag, unsigned char* temp)
{

	int i;

	// memorez in temp bit cu bit pana bitFlag ajunge la 8
	// atunci il resetez si scriu un byte in compressedBuffer
	for (i = 0; i < bitLength; i++)
	{
		
		int currentBit = *(codeBits + i);
		printf("%d", currentBit);
		if( *bitFlag < 7)
		{

			*(temp + *bitFlag) = currentBit;
			(*bitFlag)++;
			
		}
		else
		{

			*(temp + *bitFlag) = currentBit;
			/*printf("\ninauntrul lui temp%d: ", *iterator);
			printTemp(temp);*/
			iterator = updateBuffer(compressedBuffer, iterator, temp);
			*bitFlag = 0;
			*(temp + *bitFlag) = currentBit;
		}
	}


	return bitFlag;
}


int* loadCompressedBuffer(HuffmanMap* myMap, int position, unsigned char* compressedBuffer, 
					int* iterator, int* bitFlag, unsigned char* temp)
{

	// memorez numarul de biti ai caracterului curent
	int bitLength = myMap->bitLength[position];

	/*
	- incarc bitii corespunzatori codificarii caracterului curent in compressedBuffer
	- semnific numarul de biti ramasi liberi
	*/
	bitFlag = writeBits(bitLength, compressedBuffer, myMap->codes[position], iterator, bitFlag, temp);
	return bitFlag;
} 

void writeBufferToFile(unsigned char* buffer, FILE* outFile, int* length)
{

	fwrite(buffer, sizeof(unsigned char), *length, outFile);
	printf("\n********** Am scris in compressedFile. **********\n");
	fclose(outFile);
}

void printBits(int* v, int n)
{
	int i;
	for (i = 0; i < n; i ++)
	{
	
		printf("%d", *(v+i));
	}
	printf("\n");
}


void writeCompressedFile(FILE* outFile, char* fileBuffer, HuffmanMap* myMap, int fileLength)
{

	int i;


	// lungimea fisierului in biti
	int compressedLength = determineCompressedLength(myMap) + 1;
	printf("\nFile Length: %d bytes\nCompressed Length: %d bytes\n", fileLength, compressedLength);


	//aloc spatiu pentu compressedBuffer
	unsigned char* compressedBuffer = (unsigned char*) malloc (3 * (compressedLength) *  sizeof(unsigned char));
	
	//caracterul curent
	unsigned char current;
	
	//variabila auxiliara care memoreaza pozitia la care am ramas in compressedBuffer
	int* iterator = (int*) calloc (1, sizeof(int));

	//variabila auxiliara care memoreaza numarul de biti parcursi
	int* bitFlag = (int*) calloc (1, sizeof(int));

	// vector care stocheaza 8 de biti
	unsigned char *temp = (unsigned char*) calloc ( 9, sizeof(unsigned char));
	

	// creez un hashTable pentru acces in O(1) la elemente
	// retin pozitiile i din myMap in hashTable[myMap->data[i]]
	int* hashTable = (int*) malloc (255 * sizeof(int));
	createHashTable(hashTable, myMap);


	printf("\n*****FISIERUL CODIFICAT ESTE******\n\n");
	//functie care parcurge textul si il codifica in compressedBuffer
	for (i = 0; i < fileLength; i++)
	{
		
		current = *(fileBuffer + i);

		//determin pozitia caracterului curent in myMap cu ajutorul hashTable-ului
		int position = returnPosition(current, hashTable);

		//incarc in compressedBuffer informatia
		bitFlag = loadCompressedBuffer(myMap, position, compressedBuffer, iterator, bitFlag, temp);
		
	}
	printf("\n\n");
	
	writeBufferToFile(compressedBuffer, outFile, iterator);

	free(fileBuffer);
	free(compressedBuffer);
	/*fac curatenie la locul de munca
	freePointers(myMap, fileBuffer, compressedBuffer, iterator, bitFlag, temp);*/
	
}


/********************************************************
*********** FUNCTII PENTRU LUCRUL CU FISIERE ************
*************** PARTEA II -  DECODIFICARE ***************
********************************************************/




/****************************************************************************** 
- functie care parcurge arborele (st,dr) in functie de bitul curent
- se opreste cand s-a terminat vectorul temp, si intoarce pozitia curenta
*******************************************************************************/

MinHeapNode* findElement(MinHeapNode* root, int* temp, char* decodedBuffer, int* iterator, MinHeapNode* currentNode)
{
	int i;

	for (i = 0; i < 8; i++)
	{

		if(*(temp + i) == 0)
		{

			currentNode = currentNode->st;
		}
		else
		{

			currentNode = currentNode->dr;
		}
	
		// am gasit caracterul
		if (isLeaf(currentNode)) 
		{
	
			//salvez caracterul gasit in buffer si incrementez iteratorul
			*(decodedBuffer + *iterator) = currentNode->data;
			printf("%c", currentNode->data);
			(*iterator)++;
			currentNode = root;
		}
	}

	//daca nu am identificat un caracter, continui cu arborele de unde am ramas
	return currentNode;
}


/*********************************************************************************************
- functia returneaza pozitia curenta in arbore, daca ultimul bit nu a descifrat un caracter 
- functia returneaza radacina in caz contrar
- functia se apeleaza de fiecare data cand este citit un byte din fisier
- verifica daca in byte-ul curent se afla caractere
- currentNode memoreaza pozitia curenta din arbore, fapt care imi asigura decodificare in O(1)
- verific dupa fiecare byte citit ce caractere contine temp
- functia memoreaza caracterele gasite in decodedBuffer
- daca fisierul nu a fost alterat, atunci parcurgerile arborelui il decodifica
**********************************************************************************************/
MinHeapNode* decodeByte (MinHeapNode* root, unsigned char* currentByte, 
		char* decodedBuffer, int* iterator, MinHeapNode* currentNode)
{
	
	int i;
	int* temp = (int*) malloc (9 * sizeof(int));
	
	for(i = 0; i < 8; i ++)
	{

		//determin bitul curent si il memorez la pozitia temp[i]]
		
		*(temp + i) = (*currentByte & (1<<7-i)) / (1<<7-i);
		//printf("%d", *(temp + i));	
	}
	//printf("\nInauntrul lui temp_%d: ", *iterator);
	//printTempInt(temp);

	//caut caractere in byte-ul curent, pana ajung la final, si memorez pozitia la care am ramas
	currentNode = findElement(root, temp, decodedBuffer, iterator, currentNode);


	return currentNode;
}


// functie auxiliara care imi scrie buffer-ul decodificat in fisier
void writeBufferToFile2(char* buffer, FILE* outFile, int* length)
{

	fwrite(buffer, sizeof(char), *length, outFile);
	printf("\n********** Fisierul decodedFile.txt a fost scris. **********\n");
	fclose(outFile);
}


/************************************************************************************
- root : am nevoie de arborele Huffman pentru a putea decodifica bitii in O(1) 
- currentNode : nod folosit pentru parcurgerea arborelui si pentru retinerea pozitiei
- buffer : incarca in buffer continutul fisierului
- currentByte : octetul curent
- decodedBuffer : aloc suficient spatiu pentru buffer-ul decodificat
- iterator : am nevoie de o variabila care actualizeaza lungimea curenta a buffer-ului
**************************************************************************************/
void decodeFile(FILE* outFile, HuffmanMap* myMap, int uncompressedLength, FILE* decodedFile)
{
	
	outFile = fopen ("compressedFile.txt", "r");
	int length = getFileLength(outFile); 
	int i;
	MinHeapNode* root = buildHuffmanTree(myMap->data, myMap->freq, myMap->count); 
	MinHeapNode* currentNode = root;
	unsigned char* buffer = loadBuffer(outFile, length); 
	unsigned char* currentByte = (unsigned char*) malloc (sizeof(unsigned char));
	char* decodedBuffer = (char*) malloc (3 * uncompressedLength * sizeof(char));
	int* iterator = (int*) calloc (1, sizeof(int));
	
	printf("\n\n*********** DECODIFICARE ***********\n\n");
	for(i = 0; i < length; i++)
	{
		//printf("\nOctetul_%d: ", i);
		*currentByte = *(buffer + i);
		currentNode = decodeByte(root, currentByte, decodedBuffer, iterator, currentNode); 
	}
	
	printf("\n");

	decodedFile = fopen("decodedFile.txt", "w");
	writeBufferToFile2(decodedBuffer, decodedFile, iterator);
}




/****************************************************************************
- Constrieste arborele Huffman pe baza caracterelor si a frecventei acestora
- Creaza un vector de vectori pentru codurile caracterelor
- Creaza un vector care memoreaza lungimile codurilor caracterelor
- Memoreaza codurilor caracterelor
- Memoreaza toate datele structurii in myMap
*****************************************************************************/
HuffmanMap* HuffmanCodes(char data[], int freq[], int size) 
{ 

	int** codes = createCodesArray(size);
	int* bitLength = (int*) malloc (size * sizeof(int));
	MinHeapNode* root = buildHuffmanTree(data, freq, size); 
	int arr[100], top = 0;
	storeCodes(root, arr, top, codes, bitLength);
	HuffmanMap* myMap = createHuffmanMap(data, freq, bitLength, codes, size);

	return myMap;
} 



long double computeHuffmanAverage(HuffmanMap* myMap, int length)
{

	int i;
	long double H = 0;

	for(i = 0; i < myMap->count; i++)
	{
	
		H += myMap->freq[i] * myMap->bitLength[i];
	}
	H /= length;

	return H;
}

/*************************************************************
			 FUNCTIA MAIN 
*************************************************************/
int main (int argc, char **argv)
{

/******************************************************************************************
Etapa I:
Execut toate functiile necesare pentru:
- deschiderea fisierului
- determinarea numarului de caractere din fisier
- incarcarea in buffer a continutului fisierului
- determinarea frecventelor si a probabilitatilor de aparitie a fiecarui caracter in parte
- calcularea informatiei Shannon
*******************************************************************************************/
	
	FILE *inFile = fopen(argv[1], "r");
	int length = getFileLength(inFile); 
	char* buffer = loadBuffer(inFile, length);
	printBuffer(buffer, length); 
	computeProbabilities(buffer, length); 
	long double I = computeShannonInformation();

/**********************************************************
 Etapa II:
- execut codificarea Huffmann pentru fisierul rezultat
- salvez toate datele in structura myMap
- afisez structura obtinuta
- execut functia care imi scrie fisierul in compressedFile
**********************************************************/	

	int count = returnCount();
	int *frecv = createFrecv(count);
	char *array = createData(count);
	HuffmanMap *myMap = HuffmanCodes(array, frecv, count);
	printMap(myMap);

	FILE *compressedFile = fopen("compressedFile.txt", "wb");
	writeCompressedFile(compressedFile, buffer, myMap, length);



/****************************************************
 Etapa III:
- execut functia care imi scrie fisierul decodificat
****************************************************/
	
	FILE *decodedFile;
	decodeFile(compressedFile, myMap, length, decodedFile);

	long double H = computeHuffmanAverage(myMap, length);
	printf("\nIn medie, codul huffman foloseste %Lf biti pentru codificarea a %Lf biti de informatie.\n", H, I);

	return 0;
}

