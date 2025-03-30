// Only works for x = 2^k where k >= 0
int count_decimal_digits_power_of_2(int x) {
    if (x == 0) return 1; // Handle 2^0=1 (but 0 itself is invalid)
    
    // Find log2(x) = position of the highest set bit
    int log2 = 0;
    while (x >>= 1) log2++;
    
    // Digits in 2^k ≈ log10(2^k) + 1 = k * log10(2) + 1
    // log10(2) ≈ 0.30102999566398119521373889472449
    return (int)(log2 * 0.30102999566398119521373889472449) + 1;
}

unsigned int powerOfTwo(unsigned int exponent) {
    return 1 << exponent;  // Left shift 1 by exponent positions
}
