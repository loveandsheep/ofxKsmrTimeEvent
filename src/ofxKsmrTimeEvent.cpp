//
//  ofxKsmrTimeEvent.cpp
//  SignageController
//
//  Created by Ovis aries on 2015/02/25.
//
//

#include "ofxKsmrTimeEvent.h"

ofEvent <ofxKsmrTimeEventArgs> ofxKsmrTimeEvent::newEvent;

void ofxKsmrTimeEvent::setOscDistributor(ofxKsmrOscDistributor *dst){

	receiver = ofPtr<ofxKsmrOscTip>(new ofxKsmrOscTip);

	receiver->addAddress("/ksmr/timeEvent");
	dst->addReceiverTag(receiver);

	enableEvent = true;
}

void ofxKsmrTimeEvent::update(){


	if (static_cast<bool>(receiver)){

		while (receiver->hasWaitingMessages()){
			ofxOscMessage m;
			receiver->getNextMessage(&m);

			if (m.getAddress() == "/ksmr/timeEvent/sync"){
				ofXml xml;
				xml.loadFromBuffer(m.getArgAsString(0));

				load(xml);

				if (m.getArgAsInt32(1)) save(CONTENT_PATH + "settings/timeEvent.xml");

			}

			if (m.getAddress() == "/ksmr/timeEvent/enable"){
				enableEvent = m.getArgAsInt32(0);
			}

			if (m.getAddress() == "/ksmr/timeEvent/simulate"){
				ofxKsmrTimeEventArgs arg;
				arg.label = m.getArgAsString(0);
				ofNotifyEvent(ofxKsmrTimeEvent::newEvent, arg);
			}
		}

	}



	if (enableEvent){

		unsigned long long tm = (ofGetHours() * 3600 + ofGetMinutes() * 60 + ofGetSeconds()) * 1000;
		unsigned long long ss = ofGetSystemTime();
		tm += ss - (ss / 1000 * 1000);

		map<string, ofPtr<ofxKsmrTimeQueue> >::iterator it;
		for(it = queues.begin(); it != queues.end(); ++it){
			(*it).second->update(tm);
		}

	}

}

ofPtr<ofxKsmrTimeQueue> ofxKsmrTimeEvent::getEvent(string label){

	map<string, ofPtr<ofxKsmrTimeQueue> >::iterator it = queues.find(label);
	return it->second;

}

void ofxKsmrTimeEvent::removeEvent(string label){

	if (queues.count(label) == 0) return;

	map<string, ofPtr<ofxKsmrTimeQueue> >::iterator it = queues.find(label);
	queues.erase(it);

}

bool ofxKsmrTimeEvent::addSingleEvent(string Label, int hour, int min, int sec){

	ofPtr<ofxKsmrTimeQueue> q = ofPtr<ofxKsmrTimeQueue>(new ofxKsmrTimeQueue(Label,
																			 hour,min,sec,false));

	if (queues.count(Label) != 0){

		(*queues.find(Label)->second) = *q;
		return false;

	}else{

		queues.insert(make_pair(Label, q));

		return true;
	}
}

bool ofxKsmrTimeEvent::addRepeatEvent(string Label, int hour, int min, int sec, int offset_h, int offset_m, int offset_s){

	ofPtr<ofxKsmrTimeQueue> q = ofPtr<ofxKsmrTimeQueue>(new ofxKsmrTimeQueue(Label,
																			 hour,min,sec,true));
	q->ofs_h = offset_h;
	q->ofs_m = offset_m;
	q->ofs_s = offset_s;
	q->ofs_t = (offset_h * 3600 + offset_m * 60 + offset_s) * 1000;

	if (queues.count(Label) != 0){

		(*queues.find(Label)->second) = *q;
		return false;

	}else{

		queues.insert(make_pair(Label, q));

		return true;
	}

}

void ofxKsmrTimeEvent::load(ofXml &xml){

	queues.clear();

	xml.setTo("//Events");

	int numQueue = xml.getNumChildren("Queue");
	for (int i = 0;i < numQueue;i++){
		xml.setTo("//Events");

		xml.setTo("Queue["+ofToString(i)+"]");
		if (xml.getBoolValue("Repeat")){

			addRepeatEvent(xml.getValue("Label"),
						   xml.getIntValue("Hour"),xml.getIntValue("Min"),xml.getIntValue("Sec"),
						   xml.getIntValue("OHour"),xml.getIntValue("OMin"),xml.getIntValue("OSec"));

		}else{

			addSingleEvent(xml.getValue("Label"),
						   xml.getIntValue("Hour"), xml.getIntValue("Min"), xml.getIntValue("Sec"));
			
		}
		
	}

}

void ofxKsmrTimeEvent::save(ofXml &xml){

	xml.addChild("Events");

	map<string, ofPtr<ofxKsmrTimeQueue> >::iterator it;

	int cnt = 0;
	for(it = queues.begin(); it != queues.end(); ++it){
		ofPtr<ofxKsmrTimeQueue> q = (*it).second;

		xml.setTo("//Events");

		xml.addChild("Queue");
		xml.setTo("Queue["+ofToString(cnt)+"]");

		xml.addValue("Label",q->label);
		xml.addValue("Hour",q->h);
		xml.addValue("Min",q->m);
		xml.addValue("Sec",q->s);
		xml.addValue("OHour",q->ofs_h);
		xml.addValue("OMin",q->ofs_m);
		xml.addValue("OSec",q->ofs_s);

		xml.addValue("Repeat",q->repeat);

		cnt++;
	}


	for (int i = 0;i < 5;i++){


	}

}

void ofxKsmrTimeEvent::load(string file){

	ofXml xml;
	xml.load(file);
	load(xml);

}

void ofxKsmrTimeEvent::save(string file){

	ofXml xml;
	save(xml);
	xml.save(file);

}

void ofxKsmrTimeEvent::syncTimeTable(string address, int port, bool withSave){

	ofXml xml;
	save(xml);

	string buf = xml.toString();

	ofxOscSender sender;
	sender.setup(address, port);

	ofxOscMessage m;
	m.setAddress("/ksmr/timeEvent/sync");
	m.addStringArg(buf);
	m.addIntArg(withSave);

	sender.sendMessage(m);

}