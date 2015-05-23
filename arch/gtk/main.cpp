#include <gtkmm.h>

int main(int argc, char *argv[]) {
	Glib::RefPtr<Gtk::Application> app =
	Gtk::Application::create(argc, argv,
		"uk.co.dharc.fdsb");

	Gtk::Window *window;
	//window.set_default_size(200, 200);

	Glib::RefPtr<Gtk::Builder> builder = Gtk::Builder::create();
	builder->add_from_file("ui.glade");
	builder->get_widget("fdsbmain", window);


	return app->run(*window);
}

