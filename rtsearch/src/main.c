#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include <stdio.h>
#include "json_helper.h"
#include "redis_util.h"
#include "znet.h"
#include "client_manager.h"
#include "log.h"
#include "segword.h"

log_t *log_;
static int stop_daemon = 0;

static void handler(int sig)
{
    if(sig == SIGINT)
    {
	stop_daemon = 1;
    }
}

static int daemonize()
{
    int maxfd, fd;
    switch (fork()) {
        case -1: return -1;
        case 0: break;
        default: _exit(0);
    }
    if (setsid() == -1)
        return -1;
    switch (fork()) {
        case -1: return -1;
        case 0: break;
        default: _exit(0);
    }
    umask(0); 
    maxfd = sysconf(_SC_OPEN_MAX);
    if (maxfd == -1)
        maxfd = 8192;
    for (fd = 0; fd < maxfd; fd++)
        close(fd);

    close(STDIN_FILENO);
    fd = open("/dev/null", O_RDWR);
    if (fd != STDIN_FILENO)
        return -1;
    if (dup2(STDIN_FILENO, STDOUT_FILENO) != STDOUT_FILENO)
        return -1;
    if (dup2(STDIN_FILENO, STDERR_FILENO) != STDERR_FILENO)
        return -1;
    return 0;
}

int main()
{
    //daemonize();
    //process signal
    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = handler;
    sigaction(SIGINT, &sa, NULL);

    json_object *jfile = json_object_from_file("config.json");
    if(!jobject_ptr_isvalid(jfile))
    {
        printf("open config.json failed!\n");
        return -1;
    }
    json_object *jlog = json_util_get(jfile,"CONFIG.log");
    if(!jlog)   return -1;
    const char *strlog = json_object_get_string(jlog);

    log_open(&log_,strlog);

    json_object *jlog_level = json_util_get(jfile,"CONFIG.log_level");
    if(jlog_level)
    {
        int log_level = json_object_get_int(jlog_level);
        log_level_set(log_,log_level);
    }

    loga(log_,"starting server!");

    json_object_put(jfile);

    int rv;
    rv = segword_init();
    if(rv < 0)
    {
        loga(log_,"segword_init failed!");
        return -1;
    }

    redis_init();

    rv = cm_start();
    if(rv < 0)
	return -1;

    struct timeval delay;
    while(!stop_daemon)
    {
        delay.tv_sec = 0;
        delay.tv_usec = 100000;

        int rv = select(0,NULL,NULL,NULL,&delay); 
        if(rv == 0)
        {
            continue;
        }
    }
    cm_stop();
    cm_destroy();
    redis_fini();
    segword_fini();

    printf("Normal exit!\n");
    return 0;
}
