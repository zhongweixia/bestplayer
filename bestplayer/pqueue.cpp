#include "pqueue.h"


pqueue::pqueue() //���캯��
{
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&full, NULL);
	pthread_cond_init(&empty, NULL);
}

pqueue::~pqueue()
{
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&full);
	pthread_cond_destroy(&empty);
}

int pqueue::push(void* obj)
{
	lock();
	while (nodecount >= maxsize) //������
	{
		notify_consume();
		wait_product();
		_sleep(1);
	}
	
	node* pNode = new node();
	pNode->obj = obj;
	if (nodecount > 0)
	{
		last_node->next = pNode;
		last_node = pNode;
	}
	else
	{
		first_node = pNode;
		first_node->next = pNode;
		last_node = pNode;
	}

	nodecount++;
	notify_consume();
	unlock();
	return 0;
}

void* pqueue::dequeue()
{
	lock();

	while (nodecount <= minsize && (nodecount <= 0 || enum_state != FLUSHED))  //����Ϊ��
	{
		notify_product();
		wait_consume();
		_sleep(1);
	}

	void* returnobj = first_node->obj;
	first_node = first_node->next;
	nodecount--;
	notify_product();
	unlock();
	return returnobj;
}

void pqueue::lock()
{
	pthread_mutex_lock(&mutex);
}

void pqueue::unlock()
{
	pthread_mutex_unlock(&mutex);
}

void pqueue::wait_product()  //�������������ߵȴ�
{
	pthread_cond_wait(&full, &mutex);
}
void pqueue::wait_consume()  //���пգ������ߵȴ�
{
	pthread_cond_wait(&empty, &mutex);
}
void pqueue::notify_product()  //���в�Ϊ��ʱ��֪ͨ������
{
	pthread_cond_signal(&full);
}
void pqueue::notify_consume()  //���в�Ϊ��ʱ��֪ͨ������
{
	pthread_cond_signal(&empty);
}

bool pqueue::isfull()
{
	return  nodecount >= maxsize;;
}

bool pqueue::isempty()
{
	return nodecount == 0;
}

void* pqueue::get(int index)
{
	lock();
	while (nodecount <= minsize && (nodecount <= 0 || enum_state != FLUSHED))  //����Ϊ��
	{
		notify_product();
		wait_consume();
		_sleep(1);
	}

	node* ppoint = first_node;
	for (int i = 0; i < index; i++)
	{
		ppoint = ppoint->next;
	}
	unlock();
	return ppoint->obj;
}

void pqueue::flush()
{
	enum_state = FLUSHED;
}

int pqueue::getmaxsize()
{
	return maxsize;
}

void pqueue::setmaxsize(int size)
{
	maxsize = size - 1;
}

void pqueue::setminsize(int size)
{
	minsize = size - 1;
}

int pqueue::getlength()
{
	return nodecount;
}

state pqueue::getstate()
{
	return enum_state;
}

