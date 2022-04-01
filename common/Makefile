#生成通用库  common.so

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

#编译时使用LIBRARY_PATH
#$(shell export LIBRARY_PATH=$LIBRARY_PATH:$(pwd))


COMMON_SRC_PATH = ./src

COMMON_TARGET = libcommon.a

COMMON_OBJS += $(COMMON_SRC_PATH)/protocol.o
COMMON_OBJS += $(COMMON_SRC_PATH)/network.o
COMMON_OBJS += $(COMMON_SRC_PATH)/queue.o
COMMON_OBJS += $(COMMON_SRC_PATH)/debug.o
COMMON_OBJS += $(COMMON_SRC_PATH)/lstLib.o
COMMON_OBJS += $(COMMON_SRC_PATH)/serial.o
COMMON_OBJS += $(COMMON_SRC_PATH)/mcu_protocol.o


CFLAGS += -g -Wall
CFLAGS += -I ./include

#LDFLAGS += -lpthread


all:$(COMMON_TARGET)

#	make -C ../
#	make -C ../../common/src/

$(COMMON_TARGET) : $(COMMON_OBJS)
#生成静态库
#	$(CC) $^ -o $@  $(LDFLAGS) -shared -fpic
#	@echo `cp $(COMMON_TARGET) ../client`
	$(AR) -cr $@ $^ 
#	@echo `export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$(pwd)`
#	@echo `cp $(COMMON_TARGET) /usr/lib/`

%.o : %.c

	$(CC) -c $< $(CFLAGS) -o $@
 
install:
	cp $(COMMON_TARGET) /mnt/d/nfs/feed/ -rf

clean:
	rm $(COMMON_TARGET) $(COMMON_OBJS) -rf
