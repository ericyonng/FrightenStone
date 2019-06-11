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
 * @file  : TestSystemUtil.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/6/10
 * @brief :
 * 
 *
 * 
 */
#ifndef __Test_TestTestSystemUtil_H__
#define __Test_TestTestSystemUtil_H__
#pragma once
#include "stdafx.h"

class TestSysTemUtil
{
public:
    static void Run()
    {
        // 内存情况
        std::cout << "cur process avail mem size:" << fs::SystemUtil::GetAvailPhysMemSize() << std::endl;
        std::cout << "cur process total mem size: " << fs::SystemUtil::GetTotalPhysMemSize() << std::endl;
        std::cout << "cur process mem in used: " << fs::SystemUtil::GetMemoryLoad() << std::endl;
        // 当前进程
        fs::FS_String path;
        fs::SystemUtil::GetProgramPath(true,path);
        std::cout <<"current process id:"<<fs::SystemUtil::GetProcessId()<< " current process path:" << path << std::endl;

//         // 遍历进程
//         auto hProcModule = fs::SystemUtil::CreateProcessSnapshot();
//         auto nPid = fs::SystemUtil::GetFirstProcessPid(hProcModule);
//         bool bFirst = true;
//         fs::FS_String pachCache;
//         for(; bFirst ? bFirst : (nPid > 0); nPid = fs::SystemUtil::GetNextProcessPid(hProcModule))
//         {
//             bFirst = false;
//             pachCache.Clear();
//             if(!fs::SystemUtil::GetProgramPath(false, nPid, pachCache))
//                 continue;
// 
//             std::cout << "process id:" << nPid << " path:" << pachCache << std::endl;
//         }
        std::cout << fs::SystemUtil::IsProcessExist(fs::FS_String("TestSuit.exe")) << std::endl;
    }
};
#endif
