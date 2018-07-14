#ifndef UARM_CODE
#define UARM_CODE

void testSerialUno();
void configureUArm();
void getFood(int storage[]);
void recvWithStartEndMarkers();
void parseData();
void showParsedData();
static void Init();
void run();
void tickTaskRun();
void manage_inactivity(void);

#endif