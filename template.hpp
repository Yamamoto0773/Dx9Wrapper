#pragma once


#include <type_traits>

namespace dx9 {


	// 基底型にキャスト
	template<class T> std::underlying_type<T> undering_cast(T a) {
		return static_cast<std::underlying_type<T>>(a);
	}


	template<class T> int operator |(T L, T R) {
		return undering_cast(L)|undering_cast(R);
	}
	template<class T> int operator &(T L, T R) {
		return undering_cast(L)|undering_cast(R);
	}


}



