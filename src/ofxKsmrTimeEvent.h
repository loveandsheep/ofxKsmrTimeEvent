//
//  ofxKsmrTimeEvent.h
//  SignageController
//
//  Created by Ovis aries on 2015/02/25.
//
//

#ifndef __SignageController__ofxKsmrTimeEvent__
#define __SignageController__ofxKsmrTimeEvent__

#include "ofMain.h"
#include "ofxKsmrOscDistributor.h"

class ofxKsmrTimeEventArgs;
class ofxKsmrTimeQueue;

class ofxKsmrTimeEvent{
public:

	void update();

	bool addSingleEvent(string Label, int hour, int min, int sec);
	bool addRepeatEvent(string Label, int hour, int min, int sec,
						int offset_h, int offset_m, int offset_s);

	void setOscDistributor(ofxKsmrOscDistributor *dst);
	void removeEvent(string label);

	void save(ofXml &xml);
	void save(string file);
	void load(ofXml &xml);
	void load(string file);

	void syncTimeTable(string address, int port, bool withSave);

	ofPtr<ofxKsmrTimeQueue> getEvent(string label);

	map<string, ofPtr<ofxKsmrTimeQueue> > queues;

	ofPtr<ofxKsmrOscTip> receiver;

	static ofEvent <ofxKsmrTimeEventArgs> newEvent;

	bool enableEvent;
};

class ofxKsmrTimeEventArgs : public ofEventArgs{
public:

	ofxKsmrTimeQueue* queuePtr;
	string label;
	int h;
	int m;
	int s;
};

class ofxKsmrTimeQueue{
public:

	ofxKsmrTimeQueue(string l, int hh, int mm, int ss, bool rep){

		isDone = false;
		repeat = rep;
		label = l;

		while (ss > 60){
			ss -= 60;
			mm++;
		}

		while (mm > 60){
			mm -= 60;
			hh++;
		}

		hh %= 24;

		h = hh;
		m = mm;
		s = ss;

		ofs_h = 0;
		ofs_m = 0;
		ofs_s = 0;
		ofs_t = 0;

		timeMillis = (ss + mm * 60 + hh * 3600) * 1000;


		if (repeat){
			unsigned long long tm = (ofGetHours() * 3600 + ofGetMinutes() * 60 + ofGetSeconds()) * 1000;
			unsigned long long ss = ofGetSystemTime();
			tm += ss - (ss / 1000 * 1000);

			genNextTargetMillisForRepeat(tm);
		}
	}

	void update(unsigned long long crnt){

		unsigned long long cOfs = crnt - ofs_t;

		if (!repeat){

			passed = crnt - timeMillis;
			if (crnt > timeMillis && !isDone) doEvent();
			if (crnt < timeMillis &&  isDone) isDone = false;

		}else{

			passed = cOfs - beforeMillis;

			if ((cOfs) > targetMillis){

				doEvent();
				beforeMillis = targetMillis;
				genNextTargetMillisForRepeat(crnt);

			}

			if (cOfs < (targetMillis - timeMillis * 2)){

				genNextTargetMillisForRepeat(crnt);

			}

		}

	}

	void genNextTargetMillisForRepeat(unsigned long long crnt){

		int num = crnt / timeMillis + 1;
		targetMillis = timeMillis * num;
		
	}

	void doEvent(){
		
		ofxKsmrTimeEventArgs arg;
		arg.queuePtr = this;
		arg.h = h;
		arg.m = m;
		arg.s = s;
		arg.label = label;

		isDone = true;
		ofNotifyEvent(ofxKsmrTimeEvent::newEvent, arg);

	}

	bool isDone;
	bool repeat;

	string label;
	int h,m,s;
	int ofs_h,ofs_m,ofs_s;
	unsigned long long ofs_t;
	long long passed;
	unsigned long long targetMillis;
	unsigned long long timeMillis;
	unsigned long long beforeMillis;
};

#endif /* defined(__SignageController__ofxKsmrTimeEvent__) */
