#ifndef __Base_Common_Assist_AssistObjs_Defs_AssistObjsDefs_H__
#define __Base_Common_Assist_AssistObjs_Defs_AssistObjsDefs_H__
/**
* @file AssistObjsDefs.h
* @auther Huiya Song <120453674@qq.com>
* @date 2019/04/28
* @brief
*/

#pragma once

#include<base/exportbase.h>
#include "base/common/basedefs/Macro/MacroDefs.h"

#ifdef FRIGHTEN_STONE_BASE_EXPORT_BASE_DLL

// inner use

#endif

class BASE_EXPORT AssistObjsDefs
{
public:
    enum DelMethods
    {
        Delete = 0,
        MultiDelete,
        SelfRelease,
        NoDel,
    };
};

FS_NAMESPACE_BEGIN

// auto free obj
template<typename Type, AssistObjsDefs::DelMethods del>
class AutoDelObj
{
public:
    void Release();
    void Release(Type *&p);
};

template<AssistObjsDefs::DelMethods del>
class AutoDelHelper
{
public:
    template<typename _Type>
    void Release(_Type *&val);
};

FS_NAMESPACE_END


#include "base/common/assist/assistobjs/Defs/AssistObjsDefsImpl.h"
#endif // !__Base_Common_Assist_AssistObjs_Defs_AssistObjsDefs_H__