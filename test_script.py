import subprocess
import os
import math

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
        assert line == "message received: " + str(expected_chunks)

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
        assert line == "message received: " + str(expected_chunks)

    # Stop the server process
    server.kill()

    # Delete the test file
    os.remove(TEST_FILE_NAME)