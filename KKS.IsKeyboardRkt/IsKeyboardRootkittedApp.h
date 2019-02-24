#pragma once
#include "../Include/IS_KEYBOARD_RKT_OUTPUT_DATA.h"

namespace IsKeyboardRootkitted
{
	class IsKeyboardRootkittedApp
	{
	private:
		bool m_bHitIOTR0;
		bool m_bHitIOTR1;
		bool m_bHitIOTR2;
		bool m_bHitIOTR3;

		bool m_bHitIoApicIRQ1;

		bool m_bRunFirstHit;

		bool m_bVerboseOutput;

		WORD m_wHitPortIOTR0;
		WORD m_wHitPortIOTR1;
		WORD m_wHitPortIOTR2;
		WORD m_wHitPortIOTR3;

		unsigned long long  m_qwHitIOTR0;
		unsigned long long	m_qwHitIOTR1;
		unsigned long long	m_qwHitIOTR2;
		unsigned long long	m_qwHitIOTR3;

		bool GetDevicePath(LPCGUID GUID_DEVINTEFACE, PWCHAR DevicePath, size_t BufLen);
		void AlertSmiHandlerFound(PWCHAR iotr, WORD port);
		void AlertSmiHandlerFound(PWCHAR iotr);
		void AlertW32ErrorMessage(DWORD dwError);
		void AlertNTErrorMessage(NTSTATUS dwError);
		int AlertPressAnyKeyAndReturn(int returnCode);
		void AlertDriverError(PIS_KEYBOARD_RKT_OUTPUT_DATA pOutputData);

		BOOL ProcessResult(IS_KEYBOARD_RKT_OUTPUT_DATA *pOutputData);		

		void ClearState()
		{
			m_bHitIOTR0 = false;
			m_bHitIOTR1 = false;
			m_bHitIOTR2 = false;
			m_bHitIOTR3 = false;
			m_bHitIoApicIRQ1 = false;
			m_wHitPortIOTR0 = 0;
			m_wHitPortIOTR1 = 0;
			m_wHitPortIOTR2 = 0;
			m_wHitPortIOTR3 = 0;
			m_qwHitIOTR0 = 0;
			m_qwHitIOTR1 = 0;
			m_qwHitIOTR2 = 0;
			m_qwHitIOTR3 = 0;			
			m_bRunFirstHit = false;			
		}
	protected:
	public:
		IsKeyboardRootkittedApp()			
		{
			this->ClearState();
			this->m_bVerboseOutput = false;
		}

		int Main();
	};
}