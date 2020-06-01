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
 * @file  : ServiceFactory.cpp
 * @author: Eric Yonng<120453674@qq.com>
 * @date  : 2020/1/15
 * @brief :
 */
#include "stdafx.h"
#include "Service/ServiceFactory.h"
#include "Service/ServiceType.h"
#include "Service/LogicSvc/LogicSvc.h"

fs::IFS_BusinessLogic *ServiceFactory::Create(Int32 serviceId)
{
    switch(serviceId)
    {
        case ServiceType::Account:
        {
            g_Log->sys<ServiceFactory>(_LOGFMT_("will create account service... id[%d]"), serviceId);
        }
        break;
        case ServiceType::BusinessLogic:
        {
            g_Log->sys<ServiceFactory>(_LOGFMT_("will create businesslogic service... id[%d]"), serviceId);
            return new LogicSvc(serviceId);
        }
        break;
        case ServiceType::Center:
        {
            g_Log->sys<ServiceFactory>(_LOGFMT_("will create center service... id[%d]"), serviceId);
        }
        break;
        case ServiceType::DB:
        {
            g_Log->sys<ServiceFactory>(_LOGFMT_("will create db service... id[%d]"), serviceId);
        }
        break;
        case ServiceType::Gateway:
        {
            g_Log->sys<ServiceFactory>(_LOGFMT_("will create gateway service... id[%d]"), serviceId);
        }
        break;
        case ServiceType::ClientSimulation:
        {
            g_Log->sys<ServiceFactory>(_LOGFMT_("will create ClientSimulation service... id[%d]"), serviceId);
            return new LogicSvc(serviceId);
        }
        break;
        default:
        {
            g_Log->w<ServiceFactory>(_LOGFMT_("unknown service id[%d]"), serviceId);
        }
        break;
    }

    return NULL;
}
