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
 * @file  : FS_Event.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/7/31
 * @brief :
 * 
 *
 * 
 */
#ifndef __Frame_Include_FrightenStone_Common_Event_Impl_FS_Event_H__
#define __Frame_Include_FrightenStone_Common_Event_Impl_FS_Event_H__
#pragma once

#include "FrightenStone/exportbase.h"
#include "FrightenStone/common/basedefs/BaseDefs.h"
#include "FrightenStone/common/component/Impl/FS_String.h"
#include "FrightenStone/common/component/Impl/SmartVar/SmartVar.h"
#include "FrightenStone/common/objpool/objpool.h"

FS_NAMESPACE_BEGIN

class BASE_EXPORT FS_Event
{
    OBJ_POOL_CREATE_DEF(FS_Event);
public:
    FS_Event(int id = 0, bool dontDelAfterFire = false);
    virtual ~FS_Event();

public:
    /**
     * Get the event Id.
     * @return int - the event Id.
     */
    int GetId() const;

    /**
     * Check dont delete after fire option.
     * @return bool - the option.
     */
    bool IsDontDelAfterFire() const;

    /**
     * Set don't delete after fire option.
     * @param[in] dontDelAfterFire - the don't delete after fire option.
     */
    void SetDontDelAfterFire(bool dontDelAfterFire);

public:
    /**
     * Get integer key indexed event param.
     * @param[in] key - the integer key.
     * @return const SmartVar & - the event param.
     */
    const SmartVar &GetParam(int key) const;

    /**
    * Get constant string key indexed event param.
    * @param[in] key - the constant string key.
    * @return const SmartVar & - the event param.
    */
    const SmartVar &GetParam(const char* key) const;

    /**
     * Get string key indexed event param.
     * @param[in] key - the string key.
     * @return const SmartVar & - the event param.
     */
    const SmartVar &GetParam(const FS_String &key) const;

    /**
     * Set integer key indexed event param.
     * @param[in] key   - the param key.
     * @param[in] param - the param.
     * @return FS_Event & - this reference.
     */
    FS_Event &SetParam(int key, const SmartVar &param);
    /**
     * Set integer key indexed event param(template version).
     * @param[in] key   - the param key.
     * @param[in] param - the param.
     * @return FS_Event & - this reference.
     */
    template <typename ParamType>
    FS_Event &SetParam(int key, const ParamType &param);

    /**
    * Set constant string key indexed event param.
    * @param[in] key   - the param key.
    * @param[in] param - the param.
    * @return FS_Event & - this reference.
    */
    FS_Event &SetParam(const char* key, const SmartVar &param);
    /**
    * Set constant string key indexed event param(template version).
    * @param[in] key   - the param key.
    * @param[in] param - the param.
    * @return FS_Event & - this reference.
    */
    template <typename ParamType>
    FS_Event &SetParam(const char* key, const ParamType &param);

    /**
     * Set string key indexed event param.
     * @param[in] key   - the param key.
     * @param[in] param - the param.
     * @return FS_Event & - this reference.
     */
    FS_Event &SetParam(const FS_String &key, const SmartVar &param);
    /**
     * Set string key indexed event param(template version).
     * @param[in] key   - the param key.
     * @param[in] param - the param.
     * @return FS_Event & - this reference.
     */
    template <typename ParamType>
    FS_Event &SetParam(const FS_String &key, const ParamType &param);

public:
    /**
     * Get all int key indexed params.
     * @return const std::map<int, SmartVar> & - the int key indexed params const reference.
     */
    const std::map<int, SmartVar> &GetIntKeyParams() const;

    /**
     * Get all string key indexed params count.
     * @return size_t - the integer key indexed params count.
     */
    size_t GetIntKeyParamsCount() const;

    /**
    * Get all constant string key indexed params.
    * @return const std::map<const char*, SmartVar> & - the constant string key indexed params const reference.
    */
    const std::map<const char*, SmartVar> &GetConstantStrKeyParams() const;

    /**
    * Get all constant string key indexed params count.
    * @return size_t - the constant string key indexed params count.
    */
    size_t GetConstantStrKeyParamsCount() const;

    /**
     * Get all string key indexed params.
     * @return const std::map<FS_String, SmartVar> & - the string key indexed params const reference.
     */
    const std::map<FS_String, SmartVar> &GetStrKeyParams() const;

    /**
     * Get all string key indexed params count.
     * @return size_t - the string key indexed params count.
     */
    size_t GetStrKeyParamsCount() const;

public:
    /**
     * Subscript supports.
     */
    SmartVar &operator [](int key);
    SmartVar &operator [](const char* key);
    SmartVar &operator [](const FS_String &key);
    const SmartVar &operator [](int key) const;
    const SmartVar &operator [](const char* key) const;
    const SmartVar &operator [](const FS_String &key) const;

    /**
     * Disable assignment.
     */
    NO_COPY(FS_Event)

protected:
    int _id;
    bool _dontDelAfterFire;

    typedef std::map<int, SmartVar> _IntKeyParams;
    _IntKeyParams *_intKeyParams;

    typedef std::map<const char*, SmartVar> _ConstantStrKeyParams;
    _ConstantStrKeyParams *_constantStrKeyParams;

    typedef std::map<FS_String, SmartVar> _StrKeyParams;
    _StrKeyParams *_strKeyParams;
};

FS_NAMESPACE_END

#include "FrightenStone/common/event/Impl/FS_EventImpl.h"

#endif
