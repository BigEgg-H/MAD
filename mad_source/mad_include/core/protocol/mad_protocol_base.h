/**************************************************************************/
/*                         This file is part of:                          */
/*                      Marisa's Atelier of Danmaku                       */
/*                              2024/06/19                                */
/**************************************************************************/
/*                                                                        */
/*                                                                        */
/*                                                                        */
/*                                                                        */
/*                                                                        */
/*                                                                        */
/*                                                                        */
/*                                                                        */
/**************************************************************************/

#pragma once

static const MADVERSION MADCGlobalProtocolVersion = MADVERSION(1, 0, 0, 0);

struct MADCProtocol
{
	MADVERSION Version; 

	virtual bool ShakeHands() = 0;

	MADCProtocol(MADVERSION _ver) :Version(_ver){}
};

struct MADCProtocolHDVDBPS:public MADCProtocol
{


	MADCProtocolHDVDBPS():MADCProtocol(MADCGlobalProtocolVersion){}

	bool ShakeHands() override {
		return true;
	}
};

struct MADCProtocolTIGAS :public MADCProtocol
{


	MADCProtocolTIGAS() :MADCProtocol(MADCGlobalProtocolVersion) {}

	bool ShakeHands() override {

		return true;
	}
};
