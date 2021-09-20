#pragma once

#include <cxxabi.h>

#include <iostream>
#include <memory>
#include <string>
#include <typeinfo>
#include <unordered_map>

#include "Node.h"

namespace algo {
namespace vision {
template <typename... Targs>
class NodeFactory {
 public:
    static NodeFactory* Instance() {
        if (nullptr == node_Factory_) {
            node_Factory_ = new NodeFactory();
        }
        return (node_Factory_);
    }

    virtual ~NodeFactory(){};

    bool Regist(const std::string& strTypeName,
                std::function<Node*(Targs&&... args)> pFunc) {
        if (nullptr == pFunc) {
            return (false);
        }
        std::string strRealTypeName = strTypeName;

        bool bReg =
            create_func_map_.insert(std::make_pair(strRealTypeName, pFunc))
                .second;
        return (bReg);
    }

    Node* Create(const std::string& strTypeName, Targs&&... args) {
        auto iter = create_func_map_.find(strTypeName);
        if (iter == create_func_map_.end()) {
            return (nullptr);
        } else {
            return (iter->second(std::forward<Targs>(args)...));
        }
    }

 private:
    NodeFactory(){};
    static NodeFactory<Targs...>* node_Factory_;
    std::unordered_map<std::string, std::function<Node*(Targs&&...)> >
        create_func_map_;
};

template <typename... Targs>
NodeFactory<Targs...>* NodeFactory<Targs...>::node_Factory_ =
    nullptr;

template <typename T, typename... Targs>
class DynamicCreator {
 public:
    struct Register {
        Register() {
            char* szDemangleName = nullptr;
            std::string strTypeName;
#ifdef __GNUC__
            szDemangleName = abi::__cxa_demangle(typeid(T).name(), nullptr,
                                                 nullptr, nullptr);
#else
            seDemangleName = typeid(T).name();
#endif
            if (nullptr != szDemangleName) {
                strTypeName = szDemangleName;
                free(szDemangleName);
            }

            NodeFactory<Targs...>::Instance()->Regist(strTypeName,
                                                           CreateObject);
        }

        inline void doNothing() const {};
    };

    DynamicCreator() { register_.doNothing(); }
    virtual ~DynamicCreator() { register_.doNothing(); };

    static T* CreateObject(Targs&&... args) {
        return new T(std::forward<Targs>(args)...);
    }

    static Register register_;
};

template <typename T, typename... Targs>
typename DynamicCreator<T, Targs...>::Register
    DynamicCreator<T, Targs...>::register_;

class NodeBuild {
 public:
    template <typename... Targs>
    static Node* CreateNode(const std::string& strTypeName,
                                      Targs&&... args) {
        Node* p = NodeFactory<Targs...>::Instance()->Create(
            strTypeName, std::forward<Targs>(args)...);
        return (p);
    }

    template <typename... Targs>
    static std::shared_ptr<Node> MakeSharedNode(
        const std::string& strTypeName, Targs&&... args) {
        std::shared_ptr<Node> ptr(
            CreateNode(strTypeName, std::forward<Targs>(args)...));
        return ptr;
    }
};
}  // namespace vision
}  // namespace algo