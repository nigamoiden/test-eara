================================================================
  ESP32 LVGL Audio Hub + LeafDra Pet — Huong dan cai dat
  LVGL 9.x + BT Classic HFP + WiFi Music + Pet Animation
================================================================

THU VIEN CAN CAI (Arduino IDE > Library Manager):
  1. lvgl (9.x)              — by LVGL
  2. TFT_eSPI                — by Bodmer
  3. XPT2046_Touchscreen     — by Paul Stoffregen

BUOC 1: Cai thu vien lvgl 9.x
  - Arduino IDE > Sketch > Include Library > Manage Libraries
  - Tim "lvgl" cai version 9.x (moi nhat)

BUOC 2: Copy lv_conf.h
  - Copy file lv_conf.h vao:
    Windows: C:\Users\<ten>\Documents\Arduino\libraries\
    Mac:     ~/Documents/Arduino/libraries/
    Linux:   ~/Arduino/libraries/
  - File PHAI nam CUNG CAP voi thu muc lvgl, KHONG phai ben trong

  Cau truc thu muc:
    Arduino/libraries/
    ├── lv_conf.h          <-- DAT O DAY
    ├── lvgl/
    │   ├── src/
    │   └── ...
    ├── TFT_eSPI/
    └── XPT2046_Touchscreen/

BUOC 3: Config TFT_eSPI
  - Mo file Arduino/libraries/TFT_eSPI/User_Setup.h
  - Sua theo board cua ban:

  === Cho ESP32 WROOM-32: ===
    #define ILI9341_DRIVER
    #define TFT_WIDTH  240
    #define TFT_HEIGHT 320
    #define TFT_MOSI   23
    #define TFT_SCLK   18
    #define TFT_CS     5
    #define TFT_DC     2
    #define TFT_RST    4
    #define TFT_BL     15
    #define TOUCH_CS   21
    #define SPI_FREQUENCY 40000000

BUOC 4: Sua pin trong ESP32_LVGL.ino
  - Dong 37-38: doi TOUCH_CS va TFT_BL cho khop voi board

BUOC 5: Upload
  - Chon board: ESP32 Dev Module
  - Partition: Default 4MB with spiffs
  - Upload speed: 921600
  - Upload!

================================================================
  CAU TRUC THU MUC SKETCH
================================================================

  LVGL_ESP32/
  ├── ESP32_LVGL.ino        — Code chinh (6 man hinh)
  ├── lv_conf.h             — LVGL 9 config
  ├── bg_image.h            — Header cho background (can resize)
  ├── walk_frames/          — Animation di bo (mac dinh)
  │   ├── walk_frames.c
  │   ├── walk_frames.h
  │   └── frames/           — 9 PNG frames (64x64)
  ├── play_frames/          — Animation choi (khi tap)
  ├── happy_frames/         — Animation vui
  ├── angry_frames/         — Animation gian
  └── love_frames/          — Animation yeu thuong

================================================================
  6 MAN HINH
================================================================

1. Home (Pet)   — Nhan vat LeafDra animated + mood buttons
                  Mini player bar + nut Music/Phone/Settings
                  Binh thuong = walk, tap = play
                  3 nut mood: Happy / Angry / Love
                  Mood duoc gui kem trong HTTP request chatbot

2. Music Browser — Tab Online (stream WiFi) / Downloaded (SD)
                   Voice search (mic icon)
                   Moi bai co nut Play + Download/Delete

3. Now Playing  — Ten bai + nghe si, 7 thanh EQ animated
                  Progress bar, Prev/Play/Next, Volume slider

4. Incoming Call — Fade-in, so dien thoai (khong co ten
                   vi BT Classic khong truy cap danh ba)
                   Reject (do) / Accept (xanh)

5. In-Call      — So dien thoai, timer MM:SS
                   Mute mic / Speaker / End call

6. Settings     — Bluetooth toggle + ten thiet bi
                   WiFi toggle + SSID
                   Brightness slider, Volume slider
                   SD storage bar, About info

================================================================
  HE THONG PET / CHATBOT
================================================================

ANIMATION:
  - Mac dinh: walk (di bo lien tuc)
  - Tap vao pet: play (choi 1 vong roi quay ve walk)
  - Chon mood: happy/angry/love (hien thi lien tuc)

CHATBOT INTEGRATION:
  - get_current_emotion()      → "happy"/"angry"/"love"
    Gui kem trong HTTP request khi chat voi server

  - set_emotion_from_server()  → Nhan emotion tu server response
    Server tra ve "mood": "happy" → pet tu dong doi animation

  Vi du JSON request:
    { "message": "xin chao", "mood": "happy" }

  Vi du JSON response:
    { "reply": "chao ban!", "mood": "love" }

BACKGROUND:
  - File goc 1628x640 qua lon (3.1MB) cho ESP32
  - Can resize ve 320x240 bang LVGL Image Converter:
    https://lvgl.io/tools/imageconverter
  - Sau khi resize, uncomment #define USE_BG_IMAGE

================================================================
  ANIMATION FRAMES
================================================================

Moi animation gom 9 frames, 64x64 pixel, RGB565A8
Frame rate: 4 fps (250ms/frame)
Tong dung luong: ~540KB flash (5 anim x 9 frames x 12KB)

================================================================
