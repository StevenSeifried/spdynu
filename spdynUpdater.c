/***************************************************************
 Copyright (C) 2014 by 3S-Berlin
 Copyright (C) 2017-2018 by Steven Seifried
 
 This program is free software; you can redistribute and/or
 modify it under the terms of the GNU General Public License
 version 2 as published by the Free Software Foundation.

 This program is distributed WITHOUT ANY WARRANTY;
 even without the implied WARRANTY OF MERCHANTABILITY or
 FITNESS FOR A PARTICULAR PURPOSE.
 See the GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program (see gnu-gpl v2.txt). If not, write to
 the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 Boston, MA 02111-1307, USA or visit the Free
 Software Foundation web page, http://www.fsf.org.

 ***************************************************************/


 /***************************************************************

 Implementation of the base64encode-function is taken from here : http://en.wikibooks.org/wiki/Algorithm_Implementation/Miscellaneous/Base64

 ****************************************************************/

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <sys/socket.h>
#include <netdb.h>
#include <time.h>
#include <regex.h>
#include <arpa/inet.h>
#include <inttypes.h>
#include <malloc.h>
#include <errno.h>
#include <unistd.h>
#include <getopt.h>

// Prototypen *******************************************************
char * getValue( char * );
int readConfig( void );
int readIPFile(void);
int writeIPFile(void);
int sendRequest(char *, char *, char *);
int writeLog(char *, ...);
void writeError( int );
int checkIP( char *, char * );
int base64encode(const void* , size_t , char* , size_t);
int checkResponse(char *);


// Gobal ************************************************************
#define MAX_CONFIG_LINESIZE 1024
#define MAX_MATCHES 1 //The maximum number of matches allowed in a single string
#define RESPONSE_LENGTH 2048
#define REQUEST_LENGTH 500
#define MAX_NUMBER_HOSTS 5
#define NUMBER_IP_HOSTS 3

// Hosts to check the current IP-Address
// *************************************
char * ipHost[NUMBER_IP_HOSTS] = { "checkip.spdyn.de", "checkip.dyn.com", "checkip.dyn.org" };  // list of Hosts to check the IP


// URL for the Host to be updated, e.g. update.spdyn.de (SPDYN) or dynupdate.no-ip.com (No-IP)
#define DEFAULT_UPDATE_HOST   "update.spdyn.de"

// Path to the config-file. can be changed using command line parameters
#define DEFAULT_CONF_FILENAME "/etc/spdynu.conf"

// path of config file and log file. Note that both files must be writeable.
#define DEFAULT_LOG_FILENAME  "/home/pi/spdynu.log"
#define DEFAULT_IP_FILENAME  "/tmp/spdynuIP.cnf"

char *confFileName = NULL;
char *logFileName = NULL;
char *ipFileName = NULL;
char *currentIP = NULL;
	
	
struct data
{
	char *updateHost;
	char *host;
	char *user;
	char *password;
	int  isToken;
};

struct data hostData[MAX_NUMBER_HOSTS];
int nrHosts = 0;

void showUsage(char *prgname) {
		printf("Usage: %s [--configfile configfile]\n", prgname);
		printf("Usage: %s [--version]\n", prgname);
}

/*************************************************************************************
**  Evaluate command line parameters and set defaults.
**
**************************************************************************************/
int evalCommandLine(int argc, char *argv[])
{
	struct option long_options[] = {
		{"configfile", optional_argument, 0, 'c'},
		{"version",    optional_argument, 0, 'v'},
		{"help",       no_argument,       0, 'h'},
		{0, 0, 0, 0}
	};

	int ch;
	while ((ch = getopt_long_only (argc, argv, "c:v:h", long_options, NULL)) > -1)
	{
		switch(ch) {
		case 'c':
			// confFileName = (char*)malloc( strlen( optarg ) + 1 );
			confFileName = strdup(optarg);
			break;
		case 'v':
			version = 2018.09;
			break;
		case 'h':
			showUsage(argv[0]);
			return 1;
		case -1:
			break;
		default:
			printf("Error: Invalid command line option specified\n");
			showUsage(argv[0]);
			return -1;
		}
	} while (ch >= 0);

	// apply defaults
	if (confFileName == NULL) {
		confFileName = strdup(DEFAULT_CONF_FILENAME);
	}

	return 0;
}

//*********************************************************************
int main(int argc , char *argv[])
{
	int i=0, erg, iperg=1;
	char serverResponse[RESPONSE_LENGTH];
	char request[REQUEST_LENGTH] = "";
	char checkedIP[16] = "";
	char Auth[70];
	char base64Auth[256];

	bzero(hostData, sizeof(struct data) * MAX_NUMBER_HOSTS);

	erg = evalCommandLine(argc, argv);
	if (erg == 1) {
		// usage was printed
		return 0;
	} else if (erg < 0) {
		// error
		return -1;
	}

	erg = readConfig();
	if( erg != 0)
	{
		writeLog("Error reading the config file in %s", confFileName);
		// notify user of error
		printf("Error reading the config file in %s\n", confFileName);
		return 1;
	}

	// result doesn't matter. If file doesn't exist, currentIP remains NULL
	erg = readIPFile();

	while( (i<NUMBER_IP_HOSTS) && (iperg != 0) )
	{
		bzero((char *) &serverResponse, RESPONSE_LENGTH);
		sprintf( request, "GET / HTTP/1.0\r\nHost: %s\r\n\r\n", ipHost[i] );
		erg = sendRequest( request, serverResponse, ipHost[i] );
		if( erg != 0 )
		{
			writeError(erg);
			i++;
			continue;
		}

		iperg = checkIP( serverResponse, checkedIP);
		if( iperg != 0 )
		{
			writeLog("No IP-Address found in response from server >>%s<<", ipHost[i]);
		}

		i++;
	}

	if( (iperg != 0)  ) return 1;

	if( ( currentIP == NULL ) || ( strcmp(checkedIP, currentIP) != 0 ) )
	{

		if (currentIP != NULL ) free( currentIP );
		currentIP = strdup( checkedIP );

		// assuming that all hosts need an Update
		for( i=0; i<nrHosts; i++ )
		{
			memset(serverResponse,'0', RESPONSE_LENGTH);

			if( hostData[i].isToken != 0 ) sprintf(Auth, "%s:%s", hostData[i].host, hostData[i].password);
			else sprintf(Auth, "%s:%s", hostData[i].user, hostData[i].password);

			base64encode( Auth, strlen(Auth), base64Auth, 256 );

			sprintf(request, "GET /nic/update?hostname=%s&myip=%s HTTP/1.0\r\nHost: %s\r\nAuthorization: Basic %s\r\n\r\n", hostData[i].host, currentIP, hostData[i].updateHost, base64Auth);

			erg = sendRequest( request, serverResponse, hostData[i].updateHost );
			if( erg!= 0 )
			{
				writeError(erg);
				return 1;
			}

			// write to logFile
			if( checkResponse(serverResponse) != 0 )
			{
				writeLog("IP from host %s updated to %s", hostData[i].host, currentIP);
			}
		}
		writeIPFile();
	}

	return 0;
}

static inline int iswhitespace( char c ){
	if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
		return 1;
	}
	return 0;
}

/*************************************************************************************
** Trim string, remove leading and trailing whitespace.
**************************************************************************************/
char * trimstr(char * s)
{
	char * retVal;
	char * last;

	retVal = s;

	// trim leading spaces, find first non-whitespace character
	while (iswhitespace(*retVal)) {
		retVal++;
	}

	if (*retVal == '\0') {
		// only whitespaces in line, or empty line
		return retVal;
	}

	// trim trailing spaces, find last non-whitespace character
	last = retVal + strlen(retVal) - 1;

	while (iswhitespace(*last)) {
		last--;
	}

	// last points to last non-whitespace character, terminate string
	*(last + 1) = '\0';

	return retVal;
}


/*************************************************************************************
**  read the config file given in confFileName
**
**************************************************************************************/
int readConfig(void)
{
	char line [MAX_CONFIG_LINESIZE];
	FILE * fp;

	fp = fopen (confFileName,"r");

	if (!fp) return 1;

	for (;;)
	{
		fgets(line, MAX_CONFIG_LINESIZE, fp);

		if (ferror(fp)) {
			writeLog("Error reading config file: %s", strerror(errno));
			fclose(fp);
			return errno;
		}

		if (feof(fp)) {
			break;
		}

		// paranoia
		line[MAX_CONFIG_LINESIZE - 1] = '\0';

		// first trim
		char *trimline = trimstr(line);

		// check for empty line
		if (trimline[0] == '\0') {
			continue;
		}

		// check for comment line
		if (trimline[0] == '#') {
			continue;
		}

		if(strcasecmp(line, "[HOST]") == 0)
		{
			nrHosts++;
			if( nrHosts > MAX_NUMBER_HOSTS ) return 0;
			continue;
		}

		// extract   keyword '=' value
		char *equalpos = strchr(trimline, '=');
		if (equalpos == NULL)
		{
			continue;
		}

		// separate kw and value
		*equalpos = '\0';
		equalpos++; // begin of value

		char *key = trimstr(trimline);
		char *value = trimstr(equalpos);


		if (strlen(key) == 0) {
			writeLog("Empty key, check the config file");
			fclose(fp);
			return 1;
		} else if (strcasecmp(key, "logfile") == 0) {
			logFileName = strdup(value);
		} else if (strcasecmp(key, "ipFile") == 0) {
			ipFileName = strdup(value);
		} else if (strcasecmp(key, "updateHost") == 0) {
			hostData[nrHosts-1].updateHost = strdup(value);
		} else if (strcasecmp(key, "host") == 0) {
			hostData[nrHosts-1].host = strdup(value);
		} else if (strcasecmp(key, "user") == 0) {
			hostData[nrHosts-1].user = strdup(value);
		} else if (strcasecmp(key, "pwd") == 0) {
			hostData[nrHosts-1].password = strdup(value);
		} else if (strcasecmp(key, "isToken") == 0) {
			hostData[nrHosts-1].isToken = atoi(value);
		} else {
			writeLog("Unknown key, check the config file: %s", key);
			return 1;
		}
	}

	fclose(fp);
	
	// set the defaults
	if ( ipFileName == NULL )
	{ 
		ipFileName = strdup(DEFAULT_IP_FILENAME);
	}
	
	if ( logFileName == NULL )
	{
		logFileName == strdup(DEFAULT_LOG_FILENAME);
	}

	// validate host configuration
	int i;
	for (i=0; i < nrHosts; i++) {
		if (hostData[i].host == NULL)
		{
			writeLog("'host' is not specified, check the config file");
			return 1;
		}
		if (hostData[i].password == NULL)
		{
			writeLog("'pwd' is not specified, check the config file");
			return 1;
		}
		// user is not needed when isToken == 1
		if (hostData[i].isToken == 0 && hostData[i].user == NULL)
		{
			writeLog("'user' is not specified and 'isToken' is 0, check the config file");
			return 1;
		}
		if( hostData[i].updateHost == NULL )
		{
			hostData[i].updateHost = strdup(DEFAULT_UPDATE_HOST);
		}
	}

	return 0;
}




/*************************************************************************************
**  read the IP from the IP-File
**
**************************************************************************************/
int readIPFile(void)
{
	char line [MAX_CONFIG_LINESIZE];
	int i;
	FILE * fp;

	currentIP = NULL;

	fp = fopen (ipFileName,"r");
	
	if (!fp) return 1;

	for (;;)
	{
		fgets(line, MAX_CONFIG_LINESIZE, fp);

		if (ferror(fp)) {
			writeLog("Error reading config file: %s", strerror(errno));
			fclose(fp);
			return errno;
		}

		if (feof(fp)) {
			break;
		}

		// paranoia
		line[MAX_CONFIG_LINESIZE - 1] = '\0';

		// first trim
		char *trimline = trimstr(line);

		// check for empty line
		if (trimline[0] == '\0') {
			continue;
		}

		// check for comment line
		if (trimline[0] == '#') {
			continue;
		}
		
		// extract   keyword '=' value
		char *equalpos = strchr(trimline, '=');
		if (equalpos == NULL)
		{
			continue;
		}

		// separate kw and value
		*equalpos = '\0';
		equalpos++; // begin of value

		char *key = trimstr(trimline);
		char *value = trimstr(equalpos);
		
		if (strcasecmp(key, "currentIP") == 0) 
		{
			currentIP = strdup(value);
		}
	}

	fclose(fp);

	return 0;
}


/*************************************************************************************
**  read the IP from the IP-File
**
**************************************************************************************/
int writeIPFile(void)
{
	FILE * fp;

	fp = fopen (ipFileName,"w");
	
	if (!fp)
			return 1;

	fprintf(fp, "currentIP=%s\n", currentIP);
	
	fclose(fp);
}


/*************************************************************************************
**  create the socket, sends the request given in request and receives
**  the server response and stores it in svr_reply
**************************************************************************************/
int sendRequest( char * request, char * svr_reply, char * hostname )
{
	struct sockaddr_in server;
	int sock;
	struct hostent *he;
	struct in_addr *addr_list;
	char ipAddress[16];
	char msg[256];

	//Create socket
	sock = socket(AF_INET , SOCK_STREAM , 0);
	if (sock == -1)
	{
		return 1;
	}

	he = gethostbyname(hostname); // get the host info

	if (he == NULL) {
		sprintf(msg, "Error resolving hostname %s", hostname);
		writeLog( msg );
		return 5;
	}

	 // get the Address
	addr_list = (struct in_addr *)he->h_addr;
	sprintf(ipAddress, "%s", inet_ntoa(*addr_list));

	bzero((char *) &server, sizeof(server));
	server.sin_addr.s_addr = inet_addr(ipAddress);
	server.sin_family = AF_INET;
	server.sin_port = htons( 80 );

	//Connect to remote server
	if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		return 2;
	}

	//Send data
	if( send(sock , request , strlen(request) , 0) < 0)
	{
		return 3;
	}

	if( recv(sock , svr_reply , RESPONSE_LENGTH , 0) < 0)
	{
		return 4;
	}

	// puts( svr_reply );

	close(sock);
	return 0;
}

/*************************************************************************************
**  write the error occured while sending to the log-file
**
**************************************************************************************/
void writeError( int nr )
{
	switch( nr )
	{
		case 1: writeLog("Error creating the socket"); break;
		case 2: writeLog("Error connecting to remote host"); break;
		case 4: writeLog("Error sending the request"); break;
		case 3: writeLog("Error receiving the server response"); break;
		case 6: writeLog("Error compiling the regular expression"); break;
	}
}

/*************************************************************************************
**  writes the message to the logfile
**
**************************************************************************************/
int writeLog(char * message, ...)
{

	FILE *fp = fopen (logFileName,"a");

	if (!fp)
		return 1;

	// format current time and date
	time_t t = time(NULL);
	struct tm *ts = localtime(&t);
	char *timeStr = asctime(ts); // result contains \n at end of string!
	timeStr[strlen(timeStr)-1] = '\0';

	va_list ap;
	va_start(ap, message);

	fprintf(fp, "%s - ", timeStr);
	vfprintf(fp, message, ap);
	fprintf(fp, "\n");

	fclose(fp);

	return 0;
}

/*************************************************************************************
**  checks the current IP in the server-response using a regular expression
**
**************************************************************************************/
int checkIP( char *sz, char * i)
{
	int erg, start, end;
	char * pointer;
	regmatch_t matches[MAX_MATCHES]; //A list of the matches in the string (a list of 1)

	regex_t exp;
	erg = regcomp(&exp, "[0-9]{1,3}[.][0-9]{1,3}[.][0-9]{1,3}[.][0-9]{1,3}", REG_EXTENDED);

	if( erg != 0 )
	{
		return 6;
	}


	if (regexec(&exp, sz, MAX_MATCHES, matches, 0) == 0) //regexec() returns 0 on match, otherwise REG_NOMATCH
	{
		start = matches[0].rm_so;
		end = matches[0].rm_eo;

		pointer =  &sz[start];
		strncpy(i, pointer,  end-start);
	} else {
		return 7;
	}

	return 0;
}

/*************************************************************************************
**  checks the update-response. return true if update successful, otherwise false
**
**************************************************************************************/
int checkResponse(char * response)
{
	char * pointer = NULL;

	pointer = strstr( response, "\r\n\r\n" );
	if( pointer == NULL ) return 0;

	if(strstr(pointer, "good") != NULL )
	{
		return 1; 	// update successful
	}

	if(strstr(pointer, "abuse") != NULL )
	{
		writeLog("The host can not be updated because it is locked due to previous faulty update attempts.");
		return 0;
	}
	if(strstr(pointer, "badauth") != NULL )
	{
		writeLog("An invalid username and / or an invalid password has been entered.");
		return 0;
	}
	if(strstr(pointer, "!yours") != NULL )
	{
		writeLog("The specified host can not be used under this user account.");
		return 0;
	}
	if(strstr(pointer, "notfqdn") != NULL )
	{
		writeLog("The specified host is not a FQDN.");
		return 0;
	}
	if(strstr(pointer, "numhost") != NULL )
	{
		writeLog("Tried to update more than 20 hosts in a request has been.");
		return 0;
	}
	if(strstr(pointer, "nochg") != NULL )
	{
		writeLog("The IP didn't change since the last update.");
		return 0;			// no Update needed, but OK
	}
	return 0;
}

int base64encode(const void* data_buf, size_t dataLength, char* result, size_t resultSize)
{
	 const char base64chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
	 const uint8_t *data = (const uint8_t *)data_buf;
	 size_t resultIndex = 0;
	 size_t x;
	 uint32_t n = 0;
	 int padCount = dataLength % 3;
	 uint8_t n0, n1, n2, n3;

	 /* increment over the length of the string, three characters at a time */
	 for (x = 0; x < dataLength; x += 3)
	 {
	  /* these three 8-bit (ASCII) characters become one 24-bit number */
	  n = data[x] << 16;

	  if((x+1) < dataLength)
		 n += data[x+1] << 8;

	  if((x+2) < dataLength)
		 n += data[x+2];

	  /* this 24-bit number gets separated into four 6-bit numbers */
	  n0 = (uint8_t)(n >> 18) & 63;
	  n1 = (uint8_t)(n >> 12) & 63;
	  n2 = (uint8_t)(n >> 6) & 63;
	  n3 = (uint8_t)n & 63;

	  /*
	   * if we have one byte available, then its encoding is spread
	   * out over two characters
	   */
	  if(resultIndex >= resultSize) return 0;   /* indicate failure: buffer too small */
	  result[resultIndex++] = base64chars[n0];
	  if(resultIndex >= resultSize) return 0;   /* indicate failure: buffer too small */
	  result[resultIndex++] = base64chars[n1];

	  /*
	   * if we have only two bytes available, then their encoding is
	   * spread out over three chars
	   */
	  if((x+1) < dataLength)
	  {
		 if(resultIndex >= resultSize) return 0;   /* indicate failure: buffer too small */
		 result[resultIndex++] = base64chars[n2];
	  }

	  /*
	   * if we have all three bytes available, then their encoding is spread
	   * out over four characters
	   */
	  if((x+2) < dataLength)
	  {
		 if(resultIndex >= resultSize) return 0;   /* indicate failure: buffer too small */
		 result[resultIndex++] = base64chars[n3];
	  }
	 }

	 /*
	* create and add padding that is required if we did not have a multiple of 3
	* number of characters available
	*/
	 if (padCount > 0)
	 {
	  for (; padCount < 3; padCount++)
	  {
		 if(resultIndex >= resultSize) return 0;   /* indicate failure: buffer too small */
		 result[resultIndex++] = '=';
	  }
	 }
	 if(resultIndex >= resultSize) return 0;   /* indicate failure: buffer too small */
	 result[resultIndex] = 0;
	 return 1;   /* indicate success */
}

