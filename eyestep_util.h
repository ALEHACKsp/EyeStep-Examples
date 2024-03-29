#ifndef EYESTEP_UTIL
#define EYESTEP_UTIL
#include "eyestep.h"

#define pmread	 ReadProcessMemory
#define pmwrite	 WriteProcessMemory
#define vcast(x) reinterpret_cast<void*>(x)

namespace PEB {
	typedef struct _LIST_ENTRY {
		struct _LIST_ENTRY *Flink;
		struct _LIST_ENTRY *Blink;
	} LIST_ENTRY, *PLIST_ENTRY, *RESTRICTED_POINTER PRLIST_ENTRY;

	typedef struct _PEB_FREE_BLOCK {
		void*				Next; // PPEB_FREE_BLOCK
		ULONG				Size;
	} PEB_FREE_BLOCK, *PPEB_FREE_BLOCK;


	struct THREAD_BASIC_INFORMATION {
		unsigned long ExitStatus;
		unsigned long TEBAddress;
		unsigned long shit[0x5]; //Only to preserve the structure's size
	};

	typedef enum _MEMORY_INFORMATION_CLASS {
		MemoryBasicInformation
	} MEMORY_INFORMATION_CLASS;


	typedef struct _PEB_LDR_DATA {
		UCHAR			Reserved1[8];
		PVOID			Reserved2[3];
		LIST_ENTRY		InMemoryOrderModuleList;
	} PEB_LDR_DATA, *PPEB_LDR_DATA;


	typedef struct _UNICODE_STRING {
		USHORT			Length;
		USHORT			MaximumLength;
		WCHAR *			Buffer;
	} UNICODE_STRING, *PUNICODE_STRING;


	typedef struct _RTL_USER_PROCESS_PARAMETERS {
		UCHAR			Reserved1[16];
		PVOID			Reserved2[10];
		UNICODE_STRING	ImagePathName;
		UNICODE_STRING	CommandLine;
	} RTL_USER_PROCESS_PARAMETERS, *PRTL_USER_PROCESS_PARAMETERS;


	typedef struct _IO_STATUS_BLOCK {
		union {
			LONG		Status; // NTSTATUS
			PVOID		Pointer;
		} DUMMYUNIONNAME;
		ULONG_PTR		Information;
	} IO_STATUS_BLOCK, *PIO_STATUS_BLOCK;


	typedef struct _OBJECT_ATTRIBUTES {
		ULONG           Length;
		HANDLE          RootDirectory;
		PUNICODE_STRING ObjectName;
		ULONG           Attributes;
		PVOID           SecurityDescriptor;
		PVOID           SecurityQualityOfService;
	} OBJECT_ATTRIBUTES;
	typedef OBJECT_ATTRIBUTES* POBJECT_ATTRIBUTES;

	struct PEB {
		/*0x000 */ BYTE InheritedAddressSpace;
		/*0x001 */ BYTE ReadImageFileExecOptions;
		/*0x002 */ BYTE BeingDebugged;
		/*0x003 */ BYTE SpareBool;
		/*0x004 */ void* Mutant;
		/*0x008 */ void* ImageBaseAddress;
		/*0x00C */ _PEB_LDR_DATA* Ldr;
		/*0x010 */ _RTL_USER_PROCESS_PARAMETERS* ProcessParameters;
		/*0x014 */ void* SubSystemData;
		/*0x018 */ void* ProcessHeap;
		/*0x01C */ _RTL_CRITICAL_SECTION* FastPebLock;
		/*0x020 */ void* FastPebLockRoutine;
		/*0x024 */ void* FastPebUnlockRoutine;
		/*0x028 */ DWORD EnvironmentUpdateCount;
		/*0x02C */ void* KernelCallbackTable;
		/*0x030 */ DWORD SystemReserved[1];
		/*0x034 */ DWORD ExecuteOptions : 2; // bit offset: 34, len=2
		/*0x034 */ DWORD SpareBits : 30; // bit offset: 34, len=30
		/*0x038 */ _PEB_FREE_BLOCK* FreeList;
		/*0x03C */ DWORD TlsExpansionCounter;
		/*0x040 */ void* TlsBitmap;
		/*0x044 */ DWORD TlsBitmapBits[2];
		/*0x04C */ void* ReadOnlySharedMemoryBase;
		/*0x050 */ void* ReadOnlySharedMemoryHeap;
		/*0x054 */ void** ReadOnlyStaticServerData;
		/*0x058 */ void* AnsiCodePageData;
		/*0x05C */ void* OemCodePageData;
		/*0x060 */ void* UnicodeCaseTableData;
		/*0x064 */ DWORD NumberOfProcessors;
		/*0x068 */ DWORD NtGlobalFlag;
		/*0x070 */ _LARGE_INTEGER CriticalSectionTimeout;
		/*0x078 */ DWORD HeapSegmentReserve;
		/*0x07C */ DWORD HeapSegmentCommit;
		/*0x080 */ DWORD HeapDeCommitTotalFreeThreshold;
		/*0x084 */ DWORD HeapDeCommitFreeBlockThreshold;
		/*0x088 */ DWORD NumberOfHeaps;
		/*0x08C */ DWORD MaximumNumberOfHeaps;
		/*0x090 */ void** ProcessHeaps;
		/*0x094 */ void* GdiSharedHandleTable;
		/*0x098 */ void* ProcessStarterHelper;
		/*0x09C */ DWORD GdiDCAttributeList;
		/*0x0A0 */ void* LoaderLock;
		/*0x0A4 */ DWORD OSMajorVersion;
		/*0x0A8 */ DWORD OSMinorVersion;
		/*0x0AC */ WORD OSBuildNumber;
		/*0x0AE */ WORD OSCSDVersion;
		/*0x0B0 */ DWORD OSPlatformId;
		/*0x0B4 */ DWORD ImageSubsystem;
		/*0x0B8 */ DWORD ImageSubsystemMajorVersion;
		/*0x0BC */ DWORD ImageSubsystemMinorVersion;
		/*0x0C0 */ DWORD ImageProcessAffinityMask;
		/*0x0C4 */ DWORD GdiHandleBuffer[34];
		/*0x14C */ void(*PostProcessInitRoutine)();
		/*0x150 */ void* TlsExpansionBitmap;
		/*0x154 */ DWORD TlsExpansionBitmapBits[32];
		/*0x1D4 */ DWORD SessionId;
		/*0x1D8 */ _ULARGE_INTEGER AppCompatFlags;
		/*0x1E0 */ _ULARGE_INTEGER AppCompatFlagsUser;
		/*0x1E8 */ void* pShimData;
		/*0x1EC */ void* AppCompatInfo;
		/*0x1F0 */ _UNICODE_STRING CSDVersion;
		/*0x1F8 */ void* ActivationContextData;
		/*0x1FC */ void* ProcessAssemblyStorageMap;
		/*0x200 */ void* SystemDefaultActivationContextData;
		/*0x204 */ void* SystemAssemblyStorageMap;
		/*0x208 */ DWORD MinimumStackCommit;
	};
}

namespace eyestep {
	const int MAX_STR_READ = 1024;

	bool isgood(uint32_t addr) {
		return (addr>base && addr<0x3FFFFFFF && addr%4==0);
	}

	typedef uint8_t conv;
	conv conv_cdecl		= 0;
	conv conv_stdcall	= 1;
	conv conv_thiscall	= 2;
	conv conv_fastcall	= 3;

	struct cbyte {
		std::vector<uint8_t>bytes;
		void add(uint8_t b) {
			bytes.push_back(b);
		}
		size_t size() {
			return bytes.size();
		}
		// constructors
		cbyte() {
			bytes = std::vector<uint8_t>();
		}
		cbyte(const char* str) {
			bytes = std::vector<uint8_t>();
			std::string newstr = "";
			for (int i = 0; i < lstrlenA(str); i++) {
				if (str[i] != 0x20) {
					newstr += str[i];
				}
			}
			if ((newstr.length() / 2) > 0 && (newstr.length() % 2) == 0) {
				for (int i = 0; i < newstr.length(); i += 2) {
					char s[2];
					s[0] = newstr[i];
					s[1] = newstr[i + 1];
					add(convert::to_hex(s));
				}
			}
		}
		cbyte(uint8_t* byte_array, size_t count) {
			bytes = std::vector<uint8_t>();
			if (sizeof(byte_array) > 0) {
				for (int i = 0; i < count; i++) {
					add(byte_array[i]);
				}
			}
		}
		// cool functions
		uint8_t at(int index) {
			if (index < bytes.size() && index >= 0) {
				return bytes[index];
			}
			else {
				throw std::exception("BAD CBYTE INDEX\n");
				return 0;
			}
		}
		std::string to_string() {
			std::string str = "";
			for (int i = 0; i < bytes.size(); i++) {
				str += convert::to_str(bytes[i]);
				if (i != (size() - 1))
					str += ", ";
				else
					str += ".";
			}
			return str;
		};
	};

	namespace exeutil {
		uint8_t* readb(uint32_t addr, size_t count){ uint8_t* x = new uint8_t[count]; pmread(handle, vcast(addr), x, count, 0); return x; }
		uint8_t readb(uint32_t addr) { uint8_t		x = 0; pmread(handle, vcast(addr), &x, 1, 0); return x; }
		uint16_t readus(uint32_t addr) { uint16_t	x = 0; pmread(handle, vcast(addr), &x, 2, 0); return x; }
		uint32_t readui(uint32_t addr) { uint32_t	x = 0; pmread(handle, vcast(addr), &x, 4, 0); return x; }
		uint64_t readull(uint32_t addr) { uint64_t	x = 0; pmread(handle, vcast(addr), &x, 8, 0); return x; }
		char	readc(uint32_t addr) { char			x = 0; pmread(handle, vcast(addr), &x, 1, 0); return x; }
		int16_t reads(uint32_t addr) { int16_t		x = 0; pmread(handle, vcast(addr), &x, 2, 0); return x; }
		int32_t readi(uint32_t addr) { int32_t		x = 0; pmread(handle, vcast(addr), &x, 4, 0); return x; }
		int64_t readll(uint32_t addr) { int64_t		x = 0; pmread(handle, vcast(addr), &x, 8, 0); return x; }
		float readf(uint32_t addr) { float			x = 0; pmread(handle, vcast(addr), &x, 4, 0); return x; }
		double readd(uint32_t addr) { double		x = 0; pmread(handle, vcast(addr), &x, 8, 0); return x; }
		void write(uint32_t addr, uint8_t* v, size_t count) { pmwrite(handle, vcast(addr), v, count, 0); }
		void write(uint32_t addr, uint8_t v)  { pmwrite(handle, vcast(addr), &v, 1, 0); }
		void write(uint32_t addr, uint16_t v) { pmwrite(handle, vcast(addr), &v, 2, 0); }
		void write(uint32_t addr, uint32_t v) { pmwrite(handle, vcast(addr), &v, 4, 0); }
		void write(uint32_t addr, uint64_t v) { pmwrite(handle, vcast(addr), &v, 8, 0); }
		void write(uint32_t addr, char v)     { pmwrite(handle, vcast(addr), &v, 1, 0); }
		void write(uint32_t addr, int16_t v)  { pmwrite(handle, vcast(addr), &v, 2, 0); }
		void write(uint32_t addr, int32_t v)  { pmwrite(handle, vcast(addr), &v, 4, 0); }
		void write(uint32_t addr, int64_t v)  { pmwrite(handle, vcast(addr), &v, 8, 0); }
		void write(uint32_t addr, float v)	  { pmwrite(handle, vcast(addr), &v, 4, 0); }
		void write(uint32_t addr, double v)	  { pmwrite(handle, vcast(addr), &v, 8, 0); }
		void write(uint32_t addr, cbyte v)	  { pmwrite(handle, vcast(addr), v.bytes.data(), v.bytes.size(), 0); }
		
		std::string sreadb(uint32_t addr, size_t count) {
			std::string str="";
			if (count!=0){
				uint8_t* x=new uint8_t[count];
				pmread(handle, vcast(addr), x, count, 0);
				for (int i=0;i<count;i++){
					str+=convert::to_str(x[i]);
				}
				delete x;
			}
			return str;
		}

		std::string readstring(uint32_t addr) {
			uint32_t ptr = readui(addr);
			if (!(isgood(ptr))) ptr=addr;
			std::string str = "";
			char c[MAX_STR_READ];
			pmread(handle, vcast(ptr), &c, MAX_STR_READ, 0);
			int i=0;
			while (i<MAX_STR_READ && c[i]>=0x20 && c[i]<=0x7E) {
				str+=c[i++];
			}
			return str;
		}

		namespace edit {
			struct PROTECT_INFO {
				uint32_t address;
				size_t size;
				DWORD oldProtect;
			} last_unprotect;

			PROTECT_INFO unprotect(uint32_t address, size_t size) {
				PROTECT_INFO x;
				x.address = address;
				x.size = size;
				VirtualProtectEx(handle, vcast(address), size, 0x40, &x.oldProtect);
				last_unprotect = x;
				return x;
			}

			// restores page protection after calling unprotect()
			void restore(PROTECT_INFO x) {
				DWORD nothing = 0;
				VirtualProtectEx(handle, vcast(x.address), x.size, x.oldProtect, &nothing);
			}
			void restore() { restore(last_unprotect); }
		}

		bool isepilogue(uint32_t address) {
			uint8_t* b = readb(address, 2);
			bool x =((b[0]==0x5D || b[0]==0x5E) && // pop ebp, or pop esi,
					 (b[1]==0xC2 || b[1]==0xC3));  // with a retn or ret XX
			delete b;
			return x;
		}

		bool isprologue(uint32_t address) {
			if (!(address%16==0)) return false;
			std::string s = sreadb(address, 3);
			if (s == "558BEC") return true; // standard prologue
			if (s == "568BF1") return true; // some prologues use ESI
			return false;
		}

		uint32_t nextprologue(uint32_t address, direction d, bool aligned = true){
			uint32_t at = address, count = 0;
			// Skip this prologue if we're already at one
			if (isprologue(at)){
				if (d == behind) at -= 16;
				if (d == ahead)  at += 16;
			}
			while (!isprologue(at) && !(at>base+base_size && readll(at) == 0 && readll(at+8) == 0)) {
				if (count++ > 0xFFFF) break;
				if (d == ahead)  if (!aligned) at++; else at += 16;
				if (d == behind) if (!aligned) at--; else at -= 16;
			}
			return at;
		}

		uint32_t nextepilogue(uint32_t address, direction d){
			uint32_t at = address, count = 0;
			while (!isepilogue(at)){
				if (count++ > 0xFFFF) break;
				if (d == ahead)  at++;
				if (d == behind) at--;
			}
			return (at+1); // Return the functions retn address
		}

		int fsize(uint32_t func) {
			uint32_t eof = nextprologue(func, ahead, true);
			return static_cast<int>(eof - func);
		}

		uint32_t getprologue(uint32_t addr) {
			return nextprologue(addr, behind, false);
		}

		std::vector<uint32_t> getprologues(uint32_t func, direction d, int count) {
			std::vector<uint32_t> result_list = std::vector<uint32_t>();
			uint32_t addr = func, current = 0;
			while (current < count){
				addr = nextprologue(addr, d, true);
				result_list.push_back(addr);
				current++;
			}
			return result_list;
		}

		uint32_t getepilogue(uint32_t func) {
			return nextepilogue(nextprologue(func, ahead, true), behind);
		}

		std::vector<uint32_t> getepilogues(uint32_t func) {
			std::vector<uint32_t> result_list = std::vector<uint32_t>();
			uint32_t start = func;
			uint32_t end = (start + fsize(func));
			while (start < end) {
				if (isepilogue(start)){
					result_list.push_back(start+1);
				}
				start++;
			}
			return result_list;
		}

		short fretn(uint32_t func) {
			for (uint32_t addr : getepilogues(func)) {
				if (readb(addr) == 0xC2) {
					return reads(addr+1);
				}
			}
			return 0;
		}

		std::vector<uint32_t> getcalls(uint32_t func) {
			std::vector<uint32_t> result_list = std::vector<uint32_t>();
			uint32_t funcSize = fsize(func);
			for (int i=0; i<funcSize; i++){
				if (readb(func+i) == 0xE8){
					uint32_t o = (func+i)+readui(func+i+1)+5;
					if (o%16==0 && o>base && o<base+base_size){
						result_list.push_back(o);
					}
				}
			}
			return result_list;
		}

		std::vector<uint32_t> getpointers(uint32_t func) {
			std::vector<uint32_t> result_list = std::vector<uint32_t>();
			uint32_t start = func;
			uint32_t end = (func + fsize(func));
			while (start < end) {
				inst i = read(start);
				uint32_t p = 0;
				if (i.flags & Fl_src_disp32)	p = i.src.disp32;
				if (i.flags & Fl_dest_disp32)	p = i.dest.disp32;
				if (i.flags & Fl_src_imm32)		p = i.src.imm32;
				if (i.flags & Fl_dest_imm32)	p = i.dest.imm32;
				if (p != 0){
					if (p%4==0 && p>base && p<base+base_size) result_list.push_back(p);
					if (p%4==0 && p>base && p<base+base_size) result_list.push_back(p);
				}
				start += i.len;
			}
			return result_list;
		}

		uint32_t nextcall(uint32_t func, direction d, bool loc = false){
			uint32_t start = func;
			// Skip current call if we're already at one
			if (readb(start) == 0xE8){
				if (d == ahead)  start++;
				if (d == behind) start--;
			}
			while (readb(start) != 0xE8){
				if (d == ahead)  start++;
				if (d == behind) start--;
			}
			uint32_t o = (start+readui(start+1)+5);
			if (o%16==0 && o>base && o<base+base_size)
				if (!loc)
					return o;
				else
					return start;
			return 0;
		}

		uint32_t getcall(uint32_t addr){
			uint32_t o = (addr+readui(addr+1)+5);
			if (o%16==0 && o>base && o<base+base_size)
				return o;
			return 0;
		}

		conv getconv(uint32_t func) {
			conv c				= conv_cdecl;
			uint32_t eof		= nextprologue(func, ahead);
			uint32_t at			= func;
			bool not_fastcall	= false;
			bool neither		= false;
			if (fretn(func))  c = conv_stdcall;

			while (at < eof) {
				inst i = eyestep::read(at);
				at += i.len;
				// if edx or ecx was pushed thats an instant indication
				if (strcmp(i.opcode, "push") == 0){
					if (i.src.r32 == ecx) neither		= true;
					if (i.src.r32 == edx) not_fastcall	= true;
				}
				// completely ignore a test instruction, with ecx,ecx or edx,edx
				if (strcmp(i.opcode, "test") == 0) {
					if ((i.src.r32 == ecx && i.dest.r32 == ecx) || (i.src.r32 == edx && i.dest.r32 == edx)) {
						continue;
					}
				}
				// DONT ignore a xor instruction, with ecx,ecx or edx,edx
				if (strcmp(i.opcode, "xor") == 0) {
					if (i.src.r32 == ecx && i.dest.r32 == ecx) {
						neither = true;
						continue;
					} else if (i.src.r32 == edx && i.dest.r32 == edx) {
						not_fastcall = true;
						continue;
					}
				}
				if (i.flags & Fl_src_r32 || i.flags & Fl_src_rm32){
					if (strcmp(i.opcode, "mov") == 0 || strcmp(i.opcode, "lea") == 0 ||
						strcmp(i.opcode, "movzx") == 0 || strcmp(i.opcode, "or") == 0 ||
						strcmp(i.opcode, "xor") == 0
						){
						// because something like a mov ecx,edx
						// can also throw things off
						if (i.dest.r32 == edx && /*!neither && */!not_fastcall) {
							// if !neither is included,
							// it wont work if there's a push ecx prior to mov ???, edx
							// even though some __fastcall's/__thiscall's push ecx, fuck knows why
							return conv_fastcall;
						}
						// it will take a mov or lea instruction with ecx or edx
						// to indicate it is NOT a fastcall or thiscall.
						// But, something like "or ecx,1" has ecx in the src operand,
						// and its using it as if it was passed to the function
						if (i.src.r32 == ecx) neither = true;
						if (i.src.r32 == edx) not_fastcall = true;
					}
				}
				// now, we check if an instruction USES edx or ecx after
				// all that has passed, now that it's determined whether
				// it was or wasn't passed to this function
				if (i.flags & Fl_dest_r32 || i.flags & Fl_dest_rm32){
					if (i.dest.r32 == edx){
						if (!neither && !not_fastcall){
							return conv_fastcall;
						}
					}
					if (i.dest.r32 == ecx){
						if (c != conv_fastcall && !neither){
							c = conv_thiscall;
						}
					}
					if (not_fastcall && c == conv_thiscall) {
						return conv_thiscall;
					}
				}
			}
		
			return c;
		}

		const char* getsconv(conv c) {
			return (c==0)?"cdecl":(c==1)?"stdcall":(c==2)?"thiscall":(c==3)?"fastcall":"";
		}

		// go from [start] location, and search for a call to "func"
		// in the specificied direction.
		// Returns the location of the call.
		// if parentfunc is true it returns the function
		// that has this call in it
		uint32_t nextxref(uint32_t start, direction d, uint32_t func, bool parentfunc = false) {
			uint32_t at = start;
			while (getcall(at) != func) {
				uint32_t old_at = at;
				at = nextcall(at, d, true);
				if (at == 0) if (d==behind) at = --old_at; else at = ++old_at;
			}
			if (!parentfunc) return at;
			return nextprologue(at, behind, false);
		}

		std::vector<uint32_t> __fastcall scan(const char* aob, const char* _mask, uint32_t begin = 0, uint32_t to = 0) {
			uint32_t size=0, at=0, start=begin, end=to;
			if (!(start && end)){
				start = base;
				end = base+base_size;
			}

			uint32_t chunksize		= ((end-start)<(64*64*64))?(end-start):(64*64*64);
			uint8_t* buffer			= new uint8_t[chunksize];
			char match				= 1;
			char wildchar			= 'x'; // 'x' means it can be any byte; put '.' for the rest

			std::vector<uint32_t>	results_list = std::vector<uint32_t>();
			std::vector<uint8_t>	bytes = cbyte(aob).bytes;
			size					= bytes.size();

			// create mask for AOB if string is empty
			char mask[256];
			mask[0] = '\0';
			if (lstrlenA(_mask) == 0){
				for (int i=0; i<size; i++){
					strcat_s(mask, ".");
				}
			} else {
				strcpy_s(mask, _mask);
			}

			uint32_t padding = (size - 1);
			while (start < end){
				bool read = pmread(handle, vcast(start), buffer, chunksize, 0);
				if (read){
					__asm push edi
					__asm xor edi, edi
					__asm jmp L2
				L1: __asm inc edi
					__asm mov at, edi
					match=1;
					for (uint32_t x=0; x<size; x++)
					if (buffer[at+x]!=bytes.at(x) && mask[x]!=wildchar)
					match=0;
					if (match) results_list.push_back(start+at);
				L2:	__asm cmp edi, chunksize
					__asm jb L1
					__asm sub edi, padding
					__asm add start, edi
					__asm pop edi
				} else {
					printf("[!] Failed to read process region [0x%08X]\n", start);
					start += chunksize;
					continue;
				}
			}
			delete buffer;
			return results_list;
		}

		std::vector<uint32_t> __fastcall scanpointer(uint32_t address){
			return scan(convert::to_bytes(address).c_str(), "....");
		}

		// strings are located onward from .rdata
		// roblox detects this and you're kicked, and I never got
		// around to a bypass for it; thats why eyecrawl never worked as a DLL
		std::vector<uint32_t> __fastcall scanxrefs(const char* str, int result_number = 0) {
			char* mask = new char[lstrlenA(str)];
			for (int i=0; i<lstrlenA(str); i++){
				mask[i]='.';
			}
			std::vector<uint32_t>strlocs = scan(convert::to_bytes(str).c_str(), mask);
			delete mask;
			if (strlocs.size() == 0) return strlocs;
			return scanpointer(strlocs[result_number]);
		}

		// xrefs made to a function
		std::vector<uint32_t> __fastcall scanxrefs(uint32_t func, uint32_t begin = 0, uint32_t to = 0){
			uint32_t size=0, at=0, start=begin, end=to;
			if (!(start && end)){
				start = eyestep::base;
				end = eyestep::base + eyestep::base_size;
			}

			uint32_t chunksize		= ((end-begin)<(64*64*32))?(end-begin):(64*64*32);
			uint8_t* buffer			= new uint8_t[chunksize];
			bool found				= false;
			std::vector<uint32_t>	xrefs = std::vector<uint32_t>();

			while (start < end && start < base + base_size){
				bool read = pmread(handle, vcast(start), buffer, chunksize, 0);
				if (read){
					for (int i=0; i<chunksize; i++){
						if (readui(start+i) == 0xE8){ // call instruction
							// calculate relative offset
							uint32_t o = readui(start+i+1);
							if ((start+i+5+o) == func){
								xrefs.push_back(start+i);
								found = true;
								break;
							}
							i += 4;
						}
					}
				}
				if (found) break;
				start += chunksize - 5;
			}
			delete buffer;
			return xrefs;
		}

		// places a hook that removes itself, after
		// reading an offset of a register
		uint32_t debug(uint32_t address, reg_32 r32, int offset = 0) {
			uint32_t size=5, nop, isize=0, value=0, at=0;
			uint32_t code_loc	= reinterpret_cast<uint32_t>(VirtualAllocEx(handle, nullptr, 64, 0x1000, 0x40));
			uint32_t trace_loc	= reinterpret_cast<uint32_t>(VirtualAllocEx(handle, nullptr, 8, 0x1000, 0x4));
			uint32_t hit_loc	= trace_loc + 4;

			// Interpret size of original instruction(s)
			inst i = read(address);
			while (i.address<(address+size)){
				isize += i.len;
				nop = ((i.address+i.len)-(address+size));
				i = read(address+isize);
			}

			// store original bytes
			uint8_t* old_bytes = new uint8_t[size+nop];
			pmread(handle, vcast(address), old_bytes, size+nop, 0);

			// Make a JMP from the address to our own code
			uint8_t* code_data = new uint8_t[64];
			uint8_t* inject = new uint8_t[5];
			*(uint8_t*)(inject) = 0xE9;
			*(uint32_t*)(inject+1) = (code_loc-(address+5));

			if (offset == 0){ // simply place one instruction to capture 
				// the value of the register to our readout location
				*(uint8_t*)(code_data+at++) = (0x50+r32); // push (r32)
				switch (r32) {
					case reg_32::eax:
						*(uint8_t*)(code_data+at++) = 0xA3;
						break;
					default:
						*(uint8_t*)(code_data+at++) = 0x89; // ecx-edi (0xD,0x15,0x1D,0x25,0x2D . . .)
						*(uint8_t*)(code_data+at++) = (0x5+(r32*8));
					break;
				}
				// trace the register to our location
				*(uint32_t*)(code_data+at)	= trace_loc;at += 4;
				// hit detected
				*(uint8_t*)(code_data+at++)	= 0xC7;
				*(uint8_t*)(code_data+at++)	= 0x5;
				*(uint32_t*)(code_data+at)	= hit_loc;	at += 4;
				*(uint32_t*)(code_data+at)	= 1;		at += 4;
				// pop (r32)
				*(uint8_t*)(code_data+at++)	= (0x58+r32);
			} else { // or, if we want an offset of a register ...
				// move the offset into EAX and show EAX
				*(uint8_t*)(code_data+at++) = 0x50; // push eax
				*(uint8_t*)(code_data+at++) = 0x8B;
				if (offset > UCHAR_MAX){	// 32 bit offset
					if (r32 != reg_32::esp)
						*(uint8_t*)(code_data+at++) = (0x80+r32);
					else {
						*(uint8_t*)(code_data+at++) = 0x84;
						*(uint8_t*)(code_data+at++) = 0x24;
					}
					*(uint32_t*)(code_data+at++) = offset;
				} else {					// 8 bit offset
					if (r32 != reg_32::esp)
						*(uint8_t*)(code_data+at++) = (0x40+r32);
					else {
						*(uint8_t*)(code_data+at++) = 0x44;
						*(uint8_t*)(code_data+at++) = 0x24;
					}
					*(uint8_t*)(code_data+at++) = offset;
				}
				// Trace register to our trace location
				*(uint8_t*)(code_data+at++)	= 0xA3;
				*(uint32_t*)(code_data+at)	= trace_loc;at += 4;
				// hit detected
				*(uint8_t*)(code_data+at++)	= 0xC7;
				*(uint8_t*)(code_data+at++)	= 0x5;
				*(uint32_t*)(code_data+at)	= hit_loc;	at += 4;
				*(uint32_t*)(code_data+at)	= 1;		at += 4;
				// pop eax
				*(uint8_t*)(code_data +at++) = 0x58;
			}

			// Put overwritten bytes back (full instruction(s))
			pmwrite(handle, vcast(code_loc), code_data, at, 0);
			pmwrite(handle, vcast(code_loc+at), old_bytes, size+nop, 0);
			at += (size+nop);

			// Place our JMP back
			uint8_t* jmp_back = new uint8_t[5];
			*(uint8_t*)(jmp_back+at) = 0xE9;
			*(uint32_t*)(jmp_back+at+1)  = ((address+5)-(code_loc+at+5));
			pmwrite(handle, vcast(code_loc+at), jmp_back, 5, 0);
			delete jmp_back;
			delete code_data;

			edit::unprotect(address, size+nop);
			// Inject the JMP to our own code
			pmwrite(handle, vcast(address), inject, size, 0);
			for (int i=0; i<nop; i++){
				write(address+size+i, static_cast<uint8_t>(0x90));
			}
			edit::restore();
			delete inject;

			// wait for our masked value to be modified
			// this means something wrote to our location
			uint32_t d=0;
			while (readb(hit_loc) != 1){
				Sleep(1); if (d++>0xFFFF) break;
			} value=readui(trace_loc);

			edit::unprotect(address, size+nop);
			pmwrite(handle, vcast(address), old_bytes, size+nop, 0);
			edit::restore();
			delete old_bytes;

			VirtualFreeEx(handle, vcast(code_loc), 64, MEM_RELEASE);
			VirtualFreeEx(handle, vcast(trace_loc), 8, MEM_RELEASE);
			return value;
		}
	}




	namespace dllutil {
		namespace edit {
			struct PROTECT_INFO {
				uint32_t address;
				SIZE_T size;
				DWORD oldProtect;
			} last_unprotect;

			PROTECT_INFO unprotect(uint32_t address, SIZE_T size) {
				PROTECT_INFO x;
				x.address = address;
				x.size = size;
				VirtualProtect(vcast(address), size, 0x40, &x.oldProtect);
				last_unprotect = x;
				return x;
			}

			// restores page protection after calling unprotect()
			void restore(PROTECT_INFO x) {
				DWORD nothing;
				VirtualProtect(vcast(x.address), x.size, x.oldProtect, &nothing);
			}
			void restore() { restore(last_unprotect); }
		}

		

		std::string readstring(uint32_t addr) {
			uint32_t ptr = addr;
			std::string str = "";
			if (isgood(ptr)){
				if (isgood(*(uint32_t*)(ptr))) {
					ptr = *(uint32_t*)ptr;
				}
				char c[MAX_STR_READ];
				memcpy(&c, vcast(ptr), MAX_STR_READ);
				int i = 0;
				while (i < MAX_STR_READ && c[i] >= 0x20 && c[i] <= 0x7E){
					str += c[i++];
				}
			}
			return str;
		}
		
		bool isprologue(uint32_t address) {
			uint8_t	b[3];
			memcpy(&b, vcast(address), 3);

			if (b[0] == 0x55 && b[1] == 0x8B && b[2] == 0xEC) {
				return true;
			}
			// some prologues use ESI
			if (b[0] == 0x56 && b[1] == 0x8B && b[2] == 0xF1) {
				for (int i = 0; i < 0xFFFF; i++) {
					memcpy(&b, vcast(address + i), 2);
					if (b[0] == 0x5E && (b[1] == 0xC3 || b[1] == 0xC2)) {
						return true;
					}
				}
			}
			return false;
		}

		bool isepilogue(uint32_t address) {
			uint8_t	b[2];
			memcpy(&b, vcast(address), 2);
			return ((b[0]==0x5D || b[0]==0x5E) && // pop ebp, or pop esi,
					(b[1]==0xC2 || b[1]==0xC3));  // with a retn or ret XX
		}

		uint32_t nextprologue(uint32_t address, direction d, bool aligned = true){
			uint32_t at = address, count = 0;
			// Skip this prologue if we're already at one
			if (isprologue(at)){
				if (d == behind) at -= 16;
				if (d == ahead)  at += 16;
			}
			while (!isprologue(at) && !(at>base+base_size && *(uint64_t*)at == 0 && *(uint64_t*)(at+8) == 0)){
				if (count++ > 0xFFFF) break;
				if (d == ahead)  if (!aligned) at++; else at += 16;
				if (d == behind) if (!aligned) at--; else at -= 16;
			}
			return at;
		}

		uint32_t nextepilogue(uint32_t address, direction d){
			uint32_t at = address, count = 0;
			while (!isepilogue(at)){
				if (count++ > 0xFFFF) break;
				if (d == ahead)  at++;
				if (d == behind) at--;
			}
			return (at+1); // Return the functions retn address
		}

		int fsize(uint32_t func) {
			uint32_t eof = nextprologue(func, ahead, true);
			return static_cast<int>(eof - func);
		}

		uint32_t getprologue(uint32_t addr) {
			return nextprologue(addr, behind, false);
		}

		std::vector<uint32_t> getprologues(uint32_t func, direction d, int count) {
			std::vector<uint32_t> result_list = std::vector<uint32_t>();
			uint32_t addr = func, current = 0;
			while (current < count){
				addr = nextprologue(addr, d, true);
				result_list.push_back(addr);
				current++;
			}
			return result_list;
		}

		uint32_t getepilogue(uint32_t func) {
			return nextepilogue(nextprologue(func, ahead, true), behind);
		}

		std::vector<uint32_t> getepilogues(uint32_t func) {
			std::vector<uint32_t> result_list = std::vector<uint32_t>();
			uint32_t start = func;
			uint32_t end = (start + fsize(func));
			while (start < end) {
				if (isepilogue(start)){
					result_list.push_back(start+1);
				}
				start++;
			}
			return result_list;
		}

		short fretn(uint32_t func) {
			for (uint32_t addr : getepilogues(func)) {
				if (*(uint8_t*)addr == 0xC2) {
					return *(short*)(addr+1);
				}
			}
			return 0;
		}

		std::vector<uint32_t> getcalls(uint32_t func) {
			std::vector<uint32_t> result_list = std::vector<uint32_t>();
			uint32_t start = func;
			uint32_t end = (func + fsize(func));
			while (start < end) {
				if (*(uint8_t*)start == 0xE8){
					uint32_t o = (start+*(uint32_t*)(start+1)+5);
					if (o%16==0 && o>base && o<base+base_size){
						result_list.push_back(o);
					}
				}
				start++;
			}
			return result_list;
		}

		std::vector<uint32_t> getpointers(uint32_t func) {
			std::vector<uint32_t> result_list = std::vector<uint32_t>();
			uint32_t start = func;
			uint32_t end = (func + fsize(func));
			while (start < end) {
				inst i = read(start);
				uint32_t p = 0;
				if (i.flags & Fl_src_disp32)	p = i.src.disp32;
				if (i.flags & Fl_dest_disp32)	p = i.dest.disp32;
				if (i.flags & Fl_src_imm32)		p = i.src.imm32;
				if (i.flags & Fl_dest_imm32)	p = i.dest.imm32;
				if (p != 0){
					if (p%4==0 && p>base && p<base+base_size) result_list.push_back(p);
					if (p%4==0 && p>base && p<base+base_size) result_list.push_back(p);
				}
				start += i.len;
			}
			return result_list;
		}

		uint32_t nextcall(uint32_t func, direction d, bool loc = false){
			uint32_t start = func;
			// Skip current call if we're already at one
			if (*(uint8_t*)start == 0xE8){
				if (d == ahead)  start++;
				if (d == behind) start--;
			}
			while (*(uint8_t*)start != 0xE8){
				if (d == ahead)  start++;
				if (d == behind) start--;
			}
			uint32_t o = (start+*(uint32_t*)(start+1)+5);
			if (o%16==0 && o>base && o<base+base_size)
				if (!loc)
					return o;
				else
					return start;
			return 0;
		}

		uint32_t getcall(uint32_t addr){
			uint32_t o = (addr+*(uint32_t*)(addr+1)+5);
			if (o%16==0 && o>base && o<base+base_size)
				return o;
			return 0;
		}

		uint32_t nextxref(uint32_t start, direction d, uint32_t func, bool parentfunc = false) {
			uint32_t at = start;
			while (getcall(at) != func) {
				uint32_t old_at = at;
				at = nextcall(at, d, true);
				if (at == 0) if (d==behind) at = --old_at; else at = ++old_at;
			}
			if (!parentfunc) return at;
			return nextprologue(at, behind, false);
		}

		std::vector<uint32_t> __fastcall scan(const char* aob, const char* _mask, uint32_t begin = 0, uint32_t to = 0) {
			uint32_t size=0, at=0, start=begin, end=to;
			if (!(start && end)){
				start = eyestep::base;
				end = eyestep::base+eyestep::base_size;
			}

			uint32_t chunksize		= ((end-start)<(64*64*32))?(end-start):(64*64*32);
			uint8_t* buffer			= new uint8_t[chunksize];
			char match				= 1;
			char wildchar			= 'x'; // 'x' means it can be any byte; put '.' for the rest

			std::vector<uint32_t>	results_list = std::vector<uint32_t>();
			std::vector<uint8_t>	bytes = cbyte(aob).bytes;
			size					= bytes.size();

			// create mask for AOB if string is empty
			char mask[256];
			mask[0] = '\0';
			if (lstrlenA(_mask) == 0){
				for (int i=0; i<size; i++)
					strcat(mask, ".");
			} else strcpy(mask, _mask);

			uint32_t padding = (size - 1);
			// start scanning...
			//SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
			while (start < end){ try {
					memcpy(buffer, vcast(start), chunksize);
				} catch (std::exception e){ printf("Exception: %s\n", e.what()); }
				__asm push edi
				__asm xor edi, edi
				__asm jmp L2
			L1: __asm inc edi
				__asm mov at, edi
				match=1;
				for (uint32_t x=0; x<size; x++)
				if (buffer[at+x]!=bytes[x] && mask[x]!=wildchar)
				match=0;
				if (match) results_list.push_back(start+at);
			L2:	__asm cmp edi, chunksize
				__asm jb L1
				__asm sub edi, padding
				__asm add start, edi
				__asm pop edi
			}
			//SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
			free(buffer);
			return results_list;
		}

		std::vector<uint32_t> __fastcall scanpointer(uint32_t address){
			return scan(convert::to_bytes(address).c_str(), "....");
		}

		// strings are located onward from .rdata
		// roblox detects this and you're kicked, and I never got
		// around to a bypass for it; thats why eyecrawl never worked as a DLL
		std::vector<uint32_t> __fastcall scanxrefs(const char* str, int result_number = 0) {
			char* mask = new char[lstrlenA(str)];
			for (int i=0; i<lstrlenA(str); i++){
				mask[i]='.';
			}
			std::vector<uint32_t>strlocs = scan(convert::to_bytes(str).c_str(), mask);
			free(mask);
			if (strlocs.size() == 0) return strlocs;
			return scanpointer(strlocs[result_number]);
		}

		// xrefs made to a function
		std::vector<uint32_t> __fastcall scanxrefs(uint32_t func, uint32_t begin = 0, uint32_t to = 0){
			uint32_t size=0, at=0, start=begin, end=to;
			if (!(start && end)){
				start = eyestep::base;
				end = eyestep::base + eyestep::base_size;
			}

			uint32_t chunksize		= ((end-begin)<(64*64*32))?(end-begin):(64*64*32);
			uint8_t* buffer			= new uint8_t[chunksize];
			bool found				= false;
			std::vector<uint32_t>	xrefs = std::vector<uint32_t>();

			//SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
			while (start < end && start < base + base_size){
				bool read = true;
				memcpy(buffer, vcast(start), chunksize);
				if (read){
					for (int i=0; i<chunksize; i++){
						if (*(uint8_t*)(start+i) == 0xE8){ // call instruction
							// calculate relative offset
							uint32_t o = *(uint32_t*)(start+i+1);
							if ((start+i+5+o) == func){
								xrefs.push_back(start+i);
								found = true;
								break;
							}
							i += 4;
						}
					}
				}
				if (found) break;
				start += chunksize - 5;
			}
			//SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
			free(buffer);
			return xrefs;
		}



		// places a hook that removes itself, after
		// reading an offset of a register
		uint32_t debug(uint32_t address, reg_32 r32, int offset = 0) {
			uint32_t size=5, nop, isize=0, value=0, at=0;
			uint32_t code_loc	= reinterpret_cast<uint32_t>(VirtualAlloc(nullptr, 64, 0x1000, 0x40));
			uint32_t trace_loc	= reinterpret_cast<uint32_t>(VirtualAlloc(nullptr, 8, 0x1000, 0x4));
			uint32_t hit_loc	= trace_loc + 4;

			// Interpret size of original instruction(s)
			inst i = read(address);
			while (i.address<(address+size)){
				isize += i.len;
				nop = ((i.address+i.len)-(address+size));
				i = read(address+isize);
			}

			// store original bytes
			uint8_t* old_bytes = new uint8_t[size+nop];
			memcpy(old_bytes, vcast(address), size+nop);

			// Make a JMP from the address to our own code
			uint8_t* inject = new uint8_t[5];
			*(uint8_t*)(inject) = 0xE9;
			*(uint32_t*)(inject+1) = (code_loc-(address+5));

			if (offset == 0){ // simply place one instruction to capture 
				// the value of the register to our readout location
				*(uint8_t*)(code_loc+at++) = (0x50+r32); // push (r32)
				switch (r32) {
					case reg_32::eax:
						*(uint8_t*)(code_loc+at++) = 0xA3;
						break;
					default:
						*(uint8_t*)(code_loc+at++) = 0x89; // ecx-edi (0xD,0x15,0x1D,0x25,0x2D . . .)
						*(uint8_t*)(code_loc+at++) = (0x5+(r32*8));
					break;
				}
				// trace the register to our location
				*(uint32_t*)(code_loc+at)	= trace_loc;at += 4;
				// hit detected
				*(uint8_t*)(code_loc+at++)	= 0xC7;
				*(uint8_t*)(code_loc+at++)	= 0x5;
				*(uint32_t*)(code_loc+at)	= hit_loc;	at += 4;
				*(uint32_t*)(code_loc+at)	= 1;		at += 4;
				// pop (r32)
				*(uint8_t*)(code_loc+at++)	= (0x58+r32);
			} else { // or, if we want an offset of a register ...
				// move the offset into EAX and show EAX
				*(uint8_t*)(code_loc+at++) = 0x50; // push eax
				*(uint8_t*)(code_loc+at++) = 0x8B;
				if (offset > UCHAR_MAX){	// 32 bit offset
					if (r32 != reg_32::esp)
						*(uint8_t*)(code_loc+at++) = (0x80+r32);
					else {
						*(uint8_t*)(code_loc+at++) = 0x84;
						*(uint8_t*)(code_loc+at++) = 0x24;
					}
					*(uint32_t*)(code_loc+at++) = offset;
				} else {					// 8 bit offset
					if (r32 != reg_32::esp)
						*(uint8_t*)(code_loc+at++) = (0x40+r32);
					else {
						*(uint8_t*)(code_loc+at++) = 0x44;
						*(uint8_t*)(code_loc+at++) = 0x24;
					}
					*(uint8_t*)(code_loc+at++) = offset;
				}
				// Trace register to our trace location
				*(uint8_t*)(code_loc+at++)	= 0xA3;
				*(uint32_t*)(code_loc+at)	= trace_loc;at += 4;
				// hit detected
				*(uint8_t*)(code_loc+at++)	= 0xC7;
				*(uint8_t*)(code_loc+at++)	= 0x5;
				*(uint32_t*)(code_loc+at)	= hit_loc;	at += 4;
				*(uint32_t*)(code_loc+at)	= 1;		at += 4;
				// pop eax
				*(uint8_t*)(code_loc+at++)	= 0x58;
			}

			// Put overwritten bytes back (full instruction(s))
			memcpy(vcast(code_loc+at), old_bytes, size+nop);
			at += (size+nop);

			// Place our JMP back
			*(uint8_t*)(code_loc+at++) = 0xE9;
			*(uint32_t*)(code_loc+at)  = ((address+5)-(code_loc+at+4));
			at += 4;

			edit::unprotect(address, size+nop);
			// Inject the JMP to our own code
			memcpy(vcast(address), inject, size);
			for (int i=0; i<nop; i++){
				*(uint8_t*)(address+size+i) = 0x90;
			}
			edit::restore();
			free(inject);

			// wait for our masked value to be modified
			// this means something wrote to our location
			uint32_t d=0;
			while (*(uint8_t*)hit_loc != 1){
				Sleep(1); if (d++>0xFFFF) break;
			}
			value=*(uint32_t*)trace_loc;

			edit::unprotect(address, size+nop);
			memcpy(vcast(address), old_bytes, size+nop);
			edit::restore();
			free(old_bytes);

			VirtualFree(vcast(code_loc), 64, MEM_RELEASE);
			VirtualFree(vcast(trace_loc), 8, MEM_RELEASE);
			return value;
		}


		// Use on an active function to dump [count] offsets of the register [reg].
		// neg determines whether the offsets are negative.
		// for example, dumpreg(location, ecx, 3, true);
		// returns the values of [ecx-0], [ecx-4], [ecx-8],
		// at that location in a function
		// dumpreg(location, ebp, 10, false); will give you a readout of all(well technically 10) args
		// presently used in the function (ignore the first two results/args start at ebp+8)
		// dumpreg(location, ebp, 10, true); will give you a readout of all local function-scope variables
		// presently used in the function
		std::vector<uint32_t> dumpreg(uint32_t address, uint8_t reg, int count, bool neg = false) {
			uint32_t size=5, nop, isize=0, at=0;
			uint32_t trace_loc	= reinterpret_cast<uint32_t>(VirtualAlloc(nullptr, 128, 0x1000, 0x40));
			uint32_t code_loc	= reinterpret_cast<uint32_t>(VirtualAlloc(nullptr, 128, 0x1000, 0x40));
			uint32_t hit_loc	= trace_loc + 128 - 4;
			std::vector<uint32_t> values;

			// Interpret size of original instruction(s)
			inst i = read(address);
			while (i.address<(address+size)){
				isize += i.len;
				nop = ((i.address+i.len)-(address+size));
				i = read(address+isize);
			}

			// store original bytes
			uint8_t* old_bytes = new uint8_t[size+nop];
			memcpy(old_bytes, vcast(address), size+nop);

			uint8_t dist = 19;
			if (neg) dist += 2;

			// loop/read +4 of the reg each time until we reach count*4
			// place it along trace_loc
			*(uint8_t*)(code_loc+at++)	= 0x60; // pushad
			*(uint8_t*)(code_loc+at++)	= 0x50; // push eax
			*(uint8_t*)(code_loc+at++)	= 0x33; // xor eax,eax
			*(uint8_t*)(code_loc+at++)	= 0xC0;
			*(uint8_t*)(code_loc+at++)	= 0x53; // push ebx
			*(uint8_t*)(code_loc+at++)	= 0x33; // xor ebx,ebx
			*(uint8_t*)(code_loc+at++)	= 0xDB;
			*(uint8_t*)(code_loc+at++)	= 0x51; // push ecx
			// loop:
			*(uint8_t*)(code_loc+at++)	= 0x8B; // mov ebx,eax
			*(uint8_t*)(code_loc+at++)	= 0xD8;
			if (neg){
				*(uint8_t*)(code_loc+at++) = 0xF7; // neg ebx
				*(uint8_t*)(code_loc+at++) = 0xDB;
			}
			*(uint8_t*)(code_loc+at++)	= 0x8B; // mov ecx,[ebx+ebp]
			*(uint8_t*)(code_loc+at++)	= 0x0C;
			*(uint8_t*)(code_loc+at++)	= ebx + (reg * 8);
			*(uint8_t*)(code_loc+at++)	= 0x89; // mov [eax+trace_loc],ecx
			*(uint8_t*)(code_loc+at++)	= 0x88;
			*(uint32_t*)(code_loc+at)	= trace_loc; at += 4;
			*(uint8_t*)(code_loc+at++)	= 0x83; // add eax,04
			*(uint8_t*)(code_loc+at++)	= 0xC0;
			*(uint8_t*)(code_loc+at++)	= 0x04;
			*(uint8_t*)(code_loc+at++)	= 0x83; // cmp eax,0x40 (64)
			*(uint8_t*)(code_loc+at++)	= 0xF8;
			*(uint8_t*)(code_loc+at++)	= static_cast<uint8_t>(count * 4);
			*(uint8_t*)(code_loc+at++)	= 0x72; // jb loop
			*(uint8_t*)(code_loc+at++) = static_cast<uint8_t>(-dist);
			*(uint8_t*)(code_loc+at++)	= 0xC7; // mov hit_loc, 1
			*(uint8_t*)(code_loc+at++)	= 0x5;
			*(uint32_t*)(code_loc+at)	= hit_loc;	 at += 4;
			*(uint32_t*)(code_loc+at)	= 1;		 at += 4;
			// end
			*(uint8_t*)(code_loc+at++)	= 0x59; // pop ecx
			*(uint8_t*)(code_loc+at++)	= 0x5B; // pop ebx
			*(uint8_t*)(code_loc+at++)	= 0x58; // pop eax
			*(uint8_t*)(code_loc+at++)	= 0x61; // popad

			// Make a JMP from the address to our own code
			uint8_t* inject = new uint8_t[5];
			*(uint8_t*)(inject) = 0xE9;
			*(uint32_t*)(inject+1) = (code_loc-(address+5));

			// Put overwritten bytes back (full instruction(s))
			memcpy(vcast(code_loc+at), old_bytes, size+nop);
			at += (size+nop);

			// Place our JMP back
			*(uint8_t*)(code_loc+at++) = 0xE9;
			*(uint32_t*)(code_loc+at)  = ((address+5)-(code_loc+at+4));
			at += 4;

			edit::unprotect(address, size+nop);
			// Inject the JMP to our own code
			memcpy(vcast(address), inject, size);
			for (int i=0; i<nop; i++){
				*(uint8_t*)(address+size+i) = 0x90;
			}
			edit::restore();
			free(inject);

			// wait for our masked value to be modified
			// this means something wrote to our location
			uint32_t d=0;
			while (*(uint8_t*)hit_loc != 1){
				Sleep(1); if (d++>0xFFFF) break;
			}

			edit::unprotect(address, size+nop);
			memcpy(vcast(address), old_bytes, size+nop);
			edit::restore();
			free(old_bytes);

			for (int i=0; i<count; i++){
				values.push_back(*(uint32_t*)(trace_loc+(i*4)));
			}

			VirtualFree(vcast(code_loc), 128, MEM_RELEASE);
			VirtualFree(vcast(trace_loc), 128, MEM_RELEASE);
			return values;
		}

		conv getconv(uint32_t func) {
			conv c				= conv_cdecl;
			uint32_t eof		= nextprologue(func, ahead);
			uint32_t at			= func;
			bool not_fastcall	= false;
			bool neither		= false;
			if (fretn(func))  c = conv_stdcall;

			while (at < eof) {
				inst i = eyestep::read(at);
				at += i.len;
				// if edx or ecx was pushed thats an instant indication
				if (strcmp(i.opcode, "push") == 0){
					if (i.src.r32 == ecx) neither		= true;
					if (i.src.r32 == edx) not_fastcall	= true;
				}
				// completely ignore a test instruction, with ecx,ecx or edx,edx
				if (strcmp(i.opcode, "test") == 0) {
					if ((i.src.r32 == ecx && i.dest.r32 == ecx) || (i.src.r32 == edx && i.dest.r32 == edx)) {
						continue;
					}
				}
				// DONT ignore a xor instruction, with ecx,ecx or edx,edx
				if (strcmp(i.opcode, "xor") == 0) {
					if (i.src.r32 == ecx && i.dest.r32 == ecx) {
						neither = true;
						continue;
					} else if (i.src.r32 == edx && i.dest.r32 == edx) {
						not_fastcall = true;
						continue;
					}
				}
				if (i.flags & Fl_src_r32 || i.flags & Fl_src_rm32){
					if (strcmp(i.opcode, "mov") == 0 || strcmp(i.opcode, "lea") == 0 ||
						strcmp(i.opcode, "movzx") == 0 || strcmp(i.opcode, "or") == 0 ||
						strcmp(i.opcode, "xor") == 0
						){
						// because something like a mov ecx,edx
						// can also throw things off
						if (i.dest.r32 == edx && /*!neither && */!not_fastcall) {
							// if !neither is included,
							// it wont work if there's a push ecx prior to mov ???, edx
							// even though some __fastcall's/__thiscall's push ecx, fuck knows why
							return conv_fastcall;
						}
						// it will take a mov or lea instruction with ecx or edx
						// to indicate it is NOT a fastcall or thiscall.
						// But, something like "or ecx,1" has ecx in the src operand,
						// and its using it as if it was passed to the function
						if (i.src.r32 == ecx) neither = true;
						if (i.src.r32 == edx) not_fastcall = true;
					}
				}
				// now, we check if an instruction USES edx or ecx after
				// all that has passed, now that it's determined whether
				// it was or wasn't passed to this function
				if (i.flags & Fl_dest_r32 || i.flags & Fl_dest_rm32){
					if (i.dest.r32 == edx){
						if (!neither && !not_fastcall){
							return conv_fastcall;
						}
					}
					if (i.dest.r32 == ecx){
						if (c != conv_fastcall && !neither){
							c = conv_thiscall;
						}
					}
					if (not_fastcall && c == conv_thiscall) {
						return conv_thiscall;
					}
				}
			}
		
			return c;
		}

		PEB::PEB* getPEB() {
			uint32_t x = 0;
			__asm push eax;
			__asm mov eax, fs:[0x30];
			__asm mov x, eax;
			__asm pop eax;
			return reinterpret_cast<PEB::PEB*>(x);
		}
	}
}


#endif
