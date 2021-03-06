#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include "log.h"
#include "sock.h"

CSock::CSock()
{
	m_fd = -1;
}

CSock::CSock(int domain, int type)
{
	if(open(domain, type) < 0)
		throw errno;
}

CSock::~CSock()
{
	if(m_fd != -1)
	{
		//KL_SYS_INFOLOG("close socket fd: %d", m_fd);
		close(m_fd);
		m_fd = -1;
	}
}

int CSock::open(int domain, int type)
{
	m_fd = socket(domain, type, 0);
	if(m_fd == -1)
	{
		KL_SYS_ERRORLOG("file: "__FILE__", line: %d, " \
			"create socket failed, err: %s", \
			__LINE__, strerror(errno));
	}
	return m_fd;
}

int CSock::getsocket() const
{
	return m_fd;
}

int CSock::getlocaladdr(CInetAddr *paddr)
{
	struct sockaddr_in sockAddr;
	socklen_t nlen = sizeof(sockAddr);

	if(paddr == NULL)
		return -1;

	if(getsockname(m_fd, (struct sockaddr*)&sockAddr, \
		&nlen) == -1)
	{
		KL_SYS_ERRORLOG("file: "__FILE__", line: %d, " \
			"get local address failed, err: %s", \
			__LINE__, strerror(errno));
		return -1;
	}

	paddr->setsockaddr(sockAddr);
	return 0;
}

int CSock::getpeeraddr(CInetAddr *paddr)
{
	struct sockaddr_in sockAddr;
	socklen_t nlen = sizeof(sockAddr);

	if(paddr == NULL)
		return -1;

	if(getpeername(m_fd, (struct sockaddr*)&sockAddr, \
		&nlen) == -1)
	{
		KL_SYS_ERRORLOG("file: "__FILE__", line: %d, " \
			"get peer address failed, err: %s", \
			__LINE__, strerror(errno));
		return -1;
	}

	paddr->setsockaddr(sockAddr);
	return 0;
}

int CSock::setnonblocking()
{
	int opt;

	opt = fcntl(m_fd, F_GETFL, 0);
	if(opt < 0)
	{
		errno = errno != 0 ? errno : EINVAL;
		KL_SYS_ERRORLOG("file: "__FILE__", line: %d, " \
			"fcntl F_GETFL failed, err: %s", \
			__LINE__, strerror(errno));
		return errno;
	}
	opt = opt | O_NONBLOCK;
	if(fcntl(m_fd, F_SETFL, opt) != 0)
	{
		errno = errno != 0 ? errno : EINVAL;
		KL_SYS_ERRORLOG("file: "__FILE__", line: %d, " \
			"fcntl F_SETFL failed, err: %s", \
			__LINE__, strerror(errno));
		return errno;
	}
	return 0;
}

int CSock::setblocking()
{
	int opt;

	opt = fcntl(m_fd, F_GETFL, 0);
	if(opt < 0)
	{
		errno = errno != 0 ? errno : EINVAL;
		KL_SYS_ERRORLOG("file: "__FILE__", line: %d, " \
			"fcntl F_GETFL failed, err: %s", \
			__LINE__, strerror(errno));
		return errno;
	}
	if(opt & O_NONBLOCK){
		if(fcntl(m_fd, F_SETFL, opt & ~O_NONBLOCK) == -1)
		{
			errno = errno != 0 ? errno : EINVAL;
			KL_SYS_ERRORLOG("file: "__FILE__", line: %d, " \
				"fcntl F_SETFL failed, err: %s", \
				__LINE__, strerror(errno));
			return errno;
		}
	}
	return 0;
}