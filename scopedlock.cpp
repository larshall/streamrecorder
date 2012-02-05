#include "scopedlock.h"

ScopedLock::ScopedLock()
{
    pthread_mutex_init(&mutex, NULL);
}
