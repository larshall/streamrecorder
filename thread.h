#ifndef THREAD_H
#define THREAD_H

class Thread
{
    public:
        Thread();
        virtual void run() = 0;
        virtual ~Thread() = 0;
};

#endif
