/*

	Deamon  SQL dla Linuksa, target iMX53

	 @TM
*/

#include <stdio.h> // printf
#include <stdlib.h> //exit
#include <unistd.h>	// fork, chdir, sysconf
#include <signal.h>	//signal
#include <sys/types.h> 
#include <sys/stat.h> // umask
#include <syslog.h>  // syslog, openlog, closelog

void daemon_SQL()
{
	t pid = fork();
	
	if(t_pid < 0) exit(1); // fork fail
	if(t_pid > 0) exit(0); // still in parent process


	if(setsid() < 0) exit(EXIT_FAILURE);
	/* setsid - tworzy nowa sesje,  proces jest teraz 
	odlaczony od kontrolujacego terminalu (CTTY) */

	/* obsluga sygnałow  -> catch and ignore the signals */
	signal(SIGCHLD, SIG_IGN);
	signal(SIGHUP, SIG_IGN); // if is receivded, reload the cfg files, it this applies
	// SIGTERM  if is received, shut downe the deamon and  exit cleanly
	

	umask(0); //User MASK, new permissions
	chdir("/"); //Change DIRectory to  root dir

	// close alle open file descriptors
	int x;
	for(x = sysconf(_SC_OPEN_MAX); x>=0; x--)
	{
		close(x);
	}
	openlog("sql_dm", LOG_PID, LOG_DAEMON);
}




int main(void)
{
	daemon_SQL();
	while(1)
	{
		syslog(LOG_NOTICE, "Start demona");
		sleep(20);
		break;
	}
	syslog(LOG_NOTICE, "Demon zak.");
	closelog();

	return EXIT_SUCCESS;
}


