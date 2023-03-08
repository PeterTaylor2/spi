#ifndef SPI_UTIL_LRU_CACHE_HPP
#define SPI_UTIL_LRU_CACHE_HPP

#include "RuntimeError.hpp"

#include <map>
#include <list>
#include <vector>

SPI_UTIL_NAMESPACE

template<typename K, typename V>
class LRUCache
{
public:
    typedef typename std::pair<K, V> list_element_type;
    typedef typename std::list<list_element_type>::iterator map_element_value_type;
    typedef typename std::pair<K, map_element_value_type> map_element_type;
    typedef typename std::map<K, map_element_value_type>::iterator map_iterator_type;

    LRUCache(size_t size)
        :
        m_size(size)
    {
        SPI_UTIL_PRE_CONDITION(size > 0);
    }

    bool find(const K& key, V& value)
    {
        map_iterator_type iter = m_map.find(key);

        // not in the cache
        if (iter == m_map.end())
            return false;

        // in the cache - we want to move it to the front of the queue
        value = iter->second->second;
        m_list.erase(iter->second);
        m_list.push_front(list_element_type(key, value));
        iter->second = m_list.begin();

        return true;
    }

    void insert(const K& key, const V& value)
    {
        map_iterator_type iter = m_map.find(key);

        // not in the cache - we may need to remove the last entry
        if (iter == m_map.end())
        {
            if (m_map.size() == m_size)
            {
                SPI_UTIL_PRE_CONDITION(m_list.size() == m_size);
                list_element_type lru = m_list.back();
                m_map.erase(lru.first);
                m_list.pop_back();
                SPI_UTIL_POST_CONDITION(m_map.size() == m_list.size());
                SPI_UTIL_POST_CONDITION((m_map.size() + 1) == m_size);
            }
            m_list.push_front(list_element_type(key, value));
            m_map.insert(map_element_type(key, m_list.begin()));
        }
        else
        {
            // in the cache - why did we not check beforehand?
            //
            // this differs from the find code in that we use the external value
            // rather than the previous value from the cache
            m_list.erase(iter->second);
            m_list.push_front(list_element_type(key, value));
            iter->second = m_list.begin();
        }
    }

    std::vector<list_element_type> all() const
    {
        std::vector<list_element_type> out;
        out.reserve(m_size);
        for (typename std::list<list_element_type>::const_iterator iter = m_list.begin(); iter != m_list.end(); ++iter)
        {
            out.push_back(list_element_type(iter->first, iter->second));
        }
        return out;
    }

private:

    std::list<list_element_type> m_list;
    std::map<K, map_element_value_type> m_map;
    size_t m_size;
};

SPI_UTIL_END_NAMESPACE

#endif


