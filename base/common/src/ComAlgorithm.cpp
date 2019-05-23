#include <BaseCode/ComAlgorithm.h>

NAMESPACE_BEGIN(com_algorithm)

//计算格式控制字符串占位符个数（%与%至少间隔一个字符，如：%s%l等%%则输出一个%，解析前）
U64 CalcFmtNum(const std::string& strFmt)
{
	//找出不相邻的%个数
	U64 nPosStart = 0, nPosSec = 0, nCount = 0;

	for (nPosStart = 0; 
		nPosStart = strFmt.find_first_of('%', nPosStart),
		( (nPosStart != std::string::npos)?( (nPosSec = strFmt.find_first_of('%', nPosStart + 1)), true):false ); )
	{
		if (nPosSec != std::string::npos)
		{
			if (max(I64(nPosSec - nPosStart), I64(0)) > 1)
			{
				//不相邻
				nCount += 2;
				nPosStart = nPosSec + 1;
			}
			else
			{
				//相邻或重叠
				//跳过这两个位置继续找
				nPosStart = nPosSec + 1;
				continue;
			}
		}
		else
		{
			++nCount;
			break;
		}
	}	

	return nCount;
}

//获取格式控制字符串第一个有效%位置
bool GetFirstValidFmtString(const std::string strFmt, U64& u64FirIndex, std::string& strOut)
{
	if (strFmt.length() <= 0)
		return false;

	u64FirIndex = 0;
	while ((u64FirIndex = strFmt.find_first_of('%', u64FirIndex), true))
	{
		if (u64FirIndex == std::string::npos)
		{
			strOut += strFmt.substr(0);
			return true;
		}
		else
		{
			if (u64FirIndex == 0)
			{
				return true;
			}
			else
			{
				strOut += strFmt.substr(0, u64FirIndex);
				return true;
			}
		}
	}

	u64FirIndex = std::string::npos;

	return false;
}

//u64StartIndex 必须为当前要格式化的%所在位置 NextFmtPosOut输出下一个未被格式化的位置输出std::string::npos表示结束
const char * NextFmtString(const std::string& strFmt, const U64& u64StartIndex, U64& NextFmtPosOut)
{
	if ((strFmt.length() <= 0) || (u64StartIndex == std::string::npos))
	{
		NextFmtPosOut = std::string::npos;
		return NULL;
	}		

	U64 u64CacheIndex = u64StartIndex;
	while ((NextFmtPosOut = strFmt.find_first_of('%', u64CacheIndex), true))
	{
		if (NextFmtPosOut == std::string::npos)
		{
			return strFmt.substr(u64StartIndex).c_str();
		}
		else
		{
			if (max(I64(NextFmtPosOut - u64CacheIndex), I64(0)) > 1)
			{
				//不相邻取得字符串片段
				return strFmt.substr(u64StartIndex, NextFmtPosOut - u64StartIndex).c_str();
			}
			else
			{
				//相邻或重叠
				//跳过这两个位置继续找
				++u64CacheIndex;
				continue;
			}
		}
	}

	//错误跳出
	NextFmtPosOut = std::string::npos;

	return NULL;
}

NAMESPACE_END(com_algorithm)