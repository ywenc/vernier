#ifndef SIGNAL_SAFE_SEMAPHORE_HH
#define SIGNAL_SAFE_SEMAPHORE_HH

// A basic semaphore built on sem_wait/sem_post
// post() is guaranteed to be async-signal-safe
class SignalSafeSemaphore {
#ifdef __APPLE__
    dispatch_semaphore_t sem;
#else
    sem_t sem;
#endif

    public:

    SignalSafeSemaphore(unsigned int value = 0) {
#ifdef __APPLE__
        sem = dispatch_semaphore_create(value);
#else
        sem_init(&sem, 0, value);
#endif
    };

    ~SignalSafeSemaphore() {
#ifdef __APPLE__
        dispatch_release(sem);
#else
        sem_destroy(&sem);
#endif
    };

    void wait() {
#ifdef __APPLE__
        dispatch_semaphore_wait(sem, DISPATCH_TIME_FOREVER);
#else
        // Use sem_timedwait so that we get a crash instead of a deadlock for
        // easier debugging
        auto ts = (TimeStamp::Now() + TimeStamp::from_seconds(5)).timespec();

        int ret;
        do {
            ret = sem_wait(&sem);
        } while (ret && errno == EINTR);
        assert(ret == 0);
#endif
    }

    void post() {
#ifdef __APPLE__
        dispatch_semaphore_signal(sem);
#else
        sem_post(&sem);
#endif
    }
};

#endif
