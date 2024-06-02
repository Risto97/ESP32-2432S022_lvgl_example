#ifndef __APP_H__
#define __APP_H__

#include "lvgl.h"
#include <string>
#include <map>
#include "stock_tab.h"


class App {
    private:

        // struct barcode_confirm_t {
        //     lv_obj_t * barcode_text;
        //     lv_obj_t * mode_switch;
        // };

        lv_obj_t * m_tabview;
        std::map<uint32_t, StockTab> m_locations;
        // lv_obj_t *m_barcode_popup;
        //
        // barcode_confirm_t m_barcode_confirm_data;

        // static void barcode_button_pressed(lv_event_t *e);
        // static void barcode_switch_toggled(lv_event_t *e);
        // static void barcode_popup_close(lv_event_t *e);
        // static void barcode_confirm(lv_event_t *e);
        // static void tab_change(lv_event_t *e);

    public:
        App();
        ~App(){
            // m_consume_add_txt = "Consume";
            // TODO delete all stock tabs
        }

        void create_app();
        void add_location(const char *name, uint32_t location_id);
        // void delete_consumed_items();
        // std::string get_location_name(uint32_t id);
        void add_product_to_stock(StockedProduct *item);
        //
        // void redraw_stock();

};

#endif // !__APP_H__
