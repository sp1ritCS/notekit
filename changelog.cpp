#include "changelog.h"
#include <gtkmm.h>
#include <tinyxml2.h>

ChangeLogRelease::ChangeLogRelease(const char* cversion, const char* cstr_date, const char* cdescription) {
    version = Glib::ustring(cversion);
    description = Glib::ustring(cdescription);
}

ChangeLog::ChangeLog(Gtk::Window& win) : Gtk::MessageDialog(win, "Latest changes in NoteKit", false, Gtk::MessageType::MESSAGE_INFO, Gtk::ButtonsType::BUTTONS_CLOSE, true) {
    signal_response().connect(sigc::hide(sigc::mem_fun(this, &Gtk::Widget::hide)));
    set_secondary_text("Some changes occured to NoteKit since the last time you used it. Here is a quick round up of them:");

    std::vector<ChangeLogRelease> changelog = load_changelog();
    for (auto release : changelog) {
        // TODO: I guess I'll have to clean those pointer up?
        auto description = manage(new Gtk::Label(release.description));
        description->set_use_markup(true);
        description->set_max_width_chars(48);
        description->set_line_wrap(true);

        auto expander = manage(new Gtk::Expander());
        expander->set_label(release.version);
        expander->add(*description);

        inner.pack_end(*expander);
    }

    sv.add(inner);
    sv.set_max_content_height(256);
    sv.set_propagate_natural_height(true);
    sv.show_all();
    Gtk::Box* node = get_message_area();
    node->pack_end(sv);
}

std::vector<ChangeLogRelease> ChangeLog::load_changelog() {
    tinyxml2::XMLDocument metainfo;
    /*metainfo.LoadFile("/home/admin/workspace/apps/notekit/freedesktop/com.github.blackhole89.notekit.metainfo.xml");
    tinyxml2::XMLNode* changelogs = metainfo.FirstChildElement("component")->FirstChildElement("releases");

    std::vector<ChangeLogRelease> releases;
    for(tinyxml2::XMLElement* release = changelogs->FirstChildElement(); release != NULL; release = release->NextSiblingElement()) {
        tinyxml2::XMLPrinter printer;
        for (tinyxml2::XMLElement* rinner = release->FirstChildElement("description")->FirstChildElement(); rinner != NULL; rinner = rinner->NextSiblingElement()) {
            rinner->Accept(&printer);
        }

        releases.push_back(ChangeLogRelease(release->Attribute("version"), release->Attribute("date"), printer.CStr()));
    }*/
    metainfo.LoadFile("/home/admin/workspace/apps/notekit/data/changelog.xml");
    tinyxml2::XMLNode* changelogs = metainfo.FirstChildElement("releases");
    std::vector<ChangeLogRelease> releases;
    for(tinyxml2::XMLElement* release = changelogs->FirstChildElement(); release != NULL; release = release->NextSiblingElement()) {
        printf("Handle %s\n", release->Attribute("version"));
        tinyxml2::XMLPrinter printer;
        release->FirstChildElement("markup")->Accept(&printer);
        printf("Printer: %s\n", printer.CStr());
        releases.push_back(ChangeLogRelease(release->Attribute("version"), release->Attribute("date"), printer.CStr()));
    }

    return releases;
}
