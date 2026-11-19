#ifndef NEWMINHH_LOG_H
#define NEWMINHH_LOG_H

#ifndef __ASSEMBLER__

#include <stdio.h>
#include <stdlib.h>

static inline int nm_log_debug_enabled(void) {
    const char *v = getenv("NEWMINHH_DEBUG");
    return (v != NULL && v[0] != '\0' && v[0] != '0');
}

#define NM_LOG_ERROR(mod, fmt, ...) \
    fprintf(stderr, "[newminhh:%s] ERROR: " fmt "\n", (mod), ##__VA_ARGS__)

#define NM_LOG_WARN(mod, fmt, ...) \
    fprintf(stderr, "[newminhh:%s] WARN: " fmt "\n", (mod), ##__VA_ARGS__)

#define NM_LOG_INFO(mod, fmt, ...)                                      \
    do {                                                                \
        if (nm_log_debug_enabled()) {                                   \
            fprintf(stderr, "[newminhh:%s] INFO: " fmt "\n",            \
                    (mod), ##__VA_ARGS__);                              \
        }                                                               \
    } while (0)

#endif /* !__ASSEMBLER__ */

#endif /* NEWMINHH_LOG_H */
