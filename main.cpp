#include <iostream>
#include "bits/stdc++.h"
#include "simulation.h"

int main()
{
	freopen("output.txt", "w", stdout);
	simulation net;
	net.init();
	net.simulateTimeFrame();
	return 0;
}
