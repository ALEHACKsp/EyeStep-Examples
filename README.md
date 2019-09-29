# About
The father API of memory exploits,
with an x86 disassembler at its core.

The eyepstep_util header contains all of the functions for
both external and DLL applications.
simply use eyestep::exeutil if you're disassembling a remote application,
and eyestep::dllutil if you're using eyestep in a DLL.



# Function Usage
"direction" can be direction::ahead, or direction::behind.
this determines if it goes forwards or backwards starting
at "function".

nextprologue(function, direction);    goes to the start address of the very next function
nextcall(function, direction);        goes to the very next call instruction, returns the call function
getcalls(function);                   gets all calls used in a function; returns those functions in a list.
fsize(function);                      returns the overall size of a function in bytes
fretn(function);                      returns the stack size ret'd at the end of a function
getconv(function);                    returns the calling convention of a function (as an ID; e.g. 1 = conv_stdcall).
getsconv(conv);                       translates a convention ID to a string, e.g. 1 --> "stdcall"
debug(address, register, offset);     Places a hook, reading the value of [register+offset] at address. For example, if we have a mov ecx,[ebp+8] instruction at 0x12000000, and we do this: uint32_t x=debug(0x12000000, ebp, 8), as soon as that instruction is executed, you will have the value at the offset 8 from ebp. The hook is removed instantly after, preventing detection.



# Writing instructions

use eyestep::write(location, "[instruction here]");

Please note:
All values and offsets are hexidecimal (this is a limited parser atm) and MUST be aligned by 2.
For example do [ebx+0C] instead of [ebx+C].

Examples of usage + notes:

uint32_t addr = 0x12000000;
addr += eyestep::write(addr, "push ebp").len;
addr += eyestep::write(addr, "mov ebp,esp").len;
addr += eyestep::write(addr, "mov [00000040],ecx").len;
addr += eyestep::write(addr, "add [edx+0C],40").len; // technically "add byte ptr" since it adds 0x40
addr += eyestep::write(addr, "sbb ebx,40").len;
addr += eyestep::write(addr, "mov ebx,00000040").len; // use 4-byte int for mov
addr += eyestep::write(addr, "call base+1F20C0").len; // calls RobloxPlayerBeta.exe+1F20C0
addr += eyestep::write(addr, "jmp 12000030").len; // inserts a short jmp to 0x12000030
addr += eyestep::write(addr, "jmp 1300FF30").len; // inserts a long jmp to 0x1300FF30
addr += eyestep::write(addr, "pop ebp").len;
addr += eyestep::write(addr, "ret 04").len;
addr += eyestep::write(addr, "int3").len;

this can make writing asm in another process easier.
however it is completely in development still and
needs optimization, and some rewriting.





# Reading instructions

Let's say we wanna dump a struct; and we have this instruction in a lua function:
.text:00847EFB                 mov eax, [esi+8]

We'll pretend esi is lua state, and we know this instruction uses the top offset of lua state. (L->top).
how can we grab the +8 which is going to be the offset to lua state's "top" property?

// first we read it with eyestep
eyestep::inst i = eyestep::read(0x847EFB);

// then, we can check if the instruction here has both operands (it does)
if (i.flags & Fl_src_dest){

  // then, we can check if the destination (second) operand uses an 8 bit offset ("imm8")
  if (i.flags & Fl_dest_imm8){

    // Now we know it uses an 8 bit offset; let's figure out what the offset is
    uint8_t offset = i.dest.imm8;
    printf("offset: +%i\n", offset); // offset: +8

  }
}


Notes:

i.src and i.dest have the exact same properties;
they are the first and second part of most instructions.
Here's an explanation of their properties:

dest.imm8 is an 8 bit(1 byte) offset from a register if there is one. (like mov eax,[edi+3C])
dest.imm32 is a 32 bit(4 byte/int) offset from a register if there is one. (like mov eax,[edi+0028CDBE])
dest.disp8 is an 8 bit(1 byte) fixed value. (like mov eax,3C)
dest.disp32 is a 32 bit(4 byte/int) fixed value. (like mov eax,0028CDBE)
dest.r32 is the first 32-bit register in the destination operand
dest.r_2 is the second register in the destination operand, following the same bit-size as the first one.
