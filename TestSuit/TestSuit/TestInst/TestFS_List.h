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
 * @file  : TestFS_List.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/8/30
 * @brief :
 * 
 *
 * 
 */
#ifndef __Test_TestFS_List_H__
#define __Test_TestFS_List_H__
#pragma once
#include "stdafx.h"

class TestFS_List
{
public:
    static void Run()
    {
        fs::FS_List<char *> listmy;
        auto newBuff = new char[32];
        memset(newBuff, 0, 32);
        strcpy(newBuff, "1");
        std::cout << "empty:" << listmy.empty() << std::endl;
        listmy.push_front(newBuff);

        newBuff = new char[32];
        memset(newBuff, 0, 32);
        strcpy(newBuff, "2");
        listmy.push_back(newBuff);

        std::cout << "empty:" << listmy.empty() << std::endl;
        std::cout << "size:" << listmy.size() << std::endl;
        std::cout << "front" << listmy.front() << std::endl;
        std::cout << "back" << listmy.back() << std::endl;
        std::cout << "begin:" << listmy.begin()->_obj << std::endl;
        std::cout << "end:" << listmy.end()->_obj << std::endl;
                
        std::cout << "size after erase:" << listmy.size() << std::endl;
        std::cout << "begin:" << listmy.begin()->_obj << std::endl;
        listmy.insert_before((char *)( "wocao"), listmy.end());
        auto curNode = listmy.begin();
        while(curNode)
        {
            std::cout << "node:" << curNode->_obj << std::endl;
            curNode = curNode->_nextNode;
        }

        listmy.erase(listmy.begin()->_nextNode);
        curNode = listmy.begin();
        while(curNode)
        {
            std::cout <<"node:" << curNode->_obj << std::endl;
            curNode = curNode->_nextNode;
        }

        listmy.pop_front();
        std::cout << "after pop front:" << std::endl;

        curNode = listmy.begin();
        while(curNode)
        {
            std::cout << "node:" << curNode->_obj << std::endl;
            curNode = curNode->_nextNode;
        }

        listmy.clear();
        std::cout << "size:" << listmy.size() << std::endl;
        std::cout << "empty:" << listmy.empty() << std::endl;

    }
};

#endif
