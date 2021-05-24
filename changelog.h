#include <gtkmm.h>

class ChangeLogRelease {
	public:
		ChangeLogRelease(const char* cversion, const char* cstr_date, const char* cdescription);
		Glib::ustring version;
		Glib::ustring description;
};

class ChangeLog : public Gtk::MessageDialog {
	public:
		ChangeLog(Gtk::Window& win);
		std::vector<ChangeLogRelease> load_changelog();
	protected:
		Gtk::ScrolledWindow sv;
		Gtk::Box inner = Gtk::Box(Gtk::Orientation::ORIENTATION_VERTICAL);
};
