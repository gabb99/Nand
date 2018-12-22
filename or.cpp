//
//  or.cpp
//  NAND
//
//  Created by Gabriel Beauchemin on 2018-12-20.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "or.hpp"

TEST(basic, or)
{
	or_t<> or_;
	
	or_.in(false, 0);
	or_.in(false, 1);
	EXPECT_FALSE(or_.out());
	
	or_.in({true, false});
	EXPECT_TRUE(or_.out());
	
	or_.in(false, 0);
	or_.in(true, 1);
	EXPECT_TRUE(or_.out());
	
	or_.in({true, true});
	EXPECT_TRUE(or_.out());
}

TEST(basic, or_n)
{
	constexpr auto n = 16;
	
	or_t<n> or_;
	
	std::bitset<n> b;
	or_.in(b); // Test all zeros
	EXPECT_FALSE(or_.out());
	
	for (auto i = 0; i < n; i++)
	{
		or_.in(true, i);
		EXPECT_TRUE(or_.out());
		or_.in(false, i);
	}
	
	b.flip();
	or_.in(b); // Test all ones
	EXPECT_TRUE(or_.out());
}
