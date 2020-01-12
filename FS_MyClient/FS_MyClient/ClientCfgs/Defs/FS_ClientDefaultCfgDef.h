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
 * @file  : FS_ClientDefaultCfgDef.h
 * @author: ericyonng<120453674@qq.com>
 * @date  : 2019/12/5
 * @brief :
 * 
 *
 * 
 */

#pragma once

// 客户端配置文件路径
#define FS_CLIENT_CFG_PATH  "./ClientCfg.ini"

// 段
#define CLIENT_CFG_SEG  "ClientCfgSeg"
// 默认目标ip
#define TARGET_IP_TO_CONNECT_KEY "TargetIp"
#define TARGET_IP_TO_CONNECT     "127.0.0.1"
// 默认目标端口
#define TARGET_PORT_TO_CONNECT_KEY "TargetPort"
#define TARGET_PORT_TO_CONNECT     "4567"
// 默认创建多少个客户端
#define CLIENT_QUANTITY_KEY  "ClientQuantity"
#define CLIENT_QUANTITY      "1000"
// 客户端单位时间发包数量
#define CLIENT_MSG_NUM_PER_PERIOD_KEY  "MsgNumPerPeriod"
#define CLIENT_MSG_NUM_PER_PERIOD      "1"
// 客户端发包周期时间
#define CLIENT_SEND_PERIOD_KEY         "SendPeriod"
#define CLIENT_SEND_PERIOD             "999"           // ms毫秒为单位
// 检查接受到的服务端消息id是否连续
#define CLIENT_CHECK_MSG_ID_KEY         "CheckMsgId"
#define CLIENT_CHECK_MSG_ID             "1"
// 收到服务器回应后才发送下一条消息
#define CLIENT_IS_SEND_AFTER_SVR_RES_ARRIVE_KEY "IsSendAfterSvrResArrive"
#define CLIENT_IS_SEND_AFTER_SVR_RES_ARRIVE     "1"

