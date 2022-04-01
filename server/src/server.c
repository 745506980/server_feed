#include "server.h"
#include "network.h"
#include "myhead.h"
#include "protocol.h"


pthread_mutex_t linkmutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t linkappmutex = PTHREAD_MUTEX_INITIALIZER;

//APP链表头尾指针
Link_node * app_head_node_point;
Link_node * app_end_node_point;

//喂食器链表头尾指针
Link_node * feed_head_node_point;
Link_node * feed_end_node_point;

//需要先初始化链表
BOOL server_init(void)
{
	BOOL status;
	status = link_init(&app_head_node_point, &app_end_node_point);
	if (status == FALSE)
		return FALSE;
	status = link_init(&feed_head_node_point, &feed_end_node_point);
	if (status == FALSE)
		return FALSE;
	status = MsgQueue_init();
	if (status == FALSE)
		return FALSE;
	
	return TRUE;
}

int server_bind_listen_fd(void)
{
    int ret;
    int socket_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_fd == -1)
    {
        return -1;
    }
    //设置端口快速复用
    int b_reuse = 1;
	setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &b_reuse, sizeof(int));
    //bind
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(SERVER_PORT);
    addr.sin_addr.s_addr = inet_addr(SERVER_IP);
    ret = bind(socket_fd, (struct sockaddr*)&addr, sizeof(struct sockaddr));
    if (ret == -1)
    {
    	ERROR(
"bind");
        close(socket_fd);
        return -1;
    }
    //listen
    ret = listen(socket_fd, BACKLOG);
    if (ret == -1)
    {
    	ERROR("listen");
        close(socket_fd);
        return -1;
    }
    return socket_fd;
}
void server_loop(void)
{
    struct epoll_event event;
    struct epoll_event events[EPOLL_LISTEN_MAX] = {0};
    int nfds, n;
    struct sockaddr_in caddr;
    int addrlen = sizeof(struct sockaddr_in);
    int conn_fd;

    int listen_fd = server_bind_listen_fd();
    if (listen_fd == -1) {
        ERROR_EXIT("server_bind_listen");
    }
    //创建一个epoll_fd;
    int epoll_fd = epoll_create(EPOLL_LISTEN_MAX);
    if (epoll_fd == -1) {
        ERROR_EXIT("epoll_create");
    }
    //将listen_fd 注册到epoll监听队列中
    event.events = EPOLLIN;
    event.data.fd = listen_fd;
    int ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &event);
    if (ret == -1) {
        ERROR_EXIT("epoll_ctl");
    }
    //客户端链接以及数据处理
    for ( ; ; ) {
        nfds = epoll_wait(epoll_fd, events, EPOLL_LISTEN_MAX, WAITING_FORVER);
        //对已准备好的描述符进行操作
        for (n = 0; n < nfds; n++) {
            //有新的客户端连接
            if (events[n].data.fd == listen_fd) {
                conn_fd = accept(listen_fd, (struct sockaddr *)&caddr, &addrlen);
                if (conn_fd == -1) {
                    ERROR_EXIT("accept");
                }
                //将新的客户端socket_fd 加入到epoll监听队列中
                DEBUG("new client fd:%d\n", conn_fd);
                event.events =EPOLLIN | EPOLLET;    //使用ET模式
                event.data.fd = conn_fd;
                ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, conn_fd, &event);
                if (ret == -1) {
                    ERROR_EXIT("epoll_ctl");
                }
                //加入到client链表
            } else {
                //处理客户端数据
                /*do_something()*/
				//客户端ID绑定时期：
				//数据绑定使用Hash表
				DEBUG("new msg\n");
                ret = server_hande(events[n].data.fd);
                if (ret == -1) {
                    ERROR_EXIT("client_hande");
                }
            }
        }
    }//end for
    close_socketfd(listen_fd);
}


int server_hande(int client_fd)
{
    char recv_data[RECV_BUF_SIZE] = {0};
    char recv_header[PROTOCOL_HEADER_SIZE] = {0};
    dcl obj;
    //取出所有数据
    //TCP会出现粘包、截断的情况
	BOOL status;
	int ret = 1;
	memset(recv_data, 0, RECV_BUF_SIZE);
	while (ret > 0) {
		memset(recv_header, 0, PROTOCOL_HEADER_SIZE);
		memset(&obj, 0, sizeof(dcl));
		memset(recv_data, 0, RECV_BUF_SIZE);
		ret = recvmessage_header(client_fd, recv_header, PROTOCOL_HEADER_SIZE);
		if (ret == PROTOCOL_HEADER_SIZE) {
		//对包头进行解析，获取包头中的数据 如 方向、命令、数据长度；
			status = protocol_header_check(recv_header, PROTOCOL_HEADER_SIZE, &obj);
			if (status == TRUE) {
				//包头解析成功,获取数据
				status = recvmessage(client_fd, recv_data, obj.length);
				if (status == TRUE) {
				//数据获取成功，根据方向、命令、数据进行处理
					//do_something
					DEBUG("SERVER_RECV:");
					debug_protol(client_fd, obj.command);
					debug_data(recv_data, obj.length);
					data_handle(client_fd, recv_data, obj.direction, obj.command, obj.length);
				}
			}
		}
		else if( ret == 0 ){
		//客户端断开连接
			MUTEX_LOCK(&linkmutex);
			if (link_delete(app_head_node_point, client_fd) == FALSE) {
				link_delete(feed_head_node_point, client_fd);
				MUTEX_UNLOCK(&linkmutex);
			}
			MUTEX_UNLOCK(&linkmutex);
			close_socketfd(client_fd);
			DEBUG("client is close\n");
		}
		else {
			ERROR("recvmessage_header\n");
		}
	}
	return 0;
}



/*
采用epoll I/O多路复用模型
*/
/*
epoll相关API
1、 int epoll_create(int size);
创建一个epoll的句柄，size用来告诉内核要监听多少个fd。当创建玩epoll句柄后，它本身也占用一个fd
使用完毕后需要使用close()关闭
2、int epoll_ctl(int epfd, int op, int fd, struct epoll_event * event);
告诉内核需要监听什么事件，
(1)epfd epoll_create的返回值

(2)op 表示动作
    EPOLL_CTL_ADD: 注册新的fd到epfd中；
    EPOLL_CTL_MOD: 修改已经注册的fd的监听事件
    EPOLL_CTL_DEL: 从epfd中删除一个fd
(3)fd 需要监听的fd

(4)告诉内核需要监听的事件类型
    struct epoll_event {
        __uint32_t events;  //关注的事件
        epoll_data_t data;  //用户变量
    };
    typedef union epoll_data {
        void *ptr;
        int fd;
        uint32_t u32;
        uint64_t u64;
    } epoll_data_t;
    epoll_data_t 一般来说就直接使用fd了

    1)events
    events可以是以下几个宏的"或"
    读写
    EPOLLIN:    表示对应的文件描述符可以读
    EPOLLIOUT:  表示对应的文件描述符可以写
    EPOLLPRI:   表示对应的文件描述符有紧急的数据可读(这里应该表示为是有socket带外数据到来)
    异常
    EPOLLERR:   表示对应的文件描述符发生错误
    EPOLLHUP:   表示对应的文件描述符被挂断

    ET模式
    EPOLLET:    设置为ET模式(高速模式，在这种模式下，当fd从未就绪变为就绪时，内核通过epoll告诉用户，然后他会假设用户已经知道fd已就绪，并且不会再为那个fd发送更多的就绪通知，也就是只发送一次就绪通知)，
                默认为LT模式(LT缺省工作模式，在这种模式下，当fd从未就绪变为就绪时，内核通过epoll告诉用户，之后用户可以对这个就绪的fd进行I/O操作。但是如果用户不进行任何操作，该就绪事件并不会丢失，
                会重复通知，直到对该fd进行操作)

    EPOLLONESHOT:   意味着一次性监听，当监听完这次事件后，如果还需要继续监听这个fd的话，需要再次把该fd加入到epoll队列中(红黑树索引)
3、int epoll_wait(int epfd, struct epool_events * events, int maxevents, int timeout);
等待事件的发生，其中
(1)epfd epoll_create的返回值
(2)events参数是输出参数，用来从内核得到事件的集合
(3)maxevents 告诉内核本次最多收多少事件，不能大于epoll_create的size
(4)timeout 为超时事件，以ms为单位 0 意味着立刻返回 -1 意味着永久等待
返回值是需要处理的事件数目，如返回0， 则表示已超时；

*/

