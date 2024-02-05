/*
 * this struct houses all of the information located
 * inside of the config.txt file, as well as 2 file handles:
 * 	-one to log.txt to record useful information
 * 	-another to whitelist.txt to set up whitelisted hosts using firewall.h
 */
typedef struct
{
	unsigned int timeout;
	unsigned int interval;
	char firewallResponse[8];
	char** portsToKnock;
	char** blacklistPorts;
	char** logLocations;
	char* logPath;
	FILE* logFile;
	FILE* whitelistFile;
} Config;


/*
 * checks numerous locations for logs
 * sets logPath and logFile according to logLocations
 * in a Config struct
 * otherwise program logs and stops
 */
void choose_log_file(Config* cfg)
{
	if(cfg->logLocations == NULL)
	{
		write_log("Configuration failed due to logLocations parameter, try deleting config.txt to restore default");
		return;
	}

	unsigned int size = str_array_size(cfg->logLocations);
	unsigned int index = 0;
	
	while(cfg->logLocations[index] != NULL)
	{			
		//case where theres only one possible location
		if(index == (size - 1))
		{
			//truncate so read_file works correctly
			size_t size = strlen(cfg->logLocations[index]);
			cfg->logLocations[index][size - 1] = '\0';
		}

		FILE* file = read_file(cfg->logLocations[index]); 
		if(file != NULL)
		{	
			//write the message to log
			write_log_two("Log file located at: ", cfg->logLocations[index]);
			
			//free concatenated string and set log pointer
			cfg->logFile = file;
			cfg->logPath = cfg->logLocations[index];
			return;
		}
		index++;
	}
	write_log("No valid log files located, are you sure you set logLocations correctly in config?");
}


/*
 * destroys a config struct
 * by freeing dynamically allocated struct
 * members before freeing the whole pointer
 */
void free_config(Config* cfg)
{
	if(cfg == NULL)
		return;
	
	//logPath gets cleaned up because it points to one of the 
	//char* inside logLocations, and logLocations is cleaned up here
	char** ptrs[] = {cfg->portsToKnock, cfg->blacklistPorts,
					 cfg->logLocations};

	free_all_double_char(ptrs, 3);
	
	//free log handle
	if(cfg->logFile != NULL)
		fclose(cfg->logFile);
	
	//free whitelist handle
	if(cfg->whitelistFile != NULL)
		fclose(cfg->whitelistFile);
	
	//lastly, free struct itself
	free(cfg);
	cfg = NULL;
}


/*
 * initializes file handles for log.txt and whitelist.txt
 * and points the Config structs respective members at them
 * returns 1 if successful and 0 otherwise
 */
int initialize_config_files(Config* cfg)
{
	//set logFile and whitelistFile to null so its possible to detect
	//if they haven't been initialized when struct is freed
	cfg->logFile = NULL;
	cfg->whitelistFile = NULL;
	
	//obtain log handle
	choose_log_file(cfg);
	if(cfg->logFile == NULL)
		return 0;
	
	//obtain whitelistFile handle
	FILE* whitelistFile = read_file("whitelist.txt");
	if(whitelistFile == NULL)
		return 0;
		
	cfg->whitelistFile = whitelistFile;
	
	return 1;
}


/*
 * constructs a config struct using the 
 * config file otherwise returns null
 *
 * side effect: must free returned pointer
 */
Config* construct_config(FILE* fptr)
{
	if(fptr == NULL)
	{
		write_log("Failed to obtain handle on config file: exiting");
		return NULL;
	}
	
	//extract parameters
	char* timeoutStr = parse_for_parameter(fptr, "timeout=");
	char* intervalStr = parse_for_parameter(fptr, "interval=");
	char* firewallResponse = parse_for_parameter(fptr, "firewallResponse=");
	char* portsToKnock = parse_for_parameter(fptr, "portsToKnock=");
	char* blacklistPorts = parse_for_parameter(fptr, "blacklistPorts=");
	char* logLocations = parse_for_parameter(fptr, "logLocations=");
	
	//check to make sure all allocations succeeded
	void* ptrs[] = {timeoutStr, intervalStr,
					firewallResponse, blacklistPorts,
					portsToKnock, logLocations};
	
	if(check_ptr_integrity(ptrs, 6) == 0)
		return NULL;
	
	//allocate memory
	Config* cfg = (Config*) malloc(sizeof(Config));
	
	//check if malloc succeeded
	if(cfg == NULL)
	{
		fclose(fptr);
		return NULL;
	}
	
	//instantiate static members of struct
	cfg->timeout = atoi(timeoutStr);
	cfg->interval = atoi(intervalStr);
		
	//copy and null terminate firewallResponse:
	//this is because firewallResponse possibly contains a newline
	//which is not a concern for the other strings due to
	//text_series_to_str_array's behavior
	firewallResponse[strcspn(firewallResponse, "\n")] = '\0';
	strncpy(cfg->firewallResponse, firewallResponse, 8);
	
	//convert list strings from config into arrays
	cfg->portsToKnock = text_series_to_str_array(portsToKnock);
	cfg->blacklistPorts = text_series_to_str_array(blacklistPorts);
	cfg->logLocations = text_series_to_str_array(logLocations);
	
	//sanitize items
	sanitize_array_dyn(cfg->portsToKnock);
	sanitize_array_dyn(cfg->blacklistPorts);
	sanitize_dyn(cfg->firewallResponse);
	
	//check to make sure results of str series to double charptr is valid
	void* dCharPtrs[] = {cfg->portsToKnock, cfg->blacklistPorts,
						 cfg->logLocations};
	
	if(check_ptr_integrity(dCharPtrs, 3) == 0)
		return NULL;
	
	//free pointers that are no longer required
	free_all(ptrs, 6);
	
	if(initialize_config_files(cfg) == 0)
	{
		free_config(cfg);
		return NULL;
	}
	
	return cfg;
}


/*
 * this function takes a pointer to a Config struct
 * and prints out the contents of the struct, does nothing
 * if null pointer
 */
void print_config(Config* cfg)
{
	if(cfg == NULL)
		return;
	
	//print static members
	printf("timeout: %d\ninterval: %d\n", cfg->timeout, cfg->interval);
	printf("firewallResponse: %s\n", cfg->firewallResponse);
	
	//print double character pointers
	printf("portsToKnock: ");
	print_str_array(cfg->portsToKnock);
	newline();
	
	printf("blacklistPorts: ");
	print_str_array(cfg->blacklistPorts);
	newline();
	
	printf("logLocations: ");
	print_str_array(cfg->logLocations);

	//print newline
	printf("\n");
}
