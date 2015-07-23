/*
 * Copyright 2015 Nicolas Pope
 */

#include "dharc/region.hpp"

using dharc::fabric::Region;
using std::pair;


Region::Region(size_t width, size_t height, size_t unitsx, size_t unitsy)
	: unitsx_(unitsx), unitsy_(unitsy), width_(width), height_(height),
		uwidth_(width / unitsx), uheight_(height / unitsy),
		outsize_(uwidth_ * uheight_) {
	assert(width % unitsx == 0);
	assert(height % unitsy == 0);

	units_.resize(1);
	makeInputLayer();
}



Region::~Region() {
}



void Region::makeInputLayer() {
	units_[0].resize(unitsx_);
	for (auto x = 0U; x < unitsx_; ++x) {
		units_[0][x].resize(unitsy_);
		for (auto y = 0U; y < unitsy_; ++y) {
			initUnit(units_[0][x][y], uwidth_, uheight_);
		}
	}
}



void Region::initUnit(Unit &unit, size_t iwidth, size_t iheight) {
	float maxdist = std::sqrt((float)(iwidth * iwidth) +
								(float)(iheight * iheight)) / 3.0f;

	const auto insize = iwidth * iheight;
	const auto linksize = outsize_ * insize;

	unit.inputs.resize(insize);
	unit.outputs.resize(outsize_);
	unit.links.resize(linksize);
	unit.counts.resize(outsize_);

	for (auto x = 0U; x < outsize_; ++x) {
		const int xi = (int)(((float)x / (float)outsize_) * (float)insize);
		const int xx = xi % uwidth_;
		const int xy = xi / uwidth_;

		unit.counts[x] = 0.0f;
		unit.outputs[x] = 0.0f;
		unit.inputs[x] = 0.0f;

		for (auto y = 0U; y < insize; ++y) {
			const int yx = y % uwidth_;
			const int yy = y / uwidth_;
			const int dx = xx - yx;
			const int dy = xy - yy;
			const float dist = std::sqrt(dx * dx + dy * dy) / maxdist;
			
			if (dist >= 1.0f) {
				unit.links[y * outsize_ + x].strength = 0.0f;
			} else {
				unit.links[y * outsize_ + x].strength = 1.0f - dist;
			}
			unit.links[y * outsize_ + x].depol = 0.0f;
		}
	}

	unit.modulation = 0.5f;
}



void Region::write(const vector<uint8_t> &v) {
	assert(v.size() == width_ * height_);

	for (auto x = 0U; x < unitsx_; ++x) {
		for (auto y = 0U; y < unitsy_; ++y) {
			float mininput = 1.1f;
			float maxinput = 0.0f;
			Unit &unit = units_[0][x][y];

			for (auto xx = 0U; xx < uwidth_; ++xx) {
				for (auto yy = 0U; yy < uheight_; ++yy) {
					const auto ix = (x * uwidth_) + xx + ((y * uheight_ + yy) * width_);
					const auto ux = xx + (yy * uwidth_);
					unit.inputs[ux] = (float)v[ix] / 255.0f;
					if (unit.inputs[ux] < mininput) mininput = unit.inputs[ux];
					if (unit.inputs[ux] > maxinput) maxinput = unit.inputs[ux];
				}
			}

			/*float scale = 1.0f / (maxinput - mininput);
			if (scale > kContrastMax) scale = kContrastMax;

			// Level the inputs
			for (auto i = 0U; i < uwidth_ * uheight_; ++i) {
				unit.inputs[i] = (unit.inputs[i] - mininput) * scale;
			}*/
		}
	}
}



void Region::process() {
	//adjustModulation();

	//#pragma omp parallel for
	for (auto i = 0U; i < 1; ++i) {
		//decaySpatial(i);
		//activate(units_[i], &inputs_[i * USIZE], USIZE);
		//adjustSpatial(i, s);
		processLayer(i);
	}
}



void Region::processLayer(size_t layer) {
	#pragma omp parallel for
	for (auto x = 0U; x < unitsx_; ++x) {
		for (auto y = 0U; y < unitsy_; ++y) {
			processUnit(units_[layer][x][y]);
		}
	}
}



void Region::reform(vector<uint8_t> &v) {
	v.resize(width_ * height_);

	for (auto i = 0U; i < v.size(); ++i) {
		const auto y = i / width_;
		const auto x = i % width_;
		const auto uy = y / uheight_;
		const auto oy = y % uheight_;
		const auto ux = x / uwidth_;
		const auto ox = x % uwidth_;
		const auto uix = oy * uwidth_ + ox;

		Unit &unit = units_[0][ux][uy];
		float tmp = 0.0f;
		int count = 0;

		for (auto j = 0U; j < outsize_; ++j) {
			if (unit.outputs[j] > 0.0001f) {
				++count;
				tmp += unit.links[uix * outsize_ + j].strength * unit.outputs[j];
			}
		}
		//tmp = unit.outputs[uix];
		tmp /= count;
		if (tmp > 1.0f) tmp = 1.0f;
		v[i] = tmp * 255.0f;
	}
}



void Region::processUnit(Unit &unit) {
	struct LinkState {
		float depol;
		size_t input;
		Link *link;
	};

	vector<pair<size_t, float>> total_depol(outsize_, {0, 0.0f});
	vector<vector<LinkState>> linkstates(outsize_);

	for (auto i = 0U; i < outsize_; ++i) {
		total_depol[i].first = i;
		linkstates[i].reserve(uwidth_ * uheight_);
	}

	float insize = uwidth_ * uheight_;
	// Percentage of max possible
	float linklimit = 0.2f * (float)insize;

	// Calculate individual link depolarisations and save
	for (auto i = 0U; i < unit.inputs.size(); ++i) {
		for (auto j = 0U; j < outsize_; ++j) {
			auto &link = unit.links[i * outsize_ + j];
			const float depol = (unit.inputs[i] * link.strength) / linklimit;

			//if (depol > 0.0001f) {
			linkstates[j].push_back({depol, i, &link});
			//}
			total_depol[j].second += depol;
		}
	}

	// Energy restrict total depols
	for (auto i = 0U; i < outsize_; ++i) {
		if (total_depol[i].second > 1.0f) {
			//float excess = total_depol[i].second;
			total_depol[i].second = 0.0f;
		}
	}

	// Sort the match percentages, highest first.
	std::sort(total_depol.begin(), total_depol.end(), [](auto a, auto b) {
		return a.second > b.second;
	});

	// Minimum % match before activation
	float threshold = 0.2f;
	float factor = 1.0f;

	// For each sorted pattern
	for (auto i = total_depol.begin(); i != total_depol.end(); ++i) {
		auto &d = *i;

		// If it matched enough then
		if (d.second * factor >= threshold) {
			float newoutput = (d.second - threshold) * (1.0f / (1.0f - threshold)) * factor;
			factor *= (1.0f - newoutput);

			// If not already activated
			if (unit.outputs[d.first] < 0.0001f) {
				// Sort individual links to find tipping point
				std::sort(linkstates[d.first].begin(), linkstates[d.first].end(), [](auto a, auto b) {
					return a.depol > b.depol;
				});

				float depolsum = 0.0f;

				// For all of this patterns links
				for (auto l : linkstates[d.first]) {
					// If this link occured after threshold
					if (depolsum >= threshold) {
						// Weaken link because it was not a main contributor
						l.link->strength -= l.depol * newoutput * kLearnRate;
					} else {
						// This input contributed to this pattern, so strengthen
						l.link->strength += unit.inputs[l.input] * (1.0f - l.link->strength) * newoutput * kLearnRate;
						//unit.counts[d.first] += l.link->strength;

						// If reached 1 then find one that is zero and double the link
						//if (l.link->strength > (1.0f - epsilon)) {
							
						//}
					}

					depolsum += l.depol;
				}

				// Resort after changes.
				//std::sort(i + 1, total_depol.end(), [](auto a, auto b) {
				//	return a.second > b.second;
				//});
			}

			unit.outputs[d.first] = newoutput;
		} else {
			unit.outputs[d.first] = 0.0f;
		}
	}
}

