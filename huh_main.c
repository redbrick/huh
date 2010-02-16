#include "g_list.h"
#include <unistd.h>
#include <stdio.h>
#include <string.h>

/* BIG FAT WARNING */
/* IS SGID ROOT/WHEEL */


// the files in question
#define LOG_FILE	"/var/log/huh.log"
#define BOMB_FILE	"/var/log/write_bomb.log"

// the line length
#define LINE_LEN	1024

// structure to store log entries in
typedef struct {
	char mesg_sender[17];
	char mesg_time[17];
	char mesg_mon[17];
	int  mesg_day;
	int  mesg_lines;
} mesg_entry;

// dont care about traversal
int mesgkey(void *key, void *data) {
	//do nothing
	return 0;
}

// need a free function for g_list
void freemesg(void *ptr) {
	free(ptr);
}


void usage(void) {

	/* old usage message is for old huh 
	printf(	"usage: huh [-b] [number]\n"
		"    -b:  filter write bomb log ( > 25 lines ) instead of standard\n"
		"number:  show last <number> messages to self\n\n"
		"send bug reports to colmmacc\n"
		"BUGS: does not report multiple messages from same person\n"
		"      which arrived very close together\n"
 		"      logs get dumped on a regular basis\n");
	*/

	printf(	"usage: huh [number]\n"
		"number:  show last <number> messages to self\n\n" );

	exit(1);
}


int main(int argc, char **argv) 
{
	int 	option;			// option
	int 	lines=5;		// number of lines to show
	FILE 	*in;			// the file
	char 	line[ LINE_LEN + 1 ];	// a line in the file
	char	*file=LOG_FILE;		// the filename
	char 	*login;			// our login
	char 	*username;		// login plus '\n'
	char	scanf_fmt[ LINE_LEN + 1];
	mesg_entry log_line, *ptr;	// mesg entries
	g_list	*g;			// the g_list
	int	i;			// stands for "iteration"


	while ((option = getopt(argc, argv, "hb")) != -1) {
		switch(option) {
			case 'B':
			case 'b':	file=BOMB_FILE;
						break;	
			default:	
						usage();							
		}
	}

	if (argc > (optind +1)) {
		usage();
	} else if (argc == (optind+1)) {
		lines=atoi(argv[optind]);
	}

	if((login=getlogin())==NULL) {
		fprintf(stderr,"huh: error detecting login\n");
		exit(1);
	}
	
	if((username=(char *)malloc(strlen(login) + 4))==NULL) {
		fprintf(stderr,"huh: error mallocing username\n");
		exit(1);
	}

	sprintf(username," %s \n" , login);
	

	// create the list
	g=g_listAlloc(mesgkey);

	i=0;
	if((in=fopen(file,"r"))!=NULL) {
		while(!feof(in),fgets(line, LINE_LEN, in)) {
			if(strstr(line, username)) {
				/* o.k. we know name is at end of line, lets make sure
				** its a log line 
				*/
		
				snprintf(scanf_fmt, LINE_LEN ,"%%16s  %%d %%16s %%*s %%*s %%16s send %%d line(s) to %%*s");

				
				if((sscanf(line , scanf_fmt,
				 log_line.mesg_mon, &log_line.mesg_day, log_line.mesg_time, log_line.mesg_sender,
				 &log_line.mesg_lines))==5) {

					// Line is O.K.	
					if((ptr=(mesg_entry *)malloc(sizeof(mesg_entry)))==NULL) {
						fprintf(stderr,"huh: error mallocing mesg_entry ptr\n");
						exit(1);
					}
				
					bcopy((void *)&log_line, (void *)ptr, sizeof(mesg_entry));
					i++;

					// add to list
					g_listQPut(g, ptr, freemesg);		
				}
			}
		}
		fclose(in);
	} else {
		fprintf(stderr,"huh: error opening %s\n", file);
		exit(1);
	}

	// discard the unwanted entries
	while( i > lines )  {
		g_listQGet(g)!=NULL;
		i--;
	}

	// now print the lines
	while((ptr=(mesg_entry *)g_listQGet(g))!=NULL) {
		printf("%s %d %s: %s sent you a message (%d lines)\n",
				ptr->mesg_mon, ptr->mesg_day, ptr->mesg_time,
				ptr->mesg_sender, ptr->mesg_lines);
	}

	// free the list */
	g_listFree(g);

	return 0;
}
