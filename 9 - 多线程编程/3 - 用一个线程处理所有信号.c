#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>

/* Simple error handling functions */

#define handle_error_en(en, msg) \
        do { errno = en; perror(msg); exit(EXIT_FAILURE); } while (0)

static void *sig_thread(void *arg)
{
    printf( "yyyyy, thread id is: %ld\n", pthread_self() );
    sigset_t aset;
    int s, sig;
    sigemptyset(&aset);
    sigaddset(&aset, SIGQUIT);
    sigaddset(&aset, SIGUSR1);
    //s = pthread_sigmask(SIG_BLOCK, &aset, NULL);
    sigset_t *set = (sigset_t *) arg;

    for (;;) {
        /*第二个步骤，调用sigwait等待信号*/
        s = sigwait(set, &sig);
        if (s != 0)
            handle_error_en(s, "sigwait");
        printf("Signal handling thread got signal %d\n", sig);
    }
}

static void handler( int arg )
{
	printf( "xxxxx, thread id is: %ld\n", pthread_self() );
}

int main(int argc, char *argv[])
{
    pthread_t thread;
    sigset_t set;
    int s;

    /* Block SIGINT; other threads created by main() will inherit
    *               a copy of the signal mask. */

    signal( SIGQUIT, handler );     // 键盘输入使进程退出（ctrl + \）
    //  if (s != 0)
    //     handle_error_en(s, "pthread_sigmask");

    /*第一个步骤，在主线程中设置信号掩码*/
    s = pthread_create(&thread, NULL, &sig_thread, (void *) &set);
    sigemptyset(&set);
    sigaddset(&set, SIGQUIT);
    sigaddset(&set, SIGUSR1);
    //s = pthread_sigmask(SIG_BLOCK, &set, NULL);

    if (s != 0)
        handle_error_en(s, "pthread_create");

    printf( "sub thread with id: %ld\n", thread );
    /* Main thread carries on to create other threads and/or do
     *               other work */

    pause();     /* Dummy pause so we can test program */
}

/**
 * 在某个线程中调用如下函数来等待信号并处理之
 * #include＜signal.h＞
 * int sigwait(const sigset_t*set,int*sig);
 * 
 * 在多线程环境下我们应该使用如下所示的pthread版本的sigprocmask函数来设置线程信号掩码
 * #include＜pthread.h＞
 * #include＜signal.h＞
 * int pthread_sigmask(int how,constsigset_t*newmask,sigset_t*oldmask);
 */