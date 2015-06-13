//
//  ofxKsmrTimeEventUI.cpp
//  SignageController
//
//  Created by Ovis aries on 2015/02/25.
//
//

#include "ofxKsmrTimeEventUI.h"

void ofxKsmrTimeEventUI::setup(ofxKsmrOscDistributor *dst){

	timer.setOscDistributor(dst);
	timer.load("settings/timeEvent.xml");

	ofAddListener(ofxKsmrTimeEvent::newEvent, this, &ofxKsmrTimeEventUI::timeEvent);

	gui.setFont("system/Avenir_bold.ttf");
	gui.setFontSize(OFX_UI_FONT_SMALL, 8);
	gui.setFontSize(OFX_UI_FONT_MEDIUM, 8);
	gui.setFontSize(OFX_UI_FONT_LARGE, 12);
	gui.setup();
	gui.setWidth(480);
	gui.addLabel("TimeEvent");
	gui.addSpacer();
	gui.addButton("Sync", false);
	gui.addButton("Save", false);
	gui.addButton("Disable Event", false);
	gui.addButton("Enable Event", false);
	gui.addButton("Simulate Event", false);
	gui.addLabel("Editor");
	gui.addSpacer();
	gui.addButton("Push", false);
	gui.addButton("Remove", false);
	gui.addLabel("EventName");
	labelInput = gui.addTextInput("eventLabel", "");
	gui.addSpacer();
	gui.addLabel("Time");
	gui.addIntSlider("Hour", 0, 24, &param_hour);
	gui.addIntSlider("Min", 0, 60, &param_min);
	gui.addIntSlider("Sec", 0, 60, &param_sec);
	gui.addSpacer();
	gui.addLabel("RepeatOption");
	gui.addToggle("Repeat", &param_Repeat);
	gui.addIntSlider("Offset_Hour", 0, 24, &param_oHour);
	gui.addIntSlider("Offset_Min", 0, 60, &param_oMin);
	gui.addIntSlider("Offset_Sec", 0, 60, &param_oSec);

	gui.addSpacer();
	gui.addLabel("Presets",OFX_UI_FONT_LARGE);
	gui.addButton("change Background", false);
	gui.addButton("scene Change", false);
	gui.addButton("Shutdown", false);
	gui.addButton("Reboot", false);
	gui.addButton("Sleep", false);

	gui.autoSizeToFitWidgets();
	labelInput->setAutoClear(false);

	ofAddListener(ofEvents().mousePressed, this, &ofxKsmrTimeEventUI::mousePressed);
	ofAddListener(gui.newGUIEvent, this, &ofxKsmrTimeEventUI::guiEvent);

}

void ofxKsmrTimeEventUI::update(){

	timer.update();

}

void ofxKsmrTimeEventUI::guiEvent(ofxUIEventArgs &e) {

	ofxUIWidget* w = e.widget;
	
	if (w->getState() == OFX_UI_STATE_DOWN){

		if (w->getName() == "Disable Event"){
			ofxOscSender sender;
			sender.setup(sync_addr, sync_port);

			ofxOscMessage m;
			m.setAddress("/ksmr/timeEvent/enable");
			m.addIntArg(0);

			sender.sendMessage(m);
		}
		if (w->getName() == "Enable Event"){
			ofxOscSender sender;
			sender.setup(sync_addr, sync_port);

			ofxOscMessage m;
			m.setAddress("/ksmr/timeEvent/enable");
			m.addIntArg(1);

			sender.sendMessage(m);
		}

		if (w->getName() == "Simulate Event"){
			ofxOscSender sender;
			sender.setup(sync_addr, sync_port);

			ofxOscMessage m;
			m.setAddress("/ksmr/timeEvent/simulate");
			m.addStringArg(labelInput->getTextString());

			sender.sendMessage(m);
		}

		if (w->getName() == "scene Change"){
			labelInput->setTextString("Transition:Box2d_1:0");
			param_Repeat = true;
		}
		if (w->getName() == "change Background"){
			labelInput->setTextString("Command:background:Param/i/bgNum/0");
			param_Repeat = true;
		}

		if (w->getName() == "Shutdown"){
			labelInput->setTextString("Shutdown");
			param_Repeat = false;
		}
		if (w->getName() == "Reboot"){
			labelInput->setTextString("Reboot");
			param_Repeat = false;
		}
		if (w->getName() == "Sleep"){
			labelInput->setTextString("Sleep");
			param_Repeat = false;
		}


		gui.addButton("change Background", false);
		gui.addButton("scene Change", false);
		gui.addButton("Shutdown", false);
		gui.addButton("Reboot", false);
		gui.addButton("Sleep", false);

		if (w->getName() == "Push"){
			if (param_Repeat){

				timer.addRepeatEvent(labelInput->getTextString(),
									 param_hour, param_min, param_sec,
									 param_oHour, param_oMin, param_oSec);

			}else{

				timer.addSingleEvent(labelInput->getTextString(),
									 param_hour, param_min, param_sec);

			}
		}

		if (w->getName() == "Remove"){
			timer.removeEvent(labelInput->getTextString());
		}

		if (w->getName() == "Sync"){
			timer.syncTimeTable(sync_addr, sync_port, false);
		}

		if (w->getName() == "Save"){
			timer.syncTimeTable(sync_addr, sync_port, true);
			timer.save("settings/timeEvent.xml");
		}

	}

}

void ofxKsmrTimeEventUI::draw(int x,int y){

	drawPos.set(x+500,y);
	gui.setPosition(x, y);

	ofSetColor(255);
	ofPushMatrix();
	ofTranslate(x+500, y);

	map<string, ofPtr<ofxKsmrTimeQueue> >::iterator it;

	int cnt = 0;
	for(it = timer.queues.begin(); it != timer.queues.end(); ++it){
		ofPtr <ofxKsmrTimeQueue> q = (*it).second;
		string tx = q->label + " - " + ofToString(q->h) + ":" + ofToString(q->m) + ":" + ofToString(q->s) + " [Offset = " + ofToString(q->ofs_h) + ":" + ofToString(q->ofs_m) + ":" + ofToString(q->ofs_s) + "]";

		ofPushMatrix();
		ofTranslate(10, cnt*20);
		ofNoFill();
		ofSetColor(255);
		ofRect(0, 0, button_width, 20);
		ofFill();

		ofColor c = ofColor(0,0,0);
		if (q->label == activeLabel) c += ofColor(80,80,0);
		if (q->passed > 0) c += ofColor(MAX(255 - q->passed,0));
		ofSetColor(c);

		ofRect(0, 0, button_width, 20);

		ofSetColor(255, 255, q->repeat ? 0 : 255);
		ofDrawBitmapString(tx, 3,13);
		ofPopMatrix();

		cnt++;
	}

	ofPopMatrix();
}

void ofxKsmrTimeEventUI::timeEvent(ofxKsmrTimeEventArgs &arg){

}

void ofxKsmrTimeEventUI::mousePressed(ofMouseEventArgs &arg){

	ofVec2f m = arg - drawPos;

	ofRectangle outline = ofRectangle(drawPos.x,drawPos.y,button_width,20*timer.queues.size());

	if (outline.inside(arg)){

		map<string, ofPtr<ofxKsmrTimeQueue> >::iterator it;

		int cnt = 0;

		for(it = timer.queues.begin(); it != timer.queues.end(); ++it){

			ofPtr<ofxKsmrTimeQueue> q = (*it).second;

			if (ofRectangle(10,cnt*20,button_width,20).inside(m)){

				activeLabel = q->label;
				param_hour	= q->h;
				param_min	= q->m;
				param_sec	= q->s;
				param_oHour	= q->ofs_h;
				param_oMin	= q->ofs_m;
				param_oSec	= q->ofs_s;
				param_Repeat= q->repeat;
				labelInput->setTextString(q->label);

			}

			cnt++;
		}

	}

}