#pragma once

#include <vector>
#include <memory>	// for unique_ptr
#include <algorithm>

#include <Signal.h>
#include <Console.h>

namespace sweet {

	namespace cmd {

		class Object;

		// ����̨����Ԫ�صĻ���
		class Object {
			friend class Object;
			friend class Controller;
			//friend class Surround;
			friend class MoveController;

		private:
			Object*					_parent;		// ������
			std::vector<Object*>	_subObjects;	// �Ӳ���
			Rect					_boundingRect;	// λ�úͳߴ�
			bool					_isSelected;	// �Ƿ�ѡ��״̬
			bool					_isVisible;		// �Ƿ����


		public:
			Object(Object *parent = nullptr) 
				: _parent(parent)
				, _isSelected(false)
				, _isVisible(false)		{
				sig_Create(this);
				setParent(_parent);
			}
			Object(Rect rect, Object *parent = nullptr) 
				: _boundingRect(rect)
				, _parent(parent)
				, _isSelected(false)
				, _isVisible(false)	{
				sig_Create(this);
				setParent(_parent);
			}
			virtual ~Object() { sig_Destroy(this); }

			Object*	parent() const { return _parent; }
			bool isSelected() const { return _isSelected; }
			bool isVisible() const { return _isVisible; }
			const Rect&	boundingRect() const { return _boundingRect; }
			std::size_t	countSubObject() const { return _subObjects.size(); }

			void repaint() {
				if (_isVisible) {
					if (nullptr == _parent ||
						_parent->isVisible())
						paint(this);
					for (auto &each : _subObjects)
						if (each && each->isVisible())
							each->repaint();
				}	
				else
					clearRect();
			}

			void setParent(Object *np);	// ���ø�����
			void setRect(Rect &r) { 
				clearRect();	// ����ԭ������

				auto tmp = _boundingRect;
				_boundingRect = r; 

				sig_SetRect(this, tmp, r);	// �����ź�setRect
			}
			void setSelected(bool state) {
				_isSelected = state;

				sig_SetSelected(this, _isSelected);
			}
			void setVisible(bool state) {
				_isVisible = state;
				repaint();
			}

			
			void removeSubObject(Object *obj);
			void addSubObject(Object *obj);
			
			virtual void	show();
			virtual void	refresh();
			virtual short   identifier() = 0;
			
		signals:
			sweet::Signal<void(Object*)>	sig_Create;			// ��������֮�ź�
			sweet::Signal<void(Object*)>	sig_Destroy;		// ��������֮�ź�
			sweet::Signal<void(Object*, Rect, Rect)>	
											sig_SetRect;		// ���ÿؼ��߽����֮�ź�
			sweet::Signal<void(Object*, bool)>
											sig_SetSelected;	// ���ÿؼ�ѡ��״̬֮�ź�
			sweet::Signal<void(Object*, Object*)>
											sig_AddSubObject;	// ����ӿؼ�֮�ź�
			sweet::Signal<void(Object*, Object*)>
											sig_RemoveSubObject;// �Ƴ��ؼ�֮�ź�
			sweet::Signal<void(Object*, bool)>
											sig_Invoke;			// �ؼ�����֮�ź�

		protected:
			virtual void	paint(Object *obj) = 0;	// ����(������ʵ�ִ˷���)
			virtual void	clearRect();	// ��ղ���(������ղ����ı߿����)

		};

		static Point Map2Sense(Object *obj) {
			Point res;
			if (nullptr == obj)
				return res;

			Object *searcher = obj;
			while (searcher) {
				Point point = searcher->boundingRect().point1();
				res.setX(res.x() + point.x());
				res.setY(res.y() + point.y());
				searcher = searcher->parent();
			}
			return res;
		}

		static Point Map2Sense(Point res, Object *obj) {
			if (nullptr == obj)
				return res;

			Object *searcher = obj;
			while (searcher) {
				Point point = searcher->boundingRect().point1();
				res.setX(res.x() + point.x());
				res.setY(res.y() + point.y());
				searcher = searcher->parent();
			}
			return res;
		}

		static short strLen(const std::wstring &str) {
			short res{ 0 };
			for (auto x : str) {
				if (tool::IsFullWidthChar(x))
					res += 2;
				else
					++res;
			}
			return res;
		}

		void Object::setParent(Object *np) {
			if (this == _parent) 
				return;
			Object *oldParent = _parent;
			if (nullptr != oldParent) {
				// �ڸ�������ɾ���Լ�������
				oldParent->removeSubObject(this);
			}
			_parent = np;

			// ����µĸ������ǿ�
			if (nullptr != np) {
				np->addSubObject(this);
			}
		}

		void Object::removeSubObject(Object *obj) {
			if (_subObjects.end() != _subObjects.erase(
				std::remove(
					_subObjects.begin(), _subObjects.end(), obj
				), _subObjects.end())
			) {
				std::cout << "throw up removeSubobject signal.\n";
				sig_RemoveSubObject(this, obj);
			}
		}

		// ����Ӳ���
		void Object::addSubObject(Object *obj) {
			if (nullptr == obj)	// obj is tan90
				return;

			// �����Ƿ��Ѿ���ӹ��ò���
			for (auto each : _subObjects) if (obj == each) return;

			_subObjects.push_back(obj);

			sig_AddSubObject(this, obj);
		}

		void Object::show() {
			setVisible(true);
		}

		void Object::refresh() {
			Object::clearRect();
			show();
		}

		void Object::clearRect() {
			auto wid = boundingRect().width(),
				len = boundingRect().length();
			auto p1 = Map2Sense(this);
			char color = 0x00;

			for (auto i = 0; i < wid; i++) {
				tool::ClearConsole({ p1.x(), static_cast<short>(p1.y() + i) }, len, color);
			}
		}

	}

}