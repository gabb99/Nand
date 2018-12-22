//
//  and.cpp
//  NAND
//
//  Created by Gabriel Beauchemin on 2018-12-20.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "and.hpp"

TEST(basic, and)
{
	and_t<> and_;
	
	and_.in(false, 0);
	and_.in(false, 1);
	EXPECT_FALSE(and_.out());
	
	and_.in({true, false});
	EXPECT_FALSE(and_.out());
	
	and_.in(false, 0);
	and_.in(true, 1);
	EXPECT_FALSE(and_.out());
	
	and_.in({true, true});
	EXPECT_TRUE(and_.out());
}

TEST(basic, and_n)
{
	constexpr auto n = 16;
	
	and_t<n> and_;
	
	std::bitset<n> b;
	and_.in(b); // Test all zeros
	EXPECT_FALSE(and_.out());
	
	for (auto i = 0; i < n; i++)
	{
		and_.in(true, i);
		EXPECT_FALSE(and_.out());
		and_.in(false, i);
	}
	
	b.flip();
	and_.in(b); // Test all ones
	EXPECT_TRUE(and_.out());
}
