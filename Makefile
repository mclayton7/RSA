# Build the RSA assignment
RSA:	RSA.cc
	$(CXX) -g -o RSA RSA.cc -lgmpxx -lgmp

clean:
	@rm -f *.o RSA
