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

    struct sockaddr_in serv_addr, clit_addr;  // �����������ַ�ṹ �� �ͻ��˵�ַ�ṹ
    socklen_t clit_addr_len;				  // �ͻ��˵�ַ�ṹ��С

    serv_addr.sin_family = AF_INET;				// IPv4
    serv_addr.sin_port = htons(SERV_PORT);		// תΪ�����ֽ���� �˿ں�
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);	// ��ȡ����������ЧIP

    lfd = socket(AF_INET, SOCK_STREAM, 0);		//����һ�� socket
    if (lfd == -1) {
        sys_err("socket error");
    }

    bind(lfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));//��������socket�󶨵�ַ�ṹ��IP+port)

    listen(lfd, 128);					//	���ü�������

    clit_addr_len = sizeof(clit_addr);	// 	��ȡ�ͻ��˵�ַ�ṹ��С

    cfd = accept(lfd, (struct sockaddr *)&clit_addr, &clit_addr_len);	// �����ȴ��ͻ�����������
    if (cfd == -1)
        sys_err("accept error");

    printf("client ip:%s port:%d\n", 
            inet_ntop(AF_INET, &clit_addr.sin_addr.s_addr, client_IP, sizeof(client_IP)), 
            ntohs(clit_addr.sin_port));			// ����accept������������ȡ�ͻ��� ip �� port

    while (1) {
        ret = read(cfd, buf, sizeof(buf));		// ���ͻ�������
        write(STDOUT_FILENO, buf, ret);			// д����Ļ�鿴

        for (i = 0; i < ret; i++)				// Сд -- ��д
            buf[i] = toupper(buf[i]);

        write(cfd, buf, ret);					// ����д��д�ظ��ͻ��ˡ�
    }

    close(lfd);
    close(cfd);

    return 0;
}
