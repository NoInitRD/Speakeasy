/* 
 * this functions attempts to wipes iptables rules
 * returns 1 if successful, otherwise 0
 */
unsigned int reset_iptables() 
{
	int result;
    result = system("iptables -P INPUT ACCEPT");
    result = system("iptables -P FORWARD ACCEPT");
    result = system("iptables -P OUTPUT ACCEPT");
    result = system("iptables -t nat -F");
    result = system("iptables -t mangle -F");
    result = system("iptables -F");
    result = system("iptables -X");
	
	//failed to clear
	if(result == -1 || WEXITSTATUS(result) != 0)
	{
		write_log("Failed to white iptables rules");
		return 0;
	}
	
	//successful clear
	return 1;
}


/*
 * this function checks to see if a given iptables
 * rule already exists, returns 1 if it exists
 * or 0 if the cmd fails or the rule doesn't exist
 */
unsigned int iptables_check_rule(char* rule, char* port)
{
	FILE* fptr = popen("iptables -L -n", "r");
	
	if(fptr == NULL)
		return 0;
	
	//search and close file
	char line[512];
	
	while(fgets(line, sizeof(line), fptr) != NULL)
	{
		if(strstr(line, rule) != NULL && strstr(line, port) != NULL)
		{
			pclose(fptr);
			return 1;
		}
	}
	pclose(fptr);
	return 0;
}


/*
 * this function drops or rejects a port using iptables
 * depending on firewallResponse's value
 * returns a 1 if successful, otherwise 0
 */
unsigned int iptables_block_port(char* port, char* firewallResponse)
{
	if(port == NULL)
		return 0;
	
	//max size port ~65000 so 5 bytes long
	if(strlen(port) > 5 || strcmp(port, "") == 0)
		return 0;
	
	//check if the rule already exists
	if(iptables_check_rule(firewallResponse, port))
		return 0;
	
	//max size of command is 66 bytes
	char cmd[66];
	sprintf(cmd, "iptables -A INPUT -p tcp --dport %s -j %s > /dev/null 2>&1",
			port, firewallResponse);
	
	//run command
	int result = system(cmd);
	
	if(result == -1 || WEXITSTATUS(result) != 0)
	{
		write_log("Failed to run iptables block command");
		return 0;
	}
	return 1;
}


/*
 * this function whitelists a host using iptables
 * returns a 1 if successful, otherwise 0
 */
unsigned int iptables_whitelist_host(char* host)
{
	if(host == NULL)
		return 0;
	
	//max size IP is 15 bytes long
	if(strlen(host) > 15)
		return 0;
	
	//check if the rule already exists
	if(iptables_check_rule("ACCEPT", host))
		return 0;
	
	//max size of command is 66 bytes
	char cmd[66];
	sprintf(cmd, "iptables -I INPUT 2 -s %s -j ACCEPT > /dev/null 2>&1", host);
	
	//run command
	int result = system(cmd);
	
	if(result == -1 || WEXITSTATUS(result) != 0)
	{
		//try again without inserting at position 2
		//this happens if no ports were blacklisted 
		char cmdRetry[64];
		sprintf(cmdRetry, "iptables -I INPUT -s %s -j ACCEPT > /dev/null 2>&1", host);
		result = system(cmdRetry);
		
		//check again
		if(result == -1 || WEXITSTATUS(result) != 0)
		{
			write_log("Failed to run iptables whitelist command");
			return 0;
		}
	}
	return 1;
}


/*
 * this function sets up logging of ports in a specified
 * range with a specified host, to log one port without
 * filtering by a host simply pass the same port twice and 
 * pass the host as an empty string ex.
 * log_ports("22", "22", "")
 */
void iptables_log_ports(char* startPort, char* endPort, char* host)
{
	if(startPort == NULL || endPort == NULL || host == NULL)
		return;
	
	//length check
	if(strlen(startPort) > 5 || strlen(endPort) > 5 || strlen(host) > 15)
		return;
	
	//track whether commands succeeded or not
	int cmdResult;
	
	//make logging chain if it doesn't exist
	if(!iptables_check_rule("LOGGING", "tcp"))
		cmdResult = system("iptables -N LOGGING > /dev/null 2>&1");
	
	//if no host specified, generally log the port/ports
	if(strlen(host) == 0)
	{
		//run both commands excluding host
		char cmd1[74];
		sprintf(cmd1, "iptables -I INPUT -p tcp --dport %s:%s -j LOGGING > /dev/null 2>&1",
				startPort, endPort);
		
		char cmd2[114];
		sprintf(cmd2, "iptables -I LOGGING -p tcp --dport %s:%s -j LOG --log-prefix \"[ICE-log]: \" --log-level 4 > /dev/null 2>&1",
				startPort, endPort);
		
		cmdResult = system(cmd1);
		cmdResult = system(cmd2);
	}
	else
	{
		//run both commands including host
		char cmd1[92];
		sprintf(cmd1, "iptables -I INPUT -p tcp -s %s --dport %s:%s -j LOGGING > /dev/null 2>&1",
				host, startPort, endPort);
		
		char cmd2[130];
		sprintf(cmd2, "iptables -I LOGGING -p tcp -s %s --dport %s:%s -j LOG --log-prefix \"[ICE-log]: \" --log-level 4 > /dev/null 2>&1",
				host, startPort, endPort);

		cmdResult = system(cmd1);
		cmdResult = system(cmd2);
	}
	
	if(cmdResult == -1 || WEXITSTATUS(cmdResult) != 0)
		write_log("Failed to run iptables logging command");
}


/*
 * this function stops logging a host by deleting them from 
 * the logging rules
 */
void iptables_stop_logging_host(char* startPort, char* endPort, char* host)
{
	if(startPort == NULL || endPort == NULL || host == NULL)
		return;
	
	//length check
	if(strlen(startPort) > 5 || strlen(endPort) > 5 || strlen(host) > 15)
		return;
	
	char cmd1[92];
	sprintf(cmd1, "iptables -D INPUT -p tcp -s %s --dport %s:%s -j LOGGING > /dev/null 2>&1",
			host, startPort, endPort);
	
	char cmd2[130];
	sprintf(cmd2, "iptables -D LOGGING -p tcp -s %s --dport %s:%s -j LOG --log-prefix \"[ICE-log]: \" --log-level 4 > /dev/null 2>&1",
			host, startPort, endPort);

	int cmdResult;
	cmdResult = system(cmd1);
	cmdResult = system(cmd2);
	
	if(cmdResult == -1 || WEXITSTATUS(cmdResult) != 0)
		write_log("Failed to run iptables stop logging command");
}


/*
 * sets up basic iptables rules using a config struct
 * and a file pointer to whitelist.txt
 * i.e. whitelisting hosts and blacklisting ports
 */
void setup_firewall(Config* cfg)
{	
	if(cfg == NULL)
		return;
	
	//clear iptables rules
	reset_iptables();
	
	//start logging the first port in port knocking sequence
	if(strlen(cfg->portsToKnock[0]) != 0)
		iptables_log_ports(cfg->portsToKnock[0], cfg->portsToKnock[0], "");
	
	//whitelist hosts then free
	char** hostArry = lines_of_file_to_str_array(cfg->whitelistFile);
	for(unsigned int i = 0; hostArry[i] != NULL; i++)
	{
		sanitize_dyn(hostArry[i]);
		iptables_whitelist_host(hostArry[i]);
	}
	
	free_all_double_char((char**[]) {hostArry}, 1);
	
	//blacklist ports
	for(unsigned int i = 0; cfg->blacklistPorts[i] != NULL; i++)
	{
		//check for EOL
		if(cfg->blacklistPorts[i][0] == '\n')
			return;
		
		//block ports specified in config
		iptables_block_port(cfg->blacklistPorts[i], cfg->firewallResponse);
	}
}