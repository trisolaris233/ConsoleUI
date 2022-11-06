#pragma once

#include <ctime>
#include <random>
#include <vector>
#include <cstdlib>
#include <functional>

namespace sweet {
	// 流的基类
	template < class ReturnType >
	struct StreamBase {
		virtual ~StreamBase() = default;
		virtual ReturnType get() = 0;
		virtual ReturnType ex()  = 0;
	};

	template < class ReturnType >
	class Stream : public StreamBase<ReturnType> {
	private:
		typedef std::function<ReturnType(void)>		wrapType;
		wrapType _wrap;

	public:
		template < class... Args >
		Stream(ReturnType(*f)(Args...), Args&&... args)
			: _wrap(std::bind(f, std::forward<Args>(args)...)) {}
		template < class... Args >
		Stream(std::function<ReturnType(Args...)> f, Args&&... args)
			: _wrap(std::bind(f, std::forward<Args>(args)...)) {}
		Stream(wrapType fun)
			: _wrap(fun) {}
		virtual ~Stream() = default;

		virtual ReturnType get() { return _wrap(); }
		virtual ReturnType ex()  { return get(); }
		template < class... Args >
		void reset(std::function<ReturnType(Args...)> f, Args&&... args) {
			_wrap = std::bind(f, std::forward<Args>(args)...);
		}
	};

	template < class ReturnType >
	class LoopStream : public StreamBase<ReturnType> {
	private:
		typedef std::function<ReturnType(void)>		wrapType;
		wrapType _wrap;

		struct LoopCallBk {
			ReturnType operator()(ReturnType arg) { return arg; }
		} def_callback;

	public:
		LoopStream(ReturnType arg)
			: _wrap(std::bind(def_callback, arg)) {}
		virtual ~LoopStream() = default;

		virtual ReturnType get() { return _wrap(); }
		virtual ReturnType ex()  { return get();  }
	};

	template < class ReturnType >
	class RandomStream : public StreamBase<ReturnType> {
		friend class RandomCallBk;
	private:
		std::vector<ReturnType>					_hold;
		static std::default_random_engine		_e;

	public:
		RandomStream(ReturnType lo, ReturnType hi) {
			while (lo < hi)
				_hold.push_back(lo++);
		}
		virtual ~RandomStream() = default;

		virtual ReturnType get() {
			std::uniform_int_distribution<> n(0, _hold.size());
			return _hold[n(_e)];
		}
		virtual ReturnType ex() {
			return get();
		}
	};

	template < class ReturnType >
	class RangeStream : public StreamBase<ReturnType> {
	private:
		std::vector<ReturnType> _list;
		std::size_t				_cursor;

	public:
		RangeStream(std::initializer_list<ReturnType>&& init)
			: _list(init.begin(), init.end())
			, _cursor(0) {}
		virtual ~RangeStream() = default;

		virtual ReturnType get() {
			_cursor = _cursor++ % _list.size();
			return _list[_cursor];
		}
		virtual ReturnType ex() {
			return _list[_cursor];
		}
	
	};

	template <typename T> std::default_random_engine RandomStream<T>::_e(static_cast<unsigned int>(time(nullptr)));

}