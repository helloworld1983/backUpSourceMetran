#ifndef SYSTEMCONFIGURE_H_
#define SYSTEMCONFIGURE_H_

#define SOLENOI_TASK_SIZE                          (5*configMINIMAL_STACK_SIZE)
#define SOLENOI_TASK_PRIORITY                      (tskIDLE_PRIORITY + 1UL)

#define ADC_TASK_SIZE                              (5*configMINIMAL_STACK_SIZE)
#define ADC_TASK_PRIORITY                          (tskIDLE_PRIORITY + 2UL)

#define FLOWRS485_TASK_SIZE                        (2*1024)
#define FLOWRS485_TASK_PRIORITY                    (tskIDLE_PRIORITY + 4UL)

#define FLOWI2C_TASK_SIZE                          (5*configMINIMAL_STACK_SIZE)
#define FLOWI2C_TASK_PRIORITY                      (tskIDLE_PRIORITY + 3UL)

#define HFOSPI_TASK_SIZE                           (5*configMINIMAL_STACK_SIZE)
#define HFOSPI_TASK_PRIORITY                       (tskIDLE_PRIORITY + 5UL)

#define USB_TASK_SIZE                              (8*configMINIMAL_STACK_SIZE)
#define USB_TASK_PRIORITY                          (tskIDLE_PRIORITY + 6UL)




#endif /* SYSTEMCONFIGURE_H_ */
