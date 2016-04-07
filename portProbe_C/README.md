#Simple TCP port probe program
------------------------------


#Introduction    
------------------------------
This is a small TCP port prob project of Network Technology course which is hosting by Prof. Chi-Shih Chao at Feng Chia University, Taiwan.



#Requirement    
------------------------------
1) libcprops. You can download it at http://cprops.sourceforge.net/
2) this project is build on Ubuntu 15.04 (Linux kernel version 3.19) and does NOT support on windows.



#Usage      
------------------------------
portProbe [-i <Target IP address>] [-p <target port number>]

e.g. portProbe -i '8.8.8.8' -p 80~90,145~143