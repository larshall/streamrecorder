#ifndef SCOPEDLOCK_H
#define SCOPEDLOCK_H

#include <pthread.h>

class ScopedLock
{
    private:
        pthread_mutex_t mutex;
    public:
        ScopedLock();
};

#endif
