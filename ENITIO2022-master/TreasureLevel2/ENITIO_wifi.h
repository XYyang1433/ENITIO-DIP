#include <WiFi.h>
#include "esp_wpa2.h" //wpa2 library for connections to Enterprise networks
#include <HTTPClient.h>
#include <Arduino_JSON.h>
#include <esp_now.h>

// WiFi Based on https://github.com/martinius96/ESP32-eduroam/blob/master/2022/test_2.0.3/test_2.0.3.ino

/**  WiFi Credentials **/
#define EAP_ANONYMOUS_IDENTITY  ""
#define EAP_IDENTITY  "@student.main.ntu.edu.sg"
#define EAP_PASSWORD  ""
#define HOME_WIFI_SSID "FreeWaffles"
#define HOME_WIFI_PASSWORD "SponsoredByCKL00"
const char *ssid = "NTUSECURE";
int wifi_reconnect_counter = 0;
int HTTP_TIMEOUT = 30 * 1000;
int wifi_timeout_or_refused_counter = 0;

struct GAME_CONSTANTS {
    int TREASURE_LEVEL2_INITIAL_HP;
    int NUM_L2TREASURES;
    int TREASURE_VIRUS_THRESHOLD;
    int TREASURE_LEVEL2_ACTION_RECV_WAIT;
    int TREASURE_LEVEL2_RECOVER_PERIOD;
    int TREASURE_LEVEL2_VIRUS_INFECTION_TIME;
    int HTTP_TIMEOUT;
};

class DBConnection {
    private:
        String DATABASE_URL = "http://enitiotreasurehunt.link/";
        String GET_Request(const char* server) {
            HTTPClient http;
            http.setTimeout(HTTP_TIMEOUT);
            http.begin(server);
            int httpResponseCode = http.GET();
        
            String payload = "{}";
        
            if (httpResponseCode > 0) {
                Serial.print("HTTP Response code: "); Serial.println(httpResponseCode);
                payload = http.getString();
            } else {
                Serial.print("Error code: "); Serial.println(httpResponseCode);
                wifi_timeout_or_refused_counter++;
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
                    wifi_timeout_or_refused_counter++;
                }
                http.end();
                return responsePayload;
            }
        };

     /*   GAME_CONSTANTS retrieveGameConstantsFromJSONArray(String json_array) {
            JSONVar json_obj = JSON.parse(json_array);
            GAME_CONSTANTS game_const;
            if (JSON.typeof(json_obj) == "undefined") {
                Serial.println("Parsing input failed!");
                return game_const;
            }
            game_const.TREASURE_LEVEL2_INITIAL_HP = JSON.stringify(json_obj["TREASURE_LEVEL2_INITIAL_HP"]).toInt();
            game_const.NUM_L2TREASURES = JSON.stringify(json_obj["NUM_L2TREASURES"]).toInt();
            game_const.TREASURE_VIRUS_THRESHOLD = JSON.stringify(json_obj["TREASURE_VIRUS_THRESHOLD"]).toInt();
            game_const.HTTP_TIMEOUT = JSON.stringify(json_obj["HTTP_TIMEOUT"]).toInt();
            game_const.TREASURE_LEVEL2_ACTION_RECV_WAIT = JSON.stringify(json_obj["TREASURE_LEVEL2_ACTION_RECV_WAIT"]).toInt();
            game_const.TREASURE_LEVEL2_RECOVER_PERIOD = JSON.stringify(json_obj["TREASURE_LEVEL2_RECOVER_PERIOD"]).toInt();
            game_const.TREASURE_LEVEL2_VIRUS_INFECTION_TIME = JSON.stringify(json_obj["TREASURE_LEVEL2_VIRUS_INFECTION_TIME"]).toInt();
            return game_const;
        };
    */
    public:
        bool connectToWiFi() {
            // returns True if connected, False if timeout
            // WiFi.begin(HOME_WIFI_SSID, HOME_WIFI_PASSWORD);
           /* WiFi.begin(ssid, WPA2_AUTH_PEAP, EAP_ANONYMOUS_IDENTITY, EAP_IDENTITY, EAP_PASSWORD);
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
            Serial.print("Connected with IP: "); Serial.println(WiFi.localIP());*/
            return true;
        };

        int hasGameStarted() {
            /*String url = DATABASE_URL + "game_status";
            String jsonArray = GET_Request(url.c_str());
            return retrieveParameterFromJSONArray("has_game_started", jsonArray).toInt();
            */
            return true;
        }
        
        String getDeviceMACAddress(int playerIdentifier) {
            String url = DATABASE_URL + "player/" + String(playerIdentifier);
            String jsonArray = GET_Request(url.c_str());
            return retrieveParameterFromJSONArray("mac_address", jsonArray);
        };

        String setTreasureAsOpened(String treasureName, int og, int participant_id) {
            String url = DATABASE_URL + "treasure/2/" + treasureName + "/" + String(og) + "/" + String(participant_id);
            String jsonArray = GET_Request(url.c_str());
            return retrieveParameterFromJSONArray("mac_address", jsonArray);
        };
        
        /*GAME_CONSTANTS getGameConstants() {
            String url = DATABASE_URL + "get_all_game_variables";
            String jsonArray = GET_Request(url.c_str());
            // Serial.println(jsonArray);
            return retrieveGameConstantsFromJSONArray(jsonArray);
        };*/
};

DBConnection dbc;
