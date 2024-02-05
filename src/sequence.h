/*
 * this struct houses information about a hosts
 * knocking progress, including when it started
 */
typedef struct
{
	char timestamp[10];
	char host[16];
	char** portsKnocked;
} Sequence;


/*
 * this function prints out a sequence
 */
void print_sequence(Sequence* seq)
{
	if(seq == NULL)
		return;
	
	printf("timestamp: %s\n", seq->timestamp);
	printf("host: %s\n", seq->host);
	printf("portsKnocked:");
	print_str_array(seq->portsKnocked);
	printf("\n\n");
}


/*
 * constructs a sequence
 *
 * side effect: must return freed pointer
 */
Sequence* construct_sequence(char* host, char** portsKnocked)
{
	if(host == NULL || portsKnocked == NULL)
		return NULL;
		
	char* ctimeStr = get_current_time();
	char* ctime = substring(ctimeStr, 7, 8);
	
	Sequence* seq = (Sequence*) malloc(sizeof(Sequence));
	
	if(seq == NULL)
		return NULL;
		
	//instantiate members
	strncpy(seq->timestamp, ctime, 8);
	strncpy(seq->host, host, 15);
	seq->portsKnocked = portsKnocked;
	
	//null terminate strings
	seq->timestamp[8] = '\0';
	seq->host[15] = '\0';
		
	//free time str
	free(ctime);
	return seq;
}


/*
 * destroys a sequence struct
 */
void free_sequence(Sequence* seq)
{
	if(seq == NULL)
		return;
	
	if(seq->portsKnocked != NULL)
	{
		char** ptrArray[] = {seq->portsKnocked};
		free_all_double_char(ptrArray, 1);
	}
	
	free(seq);
	seq = NULL;
}