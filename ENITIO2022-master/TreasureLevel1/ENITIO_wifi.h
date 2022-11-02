#include <WiFi.h>
#include "esp_wpa2.h" //wpa2 library for connections to Enterprise networks
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include <esp_now.h>
#include <esp_wifi.h>

// WiFi Based on https://github.com/martinius96/ESP32-eduroam/blob/master/2022/test_2.0.3/test_2.0.3.ino

/**  WiFi Credentials **/
#define EAP_ANONYMOUS_IDENTITY  ""
#define EAP_IDENTITY  "NG0166NG@student.main.ntu.edu.sg"
#define EAP_PASSWORD  "Y@ng#1433"
#define HOME_WIFI_SSID "TP-Link_E45E"
#define HOME_WIFI_PASSWORD "63824377"
const char *ssid = "NTUSECURE";
int wifi_reconnect_counter = 0;
int HTTP_TIMEOUT = 30 * 1000;

struct GAME_CONSTANTS {
    int TREASURE_LEVEL1_INITIAL_HP;
    int TREASURE_LEVEL1_ACTION_RECV_WAIT;
    int TREASURE_LEVEL1_RECOVER_DURATION;
    int HTTP_TIMEOUT;
};

class DBConnection {
    private:
        String DATABASE_URL = "http://nadira.pythonanywhere.com/";
        String GET_Request(const char* server) {
            HTTPClient http;
            http.setTimeout(HTTP_TIMEOUT);
            http.begin(server);
//            Serial.print("Current WiFi Status     : "); Serial.println(WiFi.status());
//            Serial.print("Reference - CONNECTED IS: "); Serial.println(WL_CONNECTED);
            Serial.print("WiFi Channel: "); Serial.println(WiFi.channel());
            int httpResponseCode = http.GET();
            String payload = "{}";
            
            if (httpResponseCode > 0) {
                Serial.print("HTTP Response code: "); Serial.println(httpResponseCode);
                payload = http.getString();
            }else {
                Serial.print("Error code: "); Serial.println(httpResponseCode);
            }
            http.end();
            return payload;
        };
    
        String retrieveParameterFromJSONArray(String parameter, String json_array) {
            JSONVar json_obj = JSON.parse(json_array);
            if (JSON.typeof(json_obj) == "undefined") {
                Serial.println("Parsing input failed!");
                return "";
            }
            return JSON.stringify(json_obj[parameter]);
        };
        
        String POST_Request(const char* server, const char* payload) {
            if (WiFi.status() == WL_CONNECTED) {
                HTTPClient http;
                http.setTimeout(HTTP_TIMEOUT);
                http.begin(server);
                http.addHeader("Content-Type", "application/json");
                int httpResponseCode = http.POST(payload);
                String responsePayload = "{}";
                if (httpResponseCode > 0) {
                    Serial.print("HTTP Response code: "); Serial.println(httpResponseCode);
                    responsePayload = http.getString();
                }
                else {
                    Serial.print("Error code: "); Serial.println(httpResponseCode);
                }
                http.end();
                return responsePayload;
            }
        };

        GAME_CONSTANTS retrieveGameConstantsFromJSONArray(String json_array) {
            JSONVar json_obj = JSON.parse(json_array);
            GAME_CONSTANTS game_const;
            if (JSON.typeof(json_obj) == "undefined") {
                Serial.println("Parsing input failed!");
                return game_const;
            }
            game_const.TREASURE_LEVEL1_INITIAL_HP = 1; //JSON.stringify(json_obj["TREASURE_LEVEL1_INITIAL_HP"]).toInt();
            game_const.TREASURE_LEVEL1_ACTION_RECV_WAIT = 3000;//JSON.stringify(json_obj["TREASURE_LEVEL1_ACTION_RECV_WAIT"]).toInt();
            game_const.TREASURE_LEVEL1_RECOVER_DURATION = 20*1000;//JSON.stringify(json_obj["TREASURE_LEVEL1_RECOVER_DURATION"]).toInt();
            game_const.HTTP_TIMEOUT = 30*1000;//JSON.stringify(json_obj["HTTP_TIMEOUT"]).toInt();//DIPEDIT
            return game_const;
        };
    
    public:
        bool connectToWiFi() {
            // returns True if connected, False if timeout
            // WiFi.begin(HOME_WIFI_SSID, HOME_WIFI_PASSWORD);
            /*
            WiFi.begin(ssid, WPA2_AUTH_PEAP, EAP_ANONYMOUS_IDENTITY, EAP_IDENTITY, EAP_PASSWORD);
            Serial.print("Connecting to Wi-Fi");
            int counter = 0;
            while (WiFi.status() != WL_CONNECTED) {
                Serial.print(".");
                delay(500);
                counter++;
                if (counter >= 60) {
                    // timeout
                    return false;
                }
            };
            Serial.print("Connected with IP: "); Serial.println(WiFi.localIP());
            */
            return true;
        };

        int hasGameStarted() {
            String url = DATABASE_URL + "game_status";
            String jsonArray = GET_Request(url.c_str());
            return 1;//retrieveParameterFromJSONArray("has_game_started", jsonArray).toInt();
        }
        
        String getDeviceMACAddress(int playerIdentifier) {
            String url = DATABASE_URL + "player/" + String(playerIdentifier);
            String jsonArray = GET_Request(url.c_str());
            return retrieveParameterFromJSONArray("mac_address", jsonArray);
        };

        String setTreasureAsOpened(String treasureName, int og, int participant_id) {
            String url = DATABASE_URL + "treasure/1/" + treasureName + "/" + String(og) + "/" + String(participant_id);
            String jsonArray = GET_Request(url.c_str());
            return retrieveParameterFromJSONArray("mac_address", jsonArray);
        };

        bool sendGameStatistics(String treasureName, int alatar, int drachen, int eva, int invicta) {
            String url = DATABASE_URL + "treasure_score";
            String httpRequestData = "{\"treasureName\": " + treasureName + ", \"alatar\": " + String(alatar) + ", \"drachen\": " + String(drachen);
            httpRequestData = httpRequestData + ", \"eva\": " + String(eva) + ", \"invicta\": " + String(invicta);
            httpRequestData = httpRequestData + "}";
            Serial.println(httpRequestData);
            String jsonArray = POST_Request(url.c_str(), httpRequestData.c_str());
            Serial.println(jsonArray);
            return jsonArray != "{}";
        };

        GAME_CONSTANTS getGameConstants() {
            String url = DATABASE_URL + "get_all_game_variables";
            String jsonArray = GET_Request(url.c_str());
            // Serial.println(jsonArray);
            return retrieveGameConstantsFromJSONArray(jsonArray);
        };

        bool uploadFailedFeedback(String treasureName, int og, int participant_id) {
            String url = DATABASE_URL + "treasurefeedback/" + treasureName + "/" + String(og) + "/" + String(participant_id);
             String jsonArray = GET_Request(url.c_str());
//            String httpRequestData = "{\"OG\": " + String(og) + ", \"ID\": " + String(participant_id) + "}";
//            String jsonArray = POST_Request(url.c_str(), httpRequestData.c_str());
            Serial.println(jsonArray);
            return jsonArray != "{}";
        }
};

DBConnection dbc;
