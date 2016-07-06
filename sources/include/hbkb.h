#ifndef HBKB_H_

#define HBKB_H_


#include <3ds.h>

#include <string>


class HB_Keyboard
{

public:
	
HB_Keyboard();
	
virtual ~HB_Keyboard();

	// Call Keyboard
	
u8 HBKB_CallKeyboard(touchPosition TouchScreenPos);

	// Return User Input
	
std::string HBKB_CheckKeyboardInput();

	
// Clean User Input
	void HBKB_Clean();


private:
	
void KeyInteraction(u8 &Key);
	
void ChangeString(u8 &Key);
	
void GFXBufferInteraction();

	
std::string UserInput;
	bool isShift;
	
bool isCaps;

	
u8 KeyboardState;
	
u8 CurrentKey;

};


#endif /* HBKB_H_ */
