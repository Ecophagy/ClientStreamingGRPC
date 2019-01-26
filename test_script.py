import subprocess
import os
import math
import sys

TEST_FILE_NAME = "test.txt"
CHUNK_SIZE = 64*1024

# Test sending a small file
def test_small_file():
    # Write a small test file
    file_size = 10 #10 bytes
    file = open(TEST_FILE_NAME, "w")
    file.write(''.join('a' for i in range(file_size)))
    file.close()

    # Start the server process
    server = subprocess.Popen("./simple_server", stdout=subprocess.PIPE)

    # Start the client process with message parameters
    client = subprocess.Popen(["./simple_client", "aaa 6 ", TEST_FILE_NAME], stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    # Check the client received the expected response
    expected_chunks = math.ceil((file_size)/CHUNK_SIZE)
    for line in client.stdout:
        text = line.decode(sys.stdout.encoding)
        assert text == "message received: " + str(expected_chunks)

    # Stop the server process
    server.kill()

    # Delete the test file
    os.remove(TEST_FILE_NAME)

# Test sending a large file
def test_large_file():
    # Write a large test file
    file_size = 10**9 #1 GB
    file = open(TEST_FILE_NAME, "w")
    for i in range(10):
        file.write(''.join('a' for j in range(file_size//10)))
    file.close()

    # Start the server process
    server = subprocess.Popen("./simple_server", stdout=subprocess.PIPE)

    # Start the client process with message parameters
    client = subprocess.Popen(["./simple_client", "aaa 6 ", TEST_FILE_NAME], stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    # Check the client received the expected response
    expected_chunks = math.ceil((file_size)/CHUNK_SIZE)
    for line in client.stdout:
        text = line.decode(sys.stdout.encoding)
        assert text == "message received: " + str(expected_chunks)

    # Stop the server process
    server.kill()

    # Delete the test file
    os.remove(TEST_FILE_NAME)

# Test sending the wrong number of parameters
def test_wrong_params(): 
        # Start the client process with message parameters
    client = subprocess.Popen(["./simple_client"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    # Check the client received the error response
    for line in client.stderr:
        text = line.decode(sys.stdout.encoding)
        assert text == "Please provide a string, integer, and file path\n"

# Test sending thean invalid integer parameter
def test_invalid_integer_params(): 
    test_integer = "invalid"

    # Start the client process with message parameters
    client = subprocess.Popen(["./simple_client", "aaa", test_integer, "filepath"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    # Check the client received the error response
    for line in client.stderr:
        text = line.decode(sys.stdout.encoding)
        assert text == "integer parameter \'" + test_integer + "\' is not a valid integer\n"