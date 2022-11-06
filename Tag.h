#pragma once

#include <string>
#include <Object.h>
#include <Paint.h>

namespace sweet {

	namespace cmd {

		class Tag : public Object {
		private:
			Pen				_foreC;			// 字体前景色
			Pen				_bkC;			// 字体背景色
			std::wstring	_text;			// 打印的信息
			bool			_autoStrech;	// 自动拉伸

		public:
			Tag(
				Object *p = nullptr,
				Rect    r = Rect(0, 0, 5, 1),
				const	std::wstring& str = L"label",
				Pen		foreColor = Pen(makeLpStream(cast2UChr(WHITE))),
				Pen		backColor = Pen(makeLpStream(cast2UChr(BLACK)))
			)
				: Object(r, p)
				, _text(str)
				, _foreC(foreColor)
				, _bkC(backColor)
				, _autoStrech(false) {}
			virtual ~Tag() = default;

			const std::wstring& text() const { return _text; }
			bool  isAutoStrech() const { return _autoStrech; }
			Pen   foreColor() const { return _foreC; }
			Pen   backColor() const { return _bkC; }

			void  setForeColor(Pen npen) { _foreC = npen; }
			void  setBackColor(Pen npen) { _bkC = npen; }
			void  setAutoStrech(bool state) { _autoStrech = state; }
			void  setText(const std::wstring& t) { _text = t; }

			virtual short identifier() { return 3; }

			virtual void paint(Object* obj) {
				Rect	r{ this->boundingRect() };
				Point   startPos{ Map2Sense(this) };
				short	length{ r.length() },
						height{ r.width() },		
						strLength{ strLen(_text) };
				std::vector<std::wstring> 
						lines(std::move(tool::SplitStrInLines(_text)));

				short	curLine{ 0 },
						cursor{ 0 },
						newLength{ length },
						newHeight{ _autoStrech ? toShort(lines.size()) : height };

				for (std::size_t i = 0; i < lines.size(); ++i) {	// 便利标签每一行
					if (i > static_cast<std::size_t>(height) && !_autoStrech)	
						return;
					if (_autoStrech) {	// 如果开启了自动拉伸
						tool::ConsoleShow(lines[i], { startPos.x(), startPos.y() + toShort(i) }, _foreC.get() | _bkC.get());
						short len{ toShort(tool::HalfLen(lines[i])) };
						
						if (len > newLength)
							newLength = len;
						//setRect(Rect{ startPos.x(), startPos.y(), startPos.x() + strLength, startPos.y() + 1 });
					}
					else {
						std::wstring tmp(tool::GetFullWidthLenStr(_text, length));
						tool::ConsoleShow(tmp, { startPos.x(), startPos.y() + toShort(i) }, _foreC.get() | _bkC.get());
					}
				}
			}

			virtual void refresh() {
				paint(this);
			}
		};
	}
}

