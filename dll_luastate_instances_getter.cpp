#include <iostream>
#include <string>
#include "eyestep.h"
#include "eyestep_util.h"
#include "eyestep_rbx.h"

void main() {
  rbx::open_console("Bypassed Console with EyeStep");
  rbx::patch_console();//patch console so it closes again
  
  
  eyestep::use(GetCurrentProcess()); //hook eyestep to the current process (aka roblox since dll is injected in roblox)
  rbx::patch_raiseexception(); //patch raise exception
  rbx::instances::fetch(); //fetch instances
  
  int rL = rbx::get_lua_state(rbx::instances::script_context); //fetch lua state
  printf("Lua State: %08X.\n", rL); //print it
}
