RSA
===

This is a simple RSA Implementation that demonstrates key generation as well as encryption and decryption. It's been setup to run benchmarks demonstrating a particular algorithm. It's been tested and confirmed to work on OSX 10.9 Mavericks.

Note: the generateSeedIO function has broken with Mavericks, so please don't use this for any real encryption. 

To Run on OSX
======
1. Using Homebrew, make sure you've installed the GNU Multiple Precision Library:
    brew install gmp

2. Then compile using G++:
    make
3. Then run:
    ./RSA
