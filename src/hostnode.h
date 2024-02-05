/*
 * this struct holds information about hosts and their
 * respective sequences, it is set up exactly like a linked list
 */
typedef struct HostNode
{
	struct HostNode* prev;
	Sequence* seq;
	struct HostNode* next;
} HostNode;


/*
 * appends a node at the end of a hostlist, this is done by 
 * following the chain starting from node until it reaches the end,
 * then nodeToAdd is appended to where the end used to be 
 */
void append_hostnode(HostNode* node, HostNode* nodeToAdd)
{
	if(node == NULL)
		return;
	
	HostNode* curr = node;
	
	while(curr->next != NULL)
		curr = curr->next;
	
	//link it back into the chain
	curr->next = nodeToAdd;
	nodeToAdd->prev = curr;
}


/*
 * constructs a hostnode 
 *
 * side effect: must free returned pointer
 */
HostNode* construct_hostnode(HostNode* prev, Sequence* seq, HostNode* next)
{
	HostNode* node = (HostNode*) malloc(sizeof(HostNode));
	
	if(node == NULL)
		return NULL;
	
	node->prev = prev;
	node->seq = seq;
	node->next = next;
	
	return node;
}


/*
 * destroys a hostnode while keeping the rest of the chain intact
 */
void free_hostnode(HostNode* node)
{
	if(node == NULL)
		return;
		
	//if there exists previous and next nodes
	if(node->prev != NULL && node->next != NULL)
	{
		//unlink from chain, link previous and next together
		node->prev->next = node->next;
		node->next->prev = node->prev;
	}
	
	//if there exists only a previous node
	if(node->prev != NULL && node->next == NULL)
		node->prev->next = NULL;
	
	node->next = NULL;
	node->prev = NULL;
	
	//free data then node itself
	if(node->seq != NULL)
		free_sequence(node->seq);
	
	free(node);
	node = NULL;
}


/*
 * checks to see if a sequence has overstayed its welcome,
 * returns 1 if timeout has expired, 0 if its still valid,
 * and -1 if a value handed to it is null
 */
unsigned int free_hostnode_if_expired(HostNode* node, Config* cfg)
{
	if(node == NULL || cfg == NULL)
		return -1;
	
	char* ctimeStr = get_current_time();
	char* ctime = substring(ctimeStr, 7, 8);
	
	int diff = time_diff_seconds(node->seq->timestamp, ctime);
	
	if(diff >= cfg->timeout)
	{
		write_log_two("Host timed out: ", node->seq->host);
		iptables_stop_logging_host("1", "65535", node->seq->host);
		free_hostnode(node);
		free(ctime);
		return 1;
	}
	free(ctime);
	return 0;
}


/*
 * returns the number of hosts inside a chain of hostnodes,
 * will return -1 if hostnode is null
 *
 * note: if you want the length of the entire chain you must
 * pass this function the very first node
 */
int count_hostlist(HostNode* node)
{
	if(node == NULL)
		return -1;
	
	HostNode* curr = node;
	
	int count = 0;
	
	while(curr->next != NULL)
	{
		curr = curr->next;
		count++;
	}
	return count;
}


/*
 * returns whether a host exists in the hostlist, returns 1
 * if host exists in list, 0 if not, and -1 if otherwise
 *
 * note: if you want the length of the entire chain you must
 * pass this function the very first node
 */
int does_hostnode_exist(HostNode* node, char* host)
{
	if(node == NULL || host == NULL)
		return -1;
	
	HostNode* curr = node;
	
	while(curr != NULL)
	{
		if(curr->seq != NULL)
		{
			if(strcmp(host, curr->seq->host) == 0)
				return 1;
		}
		curr = curr->next;
	}
	return 0;
}


/*
 * returns a pointer to the hostnode using host as an identifier
 * if hostnode is not present, returns null
 *
 * note: if you want the length of the entire chain you must
 * pass this function the very first node
 */
HostNode* get_hostnode(HostNode* node, char* host)
{
	if(node == NULL || host == NULL)
		return NULL;
	
	HostNode* curr = node;
	
	while(curr != NULL)
	{
		if(curr->seq != NULL)
		{
			if(strcmp(host, curr->seq->host) == 0)
				return curr;
		}
		curr = curr->next;
	}
	return NULL;
}


/*
 * prints a hostlist
 * 
 * note: if you want to print an entire chain you must
 * give this function the true head of the list, as it 
 * only prints the node its given and its successors
 */
int print_hostlist(HostNode* node)
{
	if(node == NULL)
		return -1;
	
	HostNode* curr = node;
				
	while(curr != NULL)
	{
		if(curr->seq != NULL)
			print_sequence(curr->seq);
		curr = curr->next;
	}
	return 0;
}


/*
 * destroys an entire list of hostnodes
 */
void free_hostlist(HostNode* node)
{
	if(node == NULL)
		return;
	
	HostNode* curr;
	
	while(node != NULL)
	{
		curr = node;
		node = node->next;
		free_hostnode(curr);
	}
}
