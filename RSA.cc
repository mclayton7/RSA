// Implementation of the RSA public-key encryption algorithm
// Mac Clayton, 2012

//	MUST COMPILE Like:
//	g++ -o RSA RSA.cc -lgmpxx -lgmp

#include <iostream>
#include <cstdlib>
#include <vector>
#include <stdio.h>
#include <fcntl.h>
#include <sys/time.h>

#include <gmpxx.h>

using namespace std;
static gmp_randclass r (gmp_randinit_default);
// gmp_randclass r;

// Pollard's rho algorithm
bool FactorRho(const mpz_class& n, mpz_class& result)
{
	mpz_class x = mpz_class(2);
	mpz_class y = mpz_class(2);
	mpz_class d = mpz_class(1);

	while(d == mpz_class(1))
	{
		// X <-- f(x)
		x = x * x + 1;
		mpz_mod(x.get_mpz_t(), x.get_mpz_t(), n.get_mpz_t());

		// Y <-- f(f(y))
		y = y * y + 1;
		mpz_mod(y.get_mpz_t(), y.get_mpz_t(), n.get_mpz_t());
		y = y * y + 1;
		mpz_mod(y.get_mpz_t(), y.get_mpz_t(), n.get_mpz_t());

		// D <-- GCD(abs(x-y), n)
		d = x - y;
		mpz_gcd(d.get_mpz_t(), d.get_mpz_t(), n.get_mpz_t());
	}
	if(d == n)
	{
		return false;
	}
	else
	{
		result = d;
		return true;
	}	
}


void generateSeed()
{
	int randomData = open("/dev/random", O_RDONLY);
	unsigned long int randSeed;
	read(randomData, &randSeed, sizeof randSeed);
	close(randomData);
	r.seed(randSeed);
}


// Generate the public and private keys
void generateKey(int size, mpz_class& n, mpz_class& d, mpz_class& e)
{
	bool prime = false;
	bool divisor = false;
	mpz_class PNUM, result, q, p, phiN;
	mpz_class one = mpz_class(1);

	// Compute p & q
	for(int i = 0; i < 2; ++i)
	{
		while(!prime)
		{
			PNUM = r.get_z_bits(size);
			int isPrime = 1;
			isPrime = mpz_probab_prime_p(PNUM.get_mpz_t(), 100);
			if(isPrime != 0) break;
		}
		if(i == 0) p = PNUM;
		if(i == 1) q = PNUM;
	}
	n = p * q;
	phiN = (p - 1) * (q - 1);

	while(!divisor)
	{
		d = r.get_z_bits(size * 2);
		mpz_gcd(result.get_mpz_t(), d.get_mpz_t(), phiN.get_mpz_t());
		if((d < phiN) && (result == one)) break;
	}

	// Compute e:
	mpz_invert(e.get_mpz_t(), d.get_mpz_t(), phiN.get_mpz_t());

}

// Encrypt Function
void encrypt(	  mpz_class& c, // Encrypted Message
			const mpz_class& m, // Message to be encrypted
			const mpz_class& e, // Keys
			const mpz_class& n)
{
	//Encrypt: c = (m ^ e) mod n
	mpz_powm(	c.get_mpz_t(), 	// Resulting Encrypted Message
				m.get_mpz_t(), 	// Message to be Encrypted
				e.get_mpz_t(), 	// Key Pair (n, e) private
				n.get_mpz_t());	// Key Pair (n, e) shared
}

// Decrypt Function
void decrypt(	  mpz_class& m, // Resulting Message
			const mpz_class& c, // Message
			const mpz_class& d, // Keys
			const mpz_class& n)
{
	//Decrypt: m = (c ^ d) mod n
	mpz_powm(	m.get_mpz_t(),	// Resulting Decrypted Message 
				c.get_mpz_t(),	// Encryted Message
				d.get_mpz_t(),	// 
				n.get_mpz_t());
}


// Main Program:
int main(int argc, char** argv)
{
	int options = argc;
	mpz_class n, d, e, c, m, result;
	if(argc > 2) exit(1);
	if(argc == 1) options = 1;
	if(argc == 2) options = atol(argv[1]);

	// Timing Stuff:
	struct timeval tp;
	double startSec;
	gettimeofday(&tp, 0);

	generateSeed();

	// Use a switch case to evaluate Factoring Methods
	// Case #1: Normal operations
	// Case #2: Generate a key, and factor 'n' using the
	//			Pollard Rho Algorithm

	switch (options)
	{
		case 1:
		{
			for(int sz = 32; sz < 1025; sz *= 2)
			{
				startSec = tp.tv_sec + tp.tv_usec/1000000.0;
				cout << "Current size is: " << sz << endl;
				// Generate 100 Keys
				for(int i = 0; i < 100; ++i)
				{
					generateKey(sz, n, d, e);
					// Generate 100 Random Messages for each key
					for(int j = 0; j < 100; ++j)
					{
						// Create a message from random bits:
						m = r.get_z_bits(sz*2);
						while(m > n) m = r.get_z_bits(sz*2);

						// Encrypt message 'm' and decrypt it in 'result'
						encrypt(c, m, e, n);
						decrypt(result, c, d, n);

						// Difference Testing:
						if((m - result) == 0)
						{
							// Debug testing code:
							// cout << "for sz = " << sz << ", j = " << j <<", passed!" << endl;
							// cout << "Passed" << endl;
						}
						else
						{
							cout << "FAILED " << endl;
							cout << "d: " << d << endl;
							cout << "e: " << e << endl;
							break;
						}
					}
				}
				gettimeofday(&tp, 0);
				cout << "Calculated " << sz << "-bit keys in: "
		        	 << (tp.tv_sec+tp.tv_usec/1000000.0) - startSec << " seconds" << endl;
			}// End Size loop
		break;
		}
			
		case 2:
		{
			int bits = 64;
			int t;
			bool worked;
			generateKey(bits, n, d, e);
			worked = FactorRho(n, result);
			t = clock();
			cout << "Time is: " << ((float)t)/CLOCKS_PER_SEC << endl;
			cout << "n = " << n << endl;
		}
		break;
	}// End switch
}// End Main
