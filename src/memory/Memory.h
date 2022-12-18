// CMemory.h by Tuxick https://github.com/emcifuntik
#pragma once

#include <functional>
#include <iomanip>
#include <vector>
#include <sstream>

#include <Windows.h>
#include <Psapi.h>
#include <immintrin.h>
#include <emmintrin.h>
#include <cstdint>
#include <MinHook.h>

#define MAX_SIG_SIZE 128

#define INRANGE(x,a,b)		(x >= a && x <= b) 
#define getBits( x )		(INRANGE(x,'0','9') ? (x - '0') : ((x&(~0x20)) - 'A' + 0xa))
#define getByte( x )		(getBits(x[0]) << 4 | getBits(x[1]))

class IMemoryCacheStorage
{
public:
	virtual ~IMemoryCacheStorage() = default;
	virtual uintptr_t Get(uint32_t hash) = 0;
	virtual void Put(uint32_t hash, uintptr_t address) = 0;
	virtual uint32_t Hash(const std::string& pattern) = 0;
};

class MemoryCacheStorage : public IMemoryCacheStorage {
public:
	uintptr_t Get(uint32_t hash) override {
		return _map[hash];
	}

	void Put(uint32_t hash, uintptr_t address) override {
		_map[hash] = address;
	}

	uint32_t Hash(const std::string& pattern) override {
		static std::hash<std::string> hash;
		return hash(pattern);
	}
	
private:
	std::unordered_map<uint32_t, uintptr_t> _map;
};

class Memory {
	uintptr_t address;
public:
	using PatternNotFoundCallback = void(*)(const std::string& pattern);

	class Pattern
	{
	private:
		static bool Match(const PBYTE addr, const PBYTE pat, const PBYTE msk)
		{
			size_t n = 0;

			while (addr[n] == pat[n] || msk[n] == (BYTE)'?')
			{
				if (!msk[++n])
					return true;
			}

			return false;
		}

		static PBYTE Find(const PBYTE rangeStart, DWORD len, const char* sig, const char* mask)
		{
			size_t l = strlen(mask);
			for (DWORD n = 0; n < (len - l); ++n)
			{
				if (Match(rangeStart + n, (PBYTE)sig, (PBYTE)mask)) {
					return rangeStart + n;
				}
			}
			return NULL;
		}

		constexpr static unsigned char FromHex(char c)
		{
			if (c >= 'a' && c <= 'f')
				return c - 'a' + 10;
			if (c >= 'A' && c <= 'F')
				return c - 'A' + 10;
			if (c >= '0' && c <= '9')
				return c - '0';

			return 0;
		}

	public:
		char sig[MAX_SIG_SIZE];
		char mask[MAX_SIG_SIZE];
		std::size_t size;

		template<std::size_t Size>
		constexpr Pattern(const char(&idaPattern)[Size]) :sig{ 0 }, mask{ 0 }, size(0)
		{
			std::size_t j = 0;
			for (std::size_t i = 0; i < Size; ++i, ++j)
			{
				char c = idaPattern[i];

				if (c >= 'a' && c <= 'f' || c >= 'A' && c <= 'F' || c >= '0' && c <= '9')
				{
					sig[j] = (FromHex(c) << 4) + FromHex(idaPattern[i + 1]);
					mask[j] = 'x';
					i += 2;
				}
				else if (c == '?')
				{
					sig[j] = '\x00';
					mask[j] = '?';
					++i;
				}
				else
				{
					--j;
				}
			}

			size = j;
		}

		Memory Search(bool useCaching = true, uintptr_t base = 0) const
		{
			uint32_t sigHash = 0;
			if (useCaching && GetCacheStorage())
			{
				std::string patString = ToString();
				sigHash = GetCacheStorage()->Hash(patString);
				uintptr_t cachedAddress = GetCacheStorage()->Get(sigHash);
				if (cachedAddress)
					return Memory(cachedAddress);
			}

			BYTE* res = Find(base == 0 ? (PBYTE)Base() : (PBYTE)base, (DWORD)GetSize(), sig, mask);

			if (!res)
			{
				std::string patString = ToString();
				if (OnPatternNotFound())
					OnPatternNotFound()(patString);
				return Memory(0xFFFFFFFF, false);
			}

			if (useCaching && GetCacheStorage())
			{
				std::string patString = ToString();
				sigHash = GetCacheStorage()->Hash(patString);
				GetCacheStorage()->Put(sigHash, (uintptr_t)res - Memory::Base());
			}
			return Memory(res);
		}

		std::string ToString() const
		{
			std::stringstream ss;

			for (size_t i = 0; i < size; ++i)
			{
				if (mask[i] == 'x')
					ss << std::setfill('0') << std::setw(2) << std::hex << (int)(unsigned char)sig[i] << " ";
				else
					ss << "? ";
			}

			return ss.str();
		}
	};

	inline static PatternNotFoundCallback OnPatternNotFound(PatternNotFoundCallback cb = nullptr)
	{
		static PatternNotFoundCallback callback = nullptr;
		if (cb != nullptr)
			callback = cb;
		return callback;
	}

	inline static size_t GetSize()
	{
		static MODULEINFO info = { 0 };
		static bool ok = GetModuleInformation(GetCurrentProcess(), NULL, &info, sizeof(MODULEINFO));
		return info.SizeOfImage;
	}

	inline static IMemoryCacheStorage* InitCacheStorage(IMemoryCacheStorage* storage = nullptr)
	{
		static IMemoryCacheStorage* cStorage = nullptr;
		if (storage != nullptr)
			cStorage = storage;
		return cStorage;
	}

	inline static IMemoryCacheStorage* GetCacheStorage()
	{
		return InitCacheStorage();
	}

	inline static uintptr_t& Base()
	{
		static uintptr_t _Base;
		return _Base;
	}

	inline static Memory& Unused()
	{
		static Memory _Unused(nullptr);
		return _Unused;
	}

	inline static Memory AllocUnused(std::size_t size)
	{
		static Memory NextUnused = Unused();

		Memory res(NextUnused);
		NextUnused += (int)size;
		return res;
	}

	struct Hook
	{
		static std::vector<std::function<void()>>& Hooks()
		{
			static std::vector<std::function<void()>> _Hooks;
			return _Hooks;
		}

		Hook(const std::function<void()>& fn) {
			Hooks().push_back(fn);
		}
	};

	static void RunHooks()
	{
		for (auto& fn : Hook::Hooks())
			fn();
	}

	Memory(uintptr_t _address, bool addBase = true) : address(addBase ? Base() + _address : _address) { }
	Memory(void* address) : address((uintptr_t)address) { }
	Memory() : address(0) { }

	bool IsValid()
	{
		return address != 0xFFFFFFFFFFFFFFFF;
	}

	Memory& Add(int offset)
	{
		address += offset;
		return *this;
	}

	void FarJump(void* func)
	{
		MemUnlock lock(address, 12);

		*((WORD*)address) = 0xB848;
		*((intptr_t *)(address + 2)) = (uintptr_t)func;
		*((WORD*)(address + 10)) = 0xE0FF;
	}

	void FarJump(const Memory& func)
	{
		FarJump((void*)func.address);
	}

	void NearCall(uint64_t func)
	{
		Put<uint8_t>(0xE8);
		(*this + 1).Put(int32_t(func - address - 5));
	}

	void NearCall(const Memory& func)
	{
		NearCall(func.address);
	}

	void SetOffset(uint64_t value, int offset = 1)
	{
		(*this + offset).Put(int32_t(value - address - (4 + offset)));
	}

	void SetOffset(const Memory& func)
	{
		SetOffset(func.address);
	}

	void Nop(size_t len)
	{
		MemUnlock lock(address, len);

		memset((void*)address, 0x90, len);
	}

	void Retn()
	{
		Put<uint8_t>(0xC3);
	}

	Memory GetOffset(int offset = 3)
	{
		int32_t* ptr = (int32_t*)(address + offset);
		return Memory(*ptr + (address + offset + 4), false);
	}

	Memory& operator+=(int rhs)
	{
		address += rhs;
		return *this;
	}

	Memory& operator-=(int rhs)
	{
		return (*this) += (-rhs);
	}

	Memory operator+(int right) const
	{
		return Memory(*this) += right;
	}

	Memory operator-(int right) const
	{
		return Memory(*this) -= right;
	}

	template <typename R = void*, typename ...Args>
	R Call(Args... args)
	{
		return ((R(*)(Args...))address)(args...);
	}

	template<class T = void*>
	T Get(int offset = 0)
	{
		return (T)(address + offset);
	}

	template<typename T>
	void Put(const T& value)
	{
		MemUnlock lock(address, sizeof(T));

		memcpy((void*)address, &value, sizeof(T));
	}

	template<std::size_t N>
	void Put(uint8_t(&bytes)[N])
	{
		Put<uint8_t[N]>(bytes);
	}

	template<class T>
	T GetCall()
	{
		return (T)(*(int32_t*)(address + 1) + address + 5);
	}

#ifdef MH_ALL_HOOKS
	template<class T>
	bool Detour(T* fn, T** orig = nullptr)
	{
		MH_CreateHook((void*)address, (void*)fn, (void**)orig);
		return MH_EnableHook((void*)address) == MH_OK;
	}

	void DeactivateDetour()
	{
		MH_DisableHook((void*)address);
	}
#endif

private:
	struct MemUnlock
	{
		DWORD rights;
		size_t len;
		void * addr;

		MemUnlock(uint64_t _addr, size_t _len) :addr((void*)_addr), len(_len)
		{
			VirtualProtect(addr, len, PAGE_EXECUTE_READWRITE, &rights);
		}

		~MemUnlock()
		{
			VirtualProtect(addr, len, rights, NULL);
		}
	};
};