#ifndef __ERRORS_H__
#define __ERRORS_H__

/* Waits for expression then timesout after set time */
#define TIMEOUT(expr)                                                          \
    {                                                                          \
        int cnt = 10000;                                                       \
        while ((expr) && cnt != 0) {                                           \
            cnt--;                                                             \
        }                                                                      \
        if (expr) {                                                            \
            return -1;                                                         \
        }                                                                      \
    }

/* Retries the expression {n} times before returning failure */
#define RETRY(expr, n)                                                         \
    {                                                                          \
        log("Trying\n");                                                    \
        int count = n;                                                         \
        while (count && expr) {                                                \
            log("Retrying\n");                                              \
            delay(10);                                                         \
            count--;                                                           \
        }                                                                      \
        if (count <= 0) {                                                      \
            log("Failure\n");                                               \
            return -1;                                                         \
        } else                                                                 \
            log("Success\n");                                               \
    }

#endif /* __ERRORS_H__ */