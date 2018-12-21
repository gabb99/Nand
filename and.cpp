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
