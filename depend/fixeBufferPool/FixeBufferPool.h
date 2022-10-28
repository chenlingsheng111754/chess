#ifndef FixeBufferPool_h
#define FixeBufferPool_h

/*
用于申请定长的内存池
*/

#include <mutex>
#include <vector>
#include <atomic>	//原子操作

namespace CLS_FixeBufferPool
{	
	template <class T>
	class FixeBufferPool {
	public:
		FixeBufferPool() {
			m_poolSize = sizeof(T);
			m_maxPoolNum = 1;
			m_activeNum = 0;
		}

		~FixeBufferPool() {
			destoryPool();
		}

		//poolSize：每次申请内存块的大小
		//poolNum：最大申请块数
		bool createPool(int poolSize, int poolNum) {
			if (poolSize <= 0 || poolNum <= 0) return false;
			m_poolSize = poolSize;
			m_maxPoolNum = poolNum;
			return true;
		}

		void destoryPool() {	//需要等到在外内存全部回收后调用
			std::lock_guard<std::mutex> l(m_poolMtx);
			for (auto it = m_pool.begin(); it != m_pool.end(); ) {
				T* p = *it;				
				delete[] p;
				it = m_pool.erase(it);
			}
		}

		std::shared_ptr<T> alloc() {
			T *ptr = nullptr;
			std::shared_ptr<T> tmp;
			if (m_activeNum < m_maxPoolNum) {
				std::lock_guard<std::mutex> l(m_poolMtx);				
				if (m_pool.size() > 0) {					
					auto it = m_pool.begin();
					ptr = *it;					
					m_pool.erase(it);
				}
				else {
					ptr = new T[m_poolSize];					
					m_activeNum++;
				}
				tmp = std::shared_ptr<T>(ptr, [&](T* ptr) {
					m_activeNum--;
					std::lock_guard<std::mutex> l(m_poolMtx);
					m_pool.push_back(ptr);
					});
			}
			return tmp;						
		}

	public:
		std::atomic<int> m_activeNum;	//内存c池中在外活动的内存数量				
		int m_poolSize;	//内存块大小	//内存池每次申请的m_poolSize个类型为T的内存块
		int m_maxPoolNum;	//内存池中保留m_maxPoolNum个申请的内存块

		std::mutex m_poolMtx;
		std::vector<T *> m_pool;
	};
}

#endif // !FixeBufferPool_h
