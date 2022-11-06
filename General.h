#pragma once

#include <utility>
#include <vector>
#include <functional>

namespace sweet {

	namespace cmd {

		class Object;

		template < class T >
		short toShort(const T &t) {
			return static_cast<short>(t);
		}

		// 坐标点类
		/*
		横坐标的单位是半角, ASCII字符占一个半角. 其他的均占俩个半角(也就是全角)
		纵坐标的单位是行。  不受半角与全角影响
		*/
		struct Point {
		private:
			short _x;	// 横坐标
			short _y;	// 纵坐标

		public:
			Point() : _x(0), _y(0) {}
			Point(short ix, short iy) : _x(ix), _y(iy) {}
			~Point() = default;

			short x() const { return _x; }
			short y() const { return _y; }
			void setX(short nx) { _x = nx; }
			void setY(short ny) { _y = ny; }
		};

		// Rect类
		/*
		表示一个矩形， 由俩个二维坐标点构成。分别表示左上角(point1)
		与右下角(point2)
		*/
		struct Rect {
		private:
			Point _p1;	// 左上角坐标
			Point _p2;	// 右下角坐标

			short _abs(short v) const {
				return (v >= 0) ? v : -v;
			}

		public:
			Rect() = default;
			Rect(short x1, short y1, short x2, short y2) :
				_p1(x1, y1), _p2(x2, y2) {}
			Rect(const Point &p1, const Point &p2) :
				_p1(p1), _p2(p2) {}
			~Rect() = default;

			const Point& point1() const { return _p1; }
			const Point& point2() const { return _p2; }
			short length() const {
				return _abs(_p1.x() - _p2.x());
			}
			short width() const {
				return _abs(_p1.y() - _p2.y());
			}

		};

		//// 流的基类
		//template < class ReturnType >
		//struct StreamBase {
		//	virtual ~StreamBase() = default;
		//	virtual ReturnType get() = 0;
		//	virtual ReturnType ex() = 0;
		//};

		///*
		//流: 用作参数传递
		//增加扩展性
		//*/
		//template < class ReturnType, class... Args >
		//class Stream : public StreamBase<ReturnType> {
		//private:
		//	typedef std::function<ReturnType(std::tuple<Args...>&)> funcType;

		//	funcType			_objFunc;
		//	std::tuple<Args...>	_args;

		//public:
		//	// 传入回调函数
		//	// 传入参数列表
		//	Stream(funcType pfun, std::tuple<Args...>&& initlist)
		//		: _objFunc(pfun)
		//		, _args(initlist) {
		//	}
		//	virtual ~Stream() = default;

		//	// 将参数列表传入回调函数
		//	virtual ReturnType get() {
		//		return _objFunc(_args);
		//	}

		//	virtual ReturnType ex() {
		//		return get();
		//	}

		//	std::tuple<Args...>& argList() {
		//		return _args;
		//	}
		//};
		//
		///*
		//循环流， 只可以从流内取得一个(一个什么跟你自己定)
		//每次get()返回的值是一样的
		//*/
		//template < class T >
		//class LoopStream : public StreamBase<T> {
		//private:
		//	struct {
		//		T operator()(std::tuple<T> &arg) {
		//			return std::get<0>(arg);
		//		}
		//	} _callback;
		//	
		//	Stream<T, T> _stream;

		//public:
		//	LoopStream(T loop)
		//		: _stream(_callback, std::make_tuple(loop)) {
		//	}
		//	virtual ~LoopStream() = default;

		//	virtual T get() {
		//		return _stream.get();
		//	}

		//	virtual T ex() {
		//		return get();
		//	}
		//};

		///*
		//随机流
		//每次get会返回[low, high)的某个字符, 随机函数是rand()
		//*/
		//template < class T >
		//class RandomStream : public StreamBase<T> {
		//private:
		//	struct {
		//		T operator()(std::tuple<T, T> &arg) {
		//			T low = std::get<0>(arg);
		//			T high = std::get<1>(arg);

		//			return (low + rand() % (high - low));
		//		}
		//	} _callback;
		//	Stream<T, T, T> _stream;

		//public:
		//	RandomStream(T low, T high)
		//		: _stream(_callback, std::make_tuple(low, high)) {
		//	}
		//	virtual ~RandomStream() = default;

		//	virtual T get() {
		//		return _stream.get();
		//	}

		//	virtual T ex() {
		//		return get();
		//	}
		//};

		///*
		//范围内循环流
		//在传入的vector<T>内循环, 每次get()得到一个,
		//超出范围之后自动回到开头
		//*/
		//template < class T >
		//class RangeStream : public StreamBase<T> {
		//private:
		//	struct {
		//		T operator()(std::tuple<std::vector<T>, int> &arg) {
		//			int &index = std::get<1>(arg);
		//			std::vector<T> &vtr = std::get<0>(arg);
		//			
		//			auto res = vtr[index % vtr.size()];
		//			index++;
		//			return res;
		//		}
		//	} _callback;
		//	Stream<T, std::vector<T>, int> _stream;

		//public:
		//	RangeStream(std::initializer_list<T>&& elements, int startIndex = 0)
		//		: _stream(_callback, std::make_tuple(std::vector<T>(elements), startIndex)) {
		//	}
		//	virtual ~RangeStream() = default;

		//	virtual T get() {
		//		return _stream.get();
		//	}
		//	
		//	virtual T ex() {
		//		return std::get<0>(_stream.argList())[0];
		//	}
		//};
		
	}
}