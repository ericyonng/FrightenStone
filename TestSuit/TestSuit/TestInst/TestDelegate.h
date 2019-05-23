#ifndef __Test_TestDelegate_H__
#define __Test_TestDelegate_H__

#pragma once
#include "stdafx.h"
#include "base/common/component/Impl/FS_ClassFactory.h"
#include "base/common/component/Impl/FS_Delegate.h"

template <class T, class R, typename... Args>
class  MyDelegate
{
public:
    MyDelegate(T* t, R(T::*f)(Args...)):m_t(t), m_f(f) {}

    R operator()(Args&&... args)
    {
        return (m_t->*m_f)(std::forward<Args>(args) ...);
    }

private:
    T* m_t;
    R(T::*m_f)(Args...);
};

template <class T, class R, typename... Args>
MyDelegate<T, R, Args...> CreateDelegate(T* t, R(T::*f)(Args...))
{
    return MyDelegate<T, R, Args...>(t, f);
}

struct A
{
    void Fun(int i) { std::cout << i << std::endl; }
    int Fun1(int &i, double &j) { std::cout << i + j << std::endl; return 0; }
};

struct bbs
{
    union
    {
        void *_voidPtr;
        std::map<int, int> *_mapPtr;
        double *_doublePtr;
    }_ptr;

    double _raw;
};

class TestDelegate
{
public:
    static void Run()
    {
        bbs ddn;
        memset(&ddn, 0, sizeof(ddn));
        ddn._ptr._voidPtr = new bbs;
        ddn._ptr._mapPtr->insert(std::make_pair(1, 1));
        std::map<int, int> a;
        std::cout << sizeof(a) << std::endl;
    }
};

#endif

