#pragma once

#include <memory>

#include <Paint.h>
#include <Frame.h>
#include <Signal.h>

namespace sweet {
	namespace cmd {

		class ScrollBar {
		private:
			Object* _watch;
			short	_length;
			short	_step;
			short	_pos;

		signals:
			sweet::Signal<void(ScrollBar*)> sig_PageUp;
			sweet::Signal<void(ScrollBar*)> sig_PageDown;

		public:
			ScrollBar(Object* w)
				: _watch(w)
				, _length(0)
				, _step(0)
				, _pos(0) {}

			Object* watch() { return _watch; }
			short   length() const { return _length; }
			short   step() const { return _step; }
			short   position() const { return _pos; }

			void setWatch(Object* nwatch) { _watch = nwatch; }
			void setLength(short nlen) { _length = nlen; }
			void setStep(short nstep) { _step = nstep; }
			void setPosition(short npos) { _pos = npos; }

			void pageUp() { if (_pos > 0) --_pos; sig_PageUp(this); }
			void pageDown() { if (_pos < _length) ++_pos; sig_PageDown(this); }
		};
		
		class ScrollFrame : public Frame {
		private:
			std::unique_ptr<ScrollBar> _yScroller;
			std::unique_ptr<ScrollBar> _xScroller;

		public:
			ScrollFrame(
				Object*	parent = nullptr,								// ¸¸²¿¼þ
				Rect	rect = Rect(0, 0, 5, 5),						// ±ß½ç¾ØÐÎ
				short	borderWidth = 1,								// ±ß¿ò¿í¶È
				Bucket	border = Bucket(makeLpStream(L'*')),			// ±ß¿ò×Ö·ûÁ÷
				Bucket	blank = Bucket(makeLpStream(L' ')),				// Ìî³ä×Ö·ûÁ÷
				Pen		pen = Pen(makeLpStream(unsigned char(WHITE))),	// »­±Ê(ÑÕÉ«Á÷)
				Pen		brush = Pen(makeLpStream(unsigned char(WHITE)))	// »­Ë¢(ÑÕÉ«Á÷)
			)
				: Frame(parent, rect, borderWidth, border, blank, pen, brush)
				, _yScroller(nullptr)
				, _xScroller(nullptr) {}

			void setXScroller() { _xScroller.reset(new ScrollBar(this)); }
			void setYScroller() { _yScroller.reset(new ScrollBar(this)); }
			ScrollBar* getXScroller() { return _xScroller.get(); }
			ScrollBar* getYScroller() { return _yScroller.get(); }
			void removeXScroller() { _xScroller.reset(nullptr); }
			void removeYScroller() { _xScroller.reset(nullptr); }

			virtual short   identifier() = 0;
			virtual void	paint(Object*) = 0;
		}; 
	}
}