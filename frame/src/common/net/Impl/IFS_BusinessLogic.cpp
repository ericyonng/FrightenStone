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
 * @file  : IFS_BusinessLogic.cpp
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2020/1/8
 * @brief :
 */
#include "stdafx.h"
#include "FrightenStone/common/net/Impl/IFS_BusinessLogic.h"

#include "FrightenStone/common/log/Log.h"
#include "FrightenStone/common/assist/utils/Impl/STLUtil.h"
#include "FrightenStone/common/logicsys/logicsys.h"
#include "FrightenStone/common/assist/utils/Impl/RTTIUtil.h"

FS_NAMESPACE_BEGIN
OBJ_POOL_CREATE_DEF_IMPL(IFS_BusinessLogic, 16);

IFS_BusinessLogic::~IFS_BusinessLogic()
{
    _BeforeFacadesClose();
    _CloseFacades();
    _DestroyWillRegFacades();

    STLUtil::DelMapContainer(_protocolCmdRefHandlers);
}

Int32 IFS_BusinessLogic::BeforeStart()
{
    OnRegisterFacades();
    return StatusDefs::Success;
}

Int32 IFS_BusinessLogic::Start()
{
    Int32 st = _InitFacades();
    if(st != StatusDefs::Success)
    {
        g_Log->e<IFS_BusinessLogic>(_LOGFMT_("_InitFacades fail st[%d]"), st);
        return st;
    }

    st = _StartFacades();
    if(st != StatusDefs::Success)
    {
        g_Log->e<IFS_BusinessLogic>(_LOGFMT_("_StartFacades fail st[%d]"), st);
        return st;
    }

    return StatusDefs::Success;
}

void IFS_BusinessLogic::WillClose()
{
}

void IFS_BusinessLogic::BeforeClose()
{
    _BeforeFacadesClose();
}

void IFS_BusinessLogic::Close()
{
    _CloseFacades();
    _DestroyWillRegFacades();
    STLUtil::DelMapContainer(_protocolCmdRefHandlers);
}

// ¶©ÔÄÐ­Òé
void IFS_BusinessLogic::SubscribeProtocol(UInt32 protocolCmd, IDelegate<void, UInt64, NetMsgDecoder *> *handler)
{
    auto iterHandler = _protocolCmdRefHandlers.find(protocolCmd);
    if(iterHandler != _protocolCmdRefHandlers.end())
    {
        g_Log->w<IFS_BusinessLogic>(_LOGFMT_("protocol cmd[%d] handler[%p] already exists, please check if cover it or has wrong operation!")
                                    , protocolCmd, iterHandler->second);
        Fs_SafeFree(iterHandler->second);
        _protocolCmdRefHandlers.erase(iterHandler);
    }

    _protocolCmdRefHandlers.insert(std::make_pair(protocolCmd, handler));
}

void IFS_BusinessLogic::InvokeProtocolHandler(UInt64 sessionId, NetMsgDecoder *msgDecoder)
{
    auto iterHandler = _protocolCmdRefHandlers.find(msgDecoder->GetCmd());
    if(iterHandler == _protocolCmdRefHandlers.end())
    {
        g_Log->e<IFS_BusinessLogic>(_LOGFMT_("sessionId[%llu], protocolCmd[%u] have no handler!"), sessionId, msgDecoder->GetCmd());
        return;
    }

    auto handler = iterHandler->second;
    handler->Invoke(sessionId, msgDecoder);
}

Int32 IFS_BusinessLogic::RegisterFacade(IFS_FacadeFactory *facadeFactory)
{
    if(UNLIKELY(!facadeFactory))
    {
        g_Log->e<IFS_BusinessLogic>(_LOGFMT_("facadeFactory is null"));
        return StatusDefs::Failed;
    }

    if(UNLIKELY(_isInit))
    {
        g_Log->e<IFS_BusinessLogic>(_LOGFMT_("it is already start"));
        return StatusDefs::Failed;
    }

    for(_WillRegFacades::iterator it = _willRegFacades.begin();
        it != _willRegFacades.end();
        ++it)
    {
        if(it->facadeFactory != NULL && it->facadeFactory == facadeFactory)
        {
            g_Log->e<IFS_BusinessLogic>(_LOGFMT_("facadefactory repeated"));
            return StatusDefs::Failed;
        }
    }

    _willRegFacades.push_back(_WillRegFacade(facadeFactory));

    return StatusDefs::Success;
}

Int32 IFS_BusinessLogic::RegisterFacade(IFS_Facade *facade)
{
    if(UNLIKELY(!facade))
    {
        g_Log->e<IFS_BusinessLogic>(_LOGFMT_("facade is null"));
        return StatusDefs::Failed;
    }

    if(UNLIKELY(_isInit))
    {
        g_Log->e<IFS_BusinessLogic>(_LOGFMT_("it is already start"));
        return StatusDefs::Failed;
    }
    else if(std::find(_facades.begin(),
                      _facades.end(), facade) != _facades.end())
    {
        g_Log->e<IFS_BusinessLogic>(_LOGFMT_("facade repeated"));
        return StatusDefs::Failed;
    }

    for(_WillRegFacades::iterator regIt = _willRegFacades.begin();
        regIt != _willRegFacades.end();
        ++regIt)
    {
        if(regIt->facade != NULL && regIt->facade == facade)
        {
            g_Log->e<IFS_BusinessLogic>(_LOGFMT_("facade repeated"));
            return StatusDefs::Failed;
        }
    }

    _willRegFacades.push_back(_WillRegFacade(facade));

    return StatusDefs::Success;
}

IFS_Facade *IFS_BusinessLogic::GetFacade(const FS_String &facadeName)
{
    _Facades2::iterator it = _facades2.find(facadeName);
    if(it == _facades2.end())
    {
        g_Log->w<IFS_BusinessLogic>(_LOGFMT_("facadeName[%s] not found"), facadeName.c_str());
        return NULL;
    }

    _Facades &facades = it->second;
    return facades[0];
}

std::vector<IFS_Facade *> IFS_BusinessLogic::GetFacades(const FS_String &facadeName)
{
    static const std::vector<IFS_Facade *> emptyFacades;

    _Facades2::iterator it = _facades2.find(facadeName);
    if(it == _facades2.end())
    {
         g_Log->w<IFS_BusinessLogic>(_LOGFMT_("facadeName[%s] not found in facades2"), facadeName.c_str());
        return emptyFacades;
    }

    return it->second;
}

Int32 IFS_BusinessLogic::_InitFacades()
{
    if(_isInit)
        return StatusDefs::Success;

    bool initSuccess = true;
    for(_WillRegFacades::iterator regIt = _willRegFacades.begin();
        regIt != _willRegFacades.end();
        ++regIt)
    {
        IFS_Facade *facade;
        _WillRegFacade &willRegFacade = *regIt;
        if(willRegFacade.facadeFactory != NULL)
        {
            facade = willRegFacade.facadeFactory->Create();
            Fs_SafeFree(willRegFacade.facadeFactory);
        }
        else
        {
            facade = willRegFacade.facade;
            willRegFacade.facade = NULL;
        }

        facade->BindLogic(this);
        _AddFacade(facade);
        Int32 st = facade->OnInitialize();
        if(UNLIKELY(st != StatusDefs::Success))
        {
            g_Log->e<IFS_BusinessLogic>(_LOGFMT_("on initialize facade fail st[%d]"), st);
            _BeforeFacadesClose();
            _CloseFacades();
            _DestroyWillRegFacades();

            initSuccess = false;
            break;
        }

        facade->_inited = true;
    }

    if(initSuccess)
        _willRegFacades.clear();

    return initSuccess ? StatusDefs::Success : StatusDefs::Failed;
}

Int32 IFS_BusinessLogic::_StartFacades()
{
    size_t startIndex = 0;
    const size_t facadesSize = _facades.size();
    for(; startIndex < facadesSize; ++startIndex)
    {
        IFS_Facade *facade = _facades[startIndex];
        if(facade->_started)
            continue;

        Int32 st = facade->BeforeStart();
        if(st != StatusDefs::Success)
        {
            g_Log->e<IFS_BusinessLogic>(_LOGFMT_("facade before start fail st[%d]"), st);
            break;
        }
    }

    if(startIndex != facadesSize)
    {
        g_Log->e<IFS_BusinessLogic>(_LOGFMT_("has some unhandled facade when do before start startIndex[%llu] size[%llu]"), startIndex, facadesSize);
        return StatusDefs::Failed;
    }
    
    startIndex = 0;
    for(; startIndex < facadesSize; ++startIndex)
    {
        IFS_Facade *facade = _facades[startIndex];
        if(facade->_started)
            continue;

        Int32 st = facade->Start();
        if(st != StatusDefs::Success)
        {
            g_Log->e<IFS_BusinessLogic>(_LOGFMT_("facade start fail st[%d]"), st);
            break;
        }

        facade->_started = true;
    }

    if(startIndex == facadesSize)
        return StatusDefs::Success;

    _BeforeFacadesClose();
    g_Log->e<IFS_BusinessLogic>(_LOGFMT_("has some unhandled facade when do start startIndex[%llu] size[%llu]"), startIndex, facadesSize);

    return StatusDefs::Failed;
}

void IFS_BusinessLogic::_BeforeFacadesClose()
{
    for(_Facades::reverse_iterator it = _facades.rbegin();
        it != _facades.rend();
        ++it)
    {
        IFS_Facade *facade = *it;
        if(!facade->_started)
            continue;

        facade->BeforeClose();
        facade->_started = false;
    }
}

void IFS_BusinessLogic::_CloseFacades()
{
    for(_Facades::reverse_iterator it = _facades.rbegin();
        it != _facades.rend();
        ++it)
    {
        IFS_Facade *facade = *it;
        if(!facade->_inited)
            continue;

        facade->Close();
    }

    STLUtil::DelVectorContainer(_facades);
    _facades2.clear();
}

void IFS_BusinessLogic::_DestroyWillRegFacades()
{
    for(_WillRegFacades::iterator it = _willRegFacades.begin();
        it != _willRegFacades.end();
        ++it)
    {
        Fs_SafeFree((*it).facade);
        Fs_SafeFree((*it).facadeFactory);
    }

    _willRegFacades.clear();
}

void IFS_BusinessLogic::_AddFacade(IFS_Facade *facade)
{
    // Add facade to _facades(vector)
    _facades.push_back(facade);
    
    // Add facade to _facades2(map<type, vector>)
    const FS_String facadeName = RTTIUtil::GetByTypeName(typeid(*facade).name());
    _Facades2::iterator facadesIt = _facades2.find(facadeName);
    if(facadesIt == _facades2.end())
        facadesIt = _facades2.insert(std::make_pair(facadeName, _Facades())).first;
    _Facades2::mapped_type &typeFacades = facadesIt->second;
    typeFacades.push_back(facade);
}

IFS_BusinessLogic::_WillRegFacade::_WillRegFacade(IFS_Facade *facade)
{
    this->facade = facade;
    facadeFactory = NULL;
}

IFS_BusinessLogic::_WillRegFacade::_WillRegFacade(IFS_FacadeFactory *facadeFactory)
{
    facade = NULL;
    this->facadeFactory = facadeFactory;
}


FS_NAMESPACE_END
