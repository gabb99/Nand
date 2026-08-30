//
//  main.cpp
//  NAND
//
//  Created by Gabriel Beauchemin on 2018-12-20.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#include "gtest/gtest.h"
#include "gmock/gmock.h"


int main(int argc, const char * argv[])
{
	testing::InitGoogleMock(&argc, const_cast<char**>(argv));
	return RUN_ALL_TESTS();
}
