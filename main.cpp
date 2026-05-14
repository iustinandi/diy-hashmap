#include <vector>
#include <concepts>
#include <type_traits>
#include <functional>
#include <utility>

template <typename Key, typename Val, typename HashFunc = std::hash<Key>>
requires std::is_invocable_r_v<std::size_t, HashFunc, Key>
class Hashtable {
	enum class BucketState {
		empty, occupied, deleted
	};

	struct Node {
		std::pair<Key, Val> data_pair;
		BucketState state;
		Node() : state(BucketState::empty) {}
		Node(Key k, Val v) : data_pair(k, v), state(BucketState::occupied) {}
	};

	int m_size;
	std::vector<Node> m_table;

	HashFunc _hash;
	inline std::size_t _h1(std::size_t init_hash) {
		return init_hash >> 32;
	}
	inline std::size_t _h2(std::size_t init_hash) {
		return init_hash & 0xFFFFFFFF | 1;
	}
public:
	Hashtable() : m_size(8), m_table(m_size) {}

};