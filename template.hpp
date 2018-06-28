#pragma once


#include <type_traits>

namespace dx9 {


	// 基底型にキャスト
	template<class T>
	auto underlying_cast(T a) {
		return static_cast<typename std::underlying_type<T>::type>(a);
	}

	template<
		class T1, class T2,
		typename std::enable_if_t<std::is_enum<T1>::value, std::nullptr_t> = nullptr,
		typename std::enable_if_t<std::is_enum<T2>::value, std::nullptr_t> = nullptr
	>
	auto operator| (T1 L, T2 R) {
		return underlying_cast(L)|underlying_cast(R);
	}
	

	template<
		class T1, class T2,
		typename std::enable_if_t<std::is_enum<T1>::value, std::nullptr_t> = nullptr,
		std::enable_if_t<std::is_enum<T2>::value, std::nullptr_t> = nullptr
	>
	auto operator& (T1 L, T2 R) {
		return underlying_cast(L)&underlying_cast(R);
	}









}



