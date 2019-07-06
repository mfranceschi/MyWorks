//---------- Implementation of module <File> (file File.cpp) 

//--------------------------------------------------------------- Includes
#pragma warning( disable: 26444) // Disables a warn' that occurs when using imbue.

#include <Windows.h>
#include <algorithm>
#include <locale>
#include <codecvt>
#include "File.h"
#include "Toolbox.h"
#include <io.h>
#include <fcntl.h>

using std::string; 
using std::locale; 
using std::ifstream; 
using std::ios_base;

typedef int FileHandle;

static const size_t NBR_BITS_TO_READ_ENCODING = 3;

namespace File
{
/////////////////////////////////////////////////////////////////  PRIVATE

//-------------------------------------------------------------- Constants

	const locale locUTF8(
		locale(), 
		new std::codecvt_utf8<char>());
	const locale locUTF16LE(
		locale(),
		new std::codecvt_utf16<char, 0x10ffff, std::little_endian>
	); 

//------------------------------------------------------------------ Types

//------------------------------------------------------- Static variables

//------------------------------------------------------ Private functions

//////////////////////////////////////////////////////////////////  PUBLIC
//------------------------------------------------------- Public functions

	bool Exists(const string& filename)
	{
		wchar_t* converted = Toolbox::ToWchar_t(filename.c_str());
		auto result = Exists(converted);
		delete[] converted;
		return result;
	}
	
	bool Exists(const wchar_t* filename)
	// Algorithm: https://stackoverflow.com/questions/3828835.
	{
		DWORD attr = GetFileAttributes(filename);
		return attr == INVALID_FILE_ATTRIBUTES || (attr & FILE_ATTRIBUTE_DIRECTORY);
	}

	/*bool IsEmpty(const char* filename, size_t charsToRead)
	{
		bool forReturn;
		FILE* f = nullptr;

		if (fopen_s(&f, filename, "r"))
		{
			forReturn = true;
		}
		else
		{
			for (size_t i = 0; i < charsToRead; ++i)
			{
				fgetc(f);
			}
			forReturn = feof(f) + ferror(f);
			fclose(f);
		}

		return forReturn;
	}*/

	bool IsEmpty(const char* filename, size_t charsToRead)
	{
		FileHandle file;
		bool forReturn;

		if (_sopen_s(&file, filename, _O_RDONLY | _O_BINARY, _SH_DENYWR, _S_IREAD))
			forReturn = true;
		else
		{
			char* content = new char[charsToRead];
			forReturn = !(_read(file, content, charsToRead) != charsToRead || _eof(file));
			delete[] content;
			_close(file);
		}

		return forReturn;
	}

	void Open(ifstream& ifs, const char* filename,
		encoding_t encoding)
	{
		ifs.close();
		if (encoding == ENC_UNKNOWN)
			encoding = File::Encoding(filename);

		if (encoding == ENC_UTF8)
		{
			ifs.open(filename);
			ifs.imbue(locUTF8);
			ifs.seekg(3);
		}
		else if (encoding == ENC_UTF16LE)
		{
			ifs.open(filename, ios_base::binary);
			ifs.imbue(locUTF16LE);
			ifs.seekg(2, ios_base::beg);
		}
		else if (encoding == ENC_DEFAULT)
		{
			ifs.open(filename, ios_base::binary);
		}
	}

	filesize_t Size(const string& filename)
	{
		wchar_t* converted = Toolbox::ToWchar_t(filename.c_str());
		auto result = Size(converted);
		delete[] converted;
		return result;
	}

	filesize_t Size(const wchar_t* filename)
	{
		HANDLE file = CreateFile(filename, GENERIC_READ, FILE_SHARE_WRITE, NULL, OPEN_EXISTING, NULL, NULL);
		if (file == INVALID_HANDLE_VALUE) return -1;
		LARGE_INTEGER res;
		GetFileSizeEx(file, &res);
		CloseHandle(file);
		return filesize_t(res.QuadPart);
	}

	/*encoding_t Encoding(const char* filename)
	{
		encoding_t forReturn;
		FILE* f = nullptr;
		if (IsEmpty(filename, 3) || fopen_s(&f, filename, "r"))
			forReturn = ENC_UNKNOWN;
		else
		{
			fseek(f, 0L, SEEK_SET);

			int char1 = fgetc(f);
			int char2 = fgetc(f);
			int char3 = fgetc(f);

			
			if (char1 == 0xff && char2 == 0xfe)
				forReturn = ENC_UTF16LE;
			else if (char1 == 0xef && char2 == 0xbb && char3 == 0xbf)
				forReturn = ENC_UTF8;
			else
				forReturn = ENC_DEFAULT;

			fclose(f);
		}
		return forReturn;
	}*/

	encoding_t Encoding(const char* filename)
	{
		FileHandle file;
		encoding_t forReturn;

		if (_sopen_s(&file, filename, _O_RDONLY | _O_BINARY, _SH_DENYWR, _S_IREAD))
			forReturn = ENC_UNKNOWN;
		else
		{
			char bits[NBR_BITS_TO_READ_ENCODING];
			int ret_read = _read(file, bits, NBR_BITS_TO_READ_ENCODING);
			
			if (ret_read != NBR_BITS_TO_READ_ENCODING)
				forReturn = ENC_UNKNOWN;

			if (bits[0] == 0xff && bits[1] == 0xfe)
				forReturn = ENC_UTF16LE;
			else if (ret_read == 3 && bits[0] == 0xef && bits[1] == 0xbb && bits[2] == 0xbf)
				forReturn = ENC_UTF8;
			else
				forReturn = ENC_DEFAULT;

			_close(file);
		}
		return forReturn;
	}

	std::ostream& operator<< (std::ostream& os, const encoding_t& enc)
	{
		switch (enc)
		{
		case ENC_UTF16LE:
			os << "UTF-16LE";
			break;
		case ENC_UTF8:
			os << "UTF-8";
			break;
		case ENC_UNKNOWN:
			os << "<encoding-error>";
			break;
		case ENC_DEFAULT:
			os << "<encoding-unknown>";
			break;
		}
		return os;
	}
} 
