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
			short							_borderWidth;	// �߿���
			Bucket							_border,		// �߿��ַ���
											_blank;			// ����ַ���
			TinyBucket						_fixer;			// ��ַ���
			Pen								_pen,			// �߿򻭱���
											_brush;			// ��仭����

		public:
			Frame(
				Object*	parent = nullptr,								// ������
				Rect	rect = Rect(0, 0, 5, 5),						// �߽����
				short	borderWidth = 1,								// �߿���
				Bucket	border = Bucket(makeLpStream(L'*')),			// �߿��ַ���
				Bucket	blank = Bucket(makeLpStream(L' ')),				// ����ַ���
				Pen		pen = Pen(makeLpStream(unsigned char(WHITE))),	// ����(��ɫ��)
				Pen		brush = Pen(makeLpStream(unsigned char(WHITE)))	// ��ˢ(��ɫ��)
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