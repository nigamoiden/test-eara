# ESP32 LVGL Audio Hub + LeafDra Pet

Firmware Arduino cho ESP32 + màn hình ILI9341 320×240 cảm ứng (XPT2046),
dùng LVGL 9.x. Gồm 6 màn hình: Home (kèm pet animation), Music Browser,
Now Playing, Incoming Call, In-Call và Settings. Pet "LeafDra" có 5 trạng
thái animation (walk / play / happy / angry / love), đồng bộ mood với
chatbot qua HTTP.

## Cấu trúc thư mục

```
LVGL_ESP32/
├── README.md                  ← file này
├── docs/
│   └── LVGL_README.txt        ← hướng dẫn cài đặt thư viện chi tiết (tiếng Việt)
├── firmware/
│   └── ESP32_LVGL/            ← Arduino sketch (mở folder này bằng Arduino IDE)
│       ├── ESP32_LVGL.ino     ← code chính, 6 màn hình
│       ├── lv_conf.h          ← config LVGL 9.x (xem docs/ để biết nơi đặt)
│       ├── bg_image.h         ← header background (cần resize trước)
│       ├── walk_frames/       ← 9 PNG + .c/.h (RGB565A8, 64×64) — đi bộ
│       ├── play_frames/       ← chơi (khi tap)
│       ├── happy_frames/      ← vui
│       ├── angry_frames/      ← giận
│       └── love_frames/       ← yêu thương
├── tools/                     ← Python scripts xử lý sprite
│   ├── extract_dragon.py      ← trích frame từ ảnh nguồn
│   ├── find_face.py           ← dò vùng mặt
│   ├── find_face2.py
│   └── export_preview_png.py  ← export preview PNG
├── preview/                   ← preview HTML chạy trên trình duyệt
│   ├── preview.html
│   ├── preview2.html
│   ├── preview3.html
│   ├── test_transparency.html
│   └── frames_data.js         ← dữ liệu frames dạng base64 cho preview
├── assets/                    ← ảnh nguồn & sprite trung gian
│   ├── bg_source.jpg          ← background gốc 1628×640 (cần resize 320×240)
│   ├── dragon_img.c           ← ảnh dragon dạng C-array (LVGL)
│   ├── dragon_annotated.png
│   ├── dragon_head_zoom.png
│   └── dragon_preview.png
```

## Phần cứng

- ESP32 WROOM-32
- Màn hình ILI9341 320×240 cảm ứng (XPT2046)
- Pin mặc định trong sketch: `TOUCH_CS = 21`, `TFT_BL = 15` — sửa ở đầu
  `firmware/ESP32_LVGL/ESP32_LVGL.ino` cho khớp board

## Thư viện cần cài (Arduino Library Manager)

1. **lvgl** 9.x — by LVGL
2. **TFT_eSPI** — by Bodmer
3. **XPT2046_Touchscreen** — by Paul Stoffregen

## Cài đặt nhanh

1. Cài 3 thư viện ở trên qua Arduino Library Manager.
2. Copy [lv_conf.h](firmware/ESP32_LVGL/lv_conf.h) vào `Arduino/libraries/`
   (đặt **cùng cấp** với folder `lvgl/`, không phải bên trong).
3. Sửa `Arduino/libraries/TFT_eSPI/User_Setup.h` theo board (xem
   [docs/LVGL_README.txt](docs/LVGL_README.txt) để có khối config mẫu cho
   ESP32 WROOM-32).
4. Mở [firmware/ESP32_LVGL/ESP32_LVGL.ino](firmware/ESP32_LVGL/ESP32_LVGL.ino)
   bằng Arduino IDE.
5. Chọn board **ESP32 Dev Module**, partition **Default 4MB with spiffs**,
   upload speed **921600**, rồi Upload.

Hướng dẫn đầy đủ kèm troubleshooting nằm trong
[docs/LVGL_README.txt](docs/LVGL_README.txt).

## Pet & Chatbot integration

- Mặc định pet chạy animation `walk`. Tap vào pet → chuyển sang `play` 1
  vòng rồi quay về `walk`.
- 3 nút mood (Happy / Angry / Love) đặt animation tương ứng và lưu state.
- Helper trong sketch:
  - `get_current_emotion()` → `"happy"` / `"angry"` / `"love"` —
    gửi kèm trong HTTP request lên server chatbot.
  - `set_emotion_from_server()` — nhận field `mood` từ response để pet
    tự đổi animation.

Ví dụ payload:

```json
// request
{ "message": "xin chao", "mood": "happy" }

// response
{ "reply": "chao ban!", "mood": "love" }
```

## Animation frames

- Mỗi animation: 9 frames, 64×64 px, định dạng **RGB565A8**.
- Frame rate: 4 fps (250 ms/frame).
- Tổng dung lượng flash: ~540 KB (5 anim × 9 frames × 12 KB).

## Background image

File gốc [assets/bg_source.jpg](assets/bg_source.jpg) là 1628×640
(~3.1 MB) — quá lớn cho ESP32. Trước khi dùng, resize về 320×240 bằng
[LVGL Image Converter](https://lvgl.io/tools/imageconverter), thay
`bg_image.h` rồi uncomment `#define USE_BG_IMAGE` trong file `.ino`.

## Tools (Python)

Yêu cầu Python 3 + Pillow / numpy. Các script trong [tools/](tools/) dùng
để bóc tách sprite dragon từ ảnh nguồn, dò vùng mặt, và export preview
PNG. Output debug được ghi vào [debug/](debug/).

## Preview trên trình duyệt

Mở [preview/preview.html](preview/preview.html) (hoặc `preview2.html` /
`preview3.html`) trong Chrome/Edge để xem trước animation mà không cần
nạp lên ESP32. Dữ liệu frames được nhúng sẵn trong
[preview/frames_data.js](preview/frames_data.js).
