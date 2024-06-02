#include "app.h"
#include "smart_fridge.h"

extern std::map<uint32_t, Location> locs_map;
extern std::map<uint32_t, Product> prods_map;

App::App(){
}


void App::add_location(const char *name, uint32_t location_id){
    lv_obj_t *tab = lv_tabview_add_tab(m_tabview, name);
    m_locations[location_id] = StockTab(tab, name, location_id);
    // m_locations[location_id] = (new StockTab(tab, name, location_id));
    // return m_locations[m_locations.size()-1];
}

void App::create_app(){

    /*Create a Tab view object*/
    // lv_obj_t* win = lv_win_create(lv_scr_act(), NULL);
    m_tabview = lv_tabview_create(lv_scr_act(), LV_DIR_TOP, 40);
 
    for(auto loc : locs_map){
        ESP_LOGI("SMART_FRIDGE", "Location: %s", loc.second.get_name().c_str());
        add_location(loc.second.get_name().c_str(), loc.second.get_id());

        for(auto item : loc.second.m_stock){
            add_product_to_stock(&(item.second));
        }
    }
    // Add barcode scanner button
    // lv_obj_t * btn1 = lv_btn_create(lv_scr_act());     /*Create a button on the screen*/
    // LV_IMG_DECLARE(barcode_scan)
    // lv_img_set_src(btn1, &barcode_scan);
    //
    // lv_obj_t * img1 = lv_img_create(btn1);
    // lv_img_set_src(img1, &barcode_scan);
    // lv_obj_align(img1, LV_ALIGN_CENTER, 0, 0);

    //
    // // lv_button_set_fit(btn1, true, true);               /*Enable automatically setting the size according to content*/
    // lv_obj_set_pos(btn1, LV_HOR_RES-80, LV_VER_RES-60);                      /*Set the position of the button*/
    //
    // // Callbacks
    // lv_obj_add_event_cb(btn1, barcode_button_pressed, LV_EVENT_ALL, this);
    // lv_obj_add_event_cb(m_tabview, tab_change, LV_EVENT_VALUE_CHANGED, this);
}

void App::add_product_to_stock(StockedProduct *item){
    // StockedProduct prod(Product(item, product_id), product_id, {}, amount);
    // locs_map[item->get_location_id()]->add_product_to_stock(item, product_id, {}, amount);
    m_locations[item->get_location_id()].add_product_to_list(item);
    // m_locations[location_id]->add_product_to_list(item, amount);
}
