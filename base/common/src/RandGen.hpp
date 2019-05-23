#ifndef RAND_GEN_HPP_
#define RAND_GEN_HPP_

#pragma once
#include <BaseCode/RandDataType.h>
#include <boost/random.hpp>

NAMESPACE_BEGIN(gtool)

template<typename RandNumType = I64, RAND_GEN_ALGORITHM_TYPE eGenType = RAND_GEN_ALGORITHM_TYPE_MT19937_64, RAND_DIS_TYPE eDisType = RAND_DIS_TYPE_NORMAL>
class  CRandGen
{
	NO_COPY_NO_MOVE(CRandGen);

public:
	CRandGen() {}
	CRandGen(const RandNumType rMin, const RandNumType rMax, const RandNumType rSRand)
		:m_Disor(rMin, rMax), m_RandSrcor(rSRand)
	{}

	virtual ~CRandGen() {}

	//产生随机数
	operator typename RandNumType()
	{
		return static_cast<RandNumType>(m_Disor.rGen(m_RandSrcor.rGen));
	}

	//分布器
protected:
	template<typename RandNumType, RAND_DIS_TYPE>
	struct Disor
	{
		
	};

	template<typename RandNumType>
	struct Disor<RandNumType, RAND_DIS_TYPE_SMALLINT>
	{
		boost::random::uniform_smallint<RandNumType> rGen;
		Disor(const RandNumType rMin = RAND_DIS_NUM_SCOPE_MIN, const RandNumType rMax = ((std::numeric_limits<RandNumType>::max)()&MAX_I64_NUM)):rGen(rMin, rMax)
		{
			
		}
	};

	template<typename RandNumType>
	struct Disor<RandNumType, RAND_DIS_TYPE_INT>
	{
		boost::random::uniform_int_distribution<RandNumType> rGen;
		Disor(const RandNumType rMin = RAND_DIS_NUM_SCOPE_MIN, const RandNumType rMax =((std::numeric_limits<RandNumType>::max)()&MAX_I64_NUM)) :rGen(rMin, rMax)
		{

		}
	};

	template<typename RandNumType>
	struct Disor<RandNumType, RAND_DIS_TYPE_01>
	{
		boost::random::uniform_01<RandNumType> rGen;
		Disor(const RandNumType rMin = RAND_DIS_NUM_SCOPE_MIN, const RandNumType rMax = ((std::numeric_limits<RandNumType>::max)()&MAX_I64_NUM)) :rGen(rMin, rMax)
		{

		}
	};

	template<typename RandNumType>
	struct Disor<RandNumType, RAND_DIS_TYPE_REAL>
	{
		boost::random::uniform_real_distribution<RandNumType> rGen;
		Disor(const RandNumType rMin = RAND_DIS_NUM_SCOPE_MIN, const RandNumType rMax = ((std::numeric_limits<RandNumType>::max)() & MAX_I64_NUM)) :rGen(rMin, rMax)
		{

		}
	};

	template<typename RandNumType>
	struct Disor<RandNumType, RAND_DIS_TYPE_BERNOULLI>
	{
		boost::random::bernoulli_distribution<RandNumType> rGen;
		Disor(const RandNumType rMin = RAND_DIS_NUM_SCOPE_MIN, const RandNumType rMax = ((std::numeric_limits<RandNumType>::max)() & MAX_I64_NUM)) :rGen(rMin, rMax)
		{

		}
	};

	template<typename RandNumType>
	struct Disor<RandNumType, RAND_DIS_TYPE_GEOMETRIC>
	{
		boost::random::geometric_distribution<RandNumType> rGen;
		Disor(const RandNumType rMin = RAND_DIS_NUM_SCOPE_MIN, const RandNumType rMax = ((std::numeric_limits<RandNumType>::max)() & MAX_I64_NUM)) :rGen(rMin, rMax)
		{

		}
	};

	template<typename RandNumType>
	struct Disor<RandNumType, RAND_DIS_TYPE_TRIANGLE>
	{
		boost::random::triangle_distribution<RandNumType> rGen;
		Disor(const RandNumType rMin = RAND_DIS_NUM_SCOPE_MIN, const RandNumType rMax = ((std::numeric_limits<RandNumType>::max)() & MAX_I64_NUM)) :rGen(rMin, rMax)
		{

		}
	};

	template<>
	struct Disor<RandNumType, RAND_DIS_TYPE_EXPONENTIAL>
	{
		boost::random::exponential_distribution<RandNumType> rGen;
		Disor(const RandNumType rMin = RAND_DIS_NUM_SCOPE_MIN, const RandNumType rMax = ((std::numeric_limits<RandNumType>::max)() & MAX_I64_NUM)) :rGen(rMin, rMax)
		{

		}
	};

	template<typename RandNumType>
	struct Disor<RandNumType, RAND_DIS_TYPE_NORMAL>
	{
		boost::random::normal_distribution<RandNumType> rGen;
		Disor(const RandNumType rMin = RAND_DIS_NUM_SCOPE_MIN, const RandNumType rMax = ((std::numeric_limits<RandNumType>::max)() & MAX_I64_NUM)) :rGen(rMin, rMax)
		{

		}
	};

	template<typename RandNumType>
	struct Disor<RandNumType, RAND_DIS_TYPE_LOGNORMAL>
	{
		boost::random::lognormal_distribution<RandNumType> rGen;
		Disor(const RandNumType rMin = RAND_DIS_NUM_SCOPE_MIN, const RandNumType rMax = ((std::numeric_limits<RandNumType>::max)() & MAX_I64_NUM)) :rGen(rMin, rMax)
		{

		}
	};

	template<typename RandNumType>
	struct Disor<RandNumType, RAND_DIS_TYPE_ON_SPHERE>
	{
		boost::random::uniform_on_sphere<RandNumType> rGen;
		Disor(const RandNumType rMin = RAND_DIS_NUM_SCOPE_MIN, const RandNumType rMax = ((std::numeric_limits<RandNumType>::max)() & MAX_I64_NUM)) :rGen(rMin, rMax)
		{

		}
	};

	template<typename RandNumType>
	struct Disor<RandNumType, RAND_DIS_TYPE_BETA>
	{
		boost::random::beta_distribution<RandNumType> rGen;
		Disor(const RandNumType rMin = RAND_DIS_NUM_SCOPE_MIN, const RandNumType rMax = ((std::numeric_limits<RandNumType>::max)() & MAX_I64_NUM)) :rGen(rMin, rMax)
		{

		}
	};

	template<typename RandNumType>
	struct Disor<RandNumType, RAND_DIS_TYPE_BINOMIAL>
	{
		boost::random::binomial_distribution<RandNumType> rGen;
		Disor(const RandNumType rMin = RAND_DIS_NUM_SCOPE_MIN, const RandNumType rMax = ((std::numeric_limits<RandNumType>::max)() & MAX_I64_NUM)) :rGen(rMin, rMax)
		{

		}
	};

	template<typename RandNumType>
	struct Disor<RandNumType, RAND_DIS_TYPE_CAUCHY>
	{
		boost::random::cauchy_distribution<RandNumType> rGen;
		Disor(const RandNumType rMin = RAND_DIS_NUM_SCOPE_MIN, const RandNumType rMax = ((std::numeric_limits<RandNumType>::max)() & MAX_I64_NUM)) :rGen(rMin, rMax)
		{

		}
	};

	template<typename RandNumType>
	struct Disor<RandNumType, RAND_DIS_TYPE_DISCRETE>
	{
		boost::random::discrete_distribution<RandNumType> rGen;
		Disor(const RandNumType rMin = RAND_DIS_NUM_SCOPE_MIN, const RandNumType rMax = ((std::numeric_limits<RandNumType>::max)() & MAX_I64_NUM)) :rGen(rMin, rMax)
		{

		}
	};

	//随机数源
protected:

	template<typename RandNumType, RAND_GEN_ALGORITHM_TYPE>
	struct RandSrc
	{

	};

	template<>
	struct RandSrc<RandNumType, RAND_GEN_ALGORITHM_TYPE_RAND48>
	{
		boost::random::rand48	rGen;
		RandSrc(const RandNumType rSRand = time(0)) :rGen(rSRand)
		{
		}
	};

	template<>
	struct RandSrc<RandNumType, RAND_GEN_ALGORITHM_TYPE_MT19937>
	{
		boost::random::mt19937	rGen;
		RandSrc(const RandNumType rSRand = time(0)):rGen(rSRand)
		{
		}
	};

	template<>
	struct RandSrc<RandNumType, RAND_GEN_ALGORITHM_TYPE_MT19937_64>
	{
		boost::random::mt19937_64	rGen;
		RandSrc(const RandNumType rSRand = time(0)) :rGen(rSRand)
		{
		}
	};

	template<>
	struct RandSrc<RandNumType, RAND_GEN_ALGORITHM_TYPE_LAGGED_FIBONACCI19937>
	{
		boost::random::lagged_fibonacci19937	rGen;
		RandSrc(const RandNumType rSRand = time(0)) :rGen(rSRand)
		{
		}
	};

	Disor<RandNumType, eDisType>		m_Disor;
	RandSrc<RandNumType, eGenType>		m_RandSrcor;
};

NAMESPACE_END(gtool)


#endif

