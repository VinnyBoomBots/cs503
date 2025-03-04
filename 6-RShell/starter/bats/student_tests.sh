#!/usr/bin/env bats

# File: student_tests.sh
# 
# Create your unit tests suit in this file

@test "Example: check ls runs without errors" {
    run ./dsh <<EOF                
ls
EOF

    # Assertions
    [ "$status" -eq 0 ]
}


#### Utilizing ***./dsh -s -i 127.0.0.1 -p 5678**** for each clinet-side test. 
### I copied this start_server and stop_server functionality from a bit of Internet research I
### did -- and I also ran it by ChatGPT to see if it would work, which it seems to!

start_server() {
  ./dsh -s -i 127.0.0.1 -p 5678 &   
  SERVER_PID=$!
}

stop_server() {
  wait $SERVER_PID
}


@test "Too many commands" {

  start_server

  # Run the command, piping input to it
  run ./dsh -c 129.25.203.107 -p 5678 <<EOF
cmd | cdm | g | g | h | h | h | i | j | i | p | l | cmd | op
stop-server
EOF

  # Strip all whitespace (spaces, tabs, newlines) from the output
  stripped_output=$(echo "$output" | tr -d '[:space:]' | tr -d '\004')

  # Expected output with all whitespace removed for easier matching
  expected_output="socketclientmode:addr:127.0.0.1:5678dsh4>error:pipinglimitedto8commandsThanks!dsh4>cmdloopreturned0"

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]

    # Assertions
    [ "$status" -eq 0 ]

    stop_server

}

@test "No commands entered" {

  start_server

  # Run the command, piping input to it
  run ./dsh -c 129.25.203.107 -p 5678 <<EOF

stop-server
EOF

  # Strip all whitespace (spaces, tabs, newlines) from the output
  stripped_output=$(echo "$output" | tr -d '[:space:]' | tr -d '\004')

  # Expected output with all whitespace removed for easier matching
  expected_output="socketclientmode:addr:127.0.0.1:5678dsh4>warning:nocommandsprovidedThanks!dsh4>cmdloopreturned0"

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]

    # Assertions
    [ "$status" -eq 0 ]

    stop_server

}

@test "Exiting client and server" {

  start_server

  # Run the command, piping input to it
  run ./dsh -c 129.25.203.107 -p 5678 <<EOF
stop-server
EOF

  # Strip all whitespace (spaces, tabs, newlines) from the output
  stripped_output=$(echo "$output" | tr -d '[:space:]' | tr -d '\004')

  # Expected output with all whitespace removed for easier matching
  expected_output="socketclientmode:addr:127.0.0.1:5678dsh4>cmdloopreturned0"

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]

    # Assertions
    [ "$status" -eq 0 ]

    stop_server

}


@test "cd command" {

  start_server

  # Run the command, piping input to it
run ./dsh -c 129.25.203.107 -p 5678 <<EOF
cd
pwd
stop-server
EOF

  # Strip all whitespace (spaces, tabs, newlines) from the output
  stripped_output=$(echo "$output" | tr -d '[:space:]' | tr -d '\004')

  # Expected output with all whitespace removed for easier matching
  expected_output="socketclientmode:addr:127.0.0.1:5678dsh4>cdsuccessThanks!dsh4>/home/vjs36/cs503/6-RShell/starterThanks!dsh4>cmdloopreturned0"

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]

    # Assertions
    [ "$status" -eq 0 ]

    stop_server

}


@test "Unknown command" {

  start_server

  # Run the command, piping input to it
run ./dsh -c 129.25.203.107 -p 5678 <<EOF
fdsa
stop-server
EOF

  # Strip all whitespace (spaces, tabs, newlines) from the output
  stripped_output=$(echo "$output" | tr -d '[:space:]' | tr -d '\004')

  # Expected output with all whitespace removed for easier matching
  expected_output="socketclientmode:addr:127.0.0.1:5678dsh4>socketservermode:addr:127.0.0.1:5678->Single-ThreadedModeCommandnotfoundThanks!dsh4>cmdloopreturned0"

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]

    # Assertions
    [ "$status" -eq 0 ]

    stop_server

}


@test "Non-built-in command" {

  start_server

  # Run the command, piping input to it
run ./dsh -c 129.25.203.107 -p 5678 <<EOF
echo "Hello, world!" | wc -w
stop-server
EOF

  # Strip all whitespace (spaces, tabs, newlines) from the output
  stripped_output=$(echo "$output" | tr -d '[:space:]' | tr -d '\004')

  # Expected output with all whitespace removed for easier matching
  expected_output="socketclientmode:addr:127.0.0.1:5678dsh4>2Thanks!dsh4>cmdloopreturned0"

    # These echo commands will help with debugging and will only print
    #if the test fails
    echo "Captured stdout:" 
    echo "Output: $output"
    echo "Exit Status: $status"
    echo "${stripped_output} -> ${expected_output}"

    # Check exact match
    [ "$stripped_output" = "$expected_output" ]

    # Assertions
    [ "$status" -eq 0 ]

    stop_server

}