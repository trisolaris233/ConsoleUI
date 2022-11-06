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

		// �������
		/*
		������ĵ�λ�ǰ��, ASCII�ַ�ռһ�����. �����ľ�ռ�������(Ҳ����ȫ��)
		������ĵ�λ���С�  ���ܰ����ȫ��Ӱ��
		*/
		struct Point {
		private:
			short _x;	// ������
			short _y;	// ������

		public:
			Point() : _x(0), _y(0) {}
			Point(short ix, short iy) : _x(ix), _y(iy) {}
			~Point() = default;

			short x() const { return _x; }
			short y() const { return _y; }
			void setX(short nx) { _x = nx; }
			void setY(short ny) { _y = ny; }
		};

		// Rect��
		/*
		��ʾһ�����Σ� ��������ά����㹹�ɡ��ֱ��ʾ���Ͻ�(point1)
		�����½�(point2)
		*/
		struct Rect {
		private:
			Point _p1;	// ���Ͻ�����
			Point _p2;	// ���½�����

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

		//// ���Ļ���
		//template < class ReturnType >
		//struct StreamBase {
		//	virtual ~StreamBase() = default;
		//	virtual ReturnType get() = 0;
		//	virtual ReturnType ex() = 0;
		//};

		///*
		//��: ������������
		//������չ��
		//*/
		//template < class ReturnType, class... Args >
		//class Stream : public StreamBase<ReturnType> {
		//private:
		//	typedef std::function<ReturnType(std::tuple<Args...>&)> funcType;

		//	funcType			_objFunc;
		//	std::tuple<Args...>	_args;

		//public:
		//	// ����ص�����
		//	// ��������б�
		//	Stream(funcType pfun, std::tuple<Args...>&& initlist)
		//		: _objFunc(pfun)
		//		, _args(initlist) {
		//	}
		//	virtual ~Stream() = default;

		//	// �������б���ص�����
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
		//ѭ������ ֻ���Դ�����ȡ��һ��(һ��ʲô�����Լ���)
		//ÿ��get()���ص�ֵ��һ����
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
		//�����
		//ÿ��get�᷵��[low, high)��ĳ���ַ�, ���������rand()
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
		//��Χ��ѭ����
		//�ڴ����vector<T>��ѭ��, ÿ��get()�õ�һ��,
		//������Χ֮���Զ��ص���ͷ
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