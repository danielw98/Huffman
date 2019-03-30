
#include <stdio.h> 
#include <math.h>
#include <stdlib.h>

// Partea I
//declarate global pentru a nu trata initializarea cu 0.
int nr_aparitii[255];
double probabilitati[255];
char* codes[255];

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

void test (char* buffer)
{
	int i;
	for(i = 0; i < 10; i++)
	{	
		// afiseaza litera si codul ASCII asociat
		printf("%c: %i\n", *(buffer+i), *(buffer+i)); 
	}
}


void computeProbabilities(char* buffer, int length)
{
	int i;
	//functie pentru determinarea frecventei aparitei caracterelor
	for(i = 0; i < length; i++)
	{
		nr_aparitii[*(buffer+i)]++;
	}
	
	//functie pentru determinarea probabilitatilor pe baza lungimii textului
 	for(i = 0; i < 255; i++)
	{
		probabilitati[i] = (float) nr_aparitii[i] / length;
	}
}

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
}


// Partea II
// un nod din arborele Huffman
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
void Heapify(struct MinHeap* minHeap, int i) 
{ 

	int minim = i; 
	int st = 2 * i + 1; 
	int dr = 2 * i + 2; 
	
	
	if (st < minHeap->size && minHeap->array[st]->freq < minHeap->array[minim]->freq)
	{

		minim = st; 
	}

	if (dr < minHeap->size && minHeap->array[dr]->freq < minHeap->array[minim]->freq) 
	{

		minim = dr; 
	}

	if (minim != i) 
	{ 

		swapMinHeapNode(&minHeap->array[minim], &minHeap->array[i]); 
		Heapify(minHeap, minim); 
	} 
} 

// functie care verifica daca dimensiunea Heap-ului este 1
int isSizeOne(MinHeap* minHeap) 
{ 

	return (minHeap->size == 1); 
} 

// functie care extrage nodul minim din heap
MinHeapNode* extractMin(MinHeap* minHeap) 
{ 

	//minimul este elementul din cap, pastrat in temp
	MinHeapNode* temp = minHeap->array[0];
	//il inlocuim cu ultimul element
	minHeap->array[0] = minHeap->array[minHeap->size - 1];
	//scadem dimensiunea heap-ului
	--minHeap->size;
	//refacem proprietatea de heap
	Heapify(minHeap, 0); 

	return temp; 
} 


//functie pentru inserarea unui nou nod in heap??
void insertMinHeap(MinHeap* minHeap, MinHeapNode* minHeapNode) 
{ 
	++minHeap->size; 
	int i = minHeap->size - 1; 
	
	// caut pozitia nodului in Heap
	while (i && minHeapNode->freq < minHeap->array[(i - 1) / 2]->freq) 
	{ 
		
		minHeap->array[i] = minHeap->array[(i - 1) / 2]; 
		i = (i - 1) / 2; 
	} 

	minHeap->array[i] = minHeapNode; 
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


// afisarea unui vector de dimensiune n
char* print(int arr[], int n) 
{ 
	
	char* a = (char*) calloc (n * sizeof(char));
	int i; 
	for (i = 0; i < n; i++)
	{

		printf("%d", arr[i]);
		*(a+i) = (char) arr[i];
	}
	printf("\n");

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
	
		minHeap->array[i] = newNode(data[i], freq[i]); 
	}

	// stabilesc dimensiunea
	minHeap->size = size;
	
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

		root = newNode('$', st->freq + dr->freq); 

		root->st = st; 
		root->dr = dr; 

		insertMinHeap(minHeap, root); 
	} 

	// Nodul ramas este radacina. Arborele este complet.
	return extractMin(minHeap); 
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
		printCodes(root->st, arr, top + 1); 
	} 

	if (root->dr) 
	{ 

		arr[top] = 1; 
		printCodes(root->dr, arr, top + 1); 
	} 

	/*
	- Daca nodul curent este frunza, atunci contine un caracter.
	- Afisez caracterul si codul asociat
	*/
	if (isLeaf(root)) 
	{ 

		printf("%c: ", root->data); 
		print(arr, top);
	} 
} 

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


char* createData(int count)
{

	int i, k = 0;
	char* data = (char*) malloc (count * sizeof(char));
	
	for (i = 0; i < 255; i ++)
		if( nr_aparitii[i] != 0)
		{	
			*(data+k) = (char) i;
			k++;
		}
	return data;
}


int* createFrecv(int count)
{
	int i, k = 0;
	int* frecv = (int*) malloc (count * sizeof(int));
	for (i = 0; i < 255; i++)
	{
		if ( nr_aparitii[i] != 0)
		{
			*(frecv+k) = nr_aparitii[i];
			k++;
		}
	}
	return frecv;
	
}
/* 
- construieste arborele Huffman
- afiseaza codurile prin traversare
*/

void storeCodes (MinHeapNode* root, int arr[], int top)
{
	char* string[];
}
void HuffmanCodes(char data[], int freq[], int size) 
{ 
	
	// Constructia arborelui
	MinHeapNode* root = buildHuffmanTree(data, freq, size); 

	// Afisarea arborelui rezultat
	int arr[100], top = 0; 

	printCodes(root, arr, top);
	storeCodes(root, arr, top);
} 

// functii pentru memorarea codurilor, scrierea in fisier binar a codului rezultat, si descifrarea acestuia.
 
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
	HuffmanCodes(array, frecv, count);
	return 0;
}

