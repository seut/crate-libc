//
// Created by smu on 26/01/16.
//

#ifndef CRATE_LIBC_DEBUG_H
#define CRATE_LIBC_DEBUG_H

#if defined(DEBUG) && DEBUG > 0
#define DEBUG_PRINT(fmt, args...) fprintf(stderr, "DEBUG: %s:%d:%s(): " fmt, \
    __FILE__, __LINE__, __func__, ##args)
#else
#define DEBUG_PRINT(fmt, args...) /* Don't do anything in release builds */
#endif

#endif //CRATE_LIBC_DEBUG_H
