#include "app.h"
#include "smart_fridge.h"
#include "src/misc/lv_area.h"
#include "src/widgets/lv_label.h"
#include "src/widgets/lv_textarea.h"

extern std::map<uint32_t, Location> locs_map;
extern std::map<uint32_t, Product> prods_map;

// App::App(){
// }
extern Grocy *g_grocy;

void App::add_location(const char *name, uint32_t location_id){
    lv_obj_t *tab = lv_tabview_add_tab(m_tabview, name);
    m_locations[location_id] = StockTab(tab, name, location_id);
    m_tab_idx_to_loc_id_map[m_tab_cnt] = location_id;
    m_tab_cnt++;
}

void App::create_app(){

    /*Create a Tab view object*/
    // lv_obj_t* win = lv_win_create(lv_scr_act(), NULL);
    m_tabview = lv_tabview_create(lv_scr_act(), LV_DIR_TOP, 40);
 
    for(auto &loc : locs_map){
        ESP_LOGI("SMART_FRIDGE", "Location: %s", loc.second.get_name().c_str());
        add_location(loc.second.get_name().c_str(), loc.second.get_id());

        for(auto &[key, val] : loc.second.m_stock){
            add_product_to_stock(&val);
        }
    }
    // Add barcode scanner button
    lv_obj_t * btn1 = lv_btn_create(lv_scr_act());     /*Create a button on the screen*/
    LV_IMG_DECLARE(barcode_scan)
    // lv_img_set_src(btn1, &barcode_scan);

    lv_obj_t * img1 = lv_img_create(btn1);
    lv_img_set_src(img1, &barcode_scan);
    // lv_obj_set_size(img1, 50, 50);
    lv_obj_align(img1, LV_ALIGN_CENTER, 0, 0);

    //
    // lv_btn_set_fit(btn1, true, true);               /*Enable automatically setting the size according to content*/
    lv_obj_set_pos(btn1, LV_HOR_RES-80, LV_VER_RES-60);                      /*Set the position of the button*/
 
    // // Callbacks
    lv_obj_add_event_cb(btn1, barcode_button_pressed, LV_EVENT_ALL, this);
    // lv_obj_add_event_cb(m_tabview, tab_change, LV_EVENT_VALUE_CHANGED, this);
}

void App::add_product_to_stock(StockedProduct *item){
    // StockedProduct prod(Product(item, product_id), product_id, {}, amount);
    // locs_map[item->get_location_id()]->add_product_to_stock(item, product_id, {}, amount);
    m_locations[item->get_location_id()].add_product_to_list(item);
    // m_locations[location_id]->add_product_to_list(item, amount);
}

void App::barcode_popup_button_pressed_cb(lv_event_t *e){
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_VALUE_CHANGED) {
        App *self = (App *)lv_event_get_user_data(e);

        lv_obj_t * obj = lv_event_get_target(e);
        uint32_t btn_id = lv_btnmatrix_get_selected_btn(obj);
        
        if(btn_id == 0){ // Add
            uint32_t loc_id = self->get_active_location_id();
            StockedProduct prod(self->m_current_scanned_product, 1.0, loc_id);
            StockedProduct *new_prod = locs_map[loc_id].add_product(prod);
            g_grocy->inventory_item(new_prod->get_product_id(), new_prod->get_location_id(), new_prod->get_amount());
        }
        ESP_LOGI("BARCODE", "Button pressed: %d, Loc btn_id: %d", btn_id, self->get_active_location_id());
    }

}
void App::add_popup_barcode_text(std::string text){
    lv_table_set_cell_value(m_barcode_table, 0, 1, text.c_str());
}

void App::add_popup_scanned_product_name(Product *product){
    lv_table_set_cell_value(m_barcode_table, 1, 1, product->get_name().c_str());
    m_current_scanned_product = product;
}

Product* App::get_current_scanned_product(){
    return m_current_scanned_product;
}

int App::get_active_location_id(){
    return m_tab_idx_to_loc_id_map[lv_tabview_get_tab_act(m_tabview)];
    // return m_current_scanned_product;
}

void App::barcode_button_pressed(lv_event_t *e){
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_SHORT_CLICKED || code == LV_EVENT_LONG_PRESSED_REPEAT) {
        App *self = (App *)lv_event_get_user_data(e);

        lv_obj_t *active_tab = lv_obj_get_parent(lv_event_get_current_target(e));

        if(lv_obj_is_valid(self->m_barcode_popup))
            lv_obj_del(self->m_barcode_popup);

        static const char * btns[] = {""};
        lv_obj_t *popup = lv_msgbox_create(active_tab, "Scan items", "", btns, true);
        self->m_barcode_popup = popup;
        lv_obj_center(popup);

        lv_obj_t * table = lv_table_create(popup);
        self->m_barcode_table = table;
        lv_obj_set_size(table, LV_PCT(100), LV_SIZE_CONTENT);

        lv_table_set_col_width(table, 0, 75);
        lv_table_set_col_width(table, 1, 150);
        lv_table_set_row_cnt(table, 2);
        lv_table_set_col_cnt(table, 2);

        lv_obj_remove_style(table, NULL, LV_PART_ITEMS | LV_STATE_PRESSED);

        lv_table_set_cell_value(table, 0, 0, "Code:");
        lv_table_set_cell_value(table, 1, 0, "Name:");

        static const char * map[] = {"Add", "Consume", ""};
        lv_obj_t * btnm = lv_btnmatrix_create(popup);
        lv_obj_set_size(btnm, LV_PCT(100), 60);
        lv_btnmatrix_set_map(btnm, map);

        lv_obj_center(btnm);

        lv_obj_add_event_cb(btnm, barcode_popup_button_pressed_cb, LV_EVENT_VALUE_CHANGED, self);
        // lv_textarea_set_one_line(ta, true);
        // lv_obj_set_size(barcode_text, LV_PCT(70), lv_font_get_line_height(lv_font_default()) + 25);

        // lv_obj_t * confirm_btn;
        // confirm_btn = lv_msgbox_add_footer_button(popup, "Confirm");

        // self->m_barcode_confirm_data.mode_switch = sw;
        // self->m_barcode_confirm_data.barcode_text = ta;

        // lv_obj_add_event_cb(close_btn, self->barcode_popup_close, LV_EVENT_PRESSED, self);
        // lv_obj_add_event_cb(sw, barcode_switch_toggled, LV_EVENT_ALL, self);
        // lv_obj_add_event_cb(confirm_btn, barcode_confirm, LV_EVENT_PRESSED, &self->m_barcode_confirm_data);
    }

}
