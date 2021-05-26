#include "mainwindow.h"
#include <gtkmm/application.h>

CMainWindow *mainwindow;

int main (int argc, char *argv[])
{
	Gsv::init();
	
	Glib::RefPtr<Gtk::Application> app = Gtk::Application::create(argc, argv, "com.github.blackhole89.notekit");
	
	app->signal_activate().connect( [app,&mainwindow]() {
			mainwindow=new CMainWindow(app);
			app->set_app_menu(mainwindow->menu);
			app->set_menubar(mainwindow->legacy);
			app->add_window(*mainwindow);
		} );
	
	app->set_accel_for_action("notebook.next-note", "<Primary>Tab");
	app->set_accel_for_action("notebook.prev-note", "<Primary><Shift>Tab");
    app->set_accel_for_action("win.sidebar", "F9");
	app->set_accel_for_action("win.zen", "F11");
	
	auto ret = app->run();

	if(mainwindow != nullptr){
		delete mainwindow;
	}

	return ret;
}
