#ifndef KL_COMMON_MSG_QUEUE_H_
#define KL_COMMON_MSG_QUEUE_H_

#include <queue>
#include <errno.h>
#include <string.h>
#include "log.h"
#include "common_protocol.h"
class CMutex;
class CEvent;
class CMsgQueue
{
public:
	CMsgQueue();
	~CMsgQueue();

	void push_msg(pkg_message *pkg_msg_ptr);
	pkg_message* get_msg();
private:
	CMsgQueue(const CMsgQueue&);
	CMsgQueue& operator=(const CMsgQueue&);

	void push(pkg_message *pkg_msg_ptr);
	pkg_message* pop();
private:
	CMutex *m_ppush_mutex;
	CMutex *m_ppop_mutex;
	CEvent *m_pmsg_event;
	std::queue<pkg_message*> m_msg_queue;
};

/*
 * @description: CMsgQueueArr obj's function is to contain
				 msg_queue_count CMsgQueue objs
 */
class CMsgQueueArr
{
public:
	CMsgQueueArr(int msg_queue_count) : \
		m_msg_queue_count(msg_queue_count), \
		m_queue_robin(0)
	{
		typedef CMsgQueue* CMsgQueuePtr;
		int msg_queue_curr;
		try
		{
			m_ppmsg_queue = new CMsgQueuePtr[m_msg_queue_count];
		}
		catch(std::bad_alloc)
		{
			m_ppmsg_queue = NULL;
			KL_SYS_ERRORLOG("file: "__FILE__", line: %d, " \
				"no more memory to create msg queue array", \
				__LINE__);
			throw ENOMEM;
		}

		for(msg_queue_curr = 0; msg_queue_curr < m_msg_queue_count; \
			msg_queue_curr++)
		{
			try
			{
				m_ppmsg_queue[msg_queue_curr] = new CMsgQueue();
			}
			catch(std::bad_alloc)
			{
				m_ppmsg_queue[msg_queue_curr] = NULL;
			}
			catch(int errcode)
			{
				KL_SYS_ERRORLOG("file: "__FILE__", line: %d, " \
					"call CMsgQueue constructor failed, err: %s", \
					__LINE__, strerror(errcode));
				throw errcode;
			}
			if(m_ppmsg_queue[msg_queue_curr] == NULL)
			{
				KL_SYS_ERRORLOG("file: "__FILE__", line: %d, " \
					"no more memory to create msg queue", \
					__LINE__);
				throw ENOMEM;
			}
		}
	}

	~CMsgQueueArr()
	{
		int msg_queue_curr;
		for(msg_queue_curr = 0; msg_queue_curr < m_msg_queue_count; \
			msg_queue_curr++)
		{
			delete m_ppmsg_queue[msg_queue_curr];
			m_ppmsg_queue[msg_queue_curr] = NULL;
		}

		if(m_ppmsg_queue != NULL)
		{
			delete [] m_ppmsg_queue;
			m_ppmsg_queue = NULL;
		}
	}

	/*
	 * @description: getmsgqueuebyrobin choose and return a msg queue 
					 in msg queue array though the mode of round robin
	 */
	CMsgQueue* getmsgqueuebyrobin()
	{
		CMsgQueue *msg_queue_ptr;
		msg_queue_ptr = m_ppmsg_queue[m_queue_robin++];
		if(m_queue_robin == m_msg_queue_count)
		{
			m_queue_robin = 0;
		}
		return msg_queue_ptr;
	}
#ifdef _DEBUG
public:
#else
private:
#endif //_DEBUG
	CMsgQueue **m_ppmsg_queue;
	int m_queue_robin;
	int m_msg_queue_count;
};

#endif //KL_COMMON_MSG_QUEUE_H_