/*
 * TestDendro.h
 *
 *  Created on: Jan 6, 2011
 *      Author: beaumont
 */

#ifndef TESTDENDRO_H_
#define TESTDENDRO_H_

#include "../TestSuite/Test.h"
#include "dendro.h"

float DendroData[30] =
      { .55, .51,   0, .44,   0,   0,
    	.51,   0, .42,   0,   0,   0,
    	  0,   0, .13,   0,   0,   0,
    	.31, .33, .15, .22,   0, .04,
    	  0,   0,   0, .21, .05, .11};

size_t k[5] = {0, 3, 19, 21, 29};
int merger[26] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
				  0, 1, 2, 3, 5, 6, 7, 4};

class TestDendro: public TestSuite::Test {
	std::valarray<float> arr;
	cbImage<float> im;
	std::vector<size_t> kernels;
	dendro d;

public:
	TestDendro(): arr(DendroData, 30), im(arr, 2, 6, 5), kernels(k, k + 6), d(im, kernels) {
		assert(kernels.size() == 6);
	}

	void run() {
		d.construct();
		std::valarray<int> result = d.getID();
		std::vector<int> creation = d.getCreation();
		for(int i = 0; i < 9; i++) {
			std::cout << creation[2*i] << "\t" << creation[2*i+1] << std::endl;
			test_(creation[2 * i] == merger[2 * i] &&
				 creation[2 * i + 1] == merger[2*i+1] ||
				 creation[2 * i] == merger[2*i+1] &&
				 creation[2*i+1] == merger[2*i]);
		}
	}

};

#endif /* TESTDENDRO_H_ */
