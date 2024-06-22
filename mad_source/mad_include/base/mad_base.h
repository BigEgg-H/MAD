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

#include <vector>

typedef int MADErr;

#define MAD_ERR_FAIL_UNKONW -1
#define MAD_ERR_OK 0
#define MAD_ERR_FAIL_VERSION_DISABLE 1
#define MAD_ERR_FAIL_PROTOCOL_FEATURE_DISABLE 2

typedef void* MADUSERDATA;
typedef void (*MADAPI)(MADUSERDATA para[]);

struct MADVERSION {
	unsigned int Major;
	unsigned int Minor;
	unsigned int Revision;
	unsigned int Build;

	MADVERSION() :Major(0), Minor(0), Revision(0), Build(0) {}
	MADVERSION(unsigned int _major, unsigned int _minor, unsigned int _revision = 0, unsigned int _build = 0)
		:Major(_major), Minor(_minor), Revision(_revision), Build(_build) {}

	bool IsValid(const MADVERSION& _maxver, const MADVERSION& _minver) {
		return (operator>(_minver)&& operator<(_maxver));
	}

	bool operator==(const MADVERSION& _ver) {
		return (_ver.Major == Major && _ver.Minor == Minor && _ver.Revision == Revision && _ver.Build == Build);
	}
	bool operator>(const MADVERSION& _ver) {
		if (_ver.Major > Major)
		{
			return true;
		}
		else
		{
			if (_ver.Major == Major)
			{
				if (_ver.Minor > Minor)
				{
					return true;
				}
				else
				{
					if (_ver.Minor == Minor)
					{
						if (_ver.Revision > Revision)
						{
							return true;
						}
						else
						{
							if (_ver.Revision == Revision)
							{
								if (_ver.Build > Build)
								{
									return true;
								}
							}
						}
					}
				}
			}
		}
		return false;
	}
	bool operator<(const MADVERSION& _ver) {
		return (!(operator>(_ver)) && !(operator==(_ver)));
	}
	bool operator>=(const MADVERSION& _ver) {
		return ((operator>(_ver)) || operator==(_ver));
	}
	bool operator<=(const MADVERSION& _ver) {
		return !(operator>(_ver));
	}
	bool operator!=(const MADVERSION& _ver) {
		return !(operator==(_ver));
	}
};



