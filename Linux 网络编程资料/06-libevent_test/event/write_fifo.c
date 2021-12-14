#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <event2/event.h>

// 对操作处理函数
void write_cb(evutil_socket_t fd, short what, void *arg)
{
    // write管道
    char buf[1024] = {0};
    
    static int num = 0;
    sprintf(buf, "hello,world-%d\n", num++);
    write(fd, buf, strlen(buf)+1);
    
    sleep(1);
}


// 写管道
int main(int argc, const char* argv[])
{
    // open file
    //int fd = open("myfifo", O_WRONLY | O_NONBLOCK);
    int fd = open("myfifo", O_WRONLY);
    if(fd == -1)
    {
        perror("open error");
        exit(1);
    }

    // 写管道
    struct event_base* base = NULL;
    base = event_base_new();

    // 创建事件
    struct event* ev = NULL;
    // 检测的写缓冲区是否有空间写
    //ev = event_new(base, fd, EV_WRITE , write_cb, NULL);
    ev = event_new(base, fd, EV_WRITE | EV_PERSIST, write_cb, NULL);

    // 添加事件
    event_add(ev, NULL);

    // 事件循环
    event_base_dispatch(base);

    // 释放资源
    event_free(ev);
    event_base_free(base);
    close(fd);
    
    return 0;
}
