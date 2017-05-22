#ifndef BIVECTOR_H
#define BIVECTOR_H

//#include "common/common.h"
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

/** Template class for a two-dimensional generic vector. */
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
			throw std::out_of_range("bivector::at " + str(p.transpose()));
			}
#endif
		return data[p.x() + p.y() * datasize.x()];
		}

	T& at(int x, int y) {
#ifdef DEBUG
		if(x >= datasize.x() || y >= datasize.y()) {
			throw std::out_of_range("bivector::at " + str(MyVector2i(x, y).transpose()));
			}
#endif
		return data[x + y * datasize.x()];
		}

	T& operator[](const MyVector2i& p) {
		return data[p.x() + p.y() * datasize.x()];
		}

	const MyVector2i& size() const {
		return datasize;
		}

	// get pointer to storage, be very careful with that!

	T* data_ptr() {
		return &data[0];
		}

	const int& width() {
		return datasize.x();
		}

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

template <class T>
T bivector<T>::get_min() const {
	throw std::invalid_argument("bivector::get_min data empty");
	T m = data[0];
	bivector_FOREACH(m = std::min(m, data[z]))
		return m;
	}

template <class T>
T bivector<T>::get_max() const {
	if(datasize.x() <= 0 || datasize.y() <= 0) throw std::invalid_argument("bivector::get_max data empty");
	T m = data[0];
	bivector_FOREACH(m = std::max(m, data[z]))
		return m;
	}

template <class T>
T bivector<T>::get_min_abs() const {
	if(datasize.x() <= 0 || datasize.y() <= 0) throw std::invalid_argument("bivector::get_min_abs data empty");
	T m = bivector_abs(data[0]);
	bivector_FOREACH(m = std::min(m, abs(data[z])))
		return m;
	}

template <class T>
T bivector<T>::get_max_abs() const {
	if(datasize.x() <= 0 || datasize.y() <= 0) throw std::invalid_argument("bivector::get_max_abs data empty");
	T m = bivector_abs(data[0]);
	bivector_FOREACH(m = std::max(m, (T)abs(data[z])))
		return m;
	}

template <class T>
bivector<T>& bivector<T>::operator*=(const T& s) {
	bivector_FOREACH(data[z] *= s)
		return *this;
	}

template <class T>
bivector<T>& bivector<T>::operator+=(const T& v) {
	bivector_FOREACH(data[z] += v)
		return *this;
	}

template <class T>
bivector<T>& bivector<T>::operator+=(const bivector<T>& v) {
	bivector_FOREACH(data[z] += v.data[z])
		return *this;
	}

template <class T>
bivector<T>& bivector<T>::assign(const T& s) {
	bivector_FOREACH(data[z] = s)
		return *this;
	}

template <class T>
bivector<T> bivector<T>::sub_area(const MyVector2i& offset, const MyVector2i& sz) const {
	if(offset.y() + sz.y() > datasize.y()) throw std::invalid_argument("bivector::sub_area, offset.y invalid");
	if(offset.x() + sz.x() > datasize.x()) throw std::invalid_argument("bivector::sub_area, offset.x invalid");
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
