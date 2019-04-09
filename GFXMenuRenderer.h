#ifndef GFX_MENU_RENDERER
#define GFX_MENU_RENDERER

#include <U8g2lib.h>
#include <MenuSystem.h>

class GFXMenuRenderer: public MenuComponentRenderer {
  public:
    GFXMenuRenderer(U8G2* gfx);
    void render(Menu const& menu) const;
    void render_menu_item(MenuItem const& menu_item) const;
    void render_back_menu_item(BackMenuItem const& menu_item) const;
    void render_numeric_menu_item(NumericMenuItem const& menu_item) const;
    void render_menu(Menu const& menu) const;
  private:
    U8G2* display;
};

#endif
