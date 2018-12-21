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
