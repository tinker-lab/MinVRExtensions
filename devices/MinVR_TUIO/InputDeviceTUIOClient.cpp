﻿/*
 * Copyright Regents of the University of Minnesota, 2015.  This software is released under the following license: http://opensource.org/licenses/GPL-2.0.
 * Source code originally developed at the University of Minnesota Interactive Visualization Lab (http://ivlab.cs.umn.edu).
 *
 * Code author(s):
 * 		Dan Orban (dtorban)
 */


#include "InputDeviceTUIOClient.H"

#include "MVRCore/StringUtils.H"
#include "MVRCore/ConfigMap.H"
#include <list>
#include "framework/plugin/PluginFramework.h"

namespace MinVR {

#ifdef USE_TUIO

using namespace TUIO;

InputDeviceTUIOClient::InputDeviceTUIOClient(int port, double xScale, double yScale)
{
	_xScale = xScale;
	_yScale = yScale;
	_tuioClient = new TuioClient(port);
	_tuioClient->connect();

	if (!_tuioClient->isConnected())
	{  
		std::cout << "InputDeviceTUIOClient: Cannot connect on port " << port << "." << std::endl;
	}
}



InputDeviceTUIOClient::InputDeviceTUIOClient(const std::string name, const ConfigMapRef map)
{
	int  port = map->get( name + "_Port", TUIO_PORT );
	double xs = map->get( name + "_XScaleFactor", 1.0 );
	double ys = map->get( name + "_YScaleFactor", 1.0 );

	_xScale = xs;
	_yScale = ys;
	_tuioClient = new TuioClient(port);
	_tuioClient->connect();

	if (!_tuioClient->isConnected())
	{ 
		std::cout << "InputDeviceTUIOClient: Cannot connect on port " << port << "." << std::endl;
	}
}

InputDeviceTUIOClient::~InputDeviceTUIOClient()
{
	if (_tuioClient) {
		_tuioClient->disconnect();
		delete _tuioClient;
	}
}

void InputDeviceTUIOClient::pollForInput(std::vector<EventRef> &events)
{
	// Send out events for TUIO "cursors" by polling the TuioClient for the current state  
	_tuioClient->lockCursorList();
	std::list<TuioCursor*> cursorList = _tuioClient->getTuioCursors();

	std::list<int> toErase;

	// Send "button" up events for cursors that were down last frame, but are now up.
	for ( auto downLast_it = _cursorsDown.begin(); downLast_it!= _cursorsDown.end(); ++downLast_it ) {
		bool stillDown = false;
		for (std::list<TuioCursor*>::iterator iter = cursorList.begin(); iter!=cursorList.end(); iter++) {
			TuioCursor *tcur = (*iter);
			if (tcur->getCursorID() == *downLast_it) {
				stillDown = true;
			}
		}
		if (!stillDown) {
			events.push_back(EventRef(new Event("TUIO_Cursor_up" + intToString(*downLast_it), nullptr, *downLast_it)));
			toErase.push_back(*downLast_it);
		}
	}

	for (std::list<int>::iterator iter = toErase.begin(); iter != toErase.end(); iter++) {
		_cursorsDown.erase(*iter);
	}

	// Send "button" down events for cursors that are new and updated positions for all cursors
	for (std::list<TuioCursor*>::iterator iter = cursorList.begin(); iter!=cursorList.end(); iter++) {
		TuioCursor *tcur = (*iter);
		glm::dvec2 pos = glm::dvec2(_xScale*tcur->getX(), _yScale*tcur->getY());

		if (_cursorsDown.find(tcur->getCursorID()) == _cursorsDown.end()) {
			events.push_back(EventRef(new Event("TUIO_Cursor_down" + intToString(tcur->getCursorID()), pos, nullptr, tcur->getCursorID())));
			_cursorsDown.insert(tcur->getCursorID());
		}

		if (tcur->getMotionSpeed() > 0.0) {
			glm::dvec4 data = glm::dvec4(pos, tcur->getMotionSpeed(), tcur->getMotionAccel());
			events.push_back(EventRef(new Event("TUIO_CursorMove" + intToString(tcur->getCursorID()), data, nullptr, tcur->getCursorID())));
		}

		// Can also access several other properties of cursors (speed, acceleration, path followed, etc.)
		//std::cout << "cur " << tcur->getCursorID() << " (" <<  tcur->getSessionID() << ") " << tcur->getX() << " " << tcur->getY() 
		// << " " << tcur->getMotionSpeed() << " " << tcur->getMotionAccel() << " " << std::endl;

		// This is how to access all the points in the path that a cursor follows:     
		//std::list<TuioPoint> path = tuioCursor->getPath();
		//if (path.size() > 0) {
		//  TuioPoint last_point = path.front();
		//  for (std::list<TuioPoint>::iterator point = path.begin(); point!=path.end(); point++) {
		//    last_point.update(point->getX(),point->getY());   
		//  }
		//}
	}
	_tuioClient->unlockCursorList();


	// Unsure what TUIO "objects" are -- perhaps tangible props.  In any case, this is how to access object data:
	_tuioClient->lockObjectList();
	std::list<TuioObject*> objectList = _tuioClient->getTuioObjects();
	
	for (std::list<TuioObject*>::iterator iter = objectList.begin(); iter!=objectList.end(); iter++) {
		TuioObject* tuioObject = (*iter);    
		int   id    = tuioObject->getSymbolID();
		double xpos  = _xScale*tuioObject->getX();
		double ypos  = _yScale*tuioObject->getY();
		double angle = tuioObject->getAngle()/M_PI*180.0;

		std::string name = "TUIO_Obj" + intToString(id);
		events.push_back(EventRef(new Event(name, glm::dvec3(xpos, ypos, angle))));
	}
	_tuioClient->unlockObjectList();
}

#endif //USE_TUIO

};         // end namespace
