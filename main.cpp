#include "mbed.h"
#include "InterfaceCAN.h"

#define CAN1_RX PD_0    // RX port
#define CAN1_TX PD_1    // TX port


#define VESC_DUTY_EXTENDED_ID 0x00   // ID to run duty cycle mode

#define VESC_ID 101     // ID of the Vesc

CAN can(CAN1_RX, CAN1_TX, 500000);  // Create the can object w/ 500k baud rate
DigitalOut led2(LED2);  // Declare D_OUT for LED

// Function to concatenate Vesc ID to the Vesc mode
static constexpr uint32_t VESC_DUTY_ID(const uint32_t &vesc_id) {
    return (static_cast<uint32_t>(VESC_DUTY_EXTENDED_ID)
        << sizeof(uint8_t) * 8) |
        static_cast<uint32_t>(vesc_id);
}

// Function to encode message within uint8_t array
void buffer_append_uint32(uint8_t *buffer, uint32_t number, int32_t *index) {
  buffer[(*index)++] = number >> 24;
  buffer[(*index)++] = number >> 16;
  buffer[(*index)++] = number >> 8;
  buffer[(*index)++] = number;
}

void send() {
    const int32_t rpm = 20000;   // 20%
    uint8_t message[4] = {0, 0, 0, 0};  // Blank message
    int32_t idx = 0;    // 0th index

    // Fill in the message
    buffer_append_uint32(&message[0], rpm, &idx);

    // Create the extended ID
    auto extended_id = VESC_DUTY_ID(VESC_ID);

    // Create the CAN message
    auto msg = CANMessage(extended_id, &message[0],
                                  sizeof(message));

    // Write the message
    if(can.write(msg)) {
        // Blink LED if writing
        led2 = !led2;
    };
}

int main() {
    while(1) { send(); }
}