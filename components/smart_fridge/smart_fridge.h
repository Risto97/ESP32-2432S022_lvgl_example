#ifndef __SMART_FRIDGE_H_
#define __SMART_FRIDGE_H_

#include "esp_log.h"
#include <map>
#include <set>
#include <stdint.h>
#include <string>
#include <cstdint>
#include "lvgl.h"
#include <array>

#include <freertos/FreeRTOS.h>
#include "freertos/queue.h"

class Product {
    public:
        Product(const char * _name, uint32_t _id) : m_name(_name), m_id(_id){
        }

        Product() : m_name("Unknown product"), m_id(-1){
        }

        void print(){
            ESP_LOGI("SMART_FRIDGE", "Product: %s, ID: %d", m_name.c_str(), m_id);
        }

        std::string get_name(){
            return m_name;
        }

        uint32_t get_product_id(){
            return m_id;
        }

        void add_barcode(std::string barcode){
            m_barcodes.insert(barcode);
        }

        std::set<std::string>& get_barcodes(){
            return m_barcodes;
        }

    private:
        std::string m_name;
        int m_id;
        std::set<std::string> m_barcodes;
};

class StockedProduct {
    public:
        StockedProduct(Product * _product, float _amount, uint32_t location_id) : m_product(_product), m_amount(_amount), m_location_id(location_id){
        }

        StockedProduct() : m_product(nullptr), m_amount(0.0), m_location_id(-1){
        }

        void print(){
            ESP_LOGI("SMART_FRIDGE", "StockedProduct: %s, amount: %f, barcodes: [", m_product->get_name().c_str(), m_amount);
            for(auto barcode : m_product->get_barcodes()){
                ESP_LOGI("SMART_FRIDGE", "%s, ", barcode.c_str());
            }
            ESP_LOGI("SMART_FRIDGE", "]");
        }

        uint32_t get_product_id(){
            return m_product->get_product_id();
        }

        std::string get_name(){
            return m_product->get_name();
        }


        void add_amount(float amount){
            m_amount += amount;
        }

        float get_amount(){
            return m_amount;
        }

        int32_t get_location_id(){
            return m_location_id;
        }

    public:
        std::array<lv_obj_t*, 5> m_gui_elements;

    private:
        Product *m_product;
        float m_amount;
        int32_t m_location_id;
};


class Location {
    public:
        Location(const char * _name, uint32_t _id) : m_name(_name), m_id(_id){
        }

        Location() : m_name(""), m_id(-1){
        }

        void print(){
            ESP_LOGI("SMART_FRIDGE", "Location: %s, ID: %d", m_name.c_str(), m_id);

            for(auto item : m_stock){
                item.second.print();
            }
        }

        StockedProduct *add_product(StockedProduct prod){
            if(m_stock.count(prod.get_product_id()) > 0){
                m_stock[prod.get_product_id()].add_amount(prod.get_amount());
            } else {
                m_stock.insert(std::make_pair(prod.get_product_id(), prod));
            }
            return &m_stock[prod.get_product_id()];
        }

        std::string get_name(){
            return m_name;
        }
        
        int get_id(){
            return m_id;
        }

    public:
        std::string m_name;
        int m_id;
        std::map<uint32_t, StockedProduct> m_stock;


};

class Grocy {
    public:

        enum class CmdType {
            add_item,
            consume_item,
            inventory_item,
            pull_stock,
            pull_locations,
            pull_products,
            pull_barcodes,
            pull_all
        };

        typedef struct  {

            CmdType type;
            struct {
                uint32_t product_id;
                uint32_t location_id;
                float amount;
            } item;
        } grocy_cmd_t;

        Grocy(QueueHandle_t *grocy_queue) {
            queue = grocy_queue;
        }

        void add_item(uint32_t product_id, float amount){
            grocy_cmd_t cmd;

            cmd.type = CmdType::add_item;
            cmd.item.product_id = product_id;
            cmd.item.amount = amount;


            if(xQueueSend(*queue, (void *)&cmd, 10) != pdTRUE){
                ESP_LOGE("GROCY API", "Cannot add command to queue");
            }
        }

        void consume_item(uint32_t product_id, float amount){
            grocy_cmd_t cmd;

            cmd.type = CmdType::consume_item;
            cmd.item.product_id = product_id;
            cmd.item.amount = amount;

            if(xQueueSend(*queue, (void *)&cmd, 10) != pdTRUE){
                ESP_LOGE("GROCY API", "Cannot add command to queue");
            }
        }

        void inventory_item(uint32_t product_id, uint32_t location_id, float amount){
            grocy_cmd_t cmd;

            cmd.type = CmdType::inventory_item;
            cmd.item.product_id = product_id;
            cmd.item.location_id = location_id;
            cmd.item.amount = amount;

            if(xQueueSend(*queue, (void *)&cmd, 10) != pdTRUE){
                ESP_LOGE("GROCY API", "Cannot add command to queue");
            }
        }

        void pull_stock(){
        }

        void pull_products(){
        }

        void pull_barcodes(){
        }

        void pull_all(){
        }


    private:
        QueueHandle_t *queue;
};

#endif // !__SMART_FRIDGE_H_



