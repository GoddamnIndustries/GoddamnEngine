// ==========================================================================================
// Copyright (C) Goddamn Industries 2018. All Rights Reserved.
// 
// This software or any its part is distributed under terms of Goddamn Industries End User
// License Agreement. By downloading or using this software or any its part you agree with 
// terms of Goddamn Industries End User License Agreement.
// ==========================================================================================

/*! 
 * @file GoddamnEngine/Core/Math/Random.cpp
 * File contains pseudo-random numbers generating algorithms.
 */
#include <GoddamnEngine/Core/Math/Random.h>

GD_NAMESPACE_BEGIN

	/*!
	 * Global pseudo-random generator.
	 */
	GDAPI extern LGCRandom GlobalPseudoRandom(/*0x25175534*/0);

	// ------------------------------------------------------------------------------------------
	// LGC random algorithm with varying coefficients.
	// ------------------------------------------------------------------------------------------

	/*!
	 * Initializes a new LGC random generator with varying coefficients.
	 * @param seed Initial seed value.
	 */
	GDAPI LGCRandom::LGCRandom(UInt32 const seed)
		: m_Seed(seed)
	{
		if (m_Seed == 0)
		{
			m_Seed = static_cast<UInt32>(reinterpret_cast<SizeTp>(&seed));
			m_Seed *= m_Seed;
			m_Seed += clock();
		}
	}

	/*!
	 * Generates the pseudo-random unsigned 32-bit integer using specified random algorithm.
	 * @returns Unsigned 32-bit integer in range [0; UInt32Max].
	 */
	GDAPI UInt32 LGCRandom::GenerateUnsigned32Impl()
	{
		UInt32 static const coefficientsCount = 100;
		UInt32 static const coefficients[coefficientsCount] = {
			149988415,	343647139,	262004706,	402761608,	215163758,
			72344579,	15036790,	371252000,	268766739,	396255952,
			132761912,	13028976,	391406981,	120044652,	356838914,
			222039732,	113413892,	97103690,	314917710,	45725471,
			106100444,	354213518,	344084185,	199980274,	294164250,
			40978795,	113940007,	68312398,	101268760,	288777869,
			11875031,	193609607,	42338591,	248717393,	397617110,
			125263633,	388991068,	198173289,	358263743,	103335884,
			111957492,	280193201,	138863179,	22692308,	421471339,
			422576677,	68312136,	286869561,	373714292,	302174084,
			313958415,	180349214,	371784124,	177190432,	255810896,
			76106936,	209597811,	300656303,	388467118,	12544262,
			91038764,	422886527,	88854507,	257940817,	280918048,
			163992378,	294978827,	310503343,	180631444,	242298145,
			345279117,	181301236,	52654258,	396282893,	381432269,
			184038652,	165224776,	11065351,	130860438,	294826793,
			81996299,	92129356,	263132802,	252365147,	271053614,
			7417968,	252330449,	137806329,	114719959,	35936564,
			140614260,	156796616,	255068926,	348503787,	417100172,
			399366209,	278891095,	247478201,	339959845,	283012064,
		};

		UInt64 const newSeed = m_Seed * coefficients[~m_Seed % coefficientsCount] + coefficients[SwapEndian(m_Seed) % coefficientsCount];
		return m_Seed = static_cast<UInt32>(newSeed);
	}

GD_NAMESPACE_END
