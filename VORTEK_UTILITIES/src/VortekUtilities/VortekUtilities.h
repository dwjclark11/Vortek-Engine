#pragma once
#include <vector>
#include <algorithm>
#include <ranges>
#include <functional>
#include <concepts>
#include <set>
#include <map>
#include <unordered_map>

namespace VORTEK_UTIL
{

	enum class AssetType
	{
		TEXTURE = 0,
		FONT,
		SOUNDFX,
		MUSIC,
		SCENE,

		NO_TYPE
	};

	/* Ensure the types that are passed in are associative map types. */
	template <typename T>
	concept MapType = std::same_as<T, std::map<typename T::key_type, typename T::mapped_type, typename T::key_compare,
		typename T::allocator_type>> ||
		std::same_as<T, std::unordered_map<typename T::key_type, typename T::mapped_type, typename T::hasher,
		typename T::key_equal, typename T::allocator_type>>;

	template <MapType Map>
	std::vector<typename Map::key_type> GetKeys(const Map& map)
	{
		auto keyView = std::views::keys(map);
		std::vector<typename Map::key_type> keys{ keyView.begin(), keyView.end() };
		return keys;
	}

	template <MapType Map, typename Func>
	std::vector<typename Map::key_type> GetKeys(const Map& map, Func func)
	{
		auto keyView = map | std::views::filter(func) | std::views::keys;
		std::vector<typename Map::key_type> keys{ keyView.begin(), keyView.end() };
		return keys;
	}

	template <MapType Map>
	bool KeyChange(Map& map, const typename Map::key_type& key, const typename Map::key_type& change)
	{
		if (!map.contains(key) || map.contains(change))
		{
			return false;
		}

		auto node = map.extract(key);
		node.key() = change;
		const auto [itr, bSuccess, nType] = map.insert(std::move(node));
		return bSuccess;
	}

	template <typename TCont, typename TFunc>
	bool CheckContainsValue(TCont& cont, TFunc func)
	{
		auto itr = std::ranges::find_if(cont, func);
		return itr == cont.end();
	}

	template <typename T>
	concept UintType = requires(T param) { requires std::is_integral_v<T>&& std::is_unsigned_v<T>; };

	template <UintType T>
	T SetBit(T num, int pos)
	{
		assert(pos < std::numeric_limits<T>::digits && "pos must be less than the number of digits.");
		T mask = 1 << pos;
		return num | mask;
	}

	template <UintType T>
	T GetFirstSetBit(T num)
	{
		int position{ 1 }, i{ 1 };
		while (!(i & num))
		{
			i <<= 1;
			++position;
		}

		return position;
	}

	template <UintType T>
	T Bit(const T& x)
	{
		return T(1) << x;
	}

	template <UintType T>
	std::set<T> GetAllSetBits(T num)
	{
		std::set<T> setBits{};
		int position{ 0 };
		while (num)
		{
			if (num & 1)
				setBits.insert(position);

			++position;
			num >>= 1;
		}

		return setBits;
	}

	template <UintType T>
	void ResetBit(T& num, int bit)
	{
		num &= ~(1 << bit);
	}

	template <UintType T>
	bool IsBitSet(const T& x, const T& y)
	{
		return 0 != (x & y);
	}

} // namespace VORTEK_UTIL