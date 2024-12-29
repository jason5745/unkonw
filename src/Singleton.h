//
// Created by qq574 on 2024/12/28.
//

#ifndef _SINGLETON_H_
#define _SINGLETON_H_

#include <memory>
template<typename T>
class Singleton {
protected:
    Singleton() {}  // 私有构造函数，防止外部实例化
public:
    static T *getInstance() {
        static T instance;
        return &instance;
    }
    // 禁止拷贝构造函数和赋值运算符
    Singleton(const Singleton &) = delete;
    Singleton &operator=(const Singleton &) = delete;
};


#endif //_SINGLETON_H_
