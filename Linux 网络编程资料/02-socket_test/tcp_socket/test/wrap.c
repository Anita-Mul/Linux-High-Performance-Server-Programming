#include "wrap.h"

void sys_err(const char *str)
{
    perror(str);
    exit(1);
}

int Socket(int domain, int type, int protocol)
{
    int n;

    n = socket(domain, type, protocol);
    if (n == -1) {
        sys_err("socket error");
        return n;
    }

    return n;
}

int Listen(int sockfd, int backlog)
{
    int n = 0;

    n = listen(sockfd, backlog);
    if (n == -1) {
        sys_err("listen error");
        return n;
    }
    return 0;
}
