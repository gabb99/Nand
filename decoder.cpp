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

	// Feeds every possible word to a decoder_t<N> and checks that exactly the
	// matching line is high, both by polling out() and by looking at what the
	// output wires actually delivered.
	template <unsigned N>
	void exhaustive()
	{
		constexpr unsigned SIZE = decoder_t<N>::SIZE;

		decoder_t<N> decoder;

		std::array<bool, SIZE> delivered;
		std::array<bool, SIZE> driven;
		delivered.fill(false);

		for (unsigned i = 0; i < SIZE; i++)
			decoder.attach([&, i](bool value) { delivered[i] = value; driven[i] = true; }, i);

		for (unsigned word = 0; word < SIZE; word++)
		{
			driven.fill(false);

			decoder.in(std::bitset<N>(word));

			std::bitset<SIZE> expected;
			expected.set(word);

			EXPECT_EQ(decoder.out(), expected) << "word " << word;

			for (unsigned i = 0; i < SIZE; i++)
			{
				EXPECT_TRUE(driven[i]) << "line " << i << " never driven for word " << word;
				EXPECT_EQ(delivered[i], expected.test(i)) << "line " << i << ", word " << word;
			}
		}
	}
}


TEST(basic, decoder_1) { exhaustive<1>(); }
TEST(basic, decoder_2) { exhaustive<2>(); }
TEST(basic, decoder_3) { exhaustive<3>(); }
TEST(basic, decoder_4) { exhaustive<4>(); }


TEST(basic, decoder_zero)
{
	// All zeros selects line 0, and the constructor settles the gates so that
	// this holds before anything is fed in.
	decoder_t<3> decoder;

	std::bitset<decoder_t<3>::SIZE> expected;
	expected.set(0);

	EXPECT_EQ(decoder.out(), expected);
}


TEST(basic, decoder_callback)
{
	decoder_t<2> decoder;
	MockCallback cb;

	decoder.attach(cb.cb(), 1);

	// Every input write re-drives every gate, so the count of calls is an
	// artifact of the propagation order. What matters is that the line is
	// driven, and that it settles high for its own word.
	EXPECT_CALL(cb, out(false)).Times(testing::AnyNumber());
	EXPECT_CALL(cb, out(true)).Times(testing::AtLeast(1));

	decoder.in(std::bitset<2>(1));
	EXPECT_TRUE(decoder.out(1));

	decoder.in(std::bitset<2>(2));
	EXPECT_FALSE(decoder.out(1));
}
