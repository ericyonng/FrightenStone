/*!
 * MIT License
 *
 * Copyright (c) 2019 Eric Yonng<120453674@qq.com>
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
 * @file  : Log.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2019/6/12
 * @brief :
 * 
 *  添加log文件方法:
    1.需要在LogDefs.h中GENERAL_ADDLOG(Workfolder, LogFileName)
    2.LogFileType.h中添加文件枚举
    3.在virtual Int32 FS_Log::_GetLogFileIndex(Int32 logTypeEnum)中case 对应的LogFileType对应的fileindex日志文件id
    4.在LogData.h中添加LogLevel对应的枚举,并在LogData.cpp中填充枚举对应的字符值
    5.可缺省:_OutputToConsole ,_IsAllowToConsole,_SetConsoleColor,填充对应的是否允许打印在控制台,打印颜色等逻辑
    6.在ILog.h中添加新日志文件对应的打印接口
 * 
 */
#ifndef __Frame_Include_FrightenStone_Common_Log_Log_H__
#define __Frame_Include_FrightenStone_Common_Log_Log_H__
#pragma once

// 添加log方法
// defs...
// interface...
#include "FrightenStone/common/log/Interface/ILog.h"

#endif

