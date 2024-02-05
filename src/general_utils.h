/*
 * returns a substring until it reaches
 * the end of string or the character specified by stopChar
 * returns null if given string is null or malloc fails
 * 
 * side effect: must free returned pointer
 */
char* substring_until_delimiter(char* givenString, unsigned int index, char stopChar)
{
	if(givenString == NULL)
		return NULL;
	
	size_t size = strlen(givenString);
	unsigned int count = 0;
	char* substr = (char*) malloc(sizeof(char) * (size + 1));

	//check if memory allocation was successful
	if(substr == NULL)
		return NULL;
	
	//collect characters into substr
	while(givenString[index] != stopChar && givenString[index] != '\0' && index < size)
	{
		substr[count] = givenString[index];
		count++;
		index++;
	}
	
	substr[count] = '\0'; //null terminate the string
	return substr;
}


/*
 * returns a substring based on the base 
 * index and the number of characters you want
 * from a given substring
 * returns null if given string is null or calloc fails 
 * 
 * side effect: must free returned pointer
 */
char* substring(char* givenString, unsigned int index, unsigned int numChars)
{
	//null check
	if(givenString == NULL)
		return NULL;
	
	//size check
	if((index + numChars) > strlen(givenString))
		return NULL;
	
	//allocate space for substring
	char* substr = (char*) malloc(sizeof(char) * (numChars + 1));
	
	//check if memory allocation was successful
	if(substr == NULL)
		return NULL;
	
	//copy over substring and null terminate
	strncpy(substr, givenString + (index * sizeof(char)), numChars);
	substr[numChars] = '\0';
	return substr;
}

	
/*
 * returns the size of a null terminated 
 * character array or -1 if the array is null
 */
int str_array_size(char** arry)
{
	if(arry == NULL)
		return -1;
	
	int i = 0;
	while(arry[i] != NULL)
		i++;
	
	return i;
}


/*
 * returns the size of a -1 terminated 
 * integer array or -1 if the array is null
 *
 * side effect: if the integer array is not 
 * terminated with -1 this function will result
 * in undefined behavior
 */
int int_array_size(const int arry[])
{
	if(arry == NULL)
		return -1;
	
	unsigned int i = 0;
	while(arry[i] != -1 && i < UINT_MAX)
		i++;
	
	return i;
}


/*
 * prints out the contents of an integer array
 * does nothing if array is null
 */
void print_int_array(const int arry[])
{
	if(arry == NULL)
		return;
	
	unsigned int i = 0;
	while(arry[i] != -1)
	{
		printf("%d ", arry[i]);
		i++;
	}	
	printf("\n");
}


/*
 * prints out the contents of a double character pointer
 * does nothing if the array is null
 */
void print_str_array(char** arry)
{
	if(arry == NULL)
		return;
	
	for(unsigned int i = 0; arry[i] != NULL; i++)
		printf(" %s", arry[i]);	
}


/*
 * returns the current time the following format
 * "Nov 20 17:11:10"
 */
char* get_current_time()
{
	time_t t;
	struct tm *tInfo;
	static char buffer[80];
	
	time(&t);
	tInfo = localtime(&t);
	
	strftime(buffer, 80, "%b %d %H:%M:%S", tInfo);
	return buffer;
}


/*
 * finds the difference between two time stamps
 * returns -1 if invalid timestamps
 */
double time_diff_seconds(char* time1, char* time2)
{	
	if(time1 == NULL || time2 == NULL)
		return -1;
	
	if(strcmp(time1, time2) == 0)
		return 0;
	
	//instantiate structs
	struct tm t1 = {0};
	struct tm t2 = {0};
    time_t tt1;
	time_t tt2;
	
	//turn both inputs into tm structs
    sscanf(time1, "%d:%d:%d", &t1.tm_hour, &t1.tm_min, &t1.tm_sec);
    sscanf(time2, "%d:%d:%d", &t2.tm_hour, &t2.tm_min, &t2.tm_sec);
	
	//seconds since last epoch
    t1.tm_year = t2.tm_year = 70; // years since 1900
    t1.tm_mon = t2.tm_mon = 0; // months since January [0-11]
    t1.tm_mday = t2.tm_mday = 1; // day of the month [1-31]
    t1.tm_isdst = t2.tm_isdst = 0; // Daylight Saving Time flag
    tt1 = mktime(&t1);
    tt2 = mktime(&t2);
	
    return difftime(tt2, tt1);
}


/*
 * frees all pointers given in an array
 * 
 * side effect: must be sure size is correct otherwise
 * undefined behavior can occur
 */
void free_all(void* ptrArray[], unsigned int size)
{
	if(ptrArray == NULL)
		return;
	
	for(unsigned int i = 0; i < size; i++)
	{
		if(ptrArray[i] != NULL)
		{
			free(ptrArray[i]);
			ptrArray[i] = NULL;
		}
	}
}


/*
 * frees all pointers in a double dynamic pointer
 * including itself
 * 
 * side effect: must be sure size is correct otherwise
 * undefined behavior can occur
 */
void free_all_double_char(char** ptrArray[], unsigned int size)
{
	if(ptrArray == NULL)
		return;
	
	for(unsigned int i = 0; i < size; i++)
	{
		for(unsigned int j = 0; ptrArray[i][j] != NULL; j++)
		{
			if(ptrArray[i][j] != NULL)
			{
				free(ptrArray[i][j]);
				ptrArray[i][j] = NULL;
			}
		}
		free(ptrArray[i]);
		ptrArray[i] = NULL;
	}
}


/*
 * frees all pointers given in an array
 * if any null pointers are detected:
 * returns 1 if pointers are all valid
 * returns 0 if invalid pointer detected
 * returns -1 if ptrArray is null
 */
int check_ptr_integrity(void* ptrArray[], unsigned int size)
{	
	if(ptrArray == NULL)
		return -1; 
	
	for(unsigned int i = 0; i < size; i++)
	{
		//if invalid pointer found
		if(ptrArray[i] == NULL)
		{
			free_all(ptrArray, size);
			return 0;
		}
	}
	//all pointers are valid
	return 1;
}


/*
 * returns the parameter associated extracted from
 * the given string, ex. if the string contains dst=22
 * and this function is passed the entire string and "dst="
 * "22" would be returned- null is returned in the event of
 * the target parameter not existing in the string
 * 
 * side effect: must free pointer
 */
char* get_parameter(char* givenString, char* substring)
{
	if(givenString == NULL || substring == NULL)
		return NULL;
	
	char* result = strstr(givenString, substring);
	
	if(result == NULL)
	{
		free(result);
		return NULL;
	}
	
	int index = result - givenString;	
	return substring_until_delimiter(givenString, index + strlen(substring), ' ');
}


/*
 * counts the amount of comma delimited items in 
 * a null terminated character array, returns -1 if 
 * character array is null
 */
int num_items_in_string(char* givenString)
{
	if(givenString == NULL)
		return -1;
	
	int numItems = 0;
	int index = 0;
	
	//check for first item
	if(strlen(givenString) != 0)
		numItems++;		

	//step through string
	while(givenString[index] != '\0' && index < INT_MAX)
	{
		if(givenString[index] == ',')
			numItems++;
		index++;
	}
	return numItems;
}


/*
 * turns a string represented series of words into 
 * a double character pointer ex.
 * "a,b,c,d,e" -> ["a", "b", "c", "d", "e", NULL]
 * returns null if givenString is null
 * 
 * this function appends null to the end of the
 * pointers as a sentinel value
 *
 * side effect: must free sub pointers and pointer
 */
char** text_series_to_str_array(char* givenString)
{		
	if(givenString == NULL)
		return NULL;
	
	//get number of items in givenString and check
	//for null case
	int numItems = num_items_in_string(givenString);
	if(numItems == -1 || numItems == 0)
		return NULL;
		
	//allocate space for character array + 1 for sentinel value
	//also + 1 to arrySize to account for null terminator
	unsigned int arrySize = sizeof(char*) * (numItems + 1);
	char** result = (char**) malloc(arrySize);
	
	//check if malloc failed
	if(result == NULL)
		return NULL;
	
	//create indexes
	unsigned int arryIndex = 0;
	unsigned int strIndex = 0;
	
	//step through string
	while(givenString[strIndex] != '\0' && strIndex < UINT_MAX)
	{
		//if substring found
		if(givenString[strIndex] != ',')
		{
			//extract substring
			char* substr = substring_until_delimiter(givenString, strIndex, ',');
			
			//check if null
			if(substr == NULL)
				return NULL;
			
			//get length of substring
			int substrSize = strlen(substr);
			
			//store it
			result[arryIndex] = substr;
			
			//increment indexes:
			//strIndex gets moved to next item in list by adding
			//size of last found substring
			arryIndex++;
			strIndex += substrSize - 1;
		}
		strIndex++;
	}
	//terminate the integer array with NULL and return
	result[numItems] = NULL;
	return result;
}


/* 
 * turns a null terminated string array into an integer array,
 * the integer array is terminated with a -1,
 * otherwise returns null
 *
 * side effect: must free returned ptr
 */
int* str_array_to_int_array(char** stringArray)
{
	if(stringArray == NULL)
		return NULL;
	
	unsigned int size = str_array_size(stringArray);
	
	//reserve space for ints + 1 for -1 terminator
	int* result = (int*) malloc(sizeof(int) * size + 1);
	
	//check if malloc succeeded
	if(result == NULL)
		return NULL;
	
	for(int i = 0; stringArray[i] != NULL; i++)
		result[i] = atoi(stringArray[i]);
	
	result[size] = -1;
	return result;
}


/*
 * checks to see if a null terminated string array is 
 * an ordered sub array of another null terminated 
 * string array, returns 2 if they have value equality
 * 1 if proper sub array, 0 if its not a proper sub array,
 * and -1 if either array is null, ex:
 *
 * 	str_array_subarray(['a','b','c'], ['a','b','c']) == 2
 * 	str_array_subarray(['a','b'], ['a','b','c']) == 1
 * 	str_array_subarray(['a','b','d'], ['a','b','c']) == 0
 * 	str_array_subarray(['a','d'], ['a','b','c']) == 0
 * 	str_array_subarray(NULL, ['a','b','c']) == -1
 *
 * note: presence of '\0' does not disqualify a subarray
 */
int str_array_subarray(char** stringArray1, char** stringArray2)
{
	if(stringArray1 == NULL || stringArray2 == NULL)
		return -1;
	
	unsigned int i = 0;
	
	//while we haven't made it through the superarray
	while(stringArray1[i] != NULL && stringArray2[i] != NULL && i < UINT_MAX)
	{
		//if we've reached the end of sub array, return 1
		if(stringArray1[i][0] == '\0')
			return 1;
		
		//compare strings in arrays
		int compare = strcmp(stringArray1[i], stringArray2[i]);
		
		//strings in array are not equal
		if(compare != 0)
			return 0;
		
		i++;
	}

	//if stringArray1 ends before stringArray2
	if(stringArray1[i] == NULL && stringArray2[i] != NULL)
		return 1;
	
	//if stringArray2 ends before stringArray1 
	if(stringArray1[i] != NULL && stringArray2[i] == NULL)
		return 0;
	
	//they must be equal
	return 2;
}


/*
 * this function initializes every pointer in a string array
 * with null as its value to prevent conditional jumps or moves
 * based on uninitialized values, the size each char* inside the 
 * char** points to is determined by itemSize while the size of the
 * array itself comes from arraySize, it also terminates the char**
 * with a pointer to null
 */
char** str_array_initialize_with_null(unsigned int arraySize, unsigned int itemSize)
{
	
	//allocate space for each pointer + 1 for null terminator
	char** stringArray = (char**) malloc(sizeof(char*) * (arraySize + 1));
	
	//check if malloc succeeded
	if(stringArray == NULL)
		return NULL;
	
	for(int i = 0; i < arraySize; i++)
	{
		//allocate space for each char* inside the char**, + 1 for null terminator
		stringArray[i] = (char*) malloc(sizeof(char) * (itemSize + 1));
		
		//set each char in the char* to null terminator
		for(int j = 0; j < itemSize; j++)
			stringArray[i][j] = '\0';
	}
	stringArray[arraySize] = NULL;
	return stringArray;
}


/*
 * this function creates a copy of a null terminated string array,
 * returns the copy or null if source array is null
 *
 * side effect: must free returned pointer
 */
char** str_array_copy(char** stringArray)
{
	if(stringArray == NULL)
		return NULL;
	
	//allocate space + 1 for null terminator
	int srcSize = str_array_size(stringArray);
	char** result = (char**) malloc(sizeof(char*) * (srcSize + 1));
	
	for(int i = 0; stringArray[i] != NULL; i++)
	{
		int strSize = strlen(stringArray[i]);
		result[i] = malloc(sizeof(char) * (strSize + 1));
			
		//copy each character
		for(int j = 0; j < strSize; j++)
			result[i][j] = stringArray[i][j];
			
		//null terminate
		result[i][strSize] = '\0';
	}
	result[srcSize] = NULL;
	return result;
}	


/*
 * this function takes a character pointer and sanitizes 
 * so as to prevent command injection attacks
 *
 * side effects: this function modifies characters in place,
 * this means the memory handed over must be writable
 * 
 * newlines will become null terminators if detected
 */
void sanitize_dyn(char* givenString) 
{
	if(givenString == NULL)
		return;
	
    char* src = givenString;
    char* dest = givenString;
				
    while(*src) 
	{
        switch(*src) {
            case ';':
            case '&':
            case '|':
            case '-':
            case '`':
            case '$':
            case '(':
            case ')':
            case '{':
            case '}':
            case '\\':
            case '\'':
            case '\"':
            case '\r':
            case '\t':
            case ' ':
                *dest++ = ' ';
				break;
			case '\n':
				*dest++ = '\0';
				return;
            default:
                *dest++ = *src;
        }
        src++;
    }
    *dest = '\0';	
}


/*
 * sanitizes a whole array of strings
 * 
 * side effects: this function modifies characters in place,
 * this means the memory handed over must be writable
 * 
 * newlines will become null terminators if detected
 */
void sanitize_array_dyn(char** ptrArray)
{
	if(ptrArray == NULL)
		return;
	
	for(unsigned int i = 0; ptrArray[i] != NULL; i++)
	{
		if(ptrArray[i] != NULL)
			sanitize_dyn(ptrArray[i]);
	}
}


/*
 * this function takes two character pointers
 * and concatenates them into a dynamically allocated
 * character pointer
 *
 * side effect: returned pointer must be freed
 */
char* concat_dyn(char* givenString1, char* givenString2)
{
	if(givenString1 == NULL || givenString2 == NULL)
		return NULL;
		
	//allocate space and concat, +1 for null terminator
	char* result = malloc(sizeof(char) * (strlen(givenString1) + strlen(givenString2) + 1));
	strcpy(result, givenString1);
	strcat(result, givenString2);
	
	return result;
}


/*
 * prints a newline
 */
void newline() { printf("\n"); }
