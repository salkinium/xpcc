// coding: utf-8
/* Copyright (c) 2016, Roboterclub Aachen e.V.
 * All Rights Reserved.
 *
 * The file is part of the xpcc library and is released under the 3-clause BSD
 * license. See the file `LICENSE` for the full license governing this code.
 */
// ----------------------------------------------------------------------------

#include <string.h>
#include <xpcc/architecture/interface/assert.hpp>

using xpcc::AssertionHandler;
using xpcc::Abandonment;

extern AssertionHandler __assertion_table_start;
extern AssertionHandler __assertion_table_end;

extern "C"
{

void
xpcc_assert_fail(const char * identifier)
{
	// just forward this call
	xpcc_assert_fail_context(identifier, 0);
}

void xpcc_assert_fail_context(const char * identifier, uintptr_t context)
{
	uint8_t state(uint8_t(Abandonment::DontCare));
	const char * module = identifier;
	const char * location = module + strlen(module) + 1;
	const char * failure = location + strlen(location) + 1;

	AssertionHandler * handler = &__assertion_table_start;
	for (; handler < &__assertion_table_end; handler++)
	{
		state |= (uint8_t) (*handler)(module, location, failure, context);
	}

	if (state == (uint8_t) Abandonment::DontCare or
		state & (uint8_t) Abandonment::Fail)
	{
		xpcc_abandon(module, location, failure, context);
		while(1) ;
	}
}

xpcc_weak
void xpcc_abandon(const char *, const char *, const char *, uintptr_t) {}

}
