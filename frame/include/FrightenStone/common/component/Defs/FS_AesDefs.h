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
 * @file  : FS_AesDefs.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/11/27
 * @brief :
 * 
 *
 * 
 */
#ifndef __Frame_Include_FrightenStone_Common_Component_Defs_FS_AesDefs_H__
#define __Frame_Include_FrightenStone_Common_Component_Defs_FS_AesDefs_H__
#pragma once

#include "FrightenStone/exportbase.h"
#include "FrightenStone/common/basedefs/DataType/DataType.h"

#pragma region macro define
#define FS_AES_256_KEY_LEN 32       // 256bit/8bit
#pragma endregion

// some define refer to aes
FS_NAMESPACE_BEGIN

class BASE_EXPORT FS_AesDefs
{
public:     
    // 密文类型
    enum AES_CYPHER_T_TYPE
    {
        AES_CYPHER_128 = 0,
        AES_CYPHER_192,
        AES_CYPHER_256,
    };
};
FS_NAMESPACE_END
#endif
