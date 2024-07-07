#include "playback.h"
#include <vector>
#include <cstdint>

int main(int argc, char **argv) {
	const char *dir = argv[1];
	std::vector<uint8_t> *file_data = read_file(dir);
	switch(playback(file_data)) {
		case PLAY_SUCCESS:
			break;
		case PLAY_FAILURE:
			exit(1);
	}
	return 0;
}