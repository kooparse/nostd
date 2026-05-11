
template <typename K, typename V>
struct Entry {
    K key;
    V value;
};

template <typename K, typename V>
struct Hash_Table {
    Entry<K, V>*    data;
    int             count;
    int             capacity;
};


