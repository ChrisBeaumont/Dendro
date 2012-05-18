/*
 * dendro.h
 *
 *  Created on: Dec 28, 2010
 *      Author: beaumont
 */
#ifndef DENDRO_H
#define DENDRO_H

#include <queue>
#include <vector>
#include <valarray>
#include <iostream>
#include <stdexcept>
#include <map>
#include "cbImage.h"

/**
 * @class Node
 *
 * @brief Labels for an individual pixel
 */
class Node {
  
 private:
  float _inten; ///< intensity of node in the data

 public:
  unsigned int index; ///< 1D array index giving this node's location
  int id; ///< the dendrogram structure ID of this node when it was inserted into the queue
  int parent; ///< index of node which was processed when this node was inserted into queue

  /**
   * Constructor method
   */
 Node(int index=0, float inten=0, int id=0, int parent=-1) : _inten(inten), index(index),
    id(id), parent(parent) {}

  /**
   * Less than operator, which compares the intensities of two nodes. N1<N2 if N1.inten < N2.inten
   */
  bool operator<(const Node& b) const{
    return _inten < b._inten;
  }

  float inten() const{return _inten;}
};


/**
 * Class used to create dendrograms from data
 */
class dendro {
  static const int unassigned = -1; ///< default value for un-visited nodes
  bool allNeighbors;
  
  std::priority_queue<Node> q; ///< holds the nodes in the proper order
  int nkernels; ///< number of kernels
  size_t top_id; ///< the largest assigned dendro ID + 1
  
  cbImage<float> data; ///< the data
  std::vector<size_t> kernels; ///< 1D indices for the kernels
  std::vector<int> creation; ///< the (2*i, 2*i+1) entry lists which 2 dendro IDs merge to make ID i. -1 for leaves.
  std::vector<int> destruction; ///< ith entry lists what ID structure i merges with
  std::vector<int> current; ///< ith entry hops along destruction until if lands on a non-destroyed ID.
  std::valarray<int> id;   ///< id of each pixel
  std::vector<int> added_id; ///< temporary id each pixel is given when added to queue
  
  /**
   * Re-calculate the current vector
   */
  void updateCurrent();
  
  /**
   * Sanity checks
   */
  bool enforceAssertions();
  
  
  /**
   * Create and return node objects for each neighbor of a given node
   */
  void neighbors(Node front, ///< the node to find neighbors for
		 int id, ///< the Dendrogram ID to assign to each neighbor node
		 std::vector<Node>& result
		 );
  
  
 public:
  
  /**
   * Constructor method.
   */
 dendro(cbImage<float>& data, ///< the data
	const std::vector<size_t>& kernels ///< kernels to seed the dendrogram
	):
  allNeighbors(false), top_id(0), data(data), kernels(kernels.begin(), kernels.end())
    {
      
      nkernels = kernels.size();
      int nst = 2 * nkernels - 1;
      
      creation.resize(2 * nst, -1);
      destruction.resize(nst, -1);
      current.resize(nst, -1);
      id.resize(data.getNElements(), -1);
      added_id.resize(data.getNElements(), -1);
    }
  
  /**
   * Constructs the dendrogram
   */
  void construct();
  
  void prune(size_t minpix, float delta);
  
  std::valarray<int>& getID() {return id;}
  std::vector<size_t>& getKernels() {return kernels;}
  std::vector<int>& getCreation() {return creation;}
  std::vector<int>& getDestruction() {return destruction;}
};

#endif
