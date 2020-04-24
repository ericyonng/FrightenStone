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
 * @file  : CreateLogFileMacro.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/6/24
 * @brief :
 * 
 *
 * 
 */
#ifndef __Frame_Src_Common_Log_Defs_CreateLogFileMacro_H__
#define __Frame_Src_Common_Log_Defs_CreateLogFileMacro_H__

#pragma once

// log起始声明
#define DECLEAR_LOG_BEGIN(LOG_BEGIN, line, logindexbase)                                        \
enum {LOG_BEGIN = logindexbase};                                                                \
template<>                                                                                      \
struct ExcludeCommentHelper<line>                                                               \
{                                                                                               \
    enum {realvalue = logindexbase-1};                                                          \
};                                                                                              \

// 添加log文件
#define ADD_LOG(log_path, log_name, base, line)                                                 \
template<>                                                                                      \
struct ExcludeCommentHelper<line>                                                               \
{                                                                                               \
    enum {realvalue = ExcludeCommentHelper<line-1>::realvalue + 1};                             \
};                                                                                              \
static const int _##log_path##_##log_name##_ = ExcludeCommentHelper<line>::realvalue;           \
template<>                                                                                      \
struct LogInitHelper<_##log_path##_##log_name##_>                                               \
{                                                                                               \
    static bool InitLog(ILog *logMgr)                                                           \
    {                                                                                           \
        if (_##log_path##_##log_name##_ > base)                                                 \
             LogInitHelper<_##log_path##_##log_name##_-1>::InitLog(logMgr);                     \
        logMgr->CreateLogFile(_##log_path##_##log_name##_,"./"#log_path"/", #log_name".log");   \
        return true;                                                                            \
    }                                                                                           \
};

// log文件结束
#define DECLEAR_LOG_END(LOG_NUMBER, line)                                                       \
enum {LOG_NUMBER = ExcludeCommentHelper<line>::realvalue};                                      \
enum {LOG_QUANTITY = ExcludeCommentHelper<line>::realvalue + 1};

// 便利宏
#define DECLEAR_GENERAL_LOG_BEGIN() DECLEAR_LOG_BEGIN(LOG_NUM_BEGIN,  __LINE__, 0)
#define GENERAL_ADDLOG(log_path, log_name) ADD_LOG(log_path, log_name, LOG_NUM_BEGIN, __LINE__)
#define DECLEAR_GENERAL_LOG_END() DECLEAR_LOG_END(LOG_NUM_MAX, __LINE__)

#endif
