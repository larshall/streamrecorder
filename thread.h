/*
 * Copyright (C) 2012 Lars Hall
 *
 * This file is part of StreamRecorder.
 *
 * StreamRecorder is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * StreamRecorder is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with StreamRecorder.  If not, see <http://www.gnu.org/licenses/>.
 */

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
