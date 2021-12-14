#include <stdio.h>
#include <ctype.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>

#define SERV_PORT 9527


void sys_err(const char *str)
{
    perror(str);
    exit(1);
}

int main(int argc, char *argv[])
{
    int lfd = 0, cfd = 0;
    int ret, i;
    char buf[BUFSIZ], client_IP[1024];

    struct sockaddr_in serv_addr, clit_addr;  // 定义服务器地址结构 和 客户端地址结构
    socklen_t clit_addr_len;				  // 客户端地址结构大小

    serv_addr.sin_family = AF_INET;				// IPv4
    serv_addr.sin_port = htons(SERV_PORT);		// 转为网络字节序的 端口号
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);	// 获取本机任意有效IP

    lfd = socket(AF_INET, SOCK_STREAM, 0);		//创建一个 socket
    if (lfd == -1) {
        sys_err("socket error");
    }

    bind(lfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));//给服务器socket绑定地址结构（IP+port)

    listen(lfd, 128);					//	设置监听上限

    clit_addr_len = sizeof(clit_addr);	// 	获取客户端地址结构大小

    cfd = accept(lfd, (struct sockaddr *)&clit_addr, &clit_addr_len);	// 阻塞等待客户端连接请求
    if (cfd == -1)
        sys_err("accept error");

    printf("client ip:%s port:%d\n", 
            inet_ntop(AF_INET, &clit_addr.sin_addr.s_addr, client_IP, sizeof(client_IP)), 
            ntohs(clit_addr.sin_port));			// 根据accept传出参数，获取客户端 ip 和 port

    while (1) {
        ret = read(cfd, buf, sizeof(buf));		// 读客户端数据
        write(STDOUT_FILENO, buf, ret);			// 写到屏幕查看

        for (i = 0; i < ret; i++)				// 小写 -- 大写
            buf[i] = toupper(buf[i]);

        write(cfd, buf, ret);					// 将大写，写回给客户端。
    }

    close(lfd);
    close(cfd);

    return 0;
}
