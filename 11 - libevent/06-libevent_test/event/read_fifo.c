#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <event2/event.h>

// 对操作处理函数
void read_cb(evutil_socket_t fd, short what, void *arg)
{
    // 读管道
    char buf[1024] = {0};
    
    int len = read(fd, buf, sizeof(buf));
    
    printf("read event: %s \n", what & EV_READ ? "Yes" : "No");
    printf("data len = %d, buf = %s\n", len, buf);
    
    sleep(1);
}


// 读管道
int main(int argc, const char* argv[])
{
    unlink("myfifo");

    //创建有名管道
    mkfifo("myfifo", 0664);

    // open file
    //int fd = open("myfifo", O_RDONLY | O_NONBLOCK);
    int fd = open("myfifo", O_RDONLY);
    if(fd == -1)
    {
        perror("open error");
        exit(1);
    }

    // 创建个event_base
    struct event_base* base = NULL;
    base = event_base_new();

    // 创建事件
    struct event* ev = NULL;
    ev = event_new(base, fd, EV_READ | EV_PERSIST, read_cb, NULL);

    // 添加事件
    event_add(ev, NULL);

    // 事件循环
    event_base_dispatch(base);  // while（1） { epoll();}

    // 释放资源
    event_free(ev);
    event_base_free(base);
    close(fd);
    
    return 0;
}
