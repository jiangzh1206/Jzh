/*�������� - ��������ģ��
����˼�壬�������� - ��������ģ����ֻ��һ�������ߺ�һ�������ߣ�
�����߲�ͣ������Ʒ���з����Ʒ��
��������Ӳ�Ʒ����ȡ�߲�Ʒ��
��Ʒ���ݻ������ƣ�ֻ������һ����Ŀ�Ĳ�Ʒ��
���������������Ʒ���ٶȹ��죬����Ҫ�ȴ�������ȡ�߲�Ʒ֮�󣬲�Ʒ�ⲻΪ�ղ��ܼ�������Ʒ���з����µĲ�Ʒ��
�෴�����������ȡ�߲�Ʒ���ٶȹ��죬��������ٲ�Ʒ����û�в�Ʒ��ʹ�õ��������ʱ��Ҫ�ȴ������߷���һ����Ʒ�������߲��ܼ�������
 */

#if 0

#include <iostream>  
using namespace std;
#include <thread>  
#include <vector>  
#include <mutex>  
#include <atomic>         // std::atomic  
#include <cstdlib>  
#include <condition_variable>  


static const int kItemRepositorySize = 10;  // Item buffer size.��������С  
static const int kItemsToProduce = 15;      // How many items we plan to produce.�ƻ�������Ʒ����  

struct ItemRepository {
    int item_buffer[kItemRepositorySize];   // ��Ʒ������, ��� read_position �� write_position ģ�ͻ��ζ���.  
    size_t read_position;                   // �����߶�ȡ��Ʒλ��.  
    size_t write_position;                  // ������д���Ʒλ��.  
    std::mutex mtx;                         // ������,������Ʒ������  
    std::condition_variable repo_not_full;  // ��������, ָʾ��Ʒ����������  
    std::condition_variable repo_not_empty; // ��������, ָʾ��Ʒ��������Ϊ��.  
} gItemRepository;                          // ��Ʒ��ȫ�ֱ���, �����ߺ������߲����ñ���.  

typedef struct ItemRepository ItemRepository;


void ProduceItem(ItemRepository *ir, int item)
{
    std::unique_lock<std::mutex> lock(ir->mtx);
    //while˵������Ϊ��������ѭ��ʹ�ã��������߼���ռ����һ��������������ʱ�����߻�û�ж�ȡ�û�������  
    //��Ҫ�ȴ������߶�ȡ���֪ͨ:repo_not_full����������  
    while (((ir->write_position + 1) % kItemRepositorySize) == ir->read_position)
    { 
        // item buffer is full, just wait here.  
        std::cout << "Producer is waiting for an empty slot...\n";
        (ir->repo_not_full).wait(lock); // �����ߵȴ�"��Ʒ�⻺������Ϊ��"��һ��������.  
    }

    (ir->item_buffer)[ir->write_position] = item; // д���Ʒ.  
    (ir->write_position)++; // д��λ�ú���.  

    if (ir->write_position == kItemRepositorySize) // д��λ�������ڶ����������������Ϊ��ʼλ��.  
        ir->write_position = 0;

    (ir->repo_not_empty).notify_all(); // ֪ͨ�����߲�Ʒ�⻺������Ϊ��.  
    lock.unlock(); // ����.  
}

int ConsumeItem(ItemRepository *ir)
{
    int data;
    std::unique_lock<std::mutex> lock(ir->mtx);
    // item buffer is empty, just wait here.  
    //while˵������Ϊ��������ѭ��ʹ�ã��������߼�����ȡ��һ������������ʱ�����߻�δ�ڸû�������Ӳ�Ʒ��  
    //�ȴ��������������֪ͨ,repo_not_empty����������  
    while (ir->write_position == ir->read_position)
    {
        std::cout << "Consumer is waiting for items...\n";
        (ir->repo_not_empty).wait(lock); // �����ߵȴ�"��Ʒ�⻺������Ϊ��"��һ��������.  
    }

    data = (ir->item_buffer)[ir->read_position]; // ��ȡĳһ��Ʒ  
    (ir->read_position)++; // ��ȡλ�ú���  

    if (ir->read_position >= kItemRepositorySize) // ��ȡλ�����Ƶ������������λ.  
        ir->read_position = 0;

    (ir->repo_not_full).notify_all(); // ֪ͨ�����߲�Ʒ�⻺����Ϊ��  
    lock.unlock(); // ����.  

    return data; // ���ز�Ʒ.  
}


void ProducerTask() // ����������  
{
    for (int i = 1; i <= kItemsToProduce; ++i)
    {
        // sleep(1);  
        //this_thread::sleep_for(chrono::seconds(1));  
        std::cout << "Produce the " << i << "^th item..." << std::endl;
        ProduceItem(&gItemRepository, i); // ѭ������ kItemsToProduce ����Ʒ.  
    }
}

void ConsumerTask() // ����������  
{
    static int cnt = 0;
    while (1)
    {
        this_thread::sleep_for(chrono::seconds(1));
        int item = ConsumeItem(&gItemRepository); // ����һ����Ʒ.  
        std::cout << "Consume the " << item << "^th item" << std::endl;
        if (++cnt == kItemsToProduce) break; // �����Ʒ���Ѹ���Ϊ kItemsToProduce, ���˳�.  
    }
}

void InitItemRepository(ItemRepository *ir)
{
    ir->write_position = 0; // ��ʼ����Ʒд��λ��.  
    ir->read_position = 0; // ��ʼ����Ʒ��ȡλ��.  
}

int main()
{
    InitItemRepository(&gItemRepository);
    std::thread producer(ProducerTask); // �����������߳�.  
    std::thread consumer(ConsumerTask); // ��������֮�߳�.  
    producer.join();
    consumer.join();
}

#endif