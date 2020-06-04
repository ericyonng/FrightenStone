/*!
 * MIT License
 *
 * Copyright (c) 2019 Eric Yonng<120453674@qq.com>
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
 * @file  : FS_Event.cpp
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2019/7/31
 * @brief :
 * 
 *
 * 
 */
#include "stdafx.h"
#include "FrightenStone/common/event/Impl/FS_Event.h"

FS_NAMESPACE_BEGIN
static const SmartVar __nilSmartVar;
static const std::map<int, SmartVar> __emptyIntKeyParams;
static const std::map<FS_String, SmartVar> __emptyStrKeyParams;
static const std::map<const char*, SmartVar> __emptyConstantStrKeyParams;
FS_NAMESPACE_END

FS_NAMESPACE_BEGIN
OBJ_POOL_CREATE_ANCESTOR_IMPL(FS_Event, __DEF_OBJ_POOL_OBJ_NUM__);

FS_Event::FS_Event(int id /*= 0*/, bool dontDelAfterFire /*= false*/)
    : _id(id)
    , _dontDelAfterFire(dontDelAfterFire)
    , _intKeyParams(NULL)
   // , _constantStrKeyParams(NULL)
    , _strKeyParams(NULL)
{

}

FS_Event::~FS_Event()
{
    if(_intKeyParams)
        Fs_SafeFree(_intKeyParams);

//     if(_constantStrKeyParams)
//         Fs_SafeFree(_constantStrKeyParams);

    if(_strKeyParams)
        Fs_SafeFree(_strKeyParams);
}


const SmartVar &FS_Event::GetParam(int key) const
{
    if(_intKeyParams == NULL)
        return __nilSmartVar;

    _IntKeyParams::const_iterator it = _intKeyParams->find(key);
    return it != _intKeyParams->end() ? it->second : __nilSmartVar;
}

const SmartVar &FS_Event::GetParam(const char *key) const
{
    return GetParam(FS_String(key));
//     if(_constantStrKeyParams == NULL)
//         return __nilSmartVar;
// 
//     _ConstantStrKeyParams::const_iterator it = _constantStrKeyParams->find(key);
//     return it != _constantStrKeyParams->end() ? it->second : __nilSmartVar;
}

const SmartVar &FS_Event::GetParam(const FS_String &key) const
{
    if(_strKeyParams == NULL)
        return __nilSmartVar;

    _StrKeyParams::const_iterator it = _strKeyParams->find(key);
    return it != _strKeyParams->end() ? it->second : __nilSmartVar;
}

FS_Event &FS_Event::SetParam(int key, const SmartVar &param)
{
    if(_intKeyParams == NULL)
        _intKeyParams = new _IntKeyParams;

    _IntKeyParams::iterator it = _intKeyParams->find(key);
    if(it == _intKeyParams->end())
        _intKeyParams->insert(std::make_pair(key, param));
    else
        it->second = param;

    return *this;
}

FS_Event &FS_Event::SetParam(const char* key, const SmartVar &param)
{
    return SetParam(FS_String(key), param);
//     if(_constantStrKeyParams == NULL)
//         _constantStrKeyParams = new _ConstantStrKeyParams;
// 
//     _ConstantStrKeyParams::iterator it = _constantStrKeyParams->find(key);
//     if(it == _constantStrKeyParams->end())
//         _constantStrKeyParams->insert(std::make_pair(key, param));
//     else
//         it->second = param;
// 
//     return *this;
}

FS_Event &FS_Event::SetParam(const FS_String &key, const SmartVar &param)
{
    if(_strKeyParams == NULL)
        _strKeyParams = new _StrKeyParams;

    _StrKeyParams::iterator it = _strKeyParams->find(key);
    if(it == _strKeyParams->end())
        _strKeyParams->insert(std::make_pair(key, param));
    else
        it->second = param;

    return *this;
}

const std::map<int, SmartVar> &FS_Event::GetIntKeyParams() const
{
    return _intKeyParams != NULL ? *_intKeyParams : __emptyIntKeyParams;
}

// const std::map<const char*, SmartVar> &FS_Event::GetConstantStrKeyParams() const
// {
//     return _constantStrKeyParams != NULL ? *_constantStrKeyParams : __emptyConstantStrKeyParams;
// }

const std::map<FS_String, SmartVar> &FS_Event::GetStrKeyParams() const
{
    return _strKeyParams != NULL ? *_strKeyParams : __emptyStrKeyParams;
}

SmartVar &FS_Event::operator [](int key)
{
    if(!_intKeyParams)
        _intKeyParams = new _IntKeyParams;

    _IntKeyParams::iterator it = _intKeyParams->find(key);
    if(it == _intKeyParams->end())
        return _intKeyParams->insert(std::make_pair(key, SmartVar())).first->second;
    else
        return it->second;
}

SmartVar &FS_Event::operator [](const char *key)
{
    return this->operator[](FS_String(key));
//     if(!_constantStrKeyParams)
//         _constantStrKeyParams = new _ConstantStrKeyParams;
// 
//     _ConstantStrKeyParams::iterator it = _constantStrKeyParams->find(key);
//     if(it == _constantStrKeyParams->end())
//         return _constantStrKeyParams->insert(std::make_pair(key, SmartVar())).first->second;
//     else
//         return it->second;
}

SmartVar &FS_Event::operator [](const FS_String &key)
{
    if(!_strKeyParams)
        _strKeyParams = new _StrKeyParams;

    _StrKeyParams::iterator it = _strKeyParams->find(key);
    if(it == _strKeyParams->end())
        return _strKeyParams->insert(std::make_pair(key, SmartVar())).first->second;
    else
        return it->second;
}

const SmartVar &FS_Event::operator [](int key) const
{
    if(!_intKeyParams)
        return __nilSmartVar;

    _IntKeyParams::const_iterator it = _intKeyParams->find(key);
    return it != _intKeyParams->end() ? it->second : __nilSmartVar;
}

const SmartVar &FS_Event::operator [](const char* key) const
{
    return this->operator[](FS_String(key));
//     if(!_constantStrKeyParams)
//         return __nilSmartVar;
// 
//     _ConstantStrKeyParams::const_iterator it = _constantStrKeyParams->find(key);
//     return it != _constantStrKeyParams->end() ? it->second : __nilSmartVar;
}

const SmartVar &FS_Event::operator [](const FS_String &key) const
{
    if(!_strKeyParams)
        return __nilSmartVar;

    _StrKeyParams::const_iterator it = _strKeyParams->find(key);
    return it != _strKeyParams->end() ? it->second : __nilSmartVar;
}

FS_NAMESPACE_END
