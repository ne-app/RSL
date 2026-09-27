// -*- C++ -*-
/***************************************************************************
 *
 * _uniqueptr.h - definition of the class template unique_ptr
 *
 * This is an internal header file used to implement the C++ Standard
 * Library. It should never be #included directly by a program.
 *
 * $Id$
 *
 ***************************************************************************
 *
 * Licensed to the Apache Software  Foundation (ASF) under one or more
 * contributor  license agreements.  See  the NOTICE  file distributed
 * with  this  work  for  additional information  regarding  copyright
 * ownership.   The ASF  licenses this  file to  you under  the Apache
 * License, Version  2.0 (the  "License"); you may  not use  this file
 * except in  compliance with the License.   You may obtain  a copy of
 * the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the  License is distributed on an  "AS IS" BASIS,
 * WITHOUT  WARRANTIES OR CONDITIONS  OF ANY  KIND, either  express or
 * implied.   See  the License  for  the  specific language  governing
 * permissions and limitations under the License.
 *
 * Copyright 2026 Ne.app.
 *
 **************************************************************************/

#ifndef _RWSTD_RW_UNIQUEPTR_H_INCLUDED
#define _RWSTD_RW_UNIQUEPTR_H_INCLUDED

#ifndef _RWSTD_NULLPTR_T
#define _RWSTD_NULLPTR_T decltype(nullptr)
#endif

#ifndef _RWSTD_RW_DEFS_H_INCLUDED
#  include <rw/_defs.h>
#endif
#ifndef _RWSTD_RW_FORWARD_INCLUDED
#  include <rw/_forward.h>
#endif
#ifndef _RWSTD_RW_META_ARR_H_INCLUDED
#  include <rw/_meta_arr.h>
#endif
_RWSTD_NAMESPACE(__rw) {

    template <class _TypeT>
    struct __rw_unique_if;

    template <class _TypeT>
    struct __rw_unique_if<_TypeT[]>;

    template <class _TypeT, _RWSTD_SIZE_T _N>
    struct __rw_unique_if<_TypeT[_N]>;


}   // namespace __rw

_RWSTD_NAMESPACE (std) {

// 20.7.1 - class template default_delete

template <class _TypeT>
struct default_delete
{
    default_delete () _THROWS (()) { }

    template <class _TypeU>
    default_delete (const default_delete<_TypeU>&) _THROWS (()) { }

    void operator() (_TypeT* __p) const _THROWS (()) {
        delete __p;
    }
};

template <class _TypeT>
struct default_delete<_TypeT[]>
{
    void operator() (_TypeT* __p) const _THROWS (()) {
        delete[] __p;
    }
};


// 20.7.1.2 - class template unique_ptr for single objects

template <class _TypeT, class _Deleter = default_delete<_TypeT> >
class unique_ptr
{
public:
    typedef _TypeT   element_type;
    typedef _Deleter deleter_type;
    typedef _TypeT*  pointer;

    // 20.7.1.2.1, constructors

    unique_ptr () _THROWS (())
        : _C_ptr (0), _C_del () { }

#ifndef _RWSTD_NO_NULLPTR
    unique_ptr (_RWSTD_NULLPTR_T) _THROWS (())
        : _C_ptr (0), _C_del () { }
#endif

    explicit unique_ptr (pointer __p) _THROWS (())
        : _C_ptr (__p), _C_del () { }

    unique_ptr (pointer __p, const deleter_type& __d) _THROWS (())
        : _C_ptr (__p), _C_del (__d) { }

#ifndef _RWSTD_NO_RVALUE_REFERENCES

    unique_ptr (unique_ptr&& __rhs) _THROWS (())
        : _C_ptr (__rhs.release ()), _C_del (__rhs._C_del) { }

    template <class _TypeU, class _DeleterU>
    unique_ptr (unique_ptr<_TypeU, _DeleterU>&& __rhs) _THROWS (())
        : _C_ptr (__rhs.release ()), _C_del (__rhs.get_deleter ()) { }

#endif   // !_RWSTD_NO_RVALUE_REFERENCES

    // 20.7.1.2.2, destructor
    ~unique_ptr () _THROWS (()) {
        if (_C_ptr)
            _C_del (_C_ptr);
    }

    // 20.7.1.2.3, assignment

#ifndef _RWSTD_NO_RVALUE_REFERENCES

    unique_ptr& operator= (unique_ptr&& __rhs) _THROWS (()) {
        reset (__rhs.release ());
        _C_del = _RWSTD_MOVE (__rhs._C_del);
        return *this;
    }

    template <class _TypeU, class _DeleterU>
    unique_ptr& operator= (unique_ptr<_TypeU, _DeleterU>&& __rhs) _THROWS (()) {
        reset (__rhs.release ());
        _C_del = _RWSTD_MOVE (__rhs.get_deleter ());
        return *this;
    }

#endif   // !_RWSTD_NO_RVALUE_REFERENCES

    // 20.7.1.2.4, observers

    element_type& operator* () const _THROWS (()) {
        _RWSTD_ASSERT (0 != _C_ptr);
        return *_C_ptr;
    }

    pointer operator-> () const _THROWS (()) {
        _RWSTD_ASSERT (0 != _C_ptr);
        return _C_ptr;
    }

    pointer get () const _THROWS (()) {
        return _C_ptr;
    }

    deleter_type& get_deleter () _THROWS (()) {
        return _C_del;
    }

    const deleter_type& get_deleter () const _THROWS (()) {
        return _C_del;
    }

    operator bool () const _THROWS (()) {
        return 0 != _C_ptr;
    }

    // 20.7.1.2.5, modifiers

    pointer release () _THROWS (()) {
        pointer __tmp = _C_ptr;
        _C_ptr = 0;
        return __tmp;
    }

    void reset (pointer __p = 0) _THROWS (()) {
        if (_C_ptr != __p) {
            if (_C_ptr)
                _C_del (_C_ptr);
            _C_ptr = __p;
        }
    }

    void swap (unique_ptr& __other) _THROWS (()) {
        pointer      __tmp_ptr = _C_ptr;
        deleter_type __tmp_del = _C_del;
        _C_ptr = __other._C_ptr;
        _C_del = __other._C_del;
        __other._C_ptr = __tmp_ptr;
        __other._C_del = __tmp_del;
    }

private:
    unique_ptr (const unique_ptr&);
    unique_ptr& operator= (const unique_ptr&);

    pointer      _C_ptr;
    deleter_type _C_del;
};


// 20.7.1.3 - class template unique_ptr for arrays

template <class _TypeT, class _Deleter>
class unique_ptr<_TypeT[], _Deleter>
{
public:
    typedef _TypeT   element_type;
    typedef _Deleter deleter_type;
    typedef _TypeT*  pointer;

    // 20.7.1.3.1, constructors

    unique_ptr () _THROWS (())
        : _C_ptr (0), _C_del () { }

#ifndef _RWSTD_NO_NULLPTR
    unique_ptr (_RWSTD_NULLPTR_T) _THROWS (())
        : _C_ptr (0), _C_del () { }
#endif

    explicit unique_ptr (pointer __p) _THROWS (())
        : _C_ptr (__p), _C_del () { }

    unique_ptr (pointer __p, const deleter_type& __d) _THROWS (())
        : _C_ptr (__p), _C_del (__d) { }

#ifndef _RWSTD_NO_RVALUE_REFERENCES

    unique_ptr (unique_ptr&& __rhs) _THROWS (())
        : _C_ptr (__rhs.release ()), _C_del (__rhs._C_del) { }

#endif   // !_RWSTD_NO_RVALUE_REFERENCES

    // 20.7.1.3.2, destructor
    ~unique_ptr () _THROWS (()) {
        if (_C_ptr)
            _C_del (_C_ptr);
    }

    // 20.7.1.3.3, assignment

#ifndef _RWSTD_NO_RVALUE_REFERENCES

    unique_ptr& operator= (unique_ptr&& __rhs) _THROWS (()) {
        reset (__rhs.release ());
        _C_del = _RWSTD_MOVE (__rhs._C_del);
        return *this;
    }

#endif   // !_RWSTD_NO_RVALUE_REFERENCES

    // 20.7.1.3.4, observers

    element_type& operator[] (_RWSTD_SIZE_T __i) const _THROWS (()) {
        return _C_ptr[__i];
    }

    pointer get () const _THROWS (()) { return _C_ptr; }

    deleter_type& get_deleter () _THROWS (()) { return _C_del; }

    const deleter_type& get_deleter () const _THROWS (()) { return _C_del; }

    operator bool () const _THROWS (()) { return 0 != _C_ptr; }

    // 20.7.1.3.5, modifiers

    pointer release () _THROWS (()) {
        pointer __tmp = _C_ptr;
        _C_ptr = 0;
        return __tmp;
    }

    void reset (pointer __p = 0) _THROWS (()) {
        if (_C_ptr != __p) {
            if (_C_ptr)
                _C_del (_C_ptr);
            _C_ptr = __p;
        }
    }

    void swap (unique_ptr& __other) _THROWS (()) {
        pointer      __tmp_ptr = _C_ptr;
        deleter_type __tmp_del = _C_del;
        _C_ptr = __other._C_ptr;
        _C_del = __other._C_del;
        __other._C_ptr = __tmp_ptr;
        __other._C_del = __tmp_del;
    }

private:
    unique_ptr (const unique_ptr&);
    unique_ptr& operator= (const unique_ptr&);

    pointer      _C_ptr;
    deleter_type _C_del;
};


// 20.7.1.4 - unique_ptr specialized algorithms

template <class _TypeT, class _Deleter>
inline void
swap (unique_ptr<_TypeT, _Deleter>& __lhs,
      unique_ptr<_TypeT, _Deleter>& __rhs) _THROWS (())
{
    __lhs.swap (__rhs);
}


// 20.7.1.5 - unique_ptr comparisons

template <class _TypeT, class _D1, class _TypeU, class _D2>
inline bool
operator== (const unique_ptr<_TypeT, _D1>& __lhs,
            const unique_ptr<_TypeU, _D2>& __rhs)
{
    return __lhs.get () == __rhs.get ();
}

template <class _TypeT, class _D1, class _TypeU, class _D2>
inline bool
operator!= (const unique_ptr<_TypeT, _D1>& __lhs,
            const unique_ptr<_TypeU, _D2>& __rhs)
{
    return __lhs.get () != __rhs.get ();
}

template <class _TypeT, class _D1, class _TypeU, class _D2>
inline bool
operator< (const unique_ptr<_TypeT, _D1>& __lhs,
           const unique_ptr<_TypeU, _D2>& __rhs)
{
    return __lhs.get () < __rhs.get ();
}

template <class _TypeT, class _D1, class _TypeU, class _D2>
inline bool
operator<= (const unique_ptr<_TypeT, _D1>& __lhs,
            const unique_ptr<_TypeU, _D2>& __rhs)
{
    return !(__rhs < __lhs);
}

template <class _TypeT, class _D1, class _TypeU, class _D2>
inline bool
operator> (const unique_ptr<_TypeT, _D1>& __lhs,
           const unique_ptr<_TypeU, _D2>& __rhs)
{
    return __rhs < __lhs;
}

template <class _TypeT, class _D1, class _TypeU, class _D2>
inline bool
operator>= (const unique_ptr<_TypeT, _D1>& __lhs,
            const unique_ptr<_TypeU, _D2>& __rhs)
{
    return !(__lhs < __rhs);
}

#ifndef _RWSTD_NO_NULLPTR

template <class _TypeT, class _Deleter>
inline bool
operator== (const unique_ptr<_TypeT, _Deleter>& __p, _RWSTD_NULLPTR_T)
{
    return !__p;
}

template <class _TypeT, class _Deleter>
inline bool
operator== (_RWSTD_NULLPTR_T, const unique_ptr<_TypeT, _Deleter>& __p)
{
    return !__p;
}

template <class _TypeT, class _Deleter>
inline bool
operator!= (const unique_ptr<_TypeT, _Deleter>& __p, _RWSTD_NULLPTR_T)
{
    return static_cast<bool> (__p);
}

template <class _TypeT, class _Deleter>
inline bool
operator!= (_RWSTD_NULLPTR_T, const unique_ptr<_TypeT, _Deleter>& __p)
{
    return static_cast<bool> (__p);
}

template <class _TypeT, class _Deleter>
inline bool
operator< (const unique_ptr<_TypeT, _Deleter>& __p, _RWSTD_NULLPTR_T)
{
    typedef typename unique_ptr<_TypeT, _Deleter>::pointer _Ptr;
    return __p.get () < static_cast<_Ptr> (0);
}

template <class _TypeT, class _Deleter>
inline bool
operator< (_RWSTD_NULLPTR_T, const unique_ptr<_TypeT, _Deleter>& __p)
{
    typedef typename unique_ptr<_TypeT, _Deleter>::pointer _Ptr;
    return static_cast<_Ptr> (0) < __p.get ();
}

template <class _TypeT, class _Deleter>
inline bool
operator<= (const unique_ptr<_TypeT, _Deleter>& __p, _RWSTD_NULLPTR_T)
{
    return !(nullptr < __p);
}

template <class _TypeT, class _Deleter>
inline bool
operator<= (_RWSTD_NULLPTR_T, const unique_ptr<_TypeT, _Deleter>& __p)
{
    return !(__p < nullptr);
}

template <class _TypeT, class _Deleter>
inline bool
operator> (const unique_ptr<_TypeT, _Deleter>& __p, _RWSTD_NULLPTR_T)
{
    return nullptr < __p;
}

template <class _TypeT, class _Deleter>
inline bool
operator> (_RWSTD_NULLPTR_T, const unique_ptr<_TypeT, _Deleter>& __p)
{
    return __p < nullptr;
}

template <class _TypeT, class _Deleter>
inline bool
operator>= (const unique_ptr<_TypeT, _Deleter>& __p, _RWSTD_NULLPTR_T)
{
    return !(__p < nullptr);
}

template <class _TypeT, class _Deleter>
inline bool
operator>= (_RWSTD_NULLPTR_T, const unique_ptr<_TypeT, _Deleter>& __p)
{
    return !(nullptr < __p);
}

#endif   // !_RWSTD_NO_NULLPTR


// 20.7.1.6 - make_unique

#if !defined (_RWSTD_NO_RVALUE_REFERENCES) && \
    !defined (_RWSTD_NO_VARIADIC_TEMPLATES)

template <class _TypeT, class... _Args>
inline typename _RW::__rw_unique_if<_TypeT>::__single_object
make_unique (_Args&&... __args)
{
    return unique_ptr<_TypeT>(new _TypeT (_RWSTD_FORWARD (_Args, __args)...));
}

template <class _TypeT>
inline typename _RW::__rw_unique_if<_TypeT>::__unknown_bound
make_unique (_RWSTD_SIZE_T __n)
{
    typedef typename _RW::__rw_remove_extent<_TypeT>::type _TypeU;
    return unique_ptr<_TypeT>(new _TypeU[__n]);
}

#endif   // rvalue refs + variadic templates


}   // namespace std

_RWSTD_NAMESPACE(__rw) {

    template <class _TypeT>
    struct __rw_unique_if
    {
        typedef _STD::unique_ptr<_TypeT> __single_object;
    };

    template <class _TypeT>
    struct __rw_unique_if<_TypeT[]>
    {
        typedef _STD::unique_ptr<_TypeT[]> __unknown_bound;
    };

    template <class _TypeT, _RWSTD_SIZE_T _N>
    struct __rw_unique_if<_TypeT[_N]>
    {
    };


}   // namespace __rw

#endif   // _RWSTD_RW_UNIQUEPTR_H_INCLUDED
