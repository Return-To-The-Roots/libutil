// Copyright (c) 2005 - 2015 Settlers Freaks (sf-team at siedler25.org)
//
// This file is part of Return To The Roots.
//
// Return To The Roots is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 2 of the License, or
// (at your option) any later version.
//
// Return To The Roots is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Return To The Roots. If not, see <http://www.gnu.org/licenses/>.

#include <stdexcept>
#include <iterator>
#include <algorithm>

/// A (almost) vector compatible container that uses uninitialized memory
template<typename Tp>
class uvector
{
    typedef Tp& Ref;
    typedef const Tp& ConstRef;
    typedef Tp* Ptr;
    typedef const Tp* ConstPtr;
    /// Iterator type
    typedef Tp* iterator;
    /// Iterator type of constant elements
    typedef const Tp* const_iterator;
    /// Reverse iterator type
    typedef std::reverse_iterator<iterator> reverse_iterator;
    /// Reverse iterator of constant elements
    typedef std::reverse_iterator<const_iterator> const_reverse_iterator;
    /// Difference between to iterators
    typedef std::ptrdiff_t difference_type;
    /// Type used for indexing elements
    typedef std::size_t size_t;
    /// Type used for indexing elements
    typedef std::size_t size_type;

    Ptr data_;
    size_t size_, capacity_;

    void check_bounds(size_t index) const
    {
        if(index >= size())
            throw std::out_of_range("Access to element past end");
    }
public:
    uvector(): data_(NULL), size_(0), capacity_(0){}
    uvector(size_t size): data_(NULL), size_(0), capacity_(0)
    {
        resize(size);
    }

    ~uvector()
    {
        delete[] data_;
        data_ = NULL;
    }

    uvector(const uvector& obj): data_(0), size_(0), capacity_(0)
    {
        resize(obj.size());
        std::copy(obj.data_, obj.data_ + obj.size_, data_);
    }

    uvector& operator=(const uvector& obj)
    {
        if(this == &obj)
            return *this;

        resize(obj.size());
        std::copy(obj.data_, obj.data_ + obj.size_, data_);

        return *this;
    }

    size_t size() const { return size_; }
    size_t capacity() const { return capacity_; }

    void resize(size_t size)
    {
        reserve(size);
        size_ = size;
    }

    void reserve(size_t size)
    {
        if(size <= capacity_)
            return;
        size_t newSize = 8;
        while(newSize < size) newSize*=2;
        Ptr newData = new Tp[newSize];
        if(size_)
            std::copy(data_, data_ + size_, newData);
        delete[] data_;
        data_ = newData;
        capacity_ = newSize;
    }

    void clear()
    {
        size_ = 0;
        if(capacity_ > 64)
        {
            capacity_ = 0;
            delete[] data_;
            data_ = NULL;
        }
    }

    Ref operator[](size_t idx){ assert(idx < size_); return data_[idx]; }
    ConstRef operator[](size_t idx) const { assert(idx < size_); return data_[idx]; }

    Ref at(size_t idx)
    {
        check_bounds(idx);
        return data_[idx];
    }
    ConstRef at(size_t idx) const
    {
        check_bounds(idx);
        return data_[idx];
    }

    Ptr data(){ return data_; }
    ConstPtr data() const {return data_; }

    iterator begin(){ return iterator(data_); }
    iterator end(){ return iterator(data_ + size()); }
    const_iterator begin() const { return const_iterator(data_); }
    const_iterator end() const { return const_iterator(data_ + size()); }
};
