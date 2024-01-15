# API Documentation

In this documentation, you will find details on how to authenticate with our API, the available endpoints, parameters and examples of requests and responses. We hope you find this documentation helpful.

## Base link / Website:

**Website root link:**

http://rcoelen.loca.lt

**API service page/router:**

http://rcoelen.loca.lt/api.php

**Adding parameters at the end of the link:**

http://rcoelen.loca.lt/api.php?

**Defining an action at the end of the link:**

http://rcoelen.loca.lt/api.php?action=read ( Getting data )

http://rcoelen.loca.lt/api.php?action=create ( Creating data )

## Format

|Function to executed|Assigning variable|Value to store/read|
|---------|----------------|--------------|
|&variable_name|     =     |variable_value|

**Example:** &number=6

**You can chain the variables:** &number=6&chipid=AE67


## Required variables

action - (Text| Example: Read/Create) defines the different function the api offers

number - (Number| Example: 3) the rolled number data of the device

## Optional variables

chipid - (Text| Example: "AE85") the device id for keeping track of records

textready - (Bool| Example: True/False) inbuilt function for a smaller list for embedded devices

limit - (Number| Example: 10) limit the number of record getting requested 

## Returnable variables

success - (Bool | Example: True/False) returns status message if message sent reached the server