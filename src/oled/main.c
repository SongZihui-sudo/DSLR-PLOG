#include "oled_0_in_96.h"
#include <stdio.h>

int main(int argc, char **argv) {
  printf("1.3inch OLED (C) test demo\n");
  if (DEV_ModuleInit() != 0) {
    return -1;
  }

  printf("OLED Init...\r\n");
  OLED_Init();

    OLED_Clear();
    OLED_ShowString(0, 0, "DSLR-PLOG-IN ", 12);
    OLED_ShowString(0, 2, "ISO: ", 16);
    OLED_ShowString(0, 4, "F: ", 16);
    OLED_ShowString(0, 6, "S: ", 16);
  return 0;
}
