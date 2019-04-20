#pragma once
#ifndef AGREEMENT_H
#define AGREEMENT_H

#include "Screen.h"


/** This state encapuslates the "Accept the license agreement" - Screen" state. */
class Agreement : public Screen {
public:
	// Public (de)Constructors
	~Agreement();
	Agreement(Installer * installer, const HINSTANCE hInstance, const HWND parent, const vec2 & pos, const vec2 & size);


	// Public Interface Implementations
	virtual void enact() override;
	virtual void paint() override;


	// Public Methods
	/** Check the 'I agree' button. */
	void checkYes();
	/** Check the 'I doon't agree' button. */
	void checkNo();
	/** Switch to the previous state. */
	void goPrevious();
	/** Switch to the next state. */
	void goNext();
	/** Switch to the cancel state. */
	void goCancel();


	// Public Attributes
	HWND m_log = nullptr, m_checkYes = nullptr, m_btnPrev = nullptr, m_btnNext = nullptr, m_btnCancel = nullptr;
	bool m_agrees = false;
};

#endif // AGREEMENT_H