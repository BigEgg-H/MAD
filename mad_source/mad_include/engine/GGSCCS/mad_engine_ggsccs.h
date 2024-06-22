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

class MADEGGSCCS
{
public:
	MADEGGSCCS(MADCProtocolHDVDBPS& _BPSProtocol, MADCProtocolTIGAS& _TIGASProtocol);
	~MADEGGSCCS();
private:
	MADETIGAS* GAS;
	MADEHDVDBPS* BPS;
};

MADEGGSCCS::MADEGGSCCS(MADCProtocolHDVDBPS& _BPSProtocol, MADCProtocolTIGAS& _TIGASProtocol)
{
	GAS = new(MADETIGAS);
	MADErr err = GAS->Initialize(_BPSProtocol);
	if (err != MAD_ERR_OK)
	{
		throw(err);
	}

	BPS = new(MADEHDVDBPS);
	err = BPS->Initialize(_BPSProtocol, GAS);
	if (err != MAD_ERR_OK)
	{
		throw(err);
	}
}

MADEGGSCCS::~MADEGGSCCS()
{
	delete(BPS);
	delete(GAS);
}


