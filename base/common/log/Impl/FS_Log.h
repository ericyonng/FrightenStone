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
 * @file  : FS_Log.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/6/12
 * @brief :
 *          1.�ṩд��־�ӿڣ��������������
 *          2.��Ŀ¼��־ crash, system, netlayerlog��Э����Ϣ��ҵ�����־��
 *          3.�����ļ���С���ļ�
 *          4.������������ȼ���:warning,error,debug,info��
 *          5.֧��json log ���
 *                              {
                                    time:(��ȷ��΢��)fmt:1970-01-01 00:00:00.123456
                                    class name:
                                    function:
                                    line:
                                    level:
                                    content:
                                    {
                                        op:
                                        status:statusCode
                                        ecxceptioninfo:
                                        stackinfo:
                                    }
                                }
 */
#ifndef __Base_Common_Log_Impl_Log_H__
#define __Base_Common_Log_Impl_Log_H__
#pragma once

#include "base/common/log/Interface/ILog.h"
#include "base/common/log/Defs/LogData.h"
#include "base/common/component/Impl/FS_Delegate.h"
#include "base/common/asyn/asyn.h"

FS_NAMESPACE_BEGIN

class FS_ThreadPool;
class LogFile;
class BASE_EXPORT FS_Log : public ILog
{
public:
    FS_Log(const Byte8 *processName);
    virtual ~FS_Log();

    /* ��־hook */
    virtual void InstallLogHookFunc(Int32 level, IDelegatePlus<void, const LogData *> *delegate);    // �����delegate
    void UnInstallLogHook(Int32 level);

    /* ���ܺ��� */
    virtual void FinishModule();
    void SetFlushInterval(Int32 interval);
    Int32 AddLog(Int32 fileUnqueIndex, const char *logPath, const char *fileName);
    
protected:
    virtual LogData *_BuildLogData(const Byte8 *className, const Byte8 *funcName, const FS_String &content, Int32 codeLine, Int32 logLevel);
    virtual void _WriteLog(Int32 fileUniqueIndex, LogData *logData);

private:
    FS_ThreadPool *_threadPool;
    FS_String _processName;
    std::map<Int32, IDelegatePlus<void, const LogData *> *> _levelRefDelegate;

    /* ��־�ļ����� */
    ConditionLocker _locker;
    std::map<Int32, LogFile *> _fileUniqueIndexRefLogFiles;
    std::map<Int32, std::list<LogData *> *> _fileUniqueIndexRefLogDatas;
};

FS_NAMESPACE_END

#include "base/common/log/Impl/FS_LogImpl.h"

#endif