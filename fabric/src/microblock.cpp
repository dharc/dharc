/*
 * Copyright 2015 Nicolas Pope
 */

#include "dharc/microblock.hpp"

using dharc::fabric::MicroBlock;
using std::unique_lock;
using std::mutex;

template<typename T>
MicroBlock<T>::MicroBlock(MacroBlock<T> *macro, uint64_t blockid) :
	blockid_(blockid), macro_(macro), sig_({this}), lastgarbage_(0) {
	harc_count_ = params::BLOCK_WIDTH * params::BLOCK_WIDTH;
}

template<typename T>
MicroBlock<T>::~MicroBlock() {}



template<typename T>
bool MicroBlock<T>::allocate(size_t count, Node &first, Node &last) {
	// Nothing to do, first BLOCK_WIDTH * BLOCK_WIDTH are reserved.
	first = Node(blockid_);
	last = Node(blockid_ | (count - 1));
	return count < (params::BLOCK_WIDTH * params::BLOCK_WIDTH);
}



template<typename T>
bool MicroBlock<T>::allocate(Node &node) {
	if (freed_.size() == 0) {
		if (harc_count_ >= params::MAX_BLOCK_HARCS) return false;
		node.value = blockid_ | harc_count_;
		++harc_count_;
		return true;
	} else {
		node.value = blockid_ | freed_.back();
		freed_.pop_back();
		return true;
	}
}



template<typename T>
void MicroBlock<T>::process(int factor) {
	array<Node, params::MAX_TAIL> signodes;
	Tail tail;
	vector<Node> tvec;

	while (factor-- > 0) {
		lock();
			if (sig_.size() < params::MAX_TAIL) {
				unlock();
				break;
			}


			// Copy the nodes to a buffer for processing
			auto it = sig_.begin();
			for (auto i = 0U; i < params::MAX_TAIL; ++i) {
				signodes[i] = *it;
				++it;
			}
			// remove first and try again
			//sig_.erase(sig_.begin());
		unlock();

		// ++processed__;

		// Generate all tail combinations
		// Vary number of tails
		for (auto t = 0U; t < (params::MAX_TAIL - params::MIN_TAIL); ++t) {
			tvec.clear();
			// Now pick t most significant 
			for (auto i = 0U; i < (params::MAX_TAIL-t); ++i) {
				tvec.push_back(signodes[i]);
			}
			Tail::make(tvec, tail);
			if (query(tail, tvec)) {
				break;
			}
		}
	}

	lock();
	sig_.clear();
	unlock();

	if ((Fabric::counter() - lastgarbage_) > 1000) {
		garbage();
		lastgarbage_ = Fabric::counter();
	}
}



template<typename T>
void MicroBlock<T>::garbage() {
	auto i = tails_.begin();
	while (i != tails_.end()) {
		//if (maxgarbage == 0) break;
		//--maxgarbage;

		//if ((*i).second != dharc::null_n) {
			Harc *h = get((*i).second);
			if (h->isWeak()) {
				freed_.push_back((*i).second.harc());
				//std::cout << "GARBAGE\n";
				i = tails_.erase(i);
				h->reset();
				// ++cullcount__;
			} else {
				++i;
			}
		//}
	}

	std::cout << "Free: " << freed_.size() << std::endl;
}



template<typename T>
bool MicroBlock<T>::get(const Tail &key, Node &hnode) {
	auto it = tails_.find(key);

	if (it != tails_.end()) {
		hnode = it->second;
		return true;
	}
	hnode = dharc::null_n;
	return false;
}



template<typename T>
bool MicroBlock<T>::query(const Tail &tail, const vector<Node> &tvec) {
	Node hnode;
	
	if (get(tail, hnode)) {
		if (hnode == dharc::null_n) return true;
		Harc *h = get(hnode);

		// Average the significance of the tail
		float sig = 0.0;
		for (auto i : tvec) sig += get(i)->significance();
		sig = sig / static_cast<float>(tvec.size());

		h->pulse(sig);
		lock();
		addToQueue(hnode, h);
		unlock();

		// Strong enough to be important?
		if (h->strength() > 0.1) {
			macro_->addStrong(hnode, tvec);
			//std::cout << "Strong: " << h->strength() <<
			//		" @ " << tvec.size() << std::endl;
		}
		return true;
	}

	if (!allocate(hnode)) {
		return false;
	}
	tails_.insert({tail, hnode});
	Harc *h = get(hnode);

	// Average significance of the tail
	float sig = 0.0;
	for (auto i : tvec) sig += get(i)->significance();
	sig = sig / static_cast<float>(tvec.size());


	h->pulse(sig);
	lock();
	addToQueue(hnode, h);
	unlock();
	return false; 
}



template<typename T>
void MicroBlock<T>::pulse(const Node &n) {
	Harc *h = get(n);
	// ++activatecount__;
	h->pulse(1.0f);
	addToQueue(n, h);
}



template<typename T>
void MicroBlock<T>::constant(const Node &n, float amount) {
	Harc *h = get(n);
	h->activateConstant(amount);
	addToQueue(n, h);

	/*const size_t count = last.value - first.value;
	// activatecount__ += count;
	Node current = first;

	for (size_t i = 0; i < count; ++i) {
		Harc *h = get(current);
		h->activateConstant(amount[i]);
		addToQueue(current, h);
		++current.value;
	}*/
}



template<typename T>
inline bool MicroBlock<T>::harcCompare(const Node &a, const Node &b) {
	return get(a)->significance() > get(b)->significance();
}



template<typename T>
void MicroBlock<T>::addToQueue(const Node &node, Harc *harc) {
	if (harc->significance() < params::SIG_THRESHOLD) return;
	
	if (sig_.size() < params::MAX_SIGNIFICANT) {
		sig_.insert(node);
	} else {
		if (harcCompare(node, *(--sig_.end()))) {
			sig_.erase(--sig_.end());
			sig_.insert(node);
		}
	}
}

