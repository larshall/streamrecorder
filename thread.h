#ifndef THREAD_H
#define THREAD_H

#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include "scopedlock.h"

class Thread
{
    private:
        pthread_t thread;
        static void *thread_function(void *param);
        int tid;
        bool running;
        Mutex mutex;

    public:
        Thread();
        virtual void run() = 0;
        virtual ~Thread() = 0;
        bool start();

        void setRunning(bool b);
        bool isRunning();
};

#endif
