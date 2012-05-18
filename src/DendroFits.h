/*
 * DendroFits.h
 *
 *  Created on: Mar 11, 2011
 *      Author: beaumont
 */

#ifndef DENDROFITS_H_
#define DENDROFITS_H_

#include <vector>
#include "cbImage.h"
#include "dendro.h"
#include <CCfits>

class DendroFits {
private:
	std::string fileName;
	std::string outName;
	std::string kernelName;
	dendro *d;
	std::vector<size_t> kernels;
	std::valarray<float> contents;
	cbImage<float> *data;
	int friends, specfriends;
	float minvalue;
	std::auto_ptr<CCfits::FITS> outFits;
	std::auto_ptr<CCfits::FITS> inFits;

	void setupFiles();
	void readFile();
	void computeKernels();
	void readKernels();
	void calculateDendrogram();
	void saveResults();

public:
	DendroFits(std::string fileName, int friends = 1, int specfriends = 1,
			float minvalue = 0., std::string outName = "", std::string kernelName = "") :
		fileName(fileName), outName(outName), kernelName(kernelName), friends(friends), specfriends(specfriends),
				minvalue(minvalue), outFits(0), inFits(0){}

	void run();
	~DendroFits() {
		free(data);
	}

};

#endif /* DENDROFITS_H_ */
