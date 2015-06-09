#include <gtkmm.h>
#include <iostream>
#include <string>

#include "dharc/monitor.hpp"

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
	auto statstore = Gtk::TreeStore::create(statscols);
	Gtk::TreeView *statsview;
	//window.set_default_size(200, 200);

	Glib::RefPtr<Gtk::Builder> builder = Gtk::Builder::create();
	builder->add_from_file("ui.glade");
	builder->get_widget("dmonmain", window);
	builder->get_widget("statsview", statsview);

	auto title = statstore->append();
	(*title)[statscols.name] = "Scale Statistics";
	auto stat_numharcs = *statstore->append(title->children());
	auto stat_numbranch = *statstore->append(title->children());
	auto stat_bfactor = *statstore->append(title->children());
	title = statstore->append();
	(*title)[statscols.name] = "Performance Statistics";
	auto stat_active = *statstore->append(title->children());
	auto stat_processed = *statstore->append(title->children());
	auto stat_follows = *statstore->append(title->children());
	
	stat_numharcs[statscols.name] = "Number of Harcs";
	stat_numbranch[statscols.name] = "Number of Branches";
	stat_bfactor[statscols.name] = "Branch Factor";
	stat_active[statscols.name] = "Activations (Kps)";
	stat_processed[statscols.name] = "Processed (Kps)";
	stat_follows[statscols.name] = "Harc Follows (Kps)";

	statsview->set_model(statstore);

	sigc::connection stat_conn = Glib::signal_timeout().connect([&]() {
		int harccount = monitor.harcCount();
		int branchcount = monitor.branchCount();
		float bfactor = static_cast<float>(branchcount) /
						static_cast<float>(harccount);
		float actives = monitor.activationsPerSecond() / 1000.0f;
		float processed = monitor.processedPerSecond() / 1000.0f;
		float follows = monitor.followsPerSecond() / 1000.0f;

		char buffer[100];
		sprintf(buffer, "%dK", harccount / 1000);
		stat_numharcs[statscols.value] = buffer;
		sprintf(buffer, "%dK", branchcount / 1000);
		stat_numbranch[statscols.value] = buffer;		
		sprintf(buffer, "%.2f", bfactor);
		stat_bfactor[statscols.value] = buffer;
		sprintf(buffer, "%.2f", actives);
		stat_active[statscols.value] = buffer;
		sprintf(buffer, "%.2f", processed);
		stat_processed[statscols.value] = buffer;
		sprintf(buffer, "%.2f", follows);
		stat_follows[statscols.value] = buffer;
		return true;
	}, 100);

	return app->run(*window);
}

