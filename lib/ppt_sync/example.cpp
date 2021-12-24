#include <iostream>
#include "libppt_sync.h"

using namespace std;

int main()
{
    PptSync* ppt_syncer = pptsync_new();

    int frame = 0;

    while (true)
    {
        if (!pptsync_wait_for_frame(ppt_syncer)) {
            cout << "error" << endl;
            break;
        }
        cout << "frame " << frame << endl;
        ++frame;
        if (frame > 1000) break;
    }
    cout << "finished!" << endl;

    pptsync_destroy(ppt_syncer);

    cin.get();
    return 0;
};