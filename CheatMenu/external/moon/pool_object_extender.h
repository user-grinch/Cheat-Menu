// This source is taken from MoonAdditions https://github.com/THE-FYP/MoonAdditions
// MIT License

// Copyright (c) 2012 DK22Pac
// Copyright (c) 2017 FYP

// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:

// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.

// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#pragma once
#include <unordered_map>

template<typename ObjT, typename DataT>
class ExtendedObjectDataPool
{
public:
	auto begin()
	{
		return _pool.begin();
	}

	auto end()
	{
		return _pool.end();
	}

	bool exists(const ObjT* obj) const
	{
		return _pool.find(obj) != _pool.end();
	}

	DataT& get(const ObjT* obj)
	{
		auto& it = _pool.find(obj);
		if (it != _pool.end())
			return *it->second;
		return *(_pool[obj] = std::make_unique<DataT>(obj));
	}

	void remove(const ObjT* obj)
	{
		_pool.erase(obj);
	}

private:
	std::unordered_map<const ObjT*, std::unique_ptr<DataT>> _pool;
};

template<typename T>
class VehicleDataExtended : public ExtendedObjectDataPool<CVehicle, T>
{
public:
	VehicleDataExtended()
	{
		plugin::Events::vehicleDtorEvent.after += [this](CVehicle* veh) {
			this->remove(veh);
		};
	}
};
