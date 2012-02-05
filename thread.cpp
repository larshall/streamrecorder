#include "thread.h"

Thread::Thread()
{
    running = false;
}

void *Thread::thread_function(void *param)
{
    Thread *t = (Thread*)param;
    if(t != NULL)
    {
        t->run();
        t->thread = 0;
    }
    return NULL;
}

bool Thread::start()
{
    bool ret = true;
    int res = pthread_create(&thread, NULL, thread_function, this);
    if (res == 0)
    {
        fprintf(stderr, "Thread started: %lu\n", (uint64_t) thread) ;
        pthread_detach(tid);
    }
    else
    {
        fprintf(stderr, "Cannot create thread");
        ret = false;
    }

    return true;
}

void Thread::setRunning(bool b)
{
    ScopedLock lock(&mutex);
    running = b;
}

bool Thread::isRunning()
{
    ScopedLock lock(&mutex);
    return running;
}
