#include "entrypoint.hh"

#include "can.h"
#include "main.h"
#include "stm32f407xx.h"

#include "stm32f4xx_hal_gpio.h"
#include "tim.h"

#include "pid_controller.hpp"
#include "motor.hpp"
#include <cstdint>
#include <cstdio>
#include <numbers>

float speed_target = 0;

float speed_current = 0.0;

int current_button_state = 1;


pid_controller pid_ctl_6020(2500,2.0,1);

float speed_output_6020=0.0;

uint8_t can_data_buffer[8];
uint8_t receive_sensor_buffer[10];
CAN_RxHeaderTypeDef can_rx_header_buffer;

/// @brief 电机控制
void motor_control_loop() {

    // analyze the messages of global
    // calculate target current
    // send can message

    speed_output_6020 = pid_ctl_6020.calculate_speed(speed_target, speed_current);
    set_motor_speed( speed_output_6020);
}

/// @brief 电机信息处理
void handle_motor_data() {

    // deal with the data from can
    // update the global values
    // 回报文头判断
    HAL_CAN_GetRxMessage(&hcan1, CAN_RX_FIFO0, &can_rx_header_buffer, can_data_buffer);

  if (can_rx_header_buffer.StdId == id_6020) {
        get_motor_status(can_data_buffer, m6020);
    speed_current = m6020.speed *std::numbers::pi/ 60.;
    }
}

// @brief 开关处理
void handle_sensor_data() {
    // handle the data from serial
    // parse the data
    // update the values

    // static bool button_pressed = true;
    if(HAL_GPIO_ReadPin(GPIOA,GPIO_PIN_0)==GPIO_PIN_RESET)
    {
        current_button_state++;
    };
    if (current_button_state%3==1){
        speed_target=0;
    }else if (current_button_state%3==0) {
        speed_target= 0.8*std::numbers::pi;
    }else if(current_button_state%3==2){
        speed_target=-0.8*std::numbers::pi;
    }
}

void entrypoint() {

    init_motor();

    HAL_TIM_Base_Start_IT(&htim14);

    while (true) {
        // motor_control_loop();
        // keep the led running

          handle_sensor_data();
    }
}

extern "C" {
/*1000 Hz*/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef* htim) {
    if (htim->Instance == TIM14) {
        // handle_motor_data();
        motor_control_loop();
        if(HAL_GPIO_ReadPin(LEDB_GPIO_Port, LEDB_Pin)==GPIO_PIN_RESET){
             HAL_GPIO_WritePin(LEDB_GPIO_Port, LEDB_Pin,GPIO_PIN_SET);
            }
        else {
             HAL_GPIO_WritePin(LEDB_GPIO_Port, LEDB_Pin,GPIO_PIN_RESET);
            }          
     
       
        //  uint8_t target=speed_target*100;
        // uint8_t speed[2];
        // speed[0]=speed_target*100;
        // speed[1]=speed_current*100;
        //  HAL_UART_Transmit(&huart1,speed,sizeof(speed),500);

    }
}

void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef* hcan) {
    if (hcan == &hcan1)
        handle_motor_data();
}
// void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef* huart, uint16_t size) {
//     if (huart == &huart1) {
//         if (size == 8)
            
//         HAL_UARTEx_ReceiveToIdle_IT(&huart1, receive_commend_buffer, size);
//          printf("target:%.2f,current:%.2f",speed_target,speed_current);
       

//     }
// }
}