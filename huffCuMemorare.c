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


// afiseaza litera si codul ASCII asociat primelor 10 caractere
void test (char* buffer)
{

	int i;
	for(i = 0; i < 10; i++)
	{	

		printf("%c: %i\n", *(buffer+i), *(buffer+i)); 
	}
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
long double ShannonInformation()
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


//afiseaza frecventa caracterelor din text
void printFrequencies()
{

	int i;
	for(i = 0; i < 255; i++)
	{

		if(nr_aparitii[i]!= 0)
		{

			printf("%c apare de %d ori\n", i, nr_aparitii[i]);
		}
	}

	printf("\n\n");
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
	
	if (st < minHeap->size && minHeap->array[st]->freq < minHeap->array[minim]->freq)
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
-Functie recursiva care afiseaza codurile Huffmann de la radacina in jos
si le retine in arr[] 
- asigneaza 0 pentru parcurgere la stanga, si 1 pentru parcurgere la dreapta
*/
void printCodes(MinHeapNode* root, int arr[], int top) 

{ 
 
	if (root->st) 
	{ 

		arr[top] = 0; 
		printCodes(root -> st, arr, top + 1); 
	} 

	if (root->dr) 
	{ 

		arr[top] = 1; 
		printCodes(root -> dr, arr, top + 1); 
	} 


	/*
	- Daca nodul curent este frunza, atunci contine un caracter.
	- Afisez caracterul si codul asociat
	*/
	if (isLeaf(root)) 
	{ 

		printf("%c: ", root -> data); 
		print(arr, top);

	} 
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
	while(root->data != *(data+pos))
	{

		pos++;
	}
	if(root->data == *(data+pos))
	{

		printf("Pozitia este: %d\n", pos);
	}

	return pos;
}


// functie auxiliara - creaza un vector de vectori elemente de dimensiune necunoscuta momentan
int** createCodesArray (int count)
{
	
	int** codes = (int**) malloc (count * sizeof(int*));

	return codes;
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
		printf("Caracterul %c: ", root -> data);
		printElement(*(codes + pos), top);
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

		printf("%d", *(character+i));
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

/*void writeCompressedFile(FILE* outFile, char* buffer, HuffmanMap myMap)
{

	int i;
	int k = 0; // lungimea fisierului in biti
	for (i = 0; i < length; i++)
	{
		
			
	}
}*/

FILE* decodeFile(FILE* outFile, int** codes, char* array)
{


}






/*
************CONSTRUIESTE ARBORELE HUFFMAN************
**********AFISEAZA CODURILE PRIN TRAVERSARE**********
*****************MEMOREAZA CODURILE******************
*************RETURNEAZA CODURILE MEMORATE************
*/

HuffmanMap* HuffmanCodes(char data[], int freq[], int size) 
{ 

	//creez un vector de vectori pentru codurile caracterelor
	int** codes = createCodesArray(size);
	
	//creez un vector care memoreaza lungimile codurilor caracterelor
	int* bitLength = (int*) malloc (size * sizeof(int));

	// Constructia arborelui
	MinHeapNode* root = buildHuffmanTree(data, freq, size); 

	// Afisarea arborelui rezultat
	int arr[100], top = 0; 
	printCodes(root, arr, top);

	// Memorarea codurilor caracterelor
	printf("\n\n********* TESTAREA FUNCTIEI STORECODES **********\n\n");
	storeCodes(root, arr, top, codes, bitLength);
	
	//Memorez toate datele structurii in myMap
	HuffmanMap* myMap = createHuffmanMap(data, freq, bitLength, codes, size);
	return myMap;
} 






/* 
********************************************************************************************************
********************************************* FUNCTIA MAIN *********************************************
******************************************************************************************************** 
*/
int main (int argc, char **argv)
{

/* Etapa I:
	Execut toate functiile necesare pentru deschiderea fisierului, determinarea lungimii, incarcarea in buffer, calcularea ponderilor, si determinarea informatiei Shannon */
	
//deschiderea fisierului
	FILE *inFile = fopen(argv[1], "r");

//determina numarul de caractere din fisier
	int len = getFileLength(inFile); 

//incarca in buffer continutul fisierului
	char* buffer = loadBuffer(inFile, len); 

// verifica pentru primele 10 caractere
	test(buffer); 

/* determina frecventa si probabilitatile de aparitie ale fiecarui 
caracter in parte */
	computeProbabilities(buffer, len); 

// functie pentru calcularea informatiei Shannon
	long double I = ShannonInformation();

// afisarea datelor obtinute
	printf("Informatia Shannon este: %Lf\n", I);
	printf("Frecventa caracterelor este: \n");
	printFrequencies();

/* Etapa II:
	Execut codificarea Huffmann pentru fisierul rezultat
*/	
	int count = returnCount();
	int *frecv = createFrecv(count);
	char *array = createData(count);
	
// salvez codificarea intr-un vector de coduri si verific functionalitatea
	HuffmanMap *myMap = HuffmanCodes(array, frecv, count);
	
	//verificarea structurii obtinute
	printMap(myMap);

// codific fisierul obtinut in outFile
	//int* bitLength = ;
	//FILE *outFile = fopen(argv[2], "wb");
	//writeCompressedFile(outFile, buffer, codes, array, frecv, len, bitLength);

// decodific fisierul pentru verificare
	//FILE *decodedFile = fopen("fisier_decodificat.txt", "w");
	//decodedFile = decodeFile(outFile, codes, array);


	return 0;
}

