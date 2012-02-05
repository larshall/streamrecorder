#include "thread.h"

Thread::Thread()
{
    running = false;
    terminate = false;
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
        pthread_detach(thread);
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

void Thread::setTerminate(bool b)
{
    ScopedLock lock(&mutex);
    terminate = b;
}

bool Thread::doTerminate()
{
    ScopedLock lock(&mutex);
    return terminate;
}

bool Thread::isRunning()
{
    ScopedLock lock(&mutex);
    return running;
}

void Thread::stop()
{
    if (isRunning())
    {
        fprintf(stderr, "Thread stopped:%lu\n", (uint64_t) thread);
        setTerminate(true);
    }
    else
        fprintf(stderr, "Trying to stop a thread that is already stopped\n");
}

Thread::~Thread()
{
    if (isRunning())
        fprintf(stderr, "Warning destroying thread while it's running\n");
}
