//
//  main.cpp
//  NAND
//
//  Created by Gabriel Beauchemin on 2018-12-20.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "clock.hpp"
#include "nand.hpp"
#include "dgate.hpp"

int main(int argc, const char * argv[])
{
	testing::InitGoogleMock(&argc, const_cast<char**>(argv));
	return RUN_ALL_TESTS();
}


TEST(basic, wire)
{
}

TEST(basic, nand)
{
}

TEST(basic, clock)
{
}

TEST(basic, dgate)
{
}
