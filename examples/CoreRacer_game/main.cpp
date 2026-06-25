#include "stdio.h"
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "vecos/scheduler.h"
#include "vecos/queue_message.h"

// --- OLED SSD1306 Full Driver ---
#define OLED_ADDR 0x3C
#define I2C_PORT i2c0
#define PIN_SDA 4
#define PIN_SCL 5

// Framebuffer global (128x64 pixels = 1024 bytes + 1 control byte)
uint8_t frame_buffer[1025];

void oled_cmd(uint8_t cmd) {
    uint8_t buf[2] = {0x00, cmd};
    i2c_write_blocking(I2C_PORT, OLED_ADDR, buf, 2, false);
}

void oled_init() {
    i2c_init(I2C_PORT, 1000 * 1000); // Super fast I2C (1 MHz) for graphics
    gpio_set_function(PIN_SDA, GPIO_FUNC_I2C);
    gpio_set_function(PIN_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(PIN_SDA);
    gpio_pull_up(PIN_SCL);

    sleep_ms(100);
    oled_cmd(0xAE); // Turn off display
    oled_cmd(0x20); oled_cmd(0x00); // Horizontal Addressing Mode
    oled_cmd(0x21); oled_cmd(0x00); oled_cmd(0x7F); // Reset Col
    oled_cmd(0x22); oled_cmd(0x00); oled_cmd(0x07); // Reset Page
    oled_cmd(0x8D); oled_cmd(0x14); // Enable charge pump
    oled_cmd(0xAF); // Turn on display
}

void fb_clear() {
    for(int i = 1; i < 1025; i++) frame_buffer[i] = 0x00;
    frame_buffer[0] = 0x40; // Data control byte
}

void fb_draw_pixel(int x, int y, bool on) {
    if (x < 0 || x >= 128 || y < 0 || y >= 64) return;
    int page = y / 8;
    int pixel_idx = 1 + (page * 128) + x;
    if (on) frame_buffer[pixel_idx] |= (1 << (y % 8));
    else    frame_buffer[pixel_idx] &= ~(1 << (y % 8));
}

void fb_render() {
    // Re-set display pointers to (0,0) before sending full buffer
    oled_cmd(0x21); oled_cmd(0x00); oled_cmd(0x7F);
    oled_cmd(0x22); oled_cmd(0x00); oled_cmd(0x07);
    i2c_write_blocking(I2C_PORT, OLED_ADDR, frame_buffer, 1025, false);
}

// --- Graphics & Bitmaps ---

// Retro Astroguy Bitmap (8x16 pixels)
const uint8_t bmp_astroguy_frame1[] = {
    0x00, 0x1E, 0x21, 0x4D, 0x4D, 0x21, 0x1E, 0x00, // Head
    0x18, 0x7E, 0x3C, 0xFF, 0xDB, 0x24, 0x66, 0x00  // Body
};

const uint8_t bmp_astroguy_frame2[] = {
    0x00, 0x1E, 0x21, 0x4D, 0x4D, 0x21, 0x1E, 0x00, // Head
    0x18, 0x7E, 0x3C, 0xFF, 0xDB, 0x64, 0x26, 0x00  // Body (legs walking)
};

void draw_astroguy(int x, int y, int frame) {
    const uint8_t* bitmap = (frame % 2 == 0) ? bmp_astroguy_frame1 : bmp_astroguy_frame2;
    for (int col = 0; col < 8; col++) {
        uint8_t byte = bitmap[col]; // Top byte (Head)
        for (int b = 0; b < 8; b++) fb_draw_pixel(x + col, y + b, (byte >> b) & 0x01);
        byte = bitmap[col + 8]; // Bottom byte (Body)
        for (int b = 0; b < 8; b++) fb_draw_pixel(x + col, y + b + 8, (byte >> b) & 0x01);
    }
}

void draw_moon_surface() {
    // Simple rocky terrain at the bottom
    for(int x = 0; x < 128; x++) {
        fb_draw_pixel(x, 63, true); // Ground line
        if (x % 16 == 0) fb_draw_pixel(x, 62, true); // Some rock dots
        if ((x + 8) % 16 == 0) fb_draw_pixel(x, 61, true);
    }
}

// --- VectOS Game Logic ---

enum class ButtonEvent {
    MOVE_LEFT,
    MOVE_RIGHT
};

vecos::Queue<ButtonEvent, 8> game_queue;
const uint PIN_BUTTON_LEFT = 2; // Button A
const uint PIN_BUTTON_RIGHT = 3; // Button B

void Task_Input() {
    gpio_init(PIN_BUTTON_LEFT); gpio_set_dir(PIN_BUTTON_LEFT, GPIO_IN); gpio_pull_up(PIN_BUTTON_LEFT);
    gpio_init(PIN_BUTTON_RIGHT); gpio_set_dir(PIN_BUTTON_RIGHT, GPIO_IN); gpio_pull_up(PIN_BUTTON_RIGHT);
    bool last_L = true, last_R = true;

    while(1) {
        bool L = gpio_get(PIN_BUTTON_LEFT);
        bool R = gpio_get(PIN_BUTTON_RIGHT);
        if (last_L && !L) game_queue.send(ButtonEvent::MOVE_LEFT);
        if (last_R && !R) game_queue.send(ButtonEvent::MOVE_RIGHT);
        last_L = L; last_R = R;
        vecos::sleep_task_ms(20);
    }
}

void Task_Game_Engine() {
    oled_init();
    int char_x = 60, char_y = 46, frame = 0;
    printf("[System] CoreRacer Game Started! Meet Astroguy.\n");

    while(1) {
        ButtonEvent event;
        game_queue.receive(event); // Sleep until input

        switch(event) {
            case ButtonEvent::MOVE_LEFT:  char_x = (char_x > 0) ? char_x - 4 : 0; break;
            case ButtonEvent::MOVE_RIGHT: char_x = (char_x < 120) ? char_x + 4 : 120; break;
        }
        frame++; // Advance animation

        // --- RENDER SCENE ---
        fb_clear();
        draw_moon_surface();
        draw_astroguy(char_x, char_y, frame); // Draw bonhomme
        fb_render(); // Send to OLED
    }
}

vecos::Task<512> input_task_handle(Task_Input);
vecos::Task<512> display_task_handle(Task_Game_Engine);
vecos::Scheduler os;

int main() {
    stdio_init_all();
    os.add_task(input_task_handle);
    os.add_task(display_task_handle);
    os.start();
    while (1) { sleep_ms(1000); }    
}