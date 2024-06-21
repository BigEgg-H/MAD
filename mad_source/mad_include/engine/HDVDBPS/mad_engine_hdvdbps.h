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
	MADErr Initialize(MADCProtocolHDVDBPS& _protocol);

	void Shoot() {
		draw(nullptr, nullptr);
	};

private:
	MADAPI draw;
};

MADErr MADEHDVDBPS::Initialize(MADCProtocolHDVDBPS& _protocol)
{
	if (!_protocol.ShakeHands())
	{
		return MAD_ERR_FAIL_PROTOCOL_FEATURE_DISABLE;
	}

	draw = _protocol.DrawBullet;

	return MAD_ERR_OK;
}