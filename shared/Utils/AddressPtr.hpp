#pragma once

template<class T, uintptr_t address>
class AddressPtr {
public:
	friend T;
	constexpr AddressPtr() {};

#pragma region "Accessors"
	constexpr inline T& Get() const {
		return *reinterpret_cast<T*>(address);
	}

	template <typename U>
	constexpr inline U& GetAs() const {
		return *reinterpret_cast<U*>(address);
	}

	constexpr inline T* const operator->() const {
		return reinterpret_cast<T*>(address);
	}

	constexpr inline operator T() const {
		return Get();
	}
#pragma endregion

#pragma region "Modify operators"
	constexpr inline void operator=(const T other) const
		requires requires(T a, T b) { a = b; } {
		Get() = other;
	}

	template <typename U>
	constexpr inline void operator=(const U other) const
		requires requires(T a, U b) { a = b; } {
		Get() = other;
	}

	template <typename U>
	constexpr inline T operator+(const U& other) const
		requires requires(T a, U b) { a + b; } {
		return Get() + other;
	}

	template <typename U>
	constexpr inline T operator-(const U& other) const
		requires requires(T a, U b) { a - b; } {
		return Get() - other;
	}

	template <typename U>
	constexpr inline T operator*(const U& other) const
		requires requires(T a, U b) { a* b; } {
		return Get() * other;
	}

	template <typename U>
	constexpr inline T operator/(const U& other) const
		requires requires(T a, U b) { a / b; } {
		return Get() / other;
	}

	template <typename U>
	constexpr inline void operator+=(const U& other) const
		requires requires(T a, U b) { a += b; } {
		Get() += other;
	}

	template <typename U>
	constexpr inline void operator-=(const U& other) const
		requires requires(T a, U b) { a - +b; } {
		Get() -= other;
	}

	template <typename U>
	constexpr inline void operator*=(const U& other) const
		requires requires(T a, U b) { a *= b; } {
		Get() *= other;
	}

	template <typename U>
	constexpr inline void operator/=(const U& other) const
		requires requires(T a, U b) { a /= b; } {
		Get() /= other;
	}

	constexpr inline T operator++(int) const
		requires requires(T a) { a++; } {
		T temp = Get();
		Get()++;
		return temp;
	}

	constexpr inline T& operator++() const
		requires requires(T a) { ++a; } {
		return ++Get();
	}

	constexpr inline T operator--(int) const
		requires requires(T a) { a--; } {
		T temp = Get();
		Get()--;
		return temp;
	}


	constexpr inline T& operator--() const
		requires requires(T a) { --a; } {
		return --Get();
	}
#pragma endregion

#pragma region "Comparison operators"
	template <typename U>
		requires requires(T a, U b) { a == b; }
	constexpr inline bool operator==(const U& other) const {
		return Get() == other;
	}

	template <typename U>
		requires requires(T a, U b) { a != b; }
	constexpr inline bool operator!=(const U& other) const {
		return Get() != other;
	}

	template <typename U>
		requires requires(T a, U b) { a < b; }
	constexpr inline bool operator<(const U& other) const {
		return Get() < other;
	}

	template <typename U>
		requires requires(T a, U b) { a <= b; }
	constexpr inline bool operator<=(const U& other) const {
		return Get() <= other;
	}

	template <typename U>
		requires requires(T a, U b) { a > b; }
	constexpr inline bool operator>(const U& other) const {
		return Get() > other;
	}

	template <typename U>
		requires requires(T a, U b) { a >= b; }
	constexpr inline bool operator>=(const U& other) const {
		return Get() >= other;
	}
#pragma endregion
};