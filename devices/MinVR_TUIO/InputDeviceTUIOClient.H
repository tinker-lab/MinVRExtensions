/* ================================================================================

This file is part of the MinVR Open Source Project, which is developed and 
maintained collaboratively by the University of Minnesota's Interactive 
Visualization Lab and the Brown University Visualization Research Lab.

File: MinVR/Plugin/Devices/TUIO/InputDeviceTUIOClient.H

Original Author(s) of this File: 
	Daniel Keefe, 2009, University of Minnesota (originally VRG3D/TUIOEvents.H)
	
Author(s) of Significant Updates/Modifications to the File:
	Bret Jackson, 2013, University of Minnesota (adapted to MinVR)

-----------------------------------------------------------------------------------
Copyright (c) 2008-2015 Regents of the University of Minnesota and Brown University
Copyright (c) 2004-2008 Brown University
All rights reserved.

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice, this
  list of conditions and the following disclaimer in the documentation and/or
  other materials provided with the distribution.

* Neither the name of the University of Minnesota, Brown University, nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
================================================================================ */


#ifndef MINVR_TUIOEVENTS_H
#define MINVR_TUIOEVENTS_H

#include  <glm/glm.hpp>

#ifdef USE_TUIO
#include  <TuioClient.h>
#endif


#include "framework/InputDevice.h"
#include "MVRCore/ConfigMap.H"
#include "MVRCore/Event.H"
#include <vector>
#include <unordered_set>
#include <log/Logger.h>



#define TUIO_PORT  3333

namespace MinVR {

class InputDeviceTUIOClient : public MinVR::framework::InputDevice
{
public:

#ifdef USE_TUIO
	InputDeviceTUIOClient(int port = TUIO_PORT, double  xScaleFactor = 1.0, double  yScaleFactor=1.0 );
	InputDeviceTUIOClient( const std::string name, const ConfigMapRef map );
	virtual ~InputDeviceTUIOClient();
	
	void pollForInput( std::vector<EventRef>  &events );


private:
	TUIO::TuioClient *_tuioClient;
	std::unordered_set<int>    _cursorsDown;
	double      _xScale;
	double      _yScale;

#else
	InputDeviceTUIOClient(int port = TUIO_PORT, double  xScaleFactor = 1.0, double  yScaleFactor=1.0 )
	{
		MinVR::Logger::getInstance().assertMessage(false, "TUIO is currently unsupported. Set cmake option USE_TUIO to ON and recompile.");
	}

	InputDeviceTUIOClient( const std::string name, const ConfigMapRef map )
	{
		MinVR::Logger::getInstance().assertMessage(false, "TUIO is currently unsupported. Set cmake option USE_TUIO to ON and recompile.");
	}

	virtual ~InputDeviceTUIOClient() {}
	void pollForInput( std::vector<EventRef> &events) {}


#endif                   // USE_TUIO
};

}                        // end namespace

#endif                  