#include <iostream>

#include "../mad_source/mad_include/mad.h"

int main() {
	MADCProtocolHDVDBPS bpsp;
	MADCProtocolTIGAS gasp;
	MADEGGSCCS ggsccs(bpsp, gasp);

	return 0;
}

