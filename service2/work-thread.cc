#include "service2/thread-class.h"


void ASRWorkThread::Run()
{
	pthread_t tid = GetThreadId();
	// thread synchronization lock
	pthread_mutex_t *pthread_pool_mutex = thread_pool->GetPthreadMutex();
	// thread condition lock
	pthread_cond_t *pthread_pool_cond = thread_pool->GetPthreadCond();

	while(1)
	{
		pthread_mutex_lock(pthread_pool_mutex);
		while(thread_pool->GetTaskSize() == 0 && !thread_pool->Shutdown())
		{
			LOG << "Thread " << tid << " wait task.";
			pthread_cond_wait(pthread_pool_cond, pthread_pool_mutex);
		}
		if(thread_pool->Shutdown())
		{
			pthread_mutex_unlock(pthread_pool_mutex);
			LOG << "Thread " << tid << " will exit.";
			pthread_exit(NULL);
		}
		LOG << "tid " << tid << " run.";

		TaskBase *task = thread_pool->GetTask();

		thread_pool->MoveToBusy(tid); // add busy list
		pthread_mutex_unlock(pthread_pool_mutex);

		task->Run(NULL);

		pthread_mutex_lock(pthread_pool_mutex);
		thread_pool->MoveToIdle(tid); // add idle list
		pthread_mutex_unlock(pthread_pool_mutex);
	}
}
