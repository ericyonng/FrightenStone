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
 * @file  : ThreadTlsTableMgr.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/10/10
 * @brief :
 * 
 *
 * 
 */
#ifndef __Frame_Include_FrightenStone_Common_Component_Impl_ThreadTlsTableMgr_H__
#define __Frame_Include_FrightenStone_Common_Component_Impl_ThreadTlsTableMgr_H__
#pragma once

#include "FrightenStone/exportbase.h"
#include "FrightenStone/common/basedefs/BaseDefs.h"
#include "FrightenStone/common/assist/assistobjs/Impl/Singleton.h"
#include "FrightenStone/common/asyn/asyn.h"

FS_NAMESPACE_BEGIN

class FS_TlsTable;

class BASE_EXPORT ThreadTlsTableMgr
{
public:
    ThreadTlsTableMgr();
    ~ThreadTlsTableMgr();

public:
    FS_TlsTable *GetThisThreadTable();
    FS_TlsTable *GetThisThreadTableNoLock();
    FS_TlsTable *GetAndCreateThisThreadTable();
    FS_TlsTable *CreateThisThreadTable();
    void FreeThisThreadTable();

private:
    FS_TlsTable *_GetThisThreadTable();
    FS_TlsTable *_CreateThisThreadTable();

private:
    Locker _guard;
    std::map<UInt64, FS_TlsTable *> _threadIdRefTlsTable;
};

template class BASE_EXPORT Singleton<ThreadTlsTableMgr, AssistObjsDefs::NoDel>;

FS_NAMESPACE_END

#include "FrightenStone/common/component/Impl/ThreadTlsTableMgrImpl.h"

#define g_ThreadTlsTableMgr fs::Singleton<fs::ThreadTlsTableMgr, fs::AssistObjsDefs::NoDel>::GetInstance()

#endif
