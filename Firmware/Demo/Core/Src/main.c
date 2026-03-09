#include "Std_Types.h"
#include "App.h"
#include "Mcal.h"
#include "Bsw.h"
#include "Mcal_Lcfg.h"

int main(void)
{
  Mcal_Init();
  Bsw_Init();
  Application_Init();

  while (1)
  {
    Application_MainFunction();
    // DELAY_MS(MAIN_TICK_DELAY_MS);
  }
}
