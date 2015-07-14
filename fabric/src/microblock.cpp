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
inline size_t MicroBlock<T>::distance(const Node &a, const Node &b) {
	const auto ah = a.harc();
	const auto bh = b.harc();
	const auto ax = ah % params::BLOCK_WIDTH;
	const auto ay = ah / params::BLOCK_WIDTH;
	const auto bx = bh % params::BLOCK_WIDTH;
	const auto by = bh / params::BLOCK_WIDTH;

	return std::max(std::abs(ax-bx),std::abs(ay-by));
}



template<typename T>
Node MicroBlock<T>::sigNeighbour(const Node *signodes, size_t count, float minsig) {
	Node focus = signodes[count-1];
	Node n[8];
	float max_sig = 0.0f;
	int sigix = -1;
	const int x = focus.harc() % params::BLOCK_WIDTH;
	const int y = focus.harc() / params::BLOCK_WIDTH;

	if ((x - 1) < 0) {
		n[0] = dharc::null_n;
		n[4] = dharc::null_n;
		n[5] = dharc::null_n;
	} else {
		n[0] = Node(focus.value - 1);
		if ((y - 1) >= 0) {
			n[4] = Node(focus.value - params::BLOCK_WIDTH - 1);
		} else {
			n[4] = dharc::null_n;
		}
		if ((y + 1) < (signed)params::BLOCK_WIDTH) {
			n[5] = Node(focus.value + params::BLOCK_WIDTH - 1);
		} else {
			n[5] = dharc::null_n;
		}
	}

	if ((x + 1) == params::BLOCK_WIDTH) {
		n[1] = dharc::null_n;
		n[6] = dharc::null_n;
		n[7] = dharc::null_n;
	} else {
		n[1] = Node(focus.value + 1);
		if ((y - 1) >= 0) {
			n[6] = Node(focus.value - params::BLOCK_WIDTH + 1);
		} else {
			n[6] = dharc::null_n;
		}
		if ((y + 1) < (signed)params::BLOCK_WIDTH) {
			n[7] = Node(focus.value + params::BLOCK_WIDTH + 1);
		} else {
			n[7] = dharc::null_n;
		}
	}

	if ((y + 1) == params::BLOCK_WIDTH) {
		n[2] = dharc::null_n;
	} else {
		n[2] = Node(focus.value + params::BLOCK_WIDTH);
	}

	if ((y - 1) < 0) {
		n[3] = dharc::null_n;
	} else {
		n[3] = Node(focus.value - params::BLOCK_WIDTH);
	}

	for (int i = 0; i < 8; ++i) {
		for (auto j = 0U; j < count; ++j) {
			if (n[i] == signodes[j]) {
				n[i] = dharc::null_n;
				break;
			}
		}

		if (n[i] != dharc::null_n) {
			Harc *h = get(n[i]);
			if (h->significance() > max_sig) {
				max_sig = h->significance();
				sigix = i;
			}
		}
	}

	if (sigix == -1 || (max_sig < minsig)) {
		return dharc::null_n;
	} else {
		return n[sigix];
	}
}



template<typename T>
void MicroBlock<T>::process(int factor) {
	Node signodes[params::MAX_TAIL];
	Tail tail;
	vector<Node> tvec;
	float minsig = 0.0f;
	const float sigdelta = 1.0f / (float)factor;

	lock();
	for (int f = 0; f < factor; ++f) {
		if (sig_.size() < params::MIN_TAIL) {
			break;
		}


		// Pick most significant node.
		auto it = sig_.begin();
		signodes[0] = *it;
		minsig = get(signodes[0])->significance() * 0.5;
		size_t count = 1;
		sig_.erase(it);

		// Find most significant neighbour
		while (count < params::MAX_TAIL) {
			Node signode = sigNeighbour(signodes, count, minsig);
			if (signode != dharc::null_n) {
				signodes[count++] = signode;
			} else {
				break;
			}
		}

		if (count < params::MIN_TAIL) continue;

		//bool has_added = false;

		// Generate all tail combinations
		// Vary number of tails
		tvec.clear();
		float sig = 0.0f;

		// Now pick t most significant 
		for (auto i = 0U; i < count; ++i) {
			tvec.push_back(signodes[i]);
			//sig += get(signodes[i])->significance();
		}

		sig = 1.0f - ((float)f * sigdelta);//sig / static_cast<float>(tvec.size());
		/*if (f == 0) {
			sig_delta = 1.0f - sig;
			sig = 1.0f;
		} else {
			sig += sig_delta;
		}*/

		Tail::make(tvec, tail);

		if (!query(tail, tvec, sig)) {
			//if (Fabric::counter() < 12000) {
				add(tail, sig);
			//}
		}
	}

	//lock();
	sig_.clear();
	unlock();

	//if (Fabric::counter() < 14000) {
		if ((Fabric::counter() - lastgarbage_) > 1000) {
			garbage();
			lastgarbage_ = Fabric::counter();
		}
	//}
}



template<typename T>
void MicroBlock<T>::garbage() {
	auto i = tails_.begin();
	size_t count = 0;
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
				++count;
				// ++cullcount__;
			} else {
				++i;
			}
		//}
	}

	std::cout << "Free: " << count << std::endl;
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
bool MicroBlock<T>::query(const Tail &tail, const vector<Node> &tvec, float sig) {
	Node hnode;
	
	if (get(tail, hnode)) {
		//if (hnode == dharc::null_n) return true;
		Harc *h = get(hnode);
		if (!h->pulse(sig)) return true;
		//addToQueue(hnode, h);

		// Strong enough to be important?
		if (h->strength() > 0.1) {
			macro_->addStrong(hnode, tvec);
		}
		return true;
	}

	return false; 
}



template<typename T>
void MicroBlock<T>::add(const Tail &tail, float sig) {
	Node hnode;

	if (!allocate(hnode)) {
		//std::cout << "Failed to allocate\n";
		return;
	}

	tails_.insert({tail, hnode});
	Harc *h = get(hnode);
	h->pulse(sig);
	//addToQueue(hnode, h);
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


