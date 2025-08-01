// func4(userinput[0], 0, 14);
int func4(int input, int a, int b) {
    int tmp1, tmp2;
    tmp1 = b - a;
    tmp2 = tmp1 >> 31;  // Get sign
    tmp1 += tmp2;       // Add sign
    tmp1 /= 2;
    tmp2 = tmp1 + a;
    if (tmp2 < input) {
        func4(input, tmp2 + 1, b);
        return 1;
    }
    else if (tmp2 == input) {
        return 0;
    }
    else {
        func4(input, a, tmp2 - 1);
        return tmp1 * 2;
    }
}

// tmp1 = 14
// tmp2 = 0
// tmp1 = 7
// tmp2 = 7