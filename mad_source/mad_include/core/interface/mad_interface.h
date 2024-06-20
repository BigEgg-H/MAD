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

#include "../protocol/mad_protocol_base.h"

template <class Protocol>
class MADCInterface
{
public:
	MADCInterface(MADCVersion _targetVersion);
	~MADCInterface();

public:
	MADErr Shakehands(Protocol* protocol);


private:
	MADCVersion TargetVersion;
	Protocol* TargetProtocol;

};

template<class Protocol>
inline MADCInterface<Protocol>::MADCInterface(MADCVersion _targetVersion)
{
	TargetVersion = _targetVersion;
	TargetProtocol = new(Protocol);
}

template<class Protocol>
inline MADCInterface<Protocol>::~MADCInterface()
{
	delete(TargetProtocol);
}

template<class Protocol>
inline MADErr MADCInterface<Protocol>::Shakehands(Protocol* protocol)
{

	return MAD_ERR_OK;
}
