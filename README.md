
# Introduction

ICE is a port knocking utility that dynamically adjusts firewall rules using iptables.
It was designed with am emphasis on speed, memory safety, and ease of use.
To use ICE all you have to do is enter information into the config.txt file.

# Information

Because ICE uses iptables to adjust rules, it requires root permissions
to operate. There are three files that ICE uses to function:

	- config.txt is shown above and it is where the port knocking sequence
	and other rules are created. There are comments to assist users in understanding
	what various things do.
 
	- whitelist.txt is a file that initializes with "127.0.0.1" only to allow machines to connect
	to themselves. To add a host without having them knock simply append their IP address to the list.
 
	- log.txt is where ICE logs relevant information. It will tell you if you're configuration is functioning
	as well as when a host has started knocking, failed the sequence, or has successfully authenticated. 

These files will be automatically generated when running ICE for the first time.

If for whatever reason these files are deleted they can be restored with default values by simply running ICE again. This feature can
also be used to regenerate the default configuration settings. 

It is important to note that you should restrict read/write access to these files to prevent malicious behavior.

At this point in time this program DOES NOT automatically start up upon a reboot. To enable this I suggest using
cron. You can create cronjobs by running the command "sudo crontab -e" and appending the job to the bottom of 
the file. Run "man cron" for more information. 

If you change the whitelist manually be sure to restart ICE to allow it to adjust rules accordingly.

> [!CAUTION]
> If this program is used incorrectly it is possible to become remotely disbarred from your machine.

# Configuration

## Here is the default configuration:
```
#timeout is in seconds, it describes how much time clients have#to complete the port knocking process
timeout=10

#interval is in microseconds, 1 second = 1,000,000 microseconds,
#it describes how often to check the logs, its worth noting logs
#aren't checked unless the size of the file changes (new entries)
interval=200000

#firewallResponse determines whether connection attempts to
#blocked ports are dropped or rejected. This should match the
#rules of your firewall so that bad actors can't discern whether
#there is a service running or not, options are "REJECT" or "DROP"
firewallResponse=REJECT

#this describes which ports to knock and in what order
portsToKnock=

#put ports to protect here
blacklistPorts=

#put possible log locations here
logLocations=/var/log/syslog
```

## Here is an example of custom configuration:

```
#timeout is in seconds, it describes how much time clients have
#to complete the port knocking process
timeout=20

#interval is in microseconds, 1 second = 1,000,000 microseconds,
#it describes how often to check the logs, its worth noting logs
#aren't checked unless the size of the file changes (new entries)
interval=1000000

#firewallResponse determines whether connection attempts to
#blocked ports are dropped or rejected. This should match the
#rules of your firewall so that bad actors can't discern whether
#there is a service running or not, options are "REJECT" or "DROP"
firewallResponse=DROP

#this describes which ports to knock and in what order
portsToKnock=123,124,1111,63000

#put ports to protect here
blacklistPorts=22,23,25

#put possible log locations here
logLocations=/root/log.log,/var/log/syslog
```
