#ifndef __URI_ROUTER__
#define __URI_ROUTER__

#include <iostream>
#include <sstream>
#include <unordered_map>
#include <string>
#include <memory>
#include <list>

template <class T>
class uri_part
{
private:
    std::unordered_map<std::string,uri_part> next;
    std::shared_ptr<T> value = nullptr;
public:
    uri_part() {};
    ~uri_part() {};
    void insert(std::list<std::string>&& list,std::shared_ptr<T> value) {
        if (!list.empty()) {
            auto it = next.find(list.front());
            if (it == next.end()) {
                it = next.insert({list.front(),uri_part()}).first;
            }
            list.pop_front();
            it->second.insert(std::move(list),value);
        } else {
            this->value = value;
        }
    }
    std::shared_ptr<T> find(std::list<std::string>&& list) {
        if (!list.empty()) {
            auto it = next.find(list.front());
            if (it != next.end()) {
                list.pop_front();
                return it->second.find(std::move(list));
            } else {
                return nullptr;
            }
        } else {
            return value;
        }
    }
};

template <class T>
class uri_router
{
private:
    uri_part<T> part;
public:
    uri_router() {};
    ~uri_router() {};
    void insert(std::string path,std::shared_ptr<T> value) {
        std::string token;
        std::stringstream ss(path);
        std::list<std::string> list;
        while (std::getline(ss, token, '/')) {
            if (!token.empty()) {
                list.push_back(token);
            }
        }
        part.insert(std::move(list),value);
    }
    std::shared_ptr<T> find(std::string path) {
        std::string token;
        std::stringstream ss(path);
        std::list<std::string> list;
        while (std::getline(ss, token, '/')) {
            if (!token.empty()) {
                list.push_back(token);
            }
        }
        return part.find(std::move(list));
    }
};


#endif