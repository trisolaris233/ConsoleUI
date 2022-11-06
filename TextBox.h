#pragma once

#include <vector>
#include <string>
#include <codecvt>
#include <Paint.h>
#include <Object.h>
#include <Controller.h>
#include <ScrollFrame.h>

namespace sweet {
	namespace cmd {

		class TextBox : public Frame {
		private:
			std::vector<std::wstring>
							_text;
			InputController _ctrl;
			Pen				_textColor;
			std::unique_ptr<ScrollBar> 
							_yScroller;
			std::unique_ptr<ScrollBar> 
							_xScroller;

		signals:
			sweet::Signal<void(TextBox*)> sig_TextChanged;
			sweet::Signal<void(TextBox*)> sig_TypeFinished;

		slots:
			sweet::Slot slot_CharRecv;
			sweet::Slot slot_FinishRecv;

		public:
			TextBox(
				Object*	parent = nullptr,								// 父部件
				Rect	rect = Rect(0, 0, 5, 5),						// 边界矩形
				short	borderWidth = 1,								// 边框宽度
				Bucket	border = Bucket(makeLpStream(L'*')),			// 边框字符流
				Bucket	blank = Bucket(makeLpStream(L' ')),				// 填充字符流
				Pen		pen = Pen(makeLpStream(unsigned char(WHITE))),	// 画笔(颜色流)
				Pen		brush = Pen(makeLpStream(unsigned char(WHITE)))	// 画刷(颜色流)
			)
				: Frame(parent, rect, borderWidth, border, blank, pen, brush)
				, _yScroller(nullptr)
				, _xScroller(nullptr)
				, _textColor(makeLpStream(cast2UChr(WHITE)))
			{
				slot_CharRecv = _ctrl.sig_CharRecv.connect(this, &TextBox::treatNextChar);
				slot_FinishRecv = _ctrl.sig_RecvFinish.connect(this, &TextBox::receiveFinish);
			}
			
			/*void setText(const std::wstring& p);*/
			void appendChar(wchar_t ch) {
				showNextChar(ch);
			}
			void startInput() { 
				tool::SetCursorVisible(true);
				_ctrl.go();		// 开始监听按钮
			}
			void finishInput() { 
				tool::SetCursorVisible(false);
				_ctrl.pause();	// 停止监听按钮
				sig_TypeFinished(this);
			}
			bool inputState() { return _ctrl.active(); }
			short getDefaultPrinableLength() { 
				return this->boundingRect().length() - 2 * getCharWidth(border().chrs()->ex());
			}
			short getDefaultPrinableHeight() { 
				return this->boundingRect().width() - 2 * borderWidth(); 
			}
			size_t countLine() const { return _text.size(); }
			size_t charCountLine(size_t numOfLine) const { return _text[numOfLine].size(); }

			virtual short identifier() { return 4; }
			virtual void paint(Object* obj) {
				Frame::paint(obj);
				for (size_t i = 0; i < _text.size(); ++i)
					showText(i, _text[i]);
			}

		protected:
			void treatNextChar(InputController*, wchar_t ch) {
				appendChar(ch);	// 追加字符
			}
			void receiveFinish(InputController*) {
				finishInput();
			}

			short getCharWidth(wchar_t chr) {
				return toShort(tool::IsFullWidthChar(chr)) + 1;
			}

			void showChar(Point p, wchar_t ch) {
				tool::ConsoleShow(std::wstring(1, ch), p, WHITE);
				sig_TextChanged(this);
				// 设置光标位置
				tool::GotoXY(p.x(), p.y());
			}

			// 计算第几行第几个字符的全局坐标
			std::pair<bool, Point> getCharPrintCrood(size_t line, size_t num, wchar_t chr) {
				Point p{ Map2Sense(this) };
				// 定位除去边框之后的坐标x
				p.setX(p.x() + borderWidth() * getCharWidth(border().chrs()->ex()));
				p.setY(p.y() + borderWidth() + toShort(line));
				Point bak{ p };	// 备份
				// 计算除去边框之后的坐标y
				short lineLength{ 0 };
				
				for (size_t i = 0; i < num; ++i) {
					lineLength += getCharWidth(_text[line][num]);
					p.setX(p.x() + getCharWidth(_text[line][i]));
				}
				//std::cout << "lineLength = " << lineLength << std::endl;

				short difX{ p.x() - bak.x() }, difY{ p.y() - bak.y() };
				// 如果横坐标超出可打印范围且没有安装x轴平行滚动装置则换行
				if (nullptr == _xScroller.get() && (difX >= getDefaultPrinableLength())) 
					p.setY(p.y() + difX / getDefaultPrinableLength());
				// 如果纵坐标超出可打印范围且没有安装y轴平行滚动装置
				if (nullptr == _yScroller.get() && (difY >= getDefaultPrinableHeight()))
					return std::make_pair(false, p);
				else
					p.setX(bak.x() + difX % getDefaultPrinableLength());
				return std::make_pair(true, p);
			}

			void showText(size_t line, const std::wstring& str) {
				for (size_t i = 0; i < str.size(); ++i) {
					std::pair<bool, Point> res = getCharPrintCrood(line, i, str[i]);
					if (res.first) {
						showChar(res.second, str[i]);
					}
				}
			}

			void showNextChar(wchar_t ch) {
				size_t	line{ _text.size() },
						num{ (line == 0) ? 0 : _text[line - 1].size() };
				std::pair<bool, Point> pairOfRes{ getCharPrintCrood(((line==0) ? 0 : line-1), num, ch) };
				if (pairOfRes.first) {
					if (_text.empty()) {
						_text.push_back(std::wstring(1, ch));
					}
					else {
						_text[line - 1] += (ch);
					}
					tool::ConsoleShow(std::wstring(1, ch), pairOfRes.second, WHITE);
					sig_TextChanged(this);
					// 设置光标位置
					tool::GotoXY(pairOfRes.second.x(), pairOfRes.second.y());
				}
				
			}
			

		};

	}
}