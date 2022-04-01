#########################################################################
# File Name: Makefile
# 作者:YJK
# mail: 745506980@qq.comc
# Created Time: 2021年07月19日 星期一 10时05分25秒
#########################################################################
#    $^ 当前规则中的所有依赖
#    $< 当前规则中的第一个依赖
#    $@ 当前规则中触发命令生成的目标
#    @ 不把执行的信息打印到显示屏上


CROSS_COMPILE = 
CC = $(CROSS_COMPILE)gcc
LD_COMMON_PATH = ../common
COMMON_PATH = ../common
SRC_PATH = ./src
COMMONDIRS += ../common

SERVER_TARGET = myserver
#CLIENT_TARGET = client11

SERVER_OBJS += $(SRC_PATH)/main.o
SERVER_OBJS += $(SRC_PATH)/server.o
SERVER_OBJS += $(SRC_PATH)/server_datahandle.o
SERVER_OBJS += $(SRC_PATH)/server_heartbeat_handle.o
SERVER_OBJS += $(SRC_PATH)/server_sendmessage_thread.o
SERVER_OBJS += $(SRC_PATH)/server_client_handle.o
SERVER_OBJS += $(SRC_PATH)/server_daemonize.o

CFLAGS += -g 
CFLAGS += -I ./include/
CFLAGS += -I ../common/include/

LDFLAGS += -lpthread
LDFLAGS += -L$(LD_COMMON_PATH) -lcommon


export CC AR


all :$(COMMONDIRS) $(SERVER_TARGET) 

$(COMMONDIRS) : ECHO

	make -C $@

$(SERVER_TARGET) : $(SERVER_OBJS)
	$(CC) $^ -o $@ $(LDFLAGS)

$(CLIENT_TARGET) : $(CLIENT_OBJS)
	$(CC) $^ -o $@  $(LDFLAGS)

ECHO:
	@echo $(COMMONDIRS)

$(SRC_PATH)/%.o : $(SRC_PATH)%.c
	$(CC) -c $< $(CFLAGS) -o $@
 
install:
	cp $(SERVER_TARGET) /mnt/nfs/exec/ -rf

clean:
	rm $(SERVER_TARGET) $(CLIENT_TARGET) $(CLIENT_OBJS) $(SERVER_OBJS) -rf
	make -C $(COMMON_PATH) clean