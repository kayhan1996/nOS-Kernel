#ifndef __ERRORS_H__
#define __ERRORS_H__

/* Waits for expression then timesout after set time */
#define TIMEOUT(expr)                                                          \
    {                                                                          \
        int cnt = 1000000;                                                     \
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
        printf("Trying\n");                                                    \
        int count = n;                                                         \
        while (count && expr) {                                                \
            printf("Retrying\n");                                              \
            delay(10000000);                                                   \
            count--;                                                           \
        }                                                                      \
        if (count <= 0) {                                                      \
            printf("Failure\n");                                               \
            return -1;                                                         \
        } else                                                                 \
            printf("Success\n");                                               \
    }

#endif /* __ERRORS_H__ */