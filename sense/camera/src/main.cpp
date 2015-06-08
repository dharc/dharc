/*
 * Copyright 2015 Nicolas Pope
 */

#include "dharc/sense.hpp"

#include <iostream>

using dharc::Sense;
using dharc::Node;
using std::cout;

int main(int argc, char *argv[]) {
	Sense sense("localhost", 7878);

	Node cam_f, cam_l;
	sense.makeHarcs(320 * 240, cam_f, cam_l);

	vector<float> data;
	data.resize(320 * 240);

	for (auto i = 0U; i < data.size(); ++i) {
		data[i] = 1.0f;
	}

	for (auto k = 0; k < 1000; ++k) {
		sense.activate(cam_f, cam_l, data);
	}

	return 0;
}

