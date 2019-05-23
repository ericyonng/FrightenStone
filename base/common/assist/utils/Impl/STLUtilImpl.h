#ifdef __Base_Common_Assist_Utils_Impl_STLUtil_H__
/**
* @file STLUtilImpl.h
* @auther Huiya Song <120453674@qq.com>
* @date 2019/05/08
* @brief
*/

#pragma once

FS_NAMESPACE_BEGIN

template<typename _Type, AssistObjsDefs::DelMethods DelType>
void STLUtil::DelMapContainer(_Type &container)
{
    AutoDelHelper<DelType> delObj;
    for(auto iterElement = container.begin(); iterElement != container.end(); ++iterElement)
        delObj.Release(iterElement->second);
}

template<typename _Type, AssistObjsDefs::DelMethods DelType>
void STLUtil::DelSetContainer(_Type &container)
{
    AutoDelHelper<DelType> delObj;
    for(auto iterElement = container.begin(); iterElement != container.end(); ++iterElement)
    {
        auto element = *iterElement;
        delObj.Release(element);
    }
}

FS_NAMESPACE_END

#endif

