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
	nand_t<> nand;
	
	nand.in(false, 0);
	nand.in(false, 1);
	EXPECT_FALSE(nand.out());
	
	nand.in({true, false});
	EXPECT_TRUE(nand.out());
	
	nand.in(false, 0);
	nand.in(true, 1);
	EXPECT_TRUE(nand.out());
	
	nand.in({true, true});
	EXPECT_TRUE(nand.out());
}
