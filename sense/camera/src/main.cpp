/*
 * Copyright 2015 Nicolas Pope
 */

#include "dharc/sense.hpp"

using dharc::Sense;
using dharc::Node;

int main(int argc, char *argv[]) {
	Sense sense("localhost", 7878);

	// Allocate Nodes for colour and dimensions
	Node colour_f, colour_l;
	sense.unique(255, colour_f, colour_l);
	Node x_f, x_l;
	sense.unique(320, x_f, x_l);
	Node y_f, y_l;
	sense.unique(240, y_f, y_l);

	vector<vector<Node>> data;
	data.resize(320);
	for (auto i = 0U; i < data.size(); ++i) {
		data[i].resize(240);
		for (auto j = 0U; j < data[i].size(); ++j) {
			data[i][j] = colour_f;
		}
	}

	for (auto k = 0; k < 100; ++k) {
		try {
		sense.write({}, x_f, y_f, data);
		} catch (std::exception) {
		}
	}

	return 0;
}

