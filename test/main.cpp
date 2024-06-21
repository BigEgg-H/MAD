#include <iostream>

#include "../mad_source/mad_include/mad.h"

static void drawbullet(void* para_in, void** para_out) {
	std::cout << "bullet here!";
}

int main() {
	MADCProtocolHDVDBPS bpspro;
	bpspro.DrawBullet = drawbullet;

	MADEHDVDBPS bps;

	bps.Initialize(bpspro);

	bps.Shoot();

	return 0;
}

