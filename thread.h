#ifndef THREAD_H
#define THREAD_H

#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <errno.h>

class Mutex
{
    private:
        pthread_mutex_t mutex;

    public:
        Mutex();
        ~Mutex();
        pthread_mutex_t *getMutex();
};

class ScopedLock
{
    private:
        Mutex *mutex;
    public:
        ScopedLock(Mutex *m);
        ~ScopedLock();
};

class Thread
{
    private:
        pthread_t thread;
        static void *thread_function(void *param);
        int tid;
        bool running;
        bool terminate;
        Mutex mutex;

    protected:
        bool doTerminate();

    public:
        Thread();
        virtual void run() = 0;
        virtual ~Thread();
        bool start();
        void stop();
        void waitForThread();

        void setRunning(bool b);
        void setTerminate(bool b);
        bool isRunning();
};

#endif
