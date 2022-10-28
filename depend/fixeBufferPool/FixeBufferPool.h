#ifndef FixeBufferPool_h
#define FixeBufferPool_h

/*
�������붨�����ڴ��
*/

#include <mutex>
#include <vector>
#include <atomic>	//ԭ�Ӳ���

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

		//poolSize��ÿ�������ڴ��Ĵ�С
		//poolNum������������
		bool createPool(int poolSize, int poolNum) {
			if (poolSize <= 0 || poolNum <= 0) return false;
			m_poolSize = poolSize;
			m_maxPoolNum = poolNum;
			return true;
		}

		void destoryPool() {	//��Ҫ�ȵ������ڴ�ȫ�����պ����
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
		std::atomic<int> m_activeNum;	//�ڴ�c�����������ڴ�����				
		int m_poolSize;	//�ڴ���С	//�ڴ��ÿ�������m_poolSize������ΪT���ڴ��
		int m_maxPoolNum;	//�ڴ���б���m_maxPoolNum��������ڴ��

		std::mutex m_poolMtx;
		std::vector<T *> m_pool;
	};
}

#endif // !FixeBufferPool_h
