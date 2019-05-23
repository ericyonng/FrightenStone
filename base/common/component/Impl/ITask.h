#ifndef __Base_Common_Component_Impl_ITask__H__
#define __Base_Common_Component_Impl_ITask__H__
/**
* @file    ITask.h
* @author  Huiya Song<120453674@qq.com>
* @date    2019/05/15
* @brief
*/

#pragma once

#include "base/exportbase.h"
#include "base/common/basedefs/Macro/MacroDefs.h"
#include "base/common/basedefs/DataType/DataType.h"

FS_NAMESPACE_BEGIN

class BASE_EXPORT ITask
{
    NO_COPY(ITask)
public:
    //任务执行体
    virtual Int32 Run() = 0;
    //释放
    virtual Int32 Release() { delete this; return 0; }
    //设置参数
    virtual void SetArg(void *arg) { _arg = arg; }
    //获取参数
    virtual void *GetArg() { return _arg; }

    //函数定义
protected:
    //构造函数
    ITask() { _arg = NULL; }
    //析构函数
    virtual ~ITask() {}

protected:
    void *_arg = NULL;
};

FS_NAMESPACE_END

#endif
