/*
 * Copyright 2015 Nicolas Pope
 */

#ifndef DHARC_MONITOR_GTK_STATSVIEW_HPP_
#define DHARC_MONITOR_GTK_STATSVIEW_HPP_

#include <gtkmm.h>
#include "dharc/monitor.hpp"

namespace dharc {
namespace gtk {
class StatsColumns : public Gtk::TreeModel::ColumnRecord {
	public:
	StatsColumns() {
		add(name); add(value);
	}

	Gtk::TreeModelColumn<Glib::ustring> name;
	Gtk::TreeModelColumn<Glib::ustring> value;
};



enum Statistic {
	kHarcCount,
	kBranchCount,
	kBranchFactor,
	kFollowCount,
	kFollowFactor,
	kActivations,
	kProcessed,
	kFollows,
	kStatEnd
};



class StatsView {
	public:
	StatsView(Gtk::TreeView *widget, dharc::Monitor &mon);
	~StatsView();

	private:
	void timeout();

	void makeTree();
	void setLabels();
	void updateStats();
	void updateTree();

	StatsColumns cols_;
	Gtk::TreeView *tree_;
	dharc::Monitor &mon_;
	Glib::RefPtr<Gtk::TreeStore> store_;

	Gtk::TreeRow stats_[kStatEnd];
};
};  // namespace gtk
};  // namespace dharc

#endif  // DHARC_MONITOR_GTK_STATSVIEW_HPP_

