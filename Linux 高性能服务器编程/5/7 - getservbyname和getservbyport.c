#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <unistd.h>
#include <assert.h>

/*
	gethostbyname函数根据主机名称获取主机的完整信息
	gethostbyaddr函数根据IP地址获取主机的完整信息
	gethostbyname函数通常先在本地的/etc/hosts配置文件中查找主机
	如果没有找到，再去访问DNS服务器
*/
int main( int argc, char *argv[] )
{
	assert( argc == 2 );
	char *host = argv[1];
	struct hostent* hostinfo = gethostbyname( host );
	assert( hostinfo );
	struct servent* servinfo = getservbyname( "daytime", "tcp" );
	assert( servinfo );
	printf( "daytime port is %d\n", ntohs( servinfo->s_port ) );

	struct sockaddr_in address;
	address.sin_family = AF_INET;
	address.sin_port = servinfo->s_port;
	address.sin_addr = *( struct in_addr* )*hostinfo->h_addr_list;

	int sockfd = socket( AF_INET, SOCK_STREAM, 0 );
	int result = connect( sockfd, (struct sockaddr* )&address, sizeof( address ) );
	assert( result != -1 );

	char buffer[128];
	result = read( sockfd, buffer, sizeof( buffer ) );
	assert( result > 0 );
	buffer[ result ] = '\0';
	printf( "the day tiem is: %s", buffer );
	close( sockfd );
    return 0;
}