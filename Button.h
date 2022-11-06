#pragma once

#include <string>

#include <Paint.h>
#include <Stream.h>
#include <Frame.h>

#include <vector>

namespace sweet {

	namespace cmd {

		/*
		��ť
		��� ���ȽԿ��Զ���, �ı��ᾡ�������ʾ��Button��������.
		*/
		class Button : public Frame {
		private:
			Pen				_foreC;	// ����ǰ��ɫ
			Pen				_bkC;	// ���屳��ɫ
			std::wstring	_text;	// �ַ���
			bool			_isPressed;	// �Ƿ��ڰ���״̬

		public:
			Button(
				Object*	parent = nullptr,									// ������
				Rect	pos = Rect(0, 0, 5, 5),								// �߽����
				std::wstring t = std::wstring(L"Button"),					// �ַ���
				Pen		fPenf = Pen(makeLpStream(cast2UChr(WHITE))),		// �ַ�ǰ��
				Pen     fPenb = Pen(makeLpStream(cast2UChr(BLACK))),		// �ַ�����
				short	borderWidth = 1,									// �߿���
				Bucket	border = Bucket(makeLpStream(L'*')),				// �߿��ַ���
				Bucket	blank = Bucket(makeLpStream(L' ')),					// ����ַ���
				Pen		penBr = Pen(makeLpStream(cast2UChr(WHITE))),		// ����(��ɫ��)
				Pen		brushBk = Pen(makeLpStream(cast2UChr(WHITE)))		// ��ˢ(��ɫ��)
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
			Frame::paint(this);	// �ȵ���frame��paint�����߿��
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