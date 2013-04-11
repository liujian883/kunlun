#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <errno.h>
#include "log.h"
#include "base_server.h"
#include "msg_parser.h"
#include "common_types.h"
#include "common_protocol.h"
#include "sockstream.h"
#include "base_server_conf.h"
#ifdef _DEBUG
#include <assert.h>
#endif //_DEBUG

#define gettid() syscall(SYS_gettid)

#define KL_COMMON_PROTOCOL_MSG_TEST1 0
//#define KL_COMMON_PROTOCOL_MSG_TEST2 1
//#define KL_COMMON_PROTOCOL_MSG_TEST3 2
#define KL_COMMON_PROTOCOL_MSG_SERVER_RESP 3
typedef pkg_header base_msg_header;

class CBaseMsgParser : public CMsgParser
{
public:
	int parse_msg(pkg_message* pkg_msg_ptr);
	int msg_test1_handle(pkg_message* pkg_msg_ptr);
	//int msg_test2_callback(pkg_message* pkg_msg_ptr);
	//int msg_test3_callback(pkg_message* pkg_msg_ptr);
};

int CBaseMsgParser::parse_msg(pkg_message* pkg_msg_ptr)
{
#ifdef _DEBUG
	assert(pkg_msg_ptr);
#endif //_DEBUG
	byte msg_cmd = *(pkg_msg_ptr->pkg_ptr);
	switch(msg_cmd)
	{
	case KL_COMMON_PROTOCOL_MSG_TEST1:
		return msg_test1_handle(pkg_msg_ptr);
	default:
		KL_SYS_WARNNINGLOG("file: "__FILE__", line: %d, " \
			"catch undefined msg...", \
			__LINE__);
		return -1;
	}
}

int CBaseMsgParser::msg_test1_handle(pkg_message* pkg_msg_ptr)
{
	int res;
	char resp_body[KL_COMMON_BUF_SIZE];
	CSockStream *psock_stream;
	base_msg_header msg_resp_header;

	psock_stream = new CSockStream(pkg_msg_ptr->sock_stream_fd, false);
	if(psock_stream == NULL)
	{
		KL_SYS_ERRORLOG("file: "__FILE__", line: %d, " \
			"no more memory to create sock stream obj", \
			__LINE__);
		res = -1;
		goto error_handle;
	}
	printf("msg_test1_callback(thread: %ld) data : %s, and server(thread: %ld) response\n", \
		gettid(), (char*)(pkg_msg_ptr->pkg_ptr + 2), gettid());

	msg_resp_header.cmd = KL_COMMON_PROTOCOL_MSG_SERVER_RESP;
	msg_resp_header.status = 0;
	snprintf(resp_body, KL_COMMON_BUF_SIZE, \
		"msg_test1_callback(thread: %ld) response the request", \
		gettid());
	CSERIALIZER::long2buff(strlen(resp_body), msg_resp_header.pkg_len);
	if((res = psock_stream->stream_send(&msg_resp_header, \
		sizeof(base_msg_header))) <= 0)
	{
		KL_SYS_ERRORLOG("file: "__FILE__", line: %d, " \
			"send responsed msg header failed, err: %s", \
			__LINE__, strerror(errno));
		res = -1;
		goto error_handle;
	}

	if((res = psock_stream->stream_send(resp_body, strlen(resp_body))) <= 0)
	{
		KL_SYS_ERRORLOG("file: "__FILE__", line: %d, " \
			"send responsed msg body failed, err: %s", \
			__LINE__, strerror(errno));
		res = -1;
		goto error_handle;
	}
	res = 0;
error_handle:
	//consume a msg, so delete the msg pkg
	delete pkg_msg_ptr;
	pkg_msg_ptr = NULL;

	if(psock_stream != NULL)
	{
		delete psock_stream;
		psock_stream = NULL;
	}
	return res;
}

int main(int argc, char *argv[])
{
	if(argc != 2){
		printf("error args, Usage %s <port>\n", argv[0]);
		return -1;
	}

	int nbind_port;
	CBaseServerConf base_server_conf;

	nbind_port = atoi(argv[1]);
	if(nbind_port < 0){
		return -1;
	}

	base_server_conf.bind_host = NULL;
	base_server_conf.nbind_port = nbind_port;
	base_server_conf.nlog_level = 4; //DEBUG level
	base_server_conf.nthread_stack_size = 1 * 1024 * 1024;
	base_server_conf.ntimeout = 5;
	base_server_conf.nwork_thread_count = 10;
	base_server_conf.sys_log_path = "./kunlun_sys.log";
	
	CBaseServer *pbase_server;
	CMsgParser *pmsg_parser;
	pmsg_parser = new CBaseMsgParser();
	pbase_server = new CBaseServer(base_server_conf, pmsg_parser);
	if(pbase_server == NULL)
	{
		printf("file: "__FILE__", line: %d, " \
			"no more memory to create base server", \
			__LINE__);
		return ENOMEM;
	}
	KL_SYS_NOTICELOG("kunlun(common test) base server has been started, " \
					 "and listen on port: %d. All copyrights are reserved " \
					 "by Leslie Wei.  Connect with me by e-mail: leslieyuchen@gmail.com", \
					 base_server_conf.nbind_port);
	pbase_server->run();
	KL_SYS_NOTICELOG("epollserver exit...");
	delete pbase_server;
	return 0;
}