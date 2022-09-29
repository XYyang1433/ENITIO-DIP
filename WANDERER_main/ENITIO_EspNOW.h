#include "ENITIO_wifi.h"

typedef struct esp_now_msg {
  int msg_type; // 1 for feedback msg, 2 for bomb msg
  int attackee_type; // 1 for player, 2 for Lvl 1 Treasure, 3 for Lvl 2 Treasure, 4 for bomb, 5 for Healing Station
  int attacker_OG;
  int attacker_ID; 
  int attackee_OG; // or Treasure ID
  int is_attackee_killed;
} feedback_message;

int EspNOW_received = 0;

// Global copy of slave
#define NUM_BOMBS_TARGET 15

const int recv_msg_buffer_size = NUM_BOMBS_TARGET + 10; 

feedback_message EspNOW_recvData[recv_msg_buffer_size] = {};
int recv_data_pointer = 0;
int recv_data_read_pointer = 0;

bool send_Status = 1 ;

class EspNOW {
  private:
    feedback_message feedbackData;
    esp_now_peer_info_t peerInfo;
    uint8_t broadcastAddress[6];

    esp_now_peer_info_t bomb_targets[NUM_BOMBS_TARGET] = {};
    int target_count = 0;
    
  public:
    int num_bombed = 0;
    
    void enable() {
      // WiFi.mode(WIFI_STA);
      WiFi.mode(WIFI_AP_STA);
      if (esp_now_init() != ESP_OK) {
        Serial.println("Error initializing ESP-NOW");
        return;
      }
      Serial.println("OK");
      esp_now_register_send_cb(OnDataSent);
      esp_now_register_recv_cb(OnDataRecv);
      Serial.print(WiFi.channel());
    }

    void getDeviceMACAddress(int attacker_OG, int attacker_ID){
      broadcastAddress[0] = 4;
      broadcastAddress[1] = 8;
      broadcastAddress[2] = 1;
      broadcastAddress[3] = attacker_OG;
      broadcastAddress[4] = attacker_ID;
      broadcastAddress[5] = 1;
    }
  
   void send_data(int msg_type, int attackee_type, int attacker_OG, int attacker_ID, int attackee_OG, int is_attackee_killed){
            // Register peer
      feedbackData.msg_type = msg_type;
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
      Serial.print("Current MAC Address:"); Serial.println(WiFi.macAddress());
       
      if (result == ESP_OK) {
        Serial.println("Sent with success");
      }
      else {
        Serial.println("Error sending the data");
      }

      esp_now_del_peer(broadcastAddress);

      if((!send_Status) && is_attackee_killed){
        failed_kill_feedback ++ ;
        failed_kill_OG[current_failed_save_pointer] = attacker_OG;
        failed_kill_ID[current_failed_save_pointer] = attacker_ID;
        current_failed_save_pointer ++ ;
        if(current_failed_save_pointer >= 50) current_failed_save_pointer -= 50;
      }
    }

    void disable(){
      esp_now_deinit();
    } 

    static void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
      feedback_message temp_recv_data;
      memcpy(&temp_recv_data, incomingData, sizeof(temp_recv_data));
      EspNOW_recvData[recv_data_pointer] = temp_recv_data;
      EspNOW_received++;
      recv_data_pointer++;
      if (recv_data_pointer >= recv_msg_buffer_size) recv_data_pointer -= recv_msg_buffer_size;
    }

    static void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
      Serial.print("\r\nLast Packet Send Status:\t");
      if (status == ESP_NOW_SEND_SUCCESS){
        Serial.println("Delivery Success");
        send_Status = 1 ;
        }
      else {
          Serial.println("Delivery Failed");
          send_Status = 0 ;
        }
    }

    feedback_message get_feedback_received(){
      feedback_message data = EspNOW_recvData[recv_data_read_pointer];
      recv_data_read_pointer++;
      if (recv_data_read_pointer >= recv_msg_buffer_size) recv_data_read_pointer -= recv_msg_buffer_size;
      return data;
    }

    void ScanForBombTarget() {
      WiFi.scanNetworks();
      int8_t scanResults = WiFi.scanComplete();
      //reset slaves
      memset(bomb_targets, 0, sizeof(bomb_targets));
      target_count = 0;
      Serial.println("");
      if (scanResults == 0) {
        Serial.println("No WiFi devices in AP Mode found");
      } else {
        Serial.print("Found "); Serial.print(scanResults); Serial.println(" devices ");
        for (int i = 0; i < scanResults; ++i) {
          // Print SSID and RSSI for each device found

          String BSSIDstr = WiFi.BSSIDstr(i);
          int mac[6];
          Serial.println(BSSIDstr);
          
          if (6 == sscanf(BSSIDstr.c_str(), "%x:%x:%x:%x:%x:%x",  &mac[0], &mac[1], &mac[2], &mac[3], &mac[4], &mac[5])) {
            if ((mac[0] == 4) && (mac[1] == 8) && (mac[2] == 1) && ((mac[5] == 1) || (mac[5] == 0))){
              for (int ii = 0; ii < 6; ++ii ) {
                bomb_targets[target_count].peer_addr[ii] = (uint8_t) mac[ii];
              }
              bomb_targets[target_count].channel = 0; // pick a channel
              bomb_targets[target_count].encrypt = 0; // no encryption
              target_count++;
            }
          }
          if (target_count >= NUM_BOMBS_TARGET) break;
        }
      }
    
      if (target_count > 0) {
        Serial.print(target_count); Serial.println(" Target(s) found, processing..");
      } else {
        Serial.println("No Target Found, trying again.");
      }
    
      // clean up ram
      WiFi.scanDelete();
    }

    void SendBombToAllTargets(int attacker_OG, int attacker_ID){
      int i;
      for (i = 0; i < target_count; i++){
        if (esp_now_add_peer(&bomb_targets[i]) != ESP_OK){
          Serial.println("Failed to add peer");
          return;
        }
        feedbackData.msg_type = 2;
        feedbackData.attackee_type = 1;
        feedbackData.attacker_OG = attacker_OG;
        feedbackData.attacker_ID = attacker_ID;
        feedbackData.attackee_OG = 255;
        feedbackData.is_attackee_killed = 255;

        // Send message via ESP-NOW
        esp_err_t result = esp_now_send(bomb_targets[i].peer_addr, (uint8_t *) &feedbackData, sizeof(feedbackData));

        if (result == ESP_OK) {
          Serial.println("Sent with success");
        }
        else {
          Serial.println("Error sending the data");
        }
        delay(100);
      }
      for (i = 0; i < target_count; i++){
        esp_now_del_peer(bomb_targets[i].peer_addr);
      }
      num_bombed = target_count;
      target_count = 0;
    }

    
};

EspNOW Player_EspNOW;
