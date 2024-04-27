#include "CyberSeaquell.h"

int main() {
	if (!drill_lib_init()) {
		return EXIT_FAILURE;
	}
	CyberSeaquell::run_cyber_seaquell();
	return 0;
}