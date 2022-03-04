#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <event2/event.h>
#include <fcntl.h>
#include <pthread.h>

void sys_err(const char *str)
{
	perror(str);
	exit(1);
}

void write_cb(evutil_socket_t fd, short what, void *arg)
{
	char buf[] = "hello libevent";
	
	write(fd, buf, strlen(buf)+1);

	sleep(1);
	
	return ;	
}

int main(int argc, char *argv[])
{
	// 打开fifo的写端，
	
	int fd = open("testfifo", O_WRONLY | O_NONBLOCK);
	if (fd == -1)
	{
		sys_err("open err");	
	}
	
	// 创建 event_base;
	struct event_base *base = event_base_new();
	
	// 创建 事件对象
	struct event *ev = NULL;
	
	// struct event *event_new(struct event_base *base，evutil_socket_t fd，short what，event_callback_fn cb;  void *arg);
	ev = event_new(base, fd, EV_WRITE | EV_PERSIST, write_cb, NULL);
	
	// 添加事件到 event_base上
	// int event_add(struct event *ev, const struct timeval *tv);
	event_add(ev, NULL);
	
	// 启动循环
	//int event_base_dispatch(struct event_base *base);
	event_base_dispatch(base);				// while(1)
	
	// 销毁 event_base;
	event_base_free(base);

	return 0;
}
