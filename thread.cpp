#include "thread.h"

// Mutex
Mutex::Mutex()
{
    pthread_mutex_init(&mutex, NULL);
}

Mutex::~Mutex()
{
    pthread_mutex_destroy(&mutex);
}

pthread_mutex_t *Mutex::getMutex()
{
    return &mutex;
}

// ScopedLock
ScopedLock::ScopedLock(Mutex *m) : mutex(m)
{
    int status = pthread_mutex_lock(mutex->getMutex());

    if (status != 0)
        fprintf(stderr, "exiting on mutex error status:%i", status);
}

ScopedLock::~ScopedLock()
{
    pthread_mutex_unlock(mutex->getMutex());
}

// Thread
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
        setRunning(true);
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
        setRunning(false);
    }
    else
        fprintf(stderr, "Trying to stop a thread that is already stopped\n");
}

void Thread::waitForThread()
{
    int err = pthread_join(thread,NULL);
    if(err != 0)
    {
        switch(err)
        {
            case ESRCH:
                fprintf(stderr, "Not a valid thread id");
                break;
            case EINVAL:
                fprintf(stderr, "Antother thread is already waiting\n");
                break;
            case EDEADLK:
                fprintf(stderr, "Thread is waiting for itself\n");
                break;
        }
    }
}

Thread::~Thread()
{
    if (isRunning())
        fprintf(stderr, "Warning destroying thread while it's running\n");
}
