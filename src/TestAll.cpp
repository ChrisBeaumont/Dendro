/*
 * TestAll.cpp
 *
 *  Created on: Jan 6, 2011
 *      Author: beaumont
 */

#include "TestImage.h"
#include "Suite.h"
#include "TestDendro.h"

int Testmain() {
	TestSuite::Suite suite("Dendro tests");
	suite.addTest(new TestImage);
	suite.addTest(new TestDendro);
	suite.run();
	long nfail=suite.report();
	suite.free();
	return nfail;
}
