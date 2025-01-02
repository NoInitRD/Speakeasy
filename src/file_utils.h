/*
 * attempts to write a file with a given name
 * returns 1 if write was successful otherwise
 * returns 0
 */
unsigned int write_file(char* fileName)
{
	FILE* fptr = fopen(fileName, "w");
	
	//if write failed, return 0
	if(fptr == NULL)
		return 0;
	
	//if write successful, close file and return 1
	fclose(fptr);
	return 1;
}


/*
 * attempts to append a line to a file
 * returns 1 on successful write, otherwise 0
 */
unsigned int append_to_file(char* fileName, char* givenString)
{
	if(givenString == NULL || fileName == NULL)
		return 0;
	
	//ensure file handle
	FILE* fptr = fopen(fileName, "a");
	
	if(fptr == NULL)
		return 0;
	
	//write and close file
	fprintf(fptr, "%s\n", givenString);
	fclose(fptr);
	
	return 1;
}


/*
 * returns a pointer to a file from the name
 * returns null if file DNE
 *
 * side effect: must close file pointer later
 */
FILE* read_file(const char fileName[])
{
	FILE *fptr;
	fptr = fopen(fileName, "r");
	return fptr;
}


/*
 * checks to see if a given file exists and then
 * follows 1 of 3 cases:
 *	-if file exists, return 1
 *	-if file did not exist but was generated successfully, return 0
 *	-if file did not exist and was not generated successfully, return -1
 */
int verify_file(char* fileName)
{
	//verify config file
	FILE* fptr = read_file(fileName);
	
	//if file cannot be read
	if(fptr == NULL)
	{
		//make it and try to get a handle again
		write_file(fileName);
		fptr = read_file(fileName);
		
		//if it fails again, return -1
		if(fptr == NULL)
			return -1;
		
		else
		{
			//close file, log, and return
			fclose(fptr);
			return 0;
		}
	}
	
	//file already exists
	fclose(fptr);
	return 1;
}


/*
 * returns the size of a file, if file cannot be found
 * this function returns -1
 */
int get_file_size(char* fileName)
{
	struct stat sb;
	
	if(stat(fileName, &sb) < 0)
		return -1;
	
	return sb.st_size;
}


/*
 * counts the number of lines in a file
 * returns -1 if fptr is null
 *
 * side effect: rewinds fptr
 */
unsigned long count_lines(FILE* fptr)
{
	if(fptr == NULL)
		return -1;
	
	char *line = NULL;
	size_t len = 0;
	long count = 0;
	
	while(getline(&line, &len, fptr) != -1 && len < LONG_MAX)
	{
		count++; 
		free(line);
		len = 0;
	}
	
	free(line);
	rewind(fptr);
	return count;
}


/*
 * returns line as char ptr where substring
 * is found, returns null if substring not found
 * or if the file pointer or substring is null
 * 
 * side effect: rewinds fptr, also must free ptr
 */
char* parse_file(FILE* fptr, char* substring)
{
	if(fptr == NULL || substring == NULL)
		return NULL;
	
	char* line = NULL;
	size_t len = 0;
	unsigned long lineNumber = 0;

	while(getline(&line, &len, fptr) != -1 && lineNumber < ULONG_MAX)
	{
		lineNumber++;
		if(strstr(line, substring) != NULL)
		{
			rewind(fptr);
			return line;
		}
		//free the memory associated with line
		free(line);
		line = NULL;
		len = 0;
	}
	free(line);
	rewind(fptr);
	return NULL;
}


/*
 * gets the line using line number
 * returns null if file pointer is null or
 * substring isn't found
 * 
 * side effect: rewinds fptr, also must free ptr
 */
char* get_line_from_index(FILE* fptr, unsigned long index)
{
	if(fptr == NULL)
		return NULL;
	
	char* line = NULL;
	size_t len = 0;
	unsigned long lineNumber = 0;
	ssize_t read;

	while((read = getline(&line, &len, fptr)) != -1 && lineNumber < ULONG_MAX)
	{
		lineNumber++;
		if(lineNumber == index)
		{
			rewind(fptr);
			return line;
		}
		
		//free the memory associated with line
		free(line);
		line = NULL;
		len = 0;
	}
	free(line);
	rewind(fptr);
	return NULL;
}


/*
 * returns line number of target substring
 * -1 for if substring not found
 * -2 if file pointer or substring are null
 *
 * side effect: rewinds fptr
 */
long get_line_num(FILE* fptr, char* substring)
{
	if(fptr == NULL || substring == NULL)
		return -2;
	
	char* line = NULL;
	size_t len = 0;
	long lineNumber = 0;

	while(getline(&line, &len, fptr) != -1 && lineNumber < LONG_MAX)
	{
		lineNumber++;
		if(strstr(line, substring) != NULL)
		{
			free(line);
			rewind(fptr);
			return lineNumber;
		}
		//free the memory associated with line
		free(line);
		line = NULL;
		len = 0;
	}
	free(line);
	rewind(fptr);
	return -1;
}


/*
 * returns line number of target substring
 * -1 for if substring not found
 * -2 if file pointer or substring are null
 * 
 * side effect: does not rewind the file pointer
 * unless the substring is not found up till EOF
 */
long get_line_num_no_rewind(FILE* fptr, char* substring)
{
	if(fptr == NULL || substring == NULL)
		return -2;
	
	char* line = NULL;
	size_t len = 0;
	long lineNumber = 0;

	while(getline(&line, &len, fptr) != -1 && lineNumber < LONG_MAX)
	{
		lineNumber++;
		if(strstr(line, substring) != NULL)
		{
			free(line);
			return lineNumber;
		}
		
		//free the memory associated with line
		free(line);
		line = NULL;
		len = 0;
	}
	free(line);
	return -1;
}


/*
 * returns line number of target substring
 * -1 for if substring not found
 * -2 if file pointer or substring are null
 * 
 * side effect: does not rewind the file pointer
 * instead if the substring cant be found it positions
 * the file pointer on the last line
 * unless the substring is not found up till EOF
 */
char* get_line_no_rewind(FILE* fptr, char* substring)
{
	if(fptr == NULL || substring == NULL)
		return NULL;
	
	char* line = NULL;
	size_t len = 0;
	long lineNumber = 0;

	while(getline(&line, &len, fptr) != -1 && lineNumber < LONG_MAX)
	{
		if(strstr(line, substring) != NULL)
			return line;
		
		//free the memory associated with line
		free(line);
		line = NULL;
		len = 0;
	}
	
	//go back a line if at end
	free(line);
	fseek(fptr, -1, SEEK_END);
	return NULL;
}


/*
 * parses the file for a line containing a parameter
 * and then accesses that parameters contents
 * ex. if some line of a file has "num=1" and you pass
 * this function "num=" as substring then it would return "1"
 * 
 * this function returns a character pointer
 * if the parameter passed is not in the file it or 
 * if either arguments are null it returns
 * a null pointer
 * this function is intended for use in configuration file
 * parsing
 *
 * side effect: must free pointer
 */
char* parse_for_parameter(FILE* fptr, char* substring)
{
	if(fptr == NULL || substring == NULL)
		return NULL;
	
	char* strBody = parse_file(fptr, substring);
	char* strParameter = get_parameter(strBody, substring);
	
	//check if results are null
	if(strBody == NULL || strParameter == NULL)
	{
		free(strBody);
		free(strParameter);
		return NULL;
	}
	
	//free file parse result and return parameter
	free(strBody);	
	return strParameter;
}


/*
 * parses the file line by line collecting each line
 * into a double character pointer, returns null if 
 * file pointer is null
 *
 * this function appends a null to the end of the pointers
 * as a sentinel value
 *
 * side effect: must free sub pointers and pointer
 */
char** lines_of_file_to_str_array(FILE* fptr)
{
	if(fptr == NULL)
		return NULL;
	
	//get number of lines in file
	unsigned int numLines = count_lines(fptr);

	//bounds checking
	if(numLines > INT_MAX)
		return NULL;
	
	unsigned int fileSize = sizeof(char*) * (numLines + 1);
	char** result = (char**) malloc(fileSize);
	
	char* line = NULL;
	size_t len = 0;
	unsigned int arryIndex = 0;
	
	while(getline(&line, &len, fptr) != -1)
	{
		result[arryIndex] = line;
		
		//increment index and reset line length
		len = 0;
		arryIndex++;
	}
	//free the last line
	free(line);
	
	//null terminate and return
	result[arryIndex] = NULL;
	return result;
}


/*
 * prints out a file note that
 * this function does not rewind the pointer
 */
void print_file(FILE* fptr)
{
	if(fptr == NULL)
		return;
	
	char* line = NULL;
	size_t len = 0;
	unsigned long lineNumber = 0;

	while(getline(&line, &len, fptr) != -1 && lineNumber < ULONG_MAX)
	{
		printf("%s", line);
		
		//free the memory associated with line
		free(line);
		line = NULL;
		len = 0;
	}
	free(line);
}


/* 
 * places the file pointer at the last line of a file
 * returns 1 if it is successful, otherwise 0
 */ 
unsigned int skip_to_last_line(FILE* fptr)
{
	if(fptr == NULL)
		return 0;
	
	//attempt to skip to end
	fseek(fptr, -1, SEEK_END);
	if(ferror(fptr))
		return 0;
	
	return 1;
}


/*
 * removes a line from a file by line number
 * returns 1 if successful, otherwise 0
 * 
 * side effect: rewinds file pointer
 */
unsigned int remove_line_from_file(FILE* fptr, unsigned int lineNum)
{
    if (fptr == NULL)
        return 0;

    //get the number of lines in the file minus one for indexing
    unsigned int numLines = count_lines(fptr);

    //bounds checking
    if (lineNum > numLines)
        return 0;

    //create temp file
    FILE* temp = tmpfile();
    if (!temp) {
        //failed to create temporary file
        return 0;
    }

    //rewind the file pointer to the beginning
    rewind(fptr);

    //copy all lines except the one to be removed
    char* line = NULL;
    size_t len = 0;
    unsigned int lineNumber = 0;

    while (getline(&line, &len, fptr) != -1)
    {
        lineNumber++;
        if (lineNumber != lineNum)
            fprintf(temp, "%s", line);

        //free the memory associated with line
        free(line);
        line = NULL;
        len = 0;
    }

    //free the line buffer one last time
    free(line);

    // Reopen the original file in write mode to overwrite it (this clears the file)
    freopen(NULL, "w", fptr);  // Reopens fptr as write-only (truncates the file)

    //rewind the temp file
    rewind(temp);

    //copy the temp file back to the original file
    char c;
    while ((c = fgetc(temp)) != EOF)
        fputc(c, fptr);

    //close the temp file
    fclose(temp);
    return 1;
}