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
 * @file  : TestAppUtil.cpp
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/3/16
 * @brief :
 */
#include "stdafx.h"
#include "TestInst/TestAppUtil.h"

void TestAppUtil::Run()
{
    // app环境初始化
    auto st = fs::AppUtil::Init();
    if(st != StatusDefs::Success)
    {
        fs::AppUtil::Finish();
        std::cout << "app init fail st:"<< st << "..." << std::endl;
        return;
    }

    // 启动
    st = fs::AppUtil::Start();
    if(st != StatusDefs::Success)
    {
        g_Log->e<TestAppUtil>(_LOGFMT_("app start fail st[%d]"), st);
        fs::AppUtil::Finish();
        return;
    }

    g_Log->custom("app start suc press any key to finish ...");

    // 静默阻塞
    fs::AppUtil::Wait();

    g_Log->custom("app will finish ...");

    // 结束
    fs::AppUtil::Finish();

    std::cout << "app finished." << std::endl;
}
