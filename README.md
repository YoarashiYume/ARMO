# armosystems\image sender
 
Input arguments:__
 
For  ClientServerApp class =  
	To run as a Server usage : ```<progname>``` ```<port>``` ```-s```  
    To run as a Client usage : ```<progname>``` ```<imagePath>``` ```<serverAddr>``` ```<port>``` ```-c```  

where 	'imagePath' is the path to the image being sent  
		'serverAddr' is the server address  
		'port' is the port in use  

		
For using class separately(Server and Client) input parameters will be:  	
For separate use:  
	Server class = usage : ```<progname>``` ```<port>```  
	Client class = usage : ```<progname>``` ```<imagePath>``` ```<serverAddr>``` ```<port>```  


> **Note**  
> main.cpp contains an implementation with **ClientServerApp** so the input parameters must be for it  
