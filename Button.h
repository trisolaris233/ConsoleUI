#pragma once

#include <string>

#include <Paint.h>
#include <Stream.h>
#include <Frame.h>

#include <vector>

namespace sweet {

	namespace cmd {

		/*
		按钮
		宽度 长度皆可自定义, 文本会尽量多的显示在Button的区域内.
		*/
		class Button : public Frame {
		private:
			Pen				_foreC;	// 字体前景色
			Pen				_bkC;	// 字体背景色
			std::wstring	_text;	// 字符串
			bool			_isPressed;	// 是否处于按下状态

		public:
			Button(
				Object*	parent = nullptr,									// 父部件
				Rect	pos = Rect(0, 0, 5, 5),								// 边界矩形
				std::wstring t = std::wstring(L"Button"),					// 字符串
				Pen		fPenf = Pen(makeLpStream(cast2UChr(WHITE))),		// 字符前景
				Pen     fPenb = Pen(makeLpStream(cast2UChr(BLACK))),		// 字符背景
				short	borderWidth = 1,									// 边框宽度
				Bucket	border = Bucket(makeLpStream(L'*')),				// 边框字符流
				Bucket	blank = Bucket(makeLpStream(L' ')),					// 填充字符流
				Pen		penBr = Pen(makeLpStream(cast2UChr(WHITE))),		// 画笔(颜色流)
				Pen		brushBk = Pen(makeLpStream(cast2UChr(WHITE)))		// 画刷(颜色流)
			)
				: Frame(
					parent
					, pos
					, borderWidth
					, border
					, blank
					, penBr
					, brushBk
				)
				, _text(t)
				, _foreC(fPenf)
				, _bkC(fPenb)
				, _isPressed(false) {
			}

			inline const std::wstring text() { return _text; }
			void  setText(const std::wstring& str) {
				sig_SetText(this, str);
				_text.assign(str);
			}

			Pen	 textForeColor() { return _foreC; }
			Pen  textBackColor() { return _bkC; }
			bool isPressed() { return _isPressed; }
			void setPressed(bool state) {
				sig_SetPressed(this, state);
				_isPressed = state; 
			}
			
			void setTextForeColor(Pen &nfc) {
				sig_SetTextForeColor(this, nfc);
				_foreC = nfc;
			}
			void setTextBackColor(Pen &nbc) {
				sig_SetTextBackColor(this, nbc);
				_bkC = nbc;
			}

			virtual short identifier() { return 2; }
			
		signals:
			sweet::Signal<void(Button*, Pen&)>	sig_SetTextForeColor;
			sweet::Signal<void(Button*, Pen&)>	sig_SetTextBackColor;
			sweet::Signal<void(Button*, const std::wstring&)>
												sig_SetText;
			sweet::Signal<void(Button*, bool)>	sig_SetPressed;

		protected:
			void paint(Object *obj);

		};

		void Button::paint(Object *obj) {
			Frame::paint(this);	// 先调用frame的paint画出边框等
			Rect   rect{ boundingRect() };
			Point  startPos{ Map2Sense(this) };
			short  borderDifference{ toShort(tool::IsFullWidthChar(border().chrs()->ex())) + 1 },
					blankDifference{ toShort(tool::IsFullWidthChar(blank().chrs()->ex())) + 1 };
			
			Rect   printableRect{
				startPos.x() + borderWidth() * borderDifference,
				startPos.y() + borderWidth(),
				rect.length() - borderWidth() * borderDifference * 2 + startPos.x() + borderWidth() * borderDifference,
				rect.width() - 2 * borderWidth() + startPos.y() + borderWidth()
			};
			std::vector<std::pair<std::wstring, Point>> vtr(std::move(tool::printInLines(_text, printableRect)));
			for (size_t i = 0; i < vtr.size(); ++i)
				tool::ConsoleShow(vtr[i].first, vtr[i].second, _foreC.get() | _bkC.get());
		}

	}

}