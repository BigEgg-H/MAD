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

class MADEBPSProtocol:public MADCProtocol
{
public:
	MADCVersion GetVersionID() { return MADC_PROTOCOL_VERSION; }
private:

};

class MADEBPSInterface:public MADCInterface<MADEBPSProtocol>
{
public:
	MADEBPSInterface();
	~MADEBPSInterface();

private:

};

inline MADEBPSInterface::MADEBPSInterface():MADCInterface<MADEBPSProtocol>(MADC_PROTOCOL_VERSION)
{

}

MADEBPSInterface::~MADEBPSInterface()
{

}
