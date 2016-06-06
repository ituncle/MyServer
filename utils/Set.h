//
// Created by root on 2/26/16.
//

#ifndef YFQSERVER_SET_H
#define YFQSERVER_SET_H

#include <cassert>
#include <set>
#include "../network/muduo/base/Mutex.h"

namespace ft {
    using muduo::MutexLockGuard;
    /**
     * a thread safe set
     */
    template <typename _Key, typename _Compare = std::less<_Key>,
            typename _Alloc = std::allocator<_Key>>
    class Set {
    public:
        using key_type = _Key;
        using value_type = _Key;
        using size_type = size_t;
        using key_compare = _Compare;
        using allocator_type = _Alloc;
        typedef typename std::set<value_type>::iterator iterator;
        typedef typename std::set<value_type>::const_iterator const_iterator;
    private:
        mutable muduo::MutexLock mutex_;
        std::set<value_type> items_;

    public:
        explicit Set(const key_compare& comp = key_compare(), const allocator_type& alloc = allocator_type())
                : items_(comp, alloc) {}
        explicit Set(const allocator_type& alloc) : items_(alloc) {}

        template <class InputIterator>
        Set(InputIterator first, InputIterator last,
        const key_compare& comp = key_compare(),
        const allocator_type& alloc = allocator_type())
                : items_(first, last, comp, alloc) {}

        // TODO may not work well
        Set(const Set& x) {
            MutexLockGuard guard(x.mutex_);
            items_.insert(x.items_.cbegin(), x.items_.cend());
        }

//        Set(const Set& x, const allocator_type& alloc) {
//            MutexLockGuard guard(x.mutex_);
//            x.items_.
//        }

        // TODO may not work well
        Set(Set&& x) : items_(std::move(x)) {

        }

//        Set(Set&& x, const allocator_type& alloc) {
//
//        }

        Set(std::initializer_list<value_type> il,
        const key_compare& comp = key_compare(),
        const allocator_type& alloc = allocator_type())
                : items_(il, comp, alloc) {}

        Set& operator=(const Set& x) {
            MutexLockGuard guard1(mutex_);
            MutexLockGuard guard2(x.mutex_);
            return items_.operator=(x.items_);
        }

        Set& operator=(Set&& x) {
            MutexLockGuard guard1(mutex_);
            MutexLockGuard guard2(x.mutex_);
            return items_.operator=(std::move(x.items_));
        }

        Set& operator=(std::initializer_list<value_type> il) {
            MutexLockGuard guard(mutex_);
            return items_.operator=(il);
        }
        // TODO: copy-ctor, operator=, move-ctor, move-operator=, and other method as you need

        size_type size() const {
            MutexLockGuard guard(mutex_);
            return items_.size();
        }

        void clear() {
            MutexLockGuard guard(mutex_);
            items_.clear();
        }

        size_type count(const value_type& val) const {
            MutexLockGuard guard(mutex_);
            return items_.count(val);
        }

        bool empty() const {
            MutexLockGuard guard(mutex_);
            return items_.empty();
        }

        std::pair<const_iterator, const_iterator> equal_range(const value_type& val) const {
            MutexLockGuard guard(mutex_);
            return items_.equal_range(val);
        }

        std::pair<iterator, iterator> equal_range (const value_type& val) {
            MutexLockGuard guard(mutex_);
            return items_.equal_range(val);
        }

        iterator erase(const_iterator position) {
            MutexLockGuard guard(mutex_);
            return items_.erase(position);
        }

        size_type erase(const value_type& val) {
            MutexLockGuard guard(mutex_);
            return items_.erase(val);
        }

        iterator erase(const_iterator first, const_iterator last) {
            MutexLockGuard guard(mutex_);
            return items_.erase(first, last);
        }

        bool find(const value_type& val, const_iterator& iter) const {
            MutexLockGuard guard(mutex_);
            iter = items_.find(val);
            return !(iter == items_.end());
        }

        bool find(const value_type& val, iterator& iter) {
            MutexLockGuard guard(mutex_);
            iter = items_.find(val);
            return !(iter == items_.end());
        }

        bool find(const value_type& val) const {
            const_iterator iter;
            return find(val, iter);
        }

        bool find(const value_type& val) {
            iterator iter;
            return find(val, iter);
        }

        std::pair<iterator, bool> insert(const value_type& val) {
            MutexLockGuard guard(mutex_);
            return items_.insert(val);
        }

        std::pair<iterator, bool> insert(value_type&& val) {
            MutexLockGuard guard(mutex_);
            return items_.insert(val);
        }

        iterator insert(const_iterator position, const value_type& val) {
            MutexLockGuard guard(mutex_);
            return items_.insert(position, val);
        }

        iterator insert(const_iterator position, value_type&& val) {
            MutexLockGuard guard(mutex_);
            return items_.insert(position, val);
        }


        template <class InputIterator>
        void insert(InputIterator first, InputIterator last) {
            MutexLockGuard guard(mutex_);
            items_.insert(first, last);
        }

        void insert(std::initializer_list<value_type> il) {
            MutexLockGuard guard(mutex_);
            items_.insert(il);
        }
    };
}

#endif //YFQSERVER_SET_H
