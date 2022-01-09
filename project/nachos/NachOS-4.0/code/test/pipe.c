/*
    pipe
    Tien trinh voi nuoc
    Quan ly 2 voi nuoc, nhan yeu cau tu process Sinh vien:
        - Yeu cau cap voi nuoc
        - Yeu cau tra voi nuoc
*/

#include "syscall.h"
#include "fileutils.h"


int main() {
    int isSuccess;
    int numFaucet, i;
    bool faucet[2];
    int fRequest, fFaucet;
    int requestType, value;
    
    numFaucet = 2;
    faucet[0] = faucet[1] = false;

    while (1) {
        // doi sinh vien yeu cau uong nuoc (hoac tra voi nuoc)
        Wait("request_water");
        // doc yeu cau
        fRequest = Open("request.txt", 1);
        ReadInt(fRequest, &requestType);
        ReadInt(fRequest, &value);
        Close(fRequest);
        
        if (requestType == 0) { // yeu cau cap voi nuoc
            // tim voi nuoc con trong
            for (i = 0; i < numFaucet; ++i) {
                if (faucet[i]) continue;
                faucet[i] = true;
                // ghi voi nuoc duoc cap cho tien trinh Sinh vien doc
                if (CreateFile("faucet.txt") == -1) {
                    PrintString("\nCan't create file faucet.txt\n");
                    Exit(1);
                }
                fFaucet = Open("faucet.txt", 0);
                WriteNum(fFaucet, i + 1);
                Close(fFaucet);
                
                Signal("open_water"); // sinh vien nhan voi nuoc
                break;
            }
        } else { // tra voi nuoc
            faucet[value] = false;
            Signal("faucet");
            Signal("open_water"); // sinh vien da tra voi nuoc
        }
    }
    Exit(0);
}