src = $(wildcard ./*.c)  
obj = $(patsubst ./%.c, ./%.o, $(src)) 

myArgs= -Wall -g 
target=server
CC=gcc

ALL:$(target)

$(target):$(obj)
	$(CC) $^ -o $@ $(myArgs) -levent

$(obj):%.o:%.c
	$(CC) -c $^ -o $@ $(myArgs) -levent

clean:
	-rm -rf $(obj) $(target)

.PHONY: clean ALL

