#ifndef __MOUSE_H__
#define __MOUSE_H__


#include <windows.h>

class Mouse
{
public:

	static const float DEFAULT_SENSITIVITY;

	Mouse();
	~Mouse();

	bool attach(HWND hwnd);
	void detach();

	void filterRelativeInput(bool enableFiltering);
	void handleMsg(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	void hideCursor(bool hideCursor);
	void setPosition(UINT x, UINT y);
	void setSensitivity(float sensitivity);


	bool cursorIsVisible() const
	{
		return m_cursorVisible;
	}

	int getXPosAbsolute() const
	{
		return m_xPosAbsolute;
	}

	int getYPosAbsolute()const
	{
		return m_yPosAbsolute;
	}

	float getXPosRelative() const
	{
		return m_xPosRelative;
	}

	float getYPosRelative() const
	{
		return m_yPosRelative;
	}

	bool relativeInputIsFiltered() const
	{
		return m_filterRelativeInput;
	}

private:
	static const int TEMP_BUFFER_SIZE = 40;
	static const int TOTAL_HISTORY_ENTRIES = 10;

	//Mouse(const Mouse &m);
	//Mouse &operator = (const Mouse &other);

	void filterInput(float x, float y);

	static BYTE m_tempBuffer[TEMP_BUFFER_SIZE];

	HWND m_hWnd;
	int m_xPosAbsolute;
	int m_yPosAbsolute;

	float m_xPosRelative;
	float m_yPosRelative;
	float m_sensitivity;

	bool m_filterRelativeInput;
	bool m_cursorVisible;
	float m_filtered[2];
	float m_history[TOTAL_HISTORY_ENTRIES * 2];

};
#endif