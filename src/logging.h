/*
 * this function writes input to "log.txt"
 * if "log.txt" does not exist then it creates it
 * returns 1 on successful write
 * returns 0 on unsuccessful write
 */
unsigned int write_log(char* message)
{
	//check for null message
	if(message == NULL)
		return 0;
	
	//ensure file handle
	FILE* log = fopen("log.txt", "a");
	
	if(log == NULL)
		return 0;
	
	char* currentTime = get_current_time();
	
	//output message into terminal
	printf("[%s] Speakeasy-LOG: %s\n", currentTime, message);
	
	//if file handle, write and close
	fprintf(log, "[%s] Speakeasy-LOG: %s\n", currentTime, message);
	fclose(log);
	
	return 1;
}


/*
 * this function writes two inputs to "log.txt"
 * if "log.txt" does not exist then it creates it
 * returns 1 on successful write
 * returns 0 on unsuccessful write
 */
unsigned int write_log_two(char* message1, char* message2)
{
	char* message = concat_dyn(message1, message2);
	unsigned int status = write_log(message);
	free(message);
	return status;
}