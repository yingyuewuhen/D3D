#include "mouse.h"

const float Mouse::DEFAULT_SENSITIVITY = 0.5f;
BYTE Mouse::m_tempBuffer[TEMP_BUFFER_SIZE];


Mouse::Mouse()
{
	//初始化
	m_hWnd = 0;
	m_xPosAbsolute = 0;
	m_yPosAbsolute = 0;

	m_xPosRelative = 0;
	m_yPosRelative = 0;

	m_filterRelativeInput = true;
	m_cursorVisible = true;

	m_sensitivity = DEFAULT_SENSITIVITY;
	m_filtered[0] = m_filtered[1] = 0.0f;

	memset(&m_history[0], 0, sizeof(m_history));		
}



Mouse::~Mouse()
{
	detach();
}


//与窗口句柄所在窗口相关联
bool Mouse::attach(HWND hwnd)
{
	if (!hwnd)
		return false;

	if (!m_hWnd)
	{
		RAWINPUTDEVICE rid[1] = { 0 };
		rid[0].dwFlags = 0;
		rid[0].hwndTarget = hwnd;
		rid[0].usUsage = 2;
		rid[0].usUsagePage = 1;

		if (!RegisterRawInputDevices(rid, 1, sizeof(rid[0])))
			return false;
		m_hWnd = hwnd;
	}
	return true;
}

void Mouse::detach()
{
	if (!m_cursorVisible)
		hideCursor(m_cursorVisible);

	m_hWnd = 0;
}

void Mouse::filterRelativeInput(bool enableFiltering)
{
	m_filterRelativeInput = enableFiltering;
}


//处理键盘消息
void Mouse::handleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	RAWINPUT *pRaw = 0;
	UINT size = TEMP_BUFFER_SIZE;

	switch (msg)
	{
	default:
		break;
	case WM_INPUT:
	{
		//获取设备的元数据
		GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT,
			m_tempBuffer, &size, sizeof(RAWINPUTHEADER));

		pRaw = reinterpret_cast<RAWINPUT *>(m_tempBuffer);

		//判断元数据的来源
		if (pRaw->header.dwType == RIM_TYPEMOUSE)
		{
			if (m_filterRelativeInput)
			{
				filterInput(static_cast<float>(pRaw->data.mouse.lLastX),
					static_cast<float>(pRaw->data.mouse.lLastY));

				m_xPosRelative = m_filtered[0];
				m_yPosRelative = m_filtered[1];
			}
			else
			{
				m_xPosRelative = static_cast<float>(pRaw->data.mouse.lLastX);
				m_yPosRelative = static_cast<float>(pRaw->data.mouse.lLastY);
			}
		}
		break;
	}
	case WM_MOUSEMOVE:
	{
		m_xPosAbsolute = static_cast<int>(static_cast<short>(LOWORD(lParam)));
		m_yPosAbsolute = static_cast<int>(static_cast<short>(HIWORD(lParam)));
		break;
	}
	}
	
}

void Mouse::hideCursor(bool hideCursor)
{
	if (hideCursor)
	{
		m_cursorVisible = false;  //不显示
		while (ShowCursor(FALSE) >= 0);
		{

		}
	}
	else
	{
		m_cursorVisible = true;
		while (ShowCursor(TRUE) < 0)
		{

		}
	}
}


//根据窗口的位置设置鼠标的位置，重置鼠标数据
void Mouse::setPosition(UINT x, UINT y)
{
	POINT pt = { x, y };

	if (ClientToScreen(m_hWnd, &pt))
	{
		SetCursorPos(pt.x, pt.y);

		m_xPosAbsolute = x;
		m_yPosAbsolute = y;

		m_xPosRelative = 0.0f;
		m_yPosRelative = 0.0f;
	}
}

void Mouse::setSensitivity(float sensitivity)
{
	m_sensitivity = sensitivity;
}

void Mouse::filterInput(float x, float y)
{
	memmove(&m_history[2], &m_history[0], sizeof(float) * ((TOTAL_HISTORY_ENTRIES - 1) * 2));

	m_history[0] = x;
	m_history[1] = y;

	float averageX = 0.0f;
	float averageY = 0.0f;
	float averageTotal = 0.0f;

	float currentWeight = 1.0f;

	for (int i = 0; i < TOTAL_HISTORY_ENTRIES; i++)
	{
		averageX += m_history[i * 2] * currentWeight;
		averageY += m_history[i * 2 + 1] * currentWeight;

		averageTotal += 1.0f * currentWeight;
		currentWeight *= m_sensitivity;
	}

	m_filtered[0] = averageX / averageTotal;
	m_filtered[1] = averageY / averageTotal;

}

