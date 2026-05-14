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

	int m_count_occupied = 0;
	int m_count_deleted = 0;

	HashFunc _hash;
	inline std::size_t _h1(std::size_t init_hash) {
		return init_hash >> 32;
	}
	inline std::size_t _h2(std::size_t init_hash) {
		return init_hash & 0xFFFFFFFF | 1;
	}

	int _get_index(Key k) {
		std::size_t hash = _hash(k);
		std::size_t h1 = _h1(hash);
		std::size_t h2 = _h2(hash);
		int index = h1 % m_size;
		while (m_table[index].state == BucketState::occupied && m_table[index].data_pair.first != k || m_table[index].state == BucketState::deleted)
			index = (index + h2) % m_size;
		return index;
	}

	void _rehash() {
		std::vector<Node> old_table = std::move(m_table);

		m_size <<= 1; // TODO: check if is this even an improvement over *= 2 or if it's just larp
		m_table = std::vector<Node>(m_size);
		m_count_occupied = m_count_deleted = 0;

		for (const Node& node : old_table) {
			if (node.state == BucketState::occupied) {
				insert(node.data_pair.first, node.data_pair.second);
			}
		}
	}

	void _check() {
		if (static_cast<double>(m_count_occupied) / m_size > 0.7 // factorul de incarcare
			|| static_cast<double>(m_count_deleted) / m_size > 0.1) // % contaminare
			_rehash();
	}

public:
	Hashtable() : m_size(8), m_table(m_size) {}
	~Hashtable() {}

	void insert(Key k, Val v) { // TODO: rehash
		int index = _get_index(k);
		Node& node = m_table[index];

		if (node.state == BucketState::occupied) 
			return;

		node.data_pair.second = v;

		node.state = BucketState::occupied;
		m_count_occupied++;

		_check();
	}

	std::pair<Key, Val>* find(Key k) { // TODO: replace with iterator
		int index = _get_index(k);
		Node& node = m_table[index];

		if (node.state == BucketState::occupied)
			return &node.data_pair;
		else
			return nullptr;
	}

	void remove(Key k) {
		int index = _get_index(k);
		Node& node = m_table[index];

		if (node.state == BucketState::occupied) {
			node.state = BucketState::deleted;
			
			m_count_occupied--;
			m_count_deleted++;

			_check();
		}
	}

};