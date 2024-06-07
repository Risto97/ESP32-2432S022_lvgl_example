#include "http_utils.h"

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <string>
#include "freertos/FreeRTOS.h"
#include "freertos/projdefs.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

#include "cJSON.h"

#include <map>
#include "smart_fridge.h"

#define RECV_BUF_SIZE 2048

std::map<uint32_t, Location> locs_map;
std::map<uint32_t, Product> prods_map;

extern QueueHandle_t grocy_cmd_queue;
extern TaskHandle_t gui_task_handle;

char *recv_buf;

#define WEB_SERVER "192.168.0.18"
#define WEB_PORT "9283"

static const char *TAG = "http";

int grocy_get(std::string api_str, char *recv_buf, int len){

    const struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
    };
    struct addrinfo *res;
    struct in_addr *addr;
    int s, r;

    int err = getaddrinfo(WEB_SERVER, WEB_PORT, &hints, &res);

    if(err != 0 || res == NULL) {
        ESP_LOGE(TAG, "DNS lookup failed err=%d res=%p", err, res);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    /* Code to print the resolved IP.

       Note: inet_ntoa is non-reentrant, look at ipaddr_ntoa_r for "real" code */
    addr = &((struct sockaddr_in *)res->ai_addr)->sin_addr;

    s = socket(res->ai_family, res->ai_socktype, 0);
    if(s < 0) {
        ESP_LOGE(TAG, "... Failed to allocate socket.");
        freeaddrinfo(res);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    if(connect(s, res->ai_addr, res->ai_addrlen) != 0) {
        ESP_LOGE(TAG, "... socket connect failed errno=%d", errno);
        close(s);
        freeaddrinfo(res);
        vTaskDelay(4000 / portTICK_PERIOD_MS);
    }

    freeaddrinfo(res);

    std::string request_str = "GET " + api_str + " HTTP/1.0\r\n"
        "Host: "WEB_SERVER":"WEB_PORT"\r\n"
        "GROCY-API-KEY: c9jCoq6RH3ltQXHJFC05XqvNBpdkFnDO8zC632Q28kqY3CO6Nx\r\n"
        "\r\n";
    if (write(s, request_str.c_str(), request_str.size()) < 0) {
        ESP_LOGE(TAG, "... socket send failed");
        close(s);
        vTaskDelay(4000 / portTICK_PERIOD_MS);
    }

    struct timeval receiving_timeout;
    receiving_timeout.tv_sec = 5;
    receiving_timeout.tv_usec = 0;
    if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &receiving_timeout,
            sizeof(receiving_timeout)) < 0) {
        ESP_LOGE(TAG, "... failed to set socket receiving timeout");
        close(s);
        vTaskDelay(4000 / portTICK_PERIOD_MS);
    }

    /* Read HTTP response */
    // do {
        // bzero(recv_buf, len);
        r = read(s, recv_buf, len-1);
        // for(int i = 0; i < r; i++) {
        //     putchar(recv_buf[i]);
        // }
    // } while(r > 0);

    close(s);

    return r;
}

int grocy_post(std::string api_str, char *send_buf, int len){

    const struct addrinfo hints = {
        .ai_family = AF_INET,
        .ai_socktype = SOCK_STREAM,
    };
    struct addrinfo *res;
    struct in_addr *addr;
    int s, r;

    int err = getaddrinfo(WEB_SERVER, WEB_PORT, &hints, &res);

    if(err != 0 || res == NULL) {
        ESP_LOGE(TAG, "DNS lookup failed err=%d res=%p", err, res);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    /* Code to print the resolved IP.

       Note: inet_ntoa is non-reentrant, look at ipaddr_ntoa_r for "real" code */
    addr = &((struct sockaddr_in *)res->ai_addr)->sin_addr;

    s = socket(res->ai_family, res->ai_socktype, 0);
    if(s < 0) {
        ESP_LOGE(TAG, "... Failed to allocate socket.");
        freeaddrinfo(res);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }

    if(connect(s, res->ai_addr, res->ai_addrlen) != 0) {
        ESP_LOGE(TAG, "... socket connect failed errno=%d", errno);
        close(s);
        freeaddrinfo(res);
        vTaskDelay(4000 / portTICK_PERIOD_MS);
    }

    freeaddrinfo(res);

    std::string request_str = "POST " + api_str + " HTTP/1.0\r\n"
        "Host: "WEB_SERVER":"WEB_PORT"\r\n"
        "Content-Type: application/json\r\n"
        "Content-Length: " + std::to_string(len) + "\r\n"
        "GROCY-API-KEY: c9jCoq6RH3ltQXHJFC05XqvNBpdkFnDO8zC632Q28kqY3CO6Nx\r\n"
        "\r\n"
        + std::string(send_buf)
        ;

    ESP_LOGI("GROCY_POST", "%s", request_str.c_str());
    if (write(s, request_str.c_str(), request_str.size()) < 0) {
        ESP_LOGE(TAG, "... socket send failed");
        close(s);
        vTaskDelay(4000 / portTICK_PERIOD_MS);
    }

    struct timeval receiving_timeout;
    receiving_timeout.tv_sec = 5;
    receiving_timeout.tv_usec = 0;
    if (setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, &receiving_timeout,
            sizeof(receiving_timeout)) < 0) {
        ESP_LOGE(TAG, "... failed to set socket receiving timeout");
        close(s);
        vTaskDelay(4000 / portTICK_PERIOD_MS);
    }

    /* Read HTTP response */
    do {
        // bzero(recv_buf, len);
        r = read(s, recv_buf, 64);
        for(int i = 0; i < r; i++) {
            putchar(recv_buf[i]);
        }
    } while(r > 0);

    close(s);

    r = 0;
    return r;
}

bool parse_locations(cJSON *array){
    cJSON *item = array ? array->child : 0;
    if(!item)
        return false;
    while (item) {
        char *name = cJSON_GetObjectItem(item, "name")->valuestring;
        uint32_t id = cJSON_GetObjectItem(item, "id")->valueint;

        locs_map.insert(std::make_pair(id, Location(name, id)));
        item=item->next;
    }

    return true;
}

bool parse_products(cJSON *array){
    cJSON *item = array ? array->child : 0;
    if(!item)
        return false;
    while (item) {
        char* name = cJSON_GetObjectItem(item, "name")->valuestring;
        uint32_t id = cJSON_GetObjectItem(item, "id")->valueint;

        prods_map.insert(std::make_pair(id, Product(name, id)));

        item=item->next;
    }

    return true;
}

bool parse_barcodes(cJSON *array){
    cJSON *item = array ? array->child : 0;
    if(!item)
        return false;
    while (item) {
        char* barcode = cJSON_GetObjectItem(item, "barcode")->valuestring;
        uint32_t product_id = cJSON_GetObjectItem(item, "product_id")->valueint;

        prods_map[product_id].add_barcode(std::string(barcode));
        // prods_map.insert(std::make_pair(id, Product(name, id)));

        item=item->next;
    }

    return true;
}

bool parse_stock(cJSON *array){
    cJSON *item = array ? array->child : 0;
    if(!item)
        return false;
    while (item) {
        uint32_t product_id = cJSON_GetObjectItem(item, "product_id")->valueint;
        float amount = cJSON_GetObjectItem(item, "amount")->valuedouble;
        uint32_t location_id = cJSON_GetObjectItem(item, "location_id")->valueint;

        locs_map[location_id].add_product(StockedProduct(&prods_map[product_id], amount, location_id));
        item=item->next;
    }
    return true;
}


void get_locations(){
    int i = 0;
    bool end_of_list = false;
    do{
        int len = grocy_get("/api/objects/locations?limit=1&offset=" + std::to_string(i), recv_buf, RECV_BUF_SIZE);
        i +=1;

        recv_buf[len] = '\0';

        // Skip the HTTP header and select json data
        char *json_str = strstr(recv_buf, "\r\n\r\n");
        if(json_str)
            json_str += 4;

        cJSON *json = cJSON_Parse(json_str); 
        if (json == NULL) { 
            const char *error_ptr = cJSON_GetErrorPtr(); 
            if (error_ptr != NULL) { 
                ESP_LOGE("HTTP", "Error: %s\n", error_ptr); 
            } 
            cJSON_Delete(json); 
        } 
        if(!parse_locations(json))
            end_of_list = true;

        cJSON_Delete(json); 
    } while(!end_of_list);

}

void get_products(){
    int i = 0;
    // ESP_LOGI("PRODUCTS+++++++++", "Getting products");
    bool end_of_list = false;
    do{
        int len = grocy_get("/api/objects/products?limit=1&offset=" + std::to_string(i), recv_buf, RECV_BUF_SIZE);
        i += 1;
        recv_buf[len] = '\0';

        // Skip the HTTP header and select json data
        char *json_str = strstr(recv_buf, "\r\n\r\n");
        if(json_str)
            json_str += 4;

        cJSON *json = cJSON_Parse(json_str); 
        if (json == NULL) { 
            const char *error_ptr = cJSON_GetErrorPtr(); 
            if (error_ptr != NULL) { 
                ESP_LOGE("HTTP", "Error: %s\n", error_ptr); 
            } 
            cJSON_Delete(json); 
        } 
        if(!parse_products(json))
            end_of_list = true;

        cJSON_Delete(json); 
    } while(!end_of_list);
    // ESP_LOGI("PRODUCTS+++++++++", "Done getting products");

}

void get_barcodes(){
    int i = 0;
    bool end_of_list = false;
    do{
        int len = grocy_get("/api/objects/product_barcodes?limit=1&offset=" + std::to_string(i), recv_buf, RECV_BUF_SIZE);
        i += 1;
        recv_buf[len] = '\0';

        // Skip the HTTP header and select json data
        char *json_str = strstr(recv_buf, "\r\n\r\n");
        if(json_str)
            json_str += 4;

        cJSON *json = cJSON_Parse(json_str); 
        if (json == NULL) { 
            const char *error_ptr = cJSON_GetErrorPtr(); 
            if (error_ptr != NULL) { 
                ESP_LOGE("HTTP", "Error: %s\n", error_ptr); 
            } 
            cJSON_Delete(json); 
        } 
        if(!parse_barcodes(json))
            end_of_list = true;

        cJSON_Delete(json); 
    } while(!end_of_list);

}

void add_product_to_stock(uint32_t product_id, float amount){
    
    cJSON *json = cJSON_CreateObject();
    cJSON *amount_json = cJSON_CreateNumber(amount);
    cJSON *bb_date = cJSON_CreateString("2024-01-30");
    cJSON *trans_type = cJSON_CreateString("purchase");
    cJSON *price = cJSON_CreateString("1.99");

    cJSON_AddItemToObject(json, "amount", amount_json);
    cJSON_AddItemToObject(json, "best_before_date", bb_date);
    cJSON_AddItemToObject(json, "transaction_type", trans_type);
    cJSON_AddItemToObject(json, "price", price);

    char *json_str = cJSON_Print(json);
    grocy_post("/api/stock/products/" + std::to_string(product_id) + "/add", json_str, strlen(json_str));

    cJSON_Delete(json);
}

void consume_product_from_stock(uint32_t product_id, float amount){
    
    cJSON *json = cJSON_CreateObject();
    cJSON *amount_json = cJSON_CreateNumber(amount);
    cJSON *spoiled = cJSON_CreateBool(false);

    cJSON_AddItemToObject(json, "amount", amount_json);
    cJSON_AddItemToObject(json, "spoiled", spoiled);

    char *json_str = cJSON_Print(json);
    grocy_post("/api/stock/products/" + std::to_string(product_id) + "/consume", json_str, strlen(json_str));

    cJSON_Delete(json);
}

void update_item_inventory(uint32_t product_id, uint32_t location_id, float new_amount){
    
    cJSON *json = cJSON_CreateObject();

    cJSON_AddItemToObject(json, "new_amount", cJSON_CreateNumber(new_amount));
    cJSON_AddItemToObject(json, "amount", cJSON_CreateNumber(new_amount));
    cJSON_AddItemToObject(json, "best_before_date", cJSON_CreateString("2024-01-30"));
    cJSON_AddItemToObject(json, "shopping_location_id", cJSON_CreateNumber(0));
    cJSON_AddItemToObject(json, "location_id", cJSON_CreateNumber(location_id));
    cJSON_AddItemToObject(json, "price", cJSON_CreateNumber(0));
    cJSON_AddItemToObject(json, "stock_label_type", cJSON_CreateNumber(0));
    cJSON_AddItemToObject(json, "note", cJSON_CreateString("Mod by smart_fridge"));

    char *json_str = cJSON_Print(json);
    grocy_post("/api/stock/products/" + std::to_string(product_id) + "/inventory", json_str, strlen(json_str));

    cJSON_Delete(json);
}

void get_stock(){
    int i = 0;
    bool end_of_list = false;
    do{
        int len = grocy_get("/api/objects/stock?limit=1&offset=" + std::to_string(i), recv_buf, RECV_BUF_SIZE);
        i+=1;

        recv_buf[len] = '\0';

        // Skip the HTTP header and select json data
        char *json_str = strstr(recv_buf, "\r\n\r\n");
        if(json_str)
            json_str += 4;

        cJSON *json = cJSON_Parse(json_str); 
        if (json == NULL) { 
            const char *error_ptr = cJSON_GetErrorPtr(); 
            if (error_ptr != NULL) { 
                ESP_LOGE("HTTP", "Error: %s\n", error_ptr); 
            } 
            cJSON_Delete(json); 
        } 
        if(!parse_stock(json))
            end_of_list = true;

        cJSON_Delete(json); 
    } while(!end_of_list);

}

void http_get_task(void *pvParameters)
{
    recv_buf  = (char *)malloc(RECV_BUF_SIZE);
    if(!recv_buf){
        ESP_LOGE("MALLOC", "Malloc failed");
    }

    // while(1) {
        ESP_LOGI("SMART_FRIDGE", "Start synchronising");
        get_products();
        get_barcodes();
        get_locations();
        get_stock();
        ESP_LOGI("SMART_FRIDGE", "Done synchronising");

        xTaskNotifyGive(gui_task_handle);

        for(auto loc : locs_map){
            loc.second.print();
        }

        // for(int countdown = 1; countdown >= 0; countdown--) {
        //     ESP_LOGI(TAG, "%d... ", countdown);
        //     vTaskDelay(1000 / portTICK_PERIOD_MS);
        // }
        // ESP_LOGI(TAG, "Starting again!");
        Grocy::grocy_cmd_t grocy_cmd;
        while(1){
            vTaskDelay(10);

            if(xQueueReceive(grocy_cmd_queue, (void *)& grocy_cmd, 0) == pdTRUE){
                ESP_LOGI("HTTP", "Received cmd: Type %d, Product_id %d", grocy_cmd.type, grocy_cmd.item.product_id);
                if(grocy_cmd.type == Grocy::CmdType::add_item)
                    add_product_to_stock(grocy_cmd.item.product_id, grocy_cmd.item.amount);
                else if (grocy_cmd.type == Grocy::CmdType::consume_item)
                    consume_product_from_stock(grocy_cmd.item.product_id, grocy_cmd.item.amount);
                else if(grocy_cmd.type == Grocy::CmdType::inventory_item)
                    update_item_inventory(grocy_cmd.item.product_id, grocy_cmd.item.location_id, grocy_cmd.item.amount);

            }
        }
}
