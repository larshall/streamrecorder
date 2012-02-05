#include "scopedlock.h"


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
