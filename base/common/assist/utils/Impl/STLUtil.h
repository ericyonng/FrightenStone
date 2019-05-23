#ifndef __Base_Common_Assist_Utils_Impl_STLUtil_H__
#define __Base_Common_Assist_Utils_Impl_STLUtil_H__
/**
* @file STLUtil.h
* @auther Huiya Song <120453674@qq.com>
* @date 2019/05/08
* @brief
*/

#pragma once

#include<base/exportbase.h>
#include "base/common/basedefs/Macro/ForAll/ForAllMacro.h"
#include "base/common/assist/assistobjs/assistobjs.h"

FS_NAMESPACE_BEGIN

class BASE_EXPORT STLUtil
{
public:
    // É¾³ýÈÝÆ÷
    template<typename _Type, AssistObjsDefs::DelMethods DelType = AssistObjsDefs::Delete>
    static void DelMapContainer(_Type &container);

    template<typename _Type, AssistObjsDefs::DelMethods DelType = AssistObjsDefs::Delete>
    static void DelSetContainer(_Type &container);
};

FS_NAMESPACE_END

#include "base/common/assist/utils/Impl/STLUtilImpl.h"
#endif // !__Base_Common_Assist_Utils_Impl_STLUtil_H__
