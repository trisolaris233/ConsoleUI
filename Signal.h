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
		/* �������� */
		return[=](Args&&... args) -> Return
		{
			/* ����ת�����ܹ���������ԭ��������ת������һ�������� */
			/* ͨ������ת�����������ݸ������õĺ��� */
			return (instance->*method)(std::forward<Args>(args)...);
		};
	}


	template < class SlotImplType >
	struct SignalImpl {
		// ����ź������Ĳ۵�ָ��
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

			// �������������ź������ӵ����в�
			for (auto it = sig->_slots.begin(); it != sig->_slots.end(); ++it) {
				// ������Ѿ���ά�ˣ� ɾ��֮
				// �����Ǹ������Լ��� ���Լ���������ά
				if (it->expired() || it->lock().get() == this) {
					it = sig->_slots.erase(it);
					if (it == sig->_slots.end()) {
						break;
					}
				}
			}
		}

		// ����������ӵ��ź�
		std::weak_ptr<SignalImpl<SlotImplT>>	signal;
		// �ص�����
		std::function<FuncType>					callback;
	};

	// ��
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
		std::shared_ptr<SlotImpl> impl;	// ָ��һ����ʵ��
	};


	/*
	�ź���
	*/
	template<class FuncType>
	class Signal
	{
	public:
		Signal() : impl(std::make_shared<SignalImpl<SlotImplT<FuncType>>>()) {}

		template<class... Args>
		void operator()(Args&&... args)		// �۴���
		{
			std::vector<std::weak_ptr<SlotImplT<FuncType>>> slotVector = impl->_slots;
			for (std::weak_ptr<SlotImplT<FuncType>>& weak_slot : slotVector)	// ����������֮���ӵĲ�
			{
				std::shared_ptr<SlotImplT<FuncType>> slot = weak_slot.lock();	// ȡ�ò�ʵ��
				if (slot) {
					// ���ò۵Ļص�����, ����ʹ������ת��
					slot->callback(std::forward<Args>(args)...);
				}
			}
		}

		Slot connect(const std::function<FuncType>& func)	// ����һ�������� ����һ����.
		{
			// �����ź�Դ�ͻص�����������һ����
			std::shared_ptr<SlotImplT<FuncType>> slotImpl = std::make_shared<SlotImplT<FuncType>>(impl, func);

			/* ���� SignalImpl ʹ�õ��� std::weak_ptr��push_back ���������������ü�����
			��ˣ�������ú��� connect ��ķ���ֵû�и�ֵ�� Slot ���󣬹������������
			������ slotImpl ����ͻᱻ�ͷŵ� */
			impl->_slots.push_back(slotImpl);

			return Slot(slotImpl);
		}

		// ͭ�� ��ĳ������ĳ�Ա����
		template<class InstanceType, class MemberFuncType>
		Slot connect(InstanceType instance, MemberFuncType func)
		{
			return connect(bind_member(instance, func));
		}

	private:
		std::shared_ptr<SignalImpl<SlotImplT<FuncType>>> impl;
	};
}