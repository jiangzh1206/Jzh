#pragma once
#include <cstdlib>
#include <iostream>

namespace MCPP {

// policy class
template<typename T>
struct NewCreator 
{
    static T* Create() { return new T; }

protected:
    // ����delete policy����(��ȥдvirtual����)
    ~NewCreator() {}
};

template<typename T>
struct MallocCreateor 
{
    static T* Create() 
    {
        void* p = std::malloc(sizeof(T));
        if (!p)
            return nullptr;
        return new(p) T;
    }

};

template<typename T>
struct PrototypeCreator 
{
    PrototypeCreator(T* obj) : m_prototype(obj) {}

    T* Create() 
    {
        return m_prototype ? m_prototype->Clone() : nullptr;
    }

    T* GetProtoType() { return m_prototype; }
    void SetProtoType(T* obj) { m_prototype = obj; }
private:
    T* m_prototype;
};

// ����policy class Ӧ���������ֽ�(�޽�������)
// host class 
// host class �̳� policy class, ����delete host class����ת�͵�policy class
template<typename T, template<typename> class Policy = NewCreator>
class ObjectManager : public Policy<T> 
{
public:
    T* Create()
    {
        return Policy<T>::Create();
    }
};


}; // MCPP