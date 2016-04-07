# SYN bomb implement
This is a small function for doing syn flooding.

#Changelog

2016/04/07 (version 1.0)
----------------
Progress:
 1) version 1.0 - done.
 2) This version doesn't support input args. so, please handle the target 
	IP and port number inside the client_bomb.c	
	
knowing issue:
 1) can't work like a command line program.
 2) the bomb will have a 90 seconds delay before removing firewall rules. 
	Hence, DO NOT use SIGINT or SIGTERM to kill the program. Please wait the
	program finished by itself.

