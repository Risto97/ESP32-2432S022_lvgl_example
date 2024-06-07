#include "stock_tab.h"
#include "esp_log.h"
#include "smart_fridge.h"
#include "src/core/lv_event.h"
#include "src/extra/widgets/spinbox/lv_spinbox.h"
#include <cstdint>
#include <tuple>

extern Grocy *g_grocy;

void StockTab::spinbox_incr_cb(lv_event_t *e){
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_SHORT_CLICKED || code == LV_EVENT_LONG_PRESSED_REPEAT) {
        auto item = (StockedProduct *)lv_event_get_user_data(e);
        // lv_obj_t * item = (lv_obj_t *)lv_event_get_user_data(e);
        lv_obj_t *sp_box = item->m_gui_elements[3];
        lv_spinbox_increment(sp_box);

        float val = (double)lv_spinbox_get_value(sp_box) / 10;
        // g_grocy->add_item(item->get_product_id(), step);
        g_grocy->inventory_item(item->get_product_id(), item->get_location_id(), val);

    }

}
void StockTab::spinbox_decr_cb(lv_event_t *e){
    lv_event_code_t code = lv_event_get_code(e);
    if(code == LV_EVENT_SHORT_CLICKED || code == LV_EVENT_LONG_PRESSED_REPEAT) {
        auto item = (StockedProduct *)lv_event_get_user_data(e);
        lv_obj_t *sp_box = item->m_gui_elements[3];
        lv_spinbox_decrement(sp_box);

        float val = (double)lv_spinbox_get_value(sp_box) / 10;
        g_grocy->inventory_item(item->get_product_id(), item->get_location_id(), val);
    }
}

// void StockTab::add_item_cb(lv_event_t *e){
//     lv_event_code_t code = lv_event_get_code(e);
//     if(code == LV_EVENT_SHORT_CLICKED || code == LV_EVENT_LONG_PRESSED_REPEAT) {
//         StockTab *self = (StockTab *)lv_event_get_user_data(e);
//         lv_obj_delete(self->m_item_add_btn);
//         self->create_new_item_form();
//     }
// }
//
// void StockTab::text_write_cb(lv_event_t * e)
// {
//     lv_event_code_t code = lv_event_get_code(e);
//     lv_obj_t * ta = (lv_obj_t *)lv_event_get_target(e);
//     if(code == LV_EVENT_CLICKED || code == LV_EVENT_FOCUSED) {
//         /*Focus on the clicked text area*/
//         StockTab *self = (StockTab *)lv_event_get_user_data(e);
//         if(self->m_kb != NULL) lv_keyboard_set_textarea(self->m_kb, ta);
//     }
//
//     else if(code == LV_EVENT_READY) {
//         StockTab *self = (StockTab *)lv_event_get_user_data(e);
//         lv_obj_move_background(self->m_kb);
//         StockItem item(lv_textarea_get_text(ta), 0, {}, 1.0 );
//         self->add_product_to_list(&item); // TODO !!!!!
//         if(lv_obj_is_valid(self->m_new_item_form))
//             lv_obj_delete(self->m_new_item_form);
//     }
// }
//
//
//
// void StockTab::add_product_to_list(StockItem *item){
//     if(lv_obj_is_valid(m_item_add_btn))
//         lv_obj_delete(m_item_add_btn);
//     // if(m_item_add_btn)
//
//     /* Label for the name of the product */
//     lv_obj_t * name_col_obj = lv_obj_create(m_flex_col);
//     lv_obj_set_size(name_col_obj, LV_PCT(60), row_height);
//
//     lv_obj_t * name_label = lv_label_create(name_col_obj);
//     lv_label_set_text_fmt(name_label, item->get_name().c_str());
//     lv_obj_center(name_label);
//
//     /* Decrement button */
//     lv_obj_t * decr_btn = lv_button_create(m_flex_col);
//     lv_obj_set_size(decr_btn, LV_PCT(7), row_height);
//     lv_obj_align_to(decr_btn, m_flex_col, LV_ALIGN_RIGHT_MID, 0, 0);
//     lv_obj_set_style_bg_image_src(decr_btn, LV_SYMBOL_MINUS, 0);
//
//     int32_t h = lv_obj_get_height(decr_btn);
//
//     /* Box with the quantity, incremented by incr and decr buttons */
//     lv_obj_t *spinbox = lv_spinbox_create(m_flex_col);
//     m_spinboxes.push_back(spinbox);
//
//     lv_spinbox_set_range(spinbox, 0, 1000);
//     lv_spinbox_set_digit_format(spinbox, 3, 2);
//     lv_spinbox_step_prev(spinbox);
//     lv_spinbox_set_value(spinbox, item->get_amount()*10);
//     // lv_obj_set_width(spinbox, row_height);
//     lv_obj_set_size(spinbox, LV_PCT(17), row_height);
//     lv_obj_align_to(spinbox, m_flex_col, LV_ALIGN_RIGHT_MID, 0, 0);
//
//     /* Increment button */
//     lv_obj_t * incr_btn = lv_button_create(m_flex_col);
//     lv_obj_set_size(incr_btn, LV_PCT(7), row_height);
//     lv_obj_align_to(decr_btn, spinbox, LV_ALIGN_OUT_LEFT_MID, -5, 0);
//     lv_obj_set_style_bg_image_src(incr_btn, LV_SYMBOL_PLUS, 0);
//
//     m_rows[item->get_name()] = {{name_col_obj, name_label, decr_btn, spinbox, incr_btn}};
//     item->m_gui_elements = {{name_col_obj, name_label, decr_btn, spinbox, incr_btn}};
//     // Callbacks
//     lv_obj_add_event_cb(incr_btn, spinbox_incr_cb, LV_EVENT_ALL, item);
//     lv_obj_add_event_cb(decr_btn, spinbox_decr_cb, LV_EVENT_ALL, item);
//
//
//     create_add_item_button();
// }
//
// void StockTab::add_product_to_stock(std::string name, uint32_t product_id, std::set<std::string> barcodes, float amount){
//     StockItem item(name, product_id, barcodes, amount);
//
//     if(m_stock.count(name)){
//         m_stock[name].purchase(item);
//
//     } else {
//         m_stock[name] = StockItem(name, product_id, barcodes, amount);;
//     }
//
// }
//
// // void StockTab::add_item(std::string name, uint32_t product_id, std::set<std::string> barcodes, float amount){
// // }
//
// void StockTab::create_add_item_button(){
//     // m_item_add_btn = lv_button_create(m_flex_col);
//     // lv_obj_set_size(m_item_add_btn, LV_PCT(100), row_height);
//     // lv_obj_align_to(m_item_add_btn, m_flex_col, LV_ALIGN_CENTER, 0, 0);
//     // lv_obj_set_style_bg_image_src(m_item_add_btn, LV_SYMBOL_PLUS, 0);
//
//     // lv_obj_add_event_cb(m_item_add_btn, add_item_cb, LV_EVENT_ALL, this);
//
// }
//
// void StockTab::create_new_item_form(){
//     /*Create the one-line mode text area*/
//     m_new_item_form = lv_textarea_create(m_flex_col);
//     lv_textarea_set_one_line(m_new_item_form, true);
//     lv_textarea_set_password_mode(m_new_item_form, false);
//     lv_obj_set_width(m_new_item_form, lv_pct(60));
//     lv_obj_add_event_cb(m_new_item_form, text_write_cb, LV_EVENT_ALL, this);
//     lv_obj_align(m_new_item_form, LV_ALIGN_CENTER, 0, 0);
//
//      /*Create a keyboard*/
//     m_kb = lv_keyboard_create(lv_screen_active());
//     lv_obj_set_size(m_kb,  LV_HOR_RES, LV_VER_RES / 2);
//
//     lv_keyboard_set_textarea(m_kb, m_new_item_form); /*Focus it on one of the text areas to start*/
//
// }
//
// void StockTab::delete_item(std::string item){
//     for(auto el : m_rows[item])
//         lv_obj_delete(el);
//
//     m_rows.erase(item);
// }
//
// void StockTab::delete_consumed_items(){
//     for(auto &[item, val]: m_rows){
//         if(lv_spinbox_get_value(val[3]) == 0)
//             delete_item(item);
//     }
// }
//
// void StockTab::delete_items(){
//     for(auto row : m_rows){
//         for(auto el : row.second)
//             lv_obj_delete(el);
//     }
// }
//
//
// void StockTab::redraw_stock(){
//     delete_items();
//
//     for( decltype(m_stock)::iterator it = m_stock.begin(); it != m_stock.end(); ++it ){
//         add_product_to_list(&it->second);
//     }
//
// }

void StockTab::add_product_to_list(StockedProduct *item){
    // if(lv_obj_is_valid(m_item_add_btn))
    //     lv_obj_delete(m_item_add_btn);

    /* Label for the name of the product */
    lv_obj_t * name_col_obj = lv_obj_create(m_flex_col);
    lv_obj_set_size(name_col_obj, LV_PCT(53), row_height);

    lv_obj_t * name_label = lv_label_create(name_col_obj);
    lv_label_set_text_fmt(name_label, item->get_name().c_str());
    lv_obj_center(name_label);

    /* Decrement button */
    lv_obj_t * decr_btn = lv_btn_create(m_flex_col);
    lv_obj_set_size(decr_btn, LV_PCT(10), row_height);
    lv_obj_align_to(decr_btn, m_flex_col, LV_ALIGN_RIGHT_MID, 0, 0);
    lv_obj_set_style_bg_img_src(decr_btn, LV_SYMBOL_MINUS, 0);

    int32_t h = lv_obj_get_height(decr_btn);

    /* Box with the quantity, incremented by incr and decr buttons */
    lv_obj_t *spinbox = lv_spinbox_create(m_flex_col);
    // m_spinboxes.push_back(spinbox);

    lv_spinbox_set_range(spinbox, 0, 1000);
    lv_spinbox_set_digit_format(spinbox, 3, 2);
    lv_spinbox_step_prev(spinbox);
    lv_spinbox_set_value(spinbox, item->get_amount()*10);
    // // lv_obj_set_width(spinbox, row_height);
    lv_obj_set_size(spinbox, LV_PCT(17), row_height);
    lv_obj_align_to(spinbox, m_flex_col, LV_ALIGN_RIGHT_MID, 0, 0);
    //
    // /* Increment button */
    lv_obj_t * incr_btn = lv_btn_create(m_flex_col);
    lv_obj_set_size(incr_btn, LV_PCT(10), row_height);
    lv_obj_align_to(decr_btn, spinbox, LV_ALIGN_OUT_LEFT_MID, -5, 0);
    lv_obj_set_style_bg_img_src(incr_btn, LV_SYMBOL_PLUS, 0);

    // m_rows[item->get_name()] = {{name_col_obj, name_label, decr_btn, spinbox, incr_btn}};
    item->m_gui_elements = {{name_col_obj, name_label, decr_btn, spinbox, incr_btn}};
    // Callbacks
    lv_obj_add_event_cb(incr_btn, spinbox_incr_cb, LV_EVENT_ALL, item);
    lv_obj_add_event_cb(decr_btn, spinbox_decr_cb, LV_EVENT_ALL, item);

    // create_add_item_button();
}
