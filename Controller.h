#pragma once

#include <map>
#include <list>
#include <queue>
#include <vector>
#include <thread>
#include <memory>
#include <codecvt>

#include <conio.h>
#include <Windows.h>

#include <Object.h>
#include <Signal.h>
//#include <Surround.h>

#define GBK_NAME ".936" 

namespace sweet {

	namespace cmd {

		class ConsoleHelper
		{
			HANDLE _hIn;
			HANDLE _hOut;
			INPUT_RECORD _InRec;
			DWORD _NumRead;
		public:
			WORD VKey;
			ConsoleHelper(void) {
				_hIn = GetStdHandle(STD_INPUT_HANDLE);
				_hOut = GetStdHandle(STD_OUTPUT_HANDLE);
				VKey = 0;
			}
			bool ReadOneInput()
			{
				return 0 != ReadConsoleInput(_hIn, &_InRec, 1, &_NumRead);
			}
			bool ReadOneInput(INPUT_RECORD& InRec)
			{
				return 0 != ReadConsoleInput(_hIn, &InRec, 1, &_NumRead);
			}
			DWORD ReadKeyDown()
			{
				if (!ReadConsoleInput(_hIn, &_InRec, 1, &_NumRead))
					return 0;
				if (_InRec.EventType != KEY_EVENT)
					return 0;
				if (_InRec.Event.KeyEvent.bKeyDown > 0)
					return 0;
				VKey = _InRec.Event.KeyEvent.wVirtualKeyCode;
				return VKey;
			}
			DWORD ReadKeyPush()
			{
				if (!ReadConsoleInput(_hIn, &_InRec, 1, &_NumRead))
					return 0;
				if (_InRec.EventType != KEY_EVENT)
					return 0;
				if (_InRec.Event.KeyEvent.bKeyDown == 0)
					return 0;
				VKey = _InRec.Event.KeyEvent.wVirtualKeyCode;
				return VKey;
			}
		public:
			~ConsoleHelper(void) {}
		};

		class Controller {
		private:
			std::map<std::string, Controller*> _friends;

		public:
			virtual ~Controller() = default;

			Controller*  getFriendWithTag(const std::string& tag) {
				if (!_friends.count(tag))
					return nullptr;
				return _friends[tag];
			}
			bool		 hasTag(const std::string& tag) {
				return 0 < _friends.count(tag);
			}
			bool		 removeFriend(const std::string& tag) {
				if (hasTag(tag)) {
					_friends.erase(_friends.find(tag));
					return true;
				}
				return false;
			}
			bool		 addFriend(Controller* ctrl, const std::string& tag) {
				if (hasTag(tag))
					return false;
				_friends.insert(std::make_pair(tag, ctrl));
				return true;
			}
			std::string  find(Controller* ctrl) {
				for (auto itr = _friends.begin(); _friends.end() != itr; ++itr) {
					if (itr->second == ctrl)
						return itr->first;
				}
				return std::string();
			}

			virtual void go() = 0;
			virtual void pause() = 0;
			virtual bool active() = 0;
		};

		class MoveController : public Controller {
			typedef std::list<Object*> listOfObjects;
			typedef std::map<short, listOfObjects> rowType;
			typedef std::map<short, rowType> squareType;
			typedef squareType::iterator lineIterator;
			typedef rowType::iterator rowIterator;
			typedef listOfObjects::iterator cursorIterator;
			typedef squareType::reverse_iterator lineReverseIterator;
			typedef rowType::reverse_iterator rowReverseIterator;
			typedef listOfObjects::reverse_iterator cursorReverseIterator;

			template < typename T >
			struct WeightedElement {
				short weight;
				T	  element;

				WeightedElement(const T& e, short w)
					: weight(w)
					, element(e) {}
				friend bool operator>(const WeightedElement<T> &lhs, const WeightedElement<T>& rhs) {
					return(lhs.weight > rhs.weight);
				}
			};
			struct Iterators3 {
				lineIterator itr1;
				rowIterator itr2;
				cursorIterator itr3;

				Iterators3(lineIterator i1, rowIterator i2, cursorIterator i3)
					: itr1(i1)
					, itr2(i2)
					, itr3(i3) {}
			};

		private:
			squareType
								_ground;
			Object*				_watch;
			std::unique_ptr<lineIterator>
								_line;
			std::unique_ptr<rowIterator>
								_row;
			std::unique_ptr<cursorIterator>
								_cursor;
			bool				_state;
			size_t				_count;

		public:
			MoveController(Object* wh)
				: _line(nullptr)
				, _row(nullptr)
				, _cursor(nullptr)
				, _state(false)
				, _count(0) {
				setWatch(wh);
			}

			Object*	watch() { return _watch; }
			void	setWatch(Object* nwatch) {
				_watch = nwatch;
				slot_InsertObject = _watch->sig_AddSubObject.connect(this, &MoveController::insertObjectBinder);
				slot_RemoveObject = _watch->sig_RemoveSubObject.connect(this, &MoveController::removeObjectBinder);
				std::vector<Object*>& vtr = _watch->_subObjects;
				clearGround();
				insertObject(vtr.begin(), vtr.end());
			}
			bool exists(short line, short row) { return(0 < _ground.count(line) && 0 < _ground[line].count(row)); }
			bool exists(short line) { return(0 < _ground.count(line)); }

			virtual void go() { _state = true; if (_count >= 1) setFirstSelected(); }
			virtual void pause() { _state = false; }
			virtual bool active() { 
				return _state; 
			}

			void goUp() {
				if (nullptr == current() && _count > 0)
					setFirstSelected();
				if (!active() || !current())
					return;
				// ���С�ڵ���һ��
				// ��ô����Ҳ�ǰ���
				if (_ground.size() <= 1)
					return;
				Object* old{ current() };
				short   targetX{ old->boundingRect().point1().x() },
						length{ old->boundingRect().length() };
				
				std::priority_queue<
					WeightedElement<Iterators3>,
					std::vector<WeightedElement<Iterators3>>,
					std::greater<WeightedElement<Iterators3>>
				> que;
				lineIterator nextLine{ getNextLine(*_line, true) };	// �õ���һ�еĵ�����

				for (; nextLine != *_line; ) {
					// ��������ÿһ��
					rowIterator closest = search4theClosestRow(nextLine, targetX);
					// ����������Ѿ�����ԭѡ��Ķ���ĳ���
					if (closest->first > targetX || (*closest->second.begin())->boundingRect().length() + closest->first < targetX)
						// �����Ԯ��
						que.push({ { nextLine, closest, closest->second.begin() }, closest->first });
					else {
						// ������һ����ʼ���겻����ԭ�����
						*_line = nextLine;
						*_row = closest;
						*_cursor = closest->second.begin();
						// �����ź�
						sig_onSelection(this, old);
						return;
					}
					nextLine = getNextLine(nextLine, true);	// ������һ��
				}
				// �Ӻ�Ԯ��������μ�ĵ���
				Iterators3 top = que.top().element;
				*_line = top.itr1;
				*_row = top.itr2;
				*_cursor = top.itr3;
				// �����ź�
				sig_onSelection(this, old);
			}
			void goDown() {
				if (nullptr == current() && _count > 0)
					setFirstSelected();
				if (!active() || !current())
					return;
				// ���С�ڵ���һ��
				// ��ô����Ҳ�ǰ���
				if (_ground.size() <= 1)
					return;
				Object* old{ current() };
				short   targetX{ old->boundingRect().point1().x() },
					length{ old->boundingRect().length() };

				std::priority_queue<
					WeightedElement<Iterators3>,
					std::vector<WeightedElement<Iterators3>>,
					std::greater<WeightedElement<Iterators3>>
				> que;
				lineIterator nextLine{ getNextLine(*_line, false) };	// �õ���һ�еĵ�����

				for (; nextLine != *_line; ) {
					// ��������ÿһ��
					rowIterator closest = search4theClosestRow(nextLine, targetX);
					// ����������Ѿ�����ԭѡ��Ķ���ĳ���
					if (closest->first > targetX || (*closest->second.begin())->boundingRect().length() + closest->first < targetX)
						// �����Ԯ��
						que.push({ { nextLine, closest, closest->second.begin() }, closest->first });
					else {
						// ������һ����ʼ���겻����ԭ�����
						*_line = nextLine;
						*_row = closest;
						*_cursor = closest->second.begin();
						// �����ź�
						sig_onSelection(this, old);
						return;
					}
					nextLine = getNextLine(nextLine, true);	// ������һ��
				}
				// �Ӻ�Ԯ��������μ�ĵ���
				Iterators3 top = que.top().element;
				*_line = top.itr1;
				*_row = top.itr2;
				*_cursor = top.itr3;
				// �����ź�
				sig_onSelection(this, old);
			}
			void goLeft() {
				if (nullptr == current() && _count > 0)
					setFirstSelected();
				if (!active() || !current())
					return;
				// ���û�л���ֻ��һ������
				if (_count <= 1)
					return;
				Object* old{ current() };

				if (*_row != (++(*_line)->second.rbegin()).base()) {	// ��ǰ����߻��ж���
					--(*_row);	// ������ǰ�������ƶ�
					*_cursor = (*_row)->second.begin();
					// �����ź�
					sig_onSelection(this, old);
					return;
				}
				if (*_row != (*_line)->second.begin()) {	// ��ǰ�����û�ж��� �ұ��ж���
					*_row = (++(*_line)->second.rbegin()).base();	// ��������Ϊ��������
					*_cursor = (*_row)->second.begin();
					// �����ź�
					sig_onSelection(this, old);
					return;
				}
				// ������ν�ãȻ
				short	oldx{ old->boundingRect().point1().x() },
						oldy{ old->boundingRect().point1().y() };
				// ���������н���ɸ��ѡ
				lineIterator itr = *_line;
				if (itr == _ground.begin())
					itr = (++(_ground.rbegin())).base();
				else
					--itr;
				while(itr != *_line){
					rowIterator start = itr->second.begin(),
								end = itr->second.end();
					rowIterator res = search4theClosestRowLess(start, end, oldx);
					if (res != end) {
						short newHeight{ (*(res->second.begin()))->boundingRect().width() };
						if (itr->first + newHeight >= oldy) {
							*_line = itr;
							*_row = res;
							*_cursor = (*_row)->second.begin();
							// �����ź�
							sig_onSelection(this, old);
							return;
						}
					}
					if (itr == _ground.begin()) {
						if (*_line == _ground.begin())
							break;
						else
							itr = (++(_ground.rbegin())).base();
					}
					else
						--itr;
				}
				for (auto i = _ground.begin(); _ground.end() != i; ++i) {
					if (i != *_line) {
						if (i->first >= oldy && i->first <= oldy + old->boundingRect().width()) {
							*_line = i;
							*_row = (++(i->second.rbegin())).base();
							*_cursor = (*_row)->second.begin();
							// �����ź�
							sig_onSelection(this, old);
							return;
						}
					}
				}
				sig_onSelection(nullptr, old);
				setFirstSelected();
			}
			void goRight() {
				if (nullptr == current() && _count > 0)
					setFirstSelected();
				if (!active() || !current())
					return;
				// ���û�л���ֻ��һ������
				if (_count <= 1)
					return;
				Object* old{ current() };

				if (*_row != (*_line)->second.begin()) {	// ��ǰ���ұ߻��ж���
					++(*_row);	// ������ǰ�������ƶ�
					*_cursor = (*_row)->second.begin();
					// �����ź�
					sig_onSelection(this, old);
					return;
				}
				if (*_row != (*_line)->second.begin()) {	// ��ǰ���ұ�û�ж��� ����ж���
					*_row = (*_line)->second.begin();		// ��������Ϊ��������
					*_cursor = (*_row)->second.begin();
					// �����ź�
					sig_onSelection(this, old);
					return;
				}
				// ������ν�ãȻ
				short	oldx{ old->boundingRect().point1().x() },
					oldy{ old->boundingRect().point1().y() };
				// ���������н���ɸ��ѡ
				lineIterator itr = *_line;
				if (itr == (++(_ground.rbegin())).base())
					itr = _ground.begin();
				else
					++itr;
				while (itr != *_line) {
					rowIterator start = itr->second.begin(),
						end = itr->second.end();
					rowIterator res = search4theClosestRowGreater(start, end, oldx);
					if (res != end) {
						short newHeight{ (*(res->second.begin()))->boundingRect().width() };
						if (itr->first + newHeight >= oldy) {
							*_line = itr;
							*_row = res;
							*_cursor = (*_row)->second.begin();
							// �����ź�
							sig_onSelection(this, old);
							return;
						}
					}
					if (itr == (++(_ground.rbegin())).base()) {
						if (*_line == (++(_ground.rbegin())).base())
							break;
						else
							itr = _ground.begin();
					}
					else
						++itr;
				}
				for (auto i = _ground.begin(); _ground.end() != i; ++i) {
					if (i != *_line) {
						if (i->first >= oldy && i->first <= oldy + old->boundingRect().width()) {
							*_line = i;
							*_row = i->second.begin();
							*_cursor = (*_row)->second.begin();
							// �����ź�
							sig_onSelection(this, old);
							return;
						}
					}
				}
				sig_onSelection(nullptr, old);
				setFirstSelected();
			}
			Object* current() {
				if (nullptr == _cursor.get())
					return nullptr;
				return **_cursor;
			}

		slots:
			sweet::Slot slot_InsertObject;
			sweet::Slot slot_RemoveObject;

		signals:
			sweet::Signal<void(MoveController*, Object*)> sig_onSelection;

		protected:
			// ���ó���ѡ�����
			// һ��Ϊ���ϽǵĶ���
			void setFirstSelected() {
				_line.reset(new lineIterator(_ground.begin()));
				_row.reset(new rowIterator((*_line.get())->second.begin()));
				_cursor.reset(new cursorIterator((*_row.get())->second.begin()));
				sig_onSelection(this, nullptr);
			}
			// ���
			void clearGround() {
				for (auto itr = _ground.begin(); _ground.end() != itr; ++itr)
					itr->second.clear();
				_ground.clear();
			}
			// �������
			void insertObject(Object *obj) {
				Point p{ obj->boundingRect().point1() };
				short x{ p.x() }, y{ p.y() };
				if (exists(y)) {
					if (_ground[y].count(x) == 0)
						_ground[y].insert(std::make_pair(x, std::list<Object*>()));
				}
				else {
					_ground.insert(std::make_pair(y, std::map<short, std::list<Object*>>()));
					_ground[y].insert(std::make_pair(x, std::list<Object*>()));
				}
				_ground[y][x].push_back(obj);
			}
			// ������󷺻���
			template < typename ForwardIterator >
			void insertObject(ForwardIterator begin, ForwardIterator end) {
				for (; end != begin; ++begin) {
					insertObject(*begin);
					++_count;
				}
			}
			// �Ƴ�����
			bool removeObject(Object* obj) {
				for (auto itr1 = _ground.begin(); itr1 != _ground.end(); ++itr1)
					for (auto itr2 = itr1->second.begin(); itr1->second.end() != itr2; ++itr2)
						for (auto itr3 = itr2->second.begin(); itr2->second.end() != itr3; ++itr3)
							if (obj == *itr3) {		// �ҵ�Ŀ�����
								if (itr3 == *_cursor) {
									if(_count > 1)	// ���ɾ��Ψһ�Ķ���
										search4theClosestObject(*_line, *_row, *_cursor);
									else {
										_line.release();
										_row.release();
										_cursor.release();
										_state = false;
									}
								}
								if (itr3 != *_cursor) {	// ���Ŀ�����û�б�ѡ��
									itr2->second.erase(itr3);		// ��Ŀ��vector��ɾ��
									if (itr2->second.empty())		// �������ûʲô������..
										itr1->second.erase(itr2);	// ɾ����һ��
									if (itr1->second.empty())		// ������ж�ûʲô������
										_ground.erase(itr1);
								}
								--_count;
								return true;
							}
				return false;
			}

			// �л���
			lineIterator getNextLine(lineIterator line, bool up) {
				if (up) {
					if (line == _ground.begin())	// ����Ѿ��ǵ�һ��
						return (++_ground.rbegin()).base();	// �����������һ��
					return --line;
				}
				// ���µ����
				// ����Ѿ������һ��
				if (line == (++_ground.rbegin()).base())
					return _ground.begin();
				return ++line;
			}

			bool switchLineGreater(lineIterator& line, short value) {
				for (auto tmp = line; _ground.end() != tmp; ++tmp) {
					if (tmp->first > value) {
						line = tmp;
						return true;
					}
				}
				return false;
			}

			// ��line��Ѱ����value���������
			rowIterator search4theClosestRow(lineIterator line, short value) {
				std::priority_queue<
					WeightedElement<rowIterator>,
					std::vector<WeightedElement<rowIterator>>,
					std::greater<WeightedElement<rowIterator>>
				> difQueue;
				auto begin = (line->second).begin(),
					end = (line->second).end();
				
				for (auto i = 0; end != begin; ++begin, ++i) {
					if (difQueue.empty() && begin->first > value)
						return begin;
					difQueue.push(WeightedElement<rowIterator>(begin, abs(begin->first - value)));
				}
				
				return difQueue.top().element;
			}
			template < typename RowIterator >
			RowIterator search4theClosestRowGreater(RowIterator start, RowIterator end, short value) {
				for (; start != end; ++start) {
					if (start->first > value)
						return start;
				}
				return end;
			}
			template < typename RowIterator >
			RowIterator search4theClosestRowLess(RowIterator start, RowIterator end, short value) {
				for (; start != end; ++start) {
					if (start->first < value)
						return start;
				}
				return end;
			}
			Object*		search4theClosestObject(lineIterator& line, rowIterator& row, cursorIterator& cursor) {
				// �ж��ص�
				cursorIterator tmp{ cursor };
				tmp++;
				if (cursor != row->second.end() && row->second.end() != tmp) {
					++cursor;
					return *cursor;
				}
				if (row->second.begin() != cursor) {
					--cursor;
					return *cursor;
				}
				Object*		current{*cursor};
				rowIterator end{line->second.end()},
							leftRow{end},
							rightRow{end};
				short		x{ current->boundingRect().point1().x() };

				// �ж�ͬһ�е�������ұ�
				// ����map���� ����ֻ���ж�������ұ�
				if (line->second.end() != row) {
					rowIterator tmp = row;
					rightRow = ++tmp;
				}
				if (line->second.begin() != row) {
					rowIterator tmp = row;
					leftRow = --tmp;
				}
				short	leftDistance{ 0 },
						rightDistance{ 0 };
				if (leftRow != end)
					leftDistance = abs(leftRow->second.front()->boundingRect().point1().x() - x);
				if(rightRow != end)
					rightDistance = abs(rightRow->second.front()->boundingRect().point1().x() - x);
			
				if (leftRow != end && rightRow != end) {
					if (leftDistance > rightDistance) {
						row = rightRow;
						cursor = rightRow->second.begin();
						return *cursor;
					}
					row = leftRow;
					cursor = leftRow->second.begin();
					return *cursor;
				}
				if (leftRow != end) {
					row = leftRow;
					cursor = leftRow->second.begin();
					return *cursor;
				}
				else if (rightRow != end) {
					row = rightRow;
					cursor = rightRow->second.begin();
					return *cursor;
				}

				// Ѱ����, �ظ�Ѱ����Ч��
				if (_ground.end() != line) {
					++line;
					rowIterator	 rowTmp = line->second.begin();
					cursorIterator cursorTmp = rowTmp->second.begin();
					return search4theClosestObject(line, rowTmp, cursorTmp);
				}
				else {
					--line;
					rowIterator	 rowTmp = line->second.begin();
					cursorIterator cursorTmp = rowTmp->second.begin();
					return search4theClosestObject(line, rowTmp, cursorTmp);
				}
			}

			void search4theClosestTowards(
				lineIterator& line,
				rowIterator& row,
				cursorIterator& cursor,
				bool isLeft
			) {
				if (isLeft) {
					if (line->second.begin() != row) {	// ��ǰ����߻��в���
						--row;
						cursor = row->second.begin();
						return;
					}
					else if (line->second.end() != row && (++(line->second.rbegin())).base() != row) {	// ����Ѿ�û���� ���ұ�
						row = (++(line->second.rbegin())).base();	// ���ұ�
						cursor = row->second.begin();
						return;
					}
				}
				else {
					if (line->second.end() != row && (++line->second.rbegin()).base() != row) {	// ��ǰ���ұ߻��в���
						++row;
						cursor = row->second.begin();
						return;
					}
					else if (line->second.begin() != row) {	// �ұ��Ѿ�û���� �����
						row = line->second.begin();
						cursor = row->second.begin();
						return;
					}
				}
				short x{ row->first };
				// ��ǰ��Ϊ�յ����
				if (isLeft) {
					// ����������������
					for (auto tmp = line; line != _ground.begin(); --tmp) {
						rowReverseIterator begin{ tmp->second.rbegin() },
							end{ tmp->second.rend() };

						rowReverseIterator res{ search4theClosestRowLess(begin, end, x) };
						if (end != res) {
							line = tmp;
							row = (++res).base();
							cursor = row->second.begin();
							return;
						}
					}
					// ��ǰ���Ϸ�û�з��ϵĲ���
					// �����������
					// �ҵ�һ������һ��Ԫ�ص����Ҷ�
					for (auto tmp = _ground.begin(); tmp != _ground.end(); ++tmp) {
						if (tmp != line && tmp->second.size() >= 1) {
							line = tmp;
							row = (++line->second.rbegin()).base();
							cursor = row->second.begin();
							return;
						}
					}
				}
				else {
					for (auto tmp = line; line != _ground.begin(); --tmp) {
						rowIterator begin{ tmp->second.begin() },
							end{ tmp->second.end() };
						rowIterator res{ search4theClosestRowGreater(begin, end, x) };
						if (res != end) {
							line = tmp;
							row = res;
							cursor = row->second.begin();
							return;
						}
					}
					// ��ǰ���Ϸ�û�з��ϵĲ���
					// �����������
					// �ҵ�һ������һ��Ԫ�ص������
					for (auto tmp = _ground.begin(); tmp != _ground.end(); ++tmp) {
						if (tmp != line && tmp->second.size() >= 1) {
							line = tmp;
							row = line->second.begin();
							cursor = row->second.begin();
							return;
						}
					}
				}


			}

		public:
			void insertObjectBinder(Object*, Object* obj) { insertObject(obj); }
			void removeObjectBinder(Object*, Object* obj) { removeObject(obj); }

		};

		class KeyController : public Controller {
		private:
			std::unique_ptr<std::thread> _thr;	// �߳�(ִ�а����ж�)
			ConsoleHelper				 _gch;	// ��������

		public:
			virtual ~KeyController() = default;

			virtual void go();
			virtual void pause() { _thr.reset(nullptr); }
			virtual bool active() { return nullptr != _thr.get(); }

		signals:
			sweet::Signal<void(Controller*, int)> sig_KeyPressed;

		};

		void KeyController::go() {
			if (nullptr == _thr.get())				// ����߳�û�д���
				_thr.reset(new std::thread);

			_thr.get()->swap(std::thread([&]() {	// ���� + �����߳�
				for (; ;) {
					// ��ⰴ�� �����ź�
					if (_gch.ReadKeyPush() != 0) {
						// ���Ͱ�����Ϣ
						sig_KeyPressed(this, _gch.VKey);
					}
					Sleep(10);	// take a breath.
				}
			}));
			_thr.get()->detach();
		}

		class KeyLastController : public Controller {
		private:
			bool			_codesMap[256];
			std::unique_ptr<std::thread>
							_thr;
			ConsoleHelper	_gch;
			bool			_state;

		public:
			KeyLastController() {}
			virtual ~KeyLastController() = default;

			virtual void go();
			virtual void pause() { _state = false; }
			virtual bool active() { return _state; }

		signals:
			sweet::Signal<void(KeyLastController*, int, bool)> sig_KeyPressed;
		};

		void KeyLastController::go() {
			if (nullptr == _thr.get())				// ����߳�û�д���
				_thr.reset(new std::thread);

			_state = true;
			_thr->swap(std::thread([&]() {
				for (;_state;) {
					// ��������
					if (!_codesMap[_gch.VKey] && 0 != _gch.ReadKeyPush()) {
						_codesMap[_gch.VKey] = true;
						sig_KeyPressed(this, _gch.VKey, true);
					}
					// ��������
					if (_codesMap[_gch.VKey] && 0 != _gch.ReadKeyDown()) {
						_codesMap[_gch.VKey] = false;
						sig_KeyPressed(this, _gch.VKey, false);
					}
					Sleep(10);
				}
			}));
			_thr->detach();
		}

		class InputController : public Controller {
		private:
			int				_keyEscape;
			std::unique_ptr<std::thread> _thr;
			bool			_state;

		signals:
			sweet::Signal<void(InputController*, wchar_t)> sig_CharRecv;
			sweet::Signal<void(InputController*)> sig_RecvFinish;

		public:
			InputController(int key2Escape = VK_ESCAPE)
				: _keyEscape(key2Escape) {}

			virtual void go() {
				if (nullptr == _thr.get())
					_thr.reset(new std::thread());
				_state = true;
				_thr->swap(std::thread([&]() {
					char ch;
					bool chinese{ false };
					std::string chineseBuffer;
					if (_state) {
						while (ch = _getch()) {
							if (ch == _keyEscape)
								break;
							if (ch < 0) {	// �ӵ�һ������
								if (!chinese) {
									chineseBuffer += ch;
									chinese = true;
									continue;
								}
								if (chinese) {
									chineseBuffer += ch;
									chinese = false;
									using wcharGBK = std::codecvt_byname<wchar_t, char, mbstate_t>;
									std::wstring_convert<wcharGBK>
										_cvtGBK(new wcharGBK(GBK_NAME));	// ������α���װ��װ��

									std::wstring tmp(_cvtGBK.from_bytes(chineseBuffer));
									sig_CharRecv(this, tmp[0]);
									chineseBuffer.clear();
								}
							}
							else sig_CharRecv(this, ch);
						}
					}
					sig_RecvFinish(this);
				}));
				_thr->detach();
			}
			virtual void pause() {
				_state = false;
			}
			virtual bool active() {
				return _state;
			}

		};
	}

}