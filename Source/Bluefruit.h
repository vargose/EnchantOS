#ifndef SOURCE_BLUEFRUIT_H_
#define SOURCE_BLUEFRUIT_H_

void Bluefruit_Init(void);
void Bluefruit_Poll();

void Bluefruit_Restart();

bool Bluefruit_GetUpdateFlag();
void Bluefruit_ResetUpdateFlag();

uint8_t Bluefruit_GetColorR();
uint8_t Bluefruit_GetColorG();
uint8_t Bluefruit_GetColorB();
bool Bluefruit_GetButton1();
bool Bluefruit_GetButton2();
bool Bluefruit_GetButton3();
bool Bluefruit_GetButton4();
bool Bluefruit_GetButtonUp();
bool Bluefruit_GetButtonDown();
bool Bluefruit_GetButtonLeft();
bool Bluefruit_GetButtonRight();

#endif /* SOURCE_BLUEFRUIT_H_ */
