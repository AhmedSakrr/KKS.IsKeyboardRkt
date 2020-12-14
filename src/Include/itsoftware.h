//
// kks.h
//
// workspace for the KKS Component Framework.
//
#pragma once
#ifndef __ITSOFTWARE_H__
#define __ITSOFTWARE_H__

#include <crtdbg.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <exception>
#include <windows.h>	
#include "atlcomcli.h"		
#include <string>
#include <codecvt>
using namespace ATL;

//
// Put spesific methods in namespace
//
namespace ItSoftware
{	
	using namespace std;
	// 
	// Macros
	//
	#define DECIMALTOSTRING2(d) #d
	#define DECIMALTOSTRING1(d) DECIMALTOSTRING2(d)
	#define ERR_AT_LOCATION(e) __FILE__ "(" DECIMALTOSTRING1(__LINE__) "): " #e 
	#define ERROR_HERE __FILE__ "(" DECIMALTOSTRING2(__LINE__) "): "  

	//
	// Debug Macros
	//
	#define ASSERT _ASSERTE

	#ifdef _DEBUG
		#define VERIFY ASSERT
	#else
		#define VERIFY(e) (e)
	#endif

	#ifdef _DEBUG
		inline auto Trace(wchar_t const * format, ...) -> void
		{
			va_list args;
			va_start(args, format);
	
			wchar_t buffer[256];
	
			ASSERT(-1 != _vsnwprintf_s(buffer, _countof(buffer) - 1, format, args));
	
			va_end(args);
	
			OutputDebugString(buffer);
		}

		struct Tracer
		{
			char const* m_filename;
			unsigned m_line;
	
			Tracer(char const * filename, unsigned const line)
				: m_filename(filename),
				m_line(line)
			{
	
			}
	
			template<typename... Args>
			auto operator()(wchar_t const * format, Args... args) const -> void
			{
				wchar_t buffer[256];
	
				auto count = swprintf_s(buffer, L"%S(%d): ", m_filename, m_line);
	
				ASSERT(count != -1);
	
				ASSERT(_snwprintf_s(buffer + count,
					_countof(buffer) - count,
					_countof(buffer) - count - 1,
					format,
					args...) != -1);
	
				OutputDebugString(buffer);
			}
		};
	#endif

	#ifdef _DEBUG
		#define TRACE Tracer(__FILE__, __LINE__)
	#else
		#define TRACE __noop
	#endif

	//
	// Includes
	//
	
	//
	// Common Structures
	//
	typedef struct WAVEHEADER {
		char	szRIFF[4];      // 'RIFF' 
		DWORD	dwFileSize;     // Filesize - 8. 8=("RIFF"+"fmt ") 
		char	szWAVEfmt_[8];	// 'WAVEfmt ' 
		DWORD	dwfmtLen;       // 0x10 
		WORD	wWaveType;      // type wave format. PCM-header eller 0x01 
		WORD	wChannels;      // channels. 1-mono, 2-stereo 
		DWORD	nSamplesPrSec;  // samples pr second 
		DWORD	nAvgBytesPerSec;// bytes pr. second 
		WORD	wBlockAlign;	// bytes pr. sample 
		WORD	wBitsPerSample; // bits pr. channel 
		char	szDATA[4];		// text: 'data' 
		DWORD	dwDataSize;		// sound data size 
	} WAVEHEADER, *LPWAVEHEADER;	
	
	namespace Encoding
	{
		struct UTF8
		{
			
			static std::wstring ToString( const std::string& str )
			{
				typedef std::codecvt_utf8<wchar_t> convert_typeX;
				std::wstring_convert<convert_typeX, wchar_t> converterX;

				return converterX.from_bytes( str );
			}
			
			static std::string ToString( const std::wstring& wstr )
			{
				typedef std::codecvt_utf8<wchar_t> convert_typeX;
				std::wstring_convert<convert_typeX, wchar_t> converterX;

				return converterX.to_bytes( wstr );
			}			

			static unsigned char* ToBytes( const std::wstring& wstr, long* cbLength)
			{
				typedef std::codecvt_utf8<wchar_t> convert_typeX;
				std::wstring_convert<convert_typeX, wchar_t> converterX;				

				std::string s = converterX.to_bytes( wstr );

				size_t length = s.length()*sizeof( char );
				unsigned char* pBytes = new (std::nothrow) unsigned char[length];
				if ( pBytes == nullptr )
				{
					*cbLength = 0;
					return nullptr;
				}

				memcpy( (void*) pBytes, s.data(), length );

				*cbLength = (long)length;

				return pBytes;
			}
		};
	}
	
	struct Util
	{
	
		//
		// Common function
		//
		static CComBSTR GetLastErrorDescription()
		{
			LPVOID lpMsgBuf;
			FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER |
				FORMAT_MESSAGE_FROM_SYSTEM |
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				GetLastError(),
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
				(wchar_t *)&lpMsgBuf,
				0,
				NULL);

			// Copy buffer contet to bstr...
			CComBSTR bstr;
			bstr += (wchar_t *)lpMsgBuf;

			// Free the buffer...
			LocalFree(lpMsgBuf);

			// Return value...
			return bstr;
		}// inline CComBSTR GetLastErrorDesc( )

		static CComBSTR GetErrorDescription(DWORD dwError)
		{
			LPVOID lpMsgBuf;
			FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER |
				FORMAT_MESSAGE_FROM_SYSTEM |
				FORMAT_MESSAGE_IGNORE_INSERTS,
				NULL,
				dwError,
				MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), // Default language
				(wchar_t *)&lpMsgBuf,
				0,
				NULL);

			// Copy buffer contet to bstr...
			CComBSTR bstr;
			bstr += (wchar_t *)lpMsgBuf;

			// Free the buffer...
			LocalFree(lpMsgBuf);

			// Return value...
			return bstr;
		}// inline CComBSTR GetLastErrorDesc( )

		static CComBSTR GetCoLastErrorInfoDescription()
		{
			IErrorInfo *pErrInfo;
			HRESULT hr = GetErrorInfo(NULL, (IErrorInfo **)&pErrInfo);
			CComBSTR bstr;
			if (hr == S_OK && pErrInfo) {
				pErrInfo->GetDescription(&bstr);
			}

			return bstr;
		}// inline CComBSTR GetLastErrorDesc( )

		static void MsgBox(BSTR text, BSTR topic)
		{
			MessageBoxW(NULL, text, topic, MB_OK | 0x00200000);
		}

		static void MsgBox(char *text, char *topic)
		{
			MessageBoxA(NULL, text, topic, MB_OK | 0x00200000);
		}

		static void SplitTime(ULONG time_ms, long* part_hours, long* part_minutes, long* part_seconds, long* part_ms)
		{
			long seconds = time_ms / 1000;

			*part_hours = 0;
			*part_minutes = 0;;
			*part_seconds = 0;
			*part_ms = 0;

			*part_hours = seconds / 3600;
			*part_minutes = (seconds - (*part_hours * 3600)) / 60;
			*part_seconds = seconds - (*part_hours * 3600) - (*part_minutes * 60);
			*part_ms = time_ms - (*part_seconds * 1000) - (*part_minutes * 60 * 1000) - (*part_hours * 3600 * 1000);
		}
	};

	class GeneralException : public exception
	{
		wstring m_message;
	public:

		GeneralException()
		{

		}

		GeneralException( wstring message )
			: m_message( message )
		{

		}

		wstring GetMessage()
		{
			return m_message;
		}

		virtual wstring ToString()
		{			
			wstringstream toString;
			toString << L"## Exception Information ##" << endl;
			toString << L"Type: GeneralException" << endl;
			toString << L"Message: " << this->GetMessage() << endl;
			toString << L"What: " << CComBSTR(this->what()).operator LPWSTR() << endl;

			return toString.str();
		}
	};

	class ArgumentException : public GeneralException
	{
		wstring m_paramName;
	public:

		wstring GetParamName()
		{
			return this->m_paramName;
		}

		ArgumentException( wstring message, wstring param )	
			: GeneralException(message),
			  m_paramName(param)
		{
						
		}

		virtual wstring ToString()
		{
			wstringstream toString;
			toString << L"## Exception Information ##" << endl;
			toString << L"Type: ArgumentNullException" << endl;
			toString << L"Message: " << this->GetMessageW() << endl;
			toString << L"Parameter: " << this->GetParamName() << endl;
			toString << L"What: " << CComBSTR(this->what()).operator LPWSTR() << endl;

			return toString.str();
		}
	};

}// namespace KKS

//////////////////////////////////////////////////////////////////////////////////////////
#endif