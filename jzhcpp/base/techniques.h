#pragma once

/*
*   ģ��ƫ�ػ�
*   �ֲ���
*   �ͱ����ֵӳ��(Int2Type,Type2Int class templates)
*   ��ת��
*   NullType EmptyType
*   Traits
*/

// 0 array �Ƿ�
#define STATIC_CHECK(expr) { char unnamed[(expr) : 1 ? 0]; }

namespace MCPP
{
// 1.�ֲ���, ĳ���ӿ�ת��������ӿ�
class Interface
{
public:
    virtual void Fun() = 0;
};

template<typename T, typename U>
Interface* MakeAdapter(const T& obj, const U& arg)
{
    class Local : public Interface
    {
        T obj_;
        U arg_;
    public:
        Local(const T& obj, const U& arg) : obj_(obj), arg_(arg) { }
        void Fun() { obj.Call(arg); }
    };

    return new Local(obj, arg);
    std::cout << "local class" << "\n";
}

// 2.����ӳ�䵽���� �����ڷ��� Int2Type<0> Int2Type<1>��ͬ����
template<int V>
struct Int2Type
{
    enum { value = V };
};

// ����ӳ�䵽����
template<typename T>
struct Type2Type
{
    using OriginalType = T;
};

template<typename T, bool IS>
class NiftyContainer
{
    void DoSomething(T* obj, Int2Type<true>)
    {
        // obj do something
        std::cout << "do something true" << "\n";
    }

    // ����
    void DoSomething(T* obj, Int2Type<false>)
    {
        // obj do something
        std::cout << "do something false" << "\n";
    }

public:
    void DoSomething(T* obj)
    {
        DoSomething(obj, Int2Type<IS>());
    }
};

class TObject;
template<typename T, typename U>
T* Create(const U& arg, Type2Type<T>)
{
    return new T(arg);
}

// 3.�ػ�����ģ��
template<typename U>
TObject* Create(const U& arg, Type2Type<TObject>)
{
    return new TObject(arg);
}


// 4.From�Ƿ��ת����To
template<typename From, typename To>
struct Conversion
{
    using Small = char;
    struct Big 
    { 
        char dummy[2]; 
    };
    
    // ����ʵ��, sizeof ������������ֵ
    static Small Test(To);
    static Big Test(...);
    static From MakeT();
    
    // MakeT����From�Ƿ��ܵ���Test(To)
    enum{value = sizeof(Test(MakeT())) == sizeof(Small) };
    // ˫��ת��
    enum{con2way = value && Conversion<To, From>::value};
    // ��ͬ����
    enum { same = 0 };
};

// ��ͬ�����ػ�
template<typename T>
struct Conversion<T, T>
{
    enum { value = 1};
    enum { con2way = 1 };
    enum { same = 1 };
};

// 5.NullType �������ɶ���, EmptyType �ɱ��̳�,����Ϊtemplateȱʡ
class NullType; 
class EmptyType{};

}
