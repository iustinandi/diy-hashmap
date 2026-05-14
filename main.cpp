#include <vector>
#include <concepts>
#include <type_traits>
#include <functional>
#include <utility>

template <typename Key, typename Val, typename HashFunc = std::hash<Key>>
requires std::is_invocable_r_v<std::size_t, HashFunc, Key>
class Hashtable {
	enum class bucket_state {
		empty, occupied, deleted
	};

	int m_size;
	std::vector<std::pair<Key, Val>> m_table;
	HashFunc _hash;
public:
	Hashtable() : m_size(8), m_table(m_size) {}

};