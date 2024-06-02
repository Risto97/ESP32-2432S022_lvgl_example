#ifndef __SMART_FRIDGE_H_
#define __SMART_FRIDGE_H_

#include "esp_log.h"
#include <map>
#include <set>
#include <stdint.h>
#include <string>
#include <cstdint>
#include "lvgl.h"

class Product {
    public:
        Product(const char * _name, uint32_t _id) : m_name(_name), m_id(_id){
        }

        Product() : m_name(""), m_id(-1){
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

        void add_product(StockedProduct prod){
            if(m_stock.count(prod.get_product_id()) > 0){
                m_stock[prod.get_product_id()].add_amount(prod.get_amount());
            } else {
                m_stock.insert(std::make_pair(prod.get_product_id(), prod));
            }
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

#endif // !__SMART_FRIDGE_H_



