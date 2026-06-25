/**
 * Copyright (c) 2013-2014 Tomas Dzetkulic
 * Copyright (c) 2013-2014 Pavol Rusnak
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include <TrezorCrypto/rand.h>

#include <stddef.h>

#if defined(__APPLE__)
// iOS / macOS
    #include <TargetConditionals.h>
    #include <Security/Security.h>

#elif defined(__ANDROID__) || defined(__linux__)
// Android / Linux
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>

#if defined(__ANDROID__)
#include <android/api-level.h>
#if __ANDROID_API__ >= 28
    #define USE_GETRANDOM 1
    #include <sys/random.h>
#endif
#elif defined(__linux__)
#include <linux/version.h>
#if LINUX_VERSION_CODE >= KERNEL_VERSION(3,17,0)
    #define USE_GETRANDOM 1
    #include <sys/random.h>
#endif
#endif

#else
    #error "Unsupported platform for random32"
#endif

static int get_random_bytes(void *buf, size_t len) {
#if defined(__APPLE__)
    // iOS / macOS：使用 SecRandomCopyBytes
    if (SecRandomCopyBytes(kSecRandomDefault, len, buf) == errSecSuccess) {
        return 0;
    }
    return -1;

#elif defined(__ANDROID__) || defined(__linux__)
    size_t done = 0;

#ifdef USE_GETRANDOM
    while (done < len) {
        ssize_t n = getrandom((unsigned char*)buf + done, len - done, 0);
        if (n < 0) {
            if (errno == EINTR) {
                continue;
            }
            done = 0;
            break;
        }
        done += (size_t)n;
    }
#endif
    if (done < len) {
        int fd = open("/dev/urandom", O_RDONLY);
        if (fd < 0) return -1;

        while (done < len) {
            ssize_t n = read(fd, (unsigned char*)buf + done, len - done);
            if (n < 0) {
                if (errno == EINTR) continue;
                close(fd);
                return -1;
            }
            if (n == 0) {
                close(fd);
                return -1;
            }
            done += (size_t)n;
        }
        close(fd);
    }

    return 0;
#endif
}

uint32_t random32(void) {
    uint32_t v = 0;
    if (get_random_bytes(&v, sizeof(v)) != 0) {
        abort();
    }
    return v;
}

void random_buffer(uint8_t *buf, size_t len) {
    if (get_random_bytes(buf, len) != 0) {
        abort();
    }
}
