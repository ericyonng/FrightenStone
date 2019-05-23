#ifndef COM_ALGORITHM_H_
#define COM_ALGORITHM_H_

#pragma once

NAMESPACE_BEGIN(com_algorithm)

//计算格式控制字符串占位符个数（%与%至少间隔一个字符，如：%s%l等%%则输出一个%，解析前）
U64 CalcFmtNum(const std::string& strFmt);

//获取格式控制字符串第一个有效%位置
bool GetFirstValidFmtString(const std::string strFmt, U64& u64FirIndex, std::string& strOut);


//下一个格式控制字符串片段
const char * NextFmtString(const std::string& strFmt, const U64& u64StartIndex, U64& NextFmtPosOut);

//排序算法

NAMESPACE_END(com_algorithm)

#endif

