#ifndef __STOCK_TAB_H__
#define __STOCK_TAB_H__

#include "lvgl.h"
#include <cstdint>
#include <string>
#include <map>
#include <vector>
#include <array>
#include "smart_fridge.h"

class StockTab {
    public:
        lv_obj_t *m_parent_tab;
        std::string m_name;
        // std::map<std::string, StockItem> m_stock;

    private:
        lv_obj_t *m_flex_col;
        int32_t m_location_id;

        // std::map<std::string, std::array<lv_obj_t*, 5>> m_rows;
        // std::vector<lv_obj_t *> m_spinboxes;
        // lv_obj_t * m_kb;
        //
        // lv_obj_t *m_item_add_btn;
        // lv_obj_t *m_new_item_form;
        //
        static constexpr uint16_t row_height = 45;

        static void spinbox_incr_cb(lv_event_t *e);
        static void spinbox_decr_cb(lv_event_t *e);
        // static void add_item_cb(lv_event_t *e);
        // static void text_write_cb(lv_event_t * e);
        // void create_new_item_form();

    public:
        // StockTab(lv_obj_t *parent, std::string name, uint32_t location_id);
        StockTab() : m_parent_tab(nullptr), m_name(""), m_location_id(-1){}
        StockTab(lv_obj_t *parent, std::string name, uint32_t location_id) : m_parent_tab(parent), m_name(name), m_location_id(location_id){
        /* Create left to right flex grid */
        /* With the following layout */
        /* | name      | - <q> + | */
        /* Elements added need to respect the width in order for layout to work */
         
        m_flex_col = lv_obj_create(m_parent_tab);
        lv_obj_set_size(m_flex_col, LV_PCT(100), LV_PCT(100));
        lv_obj_align(m_flex_col, LV_ALIGN_OUT_BOTTOM_MID, 0, 5);
        lv_obj_set_flex_flow(m_flex_col, LV_FLEX_FLOW_ROW_WRAP);

        // create_add_item_button();

        }

        void add_product_to_list(StockedProduct *item);
        // void add_product_to_stock(std::string name, uint32_t product_id, std::set<std::string> barcodes = {}, float amount = 0.0);
        // // void add_product_to_stock(Product product, float amount = 0.0);
        //
        // void create_add_item_button();
        // void delete_item(std::string item);
        // void delete_items();
        // void delete_consumed_items();
        //
        // void redraw_stock();
};

#endif // !__STOCK_TAB_H__


