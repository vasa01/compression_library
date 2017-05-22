/**
 * Image compression library supporting wavelet and contourlet
 * transformation with the possibility of encoding algorithms EZW, SPIHT and EBCOT.
 * (C) Vaclav Bradac
 *
 * This program is free software: you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/.
 */
/**
 * @file	bivector.hpp
 *
 * @brief	bivector.
 */


#ifndef BIVECTOR_H
#define BIVECTOR_H

#include<vector>
#include <iostream>
/**
 * class MyVector2i
 */
class MyVector2i {
private:
	int x1, y1;
public:
	MyVector2i(int _x, int _y):x1(_x), y1(_y) {};
	int x() const {
		return x1;
		}
	int y() const {
		return y1;
		}
};

#define bivector_FOREACH(cmd) for (int z=0; z < int(datasize.x() * datasize.y()); ++z) { cmd ; }
#define bivector_FOREACH_XY(cmd) for (int y=0; y < datasize.y(); ++y) for (int x=0; x < datasize.x(); ++x) { cmd ; }
#define bivector_FOREACH_XYZ(cmd) for (int y=0,z=0; y < datasize.y(); ++y) for (int x=0; x < datasize.x(); ++x,++z) { cmd ; }
#define bivector_abs(x) ((x>0)?x:-x)

/**
 *  class bivector
 * @tparam T
 */
template <class T>
class bivector {
protected:
	MyVector2i datasize;
	std::vector<T> vec_data;
	T * const data;

public:

	bivector() { }

	bivector(const MyVector2i& sz, const T& v = T()): datasize(sz), vec_data(sz.x() * sz.y(), v), data(vec_data.data()) { }
	bivector(const int& w, const int& h, const T& v = T()): datasize(w, h), vec_data(datasize.x() * datasize.y(), v), data(vec_data.data()) { }

	bivector(const MyVector2i& sz, T* p): datasize(sz), data(p) { }
	bivector(const int& w, const int& h, T* p): datasize(w, h), data(p) { }

	T& at(const MyVector2i& p) const {
#ifdef DEBUG
		if(p.x() >= datasize.x() || p.y() >= datasize.y()) {
			//throw std::out_of_range("bivector::at " + str(p.transpose()));
			std::cerr << "bivector::at " + str(p.transpose())<<std::endl;
			exit(-1);
			}
#endif
		return data[p.x() + p.y() * datasize.x()];
		}

	T& at(int x, int y) {
#ifdef DEBUG
		if(x >= datasize.x() || y >= datasize.y()) {
			std::cerr << "bivector::at " + str(MyVector2i(x, y).transpose())<<std::endl;
			exit(-1);
		}
#endif
		return data[x + y * datasize.x()];
		}
	/**
	 * operator[]
	 * @param p
	 * @return
	 */
	T& operator[](const MyVector2i& p) {
		return data[p.x() + p.y() * datasize.x()];
		}
	/**
	 * MyVector2i
	 * @return - size
	 */
	const MyVector2i& size() const {
		return datasize;
		}


	/**
	 * data_ptr
	 * @return  - pointer storage
	 */
	T* data_ptr() {
		return &data[0];
		}
	/**
	 * width
	 * @return - width pointer
	 */
	const int& width() {
		return datasize.x();
		}

	/**
	 * height
	 * @return - height pointer
	 */
	const int& height() {
		return datasize.y();
		}

	bivector<T> sub_area(const MyVector2i& offset, const MyVector2i& sz) const;

	// special operations
	T get_min() const;
	T get_max() const;
	T get_min_abs() const;
	T get_max_abs() const;
	bivector<T>& operator*=(const T& s);
	bivector<T>& operator+=(const T& v);
	bivector<T>& operator+=(const bivector<T>& v);

	bivector<T>& assign(const T& s);

	template<class U> friend class bivector;
	};

/**
 * get_min
 * @tparam T
 * @return - data min
 */
template <class T>
T bivector<T>::get_min() const {
	//throw std::invalid_argument("bivector::get_min data empty");
	
	T m = data[0];
	bivector_FOREACH(m = std::min(m, data[z]))
		return m;
	}

/**
 * get_max
 * @tparam T
 * @return data max
 */
template <class T>
T bivector<T>::get_max() const {
	if(datasize.x() <= 0 || datasize.y() <= 0){
		std::cerr << "bivector::get_max data empty"<<std::endl;
			exit(-1);
	} //throw std::invalid_argument("bivector::get_max data empty");
	T m = data[0];
	bivector_FOREACH(m = std::max(m, data[z]))
		return m;
	}

/**
 * get_min_abs
 * @tparam T
 * @return - min data absolut
 */
template <class T>
T bivector<T>::get_min_abs() const {
	if(datasize.x() <= 0 || datasize.y() <= 0){
		std::cerr << "bivector::get_min_abs data empty"<<std::endl;
			exit(-1);
	} //throw std::invalid_argument("bivector::get_min_abs data empty");
	T m = bivector_abs(data[0]);
	bivector_FOREACH(m = std::min(m, abs(data[z])))
		return m;
	}
/**
 * get_max_abs
 * @tparam T
 * @return - data max absolut
 */
template <class T>
T bivector<T>::get_max_abs() const {
	if(datasize.x() <= 0 || datasize.y() <= 0){
		std::cerr << "bivector::get_max_abs data empty"<<std::endl;
		exit(-1);
	}// throw std::invalid_argument("bivector::get_max_abs data empty");
	T m = bivector_abs(data[0]);
	bivector_FOREACH(m = std::max(m, (T)abs(data[z])))
		return m;
	}

/**
 * operator *=
 * @tparam T
 * @param s
 * @return
 */
template <class T>
bivector<T>& bivector<T>::operator*=(const T& s) {
	bivector_FOREACH(data[z] *= s)
		return *this;
	}

/**
 * operator +=
 * @tparam T
 * @param v - T
 * @return
 */
template <class T>
bivector<T>& bivector<T>::operator+=(const T& v) {
	bivector_FOREACH(data[z] += v)
		return *this;
	}
/**
 * operator +=
 * @tparam T
 * @param v - bivector
 * @return
 */
template <class T>
bivector<T>& bivector<T>::operator+=(const bivector<T>& v) {
	bivector_FOREACH(data[z] += v.data[z])
		return *this;
	}
/**
 * assign
 * @tparam T
 * @param s
 * @return
 */
template <class T>
bivector<T>& bivector<T>::assign(const T& s) {
	bivector_FOREACH(data[z] = s)
		return *this;
	}
/**
 * sub_area
 * @tparam T
 * @param offset
 * @param sz
 * @return - sub_area
 */
template <class T>
bivector<T> bivector<T>::sub_area(const MyVector2i& offset, const MyVector2i& sz) const {
	if(offset.y() + sz.y() > datasize.y()){
		std::cerr << "bivector::sub_area, offset.y invalid"<<std::endl;
		exit(-1);
	}// throw std::invalid_argument("bivector::sub_area, offset.y invalid");
	if(offset.x() + sz.x() > datasize.x()){
		std::cerr << "bivector::sub_area, offset.x invalid"<<std::endl;
		exit(-1);
	}// throw std::invalid_argument("bivector::sub_area, offset.x invalid");
	bivector<T> result(sz);
	for(int y = 0; y < result.datasize.y(); ++y)
		for(int x = 0; x < result.datasize.x(); ++x)
			result.at(x, y) = at(offset.x() + x, offset.y() + y);
	return result;
	}

#undef bivector_FOREACH
#undef bivector_FOREACH_XY
#undef bivector_FOREACH_XYZ
#undef bivector_abs
#endif
