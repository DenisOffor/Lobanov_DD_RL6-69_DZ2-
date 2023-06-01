

/* Includes */
#include "serial_receive_and_paralell_transfer.h"

int main(void)
{
  Tune_peripheries();
  while (1)
  {
	  check_button_press();
	  manage_states();
	  process_cmd();
  }
}
