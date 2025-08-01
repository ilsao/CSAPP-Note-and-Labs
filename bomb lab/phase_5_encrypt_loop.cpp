void EncryptLoop() {
    char* tmp;      // %rdx
    char msg[6];    // %rsp
    char userinput[6];
    for (int i = 0; i < 6; i++) {
        *tmp = userinput[i] & 0xf; // 1111
        tmp = (char*)(0x4024b0 + (int)tmp);
        msg[i+0x10] = *tmp;
    }
}