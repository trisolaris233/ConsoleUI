#pragma once

#include <string>
#include <cstddef>
#include <sstream>

#include <Paint.h>
#include <Stream.h>
#include <General.h>
#include <Windows.h>


HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
//sweet::StreamBase<char>* blankFill = nullptr;

namespace sweet {

	namespace cmd {

		class tool {
		private:
			tool() = default;

		public:
			static std::size_t	HalfLen(const std::wstring &str);								// �������ַ���
			template < class T >
			static void			ConsoleShow(const T &msg, Point coord, char color);				// ����̨�������
			template <>
			static void			ConsoleShow(const std::wstring &wmsg, Point coord, char color);	// ���ַ����ػ�
			static void			ConsoleShow(const std::wstring &wmsg, Point coord, StreamBase<Color>& color);	// ���ַ��ػ�
			static void			GotoXY(short x, short y);										// �ƶ������(x, y)
			static void			SetCursorVisible(bool visible);									// ���ù���Ƿ�ɼ�
			static int			GetWindowWidth();												// ��õ�ǰ����̨���
			static int			GetWindowHeight();												// ��õ�ǰ����̨�߶�
			static void			ClearConsole(Point p, std::size_t len, char color);				// ��p��ʼ��len���ַ�
			static void			ClearConsole(Point p, std::size_t len, StreamBase<Color>& color);
			static bool			IsFullWidthChar(wchar_t wch);									// �ж��Ƿ�Ϊȫ���ַ�
			static bool			IsFullWidthChar(StreamBase<wchar_t>& wchrStream);	
			template < typename CharT >
			static std::wstring		
								GetStringFromStream(StreamBase<CharT>& stream, unsigned short count);// ��strem�л��x���ַ�
			static void			DrawRect(
									const Rect &rect,
									wchar_t border,
									wchar_t blank,
									Color	pen,
									Color	brush,
									short	borderWidth
			);
			static void			DrawRect(
									const Rect &rect,
									StreamBase<wchar_t>& borderStream,
									StreamBase<wchar_t>& blankStream,
									StreamBase<Color>&	penStream,
									StreamBase<Color>&	brushStream,
									short BorderWidth = 1
								);
			static void			DrawRectAbs(
									const Rect &rect,
									StreamBase<wchar_t>& borderStream,
									StreamBase<wchar_t>& blankStream,
									StreamBase<Color>&   penStream,
									StreamBase<Color>&   brushStream,
									short                borderWidth,
									StreamBase<char>&    fixer
								);
			static std::vector<std::pair<std::wstring, Point>>
			printInLines(const std::wstring& str, Rect rect);
			static std::vector<std::wstring>
			SplitStrInLines(const std::wstring& str);
			static std::wstring
			GetFullWidthLenStr(const std::wstring& str, unsigned short len);
		};

		std::size_t tool::HalfLen(const std::wstring &str) {
			std::size_t size{ 0 };
			
			for (auto x : str) {
				if (x >= 0 && x < 128)
					++size;
				else
					size += 2;
			}
			return size;
		}
		
		template < class T >
		void tool::ConsoleShow(const T &msg, Point crood, char color) {
			COORD pos;
			ULONG unuse;
			std::stringstream ss;
			pos.X = crood.x();
			pos.Y = crood.y();
			
			ss << msg;
			std::string str{ ss.str() };
			std::size_t len{ str.size() };

			WriteConsoleOutputCharacterA(handle, str.c_str(), len, pos, &unuse);
			FillConsoleOutputAttribute(handle, color, len, pos, &unuse);
		}

		template <>
		void tool::ConsoleShow(const std::wstring &msg, Point crood, char color) {
			COORD pos;
			pos.X = crood.x();
			pos.Y = crood.y();
			ULONG unuse;
			size_t len{ msg.size() };
			size_t halfLen{ HalfLen(msg) };

			WriteConsoleOutputCharacterW(handle, msg.c_str(), len, pos, &unuse);
			FillConsoleOutputAttribute(handle, color, halfLen, pos, &unuse);
		}

		void tool::ConsoleShow(const std::wstring &wmsg, Point crood, StreamBase<Color>& color) {
			COORD pos;
			pos.X = crood.x();
			pos.Y = crood.y();
			ULONG unuse;
			size_t len{ wmsg.size() };
			size_t halfLen{ HalfLen(wmsg) };

			WriteConsoleOutputCharacterW(handle, wmsg.c_str(), len, pos, &unuse);
			for (auto x : wmsg) {
				FillConsoleOutputAttribute(handle, color.get(), static_cast<int>(IsFullWidthChar(x)) + 1, pos, &unuse);
				pos.X += static_cast<int>(IsFullWidthChar(x)) + 1;
			}

		}

		void tool::GotoXY(short x, short y) {
			COORD pos;
			pos.X = x;
			pos.Y = y;
			
			SetConsoleCursorPosition(handle, pos);
		}


		void tool::SetCursorVisible(bool visible) {
			CONSOLE_CURSOR_INFO cci;
			cci.bVisible = visible;
			cci.dwSize = 25;
			
			SetConsoleCursorInfo(handle, &cci);
		}

		int tool::GetWindowWidth() {
			CONSOLE_SCREEN_BUFFER_INFO info;
			GetConsoleScreenBufferInfo(handle, &info);
			return info.srWindow.Right + 1;
		}

		int tool::GetWindowHeight() {
			CONSOLE_SCREEN_BUFFER_INFO info;
			GetConsoleScreenBufferInfo(handle, &info);
			return info.srWindow.Bottom + 1;
		}

		void tool::ClearConsole(Point p, std::size_t len, char color) {
			COORD pos;
			ULONG unuse;
			pos.X = p.x();
			pos.Y = p.y();

			std::string mask(len, ' ');
			WriteConsoleOutputCharacterA(handle, mask.c_str(), len, pos, &unuse);

			FillConsoleOutputAttribute(handle, color, len, pos, &unuse);
		}

		void tool::ClearConsole(Point p, std::size_t len, StreamBase<Color>& colorStream) {
			COORD pos;
			ULONG unuse;
			pos.X = p.x();
			pos.Y = p.y();

			std::string mask(len, ' ');
			WriteConsoleOutputCharacterA(handle, mask.c_str(), len, pos, &unuse);
			
			for (size_t i = 0; i < len; ++i) {
				FillConsoleOutputAttribute(handle, colorStream.get(), 1, pos, &unuse);
				pos.X++;
			}
		}

		bool tool::IsFullWidthChar(wchar_t wchr) {
			// �жϸ�λ�Ƿ�Ϊ0x00
			// �����˵������ȫ���ַ�
			// ��֮���ǡ�
			return(*(reinterpret_cast<char*>(&wchr) + 1) != 0x00);
		}

		bool tool::IsFullWidthChar(StreamBase<wchar_t>& wchrStream) {
			return IsFullWidthChar(wchrStream.get());
		}

		template < typename CharT >
		std::wstring tool::GetStringFromStream(StreamBase<CharT>& stream, unsigned short count) {
			std::wstring str;
			for (unsigned short i = 0; i < count; ++i) {
				str += stream.get();
			}
			//std::wcout << "stream = " << wss.str() << "\"" << std::endl;
			return str;
		}

		void tool::DrawRect(
			const Rect &rect,
			wchar_t border,
			wchar_t blank,
			Color	pen,
			Color	brush,
			short	borderWidth
		) {
			Bucket borderBucket(new LoopStream<wchar_t>(border));
			Bucket blankBucket(new LoopStream<wchar_t>(blank));
			Pen	   borderPen(new LoopStream<Color>(pen));
			Pen	   blankPen(new LoopStream<Color>(brush));
			return DrawRect(
				rect
				, *borderBucket.chrs()
				, *blankBucket.chrs()
				, *borderPen.color()
				, *blankPen.color()
				, borderWidth
			);
		}

		void tool::DrawRect(
			const Rect &rect,
			StreamBase<wchar_t>& borderStream,
			StreamBase<wchar_t>& blankStream,
			StreamBase<Color>&	penStream,
			StreamBase<Color>&	brushStream,
			short borderWidth
		) {
			// ʧ������ж�
			if (rect.point1().x() > rect.point2().x() ||
				rect.point1().y() > rect.point2().y())
				return;
			if (rect.point1().x() < 0 || rect.point2().y() < 0 ||
				rect.point2().x() < 0 || rect.point1().y() < 0)
				return;
			if (borderWidth < 0)
				return;

			// startPoint������ֱ�ӽ���ת��
			// ��ΪborderChr��blankChr������һ��������ȫ��.
			// ��Ҫʹ��ʱ�ж�
			Point	startPoint{ rect.point1() };	// ����߼�����

			short	rectLen{ rect.length() },		// ���γ�
				rectWid{ rect.width() };		// ���ο�

												// ת��ϵ��
			short	cvtCftBd{ tool::IsFullWidthChar(borderStream) ? 2 : 1 },
				cvtCftBk{ tool::IsFullWidthChar(blankStream) ? 2 : 1 };

			// ����ַ���߿��ַ���ת��ϵ��
			// cvtDiff * cvtCftKb = cvtCftBd
			double cvtDiff{ (cvtCftBk / static_cast<double>(cvtCftBd)) };

			short	numBlank{ static_cast<short>((rectLen - ((cvtCftBd == 2) ? 1 : 2) * borderWidth * cvtCftBd) / cvtDiff) },	// ÿһ�е�blank��Ŀ
				numBlankLine{ rectWid - 2 * borderWidth },	// blank����
				numBlankAppend{ static_cast<short>((rectLen - ((cvtCftBd == 2) ? 1 : 2) * borderWidth * cvtCftBd)) - numBlank * cvtCftBk };

			// �������: �߿�Ϊ��� ���Ϊȫ��
			// ���ܻᵼ�²�������
			// �ѳ������̡�һ�¾ͺ���

			//// ��Ҫ�õ����ַ���
			//std::wstring	borderBlock(borderWidth, borderChr),
			//	borderLine(rectLen, borderChr),
			//	blankLine(numBlank, blankChr);

			auto getBorderLine = [&]()->std::wstring {
				std::wstring str;
				for (short i = 0; i < rectLen; ++i)
					str += borderStream.get();
				return str;
			};

			auto getBlankLine = [&]()->std::wstring {
				std::wstring str;
				for (short i = 0; i < numBlank; ++i)
					str += blankStream.get();
				return str;
			};

			auto getBorderBlock = [&]()->std::wstring {
				std::wstring str;
				for (short i = 0; i < borderWidth; ++i)
					str += borderStream.get();
				return str;
			};

			// �������top
			for (short i = 0; i < borderWidth; ++i) {
				tool::ConsoleShow(getBorderLine(), { startPoint.x() * cvtCftBd, startPoint.y() + i }, penStream);
			}

			// �������bottom
			for (short i = 0; i < borderWidth; ++i) {
				tool::ConsoleShow(getBorderLine(), { startPoint.x() * cvtCftBd, startPoint.y() + borderWidth + numBlankLine + i }, penStream);
			}

			for (short i = 0; i < numBlankLine; ++i) {
				// �����߿�
				tool::ConsoleShow(getBorderBlock(), { startPoint.x() * cvtCftBd, startPoint.y() + i + borderWidth }, penStream);

				// ����ұ߿�
				tool::ConsoleShow(getBorderBlock(), { startPoint.x() * cvtCftBd + borderWidth * cvtCftBd + numBlank * cvtCftBk + ((numBlankAppend > 0) ? numBlankAppend : 0), startPoint.y() + i + borderWidth }, penStream);

				// ���
				tool::ConsoleShow(getBlankLine(), { startPoint.x() * cvtCftBd + borderWidth * cvtCftBd, startPoint.y() + i + borderWidth }, brushStream);
			}
		}

		void tool::DrawRectAbs(
			const Rect &rect,
			StreamBase<wchar_t>& borderStream,
			StreamBase<wchar_t>& blankStream,
			StreamBase<Color>&   penStream,
			StreamBase<Color>&   brushStream,
			short                borderWidth,
			StreamBase<char>&    fixer
		) {
			// ʧ������ж�
			if (rect.point1().x() > rect.point2().x() ||
				rect.point1().y() > rect.point2().y())
				return;
			if (rect.point1().x() < 0 || rect.point2().y() < 0 ||
				rect.point2().x() < 0 || rect.point1().y() < 0)
				return;
			if (borderWidth < 0)
				return;

			Point	startPoint{ rect.point1() };	// ����߼�����

			short	rectLen{ rect.length() },		// ���γ�
					rectWid{ rect.width() };		// ���ο�

			bool	isFullWidthBorder{ tool::IsFullWidthChar(borderStream.ex()) },
					isFullWidthBlank{ tool::IsFullWidthChar(blankStream.ex()) };

			short	borderDifference{ toShort(isFullWidthBorder) + 1 },
					blankDifference{ toShort(isFullWidthBlank) + 1 },
					borderWidthOneSide{ borderWidth * borderDifference },
					fillLength{ rectLen - 2 * borderWidthOneSide },
					fillHeight{ rectWid - 2 * borderWidth };

			short   numBorderBlock{ rectLen / borderDifference },			// �߿���Ҫ���ַ���
					numBlankBlock{ (rectLen - borderWidthOneSide * 2) / blankDifference },	// �����Ҫ���ַ���
					numBorderFill{ rectLen - numBorderBlock * borderDifference },	// �߿���Ҫ�����ַ���
					numBlankFill{ fillLength - numBlankBlock * blankDifference };	// �ڲ���Ҫ�����ַ���

			//std::cout << numBorderBlock << std::endl << numBlankBlock << std::endl;

			std::wstring
					strBorder{ GetStringFromStream(borderStream, numBorderBlock) },		// �߿��ַ���(��)
					strBorderSide{ GetStringFromStream(borderStream, borderWidth) },	// �߿��ַ���(һ��)
					strBlank{ GetStringFromStream(blankStream, numBlankBlock) };		// �ڲ��ַ���

			if (0 != numBorderFill) { // �߿���Ҫ�
				strBorder.insert(strBorder.size() / 2, GetStringFromStream(fixer, numBorderFill));
			}
			if (0 != numBlankFill) { // �ڲ���Ҫ�
				strBlank.insert(strBlank.size() / 2, GetStringFromStream(fixer, numBlankFill));
			}

			for (short i = 0; i < borderWidth; ++i) {
				ConsoleShow(strBorder, { startPoint.x(), startPoint.y() + i }, penStream);
				ConsoleShow(strBorder, { startPoint.x(), startPoint.y() + borderWidth + fillHeight + i }, penStream);
			}

			for (short i = 0, y = startPoint.y() + borderWidth + i; i < fillHeight; ++i, ++y) {
				ConsoleShow(strBorderSide, { startPoint.x(), y }, penStream);	// ��߿�
				ConsoleShow(strBorderSide, { startPoint.x() + borderWidthOneSide + fillLength, y }, penStream);	// �б߿�
				ConsoleShow(strBlank, { startPoint.x() + borderWidthOneSide, y }, brushStream);	// ���
			}

		}

		std::vector<std::pair<std::wstring, Point>>
		tool::printInLines(const std::wstring& str, Rect rect) {
			Point	startPoint{ rect.point1() };
			size_t	strHalfLen{ HalfLen(str) };

			short	startX{ startPoint.x() },
					startY{ startPoint.y() },
					printableLength{ rect.length() },
					printableWidth{ rect.width() },
					maxPrint{ printableLength * printableWidth },
					numLineNeed{ toShort(strHalfLen) / printableLength };	
			if (numLineNeed * printableLength < toShort(strHalfLen))
				++numLineNeed;
			short	ySpace{ (printableWidth - numLineNeed) / 2 };
			
			std::wstring	cur;
			short			curCount{ 0 };
			size_t			cursor{ 0 };
			std::vector<std::pair<std::wstring, Point>>
							res;
			//std::cout << "printableLength = " << printableLength << std::endl;
			for (short i = 0; i < printableWidth && cursor < str.size(); ++i, ++cursor) {
				curCount = 0;
				cur.clear();
				
				while (curCount <= printableLength && cursor < str.size()) {
					if (tool::IsFullWidthChar(str[cursor])) {
						if (curCount + 2 > printableLength)
							break;
						cur += str[cursor++];
						curCount += 2;
					}
					else {
						cur += str[cursor++];
						++curCount;
					}
					//std::cout << "curCount = " << curCount << std::endl;
				}
				
				short	x{ startX + (printableLength - curCount) / 2 },
						y{ startY + i + ySpace };
				res.push_back(std::make_pair(cur, Point{x, y}));
			}



			return res;
		}

		std::vector<std::wstring>
		tool::SplitStrInLines(const std::wstring& str) {
			std::wstring cur;
			std::vector<std::wstring> res;
			for (auto x : str) {
				if (x == '\r' || x == '\n') {
					if (!cur.empty())
						res.push_back(cur);
					continue;
				}
				cur += x;
			}
			if (!cur.empty())
				res.push_back(cur);
			return res;
		}

		std::wstring
		tool::GetFullWidthLenStr(const std::wstring& str, unsigned short len) {
			std::wstring	cur;
			size_t			cursor{ 0 };
			for (unsigned short i = 0; i < len && cursor < str.size();) {
				if (tool::IsFullWidthChar(str[cursor])) {
					if (i + 2 > len)
						return cur;
					cur += str[cursor++];
					i	+= 2;
				}
				else {
					cur += str[cursor++];
					++i;
				}
			}
			return cur;
		}
	}

}