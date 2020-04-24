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
 * @file  : TestFSTlsTable.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/10/9
 * @brief :
 * 
 *
 * 
 */
#ifndef __Test_TestFSTlsTable_H__
#define __Test_TestFSTlsTable_H__
#pragma once

#include "stdafx.h"

class TestTlsTableTask 
{
public:
    static void Task(const fs::FS_ThreadPool *pool)
    {
        auto tlsTable = g_ThreadTlsTableMgr->GetAndCreateThisThreadTable();
        auto testElem = tlsTable->GetElement<fs::Tls_TestTls>(fs::TlsElemType::Tls_TestTls);
        if(!testElem)
            testElem = tlsTable->AllocElement<fs::Tls_TestTls>(fs::TlsElemType::Tls_TestTls);

        Int32 &tlsCount = testElem->count;
        UInt64 threadId = fs::SystemUtil::GetCurrentThreadId();
        for(Int32 i = 0; i < 10; ++i)
        {
            g_Log->i<TestTlsTableTask>(_LOGFMT_("threadId[%llu], tlsCount[%d]"), threadId, tlsCount);
            ++tlsCount;
            fs::SystemUtil::Sleep(1000);
        }

        // 释放本线程资源
        g_ThreadTlsTableMgr->FreeThisThreadTable();

        // 线程结束
        g_Log->i<TestTlsTableTask>(_LOGFMT_("TestTlsTableTask end"));
    }
};

class TestSystemTlsTableTask
{
public:
    static void Task(fs::FS_ThreadPool *pool)
    {
        UInt64 threadId = fs::SystemUtil::GetCurrentThreadId();
        for(Int32 i = 0; i < 10; ++i)
        {
            auto tlsTable = fs::FS_TlsUtil::GetUtilTlsTable();
            auto testElem = tlsTable->GetElement<fs::Tls_TestTls>(fs::TlsElemType::Tls_TestTls);
            if(!testElem)
                testElem = tlsTable->AllocElement<fs::Tls_TestTls>(fs::TlsElemType::Tls_TestTls);
            Int32 &tlsCount = testElem->count;
            g_Log->i<TestSystemTlsTableTask>(_LOGFMT_("threadId[%llu], tlsCount[%d]"), threadId, tlsCount);
            ++tlsCount;
        }

        // 释放本线程资源
        fs::FS_TlsUtil::FreeUtilTlsTable();

        // 线程结束
        g_Log->i<TestSystemTlsTableTask>(_LOGFMT_("TestSystemTlsTableTask end"));
    }
};

const Int32 performanceCnt = 1000000;
class TestTlsTablePerformanceTask
{
public:
    static void Task(const fs::FS_ThreadPool *pool)
    {
        fs::FS_TlsTable *tlsTable = NULL;
        fs::Time nowTime1, nowTime2;
        nowTime1.FlushTime();
        for(Int32 i = 0; i < performanceCnt; ++i)
            tlsTable = g_ThreadTlsTableMgr->GetAndCreateThisThreadTable();
        nowTime2.FlushTime();

        if(!tlsTable)
            tlsTable = g_ThreadTlsTableMgr->CreateThisThreadTable();

        auto tlsElem = tlsTable->GetElement<fs::Tls_TestTls>(fs::TlsElemType::Tls_TestTls);
        if(!tlsElem)
            tlsElem = tlsTable->AllocElement<fs::Tls_TestTls>(fs::TlsElemType::Tls_TestTls);

        const UInt64 threadId = fs::SystemUtil::GetCurrentThreadId();
        g_Log->i<TestTlsTablePerformanceTask>(_LOGFMT_("threadId[%llu], escape[%lld] tlsTablecnt[%d]")
                                              , threadId, (nowTime2 - nowTime1).GetTotalMicroSeconds(), tlsElem->count);
    }
};

class TestSystemTlsPerformanceTask
{
public:
    static void Task(const fs::FS_ThreadPool *pool)
    {
        fs::FS_TlsTable *tlsTable = NULL;
        fs::Time nowTime1, nowTime2;
        //fs::FS_TlsUtil::UtilGetAndCreateWithDefBuilder<fs::Tls_TestTls>(fs::TlsElemType::Tls_TestTls);
        nowTime1.FlushTime();
        for(Int32 i = 0; i < performanceCnt; ++i)
            fs::FS_TlsUtil::GetUtilTlsTable();
        nowTime2.FlushTime();

        if(!tlsTable)
            tlsTable = fs::FS_TlsUtil::GetUtilTlsTable();

        auto tlsElem = tlsTable->GetElement<fs::Tls_TestTls>(fs::TlsElemType::Tls_TestTls);
        if(!tlsElem)
            tlsElem = tlsTable->AllocElement<fs::Tls_TestTls>(fs::TlsElemType::Tls_TestTls);

        const UInt64 threadId = fs::SystemUtil::GetCurrentThreadId();
        g_Log->i<TestSystemTlsPerformanceTask>(_LOGFMT_("threadId[%llu], escape[%lld] tlsTablecnt[%d]")
                                              , threadId, (nowTime2 - nowTime1).GetTotalMicroSeconds(), tlsElem->count);
    }
};

class TestFSTlsTable
{
public:
    static void Run()
    {
        g_Log->InitModule(NULL);
        if(fs::FS_TlsUtil::CreateUtilTlsHandle() != StatusDefs::Success)
        {
            g_Log->e<TestFSTlsTable>(_LOGFMT_("CreateUtilTlsHandle fail"));
        }

        fs::FS_ThreadPool pool(0, 10);
        for(Int32 i = 0; i < 10; ++i)
        {
            //auto testTlsTask = fs::DelegatePlusFactory::Create(&TestTlsTableTask::Task);
            auto testTls = fs::DelegatePlusFactory::Create(&TestSystemTlsTableTask::Task);
            pool.AddTask(testTls, true);
        }
        getchar();
    }
};

#endif

