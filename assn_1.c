#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/time.h>

int inMemoryLinearSearch(char *keyFileName, char *seekFileName);
void performInMemoryLinearSearch(int *keyArray,int keyLen,int *seekArray,int seekLen, double readTime);
int inMemoryBinarySearch(char *keyFileName, char *seekFileName);
void performInMemoryBinarySearch(int* keyArray, int* seekArray, int seekLen, int keyLen, double readTime);
int binarySearchMemory(int* keyArray, int seekValue, int min, int max);
int onDiskLinearSearch(char *keyFileName, char *seekFileName);
void performOnDiskLinearSearch(FILE* keyFile, int seekLen, int* seekArray, int keyLen);
int onDiskBinarySearch(char *keyFileName, char *seekFileName);
void performOnDiskBinarySearch(FILE* keyFile, int seekLen, int* seekArray, int keyLen);
int binarySearchDisk(FILE* keyFile, int seekValue, int min, int max);
double getAccurateTime(struct timeval startTime, struct timeval endTime);

/*
=============================================================================================
				    Main Function
=============================================================================================
*/

struct timeval startTime, endTime;

int main(int argc, char *argv[])
{
	char *keyFileName, *seekFileName;
	if(argc!=4)
	{
		printf("\nToo few arguments. Expected 3 arguments. Found only %d.\n", (argc-1));
		return 0;
	}

	keyFileName = argv[2];
	seekFileName = argv[3];

	if (strcmp(argv[1], "--mem-lin") == 0)
		inMemoryLinearSearch(keyFileName, seekFileName);
	else if (strcmp(argv[1], "--mem-bin") == 0)
		inMemoryBinarySearch(keyFileName, seekFileName);
	else if (strcmp(argv[1], "--disk-lin") == 0)
		onDiskLinearSearch(keyFileName, seekFileName);
	else if (strcmp(argv[1], "--disk-bin") == 0)
		onDiskBinarySearch(keyFileName, seekFileName);
	else
		printf("\nError: Invalid search mode specified\n");

	return 0;
}


/*
=============================================================================================
				In Memory Linear Search
=============================================================================================
*/

int inMemoryLinearSearch(char *keyFileName, char *seekFileName)
{
	FILE *keyInputFile, *seekInputFile;
	int seekFileLenInBytes, keyFileLenInBytes, keyLen, seekLen,i, getVal;
	int *keyArray = NULL, *seekArray = NULL;
	double accTime;

	getVal = 0;
	keyInputFile = fopen(keyFileName, "rb");
	if( keyInputFile == NULL )
	{
		printf("\nKey File cannot be opened.\n");
		return 0;
	}

	seekInputFile = fopen(seekFileName, "rb");
	if( seekInputFile == NULL )
	{
		printf("\nSeek File cannot be opened.\n");
		return 0;
	}


	fseek(keyInputFile, 0, SEEK_END);
	keyFileLenInBytes = ftell(keyInputFile);
	fseek(keyInputFile, 0, SEEK_SET);
	fclose(keyInputFile);

	fseek(seekInputFile, 0, SEEK_END);
	seekFileLenInBytes = ftell(seekInputFile);
	fseek(seekInputFile, 0, SEEK_SET);
	
	keyArray = (int*)malloc(keyFileLenInBytes);
	seekArray = (int*)malloc(seekFileLenInBytes);

	keyLen = 0;seekLen = 0;

	gettimeofday(&startTime, NULL);
	keyInputFile = fopen("key.db", "rb");
	while(!feof(keyInputFile))
	{
		fread((keyArray+keyLen), sizeof(int), 1, keyInputFile);
		keyLen++;
	}
	gettimeofday(&endTime, NULL);
	accTime = getAccurateTime(startTime, endTime);	

	while(!feof(seekInputFile))
	{
		fread((seekArray+seekLen), sizeof(int), 1, seekInputFile);
		seekLen++;
	}
	seekLen--;
	keyLen--;

	performInMemoryLinearSearch(keyArray, keyLen, seekArray, seekLen, accTime);
	fclose(keyInputFile);
	fclose(seekInputFile);
	return 0;
}


void performInMemoryLinearSearch(int *keyArray,int keyLen,int *seekArray,int seekLen,double readTime)
{
	int hit[seekLen], seekCount, keyCount, count;
	double accTime;

	gettimeofday(&startTime, NULL);
	for(seekCount = 0; seekCount<seekLen; seekCount++)
	{
		for(keyCount = 0; keyCount<keyLen; keyCount++)
		{
			if(seekArray[seekCount] == keyArray[keyCount])
			{
				hit[seekCount] = 1;
				break;
			}
			else
				hit[seekCount] = 0;
		}
	}
	gettimeofday(&endTime, NULL);

	accTime = getAccurateTime(startTime, endTime);
	accTime+=readTime;

	for(seekCount = 0; seekCount<seekLen; seekCount++)
	{
		if( hit[seekCount] == 1 )
			printf( "%12d: Yes\n", seekArray[seekCount] );
		else
			printf( "%12d: No\n", seekArray[seekCount] );
	}
	printf( "Time: %.6f\n", accTime);
}


/*
=============================================================================================
				In Memory Binary Search
=============================================================================================
*/

int inMemoryBinarySearch(char *keyFileName, char *seekFileName)
{
	FILE *keyInputFile, *seekInputFile;
	int seekFileLenInBytes, keyFileLenInBytes, keyLen, seekLen,i, getVal;
	int *keyArray = NULL, *seekArray = NULL;
	double accTime;

	getVal = 0;
	keyInputFile = fopen(keyFileName, "rb");
	if( keyInputFile == NULL )
	{
		printf("\nKey File cannot be opened.\n");
		return 0;
	}

	seekInputFile = fopen(seekFileName, "rb");
	if( seekInputFile == NULL )
	{
		printf("\nSeek File cannot be opened.\n");
		return 0;
	}

	fseek(keyInputFile, 0, SEEK_END);
	keyFileLenInBytes = ftell(keyInputFile);
	fseek(keyInputFile, 0, SEEK_SET);
	fclose(keyInputFile);

	fseek(seekInputFile, 0, SEEK_END);
	seekFileLenInBytes = ftell(seekInputFile);
	fseek(seekInputFile, 0, SEEK_SET);
	
	keyArray = (int*)malloc(keyFileLenInBytes);
	seekArray = (int*)malloc(seekFileLenInBytes);

	keyLen = 0;
	seekLen = 0;

	gettimeofday(&startTime, NULL);
	keyInputFile = fopen(keyFileName, "rb");
	if( keyInputFile == NULL )
	{
		printf("\nKey File cannot be opened.\n");
		return 0;
	}
	while(!feof(keyInputFile))
	{
		fread((keyArray+keyLen), sizeof(int), 1, keyInputFile);		
		keyLen++;
	}
	gettimeofday(&endTime, NULL);

	accTime = getAccurateTime(startTime, endTime);

	while(!feof(seekInputFile))
	{
		fread((seekArray+seekLen), sizeof(int), 1, seekInputFile);
		seekLen++;
	}
	seekLen--;
	keyLen--;

	performInMemoryBinarySearch(keyArray, seekArray, seekLen, keyLen, accTime );

	fclose(keyInputFile);
	fclose(seekInputFile);
	return 0;
}


void performInMemoryBinarySearch(int* keyArray, int* seekArray, int seekLen, int keyLen, double readTime)
{
	int seekCount = 0, getVal = 0, count = 0;
	int hit[seekLen];
	double accTime;


	gettimeofday(&startTime, NULL);
	for(seekCount = 0; seekCount<seekLen; seekCount++)
	{
		getVal = binarySearchMemory(keyArray, seekArray[seekCount], 0, (keyLen-1));
		if (getVal == 1)
			hit[seekCount] = 1;
		else
			hit[seekCount] = 0;
	}
	gettimeofday(&endTime, NULL);	
	accTime = getAccurateTime(startTime, endTime);

	accTime+=readTime;
	for(seekCount = 0; seekCount<seekLen; seekCount++)
	{
		if( hit[seekCount] == 1 )
			printf( "%12d: Yes\n", seekArray[seekCount] );
		else
			printf( "%12d: No\n", seekArray[seekCount] );
	}
	printf( "Time: %0.6f\n", accTime);
}


int binarySearchMemory(int* keyArray, int seekValue, int min, int max)
{
	int midVal = 0;
	if(max < min)
		return(0);
	else
	{
		midVal = ((max+min)/2);
		
		if (keyArray[midVal]<seekValue)
		{
			return(binarySearchMemory(keyArray, seekValue, (midVal+1), max));
		}
		else if (keyArray[midVal]>seekValue)
		{
			return(binarySearchMemory(keyArray, seekValue, min, (midVal-1)));
		}
		else
			return(1);
	}

	return(0);
}


/*
=============================================================================================
				On Disk Linear Search
=============================================================================================
*/

int onDiskLinearSearch(char *keyFileName, char *seekFileName)
{
	FILE *keyInputFile, *seekInputFile;
	int seekFileLenInBytes, keyFileLenInBytes, keyLen, seekLen,i;
	int *seekArray = NULL;
	
	seekInputFile = fopen(seekFileName, "rb");
	if( seekInputFile == NULL )
	{
		printf("\nSeek File cannot be opened.\n");
		return 0;
	}

	fseek(seekInputFile, 0, SEEK_END);
	seekFileLenInBytes = ftell(seekInputFile);
	fseek(seekInputFile, 0, SEEK_SET);
	seekArray = (int*)malloc(seekFileLenInBytes);

	seekLen = 0;
	while(!feof(seekInputFile))
	{
		fread((seekArray+seekLen), sizeof(int), 1, seekInputFile);
		seekLen++;
	}
	seekLen--;



	keyInputFile = fopen(keyFileName, "rb");
	if( keyInputFile == NULL )
	{
		printf("\nKey File cannot be opened.\n");
		return 0;
	}

	fseek(keyInputFile, 0, SEEK_END);
	keyFileLenInBytes = ftell(keyInputFile);
	fseek(keyInputFile, 0, SEEK_SET);
	
	keyLen = (keyFileLenInBytes/(sizeof(int)));
	performOnDiskLinearSearch(keyInputFile, seekLen, seekArray, keyLen);

	fclose(keyInputFile);
	fclose(seekInputFile);
	return 0;
}


void performOnDiskLinearSearch(FILE* keyFile, int seekLen, int* seekArray, int keyLen)
{
	int readKeyValue = 0, seekCount = 0, count = 0;
	int hit[seekLen];
	double accTime;

	gettimeofday(&startTime, NULL);
	for(seekCount = 0; seekCount<seekLen; seekCount++)
	{
		fseek(keyFile, 0, SEEK_SET);
		while(!feof(keyFile))
		{			
			fread(&readKeyValue, sizeof(int), 1, keyFile);
			if(seekArray[seekCount] == readKeyValue)
			{
				hit[seekCount] = 1;
				count++;
				break;
			}
			else
				hit[seekCount] = 0;
		}
	}
	gettimeofday(&endTime, NULL);	
	accTime = getAccurateTime(startTime, endTime);

	for(seekCount = 0; seekCount<seekLen; seekCount++)
	{
		if( hit[seekCount] == 1 )
			printf( "%12d: Yes\n", seekArray[seekCount] );
		else
			printf( "%12d: No\n", seekArray[seekCount] );
	}
	printf( "Time: %0.6f\n", accTime);
}


/*
=============================================================================================
				On Disk Binary Search
=============================================================================================
*/

int onDiskBinarySearch(char *keyFileName, char *seekFileName)
{
	FILE *keyInputFile, *seekInputFile;
	int seekFileLenInBytes, keyFileLenInBytes, keyLen, seekLen,i;
	int *seekArray = NULL;
	
	seekInputFile = fopen(seekFileName, "rb");
	if( seekInputFile == NULL )
	{
		printf("\nSeek File cannot be opened.\n");
		return 0;
	}

	fseek(seekInputFile, 0, SEEK_END);
	seekFileLenInBytes = ftell(seekInputFile);
	fseek(seekInputFile, 0, SEEK_SET);
	seekArray = (int*)malloc(seekFileLenInBytes);

	seekLen = 0;
	while(!feof(seekInputFile))
	{
		fread((seekArray+seekLen), sizeof(int), 1, seekInputFile);
		seekLen++;
	}
	seekLen--;


	keyInputFile = fopen(keyFileName, "rb");
	if( keyInputFile == NULL )
	{
		printf("\nKey File cannot be opened.\n");
		return 0;
	}

	fseek(keyInputFile, 0, SEEK_END);
	keyFileLenInBytes = ftell(keyInputFile);
	fseek(keyInputFile, 0, SEEK_SET);
	
	keyLen = (keyFileLenInBytes/(sizeof(int)));
	performOnDiskBinarySearch(keyInputFile, seekLen, seekArray, keyLen);

	fclose(keyInputFile);
	fclose(seekInputFile);
	return 0;
}


void performOnDiskBinarySearch(FILE* keyFile, int seekLen, int* seekArray, int keyLen)
{
	int seekCount = 0, getVal = 0, count1 = 0;
	int hit[seekLen];
	double accTime;

	gettimeofday(&startTime, NULL);
	for(seekCount = 0; seekCount<seekLen; seekCount++)
	{
		getVal = binarySearchDisk(keyFile, seekArray[seekCount], 0, (keyLen-1));
		if (getVal == 1)
			hit[seekCount] = 1;
		else
			hit[seekCount] = 0;
	}	
	gettimeofday(&endTime, NULL);	

	accTime = getAccurateTime(startTime, endTime);

	for(seekCount = 0; seekCount<seekLen; seekCount++)
	{
		if( hit[seekCount] == 1 )
			printf( "%12d: Yes\n", seekArray[seekCount] );
		else
			printf( "%12d: No\n", seekArray[seekCount] );
	}
	printf( "Time: %0.6f\n", accTime);
}


int binarySearchDisk(FILE* keyFile, int seekValue, int min, int max)
{
	int midVal = 0, readKeyValue = 0;
	if(max < min)
		return(0);
	else
	{
		fseek(keyFile, 0, SEEK_SET);
		midVal = ((max+min)/2);
		fseek(keyFile, (midVal*sizeof(int)), SEEK_SET);
		fread(&readKeyValue, sizeof(int), 1, keyFile);
		
		if (readKeyValue<seekValue)
		{
			return(binarySearchDisk(keyFile, seekValue, (midVal+1), max));
		}
		else if (readKeyValue>seekValue)
		{
			return(binarySearchDisk(keyFile, seekValue, min, (midVal-1)));
		}
		else
			return(1);
	}

	return(0);
}



double getAccurateTime(struct timeval startTime, struct timeval endTime)
{
	double accTime = ( ((double)endTime.tv_sec + 1.0e-6*endTime.tv_usec) - ((double)startTime.tv_sec + 1.0e-6*startTime.tv_usec) );

	return(accTime);
}

