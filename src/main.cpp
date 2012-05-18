/*
 * main.c
 *
 *  Created on: Mar 11, 2011
 *      Author: beaumont
 */
#include <iostream>
#include "DendroFits.h"

using namespace std;

void usage() {
	cerr << "Usage: " << endl;
	cerr << " dendro fileName -f friends -s specfriends -m minval -o outfile" << endl;
	exit(1);
}

int main(int argc, char* argv[]) {
	if (argc < 2) usage();
	DendroFits *df;

	int friends(1), specfriends(1);
	float minval(0);
	string outfile, infile, kernels;
	bool haveInFile = false;
	int i = 1;
	cout << "parsing" << endl;
	while( i < argc) {
	  if(argv[i][0] != '-') {
	    infile = argv[i];
	    i++;
	    haveInFile = true;
	    continue;
	  } else {
	    switch (argv[i][1]) {
	    case 'f':
	      friends = atoi(argv[i+1]);
	      i += 2;
	      break;
	    case 's':
	      specfriends = atoi(argv[i+1]);
	      i += 2;
	      break;
	    case 'm':
	      minval = atof(argv[i+1]);
	      i += 2;
	      break;
	    case 'o':
	      outfile = argv[i+1];
	      i += 2;
	      break;
	    case 'k':
	      kernels = argv[i+1];
	      i += 2;
	      break;
	    default:
	      usage();
	    }
	  }
	}
	if (!haveInFile) usage();
	
	df = new DendroFits(infile, friends, specfriends, minval, outfile, kernels);
	df->run();
}

