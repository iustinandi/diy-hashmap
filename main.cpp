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
		std::pair<const Key, Val> data_pair;
		BucketState state;
		Node() : state(BucketState::empty) {}
		Node(Key k, Val v) : data_pair(k, v), state(BucketState::occupied) {}

		Node& operator=(const Node& node) {
			if (this != &node) {
				std::destroy_at(this);
				std::construct_at(this, node);
			}
			return *this;
		}
	};

	int m_size;
	std::vector<Node> m_table;

	int m_count_occupied = 0;
	int m_count_deleted = 0;

	HashFunc _hash;
	inline std::size_t _h1(std::size_t init_hash) {
		//return init_hash >> 32;
		return init_hash;
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

		m_size *= 2;
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

	class Hashterator {
		using NodeIterator = std::vector<Node>::iterator;
		NodeIterator m_current, m_end;
	public:
		using iterator_category = std::forward_iterator_tag;
		using value_type = std::pair<const Key, Val>;
		using difference_type = std::ptrdiff_t;
		using pointer = value_type*;
		using reference = value_type&;

		Hashterator(NodeIterator current, NodeIterator end) : m_current(current), m_end(end) {}

		reference operator*() const {
			return m_current->data_pair;
		}

		pointer operator->() const {
			return &(m_current->data_pair);
		}

		Hashterator& operator++() {
			m_current++;
			for (; m_current != m_end && m_current->state != BucketState::occupied; m_current++);
			return *this;
		}

		Hashterator operator++(int) {
			Hashterator tmp = *this;
			m_current++;
			for (; m_current != m_end && m_current->state != BucketState::occupied; m_current++);
			return tmp;
		}

		bool operator==(const Hashterator& operand) const {
			return m_current == operand.m_current;
		}

		bool operator!=(const Hashterator& operand) const {
			return m_current != operand.m_current;
		}
	};

	Hashterator begin() {
		Hashterator it = Hashterator(m_table.begin() - 1, m_table.end());
		return ++it;
	}

	Hashterator end() {
		return Hashterator(m_table.end(), m_table.end());
	}

	void insert(Key k, Val v) {
		int index = _get_index(k);
		Node& node = m_table[index];

		if (node.state == BucketState::occupied)
			return;

		std::destroy_at(&node);
		std::construct_at(&node, k, v);
		m_count_occupied++;

		_check();
	}

	Hashterator find(Key k) {
		int index = _get_index(k);
		Node& node = m_table[index];

		if (node.state == BucketState::occupied)
			return Hashterator(m_table.begin() + index, m_table.end());
		else
			return end();
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

	Val& operator[](Key k) {
		insert(k, Val());
		return find(k)->second;
	}
};