#include "menu.h"
#include "ga_database.h"

int main() {
    if (database_init() != 0) return -1;
    menu_show();
    database_close();
    return 0;
}