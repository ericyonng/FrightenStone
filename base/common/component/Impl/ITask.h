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
    virtual Int32 Run() = 0;
    virtual Int32 Release() { delete this; return 0; }
    virtual void SetArg(void *arg) { _arg = arg; }
    virtual void *GetArg() { return _arg; }

protected:
    ITask() { _arg = NULL; }
    virtual ~ITask() {}

protected:
    void *_arg = NULL;
};

FS_NAMESPACE_END

#endif
