<img width="400" height="750" alt="imagen" src="https://github.com/user-attachments/assets/c2145af2-3ba6-4617-b1ef-2ebc974f5408" />

This project allows your **Arduino UNO R4 WiFi** to:  

- 🌐 Scan available Wi-Fi networks  
- 🔒 Connect to a predefined home network (WPA/WPA2) or fallback to open networks  
- 📡 Scan the local network for active hosts  
- 📩 Send the network and host information to a Telegram chat  
- 💡 Display status using the onboard **12x8 LED matrix** with animations and scrolling text  

## ✨ Features

- 📝 Short text messages for each stage:  
  - `"Searching"` 🔍  
  - `"Connected"` ✅  
  - `"Hosts"` 🖧  
  - `"Sending"` 📤  
- 💡 LED matrix animations for each step  
- 📶 Wi-Fi scanning and connection handling  
- 🔎 Host ping scanning on the local subnet  
- 🤖 Telegram notification with detailed information  

## ⚙️ Requirements

- [**Arduino UNO R4 WiFi**](https://store.arduino.cc/products/uno-r4-wifi)  
- [**Arduino IDE**](https://www.arduino.cc/en/software/) (version 2.3 or higher recommended)  
- Libraries:  
  - `WiFi.h` (built-in for R4 WiFi)  
  - `HTTPClient.h` (built-in)  
  - `ArduinoGraphics`  
  - `Arduino_LED_Matrix`  
- Telegram Bot with:  
  - `TOKEN` 🔑  
  - `CHATID` 💬  

## 🛠 Setup

1. Install the required **Arduino Library Manager**
2. Replace the placeholders in the code:  

```cpp
const char* TOKEN = "YOUR_TOKEN";
const char* CHATID = "YOUR_CHATID";
const char* HOME_SSID = "YOUR_SSID";
const char* HOME_PASS = "YOUR_PASS_SSID";
```

3. Upload the code to the UNO R4 WiFi using USB-C
4. Open the Serial Monitor (115200 baud) to see debug output if needed
5. The LED matrix will show the stage animations, and the Telegram bot will receive the scanned info

🚦 Usage

Place your Arduino near the Wi-Fi network you want to scan
The device will automatically select your home network if available, otherwise it will connect to an open network
After scanning the hosts, a message will be sent to the configured Telegram chat

⚠️ Notes

Designed for educational and personal use only 🎓
Only scans networks you have permission to access ✅
Works best on small local networks due to ping scanning limitations 🏠
