#include "GFXMenuRenderer.h"

GFXMenuRenderer::GFXMenuRenderer(U8G2* gfx): MenuComponentRenderer() {
  display = gfx;
}
void GFXMenuRenderer::render(Menu const& menu) const {
  byte len = menu.get_num_components();
  MenuComponent const* mc;

  display->drawBox(0, 0, 60, 8);
  display->home();

  display->setDrawColor(0);
  display->print((class __FlashStringHelper *)menu.get_name());

  display->setDrawColor(1);

  for (byte i = 0; i < len; i++) {
    mc = menu.get_menu_component(i);
    if (mc->is_current()) display->drawStr(0, 10 + 10 * i, ">");
    display->setCursor(6, 10 + 10 * i);
    mc->render(*this);
  }
}

void GFXMenuRenderer::render_menu_item(MenuItem const& menu_item) const {
  display->print((class __FlashStringHelper *)menu_item.get_name());
}

void GFXMenuRenderer::render_back_menu_item(BackMenuItem const& menu_item) const {
  display->print((class __FlashStringHelper *)menu_item.get_name());
}

void GFXMenuRenderer::render_numeric_menu_item(NumericMenuItem const& menu_item) const {
  display->print((class __FlashStringHelper *)menu_item.get_name());
}

void GFXMenuRenderer::render_menu(Menu const& menu) const {
  display->print((class __FlashStringHelper *)menu.get_name());
}
