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
#ifndef __Frame_Include_FrightenStone_Common_Net_Defs_FS_ClientDefaultCfgDef_H__
#define __Frame_Include_FrightenStone_Common_Net_Defs_FS_ClientDefaultCfgDef_H__
#pragma once

// �ͻ��������ļ�·��
#define FS_CLIENT_CFG_PATH  "./ClientCfg.ini"

// ��
#define CLIENT_CFG_SEG  "ClientCfgSeg"
// Ĭ��Ŀ��ip
#define TARGET_IP_TO_CONNECT_KEY "TargetIp"
#define TARGET_IP_TO_CONNECT     "127.0.0.1"
// Ĭ��Ŀ��˿�
#define TARGET_PORT_TO_CONNECT_KEY "TargetPort"
#define TARGET_PORT_TO_CONNECT     "4567"
// Ĭ�ϴ������ٸ��ͻ���
#define CLIENT_QUANTITY_KEY  "ClientQuantity"
#define CLIENT_QUANTITY      "1000"
// �߳�����
#define CLIENT_THREAD_QUANTITY_KEY  "ClientThreadQuantity"
#define CLIENT_THREAD_QUANTITY      "8"
// �ͻ��˵�λʱ�䷢������
#define CLIENT_MSG_NUM_PER_PERIOD_KEY  "MsgNumPerPeriod"
#define CLIENT_MSG_NUM_PER_PERIOD      "1"
// �ͻ��˷�������ʱ��
#define CLIENT_SEND_PERIOD_KEY         "SendPeriod"
#define CLIENT_SEND_PERIOD             "999"           // ms����Ϊ��λ
// �ͻ��˷��ͻ�������С
#define CLIENT_SEND_BUFFER_SIZE_KEY     "SendBufferSize"
#define CLIENT_SEND_BUFFER_SIZE         "10240"         // Ĭ��10KB
// �ͻ��˽��ܻ�������С
#define CLIENT_RECV_BUFFER_SIZE_KEY     "RecvBufferSize"
#define CLIENT_RECV_BUFFER_SIZE         "10240"         // Ĭ��10KB
// �����ܵ��ķ������Ϣid�Ƿ�����
#define CLIENT_CHECK_MSG_ID_KEY         "CheckMsgId"
#define CLIENT_CHECK_MSG_ID             "1"
// �յ���������Ӧ��ŷ�����һ����Ϣ
#define CLIENT_IS_SEND_AFTER_SVR_RES_ARRIVE_KEY "IsSendAfterSvrResArrive"
#define CLIENT_IS_SEND_AFTER_SVR_RES_ARRIVE     "1"
// ��ʼbuffer����
#define CLIENT_MEMPOOL_BUFFER_CNT_INIT_KEY  "MemPoolBufferCntInit"
#define CLIENT_MEMPOOL_BUFFER_CNT_INIT      "1024"
// �ڴ�ط�������
#define CLIENT_MEMPOOL_ALLOC_MAX_BYTES_KEY  "MemPoolAllocMaxBytes"
#define CLIENT_MEMPOOL_ALLOC_MAX_BYTES      "549755813888"          // 512MB
// ����ʱ��
#define CLIENT_HEARTBEATINTERVAL_KEY  "HeartBeatInterval"
#define CLIENT_HEARTBEATINTERVAL      "3000000"          // 300s
#endif
