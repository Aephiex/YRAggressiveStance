#include <CCINIClass.h>
#include "Commands.h"

#include "AggressiveStance.h"

DEFINE_HOOK(0x533066, CommandClassCallback_Register, 0x6)
{
	MakeCommand<AggressiveStanceClass>();
	return 0;
}
