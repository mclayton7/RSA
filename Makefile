# Build the RSA assignment
RSA:	RSA.cc
	$(CXX) -o RSA RSA.cc -lgmpxx -lgmp

clean:
	@rm -f *.o RSA
