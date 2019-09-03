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
        TestIterator();
//         fs::FS_List<char *> listmy;
//         auto newBuff = new char[32];
//         memset(newBuff, 0, 32);
//         strcpy(newBuff, "1");
//         std::cout << "empty:" << listmy.empty() << std::endl;
//         listmy.push_front(newBuff);
// 
//         newBuff = new char[32];
//         memset(newBuff, 0, 32);
//         strcpy(newBuff, "2");
//         listmy.push_back(newBuff);
// 
//         std::cout << "empty:" << listmy.empty() << std::endl;
//         std::cout << "size:" << listmy.size() << std::endl;
//         std::cout << "front" << listmy.front() << std::endl;
//         std::cout << "back" << listmy.back() << std::endl;
//         std::cout << "begin:" << listmy.begin()->_obj << std::endl;
//         std::cout << "end:" << listmy.end()->_obj << std::endl;
//                 
//         std::cout << "size after erase:" << listmy.size() << std::endl;
//         std::cout << "begin:" << listmy.begin()->_obj << std::endl;
//         listmy.insert_before((char *)( "wocao"), listmy.end());
//         listmy.pop_back();
//         auto curNode = listmy.begin();
//         if(curNode)
//         {
//             for(auto i = 0; i < listmy.size(); ++i)
//             {
//                 std::cout << "node:" << curNode->_obj << std::endl;
//                 curNode = curNode->_nextNode;
//             }
//         }
// 
//         listmy.erase(listmy.begin()->_nextNode);
//         curNode = listmy.begin();
//         if(curNode)
//         {
//             for(auto i = 0; i < listmy.size(); ++i)
//             {
//                 std::cout << "node:" << curNode->_obj << std::endl;
//                 curNode = curNode->_nextNode;
//             }
//         }
// 
//         listmy.pop_front();
//         std::cout << "after pop front:" << std::endl;
// 
//         curNode = listmy.begin();
//         if(curNode)
//         {
//             for(auto i = 0; i < listmy.size(); ++i)
//             {
//                 std::cout << "node:" << curNode->_obj << std::endl;
//                 curNode = curNode->_nextNode;
//             }
//         }
// 
//         listmy.clear();
//         std::cout << "size:" << listmy.size() << std::endl;
//         std::cout << "empty:" << listmy.empty() << std::endl;

//         fs::FS_List<Int32> *myList = new fs::FS_List<Int32>;
//         std::list<Int32> *sysList = new std::list<Int32>;
// 
//         const Int32 loopCnt = 1000000;
//         for(auto i = 0; i < loopCnt; ++i)
//             sysList->push_front(i);
//         for(auto i = 0; i < loopCnt; ++i)
//             myList->push_front(i);
// 
//         fs::Time time1, time2;
//         //sysList->push_back(0);
//         time1.FlushTime();
//         for(auto i = 0; i < loopCnt; ++i)
//             sysList->push_front(i);
//         time2.FlushTime();
//         std::cout << "sys escape:" << (time1 - time2).GetTotalMicroSeconds() << std::endl;
// 
//         //myList->push_back(0);
//         time1.FlushTime();
//         for(auto i = 0; i < loopCnt; ++i)
//             myList->push_front(i);
//         time2.FlushTime();
//         std::cout << "mylist escape:" << (time1 - time2).GetTotalMicroSeconds() << std::endl;
//         getchar();
    }

    static void TestIterator()
    {
        struct TestStruct1
        {
            fs::ListNode<TestStruct1 *> *iterList1;
            Int32 _num;
        };

        struct TestStruct2
        {
            std::list<TestStruct2 *>::iterator iterList2;
            Int32 _num;
        };

        const Int32 testNum = 10;
        fs::FS_List<TestStruct1 *> testList1;
        for(Int32 i = 0; i < testNum; ++i)
        {
            auto newData = new TestStruct1;
            newData->_num = i;
            testList1.push_back(newData);
            newData->iterList1 = testList1.end();
        }

        std::list<TestStruct2 *> testList2;
        for(Int32 i = 0; i < testNum; ++i)
        {
            auto newData = new TestStruct2;
            newData->_num = i;
            testList2.push_back(newData);
            newData->iterList2 = --testList2.end();
        }

        Int32 delNode3 = 3;
        {
            auto iter = testList2.begin();
            auto beginNode = testList1.begin();
            for(Int32 i = 0; i < testNum; ++i)
            {
                ++iter;
                beginNode = beginNode->_nextNode;
                if(i >= delNode3)
                {
                    testList1.erase(beginNode);
                    testList2.erase(iter);
                    break;
                }
            }
        }

        // 打印出每个节点
        std::cout << "testList2" << std::endl;
        for(auto iter = testList2.begin(); iter != testList2.end(); ++iter)
        {
            std::cout << "num=" << (*((*iter)->iterList2))->_num << std::endl;
        }

        std::cout << "testList1" << std::endl;
        auto beginTest = testList1.begin();
        const Int32 sizeNode = testList1.size();
        while(beginTest)
        {
            std::cout << "num=" << beginTest->_obj->iterList1->_obj->_num << std::endl;
            beginTest = beginTest->_nextNode;
        }
    }
};

#endif
