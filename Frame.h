#pragma once

#include <vector>
#include <memory>
#include <cstdlib>
#include <cstdio>
#include <Windows.h>

#include <Paint.h>
#include <Signal.h>
#include <Object.h>
#include <Console.h>

namespace sweet {
	
	namespace cmd {

		class Frame : public Object {
		private:
			short							_borderWidth;	// ±ß¿ò¿í¶È
			Bucket							_border,		// ±ß¿ò×Ö·ûÁ÷
											_blank;			// Ìî³ä×Ö·ûÁ÷
			TinyBucket						_fixer;			// Ìî²¹×Ö·ûÁ÷
			Pen								_pen,			// ±ß¿ò»­±ÊÁ÷
											_brush;			// Ìî³ä»­±ÊÁ÷

		public:
			Frame(
				Object*	parent = nullptr,								// ¸¸²¿¼þ
				Rect	rect = Rect(0, 0, 5, 5),						// ±ß½ç¾ØÐÎ
				short	borderWidth = 1,								// ±ß¿ò¿í¶È
				Bucket	border = Bucket(makeLpStream(L'*')),			// ±ß¿ò×Ö·ûÁ÷
				Bucket	blank = Bucket(makeLpStream(L' ')),				// Ìî³ä×Ö·ûÁ÷
				Pen		pen = Pen(makeLpStream(unsigned char(WHITE))),	// »­±Ê(ÑÕÉ«Á÷)
				Pen		brush = Pen(makeLpStream(unsigned char(WHITE)))	// »­Ë¢(ÑÕÉ«Á÷)
			)
				: Object(rect, parent)
				, _borderWidth(borderWidth)
				, _border(border)
				, _blank(blank)
				, _fixer(makeLpStream(' '))
				, _pen(pen)
				, _brush(brush)
			{}
			virtual ~Frame() = default;

			Pen pen() { return _pen; }
			Pen brush() { return _brush; }
			Bucket  border() { return _border; }
			Bucket  blank() { return _blank; }
			TinyBucket fixer() { return _fixer; }
			short	borderWidth() { return _borderWidth; }

			void setPen(Pen npen) {
				_pen = npen;
			}
			void setBrush(Pen nbrush) {
				_brush = nbrush;
			}
			void setBorder(Bucket nborder) {
				_border = nborder;
			}
			void setBlank(Bucket nblank) {
				_blank = nblank;
			}
			void setFixer(TinyBucket nfixer) {
				_fixer = nfixer;
			}
			void setBorderWidth(short nw) {
				_borderWidth = nw;
			}
			
			virtual short identifier() { return 1; }

		protected:
			virtual void paint(Object *obj);

		};

		void Frame::paint(Object *obj) {
			Point			sensePoint{ Map2Sense( this) };
			short			len{ boundingRect().length() };
			short			wid{ boundingRect().width() };
			tool::DrawRectAbs(
				{
					sensePoint, { sensePoint.x() + len, sensePoint.y() + wid },
				}
				, *_border.chrs()
				, *_blank.chrs()
				, *_pen.color()
				, *_brush.color()
				, _borderWidth
				, *_fixer.chrs()
			);
		}

	}
}