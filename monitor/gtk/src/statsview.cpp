/*
 * Copyright 2015 Nicolas Pope
 */

#include "statsview.hpp"
#include <type_traits>
#include <sigc++/sigc++.h>
namespace sigc
{   
    template <typename Functor>
        struct functor_trait<Functor, false>
        {   
            typedef decltype (::sigc::mem_fun (std::declval<Functor&> (), 
                        &Functor::operator())) _intermediate;

            typedef typename _intermediate::result_type result_type;
            typedef Functor functor_type;
        };  
};

using dharc::gtk::StatsView;

StatsView::StatsView(Gtk::TreeView *widget, dharc::Monitor &mon) :
	tree_(widget), mon_(mon) {
	store_ = Gtk::TreeStore::create(cols_);
	makeTree();
	setLabels();
	tree_->set_model(store_);

	sigc::connection stat_conn = Glib::signal_timeout().connect([&]() {
		int harccount = mon_.harcCount();
		int branchcount = mon_.branchCount();
		float bfactor = static_cast<float>(branchcount) /
						static_cast<float>(harccount);
		int followcount = mon_.followCount();
		float ffactor = static_cast<float>(followcount) /
						static_cast<float>(harccount);
		float actives = mon_.activationsPerSecond() / 1000.0f;
		float processed = mon_.processedPerSecond() / 1000.0f;
		float follows = mon_.followsPerSecond() / 1000.0f;

		char buffer[100];
		sprintf(buffer, "%dK", harccount / 1000);
		stats_[kHarcCount][cols_.value] = buffer;
		sprintf(buffer, "%dK", branchcount / 1000);
		stats_[kBranchCount][cols_.value] = buffer;		
		sprintf(buffer, "%.2f", bfactor);
		stats_[kBranchFactor][cols_.value] = buffer;
		sprintf(buffer, "%dK", followcount / 1000);
		stats_[kFollowCount][cols_.value] = buffer;		
		sprintf(buffer, "%.2f", ffactor);
		stats_[kFollowFactor][cols_.value] = buffer;
		sprintf(buffer, "%.2f", actives);
		stats_[kActivations][cols_.value] = buffer;
		sprintf(buffer, "%.2f", processed);
		stats_[kProcessed][cols_.value] = buffer;
		sprintf(buffer, "%.2f", follows);
		stats_[kFollows][cols_.value] = buffer;
		return true;
	}, 100);
}	


StatsView::~StatsView() {
}

void StatsView::makeTree() {
	auto title = store_->append();
	(*title)[cols_.name] = "Scale Statistics";
	stats_[kHarcCount] = *store_->append(title->children());
	stats_[kBranchCount] = *store_->append(title->children());
	stats_[kBranchFactor] = *store_->append(title->children());
	stats_[kFollowCount] = *store_->append(title->children());
	stats_[kFollowFactor] = *store_->append(title->children());
	title = store_->append();
	(*title)[cols_.name] = "Performance Statistics";
	stats_[kActivations] = *store_->append(title->children());
	stats_[kProcessed] = *store_->append(title->children());
	stats_[kFollows] = *store_->append(title->children());
}



void StatsView::setLabels() {
	stats_[kHarcCount][cols_.name]    = "Number of Harcs";
	stats_[kBranchCount][cols_.name]  = "Number of Branches";
	stats_[kBranchFactor][cols_.name] = "Branch Factor";
	stats_[kFollowCount][cols_.name]  = "Number of Queries";
	stats_[kFollowFactor][cols_.name] = "Query Factor";
	stats_[kActivations][cols_.name]  = "Activations (Kps)";
	stats_[kProcessed][cols_.name]    = "Processed (Kps)";
	stats_[kFollows][cols_.name]      = "Harc Follows (Kps)";
}


