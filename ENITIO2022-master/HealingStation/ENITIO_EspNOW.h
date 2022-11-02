#include "ENITIO_wifi.h"

typedef struct feedback_message {
  int msg_type = 1;
  int attackee_type; // 1 for player, 2 for Lvl 1 Treasure, 3 for Lvl 2 Treasure, 4 for bomb, 5 for healing station
  int attacker_OG;
  int attacker_ID; 
  int attackee_OG; // or Treasure ID
  int is_attackee_killed;
} feedback_message;

int EspNOW_received = 0;
feedback_message EspNOW_recvData;

class EspNOW {
  private:
    feedback_message feedbackData;
    esp_now_peer_info_t peerInfo;
    uint8_t broadcastAddress[6];
    
  public:
    int is_waiting_for_feedback = 0;
    
    void enable() {
      // WiFi.mode(WIFI_STA);
      WiFi.mode(WIFI_AP_STA);
      if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
      }
      Serial.println("OK");
      esp_now_register_send_cb(OnDataSent);
      // esp_now_register_recv_cb(OnDataRecv);
    }

    void getDeviceMACAddress(int attacker_OG, int attacker_ID){
//      int playerIdentifier = attacker_OG*pow(16,2) + attacker_ID;
//      MAC_ADDRESS target_addr = dbc.getDeviceMACAddress(playerIdentifier);
      broadcastAddress[0] = 4;
      broadcastAddress[1] = 8;
      broadcastAddress[2] = 1;
      broadcastAddress[3] = attacker_OG;
      broadcastAddress[4] = attacker_ID;
      broadcastAddress[5] = 1;
    }
  
   void send_data(int attackee_type, int attacker_OG, int attacker_ID, int attackee_OG, int is_attackee_killed){
      // Register peer
      feedbackData.attackee_type = attackee_type;
      feedbackData.attacker_OG = attacker_OG;
      feedbackData.attacker_ID = attacker_ID;
      feedbackData.attackee_OG = attackee_OG;
      feedbackData.is_attackee_killed = is_attackee_killed;
  
      getDeviceMACAddress(attacker_OG, attacker_ID);
      
      memcpy(peerInfo.peer_addr, broadcastAddress, 6);
      peerInfo.channel = 0;  
      peerInfo.encrypt = false;
        
      // Add peer     
      if (esp_now_add_peer(&peerInfo) != ESP_OK){
        Serial.println("Failed to add peer");
        return;
      }
      
      // Send message via ESP-NOW
      esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &feedbackData, sizeof(feedbackData));
       
      if (result == ESP_OK) {
        Serial.println("Sent with success");
      }
      else {
        Serial.println("Error sending the data");
      }
      
      esp_now_del_peer(broadcastAddress);
    }

    void disable(){
      esp_now_deinit();
    } 

    static void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
      memcpy(&EspNOW_recvData, incomingData, sizeof(EspNOW_recvData));
      EspNOW_received = 1;
    }

    static void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
      Serial.print("\r\nLast Packet Send Status:\t");
      Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
    }

    feedback_message get_feedback_received(){
      return EspNOW_recvData;
    }
};

EspNOW HealingStation_EspNOW;
