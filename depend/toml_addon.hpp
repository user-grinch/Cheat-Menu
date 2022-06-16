#pragma once
#include "toml.hpp"

template <typename T, typename Path>
auto build_from_path(T&& value, Path&& path_component)
{
	using component_type = std::remove_cv_t<std::remove_reference_t<Path>>;
	static_assert(std::is_integral_v<component_type> || toml::is_key_or_convertible<Path&&>,
				  "path components must be integers or strings");

	// making an array
	if constexpr (std::is_integral_v<component_type>)
	{
		toml::array arr;
		const auto index = static_cast<std::size_t>(path_component);
		arr.reserve(index + 1u);

		// backfill with integers
		while (arr.size() < index)
			arr.push_back(0);

		// add the actual value
		arr.push_back(static_cast<T&&>(value));

		return arr;
	}
	else // making a table
	{
		toml::table tbl;
		tbl.insert_or_assign(path_component, value);
		return tbl;
	}
}

template <typename T, typename Path, typename... Paths>
auto build_from_path(T&& value, Path&& path_component, Paths&&... path_components)
{
	static_assert(sizeof...(Paths));

	return build_from_path(build_from_path(value, path_components...), path_component);
}

static void merge_left(toml::table& lhs, toml::table&& rhs);

static void merge_left(toml::array& lhs, toml::array&& rhs)
{
	rhs.for_each(
		[&](std::size_t index, auto&& rhs_val)
		{
			// rhs index not found in lhs - direct move
			if (lhs.size() <= index)
			{
				lhs.push_back(std::move(rhs_val));
				return;
			}

			// both elements were the same container type -  recurse into them
			if constexpr (toml::is_container<decltype(rhs_val)>)
			{
				using rhs_type = std::remove_cv_t<std::remove_reference_t<decltype(rhs_val)>>;
				if (auto lhs_child = lhs[index].as<rhs_type>())
				{
					merge_left(*lhs_child, std::move(rhs_val));
					return;
				}
			}

			// replace lhs element with rhs
			lhs.replace(lhs.cbegin() + index, std::move(rhs_val));
		});
}

static void merge_left(toml::table& lhs, toml::table&& rhs)
{
	rhs.for_each(
		[&](const toml::key& rhs_key, auto&& rhs_val)
		{
			auto lhs_it = lhs.lower_bound(rhs_key);

			// rhs key not found in lhs - direct move
			if (lhs_it == lhs.cend() || lhs_it->first != rhs_key)
			{
				using rhs_type = std::remove_cv_t<std::remove_reference_t<decltype(rhs_val)>>;
				lhs.emplace_hint<rhs_type>(lhs_it, rhs_key, std::move(rhs_val));
				return;
			}

			// both children were the same container type -  recurse into them
			if constexpr (toml::is_container<decltype(rhs_val)>)
			{
				using rhs_type = std::remove_cv_t<std::remove_reference_t<decltype(rhs_val)>>;
				if (auto lhs_child = lhs_it->second.as<rhs_type>())
				{
					merge_left(*lhs_child, std::move(rhs_val));
					return;
				}
			}

			// replace lhs value with rhs
			lhs.insert_or_assign(rhs_key, std::move(rhs_val));
		});
}