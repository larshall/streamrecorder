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

        void setRunning(bool b);
        void setTerminate(bool b);
        bool isRunning();
};

#endif
