// Delete this line if you don't use precompiled header in your project
#include "stdafx.h"

#include <CrashRpt.h>
/*
   +-------------------------------------------------+
   | @ {AppName}                                   X |
   +-------------------------------------------------+
   |                                                 |
   | {AppName} has stopped working                   |
   |                                                 |
   | Sending collected information to the {Company}. |
   | This might take several minutes...   +--------+ |
   |                                      | Cancel | |
   |                                      +--------+ |
   +-------------------------------------------------+
*/

// global crash handler
crash_rpt::CrashRpt g_crashRpt(
	"d64cc75f-0971-42fc-8a85-d91bb1adf107",
	L"OrionUO Client",
	L"OrionUO Team"
	);