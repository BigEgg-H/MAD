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

//DO NOT CHANGE THIS CODE!
#define MADC_PROTOCOL_VERSION 240619

typedef unsigned int MADCVersion;

class MADCProtocol
{
public:
	virtual MADCVersion GetVersionID() = 0;
};
