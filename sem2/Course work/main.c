#include <wchar.h>
#include <stdbool.h>
#include <errno.h>

#include "Date.h"

int main() {
        // TEST: 
	while (true) {
		wchar_t dateStr[1024];
		wscanf(L"%ls", dateStr);
		Date date = getDate(dateStr);
		if (errno != EINVAL) {
			wprintf(L"day: %d\nmonth: %d\nyear: %d\n", date.day, date.month, date.year);
		}
		errno = 0;
	}
	return 0;
}