#pragma once

#include <memory>
#include <vector>
#include <functional>
#include <Stream.h>
#include <Windows.h>

namespace sweet {

	namespace cmd {
		typedef unsigned char Color;
		enum : unsigned char {
					BLACK = 0,
					DARK_BLUE,
					DARK_GREEN,
					DARK_CYAN,
					DARK_RED,
					DARK_PURPLE,
					DARK_YELLOW,
					DARK_WHITE,
					GRAY,
					BLUE,
					GREEN,
					CYAN,
					RED,
					PURPLE,
					YELLOW,
					WHITE
				};

		class PaintDevice {
		public:
			virtual ~PaintDevice() = default;
		};

		class Pen : PaintDevice {
		private:
			std::shared_ptr<StreamBase<Color>> _color;

		public:
			Pen(StreamBase<Color>* c)
				: _color(c) {}
			virtual ~Pen() = default;

			StreamBase<Color>* color() const { return _color.get(); }
			void  setColor(StreamBase<Color>* nc) { _color.reset(nc); }
			Color get() { return _color->get(); }
		};

		class Bucket : PaintDevice {
		private:
			std::shared_ptr<StreamBase<wchar_t>> _chrs;

		public:
			Bucket(StreamBase<wchar_t>* c)
				: _chrs(c) {}
			virtual ~Bucket() = default;

			StreamBase<wchar_t>* chrs() { return _chrs.get(); }
			wchar_t get() { return _chrs->get(); }
		};

		class TinyBucket : PaintDevice {
		private:
			std::shared_ptr<StreamBase<char>> _chrs;

		public:
			TinyBucket(StreamBase<char>* c)
				: _chrs(c) {}
			virtual ~TinyBucket() = default;

			StreamBase<char>* chrs() { return _chrs.get(); }
			char get() { return _chrs->get(); }
		};

		typedef LoopStream<wchar_t>		WChrLoopStream;
		typedef RandomStream<wchar_t>	WChrRdStream;
		typedef RangeStream<wchar_t>	WChrReStream;
		typedef Stream<wchar_t>			WChrStream;
		typedef LoopStream<char>		ChrLoopStream;
		typedef RandomStream<char>		ChrRdStream;
		typedef RangeStream<char>		ChrReStream;
		typedef Stream<char>			ChrStream;
		typedef LoopStream<Color>		ColorLoopStream;
		typedef RandomStream<Color>		ColorRdStream;
		typedef RangeStream<Color>		ColorReStream;
		typedef Stream<Color>			ColorStream;

		template < class T >
		LoopStream<T>* makeLpStream(T arg) {
			return new LoopStream<T>(arg);
		}

		template < class T >
		RandomStream<T>* makeRdStream(const T& lo, const T& hi) {
			return new RandomStream<T>(lo, hi);
		}

		template < class T >
		RangeStream<T> makeReStream(std::initializer_list<T>&& l) {
			return new RangeStream<T>(l);
		}

		inline Color cast2UChr(Color c) { return static_cast<Color>(c); }

	}

}