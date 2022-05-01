# HTTP SERVER
#### _ MANDON ANAEL | PEDER LEO | ROBERT BENJAMIN _

 # Version 0.5
 - A basic HTTP server (1.0 and 1.1) capable of hosting multiple websites. (GET and HEAD only)
 # Version 0.3
 - A basic server capable of receiving messages and checking there validity. (Parser)
 # Version 0.2
 - A basic parser capable of handling HTTP requests.
 
 # How to run

 - To compile : *`make all`*
 - Before running : *`export LD_LIBRARY_PATH=.`*
 - To Run : *`./http-server `*

 ## Infos
 The first run of the programm will generate a config file and stopped. I the config file has error the programm will close and generate a new one instead so `watch out`.

 Source files are in src/

 Binary files are in bin/

 An HTTP documentation can be open (and generated with doxygen) with *`make doc`*
 
  ## Warning

 There is no security, so you can access every file the programm has permission to.
 Currently only 4 errors types are handle :
	- 200 OK
	- 404 Not Found
	- 400 Bad Request
	- 501 Not Implemented