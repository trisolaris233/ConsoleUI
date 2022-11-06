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
			static std::size_t	HalfLen(const std::wstring &str);								// 计算半角字符数
			template < class T >
			static void			ConsoleShow(const T &msg, Point coord, char color);				// 控制台输出数据
			template <>
			static void			ConsoleShow(const std::wstring &wmsg, Point coord, char color);	// 宽字符串特化
			static void			ConsoleShow(const std::wstring &wmsg, Point coord, StreamBase<Color>& color);	// 宽字符特化
			static void			GotoXY(short x, short y);										// 移动光标至(x, y)
			static void			SetCursorVisible(bool visible);									// 设置光标是否可见
			static int			GetWindowWidth();												// 获得当前控制台宽度
			static int			GetWindowHeight();												// 获得当前控制台高度
			static void			ClearConsole(Point p, std::size_t len, char color);				// 从p开始清len个字符
			static void			ClearConsole(Point p, std::size_t len, StreamBase<Color>& color);
			static bool			IsFullWidthChar(wchar_t wch);									// 判断是否为全角字符
			static bool			IsFullWidthChar(StreamBase<wchar_t>& wchrStream);	
			template < typename CharT >
			static std::wstring		
								GetStringFromStream(StreamBase<CharT>& stream, unsigned short count);// 从strem中获得x个字符
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
			// 判断高位是否为0x00
			// 如果是说明不是全角字符
			// 反之则是。
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
			// 失败情况判断
			if (rect.point1().x() > rect.point2().x() ||
				rect.point1().y() > rect.point2().y())
				return;
			if (rect.point1().x() < 0 || rect.point2().y() < 0 ||
				rect.point2().x() < 0 || rect.point1().y() < 0)
				return;
			if (borderWidth < 0)
				return;

			// startPoint不可以直接进行转化
			// 因为borderChr与blankChr其中有一个可能是全角.
			// 需要使用时判断
			Point	startPoint{ rect.point1() };	// 起点逻辑坐标

			short	rectLen{ rect.length() },		// 矩形长
				rectWid{ rect.width() };		// 矩形宽

												// 转换系数
			short	cvtCftBd{ tool::IsFullWidthChar(borderStream) ? 2 : 1 },
				cvtCftBk{ tool::IsFullWidthChar(blankStream) ? 2 : 1 };

			// 填充字符与边框字符的转换系数
			// cvtDiff * cvtCftKb = cvtCftBd
			double cvtDiff{ (cvtCftBk / static_cast<double>(cvtCftBd)) };

			short	numBlank{ static_cast<short>((rectLen - ((cvtCftBd == 2) ? 1 : 2) * borderWidth * cvtCftBd) / cvtDiff) },	// 每一行的blank数目
				numBlankLine{ rectWid - 2 * borderWidth },	// blank行数
				numBlankAppend{ static_cast<short>((rectLen - ((cvtCftBd == 2) ? 1 : 2) * borderWidth * cvtCftBd)) - numBlank * cvtCftBk };

			// 特殊情况: 边框为半角 填充为全角
			// 可能会导致不齐的情况
			// 脱出来调教♂一下就好了

			//// 需要用到的字符串
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

			// 输出矩形top
			for (short i = 0; i < borderWidth; ++i) {
				tool::ConsoleShow(getBorderLine(), { startPoint.x() * cvtCftBd, startPoint.y() + i }, penStream);
			}

			// 输出矩形bottom
			for (short i = 0; i < borderWidth; ++i) {
				tool::ConsoleShow(getBorderLine(), { startPoint.x() * cvtCftBd, startPoint.y() + borderWidth + numBlankLine + i }, penStream);
			}

			for (short i = 0; i < numBlankLine; ++i) {
				// 输出左边框
				tool::ConsoleShow(getBorderBlock(), { startPoint.x() * cvtCftBd, startPoint.y() + i + borderWidth }, penStream);

				// 输出右边框
				tool::ConsoleShow(getBorderBlock(), { startPoint.x() * cvtCftBd + borderWidth * cvtCftBd + numBlank * cvtCftBk + ((numBlankAppend > 0) ? numBlankAppend : 0), startPoint.y() + i + borderWidth }, penStream);

				// 填充
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
			// 失败情况判断
			if (rect.point1().x() > rect.point2().x() ||
				rect.point1().y() > rect.point2().y())
				return;
			if (rect.point1().x() < 0 || rect.point2().y() < 0 ||
				rect.point2().x() < 0 || rect.point1().y() < 0)
				return;
			if (borderWidth < 0)
				return;

			Point	startPoint{ rect.point1() };	// 起点逻辑坐标

			short	rectLen{ rect.length() },		// 矩形长
					rectWid{ rect.width() };		// 矩形宽

			bool	isFullWidthBorder{ tool::IsFullWidthChar(borderStream.ex()) },
					isFullWidthBlank{ tool::IsFullWidthChar(blankStream.ex()) };

			short	borderDifference{ toShort(isFullWidthBorder) + 1 },
					blankDifference{ toShort(isFullWidthBlank) + 1 },
					borderWidthOneSide{ borderWidth * borderDifference },
					fillLength{ rectLen - 2 * borderWidthOneSide },
					fillHeight{ rectWid - 2 * borderWidth };

			short   numBorderBlock{ rectLen / borderDifference },			// 边框需要的字符数
					numBlankBlock{ (rectLen - borderWidthOneSide * 2) / blankDifference },	// 填充需要的字符数
					numBorderFill{ rectLen - numBorderBlock * borderDifference },	// 边框需要填充的字符数
					numBlankFill{ fillLength - numBlankBlock * blankDifference };	// 内部需要填充的字符数

			//std::cout << numBorderBlock << std::endl << numBlankBlock << std::endl;

			std::wstring
					strBorder{ GetStringFromStream(borderStream, numBorderBlock) },		// 边框字符串(行)
					strBorderSide{ GetStringFromStream(borderStream, borderWidth) },	// 边框字符串(一边)
					strBlank{ GetStringFromStream(blankStream, numBlankBlock) };		// 内部字符串

			if (0 != numBorderFill) { // 边框需要填补
				strBorder.insert(strBorder.size() / 2, GetStringFromStream(fixer, numBorderFill));
			}
			if (0 != numBlankFill) { // 内部需要填补
				strBlank.insert(strBlank.size() / 2, GetStringFromStream(fixer, numBlankFill));
			}

			for (short i = 0; i < borderWidth; ++i) {
				ConsoleShow(strBorder, { startPoint.x(), startPoint.y() + i }, penStream);
				ConsoleShow(strBorder, { startPoint.x(), startPoint.y() + borderWidth + fillHeight + i }, penStream);
			}

			for (short i = 0, y = startPoint.y() + borderWidth + i; i < fillHeight; ++i, ++y) {
				ConsoleShow(strBorderSide, { startPoint.x(), y }, penStream);	// 左边框
				ConsoleShow(strBorderSide, { startPoint.x() + borderWidthOneSide + fillLength, y }, penStream);	// 有边框
				ConsoleShow(strBlank, { startPoint.x() + borderWidthOneSide, y }, brushStream);	// 填充
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