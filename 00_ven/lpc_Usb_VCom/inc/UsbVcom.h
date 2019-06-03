#ifndef USBVCOM_H_
#define USBVCOM_H_

#ifdef __cplusplus
extern "C"
{
#endif

/* USB Initialization */
void USB_init(void);

/* Handle interrupt from USB */
void USB_IRQHandler(void);



#ifdef __cplusplus
}
#endif

#endif /* USBVCOM_H_ */
