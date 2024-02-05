/*
 * writes a config file with default
 * parameters, returns 1 on successful write
 * returns 0 on unsuccessful write
 */
unsigned int write_config_file()
{
	FILE *fptr = fopen("config.txt", "w");
	if(fptr == NULL)
		return 0;
	
	//write default values to config file
	fprintf(fptr, "#timeout is in seconds, it describes how much time clients have\n");
	fprintf(fptr, "#to complete the port knocking process\n");
	fprintf(fptr, "timeout=10\n\n");

	fprintf(fptr, "#interval is in microseconds, 1 second = 1,000,000 microseconds,\n");
	fprintf(fptr, "#it describes how often to check the logs, its worth noting logs\n");
	fprintf(fptr, "#aren't checked unless the size of the file changes (new entries)\n");
	fprintf(fptr, "interval=200000\n\n");
	
	fprintf(fptr, "#firewallResponse determines whether connection attempts to\n");
	fprintf(fptr, "#blocked ports are dropped or rejected. This should match the\n");
	fprintf(fptr, "#rules of your firewall so that bad actors can't discern whether\n");
	fprintf(fptr, "#there is a service running or not, options are \"REJECT\" or \"DROP\"\n");
	fprintf(fptr, "firewallResponse=REJECT\n\n");

	fprintf(fptr, "#this describes which ports to knock and in what order\n");
	fprintf(fptr, "portsToKnock=\n\n");

	fprintf(fptr, "#put ports to protect here\n");
	fprintf(fptr, "blacklistPorts=\n\n");

	fprintf(fptr, "#put possible log locations here\n");
	fprintf(fptr, "logLocations=/var/log/syslog\n");

	//close the file handle
	fclose(fptr);
	return 1;
}


/*
 * returns 1 if config file is valid
 * otherwise returns 0
 */
unsigned int check_config_file()
{
	//verify config file
	int configFile = verify_file("config.txt");
	
	//general failure of obtaining file handle for config.txt
	if(configFile == -1)
	{
		write_log("Failed to find or generate config.txt");
		return 0;
	}
	
	//successfully generated a blank config.txt
	if(configFile == 0)
	{
		//attempt to write to config file
		int writeStatus = write_config_file();
		if(writeStatus)
		{
			write_log("Successfully generated config.txt file");
			return 1;
		}
		else
		{
			write_log("Config.txt file located but cannot populate it with default settings");
			return 0;
		}
	}
	
	//file must exist
	return 1;
}


/*
 * this function checks to see if the necessary
 * dependencies are present for this program 
 * to function adequately
 *
 * if the user lacks:
 * -iptables and nftables
 * -syslog
 *
 * then logging and exiting with code 1 follows
 *
 * if the user lacks:
 * -config.txt or log.txt
 *
 * the function will attempt to rectify
 * the situation by making a generic config and or 
 * an empty log file before moving on, but might still
 * fail anyways, in which it will exit with code 1
 *
 * side effect: this is an exit point for the whole program
 */
void check_requirements()
{	
	//sort of a catch-22 situation here regarding logging where
	//you can't read/write the log file, so just exit with code 1 
	//until you are sure you can actually write logs
	
	//check if root
	if(geteuid() != 0)
		exit(1);
	
	//check for log file
	if(verify_file("log.txt") == -1)
		exit(1);
	
	//find out whats going on with whitelist
	int whitelistStatus = verify_file("whitelist.txt");
	
	//first check for total failure, then check if generation worked
	//if these two conditions are both untrue it means the file is fine
	//and already exists
	if(whitelistStatus == -1)
	{
		write_log("Failed to write or find whitelist.txt file");
		exit(1);
	}
	else if(whitelistStatus == 0)
	{
		//write localhost into whitelist
		//this is to allow users to connect to their own services
		append_to_file("whitelist.txt", "127.0.0.1");
		write_log("Successfully generated whitelist.txt file");
	}
	
	//check for config file
	if(check_config_file() == 0)
	{
		write_log("Failed to write or find config.txt file");
		exit(1);
	}
		
	//check for presence of iptables and nftables
	int iptables = system("which iptables >/dev/null 2>&1");	
	if(iptables != 0)
	{
		write_log("Verify that either iptables is installed");
		exit(1);
	}
}
