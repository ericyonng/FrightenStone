/*!
 * MIT License
 *
 * Copyright (c) 2019 ericyonng<120453674@qq.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 * @file  : ForAllMacro.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/5/24
 * @brief :
 * 
 *
 * 
 */
#ifndef __Base_Common_BaseDefs_Macro_MacroDefs_ForAll_ForAllMacro_H__
#define __Base_Common_BaseDefs_Macro_MacroDefs_ForAll_ForAllMacro_H__


#pragma once

// defs ...

#undef FS_NAMESPACE_BEGIN
#undef FS_NAMESPACE_END
#define FS_NAMESPACE_BEGIN namespace fs {
#define FS_NAMESPACE_END }

#undef DISABLE_COPY_ASSIGN_MOVE
#define DISABLE_COPY_ASSIGN_MOVE(cls)                   \
cls &operator=(const cls &obj) = delete;                \
cls(const cls &obj) = delete;                           \
cls &operator=(cls &&right) = delete;                   \
cls(cls &&right) = delete;

#undef DISABLE_COPY
#define DISABLE_COPY(cls)                               \
cls(const cls &obj) = delete;

#undef FsDelete
#define FsDelete(p)                                     \
delete p

#undef FsSafeDelete
#define FsSafeDelete(p)                                 \
if(p){ delete p;                                        \
p = NULL;}

#undef FsSafeMultiDelete
#define FsSafeMultiDelete(p)                                 \
if(p){ delete []p;                                        \
p = NULL;}

// malloc/free
#define Fs_Malloc(type, size)             (reinterpret_cast<type *>(malloc(size)))
#define Fs_Calloc(type, size)             (reinterpret_cast<type *>(calloc(size, 1)))
#define Fs_Realloc(type, memblock, size)  (reinterpret_cast<type *>(realloc((memblock), (size))))
#define Fs_Free(memblock)                 (free(memblock))
#define Fs_SafeFree(memblock)        \
    do {                            \
        if (LIKELY(memblock)) {     \
            Fs_Free(memblock);      \
            (memblock) = NULL;      \
        }                           \
    } while(0)                      \

#ifndef INFINITE
#define INFINITE    0xFFFFFFFF
#endif

#undef NULL
#define NULL nullptr

#undef MAX_NAME_LEN
#define MAX_NAME_LEN 64

#undef MAX_PWD_LEN
#define MAX_PWD_LEN 128

#undef MAX_CEIL_WIDE
#define MAX_CEIL_WIDE 16

#undef DOUBLE_FMT_STR
#define DOUBLE_FMT_STR  "%.16lf"

#undef FLOAT_FMT_STR
#define FLOAT_FMT_STR  "%.8lf"

// The likely & unlikely builtin expect macro.
#ifdef __GNUC__
#ifndef LIKELY
#define LIKELY(x) __builtin_expect(!!(x), 1)
#endif
#ifndef UNLIKELY
#define UNLIKELY(x) __builtin_expect(!!(x), 0)
#endif
#else
#ifndef LIKELY
#define LIKELY(x) (x)
#endif
#ifndef UNLIKELY
#define UNLIKELY(x) (x)
#endif
#endif // __GNUC__

// c style
#ifdef _WIN32

#define __Fs_FmtArgs_WIN32(fmt, buf, len)                                    \
    do {                                                                         \
        int &___len = (len);                                                     \
        char *&___buf = (buf);                                                   \
                                                                                 \
        if (UNLIKELY(!(fmt))) {                                                  \
            ___len = 0; ___buf = NULL;                                           \
            break;                                                               \
        }                                                                        \
                                                                                 \
        va_list ___ap;                                                           \
                                                                                 \
        int ___bufSize = 1024; ___len = 0;                                       \
        ___buf = Fs_Malloc(char, ___bufSize + 1);                              \
        while (true) {                                                           \
            va_start(___ap, fmt);                                                \
            ___len = ::vsnprintf_s(___buf, ___bufSize, _TRUNCATE, (fmt), ___ap); \
            va_end(___ap);                                                       \
                                                                                 \
            if (___len >= 0)                                                     \
                break;                                                           \
                                                                                 \
            ___bufSize <<= 1;                                                    \
            ___buf = Fs_Realloc(char, ___buf, ___bufSize + 1);                 \
        }                                                                        \
        ___buf[___len] = '\0';                                                   \
    } while (0)                                                                  \

#else

#define __Fs_FmtArgs_NoWIN32(fmt, buf, len)                    \
    do {                                                            \
        int &___len = (len);                                        \
        char *&___buf = (buf);                                      \
                                                                    \
        if (UNLIKELY(!(fmt))) {                                     \
            ___len = 0; ___buf = NULL;                              \
            break;                                                  \
        }                                                           \
                                                                    \
        va_list ___ap;                                              \
                                                                    \
        int ___bufSize = 1024; ___len = 0;                          \
        ___buf = Fs_Malloc(char, ___bufSize);                     \
        while (true) {                                              \
            va_start(___ap, (fmt));                                 \
            ___len = ::vsnprintf(___buf, ___bufSize, (fmt), ___ap); \
            va_end(___ap);                                          \
                                                                    \
            /* Workded, break */                                    \
            if (___len > -1 && ___len < ___bufSize)                 \
                break;                                              \
                                                                    \
            /* Try again with more space */                         \
            if (LIKELY(___len > -1)) /* glibc 2.1 and later */      \
                ___bufSize = ___len + 1;                            \
            else /* glibc 2.0 */                                    \
                ___bufSize <<= 1;                                   \
                                                                    \
            ___buf = Fs_Realloc(char, ___buf, ___bufSize);          \
        }                                                           \
        ___buf[___len] = '\0';                                      \
    } while(0)                                                      \

#endif

// c style
#ifdef _WIN32
#define __FS_BuildFormatStr_   __Fs_FmtArgs_WIN32
#else
#define __FS_BuildFormatStr_   __Fs_FmtArgs_NoWIN32
#endif

//½ûÓÃ¿½±´¹¹Ôì
#undef  NO_COPY
#define NO_COPY(x)\
private:\
        x (const x&);   \
        const x& operator=(const x&);

#undef DISABLE_STACK_CREATE
#define DISABLE_STACK_CREATE(x)\
private:\
    x();

#endif // !__Base_Common_BaseDefs_Macro_MacroDefs_ForAll_ForAllMacro_H__
