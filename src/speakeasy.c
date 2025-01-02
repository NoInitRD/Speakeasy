#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/stat.h>
#include <time.h>
#include <limits.h>


#include "general_utils.h"
#include "file_utils.h"
#include "logging.h"
#include "config.h"
#include "logentry.h"
#include "requirements.h"
#include "firewall.h"
#include "sequence.h"
#include "hostnode.h"


//global pointers to allow for graceful cleanup
//in case of shutdown occurring mid-process
Config* _main_cfg = NULL;
LogEntry* _main_log = NULL;
HostNode* _main_head_node = NULL;


/*
 * handles signals sent from the OS
 * and performs cleanup using global pointers
 */
void signal_handler(int sig)
{		
	//handle interrupt and term
	if(sig == SIGINT || sig == SIGTERM)
	{
		//cleanup
		if(_main_cfg != NULL)
			free_config(_main_cfg);
		if(_main_log != NULL)
			free_log(_main_log);
		
		//account for if there are other nodes in chain
		if(_main_head_node != NULL && _main_head_node->next != NULL)
		{
			free_hostlist(_main_head_node->next);
			free_hostnode(_main_head_node);
		}
		else if(_main_head_node != NULL)
			free_hostnode(_main_head_node);
		
		exit(0);
	}
}


/*
 * this function is called in response to activity from
 * a host that is already being monitored and decides whether
 * they are to be removed from the chain, whitelisted, or left alone
 * until they complete the sequence
 */
void update_host_status(char* host, char* port)
{
	if(host == NULL || port == NULL)
		return;
	
	//obtain node using host string
	HostNode* node = get_hostnode(_main_head_node, host);
	if(node == NULL)
		return;

	//get to next vacant spot in hosts sequence
	unsigned int i = 0;
	while(node->seq->portsKnocked[i] != NULL && node->seq->portsKnocked[i][0] != '\0')
		i++;
	
	//copy port to host sequence and check if they failed the config sequence
	strcpy(node->seq->portsKnocked[i], port);
	int knockingStatus = str_array_subarray(node->seq->portsKnocked, _main_cfg->portsToKnock);
	
	if(knockingStatus == 0)
	{
		iptables_stop_logging_host("1", "65535", host);
		free_hostnode(node);
		write_log_two("Host failed port knocking sequence: ", host);
		return;
	}
	
	//haven't failed the sequence yet so do nothing
	if(knockingStatus == 1)
		return;
	
	//they've completed the sequence
	if(knockingStatus == 2)
	{
		//handle hosts already whitelisted
		FILE* whitelist = read_file("whitelist.txt");
		if(whitelist != NULL)
		{
			//they've already been whitelisted
			long lineNum = get_line_num(whitelist, host);
			if(lineNum != -1 && lineNum != -2)
			{
				//stop logging host and remove from whitelist
				iptables_stop_logging_host("1", "65535", host);

				unsigned int status = remove_line_from_file(whitelist, lineNum);
				if(status)
					write_log_two("Removed previously authenticated host from whitelist: ", host);
				else
					write_log_two("Failed to remove previously authenticated host from whitelist: ", host);

				free_hostnode(node);
				fclose(whitelist);
				return;
			}
			fclose(whitelist);
		}

		//otherwise whitelist them
		iptables_stop_logging_host("1", "65535", host);
		iptables_whitelist_host(host);
		append_to_file("whitelist.txt", host);
		free_hostnode(node);
		write_log_two("Host completed sequence, authentication complete: ", host);
	}
}


/*
 * this function is called in response to a host knocking the 
 * first port in the port-knocking sequence specified by config
 * and manages monitoring various potential clients, it does this 
 * by dynamically manipulating firewall logging rules and searching 
 * through a linked list of hosts to assess progress in the port 
 * knocking process
 */
void start_monitoring_host(char* host)
{
	if(host == NULL)
		return;
	
	//check if they're already whitelisted
	FILE* whitelist = read_file("whitelist.txt");
	if(whitelist != NULL)
	{
		if(get_line_num(whitelist, "host") != -1)
			return;
		fclose(whitelist);
	}
	
	//log host spotted
	write_log_two("Host hit first port in port knocking sequence: ", host);
	
	//find length of portsToKnock in config
	int lengthOfPortsToKnock = str_array_size(_main_cfg->portsToKnock);
	
	//make new char** with enough space to store the whole sequence
	char** portsKnocked = str_array_initialize_with_null(lengthOfPortsToKnock, 5);
	
	//set first port in the ports they've knocked equal to first 
	//port in port knocking sequence, because they are only given
	//to this function if they've successfully knocked the first port
	strcpy(portsKnocked[0], _main_cfg->portsToKnock[0]);
	
	//construct sequence
	Sequence* seq = construct_sequence(host, portsKnocked);
	
	//construct hostnode and link it into the chain
	HostNode* nodeToAdd = construct_hostnode(NULL, seq, NULL);
	append_hostnode(_main_head_node, nodeToAdd);
	
	//start logging connections from this host
	iptables_log_ports("1", "65535", host);
}


/*
 * this function checks if a host has timed out of the knocking process,
 * if they have they are removed from the host chain
 */
void check_hostlist_for_timeouts()
{
	if(_main_head_node == NULL || _main_cfg == NULL)
		return;
	
	HostNode* curr = _main_head_node;
	HostNode* currNext;
	
	while(curr != NULL && curr->next != NULL)
	{
		//save the next node
		currNext = curr->next;
		if(currNext->seq != NULL && free_hostnode_if_expired(currNext, _main_cfg))
		{
			//if the node was freed, next now points to freed memory
			//the next iteration will continue from the current node
			continue;
		}
		curr = currNext;
	}
}


/*
 * provided with a config file pointer this function starts
 * the logic that runs the whole program in a loop:
 *  -verifies that syslog has changed
 *  -constructs logs from syslog
 *  -checks order of attempted connections
 *  -adjusts firewall accordingly
 */
void parse_log_for_entries(Config* cfg)
{	
	//find tagged lines in syslog and construct a LogEntry
	char* logString = "";
	
	while(logString != NULL)
	{			
		logString = get_line_no_rewind(cfg->logFile, "Speakeasy-log");
		LogEntry* currLog = construct_log(logString);
		
		//set global pointer _main_log to currLog
		_main_log = currLog;
		
		//free the string parsing result
		free(logString);
	
		if(_main_log != NULL)
		{			
			//check if new host to be added or if they are already in the chain
			if(strcmp(_main_log->dpt, cfg->portsToKnock[0]) == 0)
			{
				//are they new
				if(does_hostnode_exist(_main_head_node, _main_log->src) == 0)
					start_monitoring_host(_main_log->src);
			}
			else
			{
				//are they already in the chain
				if(does_hostnode_exist(_main_head_node, _main_log->src) == 1)
					update_host_status(_main_log->src, _main_log->dpt);
			}
						
			//free last parsed log
			free_log(currLog);
			currLog = NULL;
		}
	}
}


/*
 * this function contains the main logic loop
 * that runs the port knocking server
 */
void initialize_port_knocking()
{
	//set up config handle
	FILE* cfgFptr = read_file("config.txt");
	Config* cfg = construct_config(cfgFptr);
	fclose(cfgFptr);
	if(cfg == NULL)
	{
		write_log("Check config.txt for issues, delete config.txt to regenerate");
		return;
	}
	
	//set global pointers _main_cfg to cfg
	//and _main_head_node to a sentinel node
	_main_cfg = cfg;
	HostNode* head = construct_hostnode(NULL, NULL, NULL);
	_main_head_node = head;
	
	//set up firewall with cfg and whitelist, then close whitelist
	setup_firewall(cfg);
	
	//skip to last line in file
	if(skip_to_last_line(cfg->logFile) == 0)
	{
		write_log("Check specified log file for irregularities, failed to skip to end");
		signal_handler(SIGTERM);
	}
	
	//save file size and loop
	unsigned int fSize = 0;
	while(1)
	{	
		//no point in searching if file hasn't changed
		if(fSize != get_file_size(cfg->logPath))
		{
			parse_log_for_entries(cfg);
			fSize = get_file_size(cfg->logPath);
		}
		
		//if there are active hosts knocking check them for timeout
		if(count_hostlist(_main_head_node) > 0)
			check_hostlist_for_timeouts();
		
		usleep(cfg->interval);
	}
}


int main(int argc, char *argv[])
{	
	//register handle interrupt and terminate signals
	signal(SIGINT, signal_handler);
	signal(SIGTERM, signal_handler);
	
	//verifies privileges and dependencies
	check_requirements();	
	
	//start server
	initialize_port_knocking();
	
	return 0;
}
