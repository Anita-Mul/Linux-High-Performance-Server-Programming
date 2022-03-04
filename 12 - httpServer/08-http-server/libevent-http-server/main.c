#include <stdio.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <event2/bufferevent.h>
#include <event2/listener.h>
#include <event2/event.h>
#include "libevent_http.h"

int main(int argc, char **argv)
{
    if(argc < 3)
    {
        printf("./event_http port path\n");
        return -1;
    }
    if(chdir(argv[2]) < 0) {
        printf("dir is not exists: %s\n", argv[2]);
        perror("chdir err:");
        return -1;
    }

    struct event_base *base;
    struct evconnlistener *listener;
    struct event *signal_event;

    struct sockaddr_in sin;
    base = event_base_new();
    if (!base)
    {
        fprintf(stderr, "Could not initialize libevent!\n");
        return 1;
    }

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(atoi(argv[1]));

    // 创建监听的套接字，绑定，监听，接受连接请求
    listener = evconnlistener_new_bind(base, listener_cb, (void *)base,
                    LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE, -1,
                    (struct sockaddr*)&sin, sizeof(sin));
    if (!listener)
    {
        fprintf(stderr, "Could not create a listener!\n");
        return 1;
    }

    // 创建信号事件, 捕捉并处理
    signal_event = evsignal_new(base, SIGINT, signal_cb, (void *)base);
    if (!signal_event || event_add(signal_event, NULL)<0) 
    {
        fprintf(stderr, "Could not create/add a signal event!\n");
        return 1;
    }

    // 事件循环
    event_base_dispatch(base);

    evconnlistener_free(listener);
    event_free(signal_event);
    event_base_free(base);

    printf("done\n");

    return 0;
}

