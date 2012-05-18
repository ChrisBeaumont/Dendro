/*
 * dendro.c
 *
 *  Created on: Dec 28, 2010
 *      Author: beaumont
 */


/**
 * NOTES on top_id
 * During construct, the value of top_id is the highest ID that has been assigned to something
 * There are 2*nleaf - 1 total structures. The highest ID is 2*nleaf - 2
 * Once top_id == 2*nleaf - 2, we have found all of the mergers, and can assign all remaining
 * un-labeled pixels to 2*nleaf - 2
 *
 */

/**
 * Notes on when adding a neighbor is redundant, and can be skipped
 * -- If neighbor has already been added with an id where current[old_id] == current[new_id]
 */
#include "dendro.h"
#include <iostream>
#include <cassert>


using namespace std;

void dendro::neighbors(Node front, int id, vector<Node>& result) {
	result.clear();
	vector<size_t> neighbors;
	data.neighbors(front.index, neighbors, allNeighbors);

	for(size_t i=0; i < neighbors.size(); i++) {
		Node n = Node(neighbors[i], data[neighbors[i]], id, front.index);
		result.push_back(n);
	}
}


bool dendro::enforceAssertions() {
	for(int i = 0; i <= (int)top_id; i++) {
		assert(current[i] != unassigned);
		assert(current[i] == current[current[i]]);
		assert(current[i] >= i);
		assert(current[i] != i || destruction[i] == unassigned);
		assert(current[i] == i || destruction[i] != unassigned);

		int c1 = creation[2*i]; c1 += 0;
		int c2 = creation[2*i+1]; c2 += 0;
		assert(c1 == unassigned || destruction[c1] == i);
		assert(c2 == unassigned || destruction[c2] == i);
	}
	return true;
}


void dendro::updateCurrent(){
#ifndef NDEBUG
	for(size_t i = 0; i < destruction.size(); i++) cout << destruction[i] << " ";
	cout << endl;
#endif

	for(size_t i = 0; i <= top_id; i++) {
		int c = i;

#ifndef NDEBUG
		cout << "updating id " << i << endl << "jumps: " << c << " ";
#endif
		while(destruction[c] != unassigned) {
			c = destruction[c];
#ifndef NDEBUG
			cout << c << " ";
		}
		cout << endl;
#else
		}
#endif
		current[i] = c;
	}
}


void dendro::construct() {
	top_id = kernels.size() - 1;

	// create nodes for each kernel, and push into queue
	for(size_t i = 0; i < kernels.size(); i++) {
		Node n(kernels[i], data[kernels[i]], i);
		q.push(n);
		current[i] = i;
		creation[2*i] = creation[2*i+1] = unassigned;
	}

	// process node at front of queue
	while(!q.empty() && top_id < 2 * kernels.size() - 2) {

		Node front = q.top();
		q.pop();
		assert(q.empty() || ! (front < q.top()));
		int front_current = current[front.id];

#ifndef NDEBUG
		cout << "node: index = " << front.index << " id = " << current[front.id] << " ";
#endif
		// pixel is unassigned. Label pixel, add neighbors to queue
		if (id[front.index] == unassigned) {

#ifndef NDEBUG
		cout << "-- new" << endl;
#endif

		id[front.index] = front_current;
			vector<Node> neighbors;
			this->neighbors(front, front_current, neighbors);
			for(size_t i = 0; i < neighbors.size(); i++) {
				int _id = id[neighbors[i].index];

				// only add if we haven't already added in the same hierarchy
				if (added_id[neighbors[i].index] != -1 &&
						current[added_id[neighbors[i].index]] == front_current) continue;
				assert(neighbors[i].id == front_current);

				// only add if we haven't already assigned to the same hierarchy
				if (_id != unassigned && current[_id] == front_current) continue;

				// only go "uphill" if the neighbor is unassigned
				if (_id != unassigned && neighbors[i].inten() > front.inten()) continue;

				//make sure we aren't adding a leaf
				bool skip=false;
				for(size_t z = 0; z < kernels.size(); z++) {
					if(kernels[z] == neighbors[i].index) {
						cout <<"Pushing a leaf to the queue! " << z << " "
								<< data[neighbors[i].index] << " " <<
								data[front.index] << " " << _id << endl;
						skip=true;
						break;
						Node old = q.top();
						q.pop();
						while(!q.empty()) {
							Node f = q.top();
							q.pop();

							cout << f.id << " " << f.inten() << " " << f.index << " " <<
									(f.index == neighbors[i].index) << endl;
							if (old < f) {
								cout << "Queue violation 1!" << endl;
							}
							if (old.inten() < f.inten()) {
								cout << "Queue violation 2!" << endl;
								exit(1);
							}
							if (f.index == neighbors[i].index) {
								exit(1);
							}
							old = f;
						}
						exit(1);
					}
				}
				if (!skip){
					q.push(neighbors[i]);
					added_id[neighbors[i].index] = front_current;
				}
			}
		}

		// node is already assigned with a conflicting label.
		// Merge the 2 labels into a new structure.
		else if (current[id[front.index]] != front_current) {
			cout << "Found merger " << (top_id - kernels.size() + 1) << " of " <<
					kernels.size() - 2 << ". Intensity: " << data[front.index] <<
					". Size of queue: " << q.size() << endl;
			// make sure that this isn't a leaf
			for(size_t z = 0; z < kernels.size(); z++) {
				if(kernels[z] == front.index) {
					cout << "ERROR: Trying to use a leaf as a merger point" << endl;
					cout << "Offending kernel location " << z << " " << front.index << " " <<
							data[front.index] << endl;
					break;
//					exit(1);
				}
			}

			int value2 = current[id[front.index]];
			int s = top_id + 1;
			top_id++;

			id[front.index] = s;

#ifndef NDEBUG
			cout << "-- merger " << endl;
			cout << "new id: " << s << " = (" << front_current << " , " << value2 << ")" << endl;
#endif

			destruction[front_current] = s;
			destruction[value2] = s;
			creation[2 * s] = front_current;
			creation[2 * s + 1] = value2;
			updateCurrent();
			assert(enforceAssertions());

		} else {
#ifndef NDEBUG
			cout << "-- old" << endl;
#endif
		}
	}
	assert(top_id == 2 * kernels.size() - 2);
	for(size_t i = 0; i < id.size(); i++)
		if(id[i] == unassigned) id[i] = top_id;

}

int dmain() {
	cout << "testing neighbors" << endl;
	cout << " done " << endl;

	float data[42] =
					{0, 0, 1, 1, 5, 0,
					0, 2, 0, 0, 0, 0,
					0, 3, 2, 0, 0, 0,
					0, 2, 2, 0, 0, 0,
					0, 1, 1, 1, 0, 0,
					0, 0, 3, 2, 0, 0,
					0, 0, 0, 0, 0, 0};
	valarray<float> array(data, 42);
	vector<size_t> kernels;
	cbImage<float> im(array, 2, 6, 7);
	kernels.push_back(13);
	kernels.push_back(32);
	kernels.push_back(4);

	dendro d(im, kernels);
	d.construct();

	return 1;
}
