/*
 * TestImage.h
 *
 *  Created on: Jan 6, 2011
 *      Author: beaumont
 */

#ifndef TESTIMAGE_H_
#define TESTIMAGE_H_
#include "../TestSuite/Test.h"
#include "cbImage.h"
#include <algorithm>
#include <vector>

static const int data[42] =
{0, 0, 1, 1, 5, 0,
		0, 2, 0, 0, 0, 0,
		0, 3, 2, 0, 0, 0,
		0, 2, 2, 0, 0, 0,
		0, 1, 1, 1, 0, 0,
		0, 0, 3, 2, 0, 0,
		0, 0, 0, 0, 0, 0};

using namespace std;
class TestImage: public TestSuite::Test {
private:



	valarray<int> intarr;
	cbImage<int> intim;

public:
	TestImage(): intarr(data, 42), intim(intarr, 2, 6, 7){}

	void resetData() {
		intarr.resize(42);
		for(int i = 0; i < 42; i++) intarr[i] = data[i];
		test_(&intarr == &intim.getData());
	}

	void run() {
		cout << "testDims" << endl;
		testDims();
		cout << "testGetData" << endl;
		testGetData();
		cout << "testSetData" << endl;
		testSetData();
		cout << "testNiehgbors" << endl;
		testNeighbors();
		cout << "testLocalMaxima" << endl;
		testLocalMaxima();
		cout << "testExceptions" << endl;
		testExceptions();
	}

	void testDims() {
		test_(intim.getNDim() == 2);
		test_(intim.getXDim() == 6);
		test_(intim.getYDim() == 7);
		test_(intim.getZDim() == 0);
		test_(intim.getNElements() == 42);
		test_(intim.index(1, 0, 0) == 1);
		test_(intim.index(0, 1, 0) == 6);
		test_(intim.getX(6) == 0);
		test_(intim.getY(6) == 1);
		test_(intim.getZ(41) == 0);
	}

	void testGetData() {
		resetData();
		valarray<int> &d2 = intim.getData();
		test_(&d2 == &intarr);
		test_(intim.getElement(0, 0) == intarr[0]);
		for(size_t i = 0; i < intim.getNDim(); i++) {
			test_(intim.getElement( intim.getX(i), intim.getY(i)) == intarr[i]);
			test_(intim[i] == intarr[i]);
		}
	}

	void testSetData() {
		resetData();
		for(size_t x = 0; x < intim.getXDim(); x++) {
			for(size_t y=0; y < intim.getYDim(); y++) {
				intim.setElement((int)(x+y), x, y);
				test_(intim.getElement(x, y) == (int)(x + y));
			}
		}
	}


	void testNeighbors() {
		resetData();
		vector<size_t> neighbors;
		intim.neighbors(0, neighbors);

		test_(neighbors.size() == 3);
		test_(count(neighbors.begin(), neighbors.end(), 1) == 1);
		test_(count(neighbors.begin(), neighbors.end(), 6) == 1);
		test_(count(neighbors.begin(), neighbors.end(), 7) == 1);

		{
			intim.neighbors(7, neighbors);
			test_(neighbors.size() == 8);
			int answer[8] = {0, 1, 2, 6, 8, 12, 13, 14};
			for(int i = 0; i < 8; i++)
				test_(count(neighbors.begin(), neighbors.end(), answer[i]) == 1);
		}

		{
			intim.neighbors(7, neighbors, false);
			int answer[4] = {1, 6, 8, 13};
			for(int i = 0; i < 4; i++)
				test_(count(neighbors.begin(), neighbors.end(), answer[i]) == 1);
		}
	}

	void testLocalMaxima() {
		resetData();
		vector<size_t> max;
		{
			intim.localMaxima(max, 1, 0);
			cout << "Max size: " << max.size() << endl;
			for(size_t i = 0; i < max.size(); i++) cout << max[i] << endl;
			test_(max.size() == 3);
			int answer[3] = {4, 13, 32};
			for(int i = 0; i < 2; i++)
				test_(count(max.begin(), max.end(), answer[i]) == 1);
		}

		{
			intim.localMaxima(max, 3, 0);
			test_(max.size() == 1);
			test_(count(max.begin(), max.end(), 4) == 1);
		}


	}

	void testExceptions() {
		try{
			intim.setElement(3, -1, 0);
			fail_("Did not catch bad lo x argument");
		} catch(out_of_range& o) {
			succeed_();
		}

		try{
			intim.setElement(3, 6, 0);
			fail_("Did not catch bad hi x argument");
		} catch(out_of_range& o) {
			succeed_();
		}

		try{
			intim.setElement(3, 0, -1);
			fail_("Did not catch bad lo y argument");
		} catch(out_of_range& o) {
			succeed_();
		}

		try{
			intim.setElement(3, 0, 7);
			fail_("Did not catch bad hi y argument");
		} catch(out_of_range& o) {
			succeed_();
		}

		try{
			intim.setElement(3, 0, 0, -1);
			fail_("Did not catch bad lo z argument");
		} catch(out_of_range& o) {
			succeed_();
		}

		try{
			intim.setElement(3, 0, 0, 1);
			fail_("Did not catch bad lo z argument");
		} catch(out_of_range& o) {
			succeed_();
		}

		try{
			intim.index(-1, 0, 0);
			fail_("Did not catch bad index for index");
		} catch(out_of_range& o) {
			succeed_();
		}

		try{
			intim.getX(-1);
			fail_("Did not catch bad index for getX");
		} catch(out_of_range& o) {
			succeed_();
		}

		try{
			intim.getY(-1);
			fail_("Did not catch bad index for getY");
		} catch(out_of_range& o) {
			succeed_();
		}

		try{
			intim.getZ(-1);
			fail_("Did not catch bad index for getZ");
		} catch(out_of_range& o) {
			succeed_();
		}

		try{
			intim.getElement(-1,0);
			fail_("Did not catch bad index for getElement");
		} catch(out_of_range& o) {
			succeed_();
		}


		try{
			vector<size_t> neighbors;
			intim.neighbors(-1, neighbors);
			fail_("Did not catch bad index for neighbors");
		} catch(out_of_range& o) {
			succeed_();
		}

		try{
			int data[5] = {1,2,3,4,5};
			valarray<int> arr(data, 5);
			cbImage<int> test(arr, 2, 5, 2);
			fail_("Didn't throw exception for bad object instantiation");
		} catch(invalid_argument& ia) {
			succeed_();
		}

		try{
			vector<size_t> max;
			intim.localMaxima(max, 0, 0);
			fail_("Didn't throw exception for bad friends value");
		} catch(invalid_argument& ia){
			succeed_();
		}

	}


};

#endif /* TESTIMAGE_H_ */
