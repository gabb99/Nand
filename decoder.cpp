//
//  decoder.cpp
//  NAND
//
//  Created by Gabriel Beauchemin on 2018-12-23.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "decoder.hpp"

namespace
{
	class Callback
	{
	public:
		Callback() {}
		virtual ~Callback() {}
		
		std::function<void(bool)> cb() { return [&](bool value) { out(value); }; }
		virtual void out(bool) {}
	};
	
	class MockCallback : public Callback
	{
	public:
		MOCK_METHOD1(out, void(bool));
	};
}


TEST(basic, decoder_2)
{
	decoder_t<2> decoder;
	std::array<MockCallback, decoder.SIZE> cb;

	for (unsigned i = 0; i < cb.size(); i++)
	{
		decoder.attach(cb[i].cb(), i);
		EXPECT_CALL(cb[i], out(true)).Times(testing::Exactly(1));
		EXPECT_CALL(cb[i], out(false)).Times(testing::Exactly(cb.size() - 1));
	}

	std::bitset<decoder.SIZE> res("1");
	for (unsigned i = 0; i < cb.size(); i++)
	{
		switch (i)
		{
			case 0: decoder.in(false, 0); decoder.in(false, 1); break;
			case 1: decoder.in({false, true}); break;
			case 2: decoder.in({true, false}); break;
			case 3: decoder.in(true, 0); decoder.in(true, 1); break;
		}

		EXPECT_EQ(decoder.out(), res);
		res <<= 1;
	}
}

// Cannot use INSTANTIATE_TYPED_TEST_CASE_P, since decoder_t uses constexpr and testing::Types is not constexpr
