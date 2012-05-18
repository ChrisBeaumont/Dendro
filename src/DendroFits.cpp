/*
 * DendroFits.cpp
 *
 *  Created on: Jan 18, 2011
 *      Author: beaumont
 */

#include "dendro.h"
#include "DendroFits.h"
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <string>
#include "float.h"

using namespace std;
using namespace CCfits;


void DendroFits::setupFiles() {
  cout << "reading file" << endl;
  
  // read input file
  try {
    inFits.reset(new FITS(fileName, Read, true));
  } catch (exception e) {
    cerr << "Error reading input file: " << fileName << endl << "aborting." << endl;
    exit(1);
  }
  
  cout << "Setting up keywords" << endl;
  PHDU& image = inFits->pHDU();
  cout << "reading keys" << endl;
  image.readAllKeys();
  cout << "naxis" << endl;
  long naxis(image.axes());
  long ax1(image.axis(0));
  long ax2(image.axis(1));
  long ax3(image.axis(2));
  long naxes[3] = {ax1, ax2, ax3};

  cout << "setting up output file" << endl;
  // setup output file
  if (outName == "") {
    size_t pos = fileName.rfind(".fit");
    outName = fileName.substr(0, pos).append("_dendro.fits");
    
  }
  try {
    outFits.reset( new FITS(outName, FLOAT_IMG, naxis, naxes));
  }
  catch (FITS::CantCreate) {
    cerr << "Can't create output fits file (File Exists?). Aborting" << endl;
    exit(1);
  }
  
  cout << "copying keywords" << endl;
  // copy keywords from input file to output file
  outFits->pHDU().copyAllKeys(&image);
}

void DendroFits::readFile() {
  
  PHDU& image = inFits->pHDU();
  
  if(image.bitpix() != -32) {
    cerr << "Error: Fits image must be of type FLOAT" << endl;
    exit(1);
  }
  
  image.readAllKeys();
  image.read(contents);
  
  long nax(image.axes());
  long ax1(image.axis(0));
  long ax2(image.axis(1));
  long ax3(image.axis(2));
  
  // remove nans
  float small = FLT_MAX;
  for (size_t i=0; i < contents.size(); i++) {
	  if (contents[i] < small) {
		  small = contents[i];
	  }
  }

  for (size_t i=0; i < contents.size(); i++) {
	  if (contents[i] != contents[i]) {
		  contents[i] = small;
	  }
  }

  if(nax == 2) {
    data = new cbImage<float>(contents, 2, ax1, ax2);
  } else if (nax == 3) {
    data = new cbImage<float>(contents, 3, ax1, ax2, ax3);
  } else {
    cerr << "Error: Fits file must be a 2- or 3-D image:" << nax << endl;
    exit(1);
  }
}

void DendroFits::computeKernels() {
  data->localMaxima(kernels, friends, specfriends, minvalue);
  if (kernels.size() == 0) {
    cerr << "Didn't find any local maxima that meet your criteria. Aborting." << endl;
    exit(1);
  }
}

void DendroFits::readKernels() {
	string line;
	size_t pos;
	int index;
	float inten;
	kernels.clear();

	ifstream kfile (kernelName.c_str());
	if (kfile.is_open()) {
		bool good=false;
		while (kfile.good()) {
			getline(kfile, line);
			pos = line.find(',');
			if (pos == string::npos) {
				if (good) continue;
				cerr << "Error parsing line " << line << endl;
				exit(1);
			}
			good = true;
			assert(pos != string::npos);

			index = atoi(line.substr(0, pos).c_str());
			inten = atof(line.substr(pos+1, line.length() - pos).c_str());
			if (abs(data->operator[](index) - inten) > .001) {
				cerr << "kernel index/intensity doesn't match " << inten << " " <<
							data->operator[](index) << " "<< index << endl;
				exit(1);
			}
			kernels.push_back(index);
		}
		kfile.close();
	} else{
		cerr << "Could not open file: " << kernelName << endl;
		exit(1);
	}
}

void DendroFits::calculateDendrogram() {
  d = new dendro(*data, kernels);
  d->construct();
}

void DendroFits::saveResults() {
  long naxis = data->getNDim();
  long naxes[3] = {data->getXDim(), data->getYDim(), data->getZDim()};
  vector<long> vecAx(naxis);
  vecAx[0] = naxes[0]; vecAx[1] = naxes[1];
  if (naxis == 3) vecAx[2] = naxes[2];
  
  
  long nelements = naxes[0] * naxes[1] * (naxis == 2 ? 1 : naxes[2]);
  
  //write structure ID image to extension 1
  string idName = "index_map";
  ExtHDU* idExt = outFits->addImage(idName, SHORT_IMG, vecAx);
  valarray<int> &id = d->getID();
  long first_pixel = 1;
  idExt->write(first_pixel, nelements, id);
  
  //write clusters array to extension 2
  vector<int> creation = d->getCreation();
  valarray<int> vc(creation.size());
  for(size_t i = 0; i < vc.size(); i++) vc[i] = creation[i];
  string clusterName = "Clusters";
  vector<long> ax(2); ax[0] = 2; ax[1] = creation.size() / 2;
  ExtHDU* cExt = outFits->addImage(clusterName, SHORT_IMG, ax);
  cExt->write(first_pixel, creation.size(), vc);
  
  //write input parameters to the header
  outFits->pHDU().addKey("INFILE", this->fileName,"");
  outFits->pHDU().addKey("FRIENDS", this->friends,"");
  outFits->pHDU().addKey("SPECFRIENDS", this->specfriends,"");
  outFits->pHDU().addKey("MINVALUE", this->minvalue,"");
  
  //write intensity to primary extension
  outFits->pHDU().write(first_pixel, nelements, this->contents);
  
  //write indices of seeds to extension 3
  string seedName = "seeds";
  vector<long> seedAx(2); seedAx[0] = 1; seedAx[1] = kernels.size();
  ExtHDU* seedExt = outFits->addImage(seedName, LONG_IMG, seedAx);
  valarray<int> vs(kernels.size());
  for(size_t i = 0; i < kernels.size(); i++) vs[i] = kernels[i];
  seedExt->write(first_pixel, kernels.size(), vs);
}

void DendroFits::run() {
  cout << "Setting up file" << endl;
  setupFiles();
  cout << "Reading fits file" << endl;
  readFile();
  if (kernelName.compare("") != 0) {
	  cout << "Reading Kernels" << endl;
	  readKernels();
  } else{
	  cout << "Computing Kernels" << endl;
	  computeKernels();
  }
  cout << " Found " << kernels.size() << " kernels" << endl;
  cout << "Calculating Dendrogram" << endl;
  calculateDendrogram();
  cout << "Writing Results" << endl;
  saveResults();
}

