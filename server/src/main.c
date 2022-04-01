/*************************************************************************
    > File Name: main.c
    > 作者:YJK
    > Mail: 745506980@qq.com
    > Created Time: 2021年07月19日 星期一 10时19分27秒
 ************************************************************************/

#include "myhead.h"
#include "server.h"
#include "server_heartbeat_handle.h"
#include "server_sendmessage_thread.h"

int main(int argc,char *argv[])
{
		
#if 0
	char buf[1024] = {"hello world"};
    char buf2[] = {"ni hao"};
	//memcpy(&buf[11], buf2, 6);
	memcpy(buf + 11, buf2, 6);


	printf("%s\n", buf);

	DEBUG("%ld\n", sizeof(BOOL));

	int fd = 1;
	unsigned char id[] = "hello";
	unsigned char target_id[] = "lalal";
//	link_append(app_head_node_point, app_end_node_point, fd, id, strlen(id));
//	link_append(feed_head_node_point, feed_end_node_point, fd, target_id, strlen(id));

//	pri_all_node(app_head_node_point);
//	link_delete(app_head_node_point, fd);
	facilty_information_handle(fd + 1, target_id, FEEDER_TO_SERVER, strlen(target_id));
	facilty_information_handle(fd, id, APP_TO_SERVER, strlen(id));

	update_targetid_node(app_head_node_point, fd, target_id, strlen(target_id));
	pri_all_node(app_head_node_point);
	pri_all_node(feed_head_node_point);
	

	
#endif
	server_heartbeat_handle_pthead();
	server_sendmessage_pthead();

#if 1
	server_init();
	server_loop();
#endif

 	time_t ti;
	struct tm * tim;
	time(&ti);
	tim = localtime(&ti);

	printf("year:%d \n", tim->tm_year + 1900);


	return 0;
}
