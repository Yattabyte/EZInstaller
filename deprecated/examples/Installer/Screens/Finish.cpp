#include "Screens/Finish.h"
#include "StringConversions.h"
#include "Directory.h"
#include "Installer.h"
#include <filesystem>


static LRESULT CALLBACK WndProc(HWND /*hWnd*/, UINT /*message*/, WPARAM /*wParam*/, LPARAM /*lParam*/);

Finish_Screen::~Finish_Screen()
{
	UnregisterClass("FINISH_SCREEN", m_hinstance);
	DestroyWindow(m_hwnd);
	DestroyWindow(m_checkbox);
	DestroyWindow(m_btnClose);
	for (auto checkboxHandle : m_shortcutCheckboxes)
		DestroyWindow(checkboxHandle);
}

Finish_Screen::Finish_Screen(Installer* Installer, const HINSTANCE hInstance, const HWND parent, const vec2& pos, const vec2& size)
	: Screen(Installer, pos, size)
{
	// Create window class
	m_hinstance = hInstance;
	m_wcex.cbSize = sizeof(WNDCLASSEX);
	m_wcex.style = CS_HREDRAW | CS_VREDRAW;
	m_wcex.lpfnWndProc = WndProc;
	m_wcex.cbClsExtra = 0;
	m_wcex.cbWndExtra = 0;
	m_wcex.hInstance = hInstance;
	m_wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
	m_wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	m_wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	m_wcex.lpszMenuName = nullptr;
	m_wcex.lpszClassName = "FINISH_SCREEN";
	m_wcex.hIconSm = LoadIcon(m_wcex.hInstance, IDI_APPLICATION);
	RegisterClassEx(&m_wcex);
	m_hwnd = CreateWindow("FINISH_SCREEN", "", WS_OVERLAPPED | WS_CHILD | WS_VISIBLE, pos.x, pos.y, size.x, size.y, parent, nullptr, hInstance, nullptr);
	SetWindowLongPtr(m_hwnd, GWLP_USERDATA, (LONG_PTR)this);
	setVisible(false);

	// Create check-boxes
	m_checkbox = CreateWindow("Button", "Show Installation directory on close", WS_OVERLAPPED | WS_VISIBLE | WS_CHILD | BS_CHECKBOX | BS_AUTOCHECKBOX, 10, 150, size.x, 15, m_hwnd, (HMENU)1, hInstance, nullptr);
	CheckDlgButton(m_hwnd, 1, BST_CHECKED);

	// Shortcuts
	const auto desktopStrings = m_Installer->m_mfStrings[L"shortcut"];
	const auto startmenuStrings = m_Installer->m_mfStrings[L"startmenu"];
	size_t numD = std::count(desktopStrings.begin(), desktopStrings.end(), L',') + 1ULL;
	size_t numS = std::count(startmenuStrings.begin(), startmenuStrings.end(), L',') + 1ULL;
	m_shortcutCheckboxes.reserve(numD + numS);
	m_shortcuts_d.reserve(numD + numS);
	m_shortcuts_s.reserve(numD + numS);
	size_t last = 0;
	if (!desktopStrings.empty())
		for (size_t x = 0; x < numD; ++x) {
			// Find end of shortcut
			auto nextComma = desktopStrings.find(L',', last);
			if (nextComma == std::wstring::npos)
				nextComma = desktopStrings.size();

			// Find demarcation point where left half is the shortcut path, right half is the shortcut name
			m_shortcuts_d.push_back(desktopStrings.substr(last, nextComma - last));

			// Skip whitespace, find next element
			last = nextComma + 1ULL;
			while (last < desktopStrings.size() && (desktopStrings[last] == L' ' || desktopStrings[last] == L'\r' || desktopStrings[last] == L'\t' || desktopStrings[last] == L'\n'))
				last++;
		}
	last = 0;
	if (!startmenuStrings.empty())
		for (size_t x = 0; x < numS; ++x) {
			// Find end of shortcut
			auto nextComma = startmenuStrings.find(L',', last);
			if (nextComma == std::wstring::npos)
				nextComma = startmenuStrings.size();

			// Find demarcation point where left half is the shortcut path, right half is the shortcut name
			m_shortcuts_s.push_back(startmenuStrings.substr(last, nextComma - last));

			// Skip whitespace, find next element
			last = nextComma + 1ULL;
			while (last < startmenuStrings.size() && (startmenuStrings[last] == L' ' || startmenuStrings[last] == L'\r' || startmenuStrings[last] == L'\t' || startmenuStrings[last] == L'\n'))
				last++;
		}
	int vertical = 170;
	int checkIndex = 2;
	for (const auto& shortcut : m_shortcuts_d) {
		const auto name = std::wstring(&shortcut[1], shortcut.length() - 1);
		m_shortcutCheckboxes.push_back(CreateWindowW(L"Button", (L"Create a shortcut for " + name + L" on the desktop").c_str(), WS_OVERLAPPED | WS_VISIBLE | WS_CHILD | BS_CHECKBOX | BS_AUTOCHECKBOX, 10, vertical, size.x, 15, m_hwnd, (HMENU)(LONGLONG)checkIndex, hInstance, nullptr));
		CheckDlgButton(m_hwnd, checkIndex, BST_CHECKED);
		vertical += 20;
		checkIndex++;
	}
	for (const auto& shortcut : m_shortcuts_s) {
		const auto name = std::wstring(&shortcut[1], shortcut.length() - 1);
		m_shortcutCheckboxes.push_back(CreateWindowW(L"Button", (L"Create a shortcut for " + name + L" in the start-menu").c_str(), WS_OVERLAPPED | WS_VISIBLE | WS_CHILD | BS_CHECKBOX | BS_AUTOCHECKBOX, 10, vertical, size.x, 15, m_hwnd, (HMENU)(LONGLONG)checkIndex, hInstance, nullptr));
		CheckDlgButton(m_hwnd, checkIndex, BST_CHECKED);
		vertical += 20;
		checkIndex++;
	}

	// Create Buttons
	constexpr auto BUTTON_STYLES = WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON;
	m_btnClose = CreateWindow("BUTTON", "Close", BUTTON_STYLES, size.x - 95, size.y - 40, 85, 30, m_hwnd, nullptr, hInstance, nullptr);
}

void Finish_Screen::enact()
{
	// Does nothing
}

void Finish_Screen::paint()
{
	PAINTSTRUCT ps;
	Graphics graphics(BeginPaint(m_hwnd, &ps));

	// Draw Background
	LinearGradientBrush backgroundGradient(
		Point(0, 0),
		Point(0, m_size.y),
		Color(50, 25, 255, 125),
		Color(255, 255, 255, 255)
	);
	graphics.FillRectangle(&backgroundGradient, 0, 0, m_size.x, m_size.y);

	// Preparing Fonts
	FontFamily  fontFamily(L"Segoe UI");
	Font        bigFont(&fontFamily, 25, FontStyleBold, UnitPixel);
	SolidBrush  blueBrush(Color(255, 25, 125, 225));

	// Draw Text
	graphics.SetSmoothingMode(SmoothingMode::SmoothingModeAntiAlias);
	graphics.DrawString(L"Installation Complete", -1, &bigFont, PointF{ 10, 10 }, &blueBrush);

	EndPaint(m_hwnd, &ps);
}

void Finish_Screen::goClose()
{
	m_showDirectory = (IsDlgButtonChecked(m_hwnd, 1) != 0U);
	const auto iyattaDir = m_Installer->getDirectory() + "\\" + m_Installer->getPackageName();
	// Open the Installation directory + if user wants it to
	if (m_showDirectory)
		ShellExecute(nullptr, "open", iyattaDir.c_str(), nullptr, nullptr, SW_SHOWDEFAULT);

	// Create Shortcuts
	int x = 2;
	for (const auto& shortcut : m_shortcuts_d) {
		if (IsDlgButtonChecked(m_hwnd, x) != 0U) {
			std::error_code ec;
			const auto nonwideShortcut = yatta::from_wideString(shortcut);
			auto srcPath = iyattaDir;
			if (srcPath.back() == '\\')
				srcPath = std::string(&srcPath[0], srcPath.size() - 1ULL);
			srcPath += nonwideShortcut;
			const auto dstPath = yatta::Directory::GetDesktopPath() + "\\" + std::filesystem::path(srcPath).filename().string();
			createShortcut(srcPath, iyattaDir, dstPath);
		}
		x++;
	}
	for (const auto& shortcut : m_shortcuts_s) {
		if (IsDlgButtonChecked(m_hwnd, x) != 0U) {
			std::error_code ec;
			const auto nonwideShortcut = yatta::from_wideString(shortcut);
			auto srcPath = iyattaDir;
			if (srcPath.back() == '\\')
				srcPath = std::string(&srcPath[0], srcPath.size() - 1ULL);
			srcPath += nonwideShortcut;
			const auto dstPath = yatta::Directory::GetStartMenuPath() + "\\" + std::filesystem::path(srcPath).filename().string();
			createShortcut(srcPath, iyattaDir, dstPath);
		}
		x++;
	}
	PostQuitMessage(0);
}

void Finish_Screen::createShortcut(const std::string& srcPath, const std::string& wrkPath, const std::string& dstPath)
{
	IShellLink* psl;
	if (SUCCEEDED(CoCreateIyattaance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_IShellLink, (LPVOID*)&psl))) {
		IPersistFile* ppf;

		// Set the path to the shortcut target and add the description.
		psl->SetPath(srcPath.c_str());
		psl->SetWorkingDirectory(wrkPath.c_str());
		psl->SetIconLocation(srcPath.c_str(), 0);

		// Query IShellLink for the IPersistFile interface, used for saving the
		// shortcut in persistent storage.
		if (SUCCEEDED(psl->QueryInterface(IID_IPersistFile, (LPVOID*)&ppf))) {
			WCHAR wsz[MAX_PATH];

			// Ensure that the string is Unicode.
			MultiByteToWideChar(CP_ACP, 0, (dstPath + ".lnk").c_str(), -1, wsz, MAX_PATH);

			// Save the link by calling IPersistFile::Save.
			ppf->Save(wsz, TRUE);
			ppf->Release();
		}
		psl->Release();
	}
}

static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	const auto ptr = reinterpret_cast<Finish_Screen*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
	const auto controlHandle = HWND(lParam);
	if (message == WM_PAINT)
		ptr->paint();
	else if (message == WM_CTLCOLORSTATIC) {
		// Make check-box text background color transparent
		bool isCheckbox = controlHandle == ptr->m_checkbox;
		for (auto chkHandle : ptr->m_shortcutCheckboxes)
			if (controlHandle == chkHandle) {
				isCheckbox = true;
				break;
			}
		if (isCheckbox) {
			SetBkMode(HDC(wParam), TRANSPARENT);
			return (LRESULT)GetStockObject(NULL_BRUSH);
		}
	}
	else if (message == WM_COMMAND) {
		const auto notification = HIWORD(wParam);
		if (notification == BN_CLICKED) {
			if (controlHandle == ptr->m_btnClose)
				ptr->goClose();
		}
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}