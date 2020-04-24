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
 * @file  : TestMemAlign.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/11/18
 * @brief :
 * 
 *
 * 
 */
#pragma once

#ifndef __Test_TestMemAlign_H__
#define __Test_TestMemAlign_H__

#pragma once

#include "stdafx.h"

template<typename ObjType>
class TestMemAlignObj
{
public:
    ObjType _obj;
    fs::FS_String _str;
};

class TestMemAlignHeader
{
public:
    bool _isInPool;
    char _res1;
    char _res2;
    char _res3;
};

class TestMemAlign
{
public:
    static void Run()
    {
        size_t alignBeforeSize = sizeof(TestMemAlignObj<Int64>);
        size_t alignSize = __FS_MEMORY_ALIGN__(sizeof(TestMemAlignObj<Int64>));
        size_t offSetAlignSize = alignSize + sizeof(TestMemAlignHeader);
        std::cout << "alignBeforeSize=" << alignBeforeSize << " alignSize=" << alignSize << " offSetAlignSize" << offSetAlignSize << std::endl;

        char *memBlock = static_cast<char *>(::malloc(offSetAlignSize));
        ::new(memBlock + sizeof(TestMemAlignHeader))TestMemAlignObj<Int64>();
        TestMemAlignHeader *ptrHeader = reinterpret_cast<TestMemAlignHeader *>(memBlock);
        ptrHeader->_isInPool = true;
        std::cout << "ptrheader=" << ptrHeader << std::endl;
        TestMemAlignObj<Int64> *obj = reinterpret_cast<TestMemAlignObj<Int64> *>(memBlock + sizeof(TestMemAlignHeader));
        obj->_obj = 654654646;
        std::string str;
        str = "dafasdlkjlflk;asdjl;f;k;;a;;;;llllddddddddddddddddddddddddddddddddddddddddddddddaldflasdjlad;";
        obj->_str = "dafasdlkjlflk;asdjl;f;k;;a;;;;llllddddddddddddddddddddddddddddddddddddddddddddddaldflasdjlad;";
        
        char *headerObj = reinterpret_cast<char *>(obj) - sizeof(TestMemAlignHeader);
        TestMemAlignHeader *realHeader = reinterpret_cast<TestMemAlignHeader *>(headerObj);
        std::cout << "isInPool?:" << realHeader->_isInPool << std::endl;
        ::free(headerObj);
        getchar();

    }
};
#endif