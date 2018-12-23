//
//  main.cpp
//  NAND
//
//  Created by Gabriel Beauchemin on 2018-12-20.
//  Copyright © 2018 Gabriel Beauchemin. All rights reserved.
//

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include "wire.hpp"
#include "clock.hpp"
#include "dgate.hpp"

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

int main(int argc, const char * argv[])
{
	testing::InitGoogleMock(&argc, const_cast<char**>(argv));
	return RUN_ALL_TESTS();
}


TEST(basic, wire)
{
	MockCallback cb;
	wire_t<> wire;
	wire.attach(cb.cb());
	
	EXPECT_CALL(cb, out(true)).Times(testing::Exactly(1));
	wire.in(true);

	EXPECT_CALL(cb, out(false)).Times(testing::Exactly(1));
	wire.in(false);
}

TEST(basic, wire_n)
{
	MockCallback cb;
	wire_t<4> wire;

	for (auto i = 0; i < wire.size(); i++)
		wire.attach(cb.cb(), i);
	
	EXPECT_CALL(cb, out(true)).Times(testing::Exactly(wire.size()));
	wire.in(true);

	EXPECT_CALL(cb, out(false)).Times(testing::Exactly(wire.size()));
	wire.in(false);
}


TEST(basic, clock)
{
}

TEST(basic, dgate)
{
}
