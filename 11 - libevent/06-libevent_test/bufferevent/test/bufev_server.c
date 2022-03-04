#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <event2/event.h>
#include <event2/bufferevent.h>
#include <event2/listener.h>
#include <pthread.h>

void sys_err(const char *str)
{
	perror(str);
	exit(1);
}

// 读事件回调
void read_cb(struct bufferevent *bev, void *arg) 
{
	char buf[1024] = {0};
	
	// 借助读缓冲，从客户端拿数据
	bufferevent_read(bev, buf, sizeof(buf));
	printf("clinet write: %s\n", buf);
	
	// 借助写缓冲，写数据回给客户端
	bufferevent_write(bev, "abcdefg", 7);	
}

// 写事件回调
void write_cb(struct bufferevent *bev, void *arg) 
{
	printf("-------fwq------has wrote\n");	
}

// 其他事件回调
void event_cb(struct bufferevent *bev,  short events, void *ctx) 
{
	
}

// 被回调，说明有客户端成功连接， cfd已经传入该参数内部。 创建bufferevent事件对象
// 与客户端完成读写操作。
 void listener_cb(struct evconnlistener *listener, evutil_socket_t sock, 
 			struct sockaddr *addr, int len, void *ptr)
 {
 	struct event_base *base = (struct event_base *)ptr;
 	
 	// 创建bufferevent 对象
 	struct bufferevent *bev = NULL;	
 	bev = bufferevent_socket_new(base, sock, BEV_OPT_CLOSE_ON_FREE);
 	
 	// 给bufferevent 对象 设置回调 read、write、event
 	void bufferevent_setcb(struct bufferevent * bufev,
				bufferevent_data_cb readcb,
				bufferevent_data_cb writecb,
				bufferevent_event_cb eventcb,
				void *cbarg );
		
	// 设置回调函数		
	bufferevent_setcb(bev, read_cb, write_cb, NULL, NULL);
	
	// 启动 read 缓冲区的 使能状态
	bufferevent_enable(bev, EV_READ); 
 	
 	return ;	
 }
 			 

int main(int argc, char *argv[])
{
	// 定义服务器地址结构
	struct sockaddr_in srv_addr;
	bzero(&srv_addr, sizeof(srv_addr));
	srv_addr.sin_family = AF_INET;
	srv_addr.sin_port = htons(8765);
	srv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	
	// 创建event_base
	struct event_base *base = event_base_new();
	
	/*
	struct evconnlistener *evconnlistener_new_bind (	
			struct event_base *base,
			evconnlistener_cb cb, 
			void *ptr, 
			unsigned flags,
			int backlog,
			const struct sockaddr *sa,
			int socklen);
	*/
	
	// 创建服务器监听器：
	struct evconnlistener *listener = NULL;
	listener = evconnlistener_new_bind(base, listener_cb, (void *)base, 
						LEV_OPT_CLOSE_ON_FREE | LEV_OPT_REUSEABLE, -1, 
						(struct sockaddr *)&srv_addr, sizeof(srv_addr));
	
	// 启动监听循环
	event_base_dispatch(base);
	
	// 销毁event_base
	evconnlistener_free(listener);
	event_base_free(base);

	return 0;
}
