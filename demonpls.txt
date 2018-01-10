/*
        demonizacja odbierania pakietów UDP 
	matusiaktomasz@windowslive.com
	@TM

*/

#include <stdio.h> // printf
#include <stdlib.h> //exit
#include <unistd.h>     // fork, chdir, sysconf
#include <signal.h>     //signal
#include <sys/types.h>
#include <sys/stat.h> // umask
#include <syslog.h>  // syslog, openlog, closelog
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#define BUFLEN 1024
#define PORT 1234

//#define DEBUG

void die(char *msg) // die == exit
{
        perror(msg);
        exit(1);
}


void  daemonize()
{
        int pid = fork();
        if(pid < 0) exit(1); // fork fail
        if(pid > 0) exit(0); // still in parent process


        if(setsid() < 0) exit(EXIT_FAILURE);
        /* setsid - tworzy nowa sesje,  proces jest teraz
        odlaczony od kontrolujacego terminalu (CTTY) */

	// sygna³y przekuzujemy demonom np gdy chcemy je zatrzymac
 	/* obsluga sygna³ow  -> catch and ignore the signals */
        signal(SIGCHLD, SIG_IGN);
        signal(SIGHUP, SIG_IGN); // if is receivded, reload the cfg files, it t$
        // SIGTERM  if is received, shut downe the deamon and  exit cleanly

        umask(0); //User MASK, new permissions
        chdir("/"); //Change DIRectory to  root dir

   	printf("ok\n"); fflush(stdout);

        // close alle open file descriptors
	// zamykanie wszystkich deskryptorow,
	// czyli odlaczenie stdio np!

        int x; // <- przypisuje liczbe descrypotorów

        for(x = sysconf(_SC_OPEN_MAX); x>=0; x--)
        {
                close(x);
	}

  	openlog("sql_dm", LOG_PID, LOG_DAEMON);
}


int main(void)
{

	syslog(LOG_NOTICE, "start demona");

 	FILE* f = fopen("/tmp/daneDemon.txt","a" ); //a = append, czyli dopisywanie kolejnych linijek

	#if !defined(DEBUG)
	// Dla UDP, demonizacja a potem tworzenie socketu!
        daemonize();
	#endif


        int sockfd; /* socket */
       
        int clientlen; /* byte size of client's address */
        struct sockaddr_in serveraddr; /* server's addr */
        struct sockaddr_in clientaddr; /* client addr */

        struct hostent *hostp; /* client host info */
 	
	char *hostaddrp; /* dotted decimal host addr string */
        int optval; /* flag value for setsockopt -> tricky! */ 
        int n; /* message byte size */

        struct sockaddr_in si_me, si_other;
        int s, i, slen = sizeof(si_other) , recv_len;

        char buf[BUFLEN];

        sockfd = socket(AF_INET, SOCK_DGRAM,IPPROTO_UDP ); 
	//datagram, UDP -> domyslnie jest unicast

	#if defined(DEBUG)
 	if (sockfd < 0) printf("ERROR opening socket\n");
        else 		printf("SOCKET OPEN OK\n");
	#endif

        // zero out the structure
        //memset((char *) &si_me, 0, sizeof(si_me));
        //bzero((char *) &serveraddr, sizeof(serveraddr));

        si_me.sin_family = AF_INET;
        si_me.sin_port = htons(PORT);
        si_me.sin_addr.s_addr = htonl(INADDR_ANY);


        //bind socket to port
        if( bind(sockfd , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
        {
                die("bind");
		#if defined(DEBUG)
                printf("binded\n");
		#endif
        }

	// po przybindowaniu socketu zmieniamy domyslny unicast na broadcast! plus tricky optval!
        optval = 1;
        setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, (const void*)&optval, size$, sizeof(int));

	fprintf(f, "Start demona\n");

        while(1)
        {
               	//try to receive some data, this is a blocking call

 		if ((recv_len = recvfrom(sockfd, buf, BUFLEN, 0, (struct sockad$sockaddr *) &si_other, &slen)) == -1)
 		{
                        syslog(LOG_NOTICE, "smierc demona");
 			fprintf(f, "smierc demona\n");
                        die("recvfrom()");
			fclose(f);
                }
		else
		{
			fprintf(f, "Demon dzia³a. Wiadomoœæ: \n");
			fprintf(f, "%s\n", buf);
		}                

 	}

	fclose(f);
        syslog(LOG_NOTICE, "Demon zak.");
        closelog();

     return EXIT_SUCCESS;
}















