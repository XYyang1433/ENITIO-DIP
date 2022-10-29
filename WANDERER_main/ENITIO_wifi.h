#include "EEPROM.h"
#include <string>
#include <HTTPClient.h>
#include "esp_wpa2.h" //wpa2 library for connections to Enterprise networks
#include <Arduino_JSON.h>
#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>


// WiFi Based on https://github.com/martinius96/ESP32-eduroam/blob/master/2022/test_2.0.3/test_2.0.3.ino

/**  WiFi Credentials **/
#define EAP_ANONYMOUS_IDENTITY  ""
#define EAP_IDENTITY  "NG0166NG@student.main.ntu.edu.sg"
#define EAP_PASSWORD  "Y@ng#1433"
#define HOME_WIFI_SSID "FreeWaffles"
#define HOME_WIFI_PASSWORD "SponsoredByCKL00"

const char *ssid = "NTUSECURE";
int wifi_reconnect_counter = 0;
unsigned long last_disconnected_time = 0;
int HTTP_TIMEOUT = 30 * 1000;

struct MAC_ADDRESS {
  int n1;
  int n2;
  int n3;
  int n4;
  int n5;
  int n6;
} ;

struct GAME_CONSTANTS {
    int EN_RECOVER_DURATION;
    int VIRUS_DECAY_DURATION;
    int VIRUS_IMMUNITY_DURATION;
    int VIRUS_INFECTION_PROBABILITY;  // integer between 0 and 100
    int PARTICIPANT_MaxHP;
    int GL_MaxHP;
    int PARTICIPANT_MaxEn;
    int GL_MaxEn;
    int INITIAL_MANA;
    int HEAL_MANA;
    int MAX_ATTACK_MANA;
    int MAX_COLLECT_MANA;
    int BOMB_HP_DEDUCTION;
    int KILL_UPDATE_SERVER_INTERVAL;
    int HTTP_TIMEOUT;
};

struct FailedFeedbackStatistics {
    int num_kills;
    int num_powerups;
};

class DBConnection {
    
    private:
        String DATABASE_URL = "http://nadira.pythonanywhere.com/";
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

        MAC_ADDRESS retrieveMACAddressFromJSONArray(String json_array) {
            String OGHexString, IDHexString;
            int OG,ID;
            OG=EEPROM.read(OG_add);
            ID=EEPROM.read(ID_add);
            OGHexString=String(OG,HEX);
            IDHexString=String(ID,HEX);
                                 
            JSONVar json_obj = JSON.parse(json_array);
            MAC_ADDRESS mac_addr;
            if (JSON.typeof(json_obj) == "undefined") {
                Serial.println("Parsing input failed!");
                return mac_addr;
            }
            //Serial.println(JSON.stringify(json_obj["mac_address_part0"]));//DIPEDIT
            String n1 = "04";//JSON.stringify(json_obj["mac_address_part5"]);
            String n2 = "08";//JSON.stringify(json_obj["mac_address_part4"]);
            String n3 = "01";//JSON.stringify(json_obj["mac_address_part3"]);
            String n4 = OGHexString;//JSON.stringify(json_obj["mac_address_part2"]);
            String n5 = IDHexString;//JSON.stringify(json_obj["mac_address_part1"]);
            String n6 = "01";//JSON.stringify(json_obj["mac_address_part0"]);

            n1 = n1.substring(1, n1.length() - 1);
            n2 = n2.substring(1, n2.length() - 1);
            n3 = n3.substring(1, n3.length() - 1);
            n4 = n4.substring(1, n4.length() - 1);
            n5 = n5.substring(1, n5.length() - 1);
            n6 = n6.substring(1, n6.length() - 1);
            
            mac_addr.n1 = n1.toInt();
            mac_addr.n2 = n2.toInt();
            mac_addr.n3 = n3.toInt();
            mac_addr.n4 = n4.toInt();
            mac_addr.n5 = n5.toInt();
            mac_addr.n6 = n6.toInt();

            Serial.println(mac_addr.n6);

            return mac_addr;
        };

        GAME_CONSTANTS retrieveGameConstantsFromJSONArray(String json_array) {
            JSONVar json_obj = JSON.parse(json_array);
            GAME_CONSTANTS game_const;
            if (JSON.typeof(json_obj) == "undefined") {
                Serial.println("Parsing input failed!");
                return game_const;
            }
            //DIPEDIT
            game_const.EN_RECOVER_DURATION = 5000; //JSON.stringify(json_obj["EN_RECOVER_DURATION"]).toInt();
            game_const.VIRUS_DECAY_DURATION =30 * 1000; //JSON.stringify(json_obj["VIRUS_DECAY_DURATION"]).toInt();
            game_const.VIRUS_IMMUNITY_DURATION = 2*60*1000;//JSON.stringify(json_obj["VIRUS_IMMUNITY_DURATION"]).toInt();
            game_const.VIRUS_INFECTION_PROBABILITY = 50;//JSON.stringify(json_obj["VIRUS_INFECTION_PROBABILITY"]).toInt();
            game_const.PARTICIPANT_MaxHP = 12;//JSON.stringify(json_obj["PARTICIPANT_MaxHP"]).toInt();
            game_const.GL_MaxHP = 50;//JSON.stringify(json_obj["GL_MaxHP"]).toInt();
            game_const.PARTICIPANT_MaxEn = 5; //JSON.stringify(json_obj["PARTICIPANT_MaxEn"]).toInt();
            game_const.GL_MaxEn = 50;//JSON.stringify(json_obj["GL_MaxEn"]).toInt();
            game_const.INITIAL_MANA = 1;//JSON.stringify(json_obj["INITIAL_MANA"]).toInt();
            game_const.HEAL_MANA = 10;//JSON.stringify(json_obj["HEAL_MANA"]).toInt();
            game_const.MAX_ATTACK_MANA = 3;//JSON.stringify(json_obj["MAX_ATTACK_MANA"]).toInt();
            game_const.MAX_COLLECT_MANA = 10;//JSON.stringify(json_obj["MAX_COLLECT_MANA"]).toInt();
            game_const.BOMB_HP_DEDUCTION = 6;//JSON.stringify(json_obj["BOMB_HP_DEDUCTION"]).toInt();
            game_const.KILL_UPDATE_SERVER_INTERVAL = 2*60*1000;//JSON.stringify(json_obj["KILL_UPDATE_SERVER_INTERVAL"]).toInt();
            game_const.HTTP_TIMEOUT = 30*1000;//JSON.stringify(json_obj["HTTP_TIMEOUT"]).toInt();

            return game_const;
        };
        
        FailedFeedbackStatistics retrieveStatisticsFromJSONArray(String json_array) {
            JSONVar json_obj = JSON.parse(json_array);
            FailedFeedbackStatistics feedback_stats;
            if (JSON.typeof(json_obj) == "undefined") {
                Serial.println("Parsing input failed!");
                return feedback_stats;
            }
            feedback_stats.num_kills = 0;//JSON.stringify(json_obj["num_kills"]).toInt();
            feedback_stats.num_powerups = 0;//JSON.stringify(json_obj["num_powerups"]).toInt();
            return feedback_stats;
        }
        
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
    
    public:
        void startWiFiConnection() {
            // WiFi.begin(HOME_WIFI_SSID, HOME_WIFI_PASSWORD);
            WiFi.begin(ssid, WPA2_AUTH_PEAP, EAP_ANONYMOUS_IDENTITY, EAP_IDENTITY, EAP_PASSWORD);
        }
        bool connectToWiFi() {
            // returns True if connected, False if timeout
            /*
            Serial.print("Connecting to Wi-Fi");
            startWiFiConnection();
            int counter = 0;
            while (WiFi.status() != WL_CONNECTED) {
                Serial.print(".");
                delay(500);
                counter++;
                if (counter >= 20) {
                    // timeout
                    return false;
                }
            };
            Serial.print("Connected with IP: "); Serial.println(WiFi.localIP());
            return true;
            */
            return false;
        };

        bool registerWanderer(int OG, String mac_addr) {
            String url = DATABASE_URL + "player_registration";
            String httpRequestData = "{\"OG\": " + String(OG) + ", \"mac_address\": \"" + mac_addr + "\" }";
            Serial.println(httpRequestData);
            String jsonArray = POST_Request(url.c_str(), httpRequestData.c_str());
            Serial.println(jsonArray);
            return true;//jsonArray != "{}";//DIPEDIT
        };

        bool hasGameStarted() {
            String url = DATABASE_URL + "game_status";
            String jsonArray = GET_Request(url.c_str());
            String gameStatus = retrieveParameterFromJSONArray("has_game_started", jsonArray);
            return true ;//gameStatus == "1";//DIPEDIT
        }

        int getPlayerID(int OG, String mac_addr) {
            /*String url = DATABASE_URL + "player_id/" + String(OG) + "/" + mac_addr;
            String jsonArray = GET_Request(url.c_str());
            return retrieveParameterFromJSONArray("player_id", jsonArray).toInt();*/
            return 0;//EEPROM.read(ID_add);
        };

        GAME_CONSTANTS getGameConstants() {
            String url = DATABASE_URL + "get_all_game_variables";
            String jsonArray = GET_Request(url.c_str());
            // Serial.println(jsonArray);
            return retrieveGameConstantsFromJSONArray(jsonArray);
        };
        
        MAC_ADDRESS getDeviceMACAddress(int playerIdentifier) {
            String url = DATABASE_URL + "player/" + String(playerIdentifier);
            String jsonArray = GET_Request(url.c_str());
            return retrieveMACAddressFromJSONArray(jsonArray);
        };

        FailedFeedbackStatistics sendGameStatistics(int OG, int ID, int kills, int num_level1_treasure, int num_level2_treasure) {
            String url = DATABASE_URL + "player_score";
            String httpRequestData = "{\"OG\": " + String(OG) + ", \"ID\": " + String(ID) + ", \"num_kills\": " + String(kills);
            httpRequestData = httpRequestData + ", \"level1\": " + String(num_level1_treasure) + ", \"level2\": " + String(num_level2_treasure);
            httpRequestData = httpRequestData + "}";
            Serial.println(httpRequestData);
            String jsonArray = POST_Request(url.c_str(), httpRequestData.c_str());
            Serial.println(jsonArray);
            return retrieveStatisticsFromJSONArray(jsonArray);
        };

        bool uploadFailedFeedback(int og, int participant_id) {
            String url = DATABASE_URL + "player_kill/" + String(og) + "/" + String(participant_id);
            String jsonArray = GET_Request(url.c_str());
            return jsonArray != "{}";
        }
};

DBConnection dbc;
