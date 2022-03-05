#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>

/*
socket被命名之后，还不能马上接受客户连接
我们需要使用如下系统调用来创建一个监听队列以存放待处理的客户连接

socket 指定被监听的 socket
backlog 参数提示内核监听队列的最大长度【5】
int listen(int socket, int backlog) {}
*/


/**
 *  ./testlisten 192.168.1.109 12345 5#监听12345端口，给backlog传递典型值5
 *  telnet 192.168.1.109 12345#多次执行之
 *  netstat-nt|grep 12345#多次执行之   展示12345端口上的连接状态
 */
static bool stop = false;
/*SIGTERM信号的处理函数，触发时结束主程序中的循环*/
static void handle_term( int sig )
{
    stop = true;
}

int main( int argc, char* argv[] )
{
    signal( SIGTERM, handle_term );

    if( argc <= 3 )
    {
        printf( "usage: %s ip_address port_number backlog\n", basename( argv[0] ) );
        return 1;
    }
    const char* ip = argv[1];
    int port = atoi( argv[2] );
    int backlog = atoi( argv[3] );

    int sock = socket( PF_INET, SOCK_STREAM, 0 );
    assert( sock >= 0 );

    struct sockaddr_in address;
    bzero( &address, sizeof( address ) );
    address.sin_family = AF_INET;
    inet_pton( AF_INET, ip, &address.sin_addr );
    address.sin_port = htons( port );

    int ret = bind( sock, ( struct sockaddr* )&address, sizeof( address ) );
    assert( ret != -1 );

    ret = listen( sock, backlog );
    assert( ret != -1 );

    while ( ! stop )
    {
        sleep( 1 );
    }

    close( sock );
    return 0;
}
