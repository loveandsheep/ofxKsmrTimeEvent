//
//  ofxKsmrTimeEventUI.h
//  SignageController
//
//  Created by Ovis aries on 2015/02/25.
//
//

#ifndef __SignageController__ofxKsmrTimeEventUI__
#define __SignageController__ofxKsmrTimeEventUI__

#include "ofxKsmrTimeEvent.h"
#include "ofxUI.h"

class ofxKsmrTimeEventUI{
public:

	void setup(ofxKsmrOscDistributor *dst);
	void update();
	void draw(int x,int y);

	void mouseMoved(ofMouseEventArgs & arg);
	void mousePressed(ofMouseEventArgs & arg);
	void mouseReleased(ofMouseEventArgs & arg);

	void guiEvent(ofxUIEventArgs &e);

	ofVec2f drawPos;
	string activeLabel;

	ofxKsmrTimeEvent timer;

	ofxUICanvas gui;

	ofxUITextInput* labelInput;
	int param_hour;
	int param_min;
	int param_sec;
	int param_oHour;
	int param_oMin;
	int param_oSec;
	bool param_Repeat;

	void timeEvent(ofxKsmrTimeEventArgs & arg);

	string sync_addr;
	int sync_port;

	const int button_width = 500;
};

#endif /* defined(__SignageController__ofxKsmrTimeEventUI__) */
