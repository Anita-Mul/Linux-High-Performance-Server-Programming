#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>

#define BUF_SIZE 1024

static int connfd;

/**
 * 在Linux环境下，内核通知应用程序带外数据到达主要有两种方法：
 * 一种是第9章介绍的I/O复用技术，select等系统调用在接收到带外
 * 数据时将返回，并向应用程序报告socket上的异常事件，代码清单
 * 5 - IO复用-select给出了一个这方面的例子；另外一种方法就是使用SIGURG信号
 * 
 */
// 观察服务器是如何同时处理普通数据和带外数据

/*SIGURG信号的处理函数*/
void sig_urg( int sig )
{
    int save_errno = errno;
    
    char buffer[ BUF_SIZE ];
    memset( buffer, '\0', BUF_SIZE );
    /*接收带外数据*/
    int ret = recv( connfd, buffer, BUF_SIZE-1, MSG_OOB );
    printf( "got %d bytes of oob data '%s'\n", ret, buffer );

    errno = save_errno;
}

void addsig( int sig, void ( *sig_handler )( int ) )
{
    struct sigaction sa;
    memset( &sa, '\0', sizeof( sa ) );
    sa.sa_handler = sig_handler;
    sa.sa_flags |= SA_RESTART;
    sigfillset( &sa.sa_mask );
    assert( sigaction( sig, &sa, NULL ) != -1 );
}

int main( int argc, char* argv[] )
{
    if( argc <= 2 )
    {
        printf( "usage: %s ip_address port_number\n", basename( argv[0] ) );
        return 1;
    }
    const char* ip = argv[1];
    int port = atoi( argv[2] );

    struct sockaddr_in address;
    bzero( &address, sizeof( address ) );
    address.sin_family = AF_INET;
    inet_pton( AF_INET, ip, &address.sin_addr );
    address.sin_port = htons( port );

    int sock = socket( PF_INET, SOCK_STREAM, 0 );
    assert( sock >= 0 );

    int ret = bind( sock, ( struct sockaddr* )&address, sizeof( address ) );
    assert( ret != -1 );

    ret = listen( sock, 5 );
    assert( ret != -1 );

    struct sockaddr_in client;
    socklen_t client_addrlength = sizeof( client );
    connfd = accept( sock, ( struct sockaddr* )&client, &client_addrlength );
    
    if ( connfd < 0 )
    {
        printf( "errno is: %d\n", errno );
    }
    else
    {
        addsig( SIGURG, sig_urg );
        /*使用SIGURG信号之前，我们必须设置socket的宿主进程或进程组*/
        fcntl( connfd, F_SETOWN, getpid() );

        char buffer[ BUF_SIZE ];

        /*循环接收普通数据*/
        while( 1 )
        {
            memset( buffer, '\0', BUF_SIZE );
            ret = recv( connfd, buffer, BUF_SIZE-1, 0 );

            if( ret <= 0 )
            {
                break;
            }

            printf( "got %d bytes of normal data '%s'\n", ret, buffer );
        }

        close( connfd );
    }

    close( sock );
    return 0;
}


/**
 * 有些传输层协议具有带外（Out Of Band，OOB）数据的概念，用于迅速通告对方本端发生
 * 的重要事件。因此，带外数据比普通数据（也称为带内数据）有更高的优先级，它应该总是
 * 立即被发送，而不论发送缓冲区中是否有排队等待发送的普通数据。带外数据的传输可以使
 * 用一条独立的传输层连接，也可以映射到传输普通数据的连接中。实际应用中，带外数据的
 * 使用很少见，已知的仅有telnet、ftp等远程非活跃程序。UDP没有实现带外数据传输，TCP
 * 也没有真正的带外数据。不过TCP利用其头部中的紧急指针标志和紧急指针两个字段，给应
 * 用程序提供了一种紧急方式。TCP的紧急方式利用传输普通数据的连接来传输紧急数据。这
 * 种紧急数据的含义和带外数据类似，因此后文也将TCP紧急数据称为带外数据。
 * 
 */
