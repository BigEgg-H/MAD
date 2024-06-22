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

class MADEHDVDBPS
{
public:
	MADErr Initialize(MADCProtocolHDVDBPS& _protocol, MADETIGAS* _GAS);

private:
	MADETIGAS* GAS = nullptr;
};

MADErr MADEHDVDBPS::Initialize(MADCProtocolHDVDBPS& _protocol, MADETIGAS* _GAS)
{
	GAS = _GAS;

	if (!_protocol.ShakeHands())
	{
		return MAD_ERR_FAIL_PROTOCOL_FEATURE_DISABLE;
	}

	return MAD_ERR_OK;
}