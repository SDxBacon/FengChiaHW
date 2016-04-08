SYN bomb implement
==================
This is a small function for doing syn flooding.  

Usage
-----
```shell
a@ubuntu:~/syn_bomb$ ./client_bomb -d 192.168.1.1 -p 80 -t 100
```
##### [options] #####

 -s, --sip <source ip>  

>  packet source IP address. If not set, program will use eth0's IP to attack.   
>  e.g. -s 192.168.1.1

-p, --dip <destination ip>  
>  destination IP address. If not set, program will shutdown.    
>  e.g. -p 80  

-d, --dport <destination port>  
>  destination port number. If not set, program will shutdown.  
>  e.g. -d 8.8.8.8  

-t, --time <attack times>  
>  how many packet you want to send. If not set, program will shutdown.  
>  e.g. -t 100  


Notice
------
This program will add a new rule into iptables when in order to drop RST packet from target destination IP when _**libsynbomb_init**_ or _**libsynbomb_create**_ was called.  
  
Hence,the function _**libsynbomb\_clean**_ will create a child process and sleep for 60 seconds then  
delete the rule which added by _**libsynbomb_init**_ or _**libsynbomb_create**_.



Changelog
---------

#### 2016/04/08 (version 1.1) ####
+  Progress:
	1.  This version support input args. please use it as a commandline program.
	2.  add iptable changes inside function _**libsynbomb_init**_ , _**libsynbomb_create**_ and _**libsynbomb\_clean**_ in order to drop the RST packet which kernel send it out automatically.  

+  knowing issue:
	1. iptables will be changed by the program. if you shutdown program before the rule been deleted. you may need to  
	   delete the rule by yourself.


#### 2016/04/07 (version 1.0) ####
+  Progress:
	1. version 1\.0 - done\.
	2. This version doesn't support input args. so, please handle the target  
	IP and port number inside the client_bomb.c  
+  knowing issue:
	1. can't work like a command line program.  
	2. the bomb will have a 90 seconds delay before removing firewall rules.   
	Hence, DO NOT use SIGINT or SIGTERM to kill the program. Please wait the
	program finished by itself.  

