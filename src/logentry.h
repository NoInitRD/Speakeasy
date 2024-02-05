/*
 * this struct houses relevant information from log entries which is
 *	source IP address
 *	destination IP address
 * 	destination port
 */
typedef struct
{
	//added one to account for null terminators
	char src[16];
	char dst[16];
	char dpt[6];
} LogEntry;


/*
 * this function destroys a log
 */
void free_log(LogEntry* log)
{
	if(log == NULL)
		return;
	
	free(log);
}
	

/*
 * constructs a log entry from a given string
 * returns a pointer to a LogEntry struct
 * or null if the given string is null
 * or doesn't contain a valid log entry
 * 
 * side effect: must be freed later
 */
LogEntry* construct_log(char* givenString)
{
	if(givenString == NULL)
		return NULL;
		
	//extraction of items from log entry
	char* src = get_parameter(givenString, "SRC=");
	char* dst = get_parameter(givenString, "DST=");
	char* dpt = get_parameter(givenString, "DPT=");
	
	//make an array of the pointers
	void* ptrs[] = {src, dst, dpt};
	
	//verify pointer integrity before proceeding
	if(check_ptr_integrity(ptrs, 3) == 0)
		return NULL;
	
	//proceed since pointers are valid
	LogEntry* log = (LogEntry*) malloc(sizeof(LogEntry));
		
	//check if malloc succeeded
	if(log == NULL)
		return NULL;
	
	//sanitize inputs
	sanitize_dyn(src);
	sanitize_dyn(dst);
	sanitize_dyn(dpt);
	
	//copy them over
	strncpy(log->src, src, 15);
	strncpy(log->dst, dst, 15);
	strncpy(log->dpt, dpt, 5);
	
	//null terminate
	log->src[15] = '\0';
	log->dst[15] = '\0';
	log->dpt[5] = '\0';
	
	//free and return
	free_all(ptrs, 3);
	return log;
}


/*
 * this function takes a pointer to a LogEntry struct
 * and prints out the contents of the struct, does nothing
 * if null pointer
 */
void print_log(LogEntry* log)
{
	if(log == NULL)
		return;
	
	//print constituents
	printf("src: %s\n", log->src);

	printf("dst: %s\n", log->dst);
	
	printf("dpt: %s\n\n", log->dpt);
}
