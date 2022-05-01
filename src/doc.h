/**
 * \mainpage Documentation of the HTTP-Server
 * @authors MANDON Anaël, PEDER léo, ROBERT Benjamin
 * 
 * This is the documentation of our project we did during the first year at ESISAR in FRANCE.
 * <BR>
 * <B>Documentation for important parts can be found within the menu above.</B>
 * <BR>
 * The first run of the programm will generate a config file and stopped. I the config file has error the programm will close and generate a new one instead so `watch out`.
 * 
 * \section info How to ?
 * \subsection run Run
 * - To compile : `make all`
 * - Before running : `export LD_LIBRARY_PATH=.`
 * - To Run : `./http-server`
 * - Open and generate the doc `make doc`
 * \subsection get Get
 * - Source files are in `/src`
 * - Binary files are in `/bin`
 * - Documentation files ar in `/doc_html`
 * 
 * \section update Versions
 * \subsection V05 0.5
 * - A basic HTTP server (1.0 and 1.1) capable of hosting multiple websites. (GET and HEAD only)
 * \subsection V03 0.3
 * - A basic server capable of receiving messages and checking there validity. (Parser)
 * \subsection V02 0.2
 * - A basic parser capable of handling HTTP requests.
 * 
 * \section warnings Warnings
 *  There is no security, so you can access every file the programm has permission to.
 *  Currently only 4 errors types are handle :
 *	- 200 OK
 *	- 404 Not Found
 *	- 400 Bad Request
 *	- 501 Not Implemented
 */