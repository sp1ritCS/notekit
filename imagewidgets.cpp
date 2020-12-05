#include "imagewidgets.h"

/* store the Gdk::Window so we can create accelerated surfaces for whatever app is rendering on */
CImageWidget::CImageWidget(Glib::RefPtr<Gdk::Window> wnd) : Glib::ObjectBase("CImageWidget"), Gtk::DrawingArea()
{
	target_window=wnd;
	w=h=1;
	
	signal_draw().connect(sigc::mem_fun(this,&CImageWidget::on_draw));
	
	//set_size_request(1,1);
}

void CImageWidget::on_unrealize()
{
	printf("unrealize event on CImageWidget %08lX\n", (unsigned long) this);
}

void CImageWidget::destroy_notify_()
{
	printf("destroy event on CImageWidget %08lX\n", (unsigned long) this);
}

void CImageWidget::SetSize(int x, int y)
{
	w=x; h=y;
	
	image = target_window->create_similar_surface(Cairo::CONTENT_COLOR_ALPHA,w,h);
	image_ctx = Cairo::Context::create(image);
	
	set_size_request(w,h);
}

void CImageWidget::Redraw()
{
	
}

int CImageWidget::GetBaseline()
{
	return 0;
}

bool CImageWidget::on_draw(const Cairo::RefPtr<Cairo::Context> &ctx)
{
	if(!image) Redraw();
	/* blit our internal buffer */
	ctx->set_source(image,0,0);
	ctx->rectangle(0,0,w,h);
	ctx->fill();
	
	return true;
}

CImageWidget::~CImageWidget()
{
}

#ifdef HAVE_LASEM

#include <lsm.h>
#include <lsmmathml.h>

CLatexWidget::CLatexWidget(Glib::RefPtr<Gdk::Window> wnd, Glib::ustring text, Gdk::RGBA fg) : CImageWidget(wnd)
{
	source=text;
	
	LsmDomDocument *doc;
	LsmDomView *view;
	GError *err = NULL;
	doc = LSM_DOM_DOCUMENT (lsm_mathml_document_new_from_itex(source.c_str(),source.length(),&err));
	
	if(!err) {
		view = lsm_dom_document_create_view (doc);
		lsm_dom_view_set_resolution (view, target_window->get_screen()->get_resolution());
		
		lsm_dom_view_get_size_pixels (view, (unsigned int*)&w, (unsigned int*)&h, (unsigned int*)&baseline);
		SetSize(w,h);
		lsm_dom_view_render(view, image_ctx->cobj(), 0, 0);
		
		g_object_unref(view);
		g_object_unref(doc);
	} else {
		SetSize(128,16);
		image_ctx->set_source_rgb(1,0,0);
		image_ctx->set_font_size(10);
		image_ctx->move_to(0,14);
		image_ctx->show_text(err->message);
		image_ctx->fill(); //paint();
		g_error_free(err);
	}
}

CLatexWidget::~CLatexWidget()
{
}

void CLatexWidget::Redraw()
{
	
}

int CLatexWidget::GetBaseline()
{
	return baseline;
}

#endif

#ifdef HAVE_CLATEXMATH

#define __OS_Linux__
#define BUILD_GTK

#include "latex.h"
#include "platform/cairo/graphic_cairo.h"

using namespace tex;

CLatexWidget::CLatexWidget(Glib::RefPtr<Gdk::Window> wnd, Glib::ustring text, Gdk::RGBA fg) : CImageWidget(wnd)
{	
	source=text;
	unsigned int clr;
	clr=0xff000000|int(fg.get_red()*255)<<16|int(fg.get_green()*255)<<8|int(fg.get_blue()*255);
	
	try {
		TeXRender *r;
		r = LaTeX::parse(utf82wide(text.c_str()),
			wnd->get_width()-64, 
			18,
			18 / 3.f,
			clr);
			
		float h = r->getHeight(), w = r->getWidth();
			
		SetSize(w+4,h+2);
		
		baseline=(int)(round((2.5+(h)*(1.0f-r->getBaseline()))));
		
		// workaround for not entirely correct baseline arithmetic
		if(h<12) baseline-=1;
		
		Graphics2D_cairo g2(image_ctx);
		r->draw(g2,2,1);
		
		if(r) delete r;
	} catch(tex::ex_parse &e) {
		printf("LaTeX parsing error: %s\n",e.what());
		SetSize(128,16);
		baseline=0;
		
		image_ctx->set_source_rgb(1,0,0);
		image_ctx->set_font_size(10);
		image_ctx->move_to(0,14);
		image_ctx->show_text("LaTeX parse failed");
		
		image_ctx->fill();
	} catch(tex::ex_invalid_state &e) {
		printf("LaTeX rendering error: %s\n",e.what());
		SetSize(128,16);
		baseline=0;
		
		image_ctx->set_source_rgb(1,0,0);
		image_ctx->set_font_size(10);
		image_ctx->move_to(0,14);
		image_ctx->show_text("LaTeX render failed");
		
		image_ctx->fill();
	}
}

CLatexWidget::~CLatexWidget()
{
}

void CLatexWidget::Redraw()
{
	
}

int CLatexWidget::GetBaseline()
{
	return baseline;
}

#endif

CMimeWidget::CMimeWidget(Glib::RefPtr<Gdk::Window> wnd, Glib::ustring match) : CImageWidget(wnd) {
    std::filesystem::path path{match.substr(2, match.length() - 3)};
    bool exists = std::filesystem::exists(path);
    std::string filename = exists && path.has_filename() ? path.filename().u8string() : "Invalid File";
    SetSize(0,64);
	baseline=26;

    image_ctx->select_font_face("sans-serif", Cairo::FONT_SLANT_NORMAL, Cairo::FONT_WEIGHT_BOLD);
	image_ctx->set_font_size(32);
    Cairo::TextExtents stre;
	image_ctx->get_text_extents(filename, stre);
    float potx = (8*2 + 48) + stre.width + 12;
    SetSize(potx,64);
	image_ctx->set_source_rgb(0, 0, 0);
	image_ctx->rectangle(8, 7, 48, 48);
	//image_ctx->rectangle(0, 0, w-1, 63); //Debug only?
	/*image_ctx->move_to(0, h/2);
	image_ctx->line_to(w, h/2);
	image_ctx->close_path();*/
	image_ctx->stroke();
	if (exists) {
	    image_ctx->set_source_rgb(0.24,0.24,0.24);
	} else {
	    image_ctx->set_source_rgb(0.901960784,0.223529412,0.274509804);
	}
	image_ctx->move_to(48+8*2, h/2 + stre.height/3);
	image_ctx->select_font_face("sans-serif", Cairo::FONT_SLANT_NORMAL, Cairo::FONT_WEIGHT_NORMAL);
	image_ctx->set_font_size(32);
	//printf("Height: %d, TextH: %f, TextYB: %f, TextYA: %f\n", h, stre.height, stre.y_bearing, stre.y_advance);
	image_ctx->show_text(filename);
	image_ctx->fill();
}
CMimeWidget::~CMimeWidget()
{
}

void CMimeWidget::Redraw()
{

}

int CMimeWidget::GetBaseline()
{
	return baseline;
}
