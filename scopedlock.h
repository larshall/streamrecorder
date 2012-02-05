#ifndef SCOPEDLOCK_H
#define SCOPEDLOCK_H

#include <pthread.h>
#include <stdio.h>

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

#endif
