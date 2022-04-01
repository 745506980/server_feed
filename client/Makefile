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


#CROSS_COMPILE = arm-none-linux-gnueabi-
CC = $(CROSS_COMPILE)gcc
AR = $(CROSS_COMPILE)ar

COMMONDIRS += ../common
COMMON_PATH = ../common
LD_COMMON_PATH = ../common
CLIENT_SRC_PATH = ./src
CLIENT_LIBS = LIBS
CLIENT_TARGET = client

DEPEND_ON_OBJ = ../


CLIENT_OBJS += $(CLIENT_SRC_PATH)/client.o
CLIENT_OBJS += $(CLIENT_SRC_PATH)/client_datahandle.o
CLIENT_OBJS += $(CLIENT_SRC_PATH)/client_humiture_pthread.o
CLIENT_OBJS += $(CLIENT_SRC_PATH)/client_motor_pthread.o
CLIENT_OBJS += $(CLIENT_SRC_PATH)/client_timer_pthread.o
CLIENT_OBJS += $(CLIENT_SRC_PATH)/client_sendmessage_pthread.o
CLIENT_OBJS += $(CLIENT_SRC_PATH)/client_weight_pthread.o

CFLAGS += -g -Wall
CFLAGS += -I ./include
CFLAGS += -I ../common/include/


LDFLAGS += -lpthread 
LDFLAGS += -L$(LD_COMMON_PATH) -lcommon

export CC AR

all:$(COMMONDIRS) $(CLIENT_TARGET)

$(COMMONDIRS) : ECHO

	make -C $@

$(CLIENT_TARGET) : $(CLIENT_OBJS)

	$(CC) $^ -o $@  $(LDFLAGS) 

ECHO:
	@echo $(COMMONDIRS)

%.o : %.c
	$(CC) -c $< $(CFLAGS) -o $@
 
install:
	cp $(CLIENT_TARGET) /mnt/d/nfs/feed/ -rf

clean:
	rm $(SERVER_TARGET) $(CLIENT_TARGET) $(CLIENT_OBJS) -rf
	make -C $(COMMON_PATH) clean