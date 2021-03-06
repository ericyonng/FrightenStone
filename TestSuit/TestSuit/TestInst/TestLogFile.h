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
 * @file  : TestLogFile.h
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2019/5/24
 * @brief :
 * 
 *
 * 
 */
#ifndef __Test_TestLogFile_H__
#define __Test_TestLogFile_H__
#pragma once
#include "stdafx.h"

class TestLogFile
{
public:
    static void Run()
    {
        std::cout << "TestLogFile" << std::endl;
        fs::TimeUtil::SetTimeZone();
        fs::LogFile logFile;
        logFile.Open("./log2.log", true, "ab+", true);
        fs::FS_String logStr = "hello test log file";
        logFile.Write(logStr.c_str(), logStr.GetLength());
        logFile.Flush();

        // ����
        fs::Time nowTime;
        nowTime.FlushTime(fs::Time::Now().AddDays(1).GetMicroTimestamp());

        if(logFile.IsDayPass(nowTime))
            logFile.Reopen(&nowTime);

        fs::FS_FileUtil::ResetFileCursor(*static_cast<FILE *>(logFile));
        fs::FS_String strRead;
        logFile.ReadOneLine(strRead);
        std::cout << strRead << std::endl;
        logFile.PartitionFile(false, &nowTime);
    }
};
#endif
