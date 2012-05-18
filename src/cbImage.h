/*
 * image.h
 *
 *  Created on: Jan 5, 2011
 *      Author: beaumont
 */

#ifndef CB_IMAGE_H_
#define CB_IMAGE_H_

#include<valarray>
#include<stdexcept>
#include <vector>
#include <cassert>
#include <algorithm>
#include <cmath>

template<class T>
class cbImage{
private:
	std::valarray<T>& data;
	size_t ndim, nx, ny, nz;

	void checkXYZ(size_t x, size_t y, size_t z) const {
		if (ndim == 2) {
			if(x < 0 || x >= nx || y < 0 || y >= ny || z != 0)
				throw(std::out_of_range("Bad xyz value"));
		} else {
			if(x < 0 || x >= nx || y < 0 || y >= ny || z < 0 || z >= nz)
				throw(std::out_of_range("Bad xyz value"));
		}
	}
	void checkIndex(size_t index) const {
		if(index < 0 || index >= getNElements())
			throw(std::out_of_range("Bad index"));
	}

	void checkSize() {
		if (ndim != 2 && ndim != 3)
			throw(std::invalid_argument("Ndim must be 2 or 3"));
		if ((ndim == 2 && (nx * ny != data.size())) ||
				(ndim == 3 && (nx * ny * nz != data.size())))
			throw(std::invalid_argument("Dimension sizes must match number of elements in array"));
		if (ndim == 2 && nz != 0)
			throw(std::invalid_argument("Z dimension must be zero when ndim = 2"));
	}

public:


	cbImage(std::valarray<T>& data, size_t ndim,
			size_t nx, size_t ny, size_t nz=0):
				data(data), ndim(ndim), nx(nx), ny(ny), nz(nz) {
		assert(&(this->data) == &data);
		checkSize();
	}

	cbImage(const cbImage<T>& im):
		data(im.data), ndim(im.ndim), nx(im.nx), ny(im.ny), nz(im.nz)
	{
		assert(&(this->data) == &data);
		checkSize();
	}

	std::valarray<T>& getData() const {
		return data;
	}


	T& getElement(size_t x, size_t y, size_t z=0) const {
		return data[index(x,y,z)];
	}


	T& operator[](const size_t& ind) const {
		checkIndex(ind);
		return data[ind];
	}

	void setElement(T val, size_t x, size_t y, size_t z = 0) throw(std::out_of_range) {
		size_t ind = index(x,y,z);
		data[ind] = val;
	}


	void neighbors(size_t ind, std::vector<size_t>& result, bool all=true) const {
		result.clear();
		int x = getX(ind), y = getY(ind), z = getZ(ind);
		/*int xstart = std::max(x-1, 0), xend = std::min(x+1, (int)nx-1);
		int ystart = std::max(y-1, 0), yend = std::min(y+1, (int)ny-1);
		int zstart = (ndim == 2) ? z : std::max(z-1, 0);
		int zend = (ndim == 2) ? z : std::min(z+1, (int)nz-1);
		for(int x2 = xstart; x2 <= xend; x2++) {
			for(int y2 = ystart; y2 <= yend; y2++) {
				for(int z2 = zstart; z2 <= zend; z2++) {
					if(x == x2 && y == y2 && z == z2) continue;
					if(!all && abs(x - x2) + abs(y - y2) + abs(z - z2) > 1) continue;
					result.push_back(this->index(x2,y2,z2));
				}
			}
		}
		*/
		for(int dz=-1; dz<2; dz++) {
			if (ndim == 2 && dz != 0) continue;
			if (ndim == 3 && (z+dz < 0 || z+dz >= (int(nz)))) continue;
			for(int dx=-1; dx<2; dx++) {
				if(x+dx < 0 || x+dx >= (int)nx) continue;
				for(int dy=-1; dy<2; dy++) {
					if(y+dy < 0 || y+dy >= (int)ny) continue;

					if(dx == 0 && dy == 0 && dz == 0) continue;
					if(!all && abs(dx) + abs(dy) + abs(dz) > 1) continue;
					result.push_back(this->index(x+dx, y+dy, z+dz));
				}
			}
		}
	}


	void localMaxima(std::vector<size_t>& result, size_t friends, size_t specfriends, T minvalue) const{
		if(friends == 0 || specfriends == 0)
			throw(std::invalid_argument("friend and specfriend variables must be positive"));
		result.clear();
		size_t n = getNElements();
		int f = (int) friends;
		int sf = (int) specfriends;
		for(size_t i = 0; i < n; i++) {
			if (data[i] < minvalue) continue;
			if (std::isnan(data[i])) continue;
			bool isMax = true;
			size_t ind2;
			int x=getX(i), y=getY(i), z=getZ(i);
			int xstart = std::max(x - f, 0), xstop = std::min(x + f, (int)nx-1);
			int ystart = std::max(y - f, 0), ystop = std::min(y + f, (int)ny-1);
			int zstart = std::max(z - sf, 0), zstop = std::min(z + sf, (int)nz-1);
			if (ndim == 2) {
				zstart = z; zstop = z;
			}
			for(int x2 = xstart; x2 <= xstop; x2++) {
				for(int y2 = ystart; y2 <= ystop; y2++) {
					for(int z2 = zstart; z2 <= zstop; z2++) {
						if (x2 == x && y2 == y && z2 == z) continue;
						ind2 = index(x2, y2, z2);
						assert(ind2 != i);
						if (data[i] <= data[ind2]) isMax = false;
						if (!isMax) break;
					}
					if (!isMax) break;
				}
				if (!isMax) break;
			}
			if (isMax) result.push_back(i);
		}
	}


	void localMaxima(std::vector<size_t>& result, size_t friends, T minvalue) const{
		if(ndim == 3)
			throw(std::invalid_argument("Must specify specfriends for 3D images"));
		localMaxima(result, friends, 1, minvalue);
	}


	size_t getNDim() const {return ndim;}


	size_t getXDim() const {return nx;}


	size_t getYDim() const {return ny;}


	size_t getZDim() const {return nz;}


	size_t getNElements() const {return (ndim == 2) ? nx * ny : nx * ny * nz;}


	size_t index(size_t x, size_t y, size_t z = 0) const {
		checkXYZ(x,y,z);
		return x + nx * (y + ny * z);
	}


	size_t getX(size_t index) const {
		checkIndex(index);
		return index % nx;
	}


	size_t getY(size_t index) const {
		checkIndex(index);
		return (index / nx) % ny;
	}


	size_t getZ(size_t index) const {
		checkIndex(index);
		return index / nx / ny;
	}

};


#endif /* IMAGE_H_ */
