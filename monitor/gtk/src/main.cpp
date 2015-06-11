#include <gtkmm.h>
#include <iostream>
#include <string>

#include "dharc/monitor.hpp"
#include "statsview.hpp"

using std::string;
using dharc::Monitor;

class StatsColumns : public Gtk::TreeModel::ColumnRecord {
	public:
	StatsColumns() {
		add(name); add(value);
	}

	Gtk::TreeModelColumn<Glib::ustring> name;
	Gtk::TreeModelColumn<Glib::ustring> value;
};

StatsColumns statscols;

int main(int argc, char *argv[]) {
	Monitor monitor("localhost", 7878);

	Glib::RefPtr<Gtk::Application> app =
	Gtk::Application::create(argc, argv,
		"uk.co.dharc.monitor");

	Gtk::Window *window;
	Gtk::TreeView *statstree;
	//window.set_default_size(200, 200);

	Glib::RefPtr<Gtk::Builder> builder = Gtk::Builder::create();
	builder->add_from_file("ui.glade");
	builder->get_widget("dmonmain", window);
	builder->get_widget("statsview", statstree);

	dharc::gtk::StatsView statsview(statstree, monitor);

	return app->run(*window);
}

