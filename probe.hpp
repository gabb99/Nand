//
//  probe.hpp
//  NAND
//
//  Created by Claude Opus 5 on 2026-08-30.
//  Copyright © 2026 Gabriel Beauchemin. All rights reserved.
//

#ifndef probe_hpp
#define probe_hpp

#include <functional>
#include <optional>

#include "gtest/gtest.h"

// Test helper: records what a wire actually delivered.
//
// The gate tests used to assert on how many times a callback fired, with
// Times(Exactly(n)). That count is not part of any gate's contract - it is an
// artifact of how far a given change happens to propagate, so it says nothing
// about whether the logic is right and it breaks the moment the propagation
// model changes. A probe asserts the thing that IS the contract: the last
// value a wire delivered is the value the gate settled on.
class probe_t
{
	std::optional<bool> m_last;
	unsigned            m_calls = 0;

public:
	std::function<void(bool)> cb()
	{
		return [this](bool value) { m_last = value; ++m_calls; };
	}

	// Seed with the value the wire is already holding when the probe is
	// attached, so that "last delivered" is meaningful before the first event.
	void seed(bool value) { m_last = value; }

	bool     driven() const { return m_last.has_value(); }
	bool     last() const   { return m_last.value(); }
	unsigned calls() const  { return m_calls; }
};

// The wire delivered what the gate settled on.
inline ::testing::AssertionResult delivered(const probe_t& p, bool expected)
{
	if (!p.driven())
		return ::testing::AssertionFailure() << "wire was never driven";

	if (p.last() != expected)
		return ::testing::AssertionFailure()
			<< "wire last delivered " << p.last() << ", gate settled on " << expected;

	return ::testing::AssertionSuccess();
}

#endif /* probe_hpp */
