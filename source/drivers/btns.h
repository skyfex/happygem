
typedef void (*btns_callback_t)(uint8_t);

void btns_init(btns_callback_t callback);
void btns_process();

uint8_t btn_is_down(uint8_t btn_id);