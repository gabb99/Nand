//
//  not.cpp
//  NAND
//
//  Created by Gabriel Beauchemin on 2018-12-20.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "not.hpp"

TEST(basic, not)
{
	not_t not_;

	not_.in(false);
	EXPECT_TRUE(not_.out());
	
	not_.in(true);
	EXPECT_FALSE(not_.out());
}
