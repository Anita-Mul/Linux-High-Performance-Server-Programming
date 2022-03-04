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

void read_cb(evutil_socket_t fd, short what, void *arg)
{
	char buf[1024] = {0};
	int len = read(fd, buf, sizeof(buf));
	
	printf("what = %s, read from write : %s\n", 
			what & EV_READ ? "read YES": "read NO", buf);
	sleep(1);
	
	return ;	
}

int main(int argc, char *argv[])
{
	// ���� fifo
	unlink("testfifo");
	mkfifo("testfifo", 0644);
	
	// ��fifo�Ķ��ˣ�
	
	int fd = open("testfifo", O_RDONLY | O_NONBLOCK);
	if (fd == -1)
	{
		sys_err("open err");	
	}
	
	// ���� event_base;
	struct event_base *base = event_base_new();
	
	// ���� �¼�
	struct event *ev = NULL;
	
	// struct event *event_new(struct event_base *base��evutil_socket_t fd��short what��event_callback_fn cb;  void *arg);
	ev = event_new(base, fd, EV_READ | EV_PERSIST, read_cb, NULL);
	
	// ����¼��� event_base��
	// int event_add(struct event *ev, const struct timeval *tv);
	event_add(ev, NULL);
	
	// ����ѭ��
	//int event_base_dispatch(struct event_base *base);
	event_base_dispatch(base);				// while(1)
	
	// ���� event_base;
	event_base_free(base);

	return 0;
}
