#include <TimerOne.h>
#include "ThreadController.h"
#include <EveryTimer.h>
#include <U8g2lib.h>
#include <RotaryEncoder.h>
#include <AceButton.h>
using namespace ace_button;

#include <MenuSystem.h>

#include "MeasurementThread.h"
#include "GraphDrawer.h"
#include "Value.h"
#include "GFXMenuRenderer.h"

#include "pm_strings.h"

#define INPUT_PIN       A1
#define OUTPUT_PIN      9
#define BUZZER          10
#define ENCODER_BUTTON  11

#define CHARGE_USEC 170
#define DELAY_USEC  8

#define CV_Y 2
#define CV_H 32

ThreadController controller = ThreadController();

EveryTimer timer;
U8G2_SSD1306_128X64_NONAME_1_HW_I2C display(U8G2_R0, U8X8_PIN_NONE);

AceButton encoder_button;
RotaryEncoder encoder(A2, A3);

MeasurementThread measurement = MeasurementThread(INPUT_PIN, OUTPUT_PIN, 20);
GraphDrawer graph = GraphDrawer(&display, 62, CV_Y, 62, CV_H, &measurement);

class ValueMenuItem: public MenuItem {
  public:
    const void* value;
    ValueMenuItem(const char* name, SelectFnPtr select_fn, void* v): MenuItem(name, select_fn), value(v) {}
};

GFXMenuRenderer my_renderer(&display);

MenuSystem ms(my_renderer);

ValueMenuItem *selected_value_mi = NULL;

void on_value_selected(MenuComponent* p_menu_component) {
  selected_value_mi = (ValueMenuItem*)p_menu_component;
  ms.reset();
  ms.display();
}

void on_reset_n_selected(MenuComponent*) {
  ms.back();
  ms.display();
}

void on_reset_y_selected(MenuComponent*) {
  //  for (uint8_t i = 0 ; i < EEPROM.length() ; ++i) {
  //    EEPROM.write(i, 0);
  //  }
}

void on_calibrate_selected(MenuComponent*) {
  measurement.calibrate();
}

void on_decalibrate_selected(MenuComponent*) {
  measurement.decalibrate();
}

Value<word> value_charge = Value<word>(measurement.charge_time_us, CHARGE_USEC, 10, 5000, 10);
Value<word> value_delay = Value<word>(measurement.delay_time_us, DELAY_USEC, 1, 1000, 1);
Value<int> value_trigger = Value<int>(measurement.trigger, 1, 0, 1024, 1);

Menu mu_params(m_params) PROGMEM;
Menu mu_settings(m_settings);
Menu mu_reset(m_reset);

MenuItem mi_calibrate(m_calibrate, &on_calibrate_selected);
MenuItem mi_decalibrate(m_decalibrate, &on_decalibrate_selected);
ValueMenuItem mi_charge(m_charge, &on_value_selected, &value_charge);
ValueMenuItem mi_delay(m_delay, &on_value_selected, &value_delay);
ValueMenuItem mi_trigger(m_trigger, &on_value_selected, &value_trigger);

MenuItem mi_brightness(m_contrast, NULL);

MenuItem mi_reset_n(m_no, &on_reset_n_selected);
MenuItem mi_reset_y(m_yes, &on_reset_y_selected);

ButtonConfig e_button_cfg;

void setup() {
  // pins

  pinMode(ENCODER_BUTTON, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);
  pinMode(8, OUTPUT);
  // configs

  ms.get_root_menu().set_name(m_main);
  ms.get_root_menu().add_menu(&mu_params);
  mu_params.add_item(&mi_charge);
  mu_params.add_item(&mi_delay);
  mu_params.add_item(&mi_trigger);
  mu_params.add_item(&mi_calibrate);
  mu_params.add_item(&mi_decalibrate);
  ms.get_root_menu().add_menu(&mu_settings);
  mu_settings.add_item(&mi_brightness);
  mu_settings.add_menu(&mu_reset);
  mu_reset.add_item(&mi_reset_n);
  mu_reset.add_item(&mi_reset_y);

  PCICR |= (1 << PCIE1), PCMSK1 |= (1 << PCINT10) | (1 << PCINT11);

  e_button_cfg.setEventHandler(handleEvent);
  e_button_cfg.setFeature(ButtonConfig::kFeatureLongPress);
  e_button_cfg.setFeature(ButtonConfig::kFeatureSuppressAfterLongPress);
  encoder_button.setButtonConfig(&e_button_cfg);

  Timer1.initialize(10000);
  Timer1.attachInterrupt(timerCallback);

  measurement.setInterval(5);
  controller.add(&measurement);

  // start
  display.begin();
  display.enableUTF8Print();
  //  display.setFontMode(1); // transparent
  display.setFontPosTop();
  display.setFont(u8g2_font_5x8_t_cyrillic);

  encoder_button.init(ENCODER_BUTTON, HIGH, 0);
  Timer1.start(); // threads
  timer.Every(1000 / 10, draw);
}

ISR(PCINT1_vect) {
  encoder.tick();
}

void timerCallback() {
  controller.run();
}

void buzzer_click(byte ms = 1) {
  digitalWrite(BUZZER, HIGH), delay(ms), digitalWrite(BUZZER, LOW), delay(ms);
}

void draw() {
  int t = measurement.trigger;
  int md = 1024;
  for (byte i = 0; i < measurement.count; ++i) {
    md = min(md, abs(measurement.values[i] - t));
  }
  if(md < 32) tone(8, 1024 - md * 32, 5);
  graph.update();
  display.firstPage();
  do {
    if (NULL == selected_value_mi) {
      ms.display();
    } else {
      Value<word>* value = (Value<word>*)selected_value_mi->value;
      display.home();
      display.print((class __FlashStringHelper *)(selected_value_mi->get_name()));
      display.print(F(":"));
      display.setCursor(0, 10);
      display.println(value->val);
    }

    graph.draw();
  } while ( display.nextPage() );
}

void handleEvent(AceButton*, uint8_t eventType, uint8_t) {
  switch (eventType) {
    case AceButton::kEventReleased:
      if (NULL == selected_value_mi) {
        ms.select();
        ms.display();
        break;
      }
      selected_value_mi = NULL;
      break;
    case AceButton::kEventLongPressed:
      ms.back();
      ms.display();
      break;
  }
}

void loop() {
  encoder_button.check();
  int encoder_position = encoder.getPosition();

  if (NULL == selected_value_mi) {
    if (encoder_position > 0) {
      ms.prev();
      ms.display();
    }
    if (encoder_position < 0) {
      ms.next();
      ms.display();
    }
  } else { // Value menu item selected
    if (0 != encoder_position) {
      Value<word>* value = (Value<word>*)selected_value_mi->value;
      value->change(encoder_position);
    }
  }

  if (encoder_position != 0) encoder.setPosition(0);
  timer.Update();
}


