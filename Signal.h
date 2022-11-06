#pragma once

#include <vector>
#include <memory>
#include <functional>

#define signals public
#define slots	public

namespace sweet {

	
	template<class Return, class Type, class... Args>
	std::function<Return(Args...)> bind_member(Type* instance, Return(Type::*method)(Args...))
	{
		/* 匿名函数 */
		return[=](Args&&... args) -> Return
		{
			/* 完美转发：能过将参数按原来的类型转发到另一个函数中 */
			/* 通过完美转发将参数传递给被调用的函数 */
			return (instance->*method)(std::forward<Args>(args)...);
		};
	}


	template < class SlotImplType >
	struct SignalImpl {
		// 与此信号相连的槽的指针
		std::vector<std::weak_ptr<SlotImplType>> _slots;
	};

	class SlotImpl {
	public:
		SlotImpl() {}

		virtual ~SlotImpl() {}

		SlotImpl(const SlotImpl&) = delete;
		SlotImpl& operator=(const SlotImpl&) = delete;
	};

	template <class FuncType>
	class SlotImplT : public SlotImpl {
	public:
		SlotImplT(const std::weak_ptr<SignalImpl<SlotImplT>>& signal, const std::function<FuncType>& callback)
			: signal(signal)
			, callback(callback) {
		}

		~SlotImplT() {
			std::shared_ptr<SignalImpl<SlotImplT>> sig = signal.lock();
			if (sig == nullptr) return;

			// 遍历这个与这个信号相连接的所有槽
			for (auto it = sig->_slots.begin(); it != sig->_slots.end(); ++it) {
				// 如果槽已经降维了， 删除之
				// 或者那个槽是自己， 而自己即将被降维
				if (it->expired() || it->lock().get() == this) {
					it = sig->_slots.erase(it);
					if (it == sig->_slots.end()) {
						break;
					}
				}
			}
		}

		// 与这个槽连接的信号
		std::weak_ptr<SignalImpl<SlotImplT>>	signal;
		// 回调函数
		std::function<FuncType>					callback;
	};

	// 槽
	class Slot
	{
	public:
		Slot() {}

		~Slot() {}

		template<class T>
		explicit Slot(T impl) : impl(impl) {}

		operator bool() const
		{
			return static_cast< bool >(impl);
		}

	private:
		std::shared_ptr<SlotImpl> impl;	// 指向一个槽实例
	};


	/*
	信号类
	*/
	template<class FuncType>
	class Signal
	{
	public:
		Signal() : impl(std::make_shared<SignalImpl<SlotImplT<FuncType>>>()) {}

		template<class... Args>
		void operator()(Args&&... args)		// 槽触发
		{
			std::vector<std::weak_ptr<SlotImplT<FuncType>>> slotVector = impl->_slots;
			for (std::weak_ptr<SlotImplT<FuncType>>& weak_slot : slotVector)	// 遍历所有与之链接的槽
			{
				std::shared_ptr<SlotImplT<FuncType>> slot = weak_slot.lock();	// 取得槽实例
				if (slot) {
					// 调用槽的回调函数, 参数使用完美转发
					slot->callback(std::forward<Args>(args)...);
				}
			}
		}

		Slot connect(const std::function<FuncType>& func)	// 链接一个函数， 返回一个槽.
		{
			// 传入信号源和回调函数来创建一个槽
			std::shared_ptr<SlotImplT<FuncType>> slotImpl = std::make_shared<SlotImplT<FuncType>>(impl, func);

			/* 由于 SignalImpl 使用的是 std::weak_ptr，push_back 操作不会增加引用计数。
			因此，如果调用函数 connect 后的返回值没有赋值给 Slot 对象，过了这个函数的
			作用域 slotImpl 对象就会被释放掉 */
			impl->_slots.push_back(slotImpl);

			return Slot(slotImpl);
		}

		// 铜理， 绑定某个对象的成员函数
		template<class InstanceType, class MemberFuncType>
		Slot connect(InstanceType instance, MemberFuncType func)
		{
			return connect(bind_member(instance, func));
		}

	private:
		std::shared_ptr<SignalImpl<SlotImplT<FuncType>>> impl;
	};
}