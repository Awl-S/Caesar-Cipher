
# Project: Caesar Cipher

This project implements a simple HTTP server that provides functionality for encrypting and decrypting text using the Caesar cipher.

## Main Files

1. `main.c`: Implementation of the HTTP server that processes encryption and decryption requests.  
2. `caesar.c`: Functions for encrypting and decrypting text.  
3. `caesar.h`: Header file with function declarations.  
4. `test.py`: Script for stress testing the server.

## How to Run

1. Compile the server:  
   ```bash
   mkdir build
   cd build
   cmake build ..
   make
   ```

2. Run the server:  
   ```bash
   ./Caesar-Cipher_C
   ```

3. Test the server:  
   - Encryption: `http://localhost:8080/encrypt?text=Hello&shift=3`  
   - Decryption: `http://localhost:8080/decrypt?text=Khoor&shift=3`

4. Run the stress test (optional):  
   ```bash
   python stress_test.py
   ```

## Dependencies

- `libmicrohttpd`: For the HTTP server implementation.  
- `requests`: For performing HTTP requests in the stress test script. Install it with:  
  ```bash
  pip install requests
  ```

## Features

- Easy integration of encryption/decryption into web applications.  
- Stress testing capability to evaluate performance.

## License

Free to use for educational and research purposes.