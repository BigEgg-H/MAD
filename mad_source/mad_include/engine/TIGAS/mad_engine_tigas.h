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

class MADETIGAS
{
public:
	MADErr Initialize(MADCProtocolHDVDBPS& _protocol);

public:
	std::vector<MADUSERDATA> BulletData;

private:

};

MADErr MADETIGAS::Initialize(MADCProtocolHDVDBPS& _protocol)
{
	if (!_protocol.ShakeHands())
	{
		return MAD_ERR_FAIL_PROTOCOL_FEATURE_DISABLE;
	}

	return MAD_ERR_OK;
}